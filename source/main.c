

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <citro2d.h>
#include <time.h>
#include <math.h>

// Define global variables
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
bool dispGameScreen = false, gameSetup = false, hasDiscovered = false, flagmode = false, gameover=false, youwin=false, debug=false;
int difficultyOption = 0, flagstate = 0, flags = 0, debugscreen = 0, debugseedselector = 0, bombs = 0, firstTileX=-1, firstTileY=-1;
long long int currentSeed = 0, start_time = 0, end_time = 0;
int map[24][24], tapmap[sizeof(map)/sizeof(map[0])][sizeof(map[0])/sizeof(map[0][0])];
C3D_RenderTarget* gfxBottomScreen;
u32 clrWhite, clrLightGrey, clrGrey, clrDarkGrey, clrBlack, clrGreen, clrRed, clrBlue;

void delay(int frames){
	for(int i=0;i<frames;i++) gspWaitForVBlank();
}

void draw_menu(){
	debugscreen = 0;
	if(gameover) printf("\x1b[10;6H  GAME OVER");
	if(youwin) printf("\x1b[10;6H  YOU WIN!");
	if(end_time-start_time>0) printf("\x1b[11;6H  TIME = %d:%02d", (int)floor((float)(end_time-start_time)/60), (int)(end_time-start_time)%60);
	printf("\x1b[13;6H  SMALL\x1b[14;6H  MEDIUM\x1b[15;6H  LARGE");
	if(debug) printf("\x1b[16;6H  DEBUG VARS");
	printf("\x1b[%d;6H> ",13+difficultyOption);
	gfxFlushBuffers();
}

void draw_debug(){
	debugscreen = 1;
	if(dispGameScreen){
		printf("\x1b[10;6H  SEED = %020lld", currentSeed);
		printf("\x1b[11;6H                               ");
		printf("\x1b[11;%dH^", 34-debugseedselector);
		printf("\x1b[12;6H  firstTileX = %d              ", firstTileX);
		printf("\x1b[13;6H  firstTileY = %d              ", firstTileY);
		printf("\x1b[14;6H  difficultyOption = %d        ", difficultyOption);
		printf("\x1b[15;6H  flagstate = %d               ", flagstate);
		printf("\x1b[16;6H  start_time = %lld            ", start_time);
		printf("\x1b[17;6H  end_time = %lld              ", end_time);
		printf("\x1b[18;6H  flags = %d                   ", flags);
		printf("\x1b[19;6H  bombs = %d                   ", bombs);
		printf("\x1b[20;6H  dispGameScreen = %s          ", dispGameScreen?"true":"false");
		printf("\x1b[21;6H  gameSetup = %s               ", gameSetup?"true":"false");
		printf("\x1b[22;6H  hasDiscovered = %s           ", hasDiscovered?"true":"false");
		printf("\x1b[23;6H  flagmode = %s                ", flagmode?"true":"false");
		printf("\x1b[24;6H  gameover = %s                ", gameover?"true":"false");
		printf("\x1b[25;6H  youwin = %s                  ", youwin?"true":"false");

		printf("\x1b[27;6H  PRESS A TO SET SEED");
	}else{
		printf("\x1b[10;6H  SEED = %020lld", currentSeed);
		printf("\x1b[11;6H                               ");
		printf("\x1b[11;%dH^", 34-debugseedselector);
		printf("\x1b[12;6H  start_time = %lld            ", start_time);
		printf("\x1b[13;6H  end_time = %lld              ", end_time);
		printf("\x1b[14;6H  gameover = %s                ", gameover?"true":"false");
		printf("\x1b[15;6H  youwin = %s                  ", youwin?"true":"false");

		printf("\x1b[27;6H  PRESS A TO SET SEED");
		printf("\x1b[28;6H  PRESS SELECT TO RETURN TO MENU");
	}
}

void hide_menu(){
	printf("\x1b[10;6H                 ");
	printf("\x1b[11;6H                 ");
}

