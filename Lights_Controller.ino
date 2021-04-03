/* Optimizaciones:
 *  CRITICOS:
 *  1 - sacar todos los serial print
 *  2 - cambiar el bt a un hardware serial
 *  LEVES:
 *  1 - cambiar variables a static y volatile
 *  2 - mover codigo a archivos .c y .h
 *  3 - debounce de boton con timers
 */

/*  Autor: Ezequiel Ledesma
 *  Fecha: 04/2021
 *  MCU: Arduino Nano
 *  Modulo BT: HC-05
 */
extern const uint8_t gamma8[];

#include <SoftwareSerial.h>
SoftwareSerial btSerial(10, 11); // Rx, Tx
String code;
char buff = ' ';

#include <stdint.h>
#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    120
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define BRIGHTNESS 255
CRGB leds[NUM_LEDS];
#define BUTTON 2

// Inicializaciones
void leds_init();
void entrys_init();
void timer2_init();
// Funciones
void cleanVar();
void update_bt();
void update_state();
void update_leds();
void update_entrys();
void snake(byte red, byte green, byte blue, byte background);
void snake2();
void mirror();
void orbital(byte red, byte green, byte blue);
void flash(byte red, byte green, byte blue);
void rainbow();
void rainbow_cycle();
// Drivers
void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);
void test();

// Variables de Estado
#define REST 0
#define STATIC_COLOR 1
#define FADE_COLOR 2
#define SNAKE 3
#define MIRROR 4
#define ORBITAL 5
#define FLASH 6
#define RAINBOW 7
#define RAINBOW_CYCLE 8
#define KITT 9
#define RGBLOOP 10
#define FADEINOUT 11
#define STROBE 12
#define EYES 13
#define TWINKLE 14
#define SPARKLE 15
#define RUNNING_LIGHTS 16
#define THEATER_RAINBOW 17
#define FIRE 18
#define METEOR 19
#define BOUNCING_BALLS 20
#define OFF 90
#define TEST 100

// Variables Globales
byte colorRGB[3] = {255, 0, 0};
uint8_t state = SNAKE;
boolean bFlag = false;

uint8_t timer2_ticks = 20;                  // Frecuencia de actualizacion (1 ~ 1ms)
uint16_t timer2_count = 0;
boolean timer2_flag = false;
boolean testflag = false;

void setup() {
  delay(100);
  leds_init();
  entrys_init();
  timer2_init();
  btSerial.begin(19200);
  Serial.begin(9600);                       // Serial monitor para debugging
}

void leds_init() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
}

void entrys_init() {
  pinMode(BUTTON, INPUT_PULLUP);  
}

void timer2_init() {
  SREG = (SREG & 0b01111111);              // Deshabilitar interrupciones globales
  TCNT2 = 0;                               // Limpiar el registro que guarda la cuenta del Timer-2.
  TIMSK2 = TIMSK2 | 0b00000001;            // Habilitación de la bandera 0 del registro que habilita la interrupción por sobre flujo o desbordamiento del TIMER2.
  TCCR2B = 0b00000011;                     // ft2 = 250Khz => 1ms ( (1/250.000) * 255 )
  SREG = (SREG & 0b01111111) | 0b10000000; // Habilitar interrupciones
}

ISR(TIMER2_OVF_vect) {
  timer2_count++;
  if (timer2_count == timer2_ticks) {
    timer2_flag = true;
    timer2_count = 0;
  }
}


void loop() {
  update_leds();
  //update_entrys();
  update_bt();
  update_state();
  /*
  Serial.print(" // STATE: ");
  Serial.print(state);
  Serial.print(" - BRIGTHNESS: ");
  Serial.print(FastLED.getBrightness());
  Serial.print(" - RED: ");
  Serial.print(colorRGB[0]);
  Serial.print(" - GREEN: ");
  Serial.print(colorRGB[1]);
  Serial.print(" - BLUE: ");
  Serial.print(colorRGB[2]);
  Serial.println("");
  */
}

void test() {
  static uint8_t j = 0;
  while (j < NUM_LEDS) {
    setPixel(j, 255, 255, 0);
    j++;
  }
}

