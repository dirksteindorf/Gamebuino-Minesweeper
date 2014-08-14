#include <SPI.h>
#include <Gamebuino.h>

//Font variables
#define FONT font3x5

byte fontx = 3;
byte fonty = 5;

//-------------------------------------

Gamebuino gb;
extern const byte font3x3[];
extern const byte font3x5[];
extern const byte font5x7[];

const byte FIELD_WIDTH  = fontx + 2;
const byte FIELD_HEIGHT = fonty + 2;

const byte COLUMNS = 12;
const byte ROWS    = 6;
const byte BOMB_COUNT = ((COLUMNS + ROWS) / 2) + (ROWS / 2);

const byte WIDTH  = COLUMNS + 2;
const byte HEIGHT = ROWS + 2;

const byte offset_x = 21; //((LCDWIDTH - (COLUMNS) * FIELD_WIDTH) / 2) + 17;
const byte offset_y = (LCDHEIGHT - (ROWS) * FIELD_HEIGHT) / 2;

const char text[10] = {' ', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

byte uncovered_fields;
byte flag_count;

byte first_field;

//-------------------------------------
// board
enum field_state {COVERED, UNCOVERED, FLAGGED};

typedef struct
{
    byte is_bomb;
    byte nearby_bombs;
    field_state state;
} minefield;

minefield board[WIDTH][HEIGHT];

//-------------------------------------
// cursor
typedef struct
{
    byte x;
    byte y;
} Cursor;

Cursor cursor;

//-------------------------------------
// game state
enum Game_state {RUNNING, WON, LOST};
Game_state game_state;

//-------------------------------------
//define special characters (flag, neutral face)
const byte flag[] PROGMEM=
{
	8, 5,
	B01100000,
	B11100000,
	B10100000,
	B00100000,
	B00100000,
};

const byte neutral[] PROGMEM=
{
	8, 7,
	B01110000,
	B11111000,
	B10101000,
	B11111000,
	B10001000,
	B11111000,
	B01110000,
};

//------------------------------------------------------------------------------
// initialize the playing field
void init_board()
{
    for(byte i=1; i<WIDTH-1; i++)
    {
        for(byte j=1; j<HEIGHT-1; j++)
        {
            board[i][j].is_bomb = false;
            board[i][j].nearby_bombs = 0;
            board[i][j].state = COVERED;
        }
    }
}

//------------------------------------------------------------------------------
// randomly distribute the bombs
void place_bombs()
{
    byte i = 0;
    byte x = 0;
    byte y = 0;

    while(i < BOMB_COUNT)
    {
        x = random(1, WIDTH - 2);
        y = random(1, HEIGHT - 2);

        if(!board[x][y].is_bomb && !(x == cursor.x+1 && y == cursor.y+1))
        {
            board[x][y].is_bomb = true;
            i++;
        }
    }
}

//------------------------------------------------------------------------------
// check the eight neighbours for bombs
byte get_bomb_count(byte x, byte y)
{
    byte count = 0;

    if(board[x][y-1].is_bomb)   count++;
    if(board[x][y+1].is_bomb)   count++;

    if(board[x-1][y-1].is_bomb) count++;
    if(board[x-1][y].is_bomb)   count++;
    if(board[x-1][y+1].is_bomb) count++;

    if(board[x+1][y-1].is_bomb) count++;
    if(board[x+1][y].is_bomb)   count++;
    if(board[x+1][y+1].is_bomb) count++;

    return count;
}

//------------------------------------------------------------------------------
// compute the number of adjacent bombs for each field
void compute_bomb_hints()
{
    for(byte i=1; i<WIDTH-1; i++)
    {
        for(byte j=1; j<HEIGHT-1; j++)
        {
            if(!board[i][j].is_bomb)
            {
                board[i][j].nearby_bombs = get_bomb_count(i, j);
            }
        }
    }
}

//------------------------------------------------------------------------------
// draw the playing field
void draw_board()
{
	gb.display.setFont(font5x7);
    // draw happy/sad/nuetral smiley depending on the game state
	if(game_state == LOST)
    	{
        	gb.display.drawChar(FIELD_WIDTH,
                				FIELD_HEIGHT,
                 			   0x01,
                 			   1);
    	}
    else if(gb.buttons.pressed(BTN_A))
    	{
        	gb.display.drawBitmap(FIELD_WIDTH,
              				   	 FIELD_HEIGHT,
               				     neutral,
              				   	 0,
							   	 0);
    	}
	else
		{
        	gb.display.drawChar(FIELD_WIDTH,
                				FIELD_HEIGHT,
                 			    0x02,
                 			    1);
		}
	gb.display.setFont(FONT);
	
    // draw mine fields
    for(byte i=1; i<WIDTH-1; i++)
    {
        for(byte j=1; j<HEIGHT-1; j++)
        {
            if(board[i][j].state == COVERED)
            {
                gb.display.fillRect(offset_x + FIELD_WIDTH * (i-1) + 1,
                                    offset_y + FIELD_HEIGHT * (j-1) + 1,
                                    fontx,
									fonty);
            }

            if(board[i][j].state == FLAGGED)
            {
                gb.display.drawBitmap(offset_x + FIELD_WIDTH * (i-1) + 1 ,
                                      offset_y + FIELD_HEIGHT * (j-1) + 1 ,
                                      flag,
                                      0,
                                      0);
            }

            if(board[i][j].state == UNCOVERED)
            {
                if(board[i][j].is_bomb)
                {
                    gb.display.drawChar(offset_x + FIELD_WIDTH * (i-1) + 1 ,
                                        offset_y + FIELD_HEIGHT * (j-1) + 1,
                                        0x2A,
                                        1);
                }
                else
                {
                    gb.display.drawChar(offset_x + FIELD_WIDTH * (i-1) + 1 ,
                                        offset_y + FIELD_HEIGHT * (j-1) + 1,
                                        text[board[i][j].nearby_bombs],
                                        1);
                }
            }
        }
    }

     // draw borders
    gb.display.drawRect(offset_x-1,
                        offset_y-1,
                        FIELD_WIDTH * (COLUMNS) + 2,
                        FIELD_HEIGHT * (ROWS) + 2);
}

//------------------------------------------------------------------------------
// draw the cursor
void draw_cursor()
{
    gb.display.drawRect(cursor.x * FIELD_WIDTH + offset_x-1,
                        cursor.y * FIELD_HEIGHT + offset_y-1,
                        FIELD_WIDTH + 2,
                        FIELD_HEIGHT + 2);
}

//------------------------------------------------------------------------------
// uncover all fields with no adjacent bombs and their neighbours
void uncover_harmless_neighbours(byte x, byte y)
{
    if(x > 0 && y > 0 && x < WIDTH-1 && y < HEIGHT-1)
    {
        if(board[x][y].state == COVERED)
        {
            if(!board[x][y].is_bomb)
            {
                board[x][y].state = UNCOVERED;
                uncovered_fields++;

                if(board[x][y].nearby_bombs == 0)
                {
                    uncover_harmless_neighbours(x+1, y+1);
                    uncover_harmless_neighbours(x+1, y);
                    uncover_harmless_neighbours(x+1, y-1);

                    uncover_harmless_neighbours(x, y+1);
                    uncover_harmless_neighbours(x, y-1);

                    uncover_harmless_neighbours(x-1, y+1);
                    uncover_harmless_neighbours(x-1, y);
                    uncover_harmless_neighbours(x-1, y-1);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// process button events
void process_player_input()
{
    // uncover a field
    if(gb.buttons.pressed(BTN_A))
    {
        if(first_field)
        {
            first_field = false;
            place_bombs();
            compute_bomb_hints();
        }

        if(board[cursor.x+1][cursor.y+1].state == COVERED)
        {
            //board[cursor.x+1][cursor.y+1].state = UNCOVERED;

            // game is lost, uncover all bombs
            if(board[cursor.x+1][cursor.y+1].is_bomb)
            {

                for(byte x=1; x<WIDTH-1; x++)
                {
                    for(byte y=1; y<HEIGHT-1; y++)
                    {
                        if(board[x][y].is_bomb)
                            board[x][y].state = UNCOVERED;
                    }
                }
                game_state = LOST;
                gb.popup(F("You lost. :("), 40);
                gb.sound.playCancel();
            }
            else
            {
                gb.sound.playOK();

                if(board[cursor.x+1][cursor.y+1].nearby_bombs == 0)
                {
                    uncover_harmless_neighbours(cursor.x+1, cursor.y+1);
                }
                else
                {
                    board[cursor.x+1][cursor.y+1].state = UNCOVERED;
                    uncovered_fields++;
                }

                if(uncovered_fields == COLUMNS * ROWS - BOMB_COUNT)
                {
                    game_state = WON;
                    gb.popup(F("You won! :)"), 40);
                    gb.sound.playOK();
                }
            }
        }
    }

    // flag and unflag a field
    if(gb.buttons.pressed(BTN_B))
    {
        if(first_field)
        {
            first_field = false;
            place_bombs();
            compute_bomb_hints();
        }

        if(board[cursor.x+1][cursor.y+1].state == COVERED)
        {
            board[cursor.x+1][cursor.y+1].state = FLAGGED;
            flag_count++;
            gb.sound.playOK();
        }

        else if(board[cursor.x+1][cursor.y+1].state == FLAGGED)
        {
            board[cursor.x+1][cursor.y+1].state = COVERED;
            flag_count--;
            gb.sound.playOK();
        }
    }

    // go  back to main menu
    if(gb.buttons.pressed(BTN_C))
    {
        setup();
    }

    // move cursor
    if(gb.buttons.pressed(BTN_UP))
    {
        if(cursor.y > 0) cursor.y -= 1;
        else cursor.y = ROWS-1;
        gb.sound.playTick();
    }
    if(gb.buttons.pressed(BTN_DOWN))
    {
        if(cursor.y < ROWS-1) cursor.y += 1;
        else cursor.y = 0;
        gb.sound.playTick();
    }
    if(gb.buttons.pressed(BTN_LEFT))
    {
        if(cursor.x > 0) cursor.x -= 1;
        else cursor.x = COLUMNS-1;
        gb.sound.playTick();
    }
    if(gb.buttons.pressed(BTN_RIGHT))
    {
        if(cursor.x < COLUMNS-1) cursor.x += 1;
        else cursor.x = 0;
        gb.sound.playTick();
    }
}
//------------------------------------------------------------------------------
//Logo

const byte logo[] PROGMEM = {64,36,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,
B10000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,
B10110000,B00011000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,
B10111000,B00111000,B00000000,B00111111,B10110100,B01011101,B11011101,B11011101,
B10111100,B01111000,B00000001,B10111111,B10100100,B01010001,B00010101,B00010101,
B10111110,B11111011,B00000001,B10110000,B10100100,B01010001,B00010101,B00010101,
B10111111,B11111011,B01100001,B10110000,B00100100,B01010001,B00010101,B00010101,
B10110111,B11011000,B01110001,B10110000,B00100100,B01010001,B00010101,B00010101,
B10110011,B10011011,B01110001,B10110000,B00100100,B01010001,B00011101,B00011101,
B10110001,B00011011,B01111001,B10111100,B00110100,B01011001,B10010001,B10011001,
B10110000,B00011011,B01111101,B10111000,B00010100,B01010001,B00010001,B00010101,
B10110000,B00011010,B00101111,B10110000,B00010100,B01010001,B00010001,B00010101,
B10110000,B00010000,B10100111,B10110000,B00010101,B01010001,B00010001,B00010101,
B10110000,B00000100,B10000011,B10110000,B00010101,B01010001,B00010001,B00010101,
B10110000,B00100011,B11010011,B10111111,B10010101,B01010001,B00010001,B00010101,
B10110000,B00010111,B11100001,B10111111,B10110010,B10011101,B11010001,B11010101,
B10110000,B00001111,B11111100,B00000000,B00000000,B00000000,B00000000,B00000101,
B10110111,B10001111,B11110001,B11111111,B11111111,B11111111,B11111111,B11110101,
B10110000,B00111111,B11111100,B00000000,B00000000,B00000000,B00000000,B00000101,
B10110000,B00001111,B11110000,B00000000,B00100000,B00000000,B00000100,B10010101,
B10110000,B00010111,B11101000,B00000000,B01000000,B10000000,B00000010,B10100101,
B10110000,B00100011,B11000100,B00000000,B01000001,B00000100,B00000000,B00000101,
B10110000,B00000101,B00100000,B00000000,B10000010,B00001000,B00100110,B10110101,
B10110000,B00000000,B10000000,B00000001,B11000111,B00011100,B01110000,B00000101,
B10110000,B00000001,B00000000,B00000001,B11000111,B00011100,B01110010,B10100101,
B10110000,B00000000,B10000000,B00000001,B11000111,B00011100,B01110100,B10010101,
B10110000,B00000001,B00000000,B11100000,B00000000,B00000000,B00000000,B00000001,
B10110000,B00000000,B10000001,B11100000,B00110010,B10000101,B11010001,B00010101,
B10110000,B00000001,B00000001,B11100000,B00111001,B00000101,B01010001,B00001001,
B10110000,B00000000,B10000001,B00100000,B00110001,B00011001,B11011101,B11001001,
B10110000,B00000001,B00000000,B00100000,B00000000,B00000000,B00000000,B00000001,
B10110000,B00000000,B10000000,B00100000,B00010001,B11010101,B10011100,B11011101,
B10110000,B00000001,B00000000,B00100000,B00111001,B01010101,B11001000,B10001001,
B10110000,B00000000,B10000000,B01110000,B00010001,B10011101,B10011101,B10001001,
B10110000,B00000001,B00000000,B11111000,B00000000,B00000000,B00000000,B00000001,
B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,
};
//------------------------------------------------------------------------------
void setup()
{
    gb.begin();
    gb.display.setFont(font5x7);	
    gb.titleScreen(F(""), logo);
    gb.display.setFont(FONT);
    gb.pickRandomSeed();
    gb.battery.show = false;

    //gb.display.setFont(font3x3);
    gb.display.setFont(FONT);

    game_state = RUNNING;

    init_board();

    cursor.x = 0;
    cursor.y = 0;

    uncovered_fields = 0;
    flag_count = 0;
    first_field = true;
}

//------------------------------------------------------------------------------
void loop()
{
    if(gb.update())
    {
		gb.display.setFont(font5x7);
        gb.display.print("\n\n\n\n\n");
		if(BOMB_COUNT >= 10) gb.display.setFont(font3x5);
        gb.display.print(flag_count);
        gb.display.print("/");
        gb.display.print(BOMB_COUNT);
		gb.display.setFont(FONT);

        if(game_state == RUNNING)
        {
            process_player_input();
            draw_board();
            draw_cursor();
        }

        if(game_state == WON)
        {
            draw_board();
            draw_cursor();
            if(gb.buttons.pressed(BTN_C)) setup();
        }

        if(game_state == LOST)
        {
            draw_board();
            draw_cursor();
            if(gb.buttons.pressed(BTN_C)) setup();
        }
    }
}