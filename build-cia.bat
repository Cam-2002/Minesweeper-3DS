@echo off
echo === Building Minesweeper ===
make
bannertool makebanner -i banner.png -a empty.wav -o banner.bin
makerom -f cia -o minesweeper.cia -rsf cia.rsf -target t -exefslogo -elf minesweeper.elf -icon minesweeper.smdh -banner banner.bin
echo === Done ===
pause