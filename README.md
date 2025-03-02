##  ENGLISH SEE BELOW !

# Firmware Quansheng UV-K5 par Robby69

## Ce firmware est un fork du firmware NUNU de NTOIVOLA.
## Ce firmware est fourni tel quel, sans support ni garantie d'aucune sorte. Je l'ai développé pour mon usage personnel, mais j'espère qu'il sera utile à d'autres.
## Les questions et commentaires sont les bienvenus, mais je ne fais aucune promesse.
## Le manuel est disponible à l'adresse : https://github.com/ntoivola/uv-k5-firmware-custom-nunu.
Évolutions apportées dans mon firmware par rapport au firmware NUNU de NTOIVOLA :

> [!warning]
> Toutes les fréquences sont ouvertes à la transmission pour libérer de l’espace, vous êtes responsable de l'utilisation.
    
    Réduction des puissances MID et LOW – Tests en cours… Objectif : 100mW, 500mW et 5W.
    Modification de la plage de scan pour accepter les fréquences en mémoire :
        La fréquence START correspond à la fréquence en mémoire,
        La fréquence STOP est la fréquence START + l’offset défini en mémoire.
        Le STEP est celui indiqué en mémoire.
        Sélectionnez la mémoire, faites un appui long sur 5,
        SCANRNG s’affiche avec les fréquences START/STOP, un appui sur F+5 lance le scan sur cette plage.
    Modification du comportement du spectre, déverrouillage plus rapide : SQUELCH_OFF_DELAY 100ms.
    Ajout de MENU_TEST_RANGE, non encore implémenté.
    Suppression de l'utilisation DTMF, possible de l’activer sur demande.
    Éléments du menu masqués :
        F1Shrt, F1Long, F2Shrt, F2Long, M Long, KeyLck, TxTOut, BatSav, Mic, ChDisp, POnMsg, BatTxt, BackLt, BLMin, BLMax, BltTRX, Beep, Voice, D Live, SqTone, 1 Call, FrCali, BatCal, BatTyp.

Évolutions du spectre :

    Affichage des codes CTCSS et DCS sur le spectre.
    Ajout de l’historique des fréquences :
        Lorsqu’un signal est détecté au-dessus de la barre d’historique, il est enregistré dans un tableau.
        Utilisez les touches haut/bas pour naviguer dans le tableau.
        La fréquence sélectionnée est copiée dans le VFO lors de la sortie.
        Une fréquence enregistrée en mémoire s’affiche avec son nom.
        Deux barres de squelch :
            Une pour le niveau d’historique,
            Une pour le déclenchement audio.
        Sélection des barres avec le bouton II (sous le PTT) :
            Par défaut, les 2 barres sont déplacées ensemble,
            Un appui : seule la barre d’historique se déplace,
            Un second appui : seule la barre audio se déplace.

Plage de scan avec offset - Enregistrement des fréquences START/STOP en mémoire :

    En mode VFO, un appui long sur 5 affiche SCNRNG avec les deux fréquences.
    En mode mémoire, un appui long sur 5 affiche SCNRNG avec :
        La fréquence enregistrée en mémoire comme fréquence START,
        Cette même fréquence + l’offset comme fréquence STOP.

   
   > [!Warning]
> vous devez utiliser le driver [UV-K5 CHIRP driver](https://github.com/ntoivola/uvk5-chirp-driver-nunu/) avec ce firmware

# Firmware Quansheng UV-K5 by Robby69

This source is a fork of [NTOIVOLA's NUNU firmware]([https://github.com/kamilsss655/uv-k5-firmware-custom])

## This firmware is provided as is, I offer no support and no warranty of any kind. I made it for myself but I hope someone will find it useful. 
## Questions, comments are welcome, but no promises are made.

## The manual is available at https://github.com/ntoivola/uv-k5-firmware-custom-nunu

Evolutions made in my firmware compared to NTOIVOLA's NUNU firmware:

* FLOCK removed - all frequencies are open for transmission to free up space, but you are responsible for usage.
* MID and LOW powers reduced Testing in progress... Trying to target 100mW, 500mW and 5W.
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
