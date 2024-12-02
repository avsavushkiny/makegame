/*
  Contains function settings for working with the display.

  [!] Required u8g2 library
  [!] bmp to xbmp image converter https://www.online-utility.org/image/convert/to/XBM

  0.1 - first release
  0.2 - add game Pong
      - add game Aliens Invaders
  0.3 - fix bug position
      - add position with axis restrictions

  Authors: [2024] Alexander Savushkin, Mihail Samoilov, Aleksander Miheev
           [2023] Alexander Savushkin, Ksenofontov Sergey, Mihail Samoilov

           +7 953 034 4001
*/
#include <Arduino.h>
#include <U8g2lib.h> 
#include "mg.h"
#include "xbmp.h"

//version library
const int8_t VERSION_LIB[] = {0, 3};

Graphics _gfx;

enum StateOs
{
    /* State OS */
    _ON,
    _OFF,
    _PAUSED,
    _SLEEP,
    /* State game */
    _IN_GAME,
    _OFF_GAME,
    _CLOSE_GAME,
    _RESTART_GAME,
    _PAUSED_GAME

} STATE_OS;

//for screensaver
unsigned long screenTiming{};
unsigned long sleepModeTexCirculation{};
int sleepTextX{5}, sleepTextY{10};

//for timer
unsigned long previousMillis{};
unsigned long prevTime_0{};
const long interval{300};

/* graphics chip setup */
U8G2_ST7565_ERC12864_F_4W_SW_SPI u8g2(U8G2_R0, 18, 19, 17, 16, 20);

/* Liquid crystal display resolution. */
int HEIGHT_LCD{64}, WIDTH_LCD{128};
/* Analog-to-digital converter resolution (Chip PICO 2040). */
const int8_t RESOLUTION_ADC{12};
/* Port data. */
const int8_t PIN_STICK_0X = 26; // adc 0
const int8_t PIN_STICK_0Y = 27; // adc 1
const int8_t PIN_STICK_1Y = 28; // adc 2
const int8_t PIN_STICK_1X = 29; // adc 3
const int8_t PIN_BUTTON_STICK_0 = 6;  // gp 6
const int8_t PIN_BUTTON_STICK_1 = 7;  // gp 7
const int8_t PIN_BACKLIGHT_LCD = 8;   // gp 8

/* backlight */
void Graphics::controlBacklight(bool state)
{
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);

    if (state == true)
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 1); // on
    }
    else
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 0); // off
    }
}

/* graphics */
/* graphics output objects */
void Graphics::initializationSystem()
{
    //setting the operating system state
    STATE_OS = _ON;
    //setting display, contrast
    u8g2.begin();
    u8g2.setContrast(0);
    //setting the resolution of the analog-to-digital converter
    analogReadResolution(RESOLUTION_ADC);
    //display backlight
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);
    digitalWrite(PIN_BACKLIGHT_LCD, true);
    //platform logo output
    //--
    u8g2.clearBuffer();
    u8g2.drawXBMP(((WIDTH_LCD - mg_l_w)/2), ((HEIGHT_LCD - mg_l_h)/2), mg_l_w, mg_l_h, mg_l_bits); //56x28 px
    _gfx.print(6, (String)VERSION_LIB[0] + "." + (String)VERSION_LIB[1] , 0, 63, 10, 4);
    u8g2.sendBuffer();
    //--
    delay(2500);
}

/* data render (full frame) */
void Graphics::render(void (*f)(), int timeDelay)
{
    uint32_t time;

    time = millis() + timeDelay;

    do
    {
        u8g2.clearBuffer();
        f();
        u8g2.sendBuffer();
    } while (millis() < time);
}
/* data render (full frame) no time delay */
void Graphics::render(void (*f)())
{
      u8g2.clearBuffer();
      f();
      u8g2.sendBuffer();
}

