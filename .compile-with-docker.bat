@echo off
cls
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=0 ENABLE_PL_BAND=1 TARGET=robzylV5.1b2.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1 ENABLE_PL_BAND=0 TARGET=robzylV5.1b2.fr && cp *packed.bin compiled-firmware/"
time /t
pause