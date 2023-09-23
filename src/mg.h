/*
    Library for MakeGame board.

    [!] Educational version
    [!] Required u8g2 library

    author: Savushkin Alexander
    git:    @avsavushkiny
    e-mail: avsavushkiny@live.ru
    date:   11.06.2023
*/

#include <U8g2lib.h>

#ifndef MAKEGAME_H
#define MAKEGAME_H

/* We let the compiler know that the u8g2 object is defined in another file */
extern U8G2_ST7565_ERC12864_F_4W_SW_SPI u8g2;
extern const uint8_t gears_bits[];
extern int HEIGHT_LCD, WIDTH_LCD;

class Joystick
{
protected:
private:
    /* The sticks are in the middle position. */
    int DEF_RES_Y0{2100}, DEF_RES_Y1{2100}, DEF_RES_X0{2100}, DEF_RES_X1{2100};
    /* Correction on the y0-axis. */
    const int8_t CORR_Y0{100}, CORR_Y1{100}, CORR_X0{100}, CORR_X1{100};
    /* Initial setting of coordinates. */
    int COOR_Y0 = HEIGHT_LCD / 2; 
    int COOR_Y1 = HEIGHT_LCD / 2;
    int COOR_X0 = WIDTH_LCD  / 2;
    int COOR_X1 = WIDTH_LCD  / 2;
    /* Reset the counter of objects. */
    int OBJ_Y0{}, OBJ_Y1{}, OBJ_X0{}, OBJ_X1{};
    /* Raw data from Sticks. */
    int RAW_DATA_Y0{}, RAW_DATA_Y1{}, RAW_DATA_X0{}, RAW_DATA_X1{};
public:
    /* Contains the coordinates of the Sticks along the axes. */
    int calculatePositionX0();
    int calculatePositionY0();
    int calculatePositionX1();
    int calculatePositionY1();

    /* Variables for storing coordinates from the axes of the Sticks. */
    int posX0{}, posY0{}, posX1{}, posY1{};
    
    /* Generates 1 or 0 if the button is pressed or not. */
    bool pressKeyA();
    bool pressKeyB();

    /* Updating Stick coordinates. */
    void updatePositionXY();

    /* Counts objects by +1, normally 0 */
    int8_t calculateIndexY0();
    int8_t calculateIndexY1();
    int8_t calculateIndexX0();
    int8_t calculateIndexX1();
};

class Graphics
{
protected:
private:
public:
    /* Turn on the backlight of the LCD screen. 1 enabled, 0 disabled. */
    void controlBacklight(bool state);

    /* Initial display setting. Sets Contrast to 0, analog DC at 12, sets port 8 to 1 */
    void initializationSystem();
    /* We send the void-function to the display buffer for output. 
       The void-function will be completed by time-Delay-interval. */
    void render(void (*f)(), int timeDelay);
    /* We send the void-function to the display buffer for output. */
    void render(void (*f)());
    /* Clearing the display buffer. */
    void clear();
    /* Data output in x, y coordinates. lii-line spacing (10 by default),
       chi-character spacing (6 by default).
       Line break is supported - '\n' */
    void print(String text, int x, int y, int lii, int chi);
    /* Data output in x, y coordinates. 
       Line break is supported - '\n' */
    void print(String text, int x, int y);
    /* Runs a void-function with text-string and output x-y-coordinates parameters.
       The interval-interval controls the output. */
    bool winkPrint(void (*f)(String, int, int), String text, int x, int y, /*delay*/ int interval);
};

class Timer
{
protected:
private:
    unsigned long prevTime{};
public:
    /* Starting a void-function on a interval-timer. */
    void timer(void (*f)(void), int interval);
};

class Terminal
{
private:
public:
    void terminal();
};

class Interface
{
private:
public:
    /* Greetings. Contains logo and text. */
    void greetingsBoard();
    /* Output of a message to the user. Define the text-text and duration-duration.
       Line break is supported - '\n'. */
    void message(String text, int duration);
};

class Button : Joystick
{
private:
    int xCursor, yCursor;
public:
    /* The button starts the void-function, define the button text-text and output x-y-coordinates.
       xCursor-yCursor-coordinates of interaction with the cursor. */
    bool button(String text, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
};

class Shortcut : Joystick
{
private:
public:
    /* A shortcut on the desktop to launch the void-function.
       Define an icon-image with a resolution of 32x32 pixels, x-y-coordinates for output,
       xCursor-yCursor-coordinates of interaction with the cursor. */
    bool shortcut(const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
};

class Cursor
{
private:
public:
    /* Cursor. If the stateCursor status is 1 - visible, if 0 - not visible.
       Determine the coordinates of the Cursor to interact with the selected Stick. */
    bool cursor(bool stateCursor, int xCursor, int yCursor);
};

#endif