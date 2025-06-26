#include "app/spectrum.h"
#include "chFrScanner.h"
#include "scanner.h"
#include "driver/backlight.h"
#include "driver/eeprom.h"   // EEPROM_ReadBuffer()
#include "audio.h"
#include "ui/helper.h"
#include "common.h"
#include "action.h"
#include "bands.h"
//#include "debugging.h"
/*	
          /////////////////////////DEBUG//////////////////////////
          char str[64] = "";sprintf(str, "%d\n", Spectrum_state );LogUart(str);
*/
#ifdef ENABLE_SCREENSHOT
  #include "screenshot.h"
#endif
#define MAX_VISIBLE_LINES 6
#define HISTORY_SIZE 100
//////////////////Parameters:
uint8_t DelayRssi=11;
uint8_t RandomEmission = 0;
#define PARAMETER_COUNT 2
/////////////////////////////
bool FreeTriggerLevel = 0;
bool StorePtt_Toggle_Mode = 0;
uint8_t historyListIndex = 0;
bool historyListActive = false;
static uint32_t PreviousRecorded = 0;
static int historyScrollOffset = 0;
static void RenderHistoryList();
static void RenderScanListSelect();
static void RenderParametersSelect();
static uint8_t bandListSelectedIndex = 0;
static int bandListScrollOffset = 0;
static void RenderBandSelect();
uint8_t scanListSelectedIndex = 0;
uint8_t scanListScrollOffset = 0;
uint8_t parametersSelectedIndex = 0;
uint8_t parametersScrollOffset = 0;
static uint8_t validScanListCount = 0;
bool inScanListMenu = false;
KeyboardState kbd = {KEY_INVALID, KEY_INVALID, 0,0};
bool AutoTriggerLevelbandsMode = 0;
struct FrequencyBandInfo {
    uint32_t lower;
    uint32_t upper;
    uint32_t middle;
};

bool isBlacklistApplied;
bool saved_params= false;

uint32_t cdcssFreq;
uint16_t ctcssFreq;
uint8_t refresh = 0;
uint8_t SquelchBarKeyMode = 0; //Robby69 change keys between audio and history squelch
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
static void AutoTriggerLevel(void);
static void AutoTriggerLevelbands(void);
const uint16_t RSSI_MAX_VALUE = 65535;
#define SQUELCH_OFF_DELAY 100 //Robby69
static uint16_t R30, R37, R3D, R43, R47, R48, R7E, R02, R3F;
static uint32_t initialFreq;
static char String[100];
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
uint8_t bl;
uint8_t CurrentScanBand = 1;
State currentState = SPECTRUM, previousState = SPECTRUM;
uint8_t Spectrum_state; 
PeakInfo peak;
ScanInfo scanInfo;
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
uint16_t BPRssiTriggerLevel[32]={0};

SpectrumSettings settings = {stepsCount: STEPS_128,
                             scanStepIndex: S_STEP_25_0kHz,
                             frequencyChangeStep: 80000,
                             rssiTriggerLevel: 100,
							               rssiTriggerLevelH: 100,
                             backlightAlwaysOn: false,
                             bw: BK4819_FILTER_BW_WIDE,
                             listenBw: BK4819_FILTER_BW_WIDE,
                             modulationType: false,
                             dbMin: -128,
                             dbMax: 10,
                             scanList: S_SCAN_LIST_ALL,
                             scanListEnabled: {0},
                             bandEnabled: {0}
                            };

uint32_t fMeasure = 0;
uint32_t currentFreq, tempFreq;
uint16_t rssiHistory[128];
const uint8_t FMaxNumb = HISTORY_SIZE;
uint32_t freqHistory[HISTORY_SIZE+1]= {0};
uint8_t freqCount[HISTORY_SIZE+1] = {0};
uint8_t indexFd = 0;
uint8_t indexFs = 1;
bool ShowHistory = false;
uint8_t freqInputIndex = 0;
uint8_t freqInputDotIndex = 0;
KEY_Code_t freqInputArr[10];
char freqInputString[11];
static const bandparameters BParams[32];
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
static char StringCode[10] = "";
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
  
  
  // Gestion PTT existante
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

bool IsPeakOverLevel() {return peak.rssi > settings.rssiTriggerLevel; }

bool IsPeakOverLevelH() { return scanInfo.rssi > settings.rssiTriggerLevelH; }

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
    peak.f = 0; // DODAJ TO
    peak.i = 0; // ORAZ TO (jeśli peak.i jest używane jako indeks w danych szczytu)
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
static void DeInitSpectrum(bool ComeBack) {
  
  RestoreRegisters();
  currentFreq = initialFreq = gScanRangeStart = 0;
  gVfoConfigureMode = VFO_CONFIGURE;
  isInitialized = false;
  SetState(SPECTRUM);
  if(!ComeBack) {
    uint8_t Spectrum_state = 0; //Spectrum Not Active
    EEPROM_WriteBuffer(0x1D00, &Spectrum_state, 1);
    //SetF(initialFreq);
    }
    
  else {
    EEPROM_ReadBuffer(0x1D00, &Spectrum_state, 1);
	  Spectrum_state+=10;
    EEPROM_WriteBuffer(0x1D00, &Spectrum_state, 1);
    StorePtt_Toggle_Mode = Ptt_Toggle_Mode;
    Ptt_Toggle_Mode =0;
    }
  ToggleNormalizeRssi(false);
}