void update_leds() {
  if(!timer2_flag) return;
  timer2_flag = false;
  switch (state) {
    case REST:
      state++;
      break;
    case STATIC_COLOR:
      setAll(colorRGB[0], colorRGB[1], colorRGB[2]);
      break;
    case SNAKE:
      snake(colorRGB[0], colorRGB[1], colorRGB[2], 30);
      break;
    case MIRROR:
      mirror();
      break;
    case ORBITAL:
      orbital(colorRGB[0], colorRGB[1], colorRGB[2]);
      break;
    case FLASH:
      flash(colorRGB[0], colorRGB[1], colorRGB[2]);
      break;
    case RAINBOW:
      rainbow();
      break;
    case RAINBOW_CYCLE:
      rainbow_cycle();
      break;
    case KITT:
      kitt(colorRGB[0], colorRGB[1], colorRGB[2], 5, 0);
      break;
    case OFF:
      setAll(0, 0, 0);
      break;
    case TEST:
      test();
      break;
    default:
      state = REST;
      break;
  }
  showStrip();
}

void update_entrys () {
  if (digitalRead(BUTTON) == 1 && !bFlag) {
    bFlag = true;
    state++;
    cleanVar();
    delay(100);
  }
  else if (!digitalRead(BUTTON) && bFlag) {
    bFlag = false;
  }
}


// static uint8_t
byte tira_r[NUM_LEDS] = {};
byte tira_g[NUM_LEDS] = {};
byte tira_b[NUM_LEDS] = {};

void snake(byte red, byte green, byte blue, byte background) {
  static uint16_t y = 0;
  static uint16_t newpos = 0;
  y = 0;
  while ( y < (NUM_LEDS/8) ) {
    setPixel((y+newpos)%NUM_LEDS,
             red*(y+1)*((byte)(NUM_LEDS/8))/255,
             green*(y+1)*((byte)(NUM_LEDS/8))/255,
             blue*(y+1)*((byte)(NUM_LEDS/8))/255);
    y++;
  }
  while ( y < ((NUM_LEDS / 4) - 2) ) {
    setPixel((y+newpos)%NUM_LEDS,
              red*(((NUM_LEDS / 4) - 2)-y+1)*((byte)(NUM_LEDS/8))/255,
              green*(((NUM_LEDS / 4) - 2)-y+1)*((byte)(NUM_LEDS/8))/255,
              blue*(((NUM_LEDS / 4) - 2)-y+1)*((byte)(NUM_LEDS/8))/255);
    y++;
  }
  while (y < NUM_LEDS) {
    setPixel((y+newpos)%NUM_LEDS,
              (byte)(red*background/100),
              (byte)(green*background/100),
              (byte)(blue*background/100));
    y++;
  }
  newpos++;
  newpos%=NUM_LEDS;
}


uint8_t colorR[8] = {255, 255, 255, 0, 0, 255, 0};
uint8_t colorG[8] = {255, 0, 255, 0, 255, 0, 255};
uint8_t colorB[8] = {255, 0, 0, 255, 0, 255, 255};
uint8_t s2R = 0;
uint8_t s2G = 0;
uint8_t s2B = 0;
uint8_t fondoR = 15;
uint8_t fondoG = 15;
uint8_t fondoB = 15;
uint8_t s2Sel = 0;
uint8_t estado = 0;
void snake2() {
  // sin terminar
  if (estado == 0) {
    if (s2R < colorR[s2Sel]) s2R++;
    if (s2G < colorR[s2Sel]) s2G++;
    if (s2B < colorR[s2Sel]) s2B++;
    if ( s2R == colorR[s2Sel] && s2G == colorG[s2Sel] && s2B == colorB[s2Sel] ) estado = 1;
  }
  else if (estado == 1) {
    if (s2R) s2R--;
    if (s2G) s2G--;
    if (s2B) s2B--;
    if ( !s2R && !s2G && !s2B ) estado = 2;
  }
  else if (estado == 2) {
    s2R = fondoR;
    s2G = fondoG;
    s2B = fondoB;
  }

  tira_r[0] = s2R;
  tira_g[0] = s2G;
  tira_b[0] = s2B;
  /*
    i = NUM_LEDS-1;
    while (i > 0) {
      tira_r[i] = tira_r[i-1];
      tira_g[i] = tira_g[i-1];
      tira_b[i] = tira_b[i-1];
      i--;
    }*/
}

