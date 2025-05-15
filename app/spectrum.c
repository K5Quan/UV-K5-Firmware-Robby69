#include "app/spectrum.h"
#include "chFrScanner.h"
#include "driver/backlight.h"
#include "driver/eeprom.h"   // EEPROM_ReadBuffer()
#include "audio.h"
#include "ui/helper.h"
#include "common.h"
#include "action.h"

struct FrequencyBandInfo {
    uint32_t lower;
    uint32_t upper;
    uint32_t middle;
};

bool isBlacklistApplied;

uint32_t cdcssFreq;
uint16_t ctcssFreq;
uint8_t refresh = 0;
uint8_t SquelchBarKeyMode = 2; //Robby69 change keys between audio and history squelch

#define F_MAX frequencyBandTable[ARRAY_SIZE(frequencyBandTable) - 1].upper

#define Bottom_print 51 //Robby69
Mode appMode;
#define UHF_NOISE_FLOOR 40
#define MAX_ATTENUATION 160
#define ATTENUATE_STEP  10
bool    isNormalizationApplied;
uint8_t  gainOffset[129];
uint8_t  attenuationOffset[129];
uint8_t scanChannel[MR_CHANNEL_LAST+3];
uint8_t scanChannelsCount;
void ToggleScanList();
void ToggleNormalizeRssi(bool on);
static void LoadSettings();
static void SaveSettings();
  
const uint16_t RSSI_MAX_VALUE = 65535;

#define SQUELCH_OFF_DELAY 100 //Robby69

static uint16_t R30, R37, R3D, R43, R47, R48, R7E, R02, R3F;
static uint32_t initialFreq;
static char String[32];
static char StringC[10];
uint32_t lastPeakFrequency;
bool isKnownChannel = false;
int  channel;
int  latestChannel;
char channelName[12];
char rxChannelName[12];
ModulationMode_t  channelModulation;
BK4819_FilterBandwidth_t channelBandwidth;
void LoadValidMemoryChannels(void);
bool isInitialized = false;
bool isListening = true;
bool monitorMode = false;
bool redrawStatus = true;
bool redrawScreen = false;
bool newScanStart = true;
bool preventKeypress = true;
bool audioState = true;
uint8_t waitingForScanListNumber = 0;
uint8_t bl;
uint8_t CurrentScanBand = 1;
State currentState = SPECTRUM, previousState = SPECTRUM;

PeakInfo peak;
ScanInfo scanInfo;
KeyboardState kbd = {KEY_INVALID, KEY_INVALID, 0};


#define BLACKLIST_SIZE 200
static uint16_t blacklistFreqs[BLACKLIST_SIZE];
static uint8_t blacklistFreqsIdx;
static bool IsBlacklisted(uint16_t idx);
static uint8_t CurrentScanIndex();

char     latestScanListName[12];

const char *bwOptions[] = {"  25k", "12.5k", "6.25k"};
const char *scanListOptions[] = {"SL1", "SL2", "SL3", "SL4", "SL5", "SL6", 
  "SL7", "SL8", "SL9", "SL10", "SL11", "SL12", "SL13", "SL14", "SL15", "ALL"};
const uint8_t modulationTypeTuneSteps[] = {100, 50, 10};
const uint8_t modTypeReg47Values[] = {1, 7, 5};
//bool       gWasFKeyPressed  = false;

SpectrumSettings settings = {stepsCount: STEPS_128,
                             scanStepIndex: S_STEP_25_0kHz,
                             frequencyChangeStep: 80000,
                             rssiTriggerLevel: 150,
							               rssiTriggerLevelH: 150,
                             backlightAlwaysOn: false,
                             bw: BK4819_FILTER_BW_WIDE,
                             listenBw: BK4819_FILTER_BW_WIDE,
                             modulationType: false,
                             dbMin: -130,
                             dbMax: 10,
                             scanList: S_SCAN_LIST_ALL,
                             scanListEnabled: {0},
                             bandEnabled: {0}
                            };

/*
bandparameters BParams[15] = {
  // BandName         Startfrequency    Stopfrequency   scanStep        dbMax  modulationType
    {"CB",             2651500,          2830500,       S_STEP_5_0kHz,  -20    ,MODULATION_AM},
    {"HAM 144",       14400000,         14600000,       S_STEP_12_5kHz, -20    ,MODULATION_FM},
    {"HAM 430",       43000000,         44000000,       S_STEP_10_0kHz, -30    ,MODULATION_FM},
    {"AIR",           11800000,         13600000,       S_STEP_25_0kHz, -30    ,MODULATION_AM},
    {"PMR",           44600625,         44619375,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"LPD",           43307500,         43377500,       S_STEP_6_25kHz, -30    ,MODULATION_FM},
    {"DMR VHF",       14600000,         17000000,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"DMR UHF",       45000000,         47000000,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"HAM 50",         5000000,          5256000,       S_STEP_10_0kHz, -20    ,MODULATION_FM},
    {"MARINE",        15605000,         16200000,       S_STEP_25_0kHz, -30    ,MODULATION_FM},
    {"SRD868",        86800000,         87000000,       S_STEP_6_25kHz, -30    ,MODULATION_FM},
    {"",              11100000,         128,            S_STEP_5_0kHz,  -30    ,MODULATION_FM},
    {"",              11200000,         128,            S_STEP_5_0kHz,  -30    ,MODULATION_FM},
    {"",              11400000,         128,            S_STEP_5_0kHz,  -30    ,MODULATION_FM}
    }; 
*/

bandparameters BParams[15] = {
    // BandName         Startfrequency    Stopfrequency   scanStep        dbMax  modulationType
    {"26-28MHz",         2651500,          2830500,       S_STEP_5_0kHz,  -30    ,MODULATION_AM},
    {"144-146MHz",      14400000,         14600000,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"430-440MHz",      43000000,         44000000,       S_STEP_10_0kHz, -30    ,MODULATION_FM},
    {"118-136MHz",      11800000,         13600000,       S_STEP_25_0kHz, -30    ,MODULATION_AM},
    {"446MHz",          44600625,         44619375,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"433Mhz",          43307500,         43377500,       S_STEP_6_25kHz, -30    ,MODULATION_FM},
    {"146-170Mhz",      14600000,         17000000,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"450-470MHz",      45000000,         47000000,       S_STEP_12_5kHz, -30    ,MODULATION_FM},
    {"50MHz",            5000000,          5256000,       S_STEP_10_0kHz, -30    ,MODULATION_FM},
    {"MARINE",          15605000,         16200000,       S_STEP_25_0kHz, -30    ,MODULATION_FM},
    {"SRD868",          86800000,         87000000,       S_STEP_6_25kHz, -30    ,MODULATION_FM},
    {"500M-500k",       10000000,         50000000,       S_STEP_500kHz,  -30    ,MODULATION_FM},
    {"433M_0.5k",       43200000,         43400000,       S_STEP_0_5kHz,  -30    ,MODULATION_FM},
    {"",                 2500000,          3000000,       S_STEP_5_0kHz,  -30    ,MODULATION_FM}
    }; 



uint32_t fMeasure = 0;
uint32_t currentFreq, tempFreq;
uint16_t rssiHistory[128];
const uint8_t FMaxNumb = 100;
uint32_t freqHistory[100]; //Robby69
uint8_t freqCount[100] = {0};     // nowa tablica liczników - zylka
uint8_t indexFd = 1;
uint8_t indexFs = 1;
bool ShowHistory = false;
uint8_t freqInputIndex = 0;
uint8_t freqInputDotIndex = 0;
KEY_Code_t freqInputArr[10];
char freqInputString[11];

static uint8_t nextBandToScanIndex = 0; // Indeks następnego pasma do sprawdzenia (0-14) - zylka

uint8_t menuState = 0;
uint16_t listenT = 0;
uint8_t rxChannelDisplayCountdown = 0;

RegisterSpec registerSpecs[] = {
    {},
    {"LNAs", BK4819_REG_13, 8, 0b11,  1},
    {"LNA",  BK4819_REG_13, 5, 0b111, 1},
    {"PGA",  BK4819_REG_13, 0, 0b111, 1},
    {"MIX",  BK4819_REG_13, 3, 0b11,  1},
};

