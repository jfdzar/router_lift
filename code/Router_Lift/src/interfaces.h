#include <AiEsp32RotaryEncoder.h>
#include <OneButton.h>

// Encoder
#define ROTARY_ENCODER_STEPS 1
#define ENCAPIN 15
#define ENCBPIN 2 // Same as built in LEDPIN
#define ENCBUTTON 4
#define ENCINITPOS 0

#define ENCLOWLIMIT -5000 // -10 * 100
#define ENCUPPLIMIT 10000 // 100 * 100

// Buttons and LEDs
#define BUTTONAPIN 39
#define BUTTONBPIN 36
#define LEDGREEN 27
#define LEDRED 14

void serialEvent();
void IRAM_ATTR readEncoderISR();

void startRouter();
void lockControls();
void moveStepper();
void saveZeroPosition();

// Encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCAPIN, ENCBPIN, ENCBUTTON, -1, ROTARY_ENCODER_STEPS);

// Inputs & Outputs
OneButton button_a(BUTTONAPIN, true, true);
OneButton button_b(BUTTONBPIN, true, true);
OneButton button_enc(ENCBUTTON, false, false);

bool lockedControls = false;