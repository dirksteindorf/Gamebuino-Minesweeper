#include <SPI.h>
#include <Gamebuino.h>

Gamebuino gb;
extern const byte font3x3[];
extern const byte font5x7[];

const byte FIELD_WIDTH  = 7;
const byte FIELD_HEIGHT = 9;

const byte COLUMNS = 6;
const byte ROWS    = 5;
const byte BOMB_COUNT = 6;

const byte WIDTH  = COLUMNS + 2;
const byte HEIGHT = ROWS + 2;

const byte offset_x = (LCDWIDTH - (COLUMNS) * FIELD_WIDTH) / 2;
const byte offset_y = (LCDHEIGHT - (ROWS) * FIELD_HEIGHT) / 2;

const char text[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

byte uncovered_fields = 0;

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

        if(!board[x][y].is_bomb)
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
    // draw happy/sad smiley depending on the game state
    if(game_state == WON)
    {
        gb.display.drawChar(FIELD_WIDTH,
                            FIELD_HEIGHT,
                            0x02,
                            1);
    }
    else if(game_state == LOST)
    {
        gb.display.drawChar(FIELD_WIDTH,
                            FIELD_HEIGHT,
                            0x01,
                            1);
    }

    // draw mine fields
    for(byte i=1; i<WIDTH-1; i++)
    {
        for(byte j=1; j<HEIGHT-1; j++)
        {
            if(board[i][j].state == COVERED)
            {
                gb.display.drawChar(offset_x + FIELD_WIDTH * (i-1) + 1 ,
                                    offset_y + FIELD_HEIGHT * (j-1) + 1,
                                    0x20,
                                    1);
            }

            if(board[i][j].state == FLAGGED)
            {
                gb.display.drawChar(offset_x + FIELD_WIDTH * (i-1) + 1 ,
                                    offset_y + FIELD_HEIGHT * (j-1) + 1,
                                    0x0E,
                                    1);
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
// process button events
void process_player_input()
{
    // uncover a field
    if(gb.buttons.pressed(BTN_A))
    {
        if(board[cursor.x+1][cursor.y+1].state == COVERED)
        {
            board[cursor.x+1][cursor.y+1].state = UNCOVERED;

            // game is lost, uncover all fields
            if(board[cursor.x+1][cursor.y+1].is_bomb)
            {
                game_state = LOST;
                for(int x=1; x<WIDTH-1; x++)
                {
                    for(int y=1; y<HEIGHT-1; y++)
                    {
                        board[x][y].state = UNCOVERED;
                    }
                }
                gb.popup(F("You lost. :("), 20);
                gb.sound.playCancel();
            }
            else
            {
                gb.sound.playOK();

                if(++uncovered_fields == COLUMNS * ROWS - BOMB_COUNT)
                {
                    game_state = WON;
                    gb.popup(F("You won! :)"), 20);
                    gb.sound.playOK();
                }
            }
        }
    }

    // flag and unflag a field
    if(gb.buttons.pressed(BTN_B))
    {
        if(board[cursor.x+1][cursor.y+1].state == COVERED)
        {
            board[cursor.x+1][cursor.y+1].state = FLAGGED;
            gb.sound.playOK();
        }

        else if(board[cursor.x+1][cursor.y+1].state == FLAGGED)
        {
            board[cursor.x+1][cursor.y+1].state = COVERED;
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
void setup()
{
    gb.begin();
    gb.titleScreen(F("Minesweeper"));

    gb.pickRandomSeed();

    //gb.display.setFont(font3x3);
    gb.display.setFont(font5x7);

    game_state = RUNNING;

    init_board();
    place_bombs();
    compute_bomb_hints();

    cursor.x = 0;
    cursor.y = 0;
    uncovered_fields = 0;
}

//------------------------------------------------------------------------------
void loop()
{
    if(gb.update())
    {
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