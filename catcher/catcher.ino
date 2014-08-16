#include <SPI.h>
#include <Gamebuino.h>

//Font variables

//-------------------------------------

Gamebuino gb;
extern const byte font5x7[];

//-------------------------------------
//Stuff from magpi
#define W LCDWIDTH
#define H LCDHEIGHT

#define PAD_U 0x01
#define PAD_D 0x02
#define PAD_L 0x04
#define PAD_R 0x08
#define PAD_A 0x10
#define PAD_B 0x20
#define PAD_C 0x40


int pad_hit;

long delayt;

unsigned long bt;

boolean pad_check() {
  boolean ret_val = false;

  if (gb.buttons.pressed(BTN_DOWN)) {pad_hit |= PAD_D; ret_val=true;}
  if (gb.buttons.released(BTN_DOWN)) pad_hit &= ~PAD_D;
  if (gb.buttons.pressed(BTN_UP)) {pad_hit |= PAD_U; ret_val=true;}
  if (gb.buttons.released(BTN_UP)) pad_hit &= ~PAD_U;
  if (gb.buttons.pressed(BTN_LEFT)) {pad_hit |= PAD_L; ret_val=true;}
  if (gb.buttons.released(BTN_LEFT)) pad_hit &= ~PAD_L;
  if (gb.buttons.pressed(BTN_RIGHT)) {pad_hit |= PAD_R; ret_val=true;}
  if (gb.buttons.released(BTN_RIGHT)) pad_hit &= ~PAD_R;
  if (gb.buttons.pressed(BTN_A)) {pad_hit |= PAD_A; ret_val=true;}
  if (gb.buttons.released(BTN_A)) pad_hit &= ~PAD_A;
  if (gb.buttons.pressed(BTN_B)) {pad_hit |= PAD_B; ret_val=true;}
  if (gb.buttons.released(BTN_B)) pad_hit &= ~PAD_B;
  if (gb.buttons.pressed(BTN_C)) {pad_hit |= PAD_C; ret_val=true;}
  if (gb.buttons.released(BTN_C)) pad_hit &= ~PAD_C;

  return ret_val;
}


//-------------------------------------
//define images
const byte logo[] PROGMEM= {
  84, 48,
  0x73,0x11,0x1C,0x19,0xC4,0x68,0x0,0x0,0x0,0x0,0x0,
  0x94,0xBB,0xA4,0x22,0x4E,0x88,0x0,0x0,0x0,0x0,0x0,
  0x94,0x91,0x24,0x22,0x44,0x8E,0x7,0x80,0x0,0x0,0x0,
  0x73,0x11,0x1E,0x19,0xE4,0x6A,0xF,0xC0,0x0,0x0,0x0,
  0x10,0x0,0x0,0x0,0x0,0x0,0x78,0x78,0x0,0x0,0x0,
  0x72,0x60,0x0,0x0,0x0,0x0,0x78,0x78,0x0,0x0,0x0,
  0x2,0xAF,0x7,0x24,0x80,0x0,0xF,0xC0,0x0,0x0,0x0,
  0x0,0xCA,0x89,0x24,0x80,0x0,0x7,0x80,0x0,0x0,0x0,
  0x0,0x8A,0x89,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x60,0x7,0xB6,0x80,0x1E,0x0,0x78,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x1E,0x0,0x78,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x1E,0x0,0x78,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x1E,0x0,0x78,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0xF,0xFF,0xF0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x7,0xFF,0xE0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x55,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x13,0x0,0x0,0x2B,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x1C,0xE0,0x0,0x1A,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x10,0x20,0x0,0x14,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x13,0x20,0x0,0x8,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x1C,0xE0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x10,0x0,0x0,0x8,0x0,0x0,0x0,0x0,
  0x7F,0x0,0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0xFF,0x0,0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0xFE,0x7,0xC7,0xF8,0xFC,0xC6,0x7E,0x7E,0x0,0x0,0x0,
  0xE0,0xF,0xE3,0xF1,0xF8,0xC6,0x7C,0x7F,0x0,0x0,0x0,
  0xC0,0xC,0x60,0xC1,0xC0,0xC6,0x60,0x63,0x0,0x0,0x0,
  0xC0,0xC,0x60,0xC1,0x80,0xFE,0x78,0x63,0x0,0x0,0x0,
  0xC0,0xF,0xE0,0xC1,0x80,0xEE,0x78,0x7E,0x0,0x0,0x0,
  0xC0,0xE,0xE0,0xC1,0x80,0xC6,0x60,0x7E,0x0,0x0,0x0,
  0xC0,0xC,0x60,0xC1,0xC0,0xC6,0x60,0x67,0x0,0x0,0x0,
  0xE0,0xC,0x60,0xC1,0xF8,0xC6,0x7C,0x63,0x0,0x0,0x0,
  0xFE,0x8,0x20,0xC0,0xFC,0x82,0x7E,0x61,0x0,0x0,0x0,
  0xFF,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};