uint16_t statuslineUpdateTimer = 0;

static void RelaunchScan();
static void ResetInterrupts();

static uint16_t GetRegMenuValue(uint8_t st) {
  RegisterSpec s = registerSpecs[st];
  return (BK4819_ReadRegister(s.num) >> s.offset) & s.mask;
}

static void SetRegMenuValue(uint8_t st, bool add) {
  uint16_t v = GetRegMenuValue(st);
  RegisterSpec s = registerSpecs[st];

  uint16_t reg = BK4819_ReadRegister(s.num);
  if (add && v <= s.mask - s.inc) {
    v += s.inc;
  } else if (!add && v >= 0 + s.inc) {
    v -= s.inc;
  }
  // TODO: use max value for bits count in max value, or reset by additional
  // mask in spec
  reg &= ~(s.mask << s.offset);
  BK4819_WriteRegister(s.num, reg | (v << s.offset));
  redrawScreen = true;
}

// Utility functions

KEY_Code_t GetKey() {
  KEY_Code_t btn = KEYBOARD_Poll();
  if (btn == KEY_INVALID && !GPIO_CheckBit(&GPIOC->DATA, GPIOC_PIN_PTT)) {
    btn = KEY_PTT;
  }
  return btn;
}

static int clamp(int v, int min, int max) {
  return v <= min ? min : (v >= max ? max : v);
}

void SetState(State state) {
  previousState = currentState;
  currentState = state;
  redrawScreen = true;
  redrawStatus = true;
}

// Radio functions

static void ToggleAFBit(bool on) {
  uint16_t reg = BK4819_ReadRegister(BK4819_REG_47);
  reg &= ~(1 << 8);
  if (on)
    reg |= on << 8;
  BK4819_WriteRegister(BK4819_REG_47, reg);
}

static void BackupRegisters() {
  R30 = BK4819_ReadRegister(BK4819_REG_30);
  R37 = BK4819_ReadRegister(BK4819_REG_37);
  R3D = BK4819_ReadRegister(BK4819_REG_3D);
  R43 = BK4819_ReadRegister(BK4819_REG_43);
  R47 = BK4819_ReadRegister(BK4819_REG_47);
  R48 = BK4819_ReadRegister(BK4819_REG_48);
  R7E = BK4819_ReadRegister(BK4819_REG_7E);
  R02 = BK4819_ReadRegister(BK4819_REG_02);
  R3F = BK4819_ReadRegister(BK4819_REG_3F);
}

static void RestoreRegisters() {
  BK4819_WriteRegister(BK4819_REG_30, R30);
  BK4819_WriteRegister(BK4819_REG_37, R37);
  BK4819_WriteRegister(BK4819_REG_3D, R3D);
  BK4819_WriteRegister(BK4819_REG_43, R43);
  BK4819_WriteRegister(BK4819_REG_47, R47);
  BK4819_WriteRegister(BK4819_REG_48, R48);
  BK4819_WriteRegister(BK4819_REG_7E, R7E);
  BK4819_WriteRegister(BK4819_REG_02, R02);
  BK4819_WriteRegister(BK4819_REG_3F, R3F);

}

static void ToggleAFDAC(bool on) {
  uint32_t Reg = BK4819_ReadRegister(BK4819_REG_30);
  Reg &= ~(1 << 9);
  if (on)
    Reg |= (1 << 9);
  BK4819_WriteRegister(BK4819_REG_30, Reg);
}

static void SetF(uint32_t f) {
  fMeasure = f;
  BK4819_SetFrequency(fMeasure + gEeprom.RX_OFFSET);
  BK4819_PickRXFilterPathBasedOnFrequency(fMeasure);
  uint16_t reg = BK4819_ReadRegister(BK4819_REG_30);
  BK4819_WriteRegister(BK4819_REG_30, 0);
  BK4819_WriteRegister(BK4819_REG_30, reg);
}

// Spectrum related

bool IsPeakOverLevel() { return peak.rssi >= settings.rssiTriggerLevel; }
bool IsPeakOverLevelH() { return peak.rssi >= settings.rssiTriggerLevelH; }

static void ResetInterrupts()
{
  // disable interupts
  BK4819_WriteRegister(BK4819_REG_3F, 0);
  // reset the interrupt
  BK4819_WriteRegister(BK4819_REG_02, 0);
}

static void ResetPeak() {
  peak.t = 0;
  peak.rssi = 0;
}


// scan step in 0.01khz
uint32_t GetScanStep() { return scanStepValues[settings.scanStepIndex]; }

uint16_t GetStepsCount() 
{ 
  if (appMode==CHANNEL_MODE)
  {
    return scanChannelsCount;
  }
  if(appMode==SCAN_RANGE_MODE) {
    return (2+(gScanRangeStop - gScanRangeStart) / GetScanStep()); //Robby69
  }
  if (appMode==SCAN_BAND_MODE) {return 2+(gScanRangeStop - gScanRangeStart) / scanInfo.scanStep;}
  
  return 128 >> settings.stepsCount;
}

uint32_t GetBW() { return GetStepsCount() * GetScanStep(); }

uint32_t GetFStart() {
	if(appMode==SCAN_RANGE_MODE) return currentFreq - GetScanStep();//robby69
	return (currentFreq - (GetBW() >> 1));}

uint32_t GetFEnd() {
	if(appMode==SCAN_RANGE_MODE) return currentFreq + GetBW()+ GetScanStep();//robby69
	return (currentFreq + (GetBW() >> 1));} //robby69

static void TuneToPeak() {
  scanInfo.f = peak.f;
  scanInfo.rssi = peak.rssi;
  scanInfo.i = peak.i;
  SetF(scanInfo.f);
}
static void DeInitSpectrum() {
  SetF(initialFreq);
  RestoreRegisters();
  gVfoConfigureMode = VFO_CONFIGURE;
  isInitialized = false;
  gScanRangeStart = 0;
  uint8_t Last_state = 0; //Spectrum Not Active
  EEPROM_WriteBuffer(0x1D00, &Last_state, 1);
  
}

static void ExitAndCopyToVfo() {
  RestoreRegisters();
  if (appMode==CHANNEL_MODE)
  // channel mode
  {
   // gEeprom.MrChannel[gEeprom.TX_VFO]     = scanChannel[peak.i-1];
   // gEeprom.ScreenChannel[gEeprom.TX_VFO] = scanChannel[peak.i-1];

    gRequestSaveVFO   = true;
    gVfoConfigureMode = VFO_CONFIGURE_RELOAD;
  }
  else
  // frequency mode
  {
    gTxVfo->STEP_SETTING = FREQUENCY_GetStepIdxFromStepFrequency(GetScanStep());
    gTxVfo->Modulation = settings.modulationType;
    gTxVfo->CHANNEL_BANDWIDTH = settings.listenBw;
    SETTINGS_SetVfoFrequency(freqHistory[indexFd]); //Robby69
    gRequestSaveChannel = 1;
  }
 
  // Additional delay to debounce keys
SYSTEM_DelayMs(200);
isInitialized = false;
DeInitSpectrum();
}

uint8_t GetScanStepFromStepFrequency(uint16_t stepFrequency) 
{
	for(uint8_t i = 0; i < ARRAY_SIZE(scanStepValues); i++)
		if(scanStepValues[i] == stepFrequency)
			return i;
	return S_STEP_25_0kHz;
}

uint8_t GetBWRegValueForScan() {
  return scanStepBWRegValues[settings.scanStepIndex];
}
  
uint16_t GetRssi() {
  uint16_t rssi;
  // testing resolution to sticky squelch issue
  while ((BK4819_ReadRegister(0x63) & 0b11111111) >= 255) {
    SYSTICK_DelayUs(500); // was 100 , some k5 bug when starting spectrum
  }
  rssi = BK4819_GetRSSI();
  //if ((appMode==CHANNEL_MODE) && (FREQUENCY_GetBand(fMeasure) > BAND4_174MHz))
  if (fMeasure > 3000000)
    {
      // Increase perceived RSSI for UHF bands to imitate radio squelch
      rssi+=UHF_NOISE_FLOOR;
    }
  rssi+=gainOffset[CurrentScanIndex()];
  return rssi;
}

