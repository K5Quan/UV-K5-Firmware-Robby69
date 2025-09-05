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

#ifndef MISC_H
#define MISC_H

#include <stdbool.h>
#include <stdint.h>

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef MAX
	#define MAX(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#endif

#ifndef MIN
	#define MIN(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#endif

#ifndef SWAP
	#define SWAP(a, b) ({ __typeof__ (a) _c = (a);  a = b; b = _c; })
#endif

#define IS_MR_CHANNEL(x)       ((x) <= MR_CHANNEL_LAST)
#define IS_FREQ_CHANNEL(x)     ((x) >= FREQ_CHANNEL_FIRST && (x) <= FREQ_CHANNEL_LAST)
#define IS_VALID_CHANNEL(x)    ((x) < LAST_CHANNEL)

enum {
	MR_CHANNEL_FIRST   = 0,
	MR_CHANNEL_LAST    = 199u,
	FREQ_CHANNEL_FIRST = 200u,
	FREQ_CHANNEL_LAST  = 206u,
	LAST_CHANNEL
};

#ifdef ENABLE_PWRON_PASSWORD
enum {
	PASSWORD_OFF = 10000u
};
#define PASSWORD_MAX_RETRIES 3
#endif

enum {
	FLASHLIGHT_OFF = 0,
	FLASHLIGHT_ON,
	FLASHLIGHT_BLINK,
	FLASHLIGHT_SOS
};

enum {
	VFO_CONFIGURE_NONE = 0,
	VFO_CONFIGURE,
	VFO_CONFIGURE_RELOAD
};

enum AlarmState_t {
	ALARM_STATE_OFF = 0,
	ALARM_STATE_TXALARM,
	ALARM_STATE_ALARM,
	ALARM_STATE_TX1750
};
typedef enum AlarmState_t AlarmState_t;

enum ReceptionMode_t {
	RX_MODE_NONE = 0,   // squelch close ?
	RX_MODE_DETECTED,   // signal detected
	RX_MODE_LISTENING   //
};
typedef enum ReceptionMode_t ReceptionMode_t;

enum BacklightOnRxTx_t {
	BACKLIGHT_ON_TR_OFF,
	BACKLIGHT_ON_TR_TX,
	BACKLIGHT_ON_TR_RX,
	BACKLIGHT_ON_TR_TXRX
};

extern const uint8_t         fm_radio_countdown_500ms;
extern const uint16_t        fm_play_countdown_scan_10ms;
extern const uint16_t        fm_play_countdown_noscan_10ms;
extern const uint16_t        fm_restore_countdown_10ms;

extern const uint8_t        vfo_state_resume_countdown_500ms;

extern const uint8_t         menu_timeout_500ms;
extern const uint16_t        menu_timeout_long_500ms;

extern const uint8_t         key_input_timeout_500ms;

extern const uint16_t        key_repeat_delay_10ms;
extern const uint16_t        key_repeat_10ms;
extern const uint16_t        key_debounce_10ms;

extern const uint8_t         scan_delay_10ms;

extern const uint16_t        battery_save_count_10ms;

extern const uint16_t        power_save1_10ms;
extern const uint16_t        power_save2_10ms;

extern const uint8_t         gMicGain_dB2[5];

extern bool                  gSetting_350TX;

extern uint8_t               gSetting_F_LOCK;
extern bool                  gSetting_ScrambleEnable;

extern uint8_t               gSetting_backlight_on_tx_rx;

extern uint8_t               gSetting_battery_text;

extern bool                  gMonitor;

extern const uint32_t        gDefaultAesKey[4];
extern uint32_t              gCustomAesKey[4];
extern bool                  bHasCustomAesKey;
extern uint32_t              gChallenge[4];
extern uint8_t               gTryCount;

extern uint16_t              gEEPROM_RSSI_CALIB[7][4];

extern uint16_t              gEEPROM_1F8A;
extern uint16_t              gEEPROM_1F8C;

typedef union { 
    struct {
        uint8_t
            band : 4,
            scanlist : 4;
    };
    uint8_t __val;
} ChannelAttributes_t;

#ifdef ENABLE_SPECTRUM_SHOW_CHANNEL_NAME
typedef struct
{
	uint32_t     Frequency;
	char         Name[12];
}  __attribute__((packed)) ChannelFrequencyAttributes;

extern ChannelFrequencyAttributes gMR_ChannelFrequencyAttributes[200];
#endif

#ifdef ENABLE_SCREENSHOT
         extern volatile uint8_t  gUART_LockScreenshot; // lock screenshot if Chirp is used
#endif

extern ChannelAttributes_t   gMR_ChannelAttributes[207];

typedef struct
{
	uint8_t      sLevel;      // S-level value
	uint8_t      over;        // over S9 value
	int          dBmRssi;     // RSSI in dBm
	bool         overSquelch; // determines whether signal is over squelch open threshold
}  __attribute__((packed))  sLevelAttributes;