/* clearing the output buffer */
void Graphics::clear()
{
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

/* print */
/* text output with parameters, add size font, add line interval (def: 10) and character interval (def: 6) */
void Graphics::print(int8_t sizeFont, String text, int x, int y, int8_t lii, int8_t chi) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int sizeText = text.length() + 1;
    int yy{0};

    //micro, 6, 7, 8, 10, 12, 13
    if (sizeFont == 5) u8g2.setFont(u8g2_font_micro_tr);
    else if (sizeFont == 6) u8g2.setFont(u8g2_font_4x6_tr);
    else if (sizeFont == 7) u8g2.setFont(u8g2_font_5x7_tr);
    else if (sizeFont == 8) u8g2.setFont(u8g2_font_5x8_tr);
    else if (sizeFont == 10) u8g2.setFont(u8g2_font_6x10_tr);
    else if (sizeFont == 12) u8g2.setFont(u8g2_font_6x12_tr);
    else if (sizeFont == 13) u8g2.setFont(u8g2_font_6x13_tr);
    else u8g2.setFont(u8g2_font_6x10_tr); //default

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}

/* text output with parameters, add line interval (def: 10) and character interval (def: 6) */
void Graphics::print(String text, int x, int y, int8_t lii, int8_t chi) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int sizeText = text.length() + 1;
    int yy{0};

    u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}

/* text output with parameters */
void Graphics::print(String text, int x, int y) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int8_t lii{10}, chi{6};
    int sizeText = text.length() + 1;
    int yy{0};

    u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}

/* "wink text" output  */
bool Graphics::winkPrint(void (*f)(String, int, int), String text, int x, int y, int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime_0 >= interval)
    {
        prevTime_0 = currTime;
        return 0;
    }
    else
    {
        f(text, x, y);
        return 1;
    }
}

/* displaying the cursor on the screen */
bool Cursor::cursor(bool stateCursor, int xCursor, int yCursor)
{
    if (stateCursor == true)
    {
        u8g2.setDrawColor(2);
        u8g2.setBitmapMode(1);
        u8g2.drawXBMP(xCursor, yCursor, cursor_w, cursor_h, cursor_bits);
        u8g2.setDrawColor(1);
        u8g2.setBitmapMode(0);
        return true;
    }
    else
        return false;
}

/* interface */
void Interface::greetingsBoard()
{
}

void Interface::message(String text, int duration)
{
    uint8_t x{10}, y{34};
    
    /* Counting the number of lines in a message
       Line break is supported - '\n' */   
    int sizeText = text.length() + 1, line{};

    for (int i = 0; i < sizeText; i++)
    {
        if (text[i] == '\n')
        {
          line++;
        }
    }

    uint8_t correction = (line * 10)/2;
    
    u8g2.clearBuffer();
    for (int i = 0; i < WIDTH_LCD; i += 2)
    {
        for (int j = 0; j < (y - 10) - correction; j += 2)
        {
            u8g2.drawPixel(i, j);
        }

        for (int k = y + ((line) * 10) + 4 - correction /* correction */; k < HEIGHT_LCD; k += 2)
        {
            u8g2.drawPixel(i, k);
        }
    }

    _gfx.print(text, x, y - correction, 10, 6);
    u8g2.sendBuffer();

    delay(duration);
}

/* button */
bool Button::button(String text, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  uint8_t sizeText = text.length();

  if ((xCursor >= x && xCursor <= (x + (sizeText * 5) + 4)) && (yCursor >= y - 8 && yCursor <= y + 2))
  {
    u8g2.setDrawColor(1);
    u8g2.drawRBox(x, y - 8, (sizeText * 5) + 5, 10, 2);

    if (Joystick::pressKeyA() == true)
    {
      f();
      return true;
    }
  }
  else
  {
    u8g2.setDrawColor(1);
    u8g2.drawRFrame(x, y - 8, (sizeText * 5) + 5, 10, 2);
  }

  u8g2.setCursor(x + 3, y);
  u8g2.setFont(u8g2_font_profont10_mr);
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);
  u8g2.print(text);
  u8g2.setFontMode(0);
  
  return false;
}

/* shortcut */
bool Shortcut::shortcut(const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  u8g2.setDrawColor(1);
  u8g2.setBitmapMode(0);
  u8g2.drawXBMP(x, y, 32, 32, bitMap);
  u8g2.drawXBMP(x, y + 24, 8, 8, icon_bits);

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    u8g2.drawFrame(x, y, 32, 32);
    if (Joystick::pressKeyA() == true)
    {
      f();
      return true;
    }
  }

  return false;
}

/* Joystick */
/* system button control */
bool Joystick::pressKeyA()
{
    if (digitalRead(PIN_BUTTON_STICK_0) == false)
    {
        return true;
    }
    else
        return false;
}

bool Joystick::pressKeyB()
{
    if (digitalRead(PIN_BUTTON_STICK_1) == false)
    {
        return true;
    }
    else
        return false;
}

