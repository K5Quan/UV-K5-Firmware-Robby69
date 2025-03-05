## EN COURS DE REDACTION
Firmware uses 87% of available memory ;)
# Firmware Quansheng UV-K5 par Robby69 

Ce firmware est un fork du firmware NUNU de NTOIVOLA.
IL est fourni tel quel, sans support ni garantie d'aucune sorte.
Le manuel est disponible à l'adresse : https://github.com/ntoivola/uv-k5-firmware-custom-nunu.
Évolutions apportées dans mon firmware par rapport au firmware NUNU de NTOIVOLA :

> [!warning]
> Toutes les fréquences sont ouvertes à la transmission pour libérer de l’espace, vous êtes responsable de l'utilisation.
    
- Réduction des puissances MID et LOW – Tests en cours… Objectif : 100mW, 500mW et 5W.
- Modification de la plage de scan pour accepter les fréquences en mémoire :
	- La fréquence START correspond à la fréquence en mémoire,
        - La fréquence STOP est la fréquence START + l’offset défini en mémoire.
        - Le STEP est celui indiqué en mémoire.
        - Sélectionnez la mémoire, faites un appui long sur 5,
        - SCANRNG s’affiche avec les fréquences START/STOP, un appui sur F+5 lance le scan sur cette plage.
- Modification du comportement du spectre, déverrouillage plus rapide : SQUELCH_OFF_DELAY 100ms.
- Suppression de l'utilisation DTMF, possible de l’activer sur demande.
- Éléments du menu masqués :
	- F1Shrt, F1Long, F2Shrt, F2Long, M Long, KeyLck, TxTOut, BatSav, Mic, ChDisp, POnMsg, BatTxt, BackLt, BLMin, BLMax, BltTRX, Beep, Voice, D Live, SqTone, 1 Call, FrCali, BatCal, BatTyp.

# Évolutions du spectre :

- Affichage des codes CTCSS et DCS sur le spectre.
- La fréquence sélectionnée est copiée dans le VFO lors de la sortie.
- Une fréquence enregistrée en mémoire s’affiche avec son nom.
- Ajout de l’historique des fréquences :
	- Lorsqu’un signal est détecté au-dessus de la barre d’historique, il est enregistré dans un tableau.
        	- Utilisez les touches haut/bas pour naviguer dans le tableau.

	- Deux barres de squelch :
        	- Une pour le niveau d’historique,
        	- Une pour le déclenchement audio.
        		- Sélection des barres avec le bouton II (sous le PTT) :
            		- Par défaut, les 2 barres sont déplacées ensemble,
            		- Un appui : seule la barre d’historique se déplace,
            		- Un second appui : seule la barre audio se déplace.

- Plage de scan avec offset - Enregistrement des fréquences START/STOP en mémoire :
- En mode VFO, un appui long sur 5 affiche SCNRNG avec les deux fréquences.
- En mode mémoire, un appui long sur 5 affiche SCNRNG avec :
	- La fréquence enregistrée en mémoire comme fréquence START,
	- Cette même fréquence + l’offset comme fréquence STOP.

   
   > [!Warning]
> vous devez utiliser le driver [UV-K5 CHIRP driver](https://github.com/ntoivola/uvk5-chirp-driver-nunu/) avec ce firmware



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
