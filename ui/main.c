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
#include <stdlib.h>  // abs()
#include "bitmaps.h"
#include "board.h"
#include "driver/bk4819.h"
#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "functions.h"
#include "helper/battery.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/helper.h"
#include "ui/inputbox.h"
#include "ui/main.h"
#include "ui/ui.h"
center_line_t center_line = CENTER_LINE_NONE;

// ***************************************************************************

static void DrawSmallAntennaAndBars(uint8_t *p, unsigned int level)
{
	if(level>6)
		level = 6;

	//memcpy(p, BITMAP_Antenna, ARRAY_SIZE(BITMAP_Antenna));

	for(uint8_t i = 1; i <= level; i++) {
		char bar = (0xff << (6-i)) & 0x7F;
		memset(p + 2 + i*3, bar, 2);
	}
}
#if defined ENABLE_AUDIO_BAR || defined ENABLE_RSSI_BAR

static void DrawLevelBar(uint8_t xpos, uint8_t line, uint8_t level)
{
	/*const char hollowBar[] = {
		0b01111111,
		0b01000001,
		0b01000001,
		0b01111111
	};*/

	uint8_t *p_line = gFrameBuffer[line];
	level = MIN(level, 13);

	for(uint8_t i = 0; i < level; i++) {
		if(i < 9) {
			for(uint8_t j = 0; j < 4; j++)
				p_line[xpos + i * 5 + j] = (~(0x7F >> (i+1))) & 0x7F;
		}
		/*else {
			memcpy(p_line + (xpos + i * 5), &hollowBar, ARRAY_SIZE(hollowBar));
		}*/
	}
}
#endif

#ifdef ENABLE_AUDIO_BAR

unsigned int sqrt16(unsigned int value)
{	// return square root of 'value'
	unsigned int shift = 16;         // number of bits supplied in 'value' .. 2 ~ 32
	unsigned int bit   = 1u << --shift;
	unsigned int sqrti = 0;
	while (bit)
	{
		const unsigned int temp = ((sqrti << 1) | bit) << shift--;
		if (value >= temp) {
			value -= temp;
			sqrti |= bit;
		}
		bit >>= 1;
	}
	return sqrti;
}

void UI_DisplayAudioBar(void)
{
	if(gLowBattery && !gLowBatteryConfirmed)
		return;

	const unsigned int line      = 3;

	if (gCurrentFunction != FUNCTION_TRANSMIT ||
		gScreenToDisplay != DISPLAY_MAIN
		)
	{
		return;  // screen is in use
	}
			
	#if defined(ENABLE_TX1750)
		if (gAlarmState != ALARM_STATE_OFF)
			return;
	#endif
	const unsigned int voice_amp  = BK4819_GetVoiceAmplitudeOut();  // 15:0

	// make non-linear to make more sensitive at low values
	const unsigned int level      = MIN(voice_amp * 8, 65535u);
	const unsigned int sqrt_level = MIN(sqrt16(level), 124u);
	uint8_t bars = 13 * sqrt_level / 124;

	uint8_t *p_line = gFrameBuffer[line];
	memset(p_line, 0, LCD_WIDTH);

	DrawLevelBar(6, line, bars);

	if (gCurrentFunction == FUNCTION_TRANSMIT)
		ST7565_BlitFullScreen();

}
#endif


static void DisplayRSSIBar(const int16_t rssi, const bool now)
{
    const char plus[] = {
        0b00011000,
        0b00011000,
        0b01111110,
        0b01111110,
        0b01111110,
        0b00011000,
        0b00011000,
    };
	
	if (center_line == CENTER_LINE_RSSI) {
		const unsigned int txt_width    = 7 * 8;                 // 8 text chars
		const unsigned int bar_x        = 2 + txt_width + 4;     // X coord of bar graph
		//const unsigned int bar_x        = 20; 
		const unsigned int line         = 3;
		uint8_t           *p_line        = gFrameBuffer[line];
		char               str[16];

		if (gEeprom.KEY_LOCK && gKeypadLocked > 0)
			return;     // display is in use

		if (gCurrentFunction == FUNCTION_TRANSMIT ||
			gScreenToDisplay != DISPLAY_MAIN
			)
			return;     // display is in use

		if (now)
			memset(p_line, 0, LCD_WIDTH);

		sLevelAttributes sLevelAtt;
		
		sLevelAtt = GetSLevelAttributes(rssi, gTxVfo->freq_config_RX.Frequency);
		
		uint8_t overS9Bars = MIN(sLevelAtt.over/10, 4);
		
		if(overS9Bars == 0) {
			sprintf(str, "% 4d S%d", sLevelAtt.dBmRssi, sLevelAtt.sLevel); 
		}
		else {
			sprintf(str, "% 4d  %2d", sLevelAtt.dBmRssi, sLevelAtt.over);
			memcpy(p_line + 2 + 7*5, &plus, ARRAY_SIZE(plus));
		}
		UI_PrintStringSmall(str, 2, 0, line); 
		DrawLevelBar(bar_x, line, sLevelAtt.sLevel + overS9Bars);
	}

	if (now)
		ST7565_BlitFullScreen();
}