/* calculate Stick position */
int Joystick::calculatePositionY0() // 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 200)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        COOR_Y0 -= 1;
        if (COOR_Y0 <= 0) COOR_Y0 = 0;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= 0) COOR_Y0 = 0;
        return COOR_Y0;
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 200)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        COOR_Y0 += 1;
        if (COOR_Y0 >= 63) COOR_Y0 = 63;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= 63) COOR_Y0 = 63;
        return COOR_Y0;
    }
    else
        return COOR_Y0;
}

int Joystick::calculatePositionY1() // 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 200)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        COOR_Y1 -= 1;
        if(COOR_Y1 <= 0) COOR_Y1 = 0;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        COOR_Y1 -= 2;
        if(COOR_Y1 <= 0) COOR_Y1 = 0;
        return COOR_Y1;
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 200)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        COOR_Y1 += 1;
        if(COOR_Y1 >= 63) COOR_Y1 = 63;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        COOR_Y1 += 2;
        if(COOR_Y1 >= 63) COOR_Y1 = 63;
        return COOR_Y1;
    }
    else
        return COOR_Y1;
}

int Joystick::calculatePositionX0() // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 200)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        COOR_X0 += 1;
        if(COOR_X0 >= 127) COOR_X0 = 127;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        COOR_X0 += 2;
        if(COOR_X0 >= 127) COOR_X0 = 127;
        return COOR_X0;
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 200)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        COOR_X0 -= 1;
        if(COOR_X0 >= 0) COOR_X0 = 0;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        COOR_X0 -= 2;
        if(COOR_X0 >= 0) COOR_X0 = 0;
        return COOR_X0;
    }
    else
        return COOR_X0;
}

int Joystick::calculatePositionX1() // 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 200)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        COOR_X1 += 1;
        if(COOR_X1 >= 127) COOR_X1 = 127;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        COOR_X1 += 2;
        if(COOR_X1 >= 127) COOR_X1 = 127;
        return COOR_X1;
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 200)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        COOR_X1 -= 1;
        if(COOR_X1 >= 0) COOR_X1 = 0;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        COOR_X1 -= 2;
        if(COOR_X1 >= 0) COOR_X1 = 0;
        return COOR_X1;
    }
    else
        return COOR_X1;
}

//new
/* calculate Stick position */
int Joystick::calculatePositionY0(short min, short max) // 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 200)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        COOR_Y0 -= 1;
        if (COOR_Y0 <= min) COOR_Y0 = min;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= min) COOR_Y0 = min;
        return COOR_Y0;
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 200)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        COOR_Y0 += 1;
        if (COOR_Y0 >= max) COOR_Y0 = max;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= max) COOR_Y0 = max;
        return COOR_Y0;
    }
    else
        return COOR_Y0;
}

int Joystick::calculatePositionY1(short min, short max) // 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 200)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        COOR_Y1 -= 1;
        if(COOR_Y1 <= min) COOR_Y1 = min;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        COOR_Y1 -= 2;
        if(COOR_Y1 <= min) COOR_Y1 = min;
        return COOR_Y1;
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 200)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        COOR_Y1 += 1;
        if(COOR_Y1 >= max) COOR_Y1 = max;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        COOR_Y1 += 2;
        if(COOR_Y1 >= max) COOR_Y1 = max;
        return COOR_Y1;
    }
    else
        return COOR_Y1;
}

int Joystick::calculatePositionX0(short min, short max) // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 200)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        COOR_X0 += 1;
        if(COOR_X0 >= max) COOR_X0 = max;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        COOR_X0 += 2;
        if(COOR_X0 >= max) COOR_X0 = max;
        return COOR_X0;
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 200)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        COOR_X0 -= 1;
        if(COOR_X0 <= min) COOR_X0 = min;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        COOR_X0 -= 2;
        if(COOR_X0 <= min) COOR_X0 = min;
        return COOR_X0;
    }
    else
        return COOR_X0;
}

