/*
  Contains function settings for working with the display.

  [!] Required u8g2 library
  [!] bmp to xbmp image converter https://www.online-utility.org/image/convert/to/XBM
*/

#include <Arduino.h>
#include <U8g2lib.h> 
#include "mg.h"

Graphics _gfx;

unsigned long previousMillis = 0;
unsigned long prevTime_0{};
const long interval = 300;

/* cursor */
#define cursor_w 7
#define cursor_h 8
const uint8_t cursor_bits[] = {
  0x03, 0x05, 0x09, 0x11, 0x21, 0x79, 0x05, 0x03, };

/* logotype makegame */
#define mg_l_w 56
#define mg_l_h 28
const uint8_t mg_l_bits[] = {
  0x0C, 0x0C, 0xFC, 0xC0, 0xF3, 0xF0, 0x3F, 0x0C, 0x0C, 0xFC, 0xC0, 0xF3, 
  0xF0, 0x3F, 0x33, 0x33, 0x03, 0x33, 0x0C, 0x0F, 0xC0, 0x33, 0x33, 0x03, 
  0x33, 0x0C, 0x0F, 0xC0, 0xC3, 0xF0, 0x30, 0x3C, 0xC0, 0x0C, 0x3C, 0xC3, 
  0xF0, 0x30, 0x3C, 0xC0, 0x0C, 0x3C, 0x03, 0xF0, 0x00, 0x3C, 0x30, 0x0C, 
  0x30, 0x03, 0xF0, 0x30, 0x3C, 0x30, 0x0C, 0x30, 0x33, 0xF3, 0x30, 0x3C, 
  0xC0, 0x0C, 0x3C, 0x33, 0xF3, 0x30, 0x3C, 0xC0, 0x0C, 0x3C, 0xF3, 0xF3, 
  0x30, 0x3C, 0x0C, 0x0F, 0xC0, 0xF3, 0xF3, 0x30, 0x3C, 0x0C, 0x0F, 0xC0, 
  0x0C, 0x0C, 0xCF, 0xC3, 0xF3, 0xF0, 0x3F, 0x0C, 0x0C, 0xCF, 0xC3, 0xF3, 
  0xF0, 0x3F, 0xF0, 0x0F, 0xFC, 0xC0, 0xC0, 0xF0, 0x3F, 0xF0, 0x0F, 0xFC, 
  0xC0, 0xC0, 0xF0, 0x3F, 0x0C, 0x30, 0x03, 0x33, 0x33, 0x0F, 0xC0, 0x0C, 
  0x30, 0x03, 0x33, 0x33, 0x0F, 0xC0, 0xC3, 0xCF, 0x30, 0x3C, 0x0C, 0x0F, 
  0x3C, 0xC3, 0xCF, 0x30, 0x3C, 0x0C, 0x0F, 0x3C, 0xC3, 0xF0, 0x00, 0x3C, 
  0x00, 0x0F, 0x30, 0xC3, 0xF0, 0x30, 0x3C, 0x00, 0x0F, 0x30, 0xC3, 0xF3, 
  0x30, 0x3C, 0x33, 0x0F, 0x3C, 0xC3, 0xF3, 0x30, 0x3C, 0x33, 0x0F, 0x3C, 
  0x0C, 0xF0, 0x30, 0x3C, 0x3F, 0x0F, 0xC0, 0x0C, 0xF0, 0x30, 0x3E, 0x3F, 
  0x0F, 0xC0, 0xF0, 0x3F, 0xCF, 0xC3, 0xC0, 0xF0, 0x3F, 0xF0, 0x3F, 0xCF, 
  0xC3, 0xC0, 0xF0, 0x3F, };

/* logotype terminal */
#define trm_l_w 11
#define trm_l_h 9
const uint8_t trm_l_bits[] = {
  0xFE, 0x03, 0x01, 0x04, 0x05, 0x04, 0x09, 0x04, 0x11, 0x04, 0x09, 0x04, 
  0xC5, 0x05, 0x01, 0x04, 0xFE, 0x03, };

