#ifndef DISPLAY_H /* include guards */
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //   QT-PY / XIAO

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

class RouterLiftDisplay
{
public:
    RouterLiftDisplay()
    {
    }
    void init()
    {
        // Display
        //  Show image buffer on the display hardware.
        //  Since the buffer is intialized with an Adafruit splashscreen
        //  internally, this will display the splashscreen.
        _display.begin(i2c_Address, true); // Address 0x3C default
        _display.setContrast(0);           // dim display
        _display.display();
        delay(250);
    }

    void poweron_screen()
    {
        // text display tests
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 10);
        _display.println("Automatic");
        _display.println("Router Lift");
        // display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
        _display.display();
        delay(1000);
        _display.clearDisplay();
    }

    void draw_position(float position)
    {
        _display.clearDisplay();
        _display.setTextSize(2);
        _display.setCursor(32, 32);
        _display.println(position);
    }

    void draw_locked()
    {
        _display.setTextSize(1);
        _display.setCursor(60, 0);
        _display.println("Locked");
    }

    void draw_moving()
    {
        _display.setTextSize(1);
        _display.setCursor(0, 0);
        _display.println("*");
    }

    void redraw()
    {
        _display.display();
    }

    void testdrawbitmap(Adafruit_SH1106G display, const uint8_t *bitmap, uint8_t w, uint8_t h)
    {
        uint8_t icons[NUMFLAKES][3];

        // initialize
        for (uint8_t f = 0; f < NUMFLAKES; f++)
        {
            icons[f][XPOS] = random(display.width());
            icons[f][YPOS] = 0;
            icons[f][DELTAY] = random(5) + 1;

            Serial.print("x: ");
            Serial.print(icons[f][XPOS], DEC);
            Serial.print(" y: ");
            Serial.print(icons[f][YPOS], DEC);
            Serial.print(" dy: ");
            Serial.println(icons[f][DELTAY], DEC);
        }

        while (1)
        {
            // draw each icon
            for (uint8_t f = 0; f < NUMFLAKES; f++)
            {
                display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_WHITE);
            }
            display.display();
            delay(200);

            // then erase it + move it
            for (uint8_t f = 0; f < NUMFLAKES; f++)
            {
                display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_BLACK);
                // move it
                icons[f][YPOS] += icons[f][DELTAY];
                // if its gone, reinit
                if (icons[f][YPOS] > display.height())
                {
                    icons[f][XPOS] = random(display.width());
                    icons[f][YPOS] = 0;
                    icons[f][DELTAY] = random(5) + 1;
                }
            }
        }
    }

private:
    Adafruit_SH1106G _display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
};

/*

void testdrawchar(void)
{
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    for (uint8_t i = 0; i < 168; i++)
    {
        if (i == '\n')
            continue;
        display.write(i);
        if ((i > 0) && (i % 21 == 0))
            display.println();
    }
    display.display();
    delay(1);
}

void testdrawcircle(void)
{
    for (int16_t i = 0; i < display.height(); i += 2)
    {
        display.drawCircle(display.width() / 2, display.height() / 2, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
}

void testfillrect(void)
{
    uint8_t color = 1;
    for (int16_t i = 0; i < display.height() / 2; i += 3)
    {
        // alternate colors
        display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, color % 2);
        display.display();
        delay(1);
        color++;
    }
}

void testdrawtriangle(void)
{
    for (int16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5)
    {
        display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i, SH110X_WHITE);
        display.display();
        delay(1);
    }
}

void testfilltriangle(void)
{
    uint8_t color = SH110X_WHITE;
    for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5)
    {
        display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i, SH110X_WHITE);
        if (color == SH110X_WHITE)
            color = SH110X_BLACK;
        else
            color = SH110X_WHITE;
        display.display();
        delay(1);
    }
}

void testdrawroundrect(void)
{
    for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
    {
        display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SH110X_WHITE);
        display.display();
        delay(1);
    }
}

void testfillroundrect(void)
{
    uint8_t color = SH110X_WHITE;
    for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
    {
        display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
        if (color == SH110X_WHITE)
            color = SH110X_BLACK;
        else
            color = SH110X_WHITE;
        display.display();
        delay(1);
    }
}

void testdrawrect(void)
{
    for (int16_t i = 0; i < display.height() / 2; i += 2)
    {
        display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SH110X_WHITE);
        display.display();
        delay(1);
    }
}

void testdrawline()
{
    for (int16_t i = 0; i < display.width(); i += 4)
    {
        display.drawLine(0, 0, i, display.height() - 1, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (int16_t i = 0; i < display.height(); i += 4)
    {
        display.drawLine(0, 0, display.width() - 1, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();
    for (int16_t i = 0; i < display.width(); i += 4)
    {
        display.drawLine(0, display.height() - 1, i, 0, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (int16_t i = display.height() - 1; i >= 0; i -= 4)
    {
        display.drawLine(0, display.height() - 1, display.width() - 1, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();
    for (int16_t i = display.width() - 1; i >= 0; i -= 4)
    {
        display.drawLine(display.width() - 1, display.height() - 1, i, 0, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (int16_t i = display.height() - 1; i >= 0; i -= 4)
    {
        display.drawLine(display.width() - 1, display.height() - 1, 0, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();
    for (int16_t i = 0; i < display.height(); i += 4)
    {
        display.drawLine(display.width() - 1, 0, 0, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (int16_t i = 0; i < display.width(); i += 4)
    {
        display.drawLine(display.width() - 1, 0, i, display.height() - 1, SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);
}
*/

/*
void testdrawchar(void);

void testdrawcircle(void);

void testfillrect(void);

void testdrawtriangle(void);

void testfilltriangle(void);

void testdrawroundrect(void);

void testfillroundrect(void);

void testdrawrect(void);

void testdrawline();
*/
#endif