static void ToggleAudio(bool on) {
  if (on == audioState) {
    return;
  }
  audioState = on;
  if (on) {
    AUDIO_AudioPathOn();
  } else {
    AUDIO_AudioPathOff();
  }
}

void FillfreqHistory(bool count){ //Robby69
  uint8_t i;
  bool found = 0;
  for (i=1;i < FMaxNumb;i++) 
    {if (freqHistory[i] == peak.f) 
      {found=1;
        if (count)freqCount[i]++;
      }
    }
  if (!found) {
    freqHistory[indexFs] = peak.f;
    freqCount[indexFs]=1;
    indexFd = indexFs;
    indexFs++;}
  if (indexFs > FMaxNumb) indexFs = 1;
  }

static void ToggleRX(bool on) {
  isListening = on;
  BACKLIGHT_TurnOn();

  // automatically switch modulation & bw if known channel
  if (on && isKnownChannel) {
    settings.modulationType = channelModulation;
    //  settings.listenBw = channelBandwidth;
    memmove(rxChannelName, channelName, sizeof(rxChannelName));
    RADIO_SetModulation(settings.modulationType);
    BK4819_InitAGC(gEeprom.RX_AGC, settings.modulationType);
    redrawScreen = true;
  }

  // turn on green led only if screen brightness is over 7
  if(gEeprom.BACKLIGHT_MAX > 7)
    BK4819_ToggleGpioOut(BK4819_GPIO6_PIN2_GREEN, on);

  ToggleAudio(on);
  ToggleAFDAC(on);
  ToggleAFBit(on);

  if (on)
  { FillfreqHistory(true);
    listenT = SQUELCH_OFF_DELAY;
    BK4819_SetFilterBandwidth(settings.listenBw, false);

    //gTailFound=false;

    // turn on CSS tail found interrupt
    BK4819_WriteRegister(BK4819_REG_3F, BK4819_REG_02_CxCSS_TAIL);

    // keep backlight and bold channel name display on as long as we are receiving
    gBacklightCountdown = 0;
    rxChannelDisplayCountdown = 0;
  } else
  {
    if(appMode!=CHANNEL_MODE)
      BK4819_WriteRegister(0x43, GetBWRegValueForScan());

    // keep displaying the received channel for a second or so
    rxChannelDisplayCountdown = 0; //4 Robby69
  }
}

// Scan info

static void ResetScanStats() {
  scanInfo.rssi = 0;
  scanInfo.rssiMax = 0;
  scanInfo.iPeak = 0;
  scanInfo.fPeak = 0;
}

static bool InitScan() {
    ResetScanStats();
    scanInfo.i = 0;
    
    bool scanInitializedSuccessfully = false;

    if (appMode == SCAN_BAND_MODE) {
        uint8_t checkedBandCount = 0; // Licznik sprawdzonych pasm, aby uniknąć nieskończonej pętli
        while (checkedBandCount < 15) { // Sprawdź wszystkie 15 pasm co najwyżej raz
            if (settings.bandEnabled[nextBandToScanIndex]) {
                bl = nextBandToScanIndex; // Użyj bieżącego jako aktywnego
                scanInfo.f = BParams[bl].Startfrequency;
                scanInfo.scanStep = scanStepValues[BParams[bl].scanStep];
                settings.scanStepIndex = BParams[bl].scanStep; // Aktualizuj globalny, jeśli potrzebne
                gScanRangeStart = BParams[bl].Startfrequency;
                gScanRangeStop = BParams[bl].Stopfrequency;
                scanInfo.measurementsCount = GetStepsCount();
                settings.modulationType = BParams[bl].modulationType;
                RADIO_SetModulation(BParams[bl].modulationType);      // Ustaw modulację dla pasma
                BK4819_InitAGC(gEeprom.RX_AGC, settings.modulationType);
                nextBandToScanIndex = (nextBandToScanIndex + 1) % 15; // Przygotuj indeks na następne wywołanie
                scanInitializedSuccessfully = true;
                redrawStatus = true; // Te flagi mogą być potrzebne tutaj
                redrawScreen = true;
                settings.dbMax = BParams[bl].dbMax;
                AutoTriggerLevelbands();
                break; // Znaleziono aktywne pasmo, przerwij pętlę while
            }
            nextBandToScanIndex = (nextBandToScanIndex + 1) % 15; // Przejdź do następnego pasma
            checkedBandCount++;
        }
    } else {
        // Logika dla innych trybów
        scanInfo.f = GetFStart();
        scanInfo.scanStep = GetScanStep();
        scanInfo.measurementsCount = GetStepsCount();
        scanInitializedSuccessfully = true;
    }
	if(appMode==CHANNEL_MODE)
    scanInfo.measurementsCount++;
    return scanInitializedSuccessfully;
}

static void AutoTriggerLevel() {
  if (settings.rssiTriggerLevel == RSSI_MAX_VALUE) {
  settings.rssiTriggerLevel = clamp(scanInfo.rssiMax +10, 0, RSSI_MAX_VALUE); //Robby69 +8
	settings.rssiTriggerLevelH = settings.rssiTriggerLevel; //Robby69
  }
}

static uint16_t AutoTriggerLevelbands(void) {
  static uint16_t rssiAnalyse;
  uint16_t topRssi[3] = {0}
  AnalyseStep = (gScanRangeStop - gScanRangeStart)/10;
  for (int i = 0; i < 10; ++i) {
    while ((BK4819_ReadRegister(0x63) & 0b11111111) >= 255) {
      SYSTICK_DelayUs(500); // was 100 , some k5 bug when starting spectrum
    }
    FreqAnalyse = gScanRangeStart + (AnalyseStep * i);
    SetF(FreqAnalyse);
    rssiAnalyse = BK4819_GetRSSI();

    if (rssiAnalyse > topRssi[0]) {
      topRssi[2] = topRssi[1];
      topRssi[1] = topRssi[0];
      topRssi[0] = rssiAnalyse;
      } else 
      if (rssiAnalyse > topRssi[1]) 
        {topRssi[1] = rssiAnalyse;
        }
     
    }

  settings.rssiTriggerLevel = clamp(topRssi[2] +10, 0, RSSI_MAX_VALUE);
	settings.rssiTriggerLevelH = settings.rssiTriggerLevel;
}

// resets modifiers like blacklist, attenuation, normalization
static void ResetModifiers() {
  //squelch_level_mod=10;
  	sprintf(StringC, "");
  for (int i = 0; i < 128; ++i) {
    if (rssiHistory[i] == RSSI_MAX_VALUE)
      rssiHistory[i] = 0;
  }
  memset(blacklistFreqs, 0, sizeof(blacklistFreqs));
  blacklistFreqsIdx = 0;

  if(appMode==CHANNEL_MODE){
      LoadValidMemoryChannels();
  }
  ToggleNormalizeRssi(false);
  memset(attenuationOffset, 0, sizeof(attenuationOffset));
  isBlacklistApplied = false;
  //AutoTriggerLevel();
  RelaunchScan();
  
}

static void RelaunchScan() {
  InitScan();
  ResetPeak();
  ToggleRX(false);
  //settings.rssiTriggerLevel = RSSI_MAX_VALUE; //Robby69
  preventKeypress = true;
  scanInfo.rssiMin = RSSI_MAX_VALUE;
}

static void UpdateScanInfo() {
  if (scanInfo.rssi > scanInfo.rssiMax) {
    scanInfo.rssiMax = scanInfo.rssi;
    scanInfo.fPeak = scanInfo.f;
    scanInfo.iPeak = scanInfo.i;
  }
  // add attenuation offset to prevent noise floor lowering when attenuated rx is over
  // essentially we measure non-attenuated lowest rssi
  if (scanInfo.rssi+attenuationOffset[CurrentScanIndex()] < scanInfo.rssiMin) {
    scanInfo.rssiMin = scanInfo.rssi;
    settings.dbMin = Rssi2DBm(scanInfo.rssiMin);
    redrawStatus = true;
  }
}

