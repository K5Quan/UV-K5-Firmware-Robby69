@echo off
cls
del .\compiled-firmware\*.bin
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s ENABLE_FR_BAND=1 ENABLE_RO_BAND=0 ENABLE_PL_BAND=0 TARGET=robzyl_beta.en.fr && cp *packed.bin compiled-firmware/"
time /t
pause