const byte catcher_bm[] PROGMEM=
{
	8, 3,
	B11000011,
  	B11000011,
  	B01111110,
};

const byte ball_bm[] PROGMEM=
{
	8, 3,
    B00011000,
    B01100110,
    B00011000,
};

const byte flag0_bm[] PROGMEM= {
	8, 8,
	B10000000,
	B11000000,
	B10100000,
	B10010000,
	B11010000,
	B10110000,
	B10000000,
	B10000000,
};
//    0x80,0xC0,0xa0,0xA0,0x90,0xF0,0x80,0x80}; Old hex flag0_bm
const byte flag1_bm[] PROGMEM= {
	8, 8,
    0xE1,0x9F,0x81,0xE1,0x9E,0x80,0x80,0x80};
const byte flag2_bm[] PROGMEM= {
	8, 8,
    0xB0,0xCF,0x81,0xB1,0xCF,0x80,0x80,0x80};
const byte flag3_bm[] PROGMEM= {
	8, 8,
    0x98,0xE7,0x81,0x99,0xE7,0x80,0x80,0x80};
const byte flag4_bm[] PROGMEM= {
	8, 8,
    0x8C,0xF3,0x81,0x8D,0xF3,0x80,0x80,0x80};
const byte flag5_bm[] PROGMEM= {
	8, 8,
    0x86,0xF9,0x81,0x87,0xF9,0x80,0x80,0x80};
const byte flag6_bm[] PROGMEM= {
	8, 8,
    0xC3,0xBD,0x81,0xC3,0xBC,0x80,0x80,0x80};

#define FLAG_COUNT 5
const unsigned char *flags_bm[] = {
  flag1_bm,flag2_bm,flag3_bm,flag4_bm,flag5_bm,flag6_bm};

//------------------------------------------------------------------------------
#define FRAMES_PER_SECOND 10

#define cW 8
#define cH 3

#define R .2  // rocket impulse

#define MAX_AX 2  // max accellerations
#define MAX_AY 1.5

typedef struct sprite sprite;

struct sprite {
  float x;
  float y;
  float ax;
  float ay;
  int h;
  int w;
  const unsigned char  *bitmap;
  void (*draw_fun)(void *);
};

#define MAX_SPRITES 10
#define CATCHER 0
sprite sprites[MAX_SPRITES];
int sprite_count = 1;
float gravity;
const float air_resistance = .005; //could be a variable if you want to make diferent environments
float wind;

int score;

uint8_t level,level_balls,balls_missed;
boolean level_up;
int next_ball_max;

boolean any_button_pressed() {
	if(gb.buttons.pressed(BTN_A|BTN_B|BTN_C|BTN_LEFT|BTN_RIGHT|BTN_UP|BTN_DOWN)){
		return true;
	}
}

void reset_ball(int i) {
  sprites[i].x = random(0,W-cW-1);
  sprites[i].y = 0;
  sprites[i].ax = 0;
  sprites[i].ay = 0;
  sprites[i].w = cW;
  sprites[i].h = cH;
  sprites[i].bitmap = ball_bm;
  sprites[i].draw_fun = 0;
}

void add_ball() {
  reset_ball(sprite_count);
  sprite_count++;
}

void kill_ball(int b) {
  sprite_count--;
  for(int i=b;i<sprite_count;i++) {
    sprites[i] = sprites[i+1];
  }
  if (level_balls == 0 && sprite_count == 1) {
    level_up = true;
  }
}

void reset_catcher() {
  sprites[CATCHER].x = W/2;
  sprites[CATCHER].y = H/2;
  sprites[CATCHER].ax = 0;
  sprites[CATCHER].ay = 0;
}

