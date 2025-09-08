#ifdef ENABLE_FR_BAND
static const bandparameters BParams[32] = {
    // BandName       Startfrequency    Stopfrequency   scanStep          modulationType
    {"AIR 8.33k",     11800000,         13700000,       S_STEP_8_33kHz,   MODULATION_AM},
    {"AIR MIL1",      22500000,         24107500,       S_STEP_25_0kHz,   MODULATION_AM},
    {"AIR MIL2",      33540000,         33970000,       S_STEP_25_0kHz,   MODULATION_AM},
    {"PMR 446",       44600625,         44619375,       S_STEP_12_5kHz,   MODULATION_FM},
    {"PMR 446b",      44600000,         44620000,       S_STEP_6_25kHz,   MODULATION_FM},
    {"FREENET",       14902500,         14911250,       S_STEP_12_5kHz,   MODULATION_FM},
    {"DMR VHF",       14600000,         17000000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"DMR UHF1",      45000000,         46600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"DMR UHF2",      46620000,         47000000,       S_STEP_12_5kHz,   MODULATION_FM}, //Pocsag FR removed 466-466.2
    {"REMOTE CT",     43200000,         43400000,       S_STEP_0_5kHz,    MODULATION_FM}, //Remote control search
    {"MARINE",        15550000,         16202500,       S_STEP_25_0kHz,   MODULATION_FM},
    {"CB40",           2696500,          2740500,       S_STEP_10_0kHz,   MODULATION_AM},
    {"CB",             2651500,          2830500,       S_STEP_5_0kHz,    MODULATION_AM},
    {"SRD868",        86800000,         87000000,       S_STEP_6_25kHz,   MODULATION_FM},
    {"LPD433",        43307500,         43377500,       S_STEP_6_25kHz,   MODULATION_FM},
    {"14MHz",          1400000,          1430000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"17MHz",          1740000,          1780000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"18MHz",          1806800,          1816800,       S_STEP_1_0kHz,    MODULATION_AM},
    {"21MHz",          2100000,          2145000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"24MHz",          2489000,          2499000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"HAM 28MHz",      2800000,          2970000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"HAM 50MHz",      5000000,          5200000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"70 Mhz",         7000000,          7050000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"HAM 144M",      14400000,         14600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"220Mhz",        22000000,         22500000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"HAM 430M",      43000000,         44000000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"1240MHz",      124000000,        130000000,       S_STEP_25_0kHz,   MODULATION_FM},
    {"SATCOM",        24000000,         27500000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"AIR 25k",       11800000,         13700000,       S_STEP_25_0kHz,   MODULATION_AM},
    {"NOT USED",       1400000,        130000000,        S_STEP_500kHz,   MODULATION_FM},
    {"NOT USED",       1400000,        130000000,        S_STEP_100kHz,   MODULATION_FM},
    {"NOT USED",      40000000,         50000000,        S_STEP_100kHz,   MODULATION_FM}
    }; 
#endif

#ifdef ENABLE_PL_BAND
static const bandparameters BParams[32] = {
    // BandName       Startfrequency    Stopfrequency   scanStep        modulationType
    {"HAM 144",       14400000,         14600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"HAM 430",       43000000,         44000000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"AIR 25k",       11800000,         13600000,       S_STEP_25_0kHz,   MODULATION_AM},
    {"AIR 8.33k",     11800000,         13600000,       S_STEP_8_33kHz,   MODULATION_AM},
    {"DMR-VHF",       14600000,         17000000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"DMR-UHF1",      45000000,         46600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"DMR-UHF2",      46620000,         47000000,       S_STEP_12_5kHz,   MODULATION_FM}, //Pocsag FR removed 466-466.2
    {"50-52MHz",       5000000,          5256000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"Remote433",     43200000,         43400000,       S_STEP_0_5kHz,    MODULATION_FM}, //Remote control search
    {"LPD433",        43307500,         43377500,       S_STEP_6_25kHz,   MODULATION_FM},
    {"SRD868",        86800000,         87000000,       S_STEP_6_25kHz,   MODULATION_FM},
    {"SATCOM",        24000000,         27500000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"14MHz",          1400000,          1430000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"17MHz",          1740000,          1780000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"18MHz",          1806800,          1816800,       S_STEP_1_0kHz,    MODULATION_AM},
    {"21MHz",          2100000,          2145000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"24MHz",          2489000,          2499000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"CB",             2651500,          2830500,       S_STEP_5_0kHz,    MODULATION_AM},
    {"28MHz",          2800000,          2970000,       S_STEP_1_0kHz,    MODULATION_AM},
    {"50-52MHz",       5000000,          5200000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"70 Mhz",         7000000,          7050000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"144 Mhz",       14400000,         14600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"220Mhz",        22000000,         22500000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"430 Mhz",       43000000,         44000000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"1240MHz",      124000000,        130000000,       S_STEP_25_0kHz,   MODULATION_FM},
    {"PKP FAST",      15050000,         15197500,       S_STEP_25_0kHz,   MODULATION_FM},
    {"PKP MAX",       15000000,         15600000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"CB PL",          2696000,          2740000,       S_STEP_10_0kHz,   MODULATION_AM},
    {"POGOTOWIE",     16852500,         16937500,       S_STEP_25_0kHz,   MODULATION_FM},
    {"POLICJA",       17200000,         17397500,       S_STEP_25_0kHz,   MODULATION_FM},
    {"PSP",           14866250,         14933750,       S_STEP_25_0kHz,   MODULATION_FM},
    {"MARINE",        15550000,         16202500,       S_STEP_25_0kHz,   MODULATION_FM}
    }; 
