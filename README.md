# firmware Quansheng UV-K5 V3b10

Ce source est un fork du [firmware NUNU de NTOIVOLA]([https://github.com/kamilsss655/uv-k5-firmware-custom])

** Ce firmware est fourni tel quel, je n'offre aucune assistance et aucune garantie d'aucune sorte. Je l'ai fait pour moi-même mais j'espère que quelqu'un le trouvera utile. Les questions, commentaires et PR sont les bienvenus, mais aucune promesse n'est faite.**

Mes modifications par rapport au [firmware NUNU de NTOIVOLA:]

* **FLOCK supprimé - toutes les fréquences sont ouvertes en émission** pour libérer de l'espace, mais vous êtes responsables de l'utilisation.
* **Puissances MID et LOW réduites** Tests en cours...
* **SCAN RANGE modifié pour accepter des fréquences de mémoire:**
    * la fréquence START est la fréquence en mémoire,
    * la fréquence STOP est la fréquence START+ l'offset définit en mémoire.
    * Le STEP est celui indiqué en mémoire.
    * On sélectionne la mémoire, on fait un appui long sur 5, cela affiche SCANRNG et les fréquences START/STOP, un F+5 lance le spectre sur cette plage.

> [!TIP]
> **REMARQUE !** Vous devez utiliser [le pilote UV-K5 CHIRP](https://github.com/ntoivola/uvk5-chirp-driver-nunu/) avec ce firmware.

## Credits
I have strated this work based on NTOIVOLA's repo, I have only added small changes.

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