static void UpdatePeakInfoForce() {
  peak.t = 0;
  peak.rssi = scanInfo.rssiMax;
  peak.f = scanInfo.fPeak;
  peak.i = scanInfo.iPeak;
  LookupChannelInfo();
  AutoTriggerLevel(); //Robby69
}

static void UpdatePeakInfo() {
  if (peak.f == 0 || peak.t >= 1024 || peak.rssi < scanInfo.rssiMax)
    UpdatePeakInfoForce();
}

static void Measure() 
{ 
  uint16_t rssi = scanInfo.rssi = GetRssi();
    if (rssi > settings.rssiTriggerLevelH) FillfreqHistory(false);
    if(scanInfo.measurementsCount > 128) {
      uint8_t idx = CurrentScanIndex();
      if(rssiHistory[idx] < rssi || isListening)
        rssiHistory[idx] = rssi;
      rssiHistory[(idx+1)%128] = 0;
      return;
    }
  rssiHistory[scanInfo.i] = rssi;
}

// Update things by keypress

static uint16_t dbm2rssi(int dBm)
{
  return (dBm + 160)*2;
}

static void ClampRssiTriggerLevel()
{
  settings.rssiTriggerLevel = clamp(settings.rssiTriggerLevel, dbm2rssi(settings.dbMin), dbm2rssi(settings.dbMax));
  settings.rssiTriggerLevelH = clamp(settings.rssiTriggerLevelH, dbm2rssi(settings.dbMin), dbm2rssi(settings.dbMax));
}

static void UpdateRssiTriggerLevel(bool inc) {
	if (inc){	
		if (SquelchBarKeyMode == 0) {settings.rssiTriggerLevelH +=5;}
		if (SquelchBarKeyMode == 1) {settings.rssiTriggerLevel +=5;}
		if (SquelchBarKeyMode == 2) {
			settings.rssiTriggerLevelH +=5;
			settings.rssiTriggerLevel +=5;}}
	else {
		if (SquelchBarKeyMode == 0) {settings.rssiTriggerLevelH -=5;}
		if (SquelchBarKeyMode == 1) {settings.rssiTriggerLevel -=5;}
		if (SquelchBarKeyMode == 2) {
			settings.rssiTriggerLevelH -=5;
		settings.rssiTriggerLevel  -=5;}}
  ClampRssiTriggerLevel();
}

static void UpdateDBMax(bool inc) {
  /*if (SquelchBarKeyMode ==0)
    //{
    if (inc && settings.dbMin <= 0) {settings.dbMin += 5;}
      else if (!inc && settings.dbMax > settings.dbMin) {settings.dbMin -= 5;}
        else {return;}
    }
  else*/
    if (inc && settings.dbMax <= 100) {settings.dbMax += 5;} 
      else if (!inc && settings.dbMax > settings.dbMin) {settings.dbMax -= 5;} 
           else {return;}
  settings.dbMax = ((settings.dbMax + (settings.dbMax >= 0 ? 2 : -2)) / 5) * 5;
  ClampRssiTriggerLevel();
  redrawStatus = true;
  redrawScreen = true;
  SYSTEM_DelayMs(20);
}

static void UpdateScanStep(bool inc) {
  if (inc && settings.scanStepIndex < S_STEP_500_0kHz) {
    settings.scanStepIndex++;
  } else if (!inc && settings.scanStepIndex > 0) {
    settings.scanStepIndex--;
  } else {
    return;
  }
  settings.frequencyChangeStep = GetBW() >> 1;
  ResetModifiers();
  redrawScreen = true;
}

static void UpdateCurrentFreq(bool inc) {
  if (inc && currentFreq < F_MAX) {
    currentFreq += settings.frequencyChangeStep;
  } else if (!inc && currentFreq > RX_freq_min() && currentFreq > settings.frequencyChangeStep) {
    currentFreq -= settings.frequencyChangeStep;
  } else {
    return;
  }
  ResetModifiers();
  redrawScreen = true;
}

static void UpdateCurrentFreqStill(bool inc) {
  uint8_t offset = modulationTypeTuneSteps[settings.modulationType];
  uint32_t f = fMeasure;
  if (inc && f < F_MAX) {
    f += offset;
  } else if (!inc && f > RX_freq_min()) {
    f -= offset;
  }
  SetF(f);
  redrawScreen = true;
}

static void AutoAdjustFreqChangeStep() {
  settings.frequencyChangeStep = GetBW() >> 1;
}

static void ToggleModulation() {
  if (settings.modulationType < MODULATION_UKNOWN - 1) {
    settings.modulationType++;
  } else {
    settings.modulationType = MODULATION_FM;
  }
  RADIO_SetModulation(settings.modulationType);
  BK4819_InitAGC(gEeprom.RX_AGC, settings.modulationType);
  redrawScreen = true;
}

static void ToggleListeningBW() {
  settings.listenBw = ACTION_NextBandwidth(settings.listenBw, false);
  redrawScreen = true;
}

static void ToggleBacklight() {
  settings.backlightAlwaysOn = !settings.backlightAlwaysOn;
  if (settings.backlightAlwaysOn) {
    BACKLIGHT_TurnOn();
  } else {
    if (!isListening)
      BACKLIGHT_TurnOff();
  }
}

static void ToggleStepsCount() {
  if (settings.stepsCount == STEPS_128) {
    settings.stepsCount = STEPS_16;
  } else {
    settings.stepsCount--;
  }
  AutoAdjustFreqChangeStep();
  ResetModifiers();
  redrawScreen = true;
}

static void ResetFreqInput() {
  tempFreq = 0;
  for (int i = 0; i < 10; ++i) {
    freqInputString[i] = '-';
  }
}

static void FreqInput() {
  freqInputIndex = 0;
  freqInputDotIndex = 0;
  ResetFreqInput();
  SetState(FREQ_INPUT);
}

static void UpdateFreqInput(KEY_Code_t key) {
  if (key != KEY_EXIT && freqInputIndex >= 10) {
    return;
  }
  if (key == KEY_STAR) {
    if (freqInputIndex == 0 || freqInputDotIndex) {
      return;
    }
    freqInputDotIndex = freqInputIndex;
  }
  if (key == KEY_EXIT) {
    freqInputIndex--;
    if(freqInputDotIndex==freqInputIndex)
      freqInputDotIndex = 0;    
  } else {
    freqInputArr[freqInputIndex++] = key;
  }

  ResetFreqInput();

  uint8_t dotIndex =
      freqInputDotIndex == 0 ? freqInputIndex : freqInputDotIndex;

  KEY_Code_t digitKey;
  for (int i = 0; i < 10; ++i) {
    if (i < freqInputIndex) {
      digitKey = freqInputArr[i];
      freqInputString[i] = digitKey <= KEY_9 ? '0' + digitKey-KEY_0 : '.';
    } else {
      freqInputString[i] = '-';
    }
  }

  uint32_t base = 100000; // 1MHz in BK units
  for (int i = dotIndex - 1; i >= 0; --i) {
    tempFreq += (freqInputArr[i]-KEY_0) * base;
    base *= 10;
  }

  base = 10000; // 0.1MHz in BK units
  if (dotIndex < freqInputIndex) {
    for (int i = dotIndex + 1; i < freqInputIndex; ++i) {
      tempFreq += (freqInputArr[i]-KEY_0) * base;
      base /= 10;
    }
  }
  redrawScreen = true;
}

static void Blacklist() {

  blacklistFreqs[blacklistFreqsIdx++ % ARRAY_SIZE(blacklistFreqs)] = peak.i;
  rssiHistory[CurrentScanIndex()] = RSSI_MAX_VALUE;

  rssiHistory[peak.i] = RSSI_MAX_VALUE;
  isBlacklistApplied = true;
  ResetPeak();
  ToggleRX(false);
  ResetScanStats();
}


static uint8_t CurrentScanIndex()
{
  if(scanInfo.measurementsCount > 128) {
    uint8_t i = (uint32_t)ARRAY_SIZE(rssiHistory) * 1000 / scanInfo.measurementsCount * scanInfo.i / 1000;
    return i;
  }
  else
  {
    return scanInfo.i;
  }
  
}

