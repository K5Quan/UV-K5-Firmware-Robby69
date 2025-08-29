# Firmware Quansheng UV-K5 - Robzyl
## The software is in English, and the available versions correspond to the target countries for the bands: France, Poland, and Romania. These bands can be customized; see the manual. 
## Le logiciel est en anglais, les versions disponibles correspondent aux pays cibles pour les bandes: France, Pologne, Roumanie. Ces bandes peuvent se personaliser, voir le manuel.
### Many thanks to Zylka, Yves31 and Francois87.
<h2><a href="https://egzumer.github.io/uvtools/?firmwareURL=https://github.com/Robby69400/UV-K5-Firmware-Robby69/releases/download/V5.2/robzyl.en.fr.packed.bin" rel="nofollow"> 🗲FLASH WITH CHROME WITH FRANCE FREQUENCIES
</a> <a href="https://egzumer.github.io/uvtools/?firmwareURL=https://github.com/Robby69400/UV-K5-Firmware-Robby69/releases/download/V5.2/robzyl.en.pl.packed.bin" rel="nofollow">    🗲POLAND FREQUENCIES🗲 </a></h2>
<h2><a href="https://github.com/Robby69400/UV-K5-Firmware-Robby69/tree/master/Manuals" rel="nofollow">🗲Manuals</a>
<a href="https://github.com/Robby69400/UV-K5-Firmware-Robby69/blob/master/Chirp/uvk5_Robby69.py" rel="nofollow">🗲Chirp driver </a>
<a href="https://www.youtube.com/@robby_69400" rel="nofollow">🗲Youtube</a> <a href="https://t.me/k5robby69">🗲Telegram🗲.</a> </h2>


Manuel Robzyl V5.2

Firmware pour radio Quansheng UV-K5

# Introduction

Ce firmware, fork de NUNU de NTOIVOLA, est caractérisé par ses multiples fonctions de réception mettant en œuvre l’analyseur de spectre capable de traiter jusqu’à 160 canaux par seconde.

Les liens vers les différentes ressources sont accessibles en fin de document (GitHub, Youtube, Telegram, etc.).

# Avertissements et responsabilités

**Le domaine de la radio est réglementé, chacun est responsable de l’utilisation qu’il fait de sa radio.**

# Nouveautés V5.2

- Passage en simple VFO (gain de place et simplification interface)
- Spectre : Nouvelle gestion du squelch, ajout nouvel écran sans histogramme, nouveaux menus PTT, FStart/Stop, Step, ListenBw et Modulation).
- Spectre : Historique des fréquences à repenser suite aux travaux sur le squelch.
- Nouveaux Roger Bips 😊

# Guide d’utilisation

- **Installation du firmware :**
- Télécharger la dernière version sur le GitHub (lien en fin de doc).
- Munissez-vous du câble de programmation USB compatible avec le poste.
- Brancher le poste à l’ordinateur puis démarrer le K5 tout en appuyant sur le bouton PTT
- Puis, led allumée fixe, transférer le firmware vers le K5 via le Flasher en ligne ou K5prog-win (lien en fin de doc).
- Si vous vous apprêtez à remplacer le firmware d’usine, il est recommandé de sauvegarder préalablement vos configuration et calibration à l’aide de K5prog (voir par exemple la vidéo de F5SVP)
- **Prise en main rapide :**
- Les menus cachés : les menus peu utilisés ont été cachés dans une optique de simplification. Pour afficher le menu complet, il suffit de démarrer le poste en pressant PTT + SIDE KEY 1
- La programmation avec Chirp : le driver à utiliser pour dialoguer avec le poste sous Robzyl est à télécharger (lien en fin de doc). Attention de ne pas être en mode spectre pour pouvoir communiquer avec le PC.
- Restauration du dernier état : suite à l’arrêt du K5, son redémarrage reprend dans le mode actif à son extinction en tenant compte de vos derniers paramètres de spectre sauvegardés.
- Les principales fonctionnalités propres au firmware Robzyl sont décrites dans la suite de ce document. Pour les fonctions de base du K5, veuillez vous reporter à sa documentation.
- **Les modes VFO et Mémoire :**

Ces modes sont accessibles alternativement par un appui long sur la touche 3.