/* qr-code @avsavushkiny/makegame-board */
#define qr_w 33
#define qr_h 33
const uint8_t qr_bits[] = {
  0x7F, 0x2F, 0xBB, 0xFC, 0x01, 0x41, 0x25, 0xB5, 0x05, 0x01, 0x5D, 0x18, 
  0x53, 0x75, 0x01, 0x5D, 0x55, 0x51, 0x74, 0x01, 0x5D, 0x72, 0x12, 0x75, 
  0x01, 0x41, 0x5E, 0x7A, 0x05, 0x01, 0x7F, 0x55, 0x55, 0xFD, 0x01, 0x00, 
  0x59, 0x96, 0x00, 0x00, 0xED, 0x4E, 0x7F, 0xA5, 0x01, 0x2F, 0x5D, 0xBD, 
  0x6C, 0x01, 0x69, 0x59, 0xC3, 0xBA, 0x01, 0xA3, 0xE7, 0x4D, 0x28, 0x00, 
  0xF9, 0x12, 0xA4, 0xBB, 0x01, 0x0C, 0x2B, 0x27, 0xA9, 0x00, 0x6C, 0xD1, 
  0x21, 0x3D, 0x00, 0x29, 0x78, 0xB9, 0x6E, 0x00, 0xCF, 0xD3, 0x7C, 0x76, 
  0x00, 0x9E, 0xCF, 0xF6, 0xB7, 0x00, 0x55, 0x92, 0x52, 0xDB, 0x00, 0x26, 
  0x6B, 0xA8, 0x97, 0x00, 0xEF, 0x96, 0x14, 0x62, 0x01, 0x8B, 0xD0, 0xA4, 
  0x04, 0x01, 0x64, 0x4F, 0xA5, 0xDE, 0x01, 0x8E, 0x54, 0x89, 0xBC, 0x01, 
  0x6D, 0xDB, 0x42, 0x9F, 0x01, 0x00, 0xC7, 0x1F, 0x31, 0x00, 0x7F, 0x69, 
  0xD8, 0x95, 0x00, 0x41, 0x69, 0xF0, 0xF1, 0x00, 0x5D, 0x14, 0x9A, 0x5F, 
  0x00, 0x5D, 0x03, 0xBC, 0xA8, 0x01, 0x5D, 0x05, 0x48, 0x6C, 0x00, 0x41, 
  0xDC, 0xCF, 0x1B, 0x01, 0x7F, 0xE3, 0xA6, 0x50, 0x00, };

/* icon: icon */
#define icon_wi 8
#define icon_h 8
const uint8_t icon_bits[] = {
  0xFF, 0x81, 0xBD, 0xB9, 0xBD, 0xAD, 0x81, 0xFF, };

/* icon: gears */
#define gears_w 32
#define gears_h 32
const uint8_t gears_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x60, 0x5A, 0x06, 0x00, 
  0x90, 0xC3, 0x09, 0x00, 0x10, 0x42, 0x08, 0x00, 0x30, 0x42, 0x0C, 0x00, 
  0x30, 0xC3, 0x0C, 0x00, 0x9C, 0x42, 0x39, 0x00, 0x02, 0xC3, 0x40, 0x00, 
  0x1E, 0x7E, 0x78, 0x00, 0x12, 0x00, 0x84, 0x00, 0x1C, 0xC3, 0x84, 0x0C, 
  0xB0, 0x24, 0xB7, 0x13, 0xD0, 0x25, 0x84, 0x10, 0xD0, 0x7E, 0x84, 0x18, 
  0x60, 0x64, 0x86, 0x19, 0x00, 0x3C, 0x85, 0x72, 0x00, 0x04, 0x86, 0x81, 
  0x00, 0x3C, 0xFC, 0xF0, 0x00, 0x2C, 0x00, 0xB0, 0x00, 0x38, 0x86, 0x71, 
  0x00, 0x60, 0xCF, 0x1B, 0x00, 0xA0, 0xCF, 0x17, 0x00, 0xA0, 0xFD, 0x16, 
  0x00, 0xC0, 0x48, 0x0C, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

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
    //setting display, contrast
    u8g2.begin();
    u8g2.setContrast(0);
    //setting the resolution of the analog-to-digital converter
    analogReadResolution(RESOLUTION_ADC);
    //display backlight
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);
    digitalWrite(PIN_BACKLIGHT_LCD, true);
    //platform logo output
    u8g2.clearBuffer();
    u8g2.drawXBMP(((WIDTH_LCD - mg_l_w)/2), ((HEIGHT_LCD - mg_l_h)/2), mg_l_w, mg_l_h, mg_l_bits); //56x28 px
    u8g2.sendBuffer();
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
/* text output with parameters, add line interval (def: 10) and character interval (def: 6) */
void Graphics::print(String text, int x, int y, int lii, int chi) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int sizeText = text.length() + 1;
    int yy{0};

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setFont(u8g2_font_6x10_tr);
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

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setFont(u8g2_font_6x10_tr);
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

/* Joystic */
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
        return COOR_Y0 = COOR_Y0 - 1;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        return COOR_Y0 = COOR_Y0 - 2;
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 200)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        return COOR_Y0 = COOR_Y0 + 1;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        return COOR_Y0 = COOR_Y0 + 2;
    }
    else
        return COOR_Y0;
}

int Joystick::calculatePositionY1() // 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 200)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        return COOR_Y1 = COOR_Y1 - 1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        return COOR_Y1 = COOR_Y1 - 2;
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 200)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        return COOR_Y1 = COOR_Y1 + 1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        return COOR_Y1 = COOR_Y1 + 2;
    }
    else
        return COOR_Y1;
}

int Joystick::calculatePositionX0() // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 200)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        return COOR_X0 = COOR_X0 + 1;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        return COOR_X0 = COOR_X0 + 2;
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 200)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        return COOR_X0 = COOR_X0 - 1;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        return COOR_X0 = COOR_X0 - 2;
    }
    else
        return COOR_X0;
}

int Joystick::calculatePositionX1() // 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 200)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        return COOR_X1 = COOR_X1 + 1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        return COOR_X1 = COOR_X1 + 2;
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 200)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        return COOR_X1 = COOR_X1 - 1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        return COOR_X1 = COOR_X1 - 2;
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

/* timer */
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