static void ExitAndCopyToVfo() {
  RestoreRegisters();
  switch (currentState) {
    case HISTORY_LIST: 
      SETTINGS_SetVfoFrequency(freqHistory[historyListIndex+1]); 
      gTxVfo->Modulation = MODULATION_FM;
      //SetF(freqHistory[historyListIndex+1]);
      gRequestSaveChannel = 1;
      DeInitSpectrum(0);
      break;
    case SPECTRUM:
      DeInitSpectrum(1);
      break;      
    
    default:
      DeInitSpectrum(0);
      break;
  }
    // Additional delay to debounce keys
    SYSTEM_DelayMs(200);
    isInitialized = false;
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
  // was 100 , some k5 bug when starting spectrum
  BK4819_ReadRegister(0x63);
  SYSTICK_DelayUs(DelayRssi* 1000); // Delay in microseconds
  rssi = BK4819_GetRSSI();
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

void FillfreqHistory(bool count) {
    // Validate frequency range
    if (scanInfo.f == 0 || scanInfo.f >= 130000000) {
        return;
    }

    // Check if we've already recorded this frequency
    for (uint8_t i = 1; i <= FMaxNumb; i++) {
        if (freqHistory[i] == scanInfo.f) {
            // Increment count only if it's a new detection
            if (count && (PreviousRecorded != scanInfo.f)) {
                freqCount[i]++;
                PreviousRecorded = scanInfo.f;
            }
            indexFd = i;  // Set current display index
            return;       // Exit early since we found it
        }
    }

    // If we get here, it's a new frequency
    freqHistory[indexFs] = scanInfo.f;
    freqCount[indexFs] = 1;  // Start count at 1 for new detections
    indexFd = indexFs;    // Set current display index
    
    // Advance storage index with wrap-around
    if (++indexFs > FMaxNumb) {
        indexFs = 1;
    }
    
    PreviousRecorded = scanInfo.f;
    
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
  { listenT = SQUELCH_OFF_DELAY;
    BK4819_SetFilterBandwidth(settings.listenBw, false);

    //gTailFound=false;

    // turn on CSS tail found interrupt
    BK4819_WriteRegister(BK4819_REG_3F, BK4819_REG_02_CxCSS_TAIL);

    // keep backlight and bold channel name display on as long as we are receiving
    gBacklightCountdown = 0;
    rxChannelDisplayCountdown = 0;
  } else
  {   if(appMode!=CHANNEL_MODE)
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

bool SingleBandCheck(void) {
    int count = 0;
    for (int i = 0; i < 32; i++) {
        if (settings.bandEnabled[i]) {
            if (++count > 1) {
                return false;
            }
        }
    }
    return (count == 1);
}

static bool InitScan() {
    ResetScanStats();
    scanInfo.i = 0;
    
    bool scanInitializedSuccessfully = false;

    if (appMode == SCAN_BAND_MODE) {
        uint8_t checkedBandCount = 0; // Licznik sprawdzonych pasm, aby uniknąć nieskończonej pętli
        while (checkedBandCount < 32) { // Sprawdź wszystkie 15 pasm co najwyżej raz
            if (settings.bandEnabled[nextBandToScanIndex]) {
                int j=0;
                bl = nextBandToScanIndex; // Użyj bieżącego jako aktywnego
                scanInfo.f = BParams[bl].Startfrequency;
                scanInfo.scanStep = scanStepValues[BParams[bl].scanStep];
                settings.scanStepIndex = BParams[bl].scanStep; // Aktualizuj globalny, jeśli potrzebne
                gScanRangeStart = BParams[bl].Startfrequency;
                gScanRangeStop = BParams[bl].Stopfrequency;
                scanInfo.measurementsCount = GetStepsCount();
                
                RADIO_SetModulation(BParams[bl].modulationType);      // Ustaw modulację dla pasma
                BK4819_InitAGC(gEeprom.RX_AGC, settings.modulationType);
                nextBandToScanIndex = (nextBandToScanIndex + 1) % 32; // Przygotuj indeks na następne wywołanie
                scanInitializedSuccessfully = true;
                redrawStatus = true; // Te flagi mogą być potrzebne tutaj
                //redrawScreen = true;
                if (AutoTriggerLevelbandsMode) AutoTriggerLevelbands();
                  else {if (!FreeTriggerLevel)settings.rssiTriggerLevel = BPRssiTriggerLevel[bl];}
                for (int i = 0; i < 32; i++) {if (settings.bandEnabled[i]) j++;}
                  if (j>1) settings.modulationType = BParams[bl].modulationType;
                break; // Znaleziono aktywne pasmo, przerwij pętlę while
            }
            nextBandToScanIndex = (nextBandToScanIndex + 1) % 32; // Przejdź do następnego pasma
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
  //if (settings.rssiTriggerLevel == RSSI_MAX_VALUE) {
  //settings.rssiTriggerLevel = clamp(scanInfo.rssiMax +10, 0, RSSI_MAX_VALUE); //Robby69 +8
  uint8_t max = 0;
  uint8_t i;
  for(i = 0; i < ARRAY_SIZE(rssiHistory); i++)
    {if (max < rssiHistory[i]) max = rssiHistory[i];}
  settings.rssiTriggerLevel = clamp(max +8, 0, RSSI_MAX_VALUE);
	settings.rssiTriggerLevelH = settings.rssiTriggerLevel;
  //}
}

static void AutoTriggerLevelbands(void) {
  uint8_t rssiAnalyse = 0;
  uint8_t topRssi = 0;
  static uint8_t AnalyseSize = 32;
  uint32_t AnalyseStep = (gScanRangeStop - gScanRangeStart)/AnalyseSize;
  for (int i = 0; i < AnalyseSize; ++i) {
      uint32_t FreqAnalyse = gScanRangeStart + (AnalyseStep * i);
      SetF(FreqAnalyse);
      while ((BK4819_ReadRegister(0x63) & 0b11111111) >= 255) SYSTICK_DelayUs(500);
      rssiAnalyse = BK4819_GetRSSI();
      
      if (rssiAnalyse > topRssi) {topRssi = rssiAnalyse;}    
  }
    settings.rssiTriggerLevel = clamp(topRssi + 10, 0, RSSI_MAX_VALUE);
    settings.rssiTriggerLevelH = settings.rssiTriggerLevel;
  
}

// resets modifiers like blacklist, attenuation, normalization
static void ResetModifiers() {
  memset(StringC, 0, sizeof(StringC)); 
  for (int i = 0; i < 128; ++i) {
    if (rssiHistory[i] == RSSI_MAX_VALUE) rssiHistory[i] = 0;
  }
  if(appMode==CHANNEL_MODE){LoadValidMemoryChannels();}
  memset(attenuationOffset, 0, sizeof(attenuationOffset));
  RelaunchScan();
}

static void RelaunchScan() {
  ResetPeak();
  InitScan();
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
}

static void UpdatePeakInfo() {
  if (peak.f == 0 || peak.t >= 1024 || peak.rssi < scanInfo.rssiMax)
    UpdatePeakInfoForce();
}

static void Measure() 
{ 
    uint16_t rssi = scanInfo.rssi = GetRssi();
    
    if (IsPeakOverLevel())  {FillfreqHistory(true);}
    else if (IsPeakOverLevelH()) FillfreqHistory(false);
    //if (rssi > settings.rssiTriggerLevelH) FillfreqHistory(false);
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
		if (SquelchBarKeyMode == 2) {settings.rssiTriggerLevelH +=5;}
		if (SquelchBarKeyMode == 1) {settings.rssiTriggerLevel +=5;}
		if (SquelchBarKeyMode == 0) {
			settings.rssiTriggerLevelH +=5;
			settings.rssiTriggerLevel +=5;}}
	else {
		if (SquelchBarKeyMode == 2) {settings.rssiTriggerLevelH -=5;}
		if (SquelchBarKeyMode == 1) {settings.rssiTriggerLevel -=5;}
		if (SquelchBarKeyMode == 0) {
			settings.rssiTriggerLevelH -=5;
		  settings.rssiTriggerLevel  -=5;}}
  ClampRssiTriggerLevel();
  BPRssiTriggerLevel[bl] = settings.rssiTriggerLevel;

  
}

static void UpdateDBMax(bool inc) {
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
  if (inc) settings.scanStepIndex++;
  if (settings.scanStepIndex > S_STEP_500kHz) settings.scanStepIndex = S_STEP_0_01kHz;
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

static bool IsBlacklisted(uint16_t idx){
  for(uint8_t i = 0; i < ARRAY_SIZE(blacklistFreqs); i++)
    if(blacklistFreqs[i] == idx)
      return true;
  return false;
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
  int len=0;
  int pos=0;
  len = sprintf(&String[pos],"%s %ddb %s ", gModulationStr[settings.modulationType],settings.dbMax,bwNames[settings.listenBw] );
  pos += len;  // Move position forward

  if (currentState == SPECTRUM) {
    if(isNormalizationApplied){
      len = sprintf(&String[pos], "N(%ux) ", GetStepsCount());
      pos += len;
    }
    else {
      len = sprintf(&String[pos], "%ux ", GetStepsCount());
      pos += len;
    }
    len = sprintf(&String[pos],"%dms ", DelayRssi);
    pos += len;
    
  if (appMode==CHANNEL_MODE)
    {
      len = sprintf(&String[pos], "M%i ", channel+1);
      pos += len;      
    }
    else
    {if (scanInfo.scanStep<2500)
      {len = sprintf(&String[pos],"%u.%02uk", scanInfo.scanStep / 100, scanInfo.scanStep % 100);
      pos += len;}
    else {len = sprintf(&String[pos],"%uk", scanInfo.scanStep / 100, scanInfo.scanStep % 100);pos += len;}
    }
  }
  GUI_DisplaySmallest(String, 0, 1, true,true);
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

 // Format frequency string (remove trailing zeros)
 void RemoveTrailZeros(char* freqStr){
     
     char *p = freqStr + strlen(freqStr) - 1;
     while (p > freqStr && *p == '0') *p-- = '\0';
    if (*p == '.') *p = '\0';
}

static void formatHistory(char *buf, uint8_t index, int channel, uint32_t freq) {
    char freqStr[16];
    
    // Handle 833Hz stepping adjustment if needed
    if(channel == -1 && GetScanStep() == 833) {
            uint32_t base = freq/2500*2500;
            int chno = (freq - base) / 700;
            freq = base + (chno * 833) + (chno == 3);
        }

    // Format frequency string
    snprintf(freqStr, sizeof(freqStr), "%u.%05u", freq/100000, freq%100000);
    RemoveTrailZeros(freqStr);

    if(channel != -1) {
        // Known channel format
        snprintf(buf, 19, "%u:%s(%u)", 
                index, 
                gMR_ChannelFrequencyAttributes[channel].Name,
                freqCount[index]);
                
    } else {
        // Unknown channel format
        snprintf(buf, 19, "%u:%s(%u)", 
                index,
                freqStr,
                freqCount[index]);
        }
}


static void DrawF(uint32_t f) {
    if (f == 0) return;

    // --- Frequency Formatting ---
    uint8_t Code;
    if (GetScanStep() == 833) {
        uint32_t base = f / 2500 * 2500;
        int chno = (f - base) / 700;
        f = base + (chno * 833) + (chno == 3);
    }
    char freqStr[16];
    sprintf(freqStr, "%u.%05u", f / 100000, f % 100000);
    RemoveTrailZeros(freqStr);

    // --- CTCSS/DCS Detection ---
    if (refresh == 0) {
        BK4819_CssScanResult_t scanResult = BK4819_GetCxCSSScanResult(&cdcssFreq, &ctcssFreq);
        refresh = 1;
        if (scanResult == BK4819_CSS_RESULT_CDCSS) {
            Code = DCS_GetCdcssCode(cdcssFreq);
            refresh = 30;
            if (Code != 0xFF) sprintf(StringCode, "D%03oN", DCS_Options[Code]);
        } else if (scanResult == BK4819_CSS_RESULT_CTCSS) {
            Code = DCS_GetCtcssCode(ctcssFreq);
            refresh = 30;
            sprintf(StringCode, "%u.%uHz", CTCSS_Options[Code] / 10, CTCSS_Options[Code] % 10);
        }
    }
    refresh--;
    if (refresh == 0) memset(StringCode, 0, sizeof(StringCode));

    // --- Prepare Display Lines ---
    char line1[19] = "";
    char line2[19] = "";
    char line3[19] = "";

    f = freqHistory[indexFd];
    bool showHistory = (ShowHistory) && (f > 0) && (indexFd > 0);
    int channelFd = BOARD_gMR_fetchChannel(f);
    isKnownChannel = channelFd != -1;
    
    strncpy(line1, freqStr, 18);
    
    // --- Default: Band Name or Scan List (Top Line) ---
    if (appMode == SCAN_BAND_MODE && !isListening) {
        snprintf(line1, sizeof(line1), "B%u:%s", bl+1, BParams[bl].BandName);
    } else if (appMode == CHANNEL_MODE && !isListening) {
              // Count enabled scan lists
              uint8_t enabledCount = 0;
              for (int i = 0; i < 15; i++) {
                if (settings.scanListEnabled[i]) enabledCount++;
              }
            snprintf(line1, sizeof(line1), "Scan Lists (%d)", enabledCount);
            }

    // --- If Listening or Code Detected ---
    if (isListening || refresh > 1) {
        // Priority 1: Show Frequency + Code (if available)
        if (refresh > 1 && StringCode[0]) {
            if (isKnownChannel) {
                // Try to fit "Name Code" on one line
                if (strlen(channelName) + 1 + strlen(StringCode) <= 18) {
                    snprintf(line1, sizeof(line1), "%s %s", channelName, StringCode);
                } else {
                    // Split into two lines
                    strncpy(line1, channelName, 18);
                    strncpy(line2, StringCode, 18);
                }
            } else {
                // Show "Frequency Code"
                snprintf(line2, sizeof(line2), "%s %s", freqStr, StringCode);
            }
        } 
        // Priority 2: Show Channel Name + Frequency (if known)
        else if (isKnownChannel) {
            if (strlen(channelName) + 1 + strlen(freqStr) <= 18) {
                snprintf(line1, sizeof(line2), "%s %s", channelName, freqStr);
            } else {
                strncpy(line1, channelName, 18);
                strncpy(line2, freqStr, 18);
            }
        } 
        // Priority 3: Just Frequency (if unknown channel)
    } 

    // --- Show History (if enabled) ---
    if (showHistory) {
        if (line3[0]) {
            // If line3 is already used, overwrite it with history
            formatHistory(line3, indexFd, channelFd, f);
        } else if (line2[0]) {
            formatHistory(line3, indexFd, channelFd, f);
        } else {
            formatHistory(line2, indexFd, channelFd, f);
        }
    }

    // --- Render to Screen ---
    UI_PrintStringSmallBold(line1, 1, 1, 0);  // Line 1 (Band/Scan List)
    if (line2[0]) UI_PrintStringSmallBold(line2, 1, 1, 1);  // Line 2 (Freq/Name/Code)
    if (line3[0]) UI_PrintStringSmallBold(line3, 1, 1, 2);  // Line 3 (Code/History)
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
  
  if(SquelchBarKeyMode ==0 )
  { sprintf(String, "HL");
    GUI_DisplaySmallest(String, 50, Bottom_print, false, true);
  }
  
  if(SquelchBarKeyMode ==1 )
  { sprintf(String, "L");
    GUI_DisplaySmallest(String, 50, Bottom_print, false, true);
  }
  
  if(SquelchBarKeyMode ==2 )
  { sprintf(String, "H");
    GUI_DisplaySmallest(String, 50, Bottom_print, false, true);
  }

  if(isBlacklistApplied){
    sprintf(String, "BL");
    GUI_DisplaySmallest(String, 60, Bottom_print, false, true);
  }
  if(saved_params) //Display saved for a while
      {sprintf(String, "SA");
      GUI_DisplaySmallest(String, 70, Bottom_print, false, true);
      saved_params = false;
      }
  
  if(AutoTriggerLevelbandsMode && appMode == SCAN_BAND_MODE){ //Display status
    sprintf(String, "AB");
    GUI_DisplaySmallest(String, 80, Bottom_print, false, true);
    }
}

static void DrawRssiTriggerLevel() {
  //if (settings.rssiTriggerLevel == RSSI_MAX_VALUE || monitorMode)
  //  return;
  uint8_t y = Rssi2Y(settings.rssiTriggerLevel);
  for (uint8_t x = 0; x < 128; x += 2) {
    PutPixel(x, y, true);
  }
  if (ShowHistory) {
  y = Rssi2Y(settings.rssiTriggerLevelH);
  for (uint8_t x = 0; x < 128; x += 6) {PutPixel(x, y, true);}
  }
}

static void OnKeyDown(uint8_t key) {
        BACKLIGHT_TurnOn();
  
  
    // NEW HANDLING: press of '4' key in SCAN_BAND_MODE
    if (appMode == SCAN_BAND_MODE && key == KEY_4 && currentState == SPECTRUM) {
        SetState(BAND_LIST_SELECT);
        bandListSelectedIndex = 0; // Start from the first band
        bandListScrollOffset = 0;  // Reset scrolling
        redrawScreen = true;
        return; // Key handled
    }

    // NEW HANDLING: press of '4' key in CHANNEL_MODE
    if (appMode == CHANNEL_MODE && key == KEY_4 && currentState == SPECTRUM) {
        SetState(SCANLIST_SELECT);
        scanListSelectedIndex = 0;
        scanListScrollOffset = 0;
        redrawScreen = true;
        return; // Key handled
    }
    
	if (key == KEY_5 && currentState == SPECTRUM) {
        SetState(PARAMETERS_SELECT);
        parametersSelectedIndex = 0;
        parametersScrollOffset = 0;
        redrawScreen = true;
        return; // Key handled
    }
    
    // If we're in band selection mode, use dedicated key logic
    if (currentState == BAND_LIST_SELECT) {
        switch (key) {
            case KEY_UP: //Band
                if (bandListSelectedIndex > 0) {
                    bandListSelectedIndex--;
                    if (bandListSelectedIndex < bandListScrollOffset) {
                        bandListScrollOffset = bandListSelectedIndex;
                    }
                    redrawScreen = true;
                }
                break;
            case KEY_DOWN:
                // ARRAY_SIZE(BParams) gives the number of defined bands
                if (bandListSelectedIndex < ARRAY_SIZE(BParams) - 1) {
                    bandListSelectedIndex++;
                    if (bandListSelectedIndex >= bandListScrollOffset + MAX_VISIBLE_LINES) {
                        bandListScrollOffset = bandListSelectedIndex - MAX_VISIBLE_LINES + 1;
                    }
                    redrawScreen = true;
                }
                break;
            case KEY_4: // Band selection
                if (bandListSelectedIndex < ARRAY_SIZE(BParams)) {
                    // Set the selected band as the only active one for scanning
                    settings.bandEnabled[bandListSelectedIndex] = !settings.bandEnabled[bandListSelectedIndex]; 
                    // Reset nextBandToScanIndex so InitScan starts from the selected one
                    nextBandToScanIndex = bandListSelectedIndex; 
                    bandListSelectedIndex++;
                }
                break;
            case KEY_5:   
                if (bandListSelectedIndex < ARRAY_SIZE(BParams)) {
                    // Set the selected band as the only active one for scanning
                    memset(settings.bandEnabled, 0, sizeof(settings.bandEnabled)); // Clear all flags
                    settings.bandEnabled[bandListSelectedIndex] = true; // Enable selected band
                    
                    // Reset nextBandToScanIndex so InitScan starts from the selected one
                    nextBandToScanIndex = bandListSelectedIndex; 
                }
                break;
				
				        // NOWA FUNKCJA: Przejście do wybranego zakresu po wciśnięciu MENU
        case KEY_MENU:
            if (bandListSelectedIndex < ARRAY_SIZE(BParams)) {
                // Ustaw tylko wybrane pasmo jako aktywne
                memset(settings.bandEnabled, 0, sizeof(settings.bandEnabled));
                settings.bandEnabled[bandListSelectedIndex] = true;
                
                // Ustaw indeks pasma do skanowania
                nextBandToScanIndex = bandListSelectedIndex;
                
                // Przejdź do trybu spektrum/skanowania
                SetState(SPECTRUM);
                redrawScreen = true;
                redrawStatus = true;
                
                // Uruchom skanowanie wybranego pasma
                RelaunchScan();
            }
            break;
				
            case KEY_EXIT: // Exit band list
                SetState(SPECTRUM); // Return to band scanning mode
                redrawScreen = true;
                redrawStatus = true;
                RelaunchScan(); 
                break;
            default:
                break;
        }
        return; // Finish handling if we were in BAND_LIST_SELECT
    }
// If we're in scanlist selection mode, use dedicated key logic
    if (currentState == SCANLIST_SELECT) {
        switch (key) {

            case KEY_UP://SCANLIST
                if (scanListSelectedIndex > 0) {
                    scanListSelectedIndex--;
                    if (scanListSelectedIndex < scanListScrollOffset) {
                        scanListScrollOffset = scanListSelectedIndex;
                    }
                    redrawScreen = true;
                }
                break;
            case KEY_DOWN:
                // ARRAY_SIZE(BParams) gives the number of defined bands
                if (scanListSelectedIndex < validScanListCount-1) { 
                    scanListSelectedIndex++;
                    if (scanListSelectedIndex >= scanListScrollOffset + MAX_VISIBLE_LINES) {
                        scanListScrollOffset = scanListSelectedIndex - MAX_VISIBLE_LINES + 1;
                    }
                    redrawScreen = true;
                }
                break;
            case KEY_4: // Scan list selection
                ToggleScanList(scanListSelectedIndex, 0);
                scanListSelectedIndex++;
                redrawScreen = true;
                break;
            case KEY_5:   
                ToggleScanList(scanListSelectedIndex, 1);
                 redrawScreen = true;
                break;
				
				        
        case KEY_MENU:
            if (scanListSelectedIndex < 15) {
                ToggleScanList(scanListSelectedIndex, 1);
                SetState(SPECTRUM);
                ResetModifiers();
                redrawScreen = true;
                redrawStatus = true;
                RelaunchScan();
            }
            break;
				
        case KEY_EXIT: // Exit scan list selection
                SetState(SPECTRUM); // Return to scanning mode
                ResetModifiers();
                redrawScreen = true;
                redrawStatus = true;
                RelaunchScan(); 
                break;
        default:
                break;
        }
        return; // Finish handling if we were in SCAN_LIST_SELECT
      }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // If we're in PARAMETERS_SELECT selection mode, use dedicated key logic
    if (currentState == PARAMETERS_SELECT) {
      
      
      switch (key) {
          case KEY_UP://PARAMETERS
                if (parametersSelectedIndex > 0) {
                    parametersSelectedIndex--;
                    if (parametersSelectedIndex < parametersScrollOffset) {
                        parametersScrollOffset = parametersSelectedIndex;
                    }
                }
                break;
          case KEY_DOWN:
                if (parametersSelectedIndex < PARAMETER_COUNT-1) { 
                    parametersSelectedIndex++;
                    if (parametersSelectedIndex >= parametersScrollOffset + MAX_VISIBLE_LINES) {
                        parametersScrollOffset = parametersSelectedIndex - MAX_VISIBLE_LINES + 1;
                    }
                }
                break;
          case KEY_1: // Scan list selection
                if (parametersSelectedIndex == 0){
                      DelayRssi ++; 
                      if (DelayRssi > 12) DelayRssi = 2;
                      redrawStatus = true;}
                if (parametersSelectedIndex == 1)RandomEmission = !RandomEmission; 
                break;
          case KEY_7:   
                if (parametersSelectedIndex == 0){
                      DelayRssi --; 
                      if (DelayRssi < 2) DelayRssi = 12;
                      redrawStatus = true;}
                if (parametersSelectedIndex == 1)RandomEmission = !RandomEmission; 
                break;
        case KEY_EXIT:
          // Exit parameters menu to previous menu/state
          SetState(previousState);
          
          break;

        default:
          break;
      }
      redrawScreen = true;      
      return; // Finish handling if we were in PARAMETERS_SELECT
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

  switch (key) {

      case KEY_STAR:
         FreeTriggerLevel = true;
         UpdateRssiTriggerLevel(true);
      break;
     
     case KEY_F:
        FreeTriggerLevel = true;
        UpdateRssiTriggerLevel(false);
     break;

     case KEY_3:
        UpdateDBMax(true);
     break;
     
     case KEY_9:
        UpdateDBMax(false);
    break;

     case KEY_1:
        AutoTriggerLevel();
        SquelchBarKeyMode=0;
    break;
     
     case KEY_7:
       if(appMode!=CHANNEL_MODE) {UpdateScanStep(true);}
       break;
     
     case KEY_2:
      if (appMode != SCAN_BAND_MODE || SingleBandCheck())
            ToggleNormalizeRssi(!isNormalizationApplied);
        else {
          AutoTriggerLevelbandsMode=!AutoTriggerLevelbandsMode;
          ToggleNormalizeRssi(false);}
      redrawStatus = true;
      break;

     case KEY_8:
      if ((ShowHistory) &&  (kbd.counter == 16)) { //(long press):
        memset(&freqHistory[1], 0, sizeof(freqHistory) - sizeof(freqHistory[0])); // ZMIANA: od pozycji 1
        memset(&freqCount[1], 0, sizeof(freqCount) - sizeof(freqCount[0])); // ZMIANA: od pozycji 1
        indexFd = 1;
        indexFs = 1;
      }
      else ShowHistory = !ShowHistory;
      redrawStatus = true;
      break;
      
    case KEY_UP:
    redrawScreen = true;
    if (currentState == HISTORY_LIST) {
        if (historyListIndex > 0) {
             historyListIndex--;
            if (historyListIndex < historyScrollOffset) {historyScrollOffset = historyListIndex;}
            redrawScreen = true;
        }
        } else {
          if ((!ShowHistory) && (appMode==SCAN_BAND_MODE)) {
            ToggleScanList(bl, 1);
            settings.bandEnabled[bl+1]= true;
            redrawStatus = true;
            RelaunchScan(); 
            break;
          }
      
      if (indexFd < indexFs-1) indexFd++;
      if(appMode==FREQUENCY_MODE) {UpdateCurrentFreq(true);}
    }
    break;
  case KEY_DOWN:
    redrawScreen = true;
    if (currentState == HISTORY_LIST) {
        int numValidEntries = 0;
        for(int k=0; k < FMaxNumb; ++k) {
            if (freqHistory[k] != 0) {
                numValidEntries++;
            }
        }
        if (historyListIndex < numValidEntries-1) { 
          historyListIndex++;
          if (historyListIndex >= historyScrollOffset + MAX_VISIBLE_LINES) {
                historyScrollOffset = historyListIndex - MAX_VISIBLE_LINES + 1;
            }
            redrawScreen = true;
        }
    } else {
      if ((!ShowHistory) && (appMode==SCAN_BAND_MODE)) {
        ToggleScanList(bl, 1);
        settings.bandEnabled[bl-1]= true;
        redrawStatus = true;
        RelaunchScan(); 
        break;}
    indexFd--;
    if (indexFd < 1) {indexFd = 1;}
    if(appMode==FREQUENCY_MODE){UpdateCurrentFreq(false);}
}
  break;
  
  case KEY_SIDE1:
    #ifdef ENABLE_SCREENSHOT
      getScreenShot();
    #endif
    Blacklist();
    break;
  
  case KEY_4:
    if (appMode!=SCAN_RANGE_MODE){ToggleStepsCount();}
    break;

  case KEY_0:
    ToggleModulation();
    redrawStatus = true;
    break;
  case KEY_6:
    ToggleListeningBW();
    redrawStatus = true;
    break;
  
  case KEY_SIDE2:
      if (kbd.counter == 3) { // short press        
        SquelchBarKeyMode += 1;
        if (SquelchBarKeyMode == 3) SquelchBarKeyMode = 0;
        ShowHistory = 1;
        } 
      break;

  case KEY_PTT:
      ExitAndCopyToVfo();
      break;
  
  case KEY_MENU:
  if (kbd.counter == 3) SaveSettings(); // short press
  else {
        //int validIndices[FMaxNumb + 1];
        int validCount = 0;
        
        for (int k = 1; k <= FMaxNumb; ++k) {
            if (freqHistory[k] != 0) {
                //validIndices[validCount] = k;
                validCount++;
            }
        }
        if (currentState == HISTORY_LIST) {
          uint32_t selectedFreq = freqHistory[historyListIndex+1];
          currentFreq = selectedFreq;
          fMeasure = selectedFreq;
          SetF(fMeasure);
        }
        else if (historyListIndex < validCount && ShowHistory) {
            //int realIndex = validIndices[historyListIndex];
            uint32_t selectedFreq = freqHistory[indexFd];
            currentFreq = selectedFreq;
            fMeasure = selectedFreq;
            SetF(fMeasure);
        } else {SetF(peak.f);}
        
        SetState(STILL);
        monitorMode = false;
        menuState = 0;
        redrawScreen = true;
        redrawStatus = true;
        }
    break;

  case KEY_EXIT:
    if (currentState == HISTORY_LIST) {
      SetState(previousState);
      historyListActive = false;
      redrawScreen = true;
      break;
      }
    if (menuState) { menuState = 0;break;}
      DeInitSpectrum(0);
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
  case HISTORY_LIST:
    RenderHistoryList();
    break;
  
    case BAND_LIST_SELECT:
      RenderBandSelect();
    break;

    case SCANLIST_SELECT:
      RenderScanListSelect();
    break;
    case PARAMETERS_SELECT:
      RenderParametersSelect();
                

    break;
  }

  ST7565_BlitFullScreen();
}

bool HandleUserInput() {
    kbd.prev = kbd.current;
    kbd.current = GetKey();
    if (kbd.current != KEY_INVALID && kbd.current == kbd.prev) {
        if (kbd.counter < 16)
            kbd.counter++;
        else
            kbd.counter -= 3;
        SYSTEM_DelayMs(20);
    } else {
        kbd.counter = 0;
    }

    if (kbd.counter == 3 || kbd.counter == 16) {
    // Długie wciśnięcie klawisza 0
        if (kbd.current == KEY_0 && kbd.counter == 16) {
            if (currentState != HISTORY_LIST) { // Dodatkowe zabezpieczenie
                SetState(HISTORY_LIST); // Przejście do stanu wyświetlania historii
                historyListIndex = 0;
				        historyScrollOffset = 0; // UPEWNIJ SIĘ
                historyListActive = true;
                redrawScreen = true;
                return true;
            }
    }

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
            case HISTORY_LIST: // DODANO
                OnKeyDown(kbd.current);
                break;
			      case BAND_LIST_SELECT: // DODANY BRAKUJĄCY CASE
                OnKeyDown(kbd.current);
                // Jeśli masz dedykowaną funkcję, np. OnKeyDownBandListSelect:
                // OnKeyDownBandListSelect(kbd.current, kbd.counter);
                break;
            case SCANLIST_SELECT:
                OnKeyDown(kbd.current);
                break;
            case PARAMETERS_SELECT:
                OnKeyDown(kbd.current);
                break;
        }
        return true;
    }
    return false;
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
    ResetPeak();
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


void APP_RunSpectrum(uint8_t Spectrum_state) {
  Mode mode;
  if (StorePtt_Toggle_Mode) Ptt_Toggle_Mode = StorePtt_Toggle_Mode;
  // Spectrum_state 1: MR channel, 2: band scan, 3: range scan, 4: basic spectrum, 5:new scan range 0: no spectrum
  if (Spectrum_state == 4) mode = FREQUENCY_MODE ;
  if (Spectrum_state == 3) mode = SCAN_RANGE_MODE ;
  if (Spectrum_state == 2) mode = SCAN_BAND_MODE ;
  if (Spectrum_state == 1) mode = CHANNEL_MODE ;
  EEPROM_WriteBuffer(0x1D00, &Spectrum_state, 1);
  LoadSettings();
  appMode = mode;
  ResetModifiers();
  if (appMode==CHANNEL_MODE)LoadValidMemoryChannels();

  if(appMode==SCAN_RANGE_MODE) {
    currentFreq = initialFreq = gScanRangeStart;
    for(uint8_t i = 0; i < ARRAY_SIZE(scanStepValues); i++) {
      if(scanStepValues[i] >= gTxVfo->StepFrequency) {
        settings.scanStepIndex = i;
        break;
      }
    }
  }
   if(appMode==FREQUENCY_MODE) {currentFreq = initialFreq = gTxVfo->pRX->Frequency;}
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

  void ToggleScanList(int scanListNumber, int single )
  {
    if (appMode == SCAN_BAND_MODE)
      {
      if (single) memset(settings.bandEnabled, 0, sizeof(settings.bandEnabled));
        else settings.bandEnabled[scanListNumber-1] = !settings.bandEnabled[scanListNumber-1];
      }
    if (appMode == CHANNEL_MODE) {
        if (single) {memset(settings.scanListEnabled, 0, sizeof(settings.scanListEnabled));}
        settings.scanListEnabled[scanListNumber] = !settings.scanListEnabled[scanListNumber];
      //LoadValidMemoryChannels();
      }
    //ResetModifiers();
  }

  // flattens spectrum by bringing all the rssi readings to the peak value
  void ToggleNormalizeRssi(bool on)
  {
    // we don't want to normalize when there is already active signal RX
    if(IsPeakOverLevel()){
        memset(gainOffset, 0, sizeof(gainOffset));
        isNormalizationApplied = false;
		    return;}

    if(on) {
      uint8_t max = 0;
      uint8_t i;
      for(i = 0; i < ARRAY_SIZE(rssiHistory); i++)
        {if (max < rssiHistory[i]) max = rssiHistory[i];}
      
      for(i = 0; i < ARRAY_SIZE(rssiHistory); i++)
        {gainOffset[i] = max - rssiHistory[i];}
      isNormalizationApplied = true;
      //settings.rssiTriggerLevel=RSSI_MAX_VALUE;
    }
    else {
      memset(gainOffset, 0, sizeof(gainOffset));
      isNormalizationApplied = false;
    }
    AutoTriggerLevel(); //Robby69
    RelaunchScan();
  }



typedef struct {
    // Block 1 (0x1D10 - 0x1D1F)
    uint8_t DelayRssi;
    uint8_t RandomEmission; 
    uint8_t BPRssiTriggerLevel[32]; // 32 bytes of trigger levels
    uint32_t bandListFlags;         // Bits 0-31: bandEnabled[0..31]
    uint16_t scanListFlags;          // Bits 0-14: scanListEnabled[0..14]
    uint8_t rssiTriggerLevel;
    uint8_t rssiTriggerLevelH;
    int8_t dbMax;
    uint32_t RangeStart;
    uint32_t RangeStop;
} SettingsEEPROM;


void LoadSettings()
{
  SettingsEEPROM  eepromData  = {0};
  
  // Lecture de toutes les données
  EEPROM_ReadBuffer(0x1D10, &eepromData, sizeof(eepromData));
  for (int i = 0; i < 15; i++) {settings.scanListEnabled[i] = (eepromData.scanListFlags >> i) & 0x01;}
  settings.rssiTriggerLevel = eepromData.rssiTriggerLevel;
  settings.rssiTriggerLevelH = eepromData.rssiTriggerLevelH;
  if (gScanRangeStart ==0) //load only if not set
    {gScanRangeStart = eepromData.RangeStart;
    gScanRangeStop = eepromData.RangeStop;}
  settings.dbMax = eepromData.dbMax;
  
    for (int i = 0; i < 32; i++) {BPRssiTriggerLevel[i] = eepromData.BPRssiTriggerLevel[i];}
  for (int i = 0; i < 32; i++) {settings.bandEnabled[i] = (eepromData.bandListFlags >> i) & 0x01;}
    DelayRssi = eepromData.DelayRssi;
    if (DelayRssi > 12) DelayRssi =12;
    RandomEmission = eepromData.RandomEmission;
    if (RandomEmission > 1) RandomEmission =0;
    
    validScanListCount = 0;
    ChannelAttributes_t att;
    for (int i = 0; i < 200; i++) {
      att = gMR_ChannelAttributes[i];
      if (att.scanlist > validScanListCount) {validScanListCount = att.scanlist;}
    }

  }

void SaveSettings() 
{
  SettingsEEPROM  eepromData  = {0};
  for (int i = 0; i < 15; i++) {if (settings.scanListEnabled[i]) eepromData.scanListFlags |= (1 << i);}
  eepromData.rssiTriggerLevel = settings.rssiTriggerLevel;
  eepromData.rssiTriggerLevelH =settings.rssiTriggerLevelH;
  eepromData.RangeStart = gScanRangeStart;
  eepromData.RangeStop = gScanRangeStop;
  eepromData.dbMax = settings.dbMax;
  eepromData.DelayRssi = DelayRssi;
  eepromData.RandomEmission = RandomEmission;
  for (int i = 0; i < 32; i++) { eepromData.BPRssiTriggerLevel[i] = BPRssiTriggerLevel[i];}
  for (int i = 0; i < 32; i++) {if (settings.bandEnabled[i]) eepromData.bandListFlags |= (1 << i);}
  // Write in 8-byte chunks
  for (uint16_t addr = 0; addr < sizeof(eepromData); addr += 8) 
    EEPROM_WriteBuffer(addr + 0x1D10, ((uint8_t*)&eepromData) + addr, 8);
  saved_params= true;
}


// Helper functions for history list
static uint8_t CountValidHistoryItems() {
    uint8_t count = 0;
    for (uint8_t i = 1; i <= FMaxNumb; i++) {
        if (freqHistory[i] != 0) count++;
    }
    return count;
}

static uint8_t GetHistoryRealIndex(uint8_t displayIndex) {
    uint8_t count = 0;
    for (uint8_t i = 1; i <= FMaxNumb; i++) {
        if (freqHistory[i] != 0) {
            if (count == displayIndex) return i;
            count++;
        }
    }
    return 1; // Fallback
}

// Fonction pour afficher un item ScanList
#define MAX_VALID_SCANLISTS 15

static uint8_t validScanListIndices[MAX_VALID_SCANLISTS]; // stocke les index valides


static bool GetScanListLabel(uint8_t scanListIndex, char* bufferOut) {
    ChannelAttributes_t att;
    char channel_name[10];
    for (int i = 0; i < 200; i++) {
      att = gMR_ChannelAttributes[i];
      if (att.scanlist == scanListIndex+1) {
            //validScanListCount++;
            SETTINGS_FetchChannelName(channel_name,i);
            sprintf(bufferOut, "%2d: %s %s", scanListIndex + 1, channel_name,settings.scanListEnabled[scanListIndex] ? "*" : " ");
            return true;
        }
    }
    return false; // Aucun canal associé à cette scanlist
}


static void BuildValidScanListIndices() {
    uint8_t ScanListCount = 0;
    for (uint8_t i = 0; i < 15; i++) {
        char tempName[17];
        if (GetScanListLabel(i, tempName)) {
            validScanListIndices[ScanListCount++] = i;
        }
    }
}


static void GetFilteredScanListText(uint8_t displayIndex, char* buffer) {
    uint8_t realIndex = validScanListIndices[displayIndex];
    GetScanListLabel(realIndex, buffer);
}

static void GetParametersText(uint8_t index, char *buffer) {
  if (index == 0) sprintf(buffer, "RSSI Delay : %d", DelayRssi);
  if (index == 1) sprintf(buffer, "Rnd Emit : %d", RandomEmission);
 }
  



//  skrócenia dla GetBandItemText
static void GetBandItemText(uint8_t index, char* buffer) {
    
    sprintf(buffer, "%d:%-12s%s", 
            index + 1, 
            BParams[index].BandName,
            settings.bandEnabled[index] ? "*" : "");
}



static void GetHistoryItemText(uint8_t index, char* buffer) {
    uint8_t realIndex = GetHistoryRealIndex(index);
    uint32_t frequency = freqHistory[realIndex];
    int channel = BOARD_gMR_fetchChannel(frequency);
    
    // Format frequency string with potential trailing zeros
    char freqStr[16];
    sprintf(freqStr, "%3u.%05u", frequency / 100000, frequency % 100000);
    // Remove trailing zeros and optional decimal point
    //RemoveTrailZeros(freqStr);
    
    if (channel != -1) {
        sprintf(buffer, "%2d:%-8s(%u) ", 
                realIndex, 
                gMR_ChannelFrequencyAttributes[channel].Name,
                freqCount[realIndex]);
                
    } else {
        sprintf(buffer, "%2d:%-8s(%u)", 
                realIndex,
                freqStr,
                freqCount[realIndex]);
    }
}

static void RenderList(const char* title, uint8_t numItems, uint8_t selectedIndex, uint8_t scrollOffset,
                      void (*getItemText)(uint8_t index, char* buffer)) {
    // Clear display buffer
    memset(gFrameBuffer, 0, sizeof(gFrameBuffer));
    
    // Draw title - wyrównany do lewej dla maksymalnego wykorzystania miejsca
    UI_PrintStringSmallBold(title, 1, LCD_WIDTH - 1, 0);
    // List parameters for UI_PrintStringSmall (lines 1-7 available)
    const uint8_t FIRST_ITEM_LINE = 1;  // Start from line 1 (line 0 is title)
    const uint8_t MAX_LINES = 6;        // Lines 1-7 available for items
    
    // Adjust scroll offset if needed
    if (numItems <= MAX_LINES) {
        scrollOffset = 0;
    } else if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + MAX_LINES) {
        scrollOffset = selectedIndex - MAX_LINES + 1;
    }
    
    // Maksymalna liczba znaków na linię (128 pikseli / 7 pikseli na znak = ~18)
    const uint8_t MAX_CHARS_PER_LINE = 18;
    // Draw visible items
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        uint8_t itemIndex = i + scrollOffset;
        if (itemIndex >= numItems) break;
        
        char itemText[32];
        getItemText(itemIndex, itemText);
        
        uint8_t lineNumber = FIRST_ITEM_LINE + i;
        
        // Wyrównanie maksymalnie do lewej
        if (itemIndex == selectedIndex) {
            // Zaznaczony element - ">" + tekst, maksymalnie wykorzystaj przestrzeń
            char displayText[MAX_CHARS_PER_LINE + 1];
            strcpy(displayText, itemText);
            char selectedText[MAX_CHARS_PER_LINE + 2];
            sprintf(selectedText, ">%2s", displayText);
            UI_PrintStringSmall(selectedText, 1, 0, lineNumber);
            
        } else {
            char displayText[MAX_CHARS_PER_LINE + 1];
            strcpy(displayText, itemText);
            UI_PrintStringSmall(displayText, 1, 0, lineNumber); // Minimalne wcięcie
          }
          
    }
    
    ST7565_BlitFullScreen();
}



// Wrapper functions for original calls

// Fonction pour afficher le menu ScanList
static void RenderScanListSelect() {
    BuildValidScanListIndices(); 
    RenderList("SCANLISTS:", validScanListCount,scanListSelectedIndex, scanListScrollOffset, GetFilteredScanListText);
}

static void RenderParametersSelect() {
  RenderList("PARAMETERS:", PARAMETER_COUNT,parametersSelectedIndex, parametersScrollOffset, GetParametersText);
}

static void RenderBandSelect() {RenderList("BANDS:", ARRAY_SIZE(BParams),bandListSelectedIndex, bandListScrollOffset, GetBandItemText);}

static void RenderHistoryList() {
    uint8_t validItems = CountValidHistoryItems();
    
    char headerString[24];
    sprintf(headerString, "HISTORY: %d", validItems);
    
    RenderList(headerString, validItems, 
              historyListIndex, historyScrollOffset, GetHistoryItemText);
}


