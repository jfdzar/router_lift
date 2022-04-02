#include <AiEsp32RotaryEncoder.h>
#include <OneButton.h>

// Encoder
#define ROTARY_ENCODER_STEPS 1
#define ENCAPIN 32
#define ENCBPIN 35
#define ENCBUTTON 34
#define ENCINITPOS 0

// Buttons and LEDs
#define BUTTONAPIN 27
#define LEDGREEN 4
#define LEDRED 23

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
OneButton button_enc(ENCBUTTON, false, false);

bool lockedControls = false;