uint8_t mirrorPos = NUM_LEDS / 2;
uint8_t mirrorColor[3] = {};
uint8_t set = 0;
uint8_t coloresR[] = {255, 0, 0, 255, 0, 255};
uint8_t coloresG[] = {0, 255, 0, 255, 255, 0};
uint8_t coloresB[] = {0, 0, 255, 0, 255, 255};
int8_t mirrorDir = -1;
void mirror() {
  if (mirrorPos > 0 && mirrorPos <= NUM_LEDS / 2) {
    tira_r[mirrorPos] = mirrorColor[0];
    tira_g[mirrorPos] = mirrorColor[1];
    tira_b[mirrorPos] = mirrorColor[2];
    tira_r[NUM_LEDS - mirrorPos] = mirrorColor[0];
    tira_g[NUM_LEDS - mirrorPos] = mirrorColor[1];
    tira_b[NUM_LEDS - mirrorPos] = mirrorColor[2];
    mirrorPos += mirrorDir;
    //Serial.print("mirrorPos: ");
    //Serial.print(mirrorPos);
    //Serial.println("CHAU");
    if (!mirrorPos) {
      tira_r[mirrorPos] = mirrorColor[0];
      tira_g[mirrorPos] = mirrorColor[1];
      tira_b[mirrorPos] = mirrorColor[2];
    }
  }
  else if (!mirrorPos || mirrorPos >= NUM_LEDS / 2) {
    //Serial.print("mirrorColor[0]: ");
    //Serial.print(mirrorColor[0]);
    //Serial.println("HOLA");
    set++;
    set %= 6;
    mirrorColor[0] = coloresR[set];
    mirrorColor[1] = coloresG[set];
    mirrorColor[2] = coloresB[set];
    tira_r[mirrorPos] = mirrorColor[0];
    tira_g[mirrorPos] = mirrorColor[1];
    tira_b[mirrorPos] = mirrorColor[2];
    mirrorDir *= -1;
    mirrorPos += mirrorDir;
  }
}

uint8_t auxR = 0;
uint8_t auxG = 0;
uint8_t auxB = 0;
uint16_t i = 0;
uint8_t stateOrb = 0;
uint16_t orbCont = 0;
uint16_t orbDel = 0;
#define ORB_VEL 2
#define ORB_DEL NUM_LEDS*ORB_VEL
void orbital(uint8_t red, uint8_t green, uint8_t blue) {

  if (auxR == red && auxG == green && auxB == blue && !stateOrb) stateOrb = 2;
  else if ( !auxR && !auxG && !auxB && stateOrb == 1) stateOrb = 2;

  while (orbCont < ORB_VEL) {
    if (stateOrb == 0) {
      if (auxR < red) auxR++;
      else if (auxR > red) auxR--;

      if (auxG < green) auxG++;
      else if (auxG > green) auxG--;

      if (auxB < blue) auxB++;
      else if (auxB > blue) auxB--;
    }
    else if (stateOrb == 1) {
      if (auxR) auxR--;
      if (auxG) auxG--;
      if (auxB) auxB--;
    }
    else if (stateOrb == 2) {
      if (orbDel == ORB_DEL) {
        orbDel = 0;
        if (!auxR && !auxG && !auxB) stateOrb = 0;
        else stateOrb = 1;
      }
      orbDel++;
    }
    orbCont++;
  }
  orbCont = 0;
  //Serial.print("STATEORB: ");
  //Serial.print(stateOrb);
  //Serial.print(" AUXR: ");
  //Serial.print(auxR);
  //Serial.print(" AUXG: ");
  //Serial.print(auxG);
  //Serial.print(" AUXB: ");
  //Serial.print(auxB);
  //Serial.print(" orbdel: ");
  //Serial.println(orbDel);

  tira_r[0] = auxR;
  tira_g[0] = auxG;
  tira_b[0] = auxB;

  i = NUM_LEDS - 1;
  while (i > 0) {
    tira_r[i] = tira_r[i - 1];
    tira_g[i] = tira_g[i - 1];
    tira_b[i] = tira_b[i - 1];
    i--;
  }

}


uint8_t fCont = 0;
uint8_t fState = 0;
#define FLASH_ON 10
#define FLASH_OFF 5
uint8_t fRed = 0;
uint8_t fGreen = 0;
uint8_t fBlue = 0;

