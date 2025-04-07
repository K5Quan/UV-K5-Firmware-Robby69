# Firmware Quansheng UV-K5 - Robby69 
<h1><a href="https://github-com.translate.goog/Robby69400/UV-K5-Firmware-Robby69/blob/master/README.md?_x_tr_sl=fr&_x_tr_tl=en&_x_tr_hl=fr&_x_tr_pto=wapp" rel="nofollow">ENGLISH HERE</a></h1>
<h2><a href="https://kamilsss655.github.io/uvtools/?firmwareURL=https://github.com/Robby69400/UV-K5-Firmware-Robby69/blob/master/compiled-firmware/firmware.packed.bin" rel="nofollow"> 🗲FLASHER AVEC CHROME🗲 </a></h2>
<h2><a href="https://github.com/Robby69400/UV-K5-Firmware-Robby69/tree/master/compiled-firmware" rel="nofollow"> Les firmwares compilés se trouvent ici</a></h2>
<h2><a href="https://github.com/Robby69400/UV-K5-Firmware-Robby69/releases/tag/DriverChirp" rel="nofollow"> Vous devez utiliser ce driver chirp : uvk5_nunu.py</a></h2>
<h2><a href="https://www.youtube.com/@robby_69400" rel="nofollow"> Quelques présentations et news sur ma chaine Youtube.</a></h2>
<h2><a href="https://t.me/k5robby69"> Ce canal Telegram pour discuter.</a></h2>

- Le développement de ce firmware est parti du firmware NUNU de NTOIVOLA. https://github.com/ntoivola/uv-k5-firmware-custom-nunu
- Il est fourni tel quel, sans support ni garantie d'aucune sorte.
- Le manuel est disponible à l'adresse : https://github.com/ntoivola/uv-k5-firmware-custom-nunu sauf mes évolutions décrites ci-dessous.
- Appui PTT alterné, on appuie pour émettre, on appuie à nouveau pour cesser d'émettre. Ce mode toggle est à affecter sur une touche raccourci (SIDE 1, SIDE 2 ou M)
- Radio Broadcast FM présente en version ultra simplifiée pour gagner de la place sans pour autant supprimer complètement cette fonction !!!
- Verrouillage de l'émission en PMR uniquement si souhaité.
- Forte réduction de l'utilisation de la mémoire, il reste 13% disponible.
-  Réduction des puissances MID et LOW – Tests en cours… Objectif : 100mW, 500mW et 5W.
	- Si vous faites des mesures je suis intéressé
- Gestion de plages de spectre pour accepter les fréquences en mémoire :
	- exemple d'utilisation: spectre de la plage CiBie
 	- La fréquence START correspond à la fréquence en mémoire, CH1 par exemple 26,965 MHz
        - La fréquence STOP est la fréquence START + l’offset défini en mémoire. si on veut régler STOP sur le CH40 (27,405 MHz) on met un offset de 440kHz
        - Le STEP est celui indiqué en mémoire.
        	- Sélectionnez la mémoire, faites un appui long sur 5,
        	- SCNRNG s’affiche avec les fréquences START/STOP (26965 et 27405 dans l'exemple), un appui sur F+5 lance le scan sur cette plage.
- Modification du comportement du spectre, déverrouillage plus rapide : SQUELCH_OFF_DELAY 100ms.
- Optimisation réglage AGC
- Suppression de l'utilisation DTMF pour gagner de la place.
- Suppression du scanner pour gagner de la place et spectre 5x plus rapide.
- Simplification du menu: les éléments suivants sont dans le menu masqués
	- F Lock, ScraEn, Scramb, Compnd, ChDele, ChName, F1Shrt, F1Long, F2Shrt, F2Long, M Long, KeyLck, TxTOut, BatSav, Mic, ChDisp, POnMsg, BatTxt, BackLt, BLMin, BLMax, BltTRX, Beep, SqTone, 1 Call, FrCali, BatCal, BatTyp, Reset VFO.
 	- Pour les retrouver, allumer le K5 en appuyant PTT et bouton I sous le PTT
- Affichage des codes CTCSS et DCS sur le spectre (fonctionnement pour la première moitié des codes CTSS).
- La fréquence sélectionnée est copiée dans le VFO lors de la sortie du spectre.
- Une fréquence enregistrée en mémoire s’affiche avec son nom sur le spectre.
- 15 SCANLIST utilisables dans le spectre. Limite à 5 dans le scanner, fonctionnement décrit par NUNU.
- Ajout de l’historique des fréquences :
	- Lorsqu’un signal est détecté au-dessus de la barre d’historique (en pointillés), il est enregistré dans un tableau.
        	- Utilisez les touches haut/bas pour naviguer dans le tableau.
	- Deux barres de squelch :
        	- Une pour le niveau d’historique,
        	- Une pour le déclenchement audio.
        		- Sélection des barres avec le bouton II sous le PTT :
            		- Par défaut, les 2 barres sont déplacées ensemble,
            		- Un appui : seule la barre d’historique se déplace,
            		- Un second appui : seule la barre audio se déplace.

<h2><a href="https://github.com/Robby69400/UV-K5-Firmware-Robby69/commits/master/"> Historique des versions</a></h2>