#endif

#ifdef ENABLE_RO_BAND
static const bandparameters BParams[32] = {
// BandName Startfrequency Stopfrequency scanStep modulationType
    {"20M",           1400000,           1435000,       S_STEP_1_0kHz,   MODULATION_AM},
    {"17M",           1806800,           1816800,       S_STEP_1_0kHz,   MODULATION_AM},
    {"15M",           2100000,           2145000,       S_STEP_1_0kHz,   MODULATION_AM},
    {"12M",           2489000,           2499000,       S_STEP_1_0kHz,   MODULATION_AM},
    {"10M",           2800000,           2970000,       S_STEP_1_0kHz,   MODULATION_AM},
    {"6M",            5000000,           5200000,       S_STEP_10_0kHz,  MODULATION_FM},
    {"2M",           14400000,          14600000,       S_STEP_12_5kHz,  MODULATION_FM},
    {"70CM",         43000000,          44000000,       S_STEP_10_0kHz,  MODULATION_FM},
    {"TAXI",         46000000,          47000000,       S_STEP_12_5kHz,  MODULATION_FM}, // Banda taxi UHF
    {"CB",            2696500,           2740500,       S_STEP_10_0kHz,  MODULATION_FM},
    {"AIR 25kHz",    11800000,          13700000,       S_STEP_25_0kHz,  MODULATION_AM},
    {"AIR 8.33kHz",  11800000,          13700000,       S_STEP_8_33kHz,  MODULATION_AM},
    {"AMBULANTA",    16920000,          16937500,       S_STEP_25_0kHz,  MODULATION_FM},
    {"POLITIA",      17200000,          17397500,       S_STEP_25_0kHz,  MODULATION_FM},
    {"POMPIERI",     14866250,          14933750,       S_STEP_25_0kHz,  MODULATION_FM}
    };          
#endif

#ifdef ENABLE_KO_BAND
static const bandparameters BParams[32] = {
    // BandName       Startfrequency    Stopfrequency   scanStep          modulationType
    {"AIR 25k",       11800000,         13600000,       S_STEP_25_0kHz,   MODULATION_AM},
    {"AIR MIL1",      22500000,         24107500,       S_STEP_25_0kHz,   MODULATION_AM},
    {"AIR MIL2",      33540000,         33970000,       S_STEP_25_0kHz,   MODULATION_AM},
    {"LPD433",        43307500,         43377500,       S_STEP_6_25kHz,   MODULATION_FM},
	{"PMR 446",       44600625,         44619375,       S_STEP_12_5kHz,   MODULATION_FM},
    {"PMR 446b",      44600000,         44620000,       S_STEP_6_25kHz,   MODULATION_FM},
    {"136-144",      13600000,         14400000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"HAM 144",      14400000,         14600000,       S_STEP_12_5kHz,   MODULATION_FM},
	{"146-150",      14400000,         14600000,       S_STEP_25_0kHz,   MODULATION_FM},
    {"HAM 430",      43000000,         44000000,       S_STEP_10_0kHz,   MODULATION_FM},
	{"REMOTE433",     43200000,         43400000,       S_STEP_5_0kHz,    MODULATION_FM}, //Remote control search
	{"RAIL",          15000000,         15550000,       S_STEP_25_0kHz,   MODULATION_FM},
    {"MARINE",        15550000,         16202500,       S_STEP_25_0kHz,   MODULATION_FM},
	{"VHF-H",         16200000,         17000000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"VHF-H1",        17000000,         17400000,       S_STEP_25_0kHz,   MODULATION_FM},
    {"17MHz",          1740000,          1780000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"18MHz",          1806800,          1816800,       S_STEP_5_0kHz,    MODULATION_AM},
    {"21MHz",          2100000,          2145000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"24MHz",          2489000,          2499000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"CB",             2651500,          2800000,       S_STEP_5_0kHz,    MODULATION_AM},
    {"HAM 28",        2800000,          2870000,       S_STEP_5_0kHz,    MODULATION_USB},
    {"HAM 50",        5000000,          5200000,       S_STEP_10_0kHz,   MODULATION_FM},
    {"70 Mhz",         7000000,          7050000,       S_STEP_12_5kHz,   MODULATION_FM},
    {"UHF400",        40000000,         42000000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"UHF420",        42000000,         43300000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"UHF440",        44000000,         45500000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"UHF455",        45500000,         47000000,       S_STEP_25_0kHz,   MODULATION_FM},
	{"FM-BROAD",      7000000,          10800000,       S_STEP_100kHz,   MODULATION_FM},
    {"SATCOM",        24000000,         27000000,       S_STEP_10_0kHz,   MODULATION_FM}
    }; 
