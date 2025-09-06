
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/ui.h"

void COMMON_KeypadLockToggle() 
{

    if (gScreenToDisplay != DISPLAY_MENU &&
        gCurrentFunction != FUNCTION_TRANSMIT)
    {	// toggle the keyboad lock

        gEeprom.KEY_LOCK = !gEeprom.KEY_LOCK;

        gRequestSaveSettings = true;
    }
}

void COMMON_SwitchToVFOMode()
{
    gEeprom.ScreenChannel[0] = gEeprom.FreqChannel[0];
    gRequestSaveVFO     = true;
    gVfoConfigureMode   = VFO_CONFIGURE_RELOAD;
    return;
}
void COMMON_SwitchToChannelMode()
{
    uint8_t Channel = RADIO_FindNextChannel(gEeprom.MrChannel[0], 1, false, 0);
    if (Channel != 0xFF)
    {	// swap to channel mode
        gEeprom.ScreenChannel[0] = Channel;
        gRequestSaveVFO     = true;
        gVfoConfigureMode   = VFO_CONFIGURE_RELOAD;
        return;
    }
}

void COMMON_SwitchVFOMode()
{
    if (gEeprom.VFO_OPEN)
    {
        if (IS_MR_CHANNEL(gTxVfo->CHANNEL_SAVE))
        {	// swap to frequency mode
            COMMON_SwitchToVFOMode();
        }
        else
        {
            //swap to channel mode
            COMMON_SwitchToChannelMode();
        }
    }
}