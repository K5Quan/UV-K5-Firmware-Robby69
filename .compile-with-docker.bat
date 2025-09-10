@echo off
cls
del .\compiled-firmware\*.bin
docker build -t uvk5 .
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make clean && make -s ENABLE_FR_BAND=1                     TARGET=robzyl.fr && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_FR_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.fr.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_PL_BAND=1                     TARGET=robzyl.pl && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_PL_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.pl.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_RO_BAND=1                     TARGET=robzyl.ro && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_RO_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.ro.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_KO_BAND=1                     TARGET=robzyl.ko && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_KO_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.ko.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_CZ_BAND=1                     TARGET=robzyl.cz && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_CZ_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.cz.screenshot && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_TU_BAND=1                     TARGET=robzyl.tu && cp *packed.bin compiled-firmware/"
docker run --rm -v %CD%\compiled-firmware:/app/compiled-firmware uvk5 /bin/bash -c "cd /app && make -s               ENABLE_TU_BAND=1 ENABLE_SCREENSHOT=1 TARGET=robzyl.tu.screenshot && cp *packed.bin compiled-firmware/"

time /t
pause