static bool IsBlacklisted(uint16_t idx)
{

  for(uint8_t i = 0; i < ARRAY_SIZE(blacklistFreqs); i++)
    if(blacklistFreqs[i] == idx)
      return true;
  return false;
}


// Draw things

// applied x2 to prevent initial rounding
uint8_t Rssi2PX(uint16_t rssi, uint8_t pxMin, uint8_t pxMax) {
  const int DB_MIN = settings.dbMin << 1;
  const int DB_MAX = settings.dbMax << 1;
  const int DB_RANGE = DB_MAX - DB_MIN;

  const uint8_t PX_RANGE = pxMax - pxMin;

  int dbm = clamp(rssi - (160 << 1), DB_MIN, DB_MAX);

  return ((dbm - DB_MIN) * PX_RANGE + DB_RANGE / 2) / DB_RANGE + pxMin;
}

uint8_t Rssi2Y(uint16_t rssi) {
  return DrawingEndY - Rssi2PX(rssi, 0, DrawingEndY);
}

static void DrawSpectrum()
    {//Robby69 V4.16
      uint16_t steps = GetStepsCount();
      // max bars at 128 to correctly draw larger numbers of samples
      uint8_t bars = (steps > 128) ? 128 : steps;
      // shift to center bar on freq marker
      uint8_t shift_graph = 64 / steps + 1;

        uint8_t ox = 0;
        for (uint8_t i = 0; i < 127; ++i)
        {
            uint16_t rssi = rssiHistory[1+ (i >> settings.stepsCount)];//Robby69 first bar display
            if (rssi != RSSI_MAX_VALUE)
            {
                // stretch bars to fill the screen width
                uint8_t x = i * 128 / bars + shift_graph;
                for (uint8_t xx = ox; xx < x; xx++)
                {
                    DrawVLine(Rssi2Y(rssi), DrawingEndY, xx, true);
                }
                ox = x;
            }
        }
    }


static void DrawStatus() {
  sprintf(String, "%d/%d", settings.dbMin, settings.dbMax);
  GUI_DisplaySmallest(String, 0, 1, true, true);
  static char list[2];
  // display scanlists
  if(appMode==CHANNEL_MODE || appMode==SCAN_BAND_MODE) {
    if(appMode==CHANNEL_MODE) sprintf(list,"SL");
    if(appMode==SCAN_BAND_MODE) sprintf(list,"BD");
    switch(waitingForScanListNumber) {
      case 2:
        sprintf(String, "%s ===============",list);
        break;      
      case 1:
        sprintf(String, "%s _______________",list);
        break;
      default:
        sprintf(String, "%s                ",list);
        break;
    }


    char Number[2];
    bool slEnabled = false;
    for (int i = 1; i <= 15; i++) {
      if (appMode == SCAN_BAND_MODE){
        if (settings.bandEnabled[i-1]) {
          slEnabled = true;
          sprintf(Number, "%d", i % 10);
          String[i+2] = Number[0];
        }}
      else
        if (settings.scanListEnabled[i-1]) {
        slEnabled = true;
        sprintf(Number, "%d", i % 10);
        String[i+2] = Number[0];
      }
    }
    if (slEnabled || waitingForScanListNumber)
      GUI_DisplaySmallest(String, 42, 1, true, true);
  }

  BOARD_ADC_GetBatteryInfo(&gBatteryVoltages[gBatteryCheckCounter++ % 4]);

  uint16_t voltage = (gBatteryVoltages[0] + gBatteryVoltages[1] + gBatteryVoltages[2] +
             gBatteryVoltages[3]) /
            4 * 760 / gBatteryCalibration[3];

  unsigned perc = BATTERY_VoltsToPercent(voltage);
  gStatusLine[116] = 0b00011100;
  gStatusLine[117] = 0b00111110;
  for (int i = 118; i <= 126; i++) {
    gStatusLine[i] = 0b00100010;
  }
  
  for (unsigned i = 127; i >= 118; i--) {
    if (127 - i <= (perc+5)*9/100) {
      gStatusLine[i] = 0b00111110;
    }
  }
}

static void DrawF(uint32_t f) {
	uint8_t Code;
	if (f > 0){

    if(GetScanStep() ==  833) {
          uint32_t base = f/2500*2500;
          int chno = (f - base) / 700;    // convert entered aviation 8.33Khz channel number scheme to actual frequency. 
          f = base + (chno * 833) + (chno == 3);
	      }
    
	sprintf(String, "%u.%05u", f / 100000, f % 100000);
	UI_PrintStringSmallBold(String, 1, 127, 0);}
	f= freqHistory[indexFd];				 
	int channelFd = BOARD_gMR_fetchChannel(f);
    isKnownChannel = channelFd == -1 ? false : true;



	if (f > 0){
    //Robby69 show history 
    if(isKnownChannel) {sprintf(String, "%u:%s:%u", indexFd, gMR_ChannelFrequencyAttributes[channelFd].Name, freqCount[indexFd]);}
	    else 	
        {
			 if(GetScanStep() ==  833) {
            uint32_t base = f/2500*2500;
            int chno = (f - base) / 700;    // convert entered aviation 8.33Khz channel number scheme to actual frequency. 
            f = base + (chno * 833) + (chno == 3);
            }
        sprintf(String, "%u:%u.%05u:%u",indexFd, f / 100000, f % 100000,freqCount[indexFd]);
          }
    if (ShowHistory)
    UI_PrintStringSmall(String, 1, 1, 2); 
    }
  
//Robby show CTCSS or DCS
	if (refresh == 0){
		BK4819_CssScanResult_t scanResult = BK4819_GetCxCSSScanResult(&cdcssFreq, &ctcssFreq);
		sprintf(StringC, "");
		refresh = 1;
		if (scanResult == BK4819_CSS_RESULT_CDCSS){
			Code = DCS_GetCdcssCode(cdcssFreq);
			refresh = 50;
			if (Code != 0xFF) {sprintf(StringC, " D%03oN", DCS_Options[Code]);}}
	
		if (scanResult == BK4819_CSS_RESULT_CTCSS) {
			Code = DCS_GetCtcssCode(ctcssFreq);
			refresh = 50;
			sprintf(StringC, "%u.%uHz",CTCSS_Options[Code] / 10, CTCSS_Options[Code] % 10);}}
			
	GUI_DisplaySmallest(StringC, 102, 14, false, true);
  refresh--;
    if(appMode == SCAN_BAND_MODE)
        {sprintf(String, "%u:%s",bl+1,BParams[bl].BandName);
        UI_PrintStringSmallBold(String, 1, 127, 1);}
      else 	if (isKnownChannel && isListening) {
		    sprintf(String, "%s", channelName);
		    UI_PrintStringSmallBold(String, 1, 127, 1);}

  sprintf(String, "%3s", gModulationStr[settings.modulationType]);
  GUI_DisplaySmallest(String, 116, 1, false, true);
  sprintf(String, "%s", bwNames[settings.listenBw]);
  GUI_DisplaySmallest(String, 108, 7, false, true);
}

void LookupChannelInfo() {
    if (lastPeakFrequency == peak.f) 
      return;
    
    lastPeakFrequency = peak.f;
    
    channel = BOARD_gMR_fetchChannel(peak.f);

    isKnownChannel = channel == -1 ? false : true;

    if (isKnownChannel){
      memmove(channelName, gMR_ChannelFrequencyAttributes[channel].Name, sizeof(channelName));
      LookupChannelModulation();
    }

    redrawStatus = true;
  }

void LookupChannelModulation() {
	  uint16_t base;
		base = channel * 16;

		uint8_t tmp;
		uint8_t data[8];

		EEPROM_ReadBuffer(base + 8, data, sizeof(data));

		tmp = data[3] >> 4;
		if (tmp >= MODULATION_UKNOWN)
			tmp = MODULATION_FM;
		channelModulation = tmp;

		if (data[4] == 0xFF)
		{
			channelBandwidth = BK4819_FILTER_BW_WIDE;
		}
		else
		{
			const uint8_t d4 = data[4];
			channelBandwidth = !!((d4 >> 1) & 1u);
			if(channelBandwidth != BK4819_FILTER_BW_WIDE)
				channelBandwidth = ((d4 >> 5) & 3u) + 1;
		}	

}