Mode VFO

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/c5bbe10b-27c3-46da-8e40-a````ba019d2d7d9" />


Le mode simple VFO permet de saisir librement une fréquence. Le menu touche M donne accès à tous les paramètres de step, modulation, etc.

Mode Mémoire

<img width="801" height="501" alt="image" src="https://github.com/user-attachments/assets/d3ef5d7f-a807-4002-85fa-b3c2bca3ba8f" />


Cet autre mode permet de naviguer dans la banque des 200 mémoires nommées du K5. Cette banque est à préparer et à injecter dans le K5 depuis Chirp.

- **Le mode spectre :**
Fonctionnalités communes du mode spectre :````


Ecran principal :
<img width="1206" height="613" alt="image" src="https://github.com/user-attachments/assets/14f1d3ff-61a0-47ee-8340-b587c4b50c1d" />


- Ligne 1 :
- Type de spectre : SL (Scan Listes), FR (Plage de fréquences), BD (Bandes)
- Paramètres du squelch trigger UP Uxxx (valeur de déclenchement sur signal montant), et trigger Down Dxxx (valeur d’invalidation d’un signal descendant expl)
- Délai de capture du RSSI d’un signal de 0 à 12 ms. Permet d’accélérer la vitesse de scan, mais cela réduit le rapport signal sur bruit.
- Modulation courant FM/AM/USB
- Ligne 2 : Fréquence en cours et CTCSS/DCS. Affichage pouvant varier selon le type de spectre choisi.
- Corps : Représentation graphique et dynamique des canaux analysés et leur niveau de signal.
- Ligne 3 : Etendues en cours et info complémentaire : BL (une blacklist de fréquences est en cours).

Le menu des paramètres :

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/c074bf52-fc97-4e35-a235-e2e937ef9b53" />



- RSSI Delay : temps de capture du RSSI en ms. Une valeur trop faible peut faire rater des signaux.
- SpectrumDelay : Permet de définir le temps d’attente sur un signal à l’écoute et retombé sous le squelch. Si la valeur est à l’infini : pressez la touche Exit pour quitter l’écran d’écoute.
- PTT (Option de passage en émission) : LAST RECEIVED = dernière fréquence entendue, LAST VFO FREQ = fréquence en VFO, NINJA MODE : Mode de communication expérimental par saut de fréquence à chaque PTT entre 2 K5 utilisant le spectre en mode Ninja sur une Scanlist commune. Voir vidéo sur YouTube.
- Fstart/Fstop : paramétrage des fréquence haut/bas en mode FR.
- Step : paramétrage de la canalisation des fréquences en mode FR.
- ListenBW : paramétrage de la largeur de la bande d’écoute.
- Modulation : FM/AM/USB

Spectre en vue simplifiée :

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/d32e5705-8cb4-48f5-9ddf-b78c10a4ad38" />

Cet écran offre une vue plus synthétique du scan en cours tout en permettant le réglage aisé des paramètres de squelch.

Monitoring de fréquence :

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/044f8662-8b83-47c8-b008-beef9edc87c4" />


Le monitor se lance sur une fréquence à l’écoute, soit manuellement avec la touche M soit sous l’effet du SpectrumDelay.

Les touches :

- Touche 1 : Passer une fréquence à l’écouter (« skip »)
- Touche 2 : Passage vers l’écran simplifié du spectre
- Touche 5 : Accès au Menu, puis Haut/bas pour naviguer, 1/3 pour changer des valeurs, 1/M pour saisir Fstart/Fstop.
- Touche 7 : Sauvegarde des principaux paramètres
- Touche M : Passage en Monitoring sur une fréquence
- SIDE KEY 1 : Blacklister une fréquence à l’écouter
- SIDE KEY 2 : Désactiver l’auto-ajustement du trigger D
- Touche 3/9 : Réglage squelch paramètre Uxxx
- Touche \*/F : Réglage squelch paramètre Dxxx (uniquement si SIDE KEY 2 a été activée)

Conseils :

- Les valeurs de squelch dépendent de votre environnement, de votre antenne et de votre choix de délai RSSI.
- RSSI Delay : 4-5 ms donnent des résultats très corrects.
- Trigger Up Uxxx : commencer à 10 et ajuster jusqu’à ne plus recevoir de bruits mais bien de la modulation.
- Trigger Down Dxxx : doit être positionné au-dessus du bruit de fond
  - **Spectre sur les ScanLists (mode SL) :**