int Joystick::calculatePositionX1(short min, short max) // 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 200)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        COOR_X1 += 1;
        if(COOR_X1 >= max) COOR_X1 = max;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        COOR_X1 += 2;
        if(COOR_X1 >= max) COOR_X1 = max;
        if(COOR_X1 >= 127) COOR_X1 = 127;
        return COOR_X1;
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 200)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        COOR_X1 -= 1;
        if(COOR_X1 <= min) COOR_X1 = min;
        if(COOR_X1 >= 0) COOR_X1 = 0;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        COOR_X1 -= 2;
        if(COOR_X1 <= min) COOR_X1 = min;
        if(COOR_X1 >= 0) COOR_X1 = 0;
        return COOR_X1;
    }
    else
        return COOR_X1;
}

//new
/* calculate Stick position */
int Joystick::calculatePositionY0(short min, short max) // 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 200)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        COOR_Y0 -= 1;
        if (COOR_Y0 <= min) COOR_Y0 = min;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= min) COOR_Y0 = min;
        return COOR_Y0;
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 200)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        COOR_Y0 += 1;
        if (COOR_Y0 >= max) COOR_Y0 = max;
        return COOR_Y0;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= max) COOR_Y0 = max;
        return COOR_Y0;
    }
    else
        return COOR_Y0;
}

int Joystick::calculatePositionY1(short min, short max) // 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 200)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        COOR_Y1 -= 1;
        if(COOR_Y1 <= min) COOR_Y1 = min;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        COOR_Y1 -= 2;
        if(COOR_Y1 <= min) COOR_Y1 = min;
        return COOR_Y1;
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 200)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        COOR_Y1 += 1;
        if(COOR_Y1 >= max) COOR_Y1 = max;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        COOR_Y1 += 2;
        if(COOR_Y1 >= max) COOR_Y1 = max;
        return COOR_Y1;
    }
    else
        return COOR_Y1;
}

int Joystick::calculatePositionX0(short min, short max) // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 200)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        COOR_X0 += 1;
        if(COOR_X0 >= max) COOR_X0 = max;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        COOR_X0 += 2;
        if(COOR_X0 >= max) COOR_X0 = max;
        return COOR_X0;
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 200)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        COOR_X0 -= 1;
        if(COOR_X0 >= min) COOR_X0 = min;
        return COOR_X0;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        COOR_X0 -= 2;
        if(COOR_X0 >= min) COOR_X0 = min;
        return COOR_X0;
    }
    else
        return COOR_X0;
}

int Joystick::calculatePositionX1(short min, short max) // 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 200)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        COOR_X1 += 1;
        if(COOR_X1 >= max) COOR_X1 = max;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        COOR_X1 += 2;
        if(COOR_X1 >= max) COOR_X1 = max;
        return COOR_X1;
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 200)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        COOR_X1 -= 1;
        if(COOR_X1 >= min) COOR_X1 = min;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        COOR_X1 -= 2;
        if(COOR_X1 >= min) COOR_X1 = min;
        return COOR_X1;
    }
    else
        return COOR_X1;
}

/* Updating Stick coordinates */
void Joystick::updatePositionXY()
{
    posX0 = calculatePositionX0();
    posX1 = calculatePositionX1();
    posY0 = calculatePositionY0();
    posY1 = calculatePositionY1();
}

void Joystick::updatePositionXY(short min, short max) //in all Stick 2 parameters
{
    posX0 = calculatePositionX0(min, max);
    posX1 = calculatePositionX1(min, max);
    posY0 = calculatePositionY0(min, max);
    posY1 = calculatePositionY1(min, max);
}

void Joystick::updatePositionXY(short minX, short maxX, short minY, short maxY) //in all Sticks 4 parameters
{
    posX0 = calculatePositionX0(minX, maxX);
    posX1 = calculatePositionX1(minX, maxX);
    posY0 = calculatePositionY0(minY, maxY);
    posY1 = calculatePositionY1(minY, maxY);
}

void Joystick::updatePositionX0Y0(short minX0, short maxX0, short minY0, short maxY0) //in Stick 0
{
    posX0 = calculatePositionX0(minX0, maxX0);
    posY0 = calculatePositionY0(minY0, maxY0);
}

void Joystick::updatePositionX1Y1(short minX1, short maxX1, short minY1, short maxY1) //in Stick 1
{
    posX1 = calculatePositionX1(minX1, maxX1);
    posY1 = calculatePositionY1(minY1, maxY1);
}
void Joystick::updatePositionX0Y0(short minX, short maxX, short minY, short maxY) //in Stick 0
{
    posX0 = calculatePositionX0(minX, maxX);
    posY0 = calculatePositionY0(minY, maxY);
}