void flash(uint8_t red, uint8_t green, uint8_t blue) {
  if (fState == 0) {
    fRed = red;
    fGreen = green;
    fBlue = blue;
    fCont++;
    if (fCont == FLASH_ON) {
      fCont = 0;
      fState = 1;
    }
  }
  else if (fState == 1) {
    fRed = 0;
    fGreen = 0;
    fBlue = 0;
    fCont++;
    if (fCont == FLASH_OFF) {
      fCont = 0;
      fState = 0;
    }
  }
  i = 0;
  while (i < NUM_LEDS) {
    tira_r[i] = fRed;
    tira_g[i] = fGreen;
    tira_b[i] = fBlue;
    i++;
  }
}


uint8_t rRed = 0;
uint8_t rGreen = 0;
uint8_t rBlue = 0;
void update_rainbow();

void rainbow() {
  update_rainbow();
  i = 0;
  while (i < NUM_LEDS) {
    tira_r[i] = rRed;
    tira_g[i] = rGreen;
    tira_b[i] = rBlue;
    i++;
  }
}

#define RED     0
#define D_RED   1
#define GREEN   2
#define D_GREEN 3
#define BLUE    4
#define D_BLUE  5
void update_rainbow() {
  static uint8_t r_estado = RED;
  if (r_estado == RED) {
    if (rRed == 255) r_estado = D_BLUE;
    else rRed++;
  }
  if (r_estado == D_BLUE) {
    if (!rBlue) r_estado = GREEN;
    else rBlue--;
  }
  if (r_estado == GREEN) {
    if (rGreen == 255) r_estado = D_RED;
    else rGreen++;
  }
  if (r_estado == D_RED) {
    if (!rRed) r_estado = BLUE;
    else rRed--;
  }
  if (r_estado == BLUE) {
    if (rBlue == 255) r_estado = D_GREEN;
    else rBlue++;
  }
  if (r_estado == D_GREEN) {
    if (!rGreen) r_estado = RED;
    else rGreen--;
  }
}

#define SPEED 13
#define CYCLE_DEL 3
uint8_t cycleCount = 0;
void rainbow_cycle() {
  if (cycleCount == CYCLE_DEL) {
    cycleCount = 0;
    for (uint16_t i = 0; i < SPEED; i++) {
      update_rainbow();
    }
    tira_r[0] = rRed;
    tira_g[0] = rGreen;
    tira_b[0] = rBlue;

    i = NUM_LEDS - 1;
    while (i > 0) {
      tira_r[i] = tira_r[i - 1];
      tira_g[i] = tira_g[i - 1];
      tira_b[i] = tira_b[i - 1];
      i--;
    }
  }
  else {
    cycleCount++;
  }
}

byte kittState = 0;
byte kittCount = 0;
byte kittCount2 = 0;
void kitt (byte red, byte green, byte blue, byte eyeSize, uint8_t speedDelay) {
  if (kittState == 0) { // RIGHT TO LEFT
    if (kittCount < eyeSize) {
      kittCount++;
      setPixel(0, red, green, blue);
    }
    else {
      setPixel(0, 0, 0, 0);
    }
    for (byte i = NUM_LEDS - 1; i > 0; i--) {
      leds[i].r = leds[i - 1].r;
      leds[i].g = leds[i - 1].g;
      leds[i].b = leds[i - 1].b;
    }
    kittCount2++;
    if (kittCount2 == NUM_LEDS) {
      kittCount2 = 0;
      kittState = 1;
    }
  }
  else if (kittState == 1) {
    for (byte i = 0; i < NUM_LEDS - 1; i++) {
      leds[i].r = leds[i + 1].r;
      leds[i].g = leds[i + 1].g;
      leds[i].b = leds[i + 1].b;
    }
    setPixel(NUM_LEDS - 1, 0, 0, 0);
    kittCount2++;
    if (kittCount2 == NUM_LEDS - eyeSize) {
      kittCount2 = 0;
      kittState = 2;
    }
  }
  else if (kittState == 2) {
    for (byte i = NUM_LEDS - 1; i > 0; i--) {
      leds[i].r = leds[i - 1].r;
      leds[i].g = leds[i - 1].g;
      leds[i].b = leds[i - 1].b;
      leds[NUM_LEDS - 1 - i].r = leds[i - 1].r;
      //leds[NUM_LEDS-1-i].r = leds[i-1].r;
      //leds[NUM_LEDS-1-i].g = leds[i-1].g;
      //leds[NUM_LEDS-1-i].b = leds[i-1].b;
    }
    setPixel(0, 0, 0, 0);
    kittCount2++;
    if (kittCount2 == NUM_LEDS / 2 - eyeSize) {
      kittCount2 = 0;
      kittState = 3;
    }
  }
  else if (kittState == 3) {

  }/*
  Serial.print("State: ");
  Serial.print(kittState);
  Serial.print("- COUNT: ");
  Serial.print(kittCount);
  Serial.print("- COUNT2: ");
  Serial.print(kittCount2);
  Serial.println();*/
}