extern volatile uint16_t     gBatterySaveCountdown_10ms;

extern volatile bool         gPowerSaveCountdownExpired;
extern volatile bool         gSchedulePowerSave;

extern volatile bool         gScheduleDualWatch;

extern volatile uint16_t     gDualWatchCountdown_10ms;
extern bool                  gDualWatchActive;

extern volatile uint8_t      gSerialConfigCountDown_500ms;

extern volatile bool         gNextTimeslice_500ms;
extern volatile bool         gNextTimeslice_display;
extern volatile uint16_t     gTxTimerCountdown_500ms;
extern volatile uint16_t     gTxTimerCountdown_100ms;
extern volatile bool         gTxTimeoutReached;
extern volatile bool         gTxTimeoutReached2;

extern volatile uint16_t     gTailNoteEliminationCountdown_10ms;
extern volatile uint16_t gFmPlayCountdown_10ms;
extern bool                  gEnableSpeaker;
extern uint8_t               gKeyInputCountdown;
extern uint8_t               gKeyLockCountdown;
extern bool                  bIsInLockScreen;
extern uint8_t               gUpdateStatus;
extern uint8_t               gFoundCTCSS;
extern uint8_t               gFoundCDCSS;
extern bool                  gEndOfRxDetectedMaybe;

extern int16_t               gVFO_RSSI[2];
extern uint8_t               gVFO_RSSI_bar_level[2];

// battery critical, limit functionality to minimum
extern uint8_t               gReducedService;
extern uint8_t               gBatteryVoltageIndex;

// we are searching CTCSS/DCS inside RX ctcss/dcs menu
extern bool         gCssBackgroundScan;


enum
{
	SCAN_REV = -1,
	SCAN_OFF =  0,
	SCAN_FWD = +1
};

extern volatile bool     gScheduleScanListen;
extern volatile uint16_t gScanPauseDelayIn_10ms;

extern bool                  gUpdateRSSI;
extern AlarmState_t          gAlarmState;
extern uint16_t              gMenuCountdown;
extern bool                  gPttWasReleased;
extern bool                  gPttWasPressed;
extern bool                  gFlagReconfigureVfos;
extern uint8_t               gVfoConfigureMode;
extern bool                  gFlagResetVfos;
extern bool                  gRequestSaveVFO;
extern uint8_t               gRequestSaveChannel;
extern bool                  gRequestSaveSettings;
extern uint8_t               gKeypadLocked;
extern bool                  gFlagPrepareTX;

extern bool                  gFlagAcceptSetting;   // accept menu setting
extern bool                  gFlagRefreshSetting;  // refresh menu display

extern bool                  gFlagSaveVfo;
extern bool                  gFlagSaveSettings;
extern bool                  gFlagSaveChannel;
extern bool                  g_CDCSS_Lost;
extern uint8_t               gCDCSSCodeType;
extern bool                  g_CTCSS_Lost;
extern bool                  g_CxCSS_TAIL_Found;

// true means we are receiving signal
extern bool                  g_SquelchLost;
extern uint8_t               gFlashLightState;
extern bool 				 Ptt_Toggle_Mode;
extern volatile uint16_t     gFlashLightBlinkCounter;
extern bool                  gFlagEndTransmission;
extern uint8_t               gNextMrChannel;
extern ReceptionMode_t       gRxReceptionMode;

 //TRUE when dual watch is momentarly suspended and RX_VFO is locked to either last TX or RX
extern bool                  gTxVfoIsActive;
extern uint8_t               gAlarmToneCounter;
extern uint16_t              gAlarmRunningCounter;
extern bool                  gKeyBeingHeld;
extern bool                  gPttIsPressed;
extern uint8_t               gPttDebounceCounter;
extern uint8_t               gMenuListCount;

extern uint8_t               gFSKWriteIndex;
extern volatile bool         gNextTimeslice;
extern bool                  gUpdateDisplay;
extern bool                  gF_LOCK;
extern uint8_t               gShowChPrefix;
extern volatile uint8_t      gFoundCDCSSCountdown_10ms;
extern volatile uint8_t      gFoundCTCSSCountdown_10ms;
extern volatile bool         gNextTimeslice40ms;
extern volatile bool         gNextTimeslice_keys;

extern volatile bool         gFlagTailNoteEliminationComplete;
extern volatile uint8_t      gVFOStateResumeCountdown_500ms;
extern volatile bool     gScheduleFM;
extern int16_t               gCurrentRSSI[2];   // now one per VFO
extern uint8_t               gIsLocked;
extern volatile uint8_t      boot_counter_10ms;

int32_t NUMBER_AddWithWraparound(int32_t Base, int32_t Add, int32_t LowerLimit, int32_t UpperLimit);
unsigned long StrToUL(const char * str);

bool IsValueInArray(int val, const int *arr, const int size);
sLevelAttributes GetSLevelAttributes (const int16_t rssi, const uint32_t frequency);
int Rssi2DBm(const uint16_t rssi);

#endif