/* pants */void draw_catcher(void *s) { //Draw fire from thrusters
	int i = gb.frameCount &1;
	switch (pad_hit) {
	case PAD_D:
    	switch(i) {
    	case 0:
      		gb.display.drawPixel(((sprite *)s)->x+2,((sprite *)s)->y+3);
      	  	gb.display.drawPixel(((sprite *)s)->x+5,((sprite *)s)->y+3);
      	  	gb.display.drawPixel(((sprite *)s)->x+3,((sprite *)s)->y+4);
      	  	gb.display.drawPixel(((sprite *)s)->x+4,((sprite *)s)->y+3);
      	  	gb.display.drawPixel(((sprite *)s)->x+3,((sprite *)s)->y+5);
      	  	gb.display.drawPixel(((sprite *)s)->x+4,((sprite *)s)->y+4);
      	  	break;
    	case 1:
      		gb.display.drawPixel(((sprite *)s)->x+2,((sprite *)s)->y+4);
      	  	gb.display.drawPixel(((sprite *)s)->x+5,((sprite *)s)->y+4);
      	  	gb.display.drawPixel(((sprite *)s)->x+3,((sprite *)s)->y+3);
      		gb.display.drawPixel(((sprite *)s)->x+4,((sprite *)s)->y+4);
      	  	gb.display.drawPixel(((sprite *)s)->x+3,((sprite *)s)->y+4);
      		gb.display.drawPixel(((sprite *)s)->x+4,((sprite *)s)->y+5);
      	  	break;
    	}
		break;
	case PAD_L:
    	gb.display.drawPixel(((sprite *)s)->x-1-i,((sprite *)s)->y+0);
    	gb.display.drawPixel(((sprite *)s)->x-2-i,((sprite *)s)->y+1);
    	gb.display.drawPixel(((sprite *)s)->x-1-i,((sprite *)s)->y+2);
	    break;
	case PAD_R:
    	gb.display.drawPixel(((sprite *)s)->x+8+i,((sprite *)s)->y+0);
    	gb.display.drawPixel(((sprite *)s)->x+9+i,((sprite *)s)->y+1);
    	gb.display.drawPixel(((sprite *)s)->x+8+i,((sprite *)s)->y+2);
		break;
	}
}

void move() {
  for(int i=0;i< sprite_count;i++) {

    // environmental factors: gravity wind and air resistance
    sprites[i].ay += gravity;
    if (wind > 0) {
      if ((wind - sprites[i].ax) > 0) sprites[i].ax += wind/50;
    }
    else if (wind < 0) {
      if ((wind - sprites[i].ax) < 0) sprites[i].ax += wind/50;
    }
    if (sprites[i].ax > 0) sprites[i].ax -= air_resistance;
    if (sprites[i].ax < 0) sprites[i].ax += air_resistance;

    // sprite inertia wrapping and bouncing
    sprites[i].x += sprites[i].ax;
    sprites[i].y += sprites[i].ay;
    int w = sprites[i].w;
    int h = sprites[i].h;
    float x = sprites[i].x;
    float y = sprites[i].y;
    if (x < 0) sprites[i].x = (W-w)-.1;
    if (x >= W-w) sprites[i].x = 0;
    if (y < 0) sprites[i].y = (H-h)-.1;
    if (y >= H-h) {
      sprites[i].ay /= 2.5; 
      sprites[i].ay *= -1;
      sprites[i].y += sprites[i].ay;
      if (y>= H-h) {
        sprites[i].y = H-h-1;
      }
    }

    // ball collisions //FIXME //add colision sounds
    if (i > 0) {
      float x0 = sprites[0].x;
      float y0 = sprites[0].y;

      // with the catcher
      if ((x >= x0-3 && x <= x0+3 && y>=y0-1 && y < y0+3)) {
        score++;
		gb.sound.playOK();
        kill_ball(i);
      }
      if ((x > x0-5 && x < x0-3 && y>=y0-2 && y < y0+1) ||
        (x > x0+3 && x < x0+5 && y>=y0-2 && y < y0+1)) {
        float by = sprites[i].ay;
        float cy = sprites[0].ay;
        sprites[i].ay += (cy-by);
        sprites[0].ay += (by-cy);
        sprites[i].y += sprites[i].ay;
        sprites[0].y += sprites[0].ay;
        float bx = sprites[i].ax;
        float cx = sprites[0].ax;
        sprites[i].ax += (cx-bx);
        sprites[0].ax += (bx-cx);
        sprites[i].x += sprites[i].ax;
        sprites[0].x += sprites[0].ax;
      }
      // with the ground
      if (y>=H-cH-2 && abs(sprites[i].ay) < .1) {
        kill_ball(i);
        balls_missed++;
        score--;
      }
    }
  }
}

//byte vol;

byte flag_dir;
byte flag_pos;
float flap = 0;

long dt;

//
//------------------------------------------------------------------------------
void setup()
{
    gb.begin();
    gb.titleScreen(F(""), logo);
    gb.battery.show = false;
    gb.pickRandomSeed();
	gb.setFrameRate(10);
    gb.display.setFont(font5x7);
    score = 0;
    sprite_count = 1;
    reset_catcher();
    sprites[CATCHER].w = cW;
    sprites[CATCHER].h = cH;
    sprites[CATCHER].bitmap = catcher_bm;
    sprites[CATCHER].draw_fun = &draw_catcher;

    add_ball();

    gravity = .005;
//    air_resistance = .005; //could be changed if you want to make diferent environments
    wind = 0;
    level = 0;
    balls_missed = 0;
    level_up = true;
    level_balls = 0;
    next_ball_max = 10000;
	delayt = 0;
}