static void DrawNums() {

  if (currentState == SPECTRUM) {
    if(isNormalizationApplied){
      sprintf(String, "N(%ux)", GetStepsCount());
    }
    else {
      sprintf(String, "%ux", GetStepsCount());
    }
    GUI_DisplaySmallest(String, 0, 1, false, true);

    if (appMode==CHANNEL_MODE)
    {
      sprintf(String, "M%i", channel+1);
      GUI_DisplaySmallest(String, 0, 7, false, true);
    }
    else
    {
      sprintf(String, "%u.%02uk", scanInfo.scanStep / 100, scanInfo.scanStep % 100);
      GUI_DisplaySmallest(String, 0, 7, false, true);
    }

  }

  if (appMode==CHANNEL_MODE) 
  {
    sprintf(String, "M:%d", scanChannel[0]+1);
    GUI_DisplaySmallest(String, 0, Bottom_print, false, true);

    sprintf(String, "M:%d", scanChannel[GetStepsCount()-1]+1);
    GUI_DisplaySmallest(String, 108, Bottom_print, false, true);
  }
  if(appMode==FREQUENCY_MODE){ 
    sprintf(String, "%u.%05u", GetFStart() / 100000, GetFStart() % 100000);
    GUI_DisplaySmallest(String, 0, Bottom_print, false, true);

    sprintf(String, "%u.%05u", GetFEnd() / 100000, GetFEnd() % 100000);
    GUI_DisplaySmallest(String, 90, Bottom_print, false, true);
  }

  if(appMode==SCAN_RANGE_MODE || appMode==SCAN_BAND_MODE){
    sprintf(String, "%u.%05u", gScanRangeStart / 100000, gScanRangeStart % 100000); //Robby69 was %u.%05u
    GUI_DisplaySmallest(String, 0, Bottom_print, false, true);
 
    sprintf(String, "%u.%05u", gScanRangeStop / 100000, gScanRangeStop % 100000); //Robby69 was %u.%05u
    GUI_DisplaySmallest(String, 90, Bottom_print, false, true);
  }

  if(isBlacklistApplied){
    sprintf(String, "BL");
    GUI_DisplaySmallest(String, 67, Bottom_print, false, true);
  }
}

static void DrawRssiTriggerLevel() {
  if (settings.rssiTriggerLevel == RSSI_MAX_VALUE || monitorMode)
    return;
  uint8_t y = Rssi2Y(settings.rssiTriggerLevel);
  for (uint8_t x = 0; x < 128; x += 2) {
    PutPixel(x, y, true);
  }
  y = Rssi2Y(settings.rssiTriggerLevelH);
  for (uint8_t x = 0; x < 128; x += 6) {
    PutPixel(x, y, true);
  }
}

static void OnKeyDown(uint8_t key) {
  if (!isListening)
    BACKLIGHT_TurnOn();

  if (waitingForScanListNumber > 0) {
    int scanListNumber = 0;

    if (key == KEY_0)
      scanListNumber = 10;
    else if (key <= KEY_9)
      scanListNumber = key;
    else if (key == KEY_STAR)
      scanListNumber = 11;
    else if (key == KEY_F)
      scanListNumber = 12;
    else if (key >= KEY_MENU && key <= KEY_DOWN)
      scanListNumber = key + 3;
    
    if (scanListNumber > 0)
      ToggleScanList(scanListNumber, waitingForScanListNumber - 1);

    waitingForScanListNumber = 0;
    redrawStatus = true;

    return;
  }

  switch (key) {
  case KEY_3:
    UpdateDBMax(true);
    break;
  case KEY_9:
    UpdateDBMax(false);
    break;
  case KEY_1:
    if(appMode!=CHANNEL_MODE) {UpdateScanStep(true);}
    break;
  case KEY_7:
    if(appMode!=CHANNEL_MODE) {UpdateScanStep(false);}
    break;
  case KEY_2:
    ToggleNormalizeRssi(!isNormalizationApplied);
    break;
  case KEY_8:
	memset(freqHistory, 0, sizeof(freqHistory)); //Reset History table
  memset(freqCount, 0, sizeof(freqCount));
	indexFd = 1;
  indexFs = 1;
  ShowHistory = false;
  ToggleBacklight();
  break;
  
  case KEY_UP:
  ShowHistory = true;
  indexFd++;
	if(freqHistory[indexFd]==0)indexFd--;
	if (indexFd > FMaxNumb) indexFd = 1;	  
	
    if(appMode==FREQUENCY_MODE)
    {UpdateCurrentFreq(true);}
    break;
  case KEY_DOWN:
    ShowHistory = true;
    indexFd--;
    if (indexFd < 1) indexFd = 1;
    if(appMode==FREQUENCY_MODE)
    {UpdateCurrentFreq(false);}
    break;
  
  case KEY_SIDE1:
    Blacklist();
    break;
  
  case KEY_STAR:
	  UpdateRssiTriggerLevel(true);
    break;
  
  case KEY_F:
    UpdateRssiTriggerLevel(false);
    break;
  
  case KEY_4:
    if(appMode==CHANNEL_MODE || appMode==SCAN_BAND_MODE)
    {
      waitingForScanListNumber = 1;
      redrawStatus = true;
    }
    else if (appMode!=SCAN_RANGE_MODE){ToggleStepsCount();}
    break;

  case KEY_5:

    if(appMode==FREQUENCY_MODE) FreqInput();
    
    if (appMode==CHANNEL_MODE || appMode==SCAN_BAND_MODE) {
      waitingForScanListNumber = 2;
      redrawStatus = true;
    }
    break;
  case KEY_0:
    ToggleModulation();
    break;
  case KEY_6:
    ToggleListeningBW();
    break;
  
  case KEY_SIDE2: //Robby69
  SquelchBarKeyMode += 1; //Robby69
	if (SquelchBarKeyMode > 2) SquelchBarKeyMode =0;
    break;
  case KEY_PTT:
    ExitAndCopyToVfo();
    break;
  case KEY_MENU:
    SaveSettings(); //Robby69
    //SetState(STILL);//Show radio settings in Spectrum
    //TuneToPeak();
    break;
  case KEY_EXIT:
    if (menuState) {
      menuState = 0;
      break;
    }
    DeInitSpectrum();
      break;
    default:
      break;
  }
}

static void OnKeyDownFreqInput(uint8_t key) {
  switch (key) {
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
  case KEY_STAR:
    UpdateFreqInput(key);
    break;
  case KEY_EXIT:
    if (freqInputIndex == 0) {
      SetState(previousState);
      break;
    }
    UpdateFreqInput(key);
    break;
  case KEY_MENU:
    if (tempFreq < RX_freq_min() || tempFreq > F_MAX) {
      break;
    }
    SetState(previousState);
    currentFreq = tempFreq;
    if (currentState == SPECTRUM) {
      ResetModifiers();
    } else {
      SetF(currentFreq);
    }
    break;
  default:
    break;
  }
}

void OnKeyDownStill(KEY_Code_t key) {
  switch (key) {
  case KEY_3:
    UpdateDBMax(true);
    break;
  case KEY_9:
    UpdateDBMax(false);
    break;
  case KEY_UP:
    if (menuState) {
      SetRegMenuValue(menuState, true);
      break;
    }
    UpdateCurrentFreqStill(true);
    break;
  case KEY_DOWN:
    if (menuState) {
      SetRegMenuValue(menuState, false);
      break;
    }
    UpdateCurrentFreqStill(false);
    break;
  case KEY_STAR:
    UpdateRssiTriggerLevel(true);
    break;
  case KEY_F:
    UpdateRssiTriggerLevel(false);
    break;
  case KEY_5:
    FreqInput();
    break;
  case KEY_0:
    ToggleModulation();
    break;
  case KEY_6:
    ToggleListeningBW();
    break;
  case KEY_SIDE1:
    monitorMode = !monitorMode;
    break;
  case KEY_SIDE2:
    ToggleBacklight();
    break;
  case KEY_PTT:
    ExitAndCopyToVfo();
    break;
  case KEY_MENU:
    if (menuState == ARRAY_SIZE(registerSpecs) - 1) {
      menuState = 1;
    } else {
      menuState++;
    }
    redrawScreen = true;
    break;
  case KEY_EXIT:
    if (!menuState) {
      SetState(SPECTRUM);
      monitorMode = false;
      RelaunchScan();
      break;
    }
    menuState = 0;
    break;
  default:
    break;
  }
}