void clear_menu(){
	for(int i=10; i<30; i++){
		printf("\x1b[%d;6H                                                 ", i);
	}
}

void setSeed(long long int seed){
	srand(seed);
	currentSeed = seed;
}

void setup_game(){
	// Create game map for small mode (10x8), 15 bombs
	int xM = 10, yM = 8, b = 15;
	if(difficultyOption == 1){ // Create game map for medium mode (16x12), 40 bombs
		xM = 16;
		yM = 12;
		b = 40;
	}else if(difficultyOption == 2){// Create game map for large mode (20x16), 60 bombs
		xM = 20;
		yM = 16;
		b = 60;
	}
	for(int x=0; x<xM; x++){
		for(int y=0; y<yM; y++){
			map[x][y] = 0;
		}
	}
	for(int i=0; i<b; i++){
		int bomb_x = (rand()%xM);
		int bomb_y = (rand()%yM);
		if(map[bomb_x][bomb_y]==9){
			i--;
			continue;
		}
		map[bomb_x][bomb_y]=9;
	}
	for(int x=0; x<xM; x++){
		for(int y=0; y<yM; y++){
			if(map[x][y]==9){
				if(x>0   &&   y>0) if(map[x-1][y-1]!=9) map[x-1][y-1]++;
				if(           y>0) if(map[x+0][y-1]!=9) map[x+0][y-1]++;
				if(x<xM-1&&   y>0) if(map[x+1][y-1]!=9) map[x+1][y-1]++;
				if(x>0           ) if(map[x-1][y+0]!=9) map[x-1][y+0]++;
				if(x<xM-1        ) if(map[x+1][y+0]!=9) map[x+1][y+0]++;
				if(x>0   &&y<yM-1) if(map[x-1][y+1]!=9) map[x-1][y+1]++;
				if(        y<yM-1) if(map[x+0][y+1]!=9) map[x+0][y+1]++;
				if(x<xM-1&&y<yM-1) if(map[x+1][y+1]!=9) map[x+1][y+1]++;
			}
		}
	}
	
	gameSetup = true;
}

long long int tenpow(int pow){
	long long int result = 1;
	for(int i=0; i<pow; i++) result*=10;
	return result;
}

void reset_game(){
	dispGameScreen = false;
	gameSetup = false;
	hasDiscovered = false;
	flagmode = false;
	gameover = false;
	youwin = false;
	flags = 0;
	firstTileX = -1;
	firstTileY = -1;
	for(int i=0; i<sizeof(map)/sizeof(map[0]); i++){
		for(int j=0; j<sizeof(map[i])/sizeof(map[i][0]); j++){
			map[i][j] = -1;
			tapmap[i][j] = 0;
		}
	}
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(gfxBottomScreen, clrBlack);
	C2D_SceneBegin(gfxBottomScreen);
	//C2D_DrawRectangle(0, 0, 1, SCREEN_WIDTH, SCREEN_HEIGHT, clrBlack, clrBlack, clrBlack, clrBlack);
	C3D_FrameEnd(0);
	clear_menu();
	draw_menu();
}

int num_tapmap(int num){
	int sum = 0;
	for(int i=0; i<sizeof(tapmap)/sizeof(tapmap[0]); i++){
		for(int j=0; j<sizeof(tapmap[i])/sizeof(tapmap[i][0]); j++){
			if(tapmap[i][j] == num) sum++;
		}
	}
	return sum;
}

