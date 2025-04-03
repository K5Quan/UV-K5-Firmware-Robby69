#ifndef APP_CHFRSCANNER_H
#define APP_CHFRSCANNER_H

#include <stdbool.h>
#include <stdint.h>

// scan direction, if not equal SCAN_OFF indicates 
// that we are in a process of scanning channels/frequencies
extern int8_t            gScanStateDir;
extern bool              gScanKeepResult;
extern bool              gScanPauseMode;

#ifdef ENABLE_SCAN_RANGES
extern uint32_t          gScanRangeStart;
extern uint32_t          gScanRangeStop;
#endif

#ifdef ENABLE_SCANNER
void CHFRSCANNER_Found(void);
void CHFRSCANNER_Stop(void);
void CHFRSCANNER_Start(const bool storeBackupSettings, const int8_t scan_direction);
void CHFRSCANNER_ContinueScanning(void);
#endif
#endif

typedef enum {
	SCAN_NEXT_CHAN_SCANLIST_ALL = 0,
	SCAN_NEXT_CHAN_SCANLIST1,
	SCAN_NEXT_CHAN_SCANLIST2,
	SCAN_NEXT_CHAN_SCANLIST3,
	SCAN_NEXT_CHAN_SCANLIST4,
	SCAN_NEXT_CHAN_SCANLIST5,
	SCAN_NEXT_CHAN_SCANLIST6,
	SCAN_NEXT_CHAN_SCANLIST7,
	SCAN_NEXT_CHAN_SCANLIST8,
	SCAN_NEXT_CHAN_SCANLIST9,
	SCAN_NEXT_CHAN_SCANLIST10,
	SCAN_NEXT_CHAN_SCANLIST11,
	SCAN_NEXT_CHAN_SCANLIST12,
	SCAN_NEXT_CHAN_SCANLIST13,
	SCAN_NEXT_CHAN_SCANLIST14,
	SCAN_NEXT_CHAN_SCANLIST15,
	SCAN_NEXT_CHAN_DUAL_WATCH,
	SCAN_NEXT_CHAN_MR,
	SCAN_NEXT_NUM
} scan_next_chan_t;
