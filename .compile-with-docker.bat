@echo off
cls
del .\compiled-firmware\*.bin
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 TARGET=robzyl_V5.2beta.en.fr && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=0 ENABLE_PL_BAND=1 TARGET=robzyl_V5.2beta.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_RO_BAND=1 ENABLE_PL_BAND=0 TARGET=robzyl_V5.2beta.ro && cp *packed.bin compiled-firmware/"
time /t
pause