static void RenderFreqInput() {
  UI_PrintString(freqInputString, 2, 127, 0, 8);
}

static void RenderStatus() {
  memset(gStatusLine, 0, sizeof(gStatusLine));
  DrawStatus();
  ST7565_BlitStatusLine();
}


static void RenderSpectrum() {
  DrawNums();
  DrawSpectrum();
  DrawRssiTriggerLevel();
  DrawF(peak.f); 
}



static void RenderStill() {
  DrawF(fMeasure);
  
  const uint8_t METER_PAD_LEFT = 3;

  for (int i = 0; i < 121; i++) {
    if (i % 10 == 0) {
      gFrameBuffer[2][i + METER_PAD_LEFT] = 0b01110000;
    } else if (i % 5 == 0) {
      gFrameBuffer[2][i + METER_PAD_LEFT] = 0b00110000;
    } else {
      gFrameBuffer[2][i + METER_PAD_LEFT] = 0b00010000;
    }
  }

  uint8_t x = Rssi2PX(scanInfo.rssi, 0, 121);
  for (int i = 0; i < x; ++i) {
    if (i % 5) {
      gFrameBuffer[2][i + METER_PAD_LEFT] |= 0b00000111;
    }
  }

  sLevelAttributes sLevelAtt;
  sLevelAtt = GetSLevelAttributes(scanInfo.rssi, fMeasure);

  if(sLevelAtt.over > 0)
  {
    sprintf(String, "S%2d+%2d", sLevelAtt.sLevel, sLevelAtt.over);
  }
  else
  {
    sprintf(String, "S%2d", sLevelAtt.sLevel);
  }

  GUI_DisplaySmallest(String, 4, 25, false, true);
  sprintf(String, "%d dBm", sLevelAtt.dBmRssi);
  GUI_DisplaySmallest(String, 40, 25, false, true);

  if (!monitorMode) {
    uint8_t x = Rssi2PX(settings.rssiTriggerLevel, 0, 121);
    gFrameBuffer[2][METER_PAD_LEFT + x] = 0b11111111;
  }

  const uint8_t PAD_LEFT = 4;
  const uint8_t CELL_WIDTH = 30;
  uint8_t offset = PAD_LEFT;
  uint8_t row = 4;

  for (int i = 0, idx = 1; idx <= 4; ++i, ++idx) {
    if (idx == 5) {
      row += 2;
      i = 0;
    }
    offset = PAD_LEFT + i * CELL_WIDTH;
    if (menuState == idx) {
      for (int j = 0; j < CELL_WIDTH; ++j) {
        gFrameBuffer[row][j + offset] = 0xFF;
        gFrameBuffer[row + 1][j + offset] = 0xFF;
      }
    }
    sprintf(String, "%s", registerSpecs[idx].name);
    GUI_DisplaySmallest(String, offset + 2, row * 8 + 2, false,
                        menuState != idx);
    sprintf(String, "%u", GetRegMenuValue(idx));
    GUI_DisplaySmallest(String, offset + 2, (row + 1) * 8 + 1, false,
                        menuState != idx);
  }
}

static void Render() {
  memset(gFrameBuffer, 0, sizeof(gFrameBuffer));

  switch (currentState) {
  case SPECTRUM:
    RenderSpectrum();
    break;
  case FREQ_INPUT:
    RenderFreqInput();
    break;
  case STILL:
    RenderStill();
    break;
  }

  ST7565_BlitFullScreen();
}

bool HandleUserInput() {
  kbd.prev = kbd.current;
  kbd.current = GetKey();

  if (kbd.current != KEY_INVALID && kbd.current == kbd.prev) {
    if(kbd.counter < 16)
      kbd.counter++;
    else
      kbd.counter-=3;
    SYSTEM_DelayMs(20);
  }
  else {
    kbd.counter = 0;
  }

  if (kbd.counter == 3 || kbd.counter == 16) {
    switch (currentState) {
    case SPECTRUM:
      OnKeyDown(kbd.current);
      break;
    case FREQ_INPUT:
      OnKeyDownFreqInput(kbd.current);
      break;
    case STILL:
      OnKeyDownStill(kbd.current);
      break;
    }
  }

  return true;
}

static void Scan() {
  if (rssiHistory[scanInfo.i] != RSSI_MAX_VALUE

  && !IsBlacklisted(scanInfo.i)

  ) {
    if (scanInfo.f/260000*260000 != scanInfo.f) //Robby69 remove all 26Mhz multiples
      SetF(scanInfo.f);
    Measure();
    UpdateScanInfo();
  }
}

static void NextScanStep() {
  ++peak.t;
      // channel mode
    if (appMode==CHANNEL_MODE)
    {
      if (scanInfo.i <= GetStepsCount())
      {
      int currentChannel = scanChannel[scanInfo.i];
      scanInfo.f =  gMR_ChannelFrequencyAttributes[currentChannel].Frequency;
      }
      ++scanInfo.i; 
    }
    else
    // frequency mode
    {
      ++scanInfo.i; 
      scanInfo.f += scanInfo.scanStep;
    }
}

static void UpdateScan() {
  Scan();
  if (scanInfo.i < GetStepsCount()) {
    NextScanStep();
    return;
  }

  if(scanInfo.measurementsCount < 128)
    memset(&rssiHistory[scanInfo.measurementsCount], 0, 
      sizeof(rssiHistory) - scanInfo.measurementsCount*sizeof(rssiHistory[0]));

  redrawScreen = true;
  preventKeypress = false;

  UpdatePeakInfo();
      
  if (IsPeakOverLevel()) {
    ToggleRX(true);
    TuneToPeak();
	return;
  }

  newScanStart = true;
}

static void UpdateStill() {
  Measure();
  redrawScreen = true;
  preventKeypress = false;

  peak.rssi = scanInfo.rssi;
  ToggleRX(IsPeakOverLevel() || monitorMode);
}

static void UpdateListening() {
  preventKeypress = false;
  if (currentState == STILL) {
    listenT = 0;
  }
  if (listenT) {
    listenT--;
    SYSTEM_DelayMs(1);
    return;
  }

  if (currentState == SPECTRUM) {
    if(appMode!=CHANNEL_MODE)
      BK4819_WriteRegister(0x43, GetBWRegValueForScan());
    Measure();
    BK4819_SetFilterBandwidth(settings.listenBw, false);
  } else {
    Measure();
  }

  peak.rssi = scanInfo.rssi;
  redrawScreen = true;

  if ((IsPeakOverLevel() || monitorMode) ) {
    listenT = SQUELCH_OFF_DELAY;
    return;
  }

  ToggleRX(false);
  ResetScanStats();
}

static void Tick() {
  if (gNextTimeslice_500ms) {
    if (gBacklightCountdown > 0)
      if (--gBacklightCountdown == 0)
				if (!settings.backlightAlwaysOn)
					BACKLIGHT_TurnOff();   // turn backlight off
    gNextTimeslice_500ms = false;


    // if a lot of steps then it takes long time
    // we don't want to wait for whole scan
    // listening has it's own timer
    if(GetStepsCount()>128 && !isListening) {
      UpdatePeakInfo();
    if (IsPeakOverLevel()) {
        ToggleRX(true);
        TuneToPeak();
		return;
      }
      redrawScreen = true;
      preventKeypress = false;
    }
  }


  if (!preventKeypress) {
    HandleUserInput();
  }
  if (newScanStart) {
    InitScan();
    newScanStart = false;
  }
  if (isListening && currentState != FREQ_INPUT) {
    UpdateListening();
  } else {
    if (currentState == SPECTRUM) {
      UpdateScan();
    } else if (currentState == STILL) {
      UpdateStill();
    }
  }
  if (redrawStatus || ++statuslineUpdateTimer > 4096) {
    latestScanListName[0] = '\0';
    RenderStatus();
    redrawStatus = false;
    statuslineUpdateTimer = 0;
  }
  if (redrawScreen) {
    Render();
    redrawScreen = false;
  }
}