void cleanVar() {
  tira_r[NUM_LEDS] = {};
  tira_g[NUM_LEDS] = {};
  tira_b[NUM_LEDS] = {};
  s2R = 0;
  s2G = 0;
  s2B = 0;
  fondoR = 15;
  fondoG = 15;
  fondoB = 15;
  s2Sel = 0;
  estado = 0;
  mirrorPos = NUM_LEDS / 2;
  set = 0;
  mirrorDir = -1;
  auxR = 0;
  auxG = 0;
  auxB = 0;
  i = 0;
  stateOrb = 0;
  orbCont = 0;
  orbDel = 0;
}


/////////////////////
// BT TRANSMISSION //
/////////////////////

#define LEIDO 0
#define SIN_LEER 1

uint8_t bt = LEIDO;
uint8_t change = 0;
boolean change_flag = false;
byte colorRGB_aux[3] = {};


void update_bt() {
  while (btSerial.available()) {
    buff = btSerial.read();
    /*Serial.print(" // BUFF: ");
    Serial.print(buff);
    Serial.println("");*/
    if (buff == '#') {
      code = "";
      bt = LEIDO;
    }
    else if (buff == '$') {
      Serial.print("Code: ");
      Serial.println(code);
      bt = SIN_LEER;
      break;
    }
    else if (buff == '%') {
      if (change == 1) colorRGB[0] = colorRGB_aux[0];
      else if (change == 2) colorRGB[1] = colorRGB_aux[1];
      else if (change == 3) colorRGB[2] = colorRGB_aux[2];
      change = 0;
      change_flag = false;
      break;
    }
    else if (change_flag) {
      // error de datos
      change_flag = false;
      change = 0;
      break;
    }
    else if (buff == 'r') {
      change = 1;
      break;
    }
    else if (buff == 'g') {
      change = 2;
      break;
    }
    else if (buff == 'b') {
      change = 3;
      break;
    }
    else {
      if (change == 1) {
        colorRGB_aux[0] = buff;
        change_flag = true;
      }
      else if (change == 2) {
        colorRGB_aux[1] = buff;
        change_flag = true;
      }
      else if (change == 3) {
        colorRGB_aux[2] = buff;
        change_flag = true;
      }
      else code += buff;
    }
  }
}

void update_state() {
  if (bt == LEIDO) return;
  if (code == "off") state = OFF;
  else if (code == "es") state = STATIC_COLOR;
  else if (code == "a") state = RAINBOW;
  else if (code == "pi") state = SNAKE;
  else if (code == "li") state = MIRROR;
  else if (code == "of") state = KITT;
  else if (code == "st") state = ORBITAL;
  else if (code == "l+" && (FastLED.getBrightness() <= 225)) FastLED.setBrightness(FastLED.getBrightness() + 30);
  else if (code == "l-" && (FastLED.getBrightness() >= 31)) FastLED.setBrightness(FastLED.getBrightness() - 30);
  else if (code == "l0") FastLED.setBrightness(255);
  else if (code == "p-" && (timer2_ticks <= 45)) timer2_ticks += 5;
  else if (code == "p+" && (timer2_ticks >= 10)) timer2_ticks -= 5;
  else if (code == "p0") timer2_ticks = 10;
  bt = LEIDO;
  cleanVar();
  // Limpiar cartel?
}


/////////////
// DRIVERS //
/////////////

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = pgm_read_byte(&gamma8[red]);
  leds[Pixel].g = pgm_read_byte(&gamma8[green]);
  leds[Pixel].b = pgm_read_byte(&gamma8[blue]);
  //leds[Pixel].r = red;
  //leds[Pixel].g = green;
  //leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  //showStrip();
}


///////////////
// Gamma Hue //
///////////////

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