void UI_UpdateRSSI(const int16_t rssi, const int vfo)
{
	(void)vfo;  // unused
	
	// optional larger RSSI dBm, S-point and bar level

	if (gCurrentFunction == FUNCTION_RECEIVE ||
		gCurrentFunction == FUNCTION_MONITOR ||
		gCurrentFunction == FUNCTION_INCOMING)
	{
		
		DisplayRSSIBar(rssi, true);
	}

}

// ***************************************************************************

void UI_DisplayMain(void)
{
	const unsigned int line0 = 0;  // text screen line
	char               String[22];
	unsigned int       vfo_num;

	center_line = CENTER_LINE_NONE;

	// clear the screen
	memset(gFrameBuffer, 0, sizeof(gFrameBuffer));

	if(gLowBattery && !gLowBatteryConfirmed) {
		UI_DisplayPopup("LOW BATTERY");
		ST7565_BlitFullScreen();
		return;
	}

	if (gEeprom.KEY_LOCK && gKeypadLocked > 0)
	{	// tell user how to unlock the keyboard
		UI_PrintString("Long press #", 0, LCD_WIDTH, 1, 8);
		UI_PrintString("to unlock",    0, LCD_WIDTH, 3, 8);
		ST7565_BlitFullScreen();
		return;
	}
	vfo_num = 0;
	const unsigned int line       = line0 ;
	uint8_t           *p_line    = gFrameBuffer[line + 5];
	unsigned int       mode       = 0;

		if (gCurrentFunction == FUNCTION_TRANSMIT){// transmitting
			mode = 1;
			UI_PrintStringSmallBold("TX", 0, 0, line+2);
		}
		else
		{	// receiving .. show the RX symbol
			mode = 2;
			if ((gCurrentFunction == FUNCTION_RECEIVE ||
			     gCurrentFunction == FUNCTION_MONITOR ||
			     gCurrentFunction == FUNCTION_INCOMING) &&
			     0 == vfo_num)
			{UI_PrintStringSmallBold("RX", 0, 0, line+2);}
		}

		if (IS_MR_CHANNEL(gEeprom.ScreenChannel[vfo_num]))
		{	// channel mode
			const bool inputting = (gInputBoxIndex == 0 || 0 != vfo_num) ? false : true;
			if (!inputting)
				sprintf(String, "M%u", gEeprom.ScreenChannel[vfo_num] + 1);
			else
				sprintf(String, "M%.3s", INPUTBOX_GetAscii());  // show the input text
			//UI_PrintStringSmall(String, x, 0, line);
			UI_PrintString(String , 0, 0, line ,8);
		}
		
		unsigned int state = VfoState;
		uint32_t frequency = gEeprom.VfoInfo[vfo_num].pRX->Frequency;

		if (state != VFO_STATE_NORMAL)
		{
			const char *state_list[] = {"", "BUSY", "BAT LOW", "TX DISABLE", "TIMEOUT", "ALARM", "VOLT HIGH"};
			if (state < ARRAY_SIZE(state_list))
				UI_PrintString(state_list[state], 31, 0, line+2, 8);
		}

		if (gInputBoxIndex > 0 && IS_FREQ_CHANNEL(gEeprom.ScreenChannel[vfo_num])){	// user entering a frequency
				const char * ascii = INPUTBOX_GetAscii();
				bool isGigaF = frequency>=100000000;
				sprintf(String, "%.*s.%.3s", 3 + isGigaF, ascii, ascii + 3 + isGigaF);
				// show the remaining 2 small frequency digits
				UI_PrintStringSmall(String + 7, 109, 0, line + 1); //temp
				String[7] = 0;
				// show the main large frequency digits
				UI_DisplayFrequency(String, 16, line, false);
			}
			else
			{
				if (gCurrentFunction == FUNCTION_TRANSMIT)
				{	// transmitting
				frequency = gEeprom.VfoInfo[vfo_num].pTX->Frequency;
				}
				// Always show frequency
				sprintf(String, "%3u.%05u", frequency / 100000, frequency % 100000);   //temp
				// show the remaining 2 small frequency digits
				UI_PrintString(String + 7, 100, 0, line + 4,9);
				String[7] = 0;
				// show the main large frequency digits
				UI_DisplayFrequency(String, 16, line+4, false);

				if (IS_MR_CHANNEL(gEeprom.ScreenChannel[vfo_num]))
				{	// it's a channel
				
					// show the scan list assigment symbols
					/*const ChannelAttributes_t att = gMR_ChannelAttributes[gEeprom.ScreenChannel[vfo_num]];
					if (att.scanlist > 0) {
						sprintf(String, "%d", att.scanlist);
						UI_PrintString(String , 0, 0, line+2 ,7);
					}*/
				
					SETTINGS_FetchChannelName(String, gEeprom.ScreenChannel[vfo_num]);
					if (String[0] == 0)
					{	// no channel name, show the channel number instead
						sprintf(String, "CH-%03u", gEeprom.ScreenChannel[vfo_num] + 1);
					}
						UI_PrintString(String, 46, 0,line,8);
				}
			}


		// ************

		// show the TX/RX level
		uint8_t Level = 0;
		if (mode == 1)
		{	// TX power level
			switch (gTxVfo->OUTPUT_POWER)
			{
				case OUTPUT_POWER_LOW:  Level = 2; break;
				case OUTPUT_POWER_MID:  Level = 4; break;
				case OUTPUT_POWER_HIGH: Level = 6; break;
			}
		}
		else if (mode == 2)
			{	// RX signal level
				#ifndef ENABLE_RSSI_BAR
					// bar graph
					if (gVFO_RSSI_bar_level[vfo_num] > 0)
						Level = gVFO_RSSI_bar_level[vfo_num];
				#endif
			}
		if(Level) DrawSmallAntennaAndBars(p_line + LCD_WIDTH, Level);
		


		String[0] = '\0';

		// show the modulation symbol
		const char * s = "";
		const ModulationMode_t mod = gEeprom.VfoInfo[vfo_num].Modulation;
		switch (mod){
			case MODULATION_FM: {
				const FREQ_Config_t *pConfig = (mode == 1) ? gEeprom.VfoInfo[vfo_num].pTX : gEeprom.VfoInfo[vfo_num].pRX;
				const unsigned int code_type = pConfig->CodeType;
				const char *code_list[] = {"FM", "CT", "DCS", "DCR"};
				if (code_type < ARRAY_SIZE(code_list))
					s = code_list[code_type];
				break;
			}
			default:
				s = gModulationStr[mod];
			break;
		}		
		UI_PrintStringSmall(s, LCD_WIDTH + 25, 0, line +5);

		// show the Ptt_Toggle_Mode
		
		if (Ptt_Toggle_Mode) 	UI_PrintStringSmall("T", LCD_WIDTH + 0, 0, line +5);

		if (state == VFO_STATE_NORMAL || state == VFO_STATE_ALARM)
		{	// show the TX power
			const char pwr_list[] = "LMH";
			const unsigned int i = gEeprom.VfoInfo[vfo_num].OUTPUT_POWER;
			String[0] = (i < ARRAY_SIZE(pwr_list)) ? pwr_list[i] : '\0';
			String[1] = '\0';
			UI_PrintStringSmall(String, LCD_WIDTH + 50, 0, line + 5);
		}

		if (gEeprom.VfoInfo[vfo_num].freq_config_RX.Frequency != gEeprom.VfoInfo[vfo_num].freq_config_TX.Frequency)
		{	// show the TX offset symbol
			const char dir_list[] = "\0+-";
			const unsigned int i = gEeprom.VfoInfo[vfo_num].TX_OFFSET_FREQUENCY_DIRECTION;
			String[0] = (i < sizeof(dir_list)) ? dir_list[i] : '?';
			String[1] = '\0';
			UI_PrintStringSmall(String, LCD_WIDTH + 60, 0, line + 5);
		}

		{	// show the narrow band symbol
			UI_PrintStringSmall(bwNames[gEeprom.VfoInfo[vfo_num].CHANNEL_BANDWIDTH], LCD_WIDTH + 70, 0, line + 5);
		}

		// show the audio scramble symbol
		if (gEeprom.VfoInfo[vfo_num].SCRAMBLING_TYPE > 0 && gSetting_ScrambleEnable)
			UI_PrintStringSmall("SCR", LCD_WIDTH + 106, 0, line + 5);

	if (center_line == CENTER_LINE_NONE)
	{	// we're free to use the middle line

		const bool rx = (gCurrentFunction == FUNCTION_RECEIVE ||
		                 gCurrentFunction == FUNCTION_MONITOR ||
		                 gCurrentFunction == FUNCTION_INCOMING);


		if (gCurrentFunction == FUNCTION_TRANSMIT) {
			center_line = CENTER_LINE_AUDIO_BAR;
			UI_DisplayAudioBar();
		}

		if (rx) {
			center_line = CENTER_LINE_RSSI;
			DisplayRSSIBar(gCurrentRSSI[0], false);
		}
	}

	ST7565_BlitFullScreen();
		
}