void APP_RunSpectrum(Mode mode) {
  // reset modifiers if we launched in a different then previous mode
  LoadSettings();//Robby69
  uint8_t Last_state = 1; //Spectrum Active
  if (mode == SCAN_BAND_MODE) Last_state =2;
  
  EEPROM_WriteBuffer(0x1D00, &Last_state, 1);
  
  if(appMode!=mode){
    ResetModifiers();
  }
  appMode = mode;
  if (appMode==CHANNEL_MODE)LoadValidMemoryChannels();

  if(mode==SCAN_RANGE_MODE) {
    currentFreq = initialFreq = gScanRangeStart;
    for(uint8_t i = 0; i < ARRAY_SIZE(scanStepValues); i++) {
      if(scanStepValues[i] >= gTxVfo->StepFrequency) {
        settings.scanStepIndex = i;
        break;
      }
    }
  }
  else
  currentFreq = initialFreq = gTxVfo->pRX->Frequency;
  BackupRegisters();
  ResetInterrupts();

  // turn of GREEN LED if spectrum was started during active RX
  BK4819_ToggleGpioOut(BK4819_GPIO6_PIN2_GREEN, false);

  isListening = true; // to turn off RX later
  redrawStatus = true;
  redrawScreen = true;
  newScanStart = true;


  ToggleRX(true), ToggleRX(false); // hack to prevent noise when squelch off
  if (appMode != SCAN_BAND_MODE){
      RADIO_SetModulation(settings.modulationType = gTxVfo->Modulation);
      BK4819_SetFilterBandwidth(settings.listenBw = gTxVfo->CHANNEL_BANDWIDTH, false);
      settings.scanStepIndex = GetScanStepFromStepFrequency(gTxVfo->StepFrequency);
      RADIO_SetModulation(settings.modulationType = MODULATION_FM);
      BK4819_SetFilterBandwidth(settings.listenBw = BK4819_FILTER_BW_NARROWER, false);
      AutoAdjustFreqChangeStep();
      }

  RelaunchScan();

  for (int i = 0; i < 128; ++i) {
    rssiHistory[i] = 0;
  }

  isInitialized = true;

  while (isInitialized) {
    Tick();
  }
}

void LoadValidMemoryChannels(void)
  {
    memset(scanChannel,0,sizeof(scanChannel));
    scanChannelsCount = 0;
    bool listsEnabled = false;
    
    // loop through all scanlists
    for (int sl=1; sl <= 16; sl++) {
      // skip disabled scanlist
      if (sl <= 15 && !settings.scanListEnabled[sl-1])
        continue;

      // valid scanlist is enabled
      if (sl <= 15 && settings.scanListEnabled[sl-1])
        listsEnabled = true;
      
      // break if some lists were enabled, else scan all channels
      if (sl > 15 && listsEnabled)
        break;

      uint8_t offset = scanChannelsCount;
      uint8_t listChannelsCount = RADIO_ValidMemoryChannelsCount(listsEnabled, sl-1);
      scanChannelsCount += listChannelsCount;
      signed int channelIndex=-1;
      for(int i=0; i < listChannelsCount; i++)
      {
        int nextChannel;
        nextChannel = RADIO_FindNextChannel((channelIndex)+1, 1, listsEnabled, sl-1);

        if (nextChannel == 0xFF)
        {	// no valid channel found
          break;
        }
        else
        {
          channelIndex = nextChannel;
          scanChannel[offset+i]=channelIndex;
        }
      }
    }
  }

  void ToggleScanList(int scanListNumber, int single)
  {
    if (appMode == SCAN_BAND_MODE)
      {
      if (single) memset(settings.bandEnabled, 0, sizeof(settings.bandEnabled));
        settings.bandEnabled[scanListNumber-1] = !settings.bandEnabled[scanListNumber-1];
      }
      else {
        if (single) memset(settings.scanListEnabled, 0, sizeof(settings.scanListEnabled));
        settings.scanListEnabled[scanListNumber-1] = !settings.scanListEnabled[scanListNumber-1];
      LoadValidMemoryChannels();
      }
    ResetModifiers();
  }

  // flattens spectrum by bringing all the rssi readings to the peak value
  void ToggleNormalizeRssi(bool on)
  {
    // we don't want to normalize when there is already active signal RX
    if(IsPeakOverLevel() && on){
		UpdateScan();//Robby69 Force scan continue
		UpdateScan();
		return;}

    if(on) {
      for(uint8_t i = 0; i < ARRAY_SIZE(rssiHistory); i++)
      {
        gainOffset[i] = peak.rssi - rssiHistory[i];
      }
      isNormalizationApplied = true;
      settings.rssiTriggerLevel=RSSI_MAX_VALUE;
    }
    else {
      memset(gainOffset, 0, sizeof(gainOffset));
      isNormalizationApplied = false;
    }
    AutoTriggerLevel(); //Robby69
    RelaunchScan();
  }



typedef struct {
  uint16_t scanListFlags;          // Bits 0-14: scanListEnabled[0..14]
  uint16_t bandListFlags;
  uint16_t rssiTriggerLevel;
  uint16_t rssiTriggerLevelH;
  int8_t dbMax;
  int8_t dbMin;
  
  //uint32_t gScanRangeStart;
  //uint32_t gScanRangeStop;
  Mode appMode;
  //StepsCount stepsCount;
  //ScanStep scanStepIndex;
  //uint32_t frequencyChangeStep;  
  //BK4819_FilterBandwidth_t bw;
  //BK4819_FilterBandwidth_t listenBw;
  
} SettingsEEPROM;

  
void LoadSettings()
{
  SettingsEEPROM eepromData = {0};
  
  // Lecture de toutes les données
  EEPROM_ReadBuffer(0x1D10, &eepromData, sizeof(eepromData));
  
  // Extraction des flags
  for (int i = 0; i < 15; i++) {
      settings.scanListEnabled[i] = (eepromData.scanListFlags >> i) & 0x01;
      settings.bandEnabled[i] = (eepromData.bandListFlags >> i) & 0x01;
  }
  
  // Copie des autres champs
  settings.rssiTriggerLevel = eepromData.rssiTriggerLevel;
  settings.rssiTriggerLevelH = eepromData.rssiTriggerLevelH;
  //gScanRangeStart = eepromData.gScanRangeStart;
  //gScanRangeStop = eepromData.gScanRangeStop;
  appMode = eepromData.appMode;
  //settings.stepsCount = eepromData.stepsCount;
  //settings.scanStepIndex = eepromData.scanStepIndex;
  //settings.frequencyChangeStep = eepromData.frequencyChangeStep;
  //settings.bw = eepromData.bw;
  //settings.listenBw = eepromData.listenBw;
  settings.dbMin = eepromData.dbMin;
  settings.dbMax = eepromData.dbMax;
  }

void SaveSettings() 
{
  SettingsEEPROM eepromData = {0};
  
  // Pack 15 boolean flags
  for (int i = 0; i < 15; i++) {
      if (settings.scanListEnabled[i]) eepromData.scanListFlags |= (1 << i);
      if (settings.bandEnabled[i]) eepromData.bandListFlags |= (1 << i);
      }
    
  // Copie des autres champs
  eepromData.rssiTriggerLevel = settings.rssiTriggerLevel;
  eepromData.rssiTriggerLevelH = settings.rssiTriggerLevelH;
  //eepromData.gScanRangeStart = gScanRangeStart;
  //eepromData.gScanRangeStop = gScanRangeStop;
  eepromData.appMode = appMode;
  //eepromData.stepsCount = settings.stepsCount;
  //eepromData.scanStepIndex = settings.scanStepIndex;
  //eepromData.frequencyChangeStep = settings.frequencyChangeStep;
  //eepromData.bw = settings.bw;
  //eepromData.listenBw = settings.listenBw;
  eepromData.dbMin = settings.dbMin;
  eepromData.dbMax = settings.dbMax;
  
  // Écriture de toutes les données
  EEPROM_WriteBuffer(0x1D10, &eepromData, sizeof(eepromData));
}