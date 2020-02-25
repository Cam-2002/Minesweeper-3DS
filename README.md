# Minesweeper-3DS
A simple minesweeper port for 3DS.

This is one of my first 3DS homebrew apps, so I'm fairly inexperienced with Citro.

## How to Play
The game has three modes: Small (8x10, 15 mines), Medium (12x16, 40 mines), and Large (16x20, 60 mines). <br><br>
Upon start, select a mode with the A button.
Once in game, tapping any tile will reveal a clear area, surrounded by numbers. Each number represents how many adjacent (touching or diagonal) tiles contain mines. Tapping on every tile other than a mine is a win, but tapping just one mine will be a game over.<br><br>
If you would like to flag where you believe a mine is, press any button (except start, select, but including the D-Pad) to enable flag mode. This is denoted by a small flag icon in the top right corner. Tapping a tile with flag mode enabled will place a flag on the tile, and will not allow you to mine it upon exiting flag mode. Pressing the tile again in flag mode will remove the flag. There is no indication as to whether or not your flag is correct, the mine counter will decrease regardless of whether it is correct or not.<br><br>
The timer has no impact on the game and only serves to show you how long you have taken.<br>

## Installing on 3DS

I recommend copying the CIA file from the latest release to your 3DS' SD card, then using a title manager such as FBI to install it.

Alternatively, you can download the 3DSX/SMDH file from the latest release and install it to your SD cards `/3ds/` directory for use with the Homebrew launcher.

## Debug Mode

Debug mode allows you to modify the seed and view variables while playing.
Enable with `L + R + Select`

## Building
Use make with the makefile. Nothing special should be required.
A batch file to build it has been included as well as one to also build a CIA using bannertool and makerom.
