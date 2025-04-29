/* Original work Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Modified work Copyright 2024 kamilsss655
 * https://github.com/kamilsss655
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>

#include "app/action.h"
#include "app/app.h"
#include "app/chFrScanner.h"
#include "app/common.h"
#ifdef ENABLE_FMRADIO
	#include "app/fm.h"
#endif
#include "app/generic.h"
#include "app/main.h"
#include "app/scanner.h"

#ifdef ENABLE_SPECTRUM
#include "app/spectrum.h"
#endif

#include "audio.h"
#include "board.h"
#include "driver/bk4819.h"
#include "dtmf.h"
#include "frequencies.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/inputbox.h"
#include "ui/ui.h"
#include <stdlib.h>
#ifdef ENABLE_MESSENGER
	#include "app/messenger.h"
#endif

void toggle_chan_scanlist(void)
{	// toggle the selected channels scanlist setting

	if ( SCANNER_IsScanning())
		return;
	SETTINGS_UpdateChannel(gTxVfo->CHANNEL_SAVE, gTxVfo, true);
	gVfoConfigureMode = VFO_CONFIGURE;
	gFlagResetVfos    = true;
#ifdef ENABLE_SCAN_RANGES
//Robby69 modification to use SCAN_RANGES for memories
	if(!IS_MR_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
		gScanRangeStart = gScanRangeStart ? 0 : gTxVfo->pRX->Frequency;
		gScanRangeStop = gEeprom.VfoInfo[!gEeprom.TX_VFO].freq_config_RX.Frequency;
		if(gScanRangeStart > gScanRangeStop)
			SWAP(gScanRangeStart, gScanRangeStop);
		return;
	}
	if (IS_MR_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
		//using offset to select stop frequency.
		gScanRangeStart = gScanRangeStart ? 0 : gTxVfo->pRX->Frequency;
		gScanRangeStop = gTxVfo->pTX->Frequency-gScanRangeStart; //TEST F DIRECT;
		//if(gTxVfo->pRX->Frequency > gTxVfo->pTX->Frequency) //manage higher frequencies such as 860MHz with negative offset
		//	gScanRangeStop = gTxVfo->pTX->Frequency;
		if(gScanRangeStart > gScanRangeStop)
			SWAP(gScanRangeStart, gScanRangeStop);
		return;
	}
#endif
	if (++gTxVfo->SCANLIST > 15) //Robby69 Yves mod reduce scanlist was >15
		gTxVfo->SCANLIST = 0;


}

static void processFKeyFunction(const KEY_Code_t Key, const bool beep)
{
	//uint8_t Band;
	uint8_t Vfo = gEeprom.TX_VFO;

	if (gScreenToDisplay == DISPLAY_MENU)
	{
//		if (beep)
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
		return;
	}
	
//	if (beep)
		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

	switch (Key)
	{
		case KEY_0:
			#ifdef ENABLE_FMRADIO
				ACTION_FM();
			#else


				// TODO: make use of this function key


			#endif
			break;

		case KEY_1:
			if (!IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
				gWasFKeyPressed = false;
				gUpdateStatus   = true;
				gBeepToPlay     = BEEP_1KHZ_60MS_OPTIONAL;

#ifdef ENABLE_COPY_CHAN_TO_VFO
				if (gEeprom.VFO_OPEN && !gCssBackgroundScan)
				{

					if (gScanStateDir != SCAN_OFF)
					{
						if (gCurrentFunction != FUNCTION_INCOMING ||
							gRxReceptionMode == RX_MODE_NONE      ||
							gScanPauseDelayIn_10ms == 0)
						{	// scan is running (not paused)
							return;
						}
					}

					const uint8_t vfo = gEeprom.TX_VFO;

					if (IS_MR_CHANNEL(gEeprom.ScreenChannel[vfo]))
					{	// copy channel to VFO, then swap to the VFO

						const unsigned int channel = FREQ_CHANNEL_FIRST + gEeprom.VfoInfo[vfo].Band;

						gEeprom.ScreenChannel[vfo] = channel;
						gEeprom.VfoInfo[vfo].CHANNEL_SAVE = channel;

						RADIO_SelectVfos();
						RADIO_ApplyTxOffset(gRxVfo);
						RADIO_ConfigureSquelchAndOutputPower(gRxVfo);
						RADIO_SetupRegisters(true);
						
						gRequestSaveChannel = 1;
						gUpdateDisplay = true;
					}
				}
				else
				{
					gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
				}
#endif
				return;
			}

#ifdef ENABLE_WIDE_RX
			if(gTxVfo->pRX->Frequency < 100000000) { //Robby69 directly go to 1Ghz
			//if(gTxVfo->Band == 6 && gTxVfo->pRX->Frequency < 100000000) {
					gTxVfo->Band = 7;
					gTxVfo->pRX->Frequency = 100000000;
					return;
			}
//			else 
#endif			



			gRequestSaveVFO            = true;
			gVfoConfigureMode          = VFO_CONFIGURE_RELOAD;

			gRequestDisplayScreen      = DISPLAY_MAIN;

			if (beep)
				gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

			break;

		case KEY_2:
			COMMON_SwitchVFOs();

			if (beep)
				gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
			break;

		case KEY_3:
			COMMON_SwitchVFOMode();

			if (beep)
				gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;

			break;

		case KEY_4:
			gWasFKeyPressed          = false;

			gBackup_CROSS_BAND_RX_TX  = gEeprom.CROSS_BAND_RX_TX;
			gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
			gUpdateStatus            = true;		
			if (beep)
				gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

			SCANNER_Start(false);
			gRequestDisplayScreen = DISPLAY_SCANNER;
			break;

		case KEY_5:
				ACTION_RunSpectrum();
				gRequestDisplayScreen = DISPLAY_MAIN;
			}
			else {
				toggle_chan_scanlist();
			}

			break;

		case KEY_6:
			APP_RunSpectrum(SCAN_BAND_MODE);
			//ACTION_Power();
			break;

		case KEY_7:
#ifdef ENABLE_VOX
			ACTION_Vox();
#else
			toggle_chan_scanlist();
#endif
			break;

		case KEY_8:
			gTxVfo->FrequencyReverse = gTxVfo->FrequencyReverse == false;
			gRequestSaveChannel = 1;
			break;

		case KEY_9:
			if (RADIO_CheckValidChannel(gEeprom.CHAN_1_CALL, false, 0))
			{
				gEeprom.MrChannel[Vfo]     = gEeprom.CHAN_1_CALL;
				gEeprom.ScreenChannel[Vfo] = gEeprom.CHAN_1_CALL;
#ifdef ENABLE_VOICE
				AUDIO_SetVoiceID(0, VOICE_ID_CHANNEL_MODE);
				AUDIO_SetDigitVoice(1, gEeprom.CHAN_1_CALL + 1);
				gAnotherVoiceID        = (VOICE_ID_t)0xFE;
#endif
				gRequestSaveVFO            = true;
				gVfoConfigureMode          = VFO_CONFIGURE_RELOAD;
				break;
			}

			if (beep)
				gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
			break;

		default:
			gUpdateStatus   = true;
			gWasFKeyPressed = false;

			if (beep)
				gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
			break;
	}
}

static void MAIN_Key_DIGITS(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	if (bKeyHeld)
	{	// key held down

		if (bKeyPressed)
		{
			if (gScreenToDisplay == DISPLAY_MAIN)
			{
				if (gInputBoxIndex > 0)
				{	// delete any inputted chars
					gInputBoxIndex        = 0;
					gRequestDisplayScreen = DISPLAY_MAIN;
				}

				gWasFKeyPressed = false;
				gUpdateStatus   = true;

				processFKeyFunction(Key, false);
			}
		}

		return;
	}

	if (bKeyPressed)
	{	// key is pressed
		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;  // beep when key is pressed
		return;                                 // don't use the key till it's released
	}

	if (!gWasFKeyPressed)
	{	// F-key wasn't pressed

		const uint8_t Vfo = gEeprom.TX_VFO;

		gKeyInputCountdown = key_input_timeout_500ms;

		INPUTBOX_Append(Key);

		gRequestDisplayScreen = DISPLAY_MAIN;

		if (IS_MR_CHANNEL(gTxVfo->CHANNEL_SAVE))
		{	// user is entering channel number

			uint16_t Channel;

			if (gInputBoxIndex != 3)
			{
				#ifdef ENABLE_VOICE
					gAnotherVoiceID   = (VOICE_ID_t)Key;
				#endif
				gRequestDisplayScreen = DISPLAY_MAIN;
				return;
			}

			gInputBoxIndex = 0;

			Channel = ((gInputBox[0] * 100) + (gInputBox[1] * 10) + gInputBox[2]) - 1;

			if (!RADIO_CheckValidChannel(Channel, false, 0))
			{
				gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
				return;
			}

			#ifdef ENABLE_VOICE
				gAnotherVoiceID        = (VOICE_ID_t)Key;
			#endif

			gEeprom.MrChannel[Vfo]     = (uint8_t)Channel;
			gEeprom.ScreenChannel[Vfo] = (uint8_t)Channel;
			gRequestSaveVFO            = true;
			gVfoConfigureMode          = VFO_CONFIGURE_RELOAD;

			return;
		}

//		#ifdef ENABLE_NOAA
//			if (!IS_NOAA_CHANNEL(gTxVfo->CHANNEL_SAVE))
//		#endif
		if (IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE))
		{	// user is entering a frequency

			uint32_t Frequency;
			bool isGigaF = gTxVfo->pRX->Frequency >= 100000000;
			if (gInputBoxIndex < 6 + isGigaF)
			{
				#ifdef ENABLE_VOICE
					gAnotherVoiceID = (VOICE_ID_t)Key;
				#endif

				return;
			}

			gInputBoxIndex = 0;
			Frequency = StrToUL(INPUTBOX_GetAscii()) * 100;

			SETTINGS_SetVfoFrequency(Frequency);
			
			gRequestSaveChannel = 1;
			return;
		}
		#ifdef ENABLE_NOAA
			else
			if (IS_NOAA_CHANNEL(gTxVfo->CHANNEL_SAVE))
			{	// user is entering NOAA channel

				uint8_t Channel;

				if (gInputBoxIndex != 2)
				{
					#ifdef ENABLE_VOICE
						gAnotherVoiceID   = (VOICE_ID_t)Key;
					#endif
					gRequestDisplayScreen = DISPLAY_MAIN;
					return;
				}

				gInputBoxIndex = 0;

				Channel = (gInputBox[0] * 10) + gInputBox[1];
				if (Channel >= 1 && Channel <= ARRAY_SIZE(NoaaFrequencyTable))
				{
					Channel                   += NOAA_CHANNEL_FIRST;
					#ifdef ENABLE_VOICE
						gAnotherVoiceID        = (VOICE_ID_t)Key;
					#endif
					gEeprom.NoaaChannel[Vfo]   = Channel;
					gEeprom.ScreenChannel[Vfo] = Channel;
					gRequestSaveVFO            = true;
					gVfoConfigureMode          = VFO_CONFIGURE_RELOAD;
					return;
				}
			}
		#endif

		gRequestDisplayScreen = DISPLAY_MAIN;
		gBeepToPlay           = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
		return;
	}

	gWasFKeyPressed = false;
	gUpdateStatus   = true;

	processFKeyFunction(Key, true);
}

static void MAIN_Key_EXIT(bool bKeyPressed, bool bKeyHeld)
{
	if (!bKeyHeld && bKeyPressed)
	{	// exit key pressed

		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

#ifdef ENABLE_DTMF
		if (gDTMF_CallState != DTMF_CALL_STATE_NONE && gCurrentFunction != FUNCTION_TRANSMIT)
		{	// clear CALL mode being displayed
			gDTMF_CallState = DTMF_CALL_STATE_NONE;
			gUpdateDisplay  = true;
			return;
		}
#endif

		#ifdef ENABLE_FMRADIO
			if (!gFmRadioMode)
		#endif
		{
			if (gScanStateDir == SCAN_OFF)
			{
				if (gInputBoxIndex == 0)
					return;
				gInputBox[--gInputBoxIndex] = 10;

				gKeyInputCountdown = key_input_timeout_500ms;

				#ifdef ENABLE_VOICE
					if (gInputBoxIndex == 0)
						gAnotherVoiceID = VOICE_ID_CANCEL;
				#endif
			}
			else
			{
				gScanKeepResult = false;
				#ifdef ENABLE_SCANNER
				CHFRSCANNER_Stop();
				#endif
				#ifdef ENABLE_VOICE
					gAnotherVoiceID = VOICE_ID_SCANNING_STOP;
				#endif
			}

			gRequestDisplayScreen = DISPLAY_MAIN;
			return;
		}

		#ifdef ENABLE_FMRADIO
			ACTION_FM();
		#endif

		return;
	}

	if (bKeyHeld && bKeyPressed)
	{	// exit key held down

		if (gInputBoxIndex > 0 || gDTMF_InputBox_Index > 0 || gDTMF_InputMode)
		{	// cancel key input mode (channel/frequency entry)
			gDTMF_InputMode       = false;
			gDTMF_InputBox_Index  = 0;
			memset(gDTMF_String, 0, sizeof(gDTMF_String));
			gInputBoxIndex        = 0;
			gRequestDisplayScreen = DISPLAY_MAIN;
			gBeepToPlay           = BEEP_1KHZ_60MS_OPTIONAL;
		}
	}
}

static void MAIN_Key_MENU(const bool bKeyPressed, const bool bKeyHeld)
{
	if (bKeyPressed && !bKeyHeld)
		// menu key pressed
		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;

	if (bKeyHeld)
	{	// menu key held down (long press)

		if (bKeyPressed)
		{	// long press MENU key

			gWasFKeyPressed = false;

			if (gScreenToDisplay == DISPLAY_MAIN)
			{
				if (gInputBoxIndex > 0)
				{	// delete any inputted chars
					gInputBoxIndex        = 0;
					gRequestDisplayScreen = DISPLAY_MAIN;
				}

				gWasFKeyPressed = false;
				gUpdateStatus   = true;

				ACTION_Handle(KEY_MENU, bKeyPressed, bKeyHeld);
			}
		}

		return;
	}

	if (!bKeyPressed && !gDTMF_InputMode)
	{	// menu key released
		#ifdef ENABLE_MESSENGER
			if (gWasFKeyPressed) {
				
				hasNewMessage = 0;
				gRequestDisplayScreen = DISPLAY_MSG;
				return;
			}
		#endif	
		const bool bFlag = (gInputBoxIndex == 0);
		gInputBoxIndex   = 0;

		if (bFlag)
		{
			if (gScanStateDir != SCAN_OFF) {
				#ifdef ENABLE_SCANNER
				CHFRSCANNER_Stop();
				#endif
				return;
			}

			gFlagRefreshSetting = true;
			gRequestDisplayScreen = DISPLAY_MENU;
			#ifdef ENABLE_VOICE
				gAnotherVoiceID   = VOICE_ID_MENU;
			#endif
		}
		else
		{
			gRequestDisplayScreen = DISPLAY_MAIN;
		}
	}
}

static void MAIN_Key_STAR(bool bKeyPressed, bool bKeyHeld)
{
	if (gCurrentFunction == FUNCTION_TRANSMIT)
		return;
	
	if (gInputBoxIndex)
	{
		if (!bKeyHeld && bKeyPressed)
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
		return;
	}
	#ifdef ENABLE_SCANNER
	if (bKeyHeld && !gWasFKeyPressed) // long press
	{	 
		if (!bKeyPressed) // released
			return; 

		ACTION_Scan(false);// toggle scanning

		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
		return;
	}

	if (bKeyPressed) // just pressed
	{	
		return;
	}
	
	// just released
	
	if (!gWasFKeyPressed) // pressed without the F-key
	{	
		if (gScanStateDir == SCAN_OFF 
#ifdef ENABLE_NOAA
			&& !IS_NOAA_CHANNEL(gTxVfo->CHANNEL_SAVE)
#endif
#ifdef ENABLE_SCAN_RANGES
			&& gScanRangeStart == 0
#endif		
		)
		{	// start entering a DTMF string
			gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
			memmove(gDTMF_InputBox, gDTMF_String, MIN(sizeof(gDTMF_InputBox), sizeof(gDTMF_String) - 1));
			gDTMF_InputBox_Index  = 0;
			gDTMF_InputMode       = true;

			gKeyInputCountdown    = key_input_timeout_500ms;

			gRequestDisplayScreen = DISPLAY_MAIN;
		}
		else
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
	}
	else

	{
		#endif
		// with the F-key
		gWasFKeyPressed = false;

#ifdef ENABLE_NOAA
		if (IS_NOAA_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
			return;
		}				
#endif

		// scan the CTCSS/DCS code
		gBackup_CROSS_BAND_RX_TX  = gEeprom.CROSS_BAND_RX_TX;
		gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
		SCANNER_Start(true);
		gRequestDisplayScreen = DISPLAY_SCANNER;
		#ifdef ENABLE_SCANNER
	}
	
	gPttWasReleased = true;
	gUpdateStatus   = true;
	#endif
}

static void MAIN_Key_UP_DOWN(bool bKeyPressed, bool bKeyHeld, int8_t Direction)
{
	uint8_t Channel = gEeprom.ScreenChannel[gEeprom.TX_VFO];

	if (bKeyHeld || !bKeyPressed)
	{
		if (gInputBoxIndex > 0)
			return;

		if (!bKeyPressed)
		{
			if (!bKeyHeld)
				return;

			if (IS_FREQ_CHANNEL(Channel))
				return;

			#ifdef ENABLE_VOICE
				AUDIO_SetDigitVoice(0, gTxVfo->CHANNEL_SAVE + 1);
				gAnotherVoiceID = (VOICE_ID_t)0xFE;
			#endif

			return;
		}
	}
	else
	{
		if (gInputBoxIndex > 0)
		{
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
			return;
		}

		gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
	}

	if (gScanStateDir == SCAN_OFF)
	{
		#ifdef ENABLE_NOAA
			if (!IS_NOAA_CHANNEL(Channel))
		#endif
		{
			uint8_t Next;

			if (IS_FREQ_CHANNEL(Channel))
			{	// step/down in frequency
				const uint32_t frequency = APP_SetFrequencyByStep(gTxVfo, Direction);

				if (RX_freq_check(frequency) < 0)
				{	// frequency not allowed
					gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
					return;
				}

				gTxVfo->freq_config_RX.Frequency = frequency;
				BK4819_SetFrequency(frequency);
				BK4819_RX_TurnOn();
				gRequestSaveChannel = 1;
				return;
			}

			Next = RADIO_FindNextChannel(Channel + Direction, Direction, false, 0);
			if (Next == 0xFF)
				return;

			if (Channel == Next)
				return;

			gEeprom.MrChannel[gEeprom.TX_VFO]     = Next;
			gEeprom.ScreenChannel[gEeprom.TX_VFO] = Next;

			if (!bKeyHeld)
			{
				#ifdef ENABLE_VOICE
					AUDIO_SetDigitVoice(0, Next + 1);
					gAnotherVoiceID = (VOICE_ID_t)0xFE;
				#endif
			}
		}
		#ifdef ENABLE_NOAA
			else
			{
				Channel = NOAA_CHANNEL_FIRST + NUMBER_AddWithWraparound(gEeprom.ScreenChannel[gEeprom.TX_VFO] - NOAA_CHANNEL_FIRST, Direction, 0, 9);
				gEeprom.NoaaChannel[gEeprom.TX_VFO]   = Channel;
				gEeprom.ScreenChannel[gEeprom.TX_VFO] = Channel;
			}
		#endif

		gRequestSaveVFO   = true;
		gVfoConfigureMode = VFO_CONFIGURE_RELOAD;
		return;
	}
	#ifdef ENABLE_SCANNER
	// jump to the next channel
	CHFRSCANNER_Start(false, Direction);
	gScanPauseDelayIn_10ms = 1;
	gScheduleScanListen    = false;
	#endif
	gPttWasReleased = true;
}

void MAIN_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	#ifdef ENABLE_FMRADIO
		if (gFmRadioMode && Key != KEY_PTT && Key != KEY_EXIT)
		{
			if (!bKeyHeld && bKeyPressed)
				gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
			return;
		}
	#endif

	if (gDTMF_InputMode && bKeyPressed && !bKeyHeld)
	{
		const char Character = DTMF_GetCharacter(Key);
		if (Character != 0xFF)
		{	// add key to DTMF string
			DTMF_Append(Character);
			gKeyInputCountdown    = key_input_timeout_500ms;
			gRequestDisplayScreen = DISPLAY_MAIN;
			gPttWasReleased       = true;
			gBeepToPlay           = BEEP_1KHZ_60MS_OPTIONAL;
			return;
		}
	}

	// TODO: ???
//	if (Key > KEY_PTT)
//	{
//		Key = KEY_SIDE2;      // what's this doing ???
//	}

	switch (Key)
	{
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			MAIN_Key_DIGITS(Key, bKeyPressed, bKeyHeld);
			break;
		case KEY_MENU:
			MAIN_Key_MENU(bKeyPressed, bKeyHeld);
			break;
		case KEY_UP:
			MAIN_Key_UP_DOWN(bKeyPressed, bKeyHeld, 1);
			break;
		case KEY_DOWN:
			MAIN_Key_UP_DOWN(bKeyPressed, bKeyHeld, -1);
			break;
		case KEY_EXIT:
			MAIN_Key_EXIT(bKeyPressed, bKeyHeld);
			break;
		case KEY_STAR:
			MAIN_Key_STAR(bKeyPressed, bKeyHeld);
			break;
		case KEY_F:
			GENERIC_Key_F(bKeyPressed, bKeyHeld);
			break;
		case KEY_PTT:
			GENERIC_Key_PTT(bKeyPressed);
			break;
		default:
			if (!bKeyHeld && bKeyPressed)
				gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
			break;
	}
}
