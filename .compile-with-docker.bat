@echo off
cls
del .\compiled-firmware\*.bin
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 ENABLE_NINJA=0 TARGET=robzyl_V5.1beta.en_fr && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=0 ENABLE_PL_BAND=1 ENABLE_NINJA=0 TARGET=robzyl_V5.1beta.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=1 ENABLE_PL_BAND=0 ENABLE_NINJA=0 TARGET=robzyl_V5.1beta.ro && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 ENABLE_NINJA=1 TARGET=robzyl_Ninja_V5.1beta.fr && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=0 ENABLE_PL_BAND=1 ENABLE_NINJA=1 TARGET=robzyl_Ninja_V5.1beta.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=1 ENABLE_PL_BAND=0 ENABLE_NINJA=1 TARGET=robzyl_Ninja_V5.1beta.pl && cp *packed.bin compiled-firmware/"


time /t
pause