int main(int argc, char **argv){
	//Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	setSeed(time(0));

	//Define colors
	u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	u32 clrLightGrey = C2D_Color32(0xD0, 0xD0, 0xD0, 0xFF);
	u32 clrGrey = C2D_Color32(0x80, 0x80, 0x80, 0xFF);
	u32 clrMagenta = C2D_Color32(0xF7, 0x30, 0x50, 0xFF);
	u32 clrMaroon = C2D_Color32(0x80, 0x00, 0x00, 0xFF);
	u32 clrBlack = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
	u32 clrGreen = C2D_Color32(0x00, 0xD0, 0x00, 0xFF);
	u32 clrRed   = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
	u32 clrBlue  = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
	u32 clrOrange = C2D_Color32(0xF0, 0xA0, 0x00, 0xFF);
	u32 clrTeal   = C2D_Color32(0x00, 0x80, 0x80, 0xFF);
	u32 clrPurple  = C2D_Color32(0xA0, 0x00, 0xA0, 0xFF);

	//Create screen
	gfxBottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	//Print text
	printf("\x1b[2;2HMINESWEEPER FOR 3DS BY KAM\x1b[3;2HPRESS START TO EXIT\x1b[4;2HPRESS SELECT TO RESET\x1b[5;2HPRESS ANYTHING TO FLAG");
	gfxFlushBuffers();
	gfxSwapBuffers();

	// Set values in the map
	for(int i=0; i<sizeof(map)/sizeof(map[0]); i++){
		for(int j=0; j<sizeof(map[i])/sizeof(map[i][0]); j++){
			map[i][j] = -1;
			tapmap[i][j] = 0;
		}
	}

	//Initialize text for numbers
	C2D_Text textNumbers[10];
	//C2D_TextBuf textBuffers[sizeof(textNumbers)/sizeof(textNumbers[0])];
	C2D_TextBuf staticBuf = C2D_TextBufNew(32);
	C2D_TextBuf bombsBuf = C2D_TextBufNew(64);
	for(int i=0; i<sizeof(textNumbers)/sizeof(textNumbers[0]); i++){
		//textBuffers[i] = C2D_TextBufNew(1);
		char tmpNumbers[4] = "B";
		if(i<8) sprintf(tmpNumbers, "%d", i+1);
		if(i==9) sprintf(tmpNumbers, "F");
		C2D_TextParse(&textNumbers[i],staticBuf,tmpNumbers);
		C2D_TextOptimize(&textNumbers[i]);
	}

	draw_menu();
	// Main loop
	while (aptMainLoop()){
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		//Stop program if start is pressed
		if (kDown & KEY_START) break;

		//Read touch screen
		touchPosition touch;

		//Read the touch screen coordinates
		hidTouchRead(&touch);

		//Print the touch screen coordinates
		//printf("\x1b[3;2H%03d; %03d; %lld; %d", touch.px, touch.py, time(0), flagmode ? 1:0);
		if((kHeld & (KEY_L | KEY_R)) && (kDown & KEY_SELECT)){
			debug=!debug;
			clear_menu();
		}

		//If we are on the menu screen
		if(!dispGameScreen && debugscreen==0){
			if(kDown & KEY_UP) difficultyOption--;
			if(kDown & KEY_DOWN) difficultyOption++;
			if(difficultyOption<0) difficultyOption = 2+(debug?1:0);
			if(difficultyOption>2+debug?1:0) difficultyOption = 0;
			if(kDown & KEY_A){
				if(difficultyOption < 3){
					dispGameScreen = true;
					hide_menu();
					continue;
				}else{
					clear_menu();
					draw_debug();
				}
			}else if(debugscreen==0) draw_menu();
		}
		if(debugscreen==1){
			if(kDown & KEY_LEFT) debugseedselector++;
			if(kDown & KEY_RIGHT) debugseedselector--;
			if(debugseedselector > 18) debugseedselector = 0;
			if(debugseedselector < 0) debugseedselector = 18;
			if(kDown & KEY_UP) currentSeed+=tenpow(debugseedselector);
			if(kDown & KEY_DOWN) currentSeed-=tenpow(debugseedselector);
			if(kDown & KEY_A) setSeed(currentSeed);
			if(kDown & KEY_SELECT){
				clear_menu();
				draw_menu();
			}else draw_debug();
		}

		//If we are in game
		if(dispGameScreen){
			if(!gameSetup) setup_game(); //Setup game by creating array of bombs
			if((kDown & (KEY_B | KEY_A | KEY_DUP | KEY_DDOWN | KEY_DRIGHT | KEY_DLEFT)) && !debug) flagmode = !flagmode;
			if(kDown & (KEY_L | KEY_R | KEY_ZL | KEY_ZR | KEY_X | KEY_Y)) flagmode = !flagmode;
			if(debug) draw_debug();
			//start rendering
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(gfxBottomScreen, clrBlack);
			C2D_SceneBegin(gfxBottomScreen);
			int offset_x = 80, offset_y = 48, width = 16, height = 16, rows = 8, cols = 10;
			bombs = 15;
			float numscale = 0.5f;

			if(difficultyOption==1){
				offset_x = 32;
				offset_y = 24;
				width = 16;
				height = 16;
				rows = 12;
				cols = 16;
				bombs = 40;
			}
			if(difficultyOption==2){
				offset_x = 40;
				offset_y = 24;
				width = 12;
				height = 12;
				rows = 16;
				cols = 20;
				bombs = 60;
				numscale = 0.4f;
			}

			//calculate times
			if(!hasDiscovered) start_time = time(0);
			if(!gameover && !youwin) end_time = time(0);

			//display bomb count and times
			C2D_TextBufClear(bombsBuf);
			char buf[64];
			C2D_Text bombsText;
			sprintf(buf, "Bombs = %d, Time = %d:%02d", bombs-flags, (int)floor((float)(end_time-start_time)/60), (int)(end_time-start_time)%60);
			C2D_TextParse(&bombsText, bombsBuf, buf);
			C2D_TextOptimize(&bombsText);
			C2D_DrawText(&bombsText, C2D_AtBaseline | C2D_WithColor, 4, 232, 0.5, 0.5f, 0.5f, clrLightGrey);

			if(0 < touch.px && touch.px < 30 && 0 < touch.py && touch.py < 30 && flagstate == 0){
				flagmode = !flagmode;
				flagstate = 1; //flagstate used to prevent debounce.
			}
			if(!(0 < touch.px && touch.px < 30 && 0 < touch.py && touch.py < 30) && flagstate == 1) flagstate = 0;

			//for each tile
			for(int i=0; i<sizeof(map)/sizeof(map[0]); i++){
				for(int j=0; j<sizeof(map[i])/sizeof(map[i][0]); j++){
					if(map[i][j]==-1) break; //stop when we reach end of row
					// tell us if were in flag mode
					if(flagmode){
						C2D_DrawText(&textNumbers[9], C2D_WithColor, 4, 4, 0.5, 0.75f, 0.75f, clrMaroon);
					}

					// Determine if we tapped this tile, if so unlock it or flag it
					if(offset_x+width*i < touch.px && touch.px < offset_x+width*(i+1) && offset_y+height*j < touch.py && touch.py < offset_y+height*(j+1) && !gameover && !youwin){
						//Unlock
						if(!flagmode && tapmap[i][j]!=2 && tapmap[i][j]!=3 && (map[i][j] != 9 || !hasDiscovered)){
							tapmap[i][j]=1;
							if(!hasDiscovered){
								firstTileX = i+1;
								firstTileY = j+1;
								while(map[i][j]!=0) setup_game();
								for(int i=0; i<sizeof(map)/sizeof(map[0]); i++){
									for(int j=0; j<sizeof(map[i])/sizeof(map[i][0]); j++){
										if(tapmap[i][j] == 2 || tapmap[i][j] == 3) tapmap[i][j] = 0;
									}
								}
								flags = 0;
							}
							hasDiscovered = true;
						}
						if(!flagmode && tapmap[i][j]!=2 && map[i][j] == 9){
							gameover=true;
							draw_menu();
						}
						if(flagmode){ //flag
							if(tapmap[i][j]==0){
								tapmap[i][j]=3; //set to 3 to prevent debounce
								flags++;
							}
							else if(tapmap[i][j]==2){
								tapmap[i][j]=4; //ditto
								flags--;
							}
						}
					}else if(tapmap[i][j]==3) tapmap[i][j] = 2; //if we did not tap it, and it was previously flagged, allow it to now be unflagged (prevent debounce)
					 else if(tapmap[i][j]==4) tapmap[i][j] = 0; //ditto

					if(tapmap[i][j] != 1){ // if undiscovered and
						// if neighboring tile is a discovered zero, then discover.
						if(i>0 && j>0) 	if(map[i-1][j-1]==0 && tapmap[i-1][j-1]==1) tapmap[i][j] = 1;
						if(       j>0) 	if(map[i+0][j-1]==0 && tapmap[i+0][j-1]==1) tapmap[i][j] = 1;
						if(       j>0) 	if(map[i+1][j-1]==0 && tapmap[i+1][j-1]==1) tapmap[i][j] = 1;
						if(i>0       ) 	if(map[i-1][j+0]==0 && tapmap[i-1][j+0]==1) tapmap[i][j] = 1;
										if(map[i+1][j+0]==0 && tapmap[i+1][j+0]==1) tapmap[i][j] = 1;
						if(i>0       ) 	if(map[i-1][j+1]==0 && tapmap[i-1][j+1]==1) tapmap[i][j] = 1;
										if(map[i+0][j+1]==0 && tapmap[i+0][j+1]==1) tapmap[i][j] = 1;
										if(map[i+1][j+1]==0 && tapmap[i+1][j+1]==1) tapmap[i][j] = 1;
						
						// Draw hidden tile
						C2D_DrawRectangle(offset_x+width*i,offset_y+height*j,0,width,height,clrBlack,clrBlack,clrBlack,clrBlack);
						C2D_DrawRectangle(offset_x+width*i,offset_y+height*j,0.1,width-1,height-1,clrWhite,clrWhite,clrWhite,clrWhite);
						if(gameover && map[i][j] == 9) C2D_DrawRectangle(1+offset_x+width*i,1+offset_y+height*j,0.2,width-2,height-2,clrMaroon,clrMaroon,clrMaroon,clrMaroon);
						else C2D_DrawRectangle(1+offset_x+width*i,1+offset_y+height*j,0.2,width-2,height-2,clrGrey,clrGrey,clrGrey,clrGrey);
						if(tapmap[i][j]==2 || tapmap[i][j] == 3) C2D_DrawText(&textNumbers[9], C2D_WithColor, 3+offset_x+width*i,offset_y+height*j,0.2,numscale,numscale,clrMaroon);
					}else{
						// Determine color and draw tile
						u32 boxColor = clrBlack;
						switch(map[i][j]){
							case 1: boxColor = clrBlue; break;
							case 2: boxColor = clrOrange; break;
							case 3: boxColor = clrGreen; break;
							case 4: boxColor = clrMagenta; break;
							case 5: boxColor = clrRed; break;
							case 6: boxColor = clrTeal; break;
							case 7: boxColor = clrPurple; break;
							case 8: boxColor = clrDarkGrey; break;
							case 9: boxColor = clrBlack; break;
						}
						C2D_DrawRectangle(offset_x+width*i,offset_y+height*j,0,width,height,clrGrey,clrGrey,clrGrey,clrGrey);
						C2D_DrawRectangle(offset_x+width*i,offset_y+height*j,0.1,width-1,height-1,clrLightGrey,clrLightGrey,clrLightGrey,clrLightGrey);
						if(map[i][j]>0) C2D_DrawText(&textNumbers[map[i][j]-1], C2D_WithColor, 3+offset_x+width*i,offset_y+height*j,0.2,numscale,numscale,boxColor);
					}

					if(num_tapmap(1) == cols*rows - bombs){ //If the number of discovered tiles is equal to the number of total tiles minus bombs, you win
						youwin = true;
						draw_menu();
					}
				}
			}
			C3D_FrameEnd(0);
			if((kDown & KEY_SELECT) && !(kHeld & (KEY_L | KEY_R))) reset_game();
		}
		// Flush and swap framebuffers
		//gfxSwapBuffers();
		//gfxFlushBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