void Joystick::updatePositionX1Y1(short minX, short maxX, short minY, short maxY) //in Stick 1
{
    posX1 = calculatePositionX1(minX, maxX);
    posY1 = calculatePositionY1(minY, maxY);
}

/* Calculate position index */
int8_t Joystick::calculateIndexY0() // obj 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 200)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        return OBJ_Y0 = OBJ_Y0 - 1;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        return OBJ_Y0 = OBJ_Y0 - 1; // 2
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 200)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        return OBJ_Y0 = OBJ_Y0 + 1;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        return OBJ_Y0 = OBJ_Y0 + 1; // 2
    }
    else
        return OBJ_Y0 = 0;
}

int8_t Joystick::calculateIndexY1() // obj 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 200)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        return OBJ_Y1 = OBJ_Y1 - 1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        return OBJ_Y1 = OBJ_Y1 - 1; // 2
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 200)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        return OBJ_Y1 = OBJ_Y1 + 1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        return OBJ_Y1 = OBJ_Y1 + 1; // 2
    }
    else
        return OBJ_Y1 = 0;
}

int8_t Joystick::calculateIndexX0() // obj 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 200)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        return OBJ_X0 = OBJ_X0 - 1;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        return OBJ_X0 = OBJ_X0 - 1; // 2
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 200)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        return OBJ_X0 = OBJ_X0 + 1;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        return OBJ_X0 = OBJ_X0 + 1; // 2
    }
    else
        return OBJ_X0 = 0;
}

int8_t Joystick::calculateIndexX1() // obj 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 200)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        return OBJ_X1 = OBJ_X1 - 1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        return OBJ_X1 = OBJ_X1 - 1; // 2
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 200)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        return OBJ_X1 = OBJ_X1 + 1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        return OBJ_X1 = OBJ_X1 + 1; // 2
    }
    else
        return OBJ_X1 = 0;
}

/* Timer */
void Timer::timer(void (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}

/* terminal */
/* prototype */
void clearCommandTerminal();

/* command type */
struct Command
{
    char const *text;
    void (*f)();
    bool active;
};

/* enumeration of objects - commands */
Command commands[]
{
    {"clrcom", clearCommandTerminal, false},
};

/* delete all commands */
void clearCommandTerminal()
{
  for (Command &command : commands)
  {
    command.active = false;
  }
}

/* command stack */
void calcTerminal()
{
  for (Command &command : commands)
  {
    if (command.active)
    {
      command.f();
    }
  }
}

/* pushing data onto the stack */
void Terminal::terminal()
{
  calcTerminal();

  if (Serial.available() != 0)
  {
    char text[10]{};
    Serial.readBytesUntil('\n', text, sizeof(text));

    for (Command &command : commands)
    {
      if (not strncmp(command.text, text, 10))
      {
        command.active = true;
      }
    }
  }
}

/* Screensaver */
/* The function checks whether the joystick or button is pressed at a certain moment */
bool Screensaver::isTouched()
{
  if ((calculateIndexY0() == 0) && (calculateIndexY1() == 0) && 
      (calculateIndexX0() == 0) && (calculateIndexX1() == 0) && 
      (pressKeyA() == 0) && (pressKeyB() == 0))
  {
    return true;
  }

  return false;
}

/* Shows a notification about the start of sleep mode */
void sleepModeText()
{
  if (millis() - sleepModeTexCirculation >= 5000)
  {
    sleepModeTexCirculation = millis();

    sleepTextX = random(0, 98);   // Sleep (5 character) --> 128 px - (5 char * 6 px)
    sleepTextY = random(10, 64);  // 64 px - 10 px
  }

  _gfx.print("Sleep", sleepTextX, sleepTextY, 8, 6);
}

/* Turns off the backlight and turns on an infinite loop
   with the text to pause until the joysticks are pressed or moved */
void Screensaver::screensaver(bool state, uint timeUntil)
{
  if ((state == true))
  {
    if (!isTouched())
    {
      screenTiming = millis();
    }

    if (millis() - screenTiming > timeUntil)
    {
      screenTiming = millis();

      digitalWrite(PIN_BACKLIGHT_LCD, false);

      sleepModeTexCirculation = millis();

      while (isTouched() == true)
      {
        _gfx.render(sleepModeText);
      }

      digitalWrite(PIN_BACKLIGHT_LCD, true);
    }
  }
}
                                                                                                                                               