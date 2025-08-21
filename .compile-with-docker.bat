@echo off
cls
del .\compiled-firmware\*.bin
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 TARGET=robzyl.en.fr && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 ENABLE_SCREENSHOT=1 TARGET=robzyl.en.fr.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=0 ENABLE_PL_BAND=1 TARGET=robzyl.en.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=0 ENABLE_PL_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.en.pl.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=1 ENABLE_PL_BAND=0 TARGET=robzyl.en.ro && cp *packed.bin compiled-firmware/"
time /t
pause