#endif

#ifdef ENABLE_CZ_BAND
static const bandparameters BParams[32] = {
// BandName    StartFreq    StopFreq     scanStep        modulationType
{"AviCOM",    11800000,   13699160,   S_STEP_8_33kHz, MODULATION_AM},
{"AviNAV",    10800000,   11795000,   S_STEP_50_0kHz, MODULATION_AM},
{"MilAir",    13700000,   14400000,   S_STEP_25_0kHz, MODULATION_FM},
{"Ham2m",     14400000,   14600000,   S_STEP_12_5kHz, MODULATION_FM},
{"UtilVHFA",  14600000,   15742500,   S_STEP_12_5kHz, MODULATION_FM},
{"UtilVHFB",  15840000,   17398750,   S_STEP_12_5kHz, MODULATION_FM},
{"Ham6m",     5000000,    5200000,    S_STEP_25_0kHz, MODULATION_FM},
{"CtrlA",     6750000,    7298750,    S_STEP_12_5kHz, MODULATION_FM},
{"CtrlB",     7302500,    7477500,    S_STEP_25_0kHz, MODULATION_FM},
{"LowUHF",    4067500,    4098500,    S_STEP_5_0kHz,  MODULATION_FM},
{"AviBcn",    7480000,    7520000,    S_STEP_25_0kHz, MODULATION_AM},
{"UHFSAT",    23000000,   38000000,   S_STEP_25_0kHz, MODULATION_FM},
{"TETRA",     39500000,   40000000,   S_STEP_25_0kHz, MODULATION_FM},
{"WxLEO",     40000000,   40600000,   S_STEP_25_0kHz, MODULATION_FM},
{"RepLnk",    42400000,   43000000,   S_STEP_12_5kHz, MODULATION_FM},
{"Ham70cm",   43000000,   44000000,   S_STEP_12_5kHz, MODULATION_FM},
{"PubSfty",   44000000,   44800000,   S_STEP_25_0kHz, MODULATION_FM},
{"PMR446",    44600000,   44620000,   S_STEP_6_25kHz, MODULATION_FM},
{"TrkUpA",    45740000,   45850000,   S_STEP_10_0kHz, MODULATION_FM},
{"TrkUpB",    46013000,   46149000,   S_STEP_10_0kHz, MODULATION_FM},
{"TrkDn",     46571000,   46999000,   S_STEP_10_0kHz, MODULATION_FM},
{"SRD",       4067500,    4070000,    S_STEP_1_0kHz,  MODULATION_FM},
{"PubVHF",    16000000,   16050000,   S_STEP_25_0kHz, MODULATION_FM},
{"EmgUHF",    38000000,   39500000,   S_STEP_12_5kHz, MODULATION_FM},
{"TrkSrv",    46575000,   46737000,   S_STEP_10_0kHz, MODULATION_FM},
{"CivHF",     2700000,    2800000,    S_STEP_5_0kHz,  MODULATION_FM},
{"Ham40m",    700000,     750000,     S_STEP_5_0kHz,  MODULATION_FM},
{"Mar16",     15680000,   15680000,   S_STEP_25_0kHz, MODULATION_FM},
{"DigDMR",    43800000,   44000000,   S_STEP_12_5kHz, MODULATION_FM},
{"SAR",       24300000,   24500000,   S_STEP_25_0kHz, MODULATION_FM},
{"GovUHF",    44800000,   45150000,   S_STEP_10_0kHz, MODULATION_FM},
{"TrkRes",    45700000,   45999000,   S_STEP_10_0kHz, MODULATION_FM},
};          
#endif

