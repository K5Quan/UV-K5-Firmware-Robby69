# Firmware Quansheng UV-K5 by Robby69

This source is a fork of [NTOIVOLA's NUNU firmware]([https://github.com/kamilsss655/uv-k5-firmware-custom])

## This firmware is provided as is, I offer no support and no warranty of any kind. I made it for myself but I hope someone will find it useful. 
## Questions, comments are welcome, but no promises are made.

## The manual is available at https://github.com/ntoivola/uv-k5-firmware-custom-nunu

Evolutions made in my firmware compared to NTOIVOLA's NUNU firmware:

* FLOCK removed - all frequencies are open for transmission** to free up space, but you are responsible for usage.
* MID and LOW powers reduced** Testing in progress... Trying to target 100mW, 500mW and 5W.
* SCAN RANGE modified to accept memory frequencies:
* START frequency is the frequency in memory,
* STOP frequency is the START frequency + the offset defined in memory.
* The STEP is the one indicated in memory.
* We select the memory, we do a long press on 5, 
* it displays SCANRNG and the START/STOP frequencies, an F+5 launches the spectrum on this range.
* Spectrum behaviour change, faster to unlock : SQUELCH_OFF_DELAY 100ms
* MENU_TEST_RANGE added not implemented yet
* DTMF use removed, possible to activate ask me.
* menu elements hidden:
	F1Shrt, F1Long, F2Shrt, F2Long, M Long, KeyLck, TxTOut, BatSav, Mic, ChDisp, POnMsg, BatTxt, BackLt, BLMin, BLMax, BltTRX, Beep, Voice, D Live, SqTone, 1 Call, FrCali, BatCal, BatTyp

# Spectrum evolutions:

* Display CTCSS and DCS on spectrum
* Added frequency history feature:
* 	when a signal is detected above the history bar, it is recorded in a table.
*	use up down keys to look at the table.
*	The selected frequency is copied to VFO when exit.
*	a frequency found in memory is shown with it's name.
*	2 squelch bars squelch for history level and squelch for audio trigger
*	select bars with II button (below PTT)
*		default: 2 bars stick together
*		press once history bar alone
*		press again audio bar alone


## SCAN RANGE with offset - START STOP Frequency memory save.

* in VFO mode, press long 5 will display scnrng and the 2 frequencies.
* in memory mode press long 5 will display scnrng and 
*	the frequency in memory first as start frequency 
*	and this frequency + offset value as stop frequency


> [!Warning]
> You must use [the UV-K5 CHIRP driver](https://github.com/ntoivola/uvk5-chirp-driver-nunu/) with this firmware.



<details>

## License

Original work Copyright 2023 Dual Tachyon
https://github.com/DualTachyon

Modified work Copyright 2024 kamilsss655
https://github.com/kamilsss655

Modified work Copyright 2024 ntoivola
https://github.com/ntoivola

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
</details>