- Fonction : Permet de charger dans le spectre les mémoires affectées à des scanlists.
- Lancement : Depuis le mode VFO/MR, touche F+4
- Utilisation et Conseils :
- Préalablement les fréquences en mémoires doivent avoir été affectées à une scanlist (ex. SL1 = PMR, SL2 = Répéteurs, SL3=Aéro, etc.)
- A la première utilisation, passer dans chaque SL pour ajuster les paramètres de squelch U et éventuellement D puis mémoriser vos valeurs avec la touche 7
- Enfin charger vos SL dans le spectre via le menu de sélection en touche 4.

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/a331c9c9-0ce7-4b88-8b66-166b5007c546" />


On navigue dans ce menu avec les touches haut/bas.

Touche 5 : choisir une SL en excluant les autres

Touche 4 : choisir/invalider une ou plusieurs SL

Touche \* : affichages des mémoires affectées à la SL sélectionnée

Les SL choisies apparaissent avec un symbole \*. Puis faire Exit pour lancer le spectre. Touche 7 pour enregistrer sa configuration.

- - **Spectre sur plage de Fréquences (mode FR) :**
- Fonction : Permet d’analyser une gamme de fréquence à partir d’une fréquence centrale ou bien à partir d’une étendue définie
- Lancement : Depuis le mode VFO/MR, touche F+5
- Utilisation et Conseils :
- La fréquence issue du VOF/MR est portée au spectre en tant que fréquence centrale. Ensuite, vos pouvez agir sur le paramétrage de votre spectre selon vos besoins en step, modulation, etc. Réglages touche 5.
- L’entendue des fréquences basse/haute peut être ajustée dans le menu via les paramètres FStart/FStop. Sur ces paramètres faire 1 pour accéder à la saisie et M pour valider (touche \* pour la virgule).
- Ajuster votre squelch.
  - **Spectre sur les Bandes Prédéfinis (mode BD) :**
- Fonction : Permet d’analyser en spectre des bandes prédéfinies (ex. PMR, CB, AERO, HAM, etc.).
- Lancement : Depuis le mode VFO/MR, touche F+6
- Utilisation et Conseils :
  - - Les bandes sont stockées dans un fichier bands.h personnalisable avec recompilation du firmware (procédure en lien en fin de doc).
      - Il est possible de paramétrer 32 bandes.

Exemple de fichier de configuration :
<img width="1181" height="259" alt="image" src="https://github.com/user-attachments/assets/208477f1-229c-46ff-8638-7c9fedabff48" />


De la même manière qu’en mode SL, il est demandé à la 1ère utilisation de paramétrer les valeurs de squelch sur les bandes qui vous intéressent. Touches haut/bas pour naviguer dans les bandes.

Ensuite le menu touche 4 permet de choisir les bandes à analyser de la même manière que le menu en mode SL :

<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/7d25a788-6194-49dc-bc84-3292151d03a9" />


# FAQ

- Est-il possible de verrouiller son K5 en bande PMR uniquement ? :

Oui : Affichage menus cachés, menu No 48, valeur PMR446 ONLY.

- Le firmware est-il compatible avec les mod SI4732 ? :

Non, mais ce sera peut-être envisageable.

- Le firmware est-il compatible avec les mod EEPROM ? :

Non, mais c’est une évolution possible.

# Ressources et liens utiles

Youtube : <https://www.youtube.com/@robby_69400>.

Github avec Chrome flasher : <https://github.com/Robby69400/UV-K5-Firmware-Robby69>

Telegram Robzyl Dev : <https://t.me/k5robby69>

Driver chirp : <https://github.com/Robby69400/UV-K5-Firmware-Robby69/blob/master/Chirp/uvk5_Robby69.py>

Procédure de recompilation : [https://github.c](https://github.com/Robby69400/UV-K5-Firmware-Robby69?tab=readme-ov-file#méthode-de-compilation-avec-github-codespace-pour-personaliser-les-scan-bands)om/Robby69400/UV-K5-Firmware-Robby69?tab=readme-ov-file#m%C3%A9thode-de-compilation-avec-github-codespace-pour-personaliser-les-scan-bands