//------------------------------------------------------------------------------
void loop()
{
    if(gb.update()) {
		//------------------------------------------------------------------------------
		//void catcher():		
		if (delayt > 0) {
			if (millis() > delayt) delayt = 0;
			else {
				return;
			}
		}
		
		if(gb.buttons.repeat(BTN_RIGHT, 1) | gb.buttons.repeat(BTN_LEFT, 1) | gb.buttons.repeat(BTN_DOWN, 1)/* | gb.buttons.repeat(BTN_UP, 1) (Rocket sound even though there's no thruster?)*/){
//			vol = gb.sound.getVolume();
//			gb.sound.setVolume(1); //sets volume lower for thrusters (doesn't work yet becuase of sound commands (FIXME))
			gb.sound.playTick();
//			gb.sound.setVolume(vol);
		}
		
	    if (level_up) {
	      reset_catcher();
	      if (balls_missed == 0) level++;
	      level_up = false;
	      gb.display.clear();
		  gb.display.cursorX = 0;
		  gb.display.cursorY = 0;	  
	      gb.display.print(F("-- Catcher! --"));
	      if (balls_missed > 0) {
			gb.display.cursorX = 20;
			gb.display.cursorY = 10;	  
	        gb.display.print(F("Missed "));
	        gb.display.print(balls_missed);
	      }
	      balls_missed = 0;
		  gb.display.cursorX = 20;
		  gb.display.cursorY = 20;	  
	      gb.display.print(F("Level "));
	      level_balls = level;
	      gb.display.print(level);
	      gb.display.update();
	      delayt = millis() + 2000;

	      if (next_ball_max > 3000) next_ball_max -= 500;
	      bt = millis()+random(0,next_ball_max);
	      gravity += .001;
	      if (level > 1) {
	        wind = level * .05 + random(0,10)/20.0;
	        if (random(0,2)) { //Wind is negative (right to left)
				wind *= -1;
				flag_dir = FLIPH;
				flag_pos = 5;
	      	}
			else { //If the wind is positive (left to right)
				flag_dir = NOFLIP;
				flag_pos = 13;
			}
		  }
	      else wind = 0;
		  return;
	    }
	    dt = millis();
		    switch(pad_hit) {
			    case PAD_L:
			      if (sprites[CATCHER].ax < MAX_AX) sprites[CATCHER].ax+= R;
			      break;
			    case PAD_R:
			      if (sprites[CATCHER].ax > -MAX_AX) sprites[CATCHER].ax-= R;
			      break;
			    case PAD_U:
			      if (sprites[CATCHER].ay < MAX_AY) sprites[CATCHER].ay+= R;
			      break;
			    case PAD_D:
			      if (sprites[CATCHER].ay > -MAX_AY) sprites[CATCHER].ay-= R;
			      break;
			}

	      gb.display.clear();
		  gb.display.cursorX = 0;
		  gb.display.cursorY = 0;	  
	      gb.display.print(score);
	      //display.print(" ");   display.print(wind);
	      //    display.print("ax:");display.print(sprites[0].ax);display.print(" ay:");display.print(sprites[0].ay);
	      //    display.print("x:");display.print(sprites[0].x);display.print(" y:");display.print(sprites[0].y);
	      for(int i=0;i< sprite_count;i++) {
	        gb.display.drawBitmap((int)sprites[i].x, (int)sprites[i].y,  sprites[i].bitmap);
	        if (sprites[i].draw_fun) (*sprites[i].draw_fun)(&sprites[i]);
	      }
	      if (wind != 0) {
	        gb.display.drawBitmap(flag_pos,H-9,flags_bm[(int)flap],NOROT,flag_dir);
	        if (gb.frameCount&1) flap++;
	      }
	      else {
	        gb.display.drawBitmap(13,H-9,flag0_bm);
	      }
	      if (flap > FLAG_COUNT) flap = 0;
	      gb.display.drawFastHLine(0, H-1, W);
	      gb.display.update();
	      move();
	    

	    // add balls in randomly according to lvel
	    if (dt > bt && level_balls > 0) {
	      level_balls--;
	      bt = millis()+random(0,next_ball_max);
	      add_ball();
	    } 
		
		//------------------------------------------------------------------------------
	    if (pad_check()) {
			if (pad_hit == PAD_C)
		      setup();
		}
	}
}
