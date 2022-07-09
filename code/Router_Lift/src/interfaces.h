#include <AiEsp32RotaryEncoder.h>
#include <OneButton.h>

// Encoder
#define ROTARY_ENCODER_STEPS 1
#define ENCAPIN 15
#define ENCBPIN 2 // Same as built in LEDPIN
#define ENCBUTTON 4
#define ENCINITPOS 0

#define ENCLOWLIMIT -5000 // -10 * 100
#define ENCUPPLIMIT 20000 // 200 * 100
#define ENCSPEEDUPPLIMIT 8000

// Buttons and LEDs
#define BUTTONAPIN 39
//#define BUTTONAPIN 34 test
#define BUTTONBPIN 36
#define LEDGREEN 13
#define LEDRED 12

// End Stops
#define ENDSTOPUP 35
#define ENDSTOPDOWN 34

#define MODE_POSITION 0
#define MODE_SPEED 1
#define MODE_CURRENT 2
#define MODE_USTEP 3

void serialEvent();
void IRAM_ATTR readEncoderISR();
void IRAM_ATTR toggleLED();

void startRouter();
void lockControls();
void moveStepper();
void saveZeroPosition();

// Encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCAPIN, ENCBPIN, ENCBUTTON, -1, ROTARY_ENCODER_STEPS);

// Inputs & Outputs
OneButton button_a(BUTTONAPIN, true, true);
OneButton button_b(BUTTONBPIN, true, true);
OneButton button_enc(ENCBUTTON, true, true);

bool lockedControls = false;

uint8_t mode = 0;