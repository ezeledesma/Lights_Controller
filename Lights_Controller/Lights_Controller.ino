#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    120
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];
#define BUTTON 2

uint8_t BRIGHTNESS = 255;

void setup() {
  delay(1000);  // Por las dudas
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
}

unsigned long previousMillis = 0;
unsigned long interval = 20;                 // Ticks en ms

void timer();
void timer_handler();
void mostrar();
void snake();
void snake2();
void mirror();
void orbital(uint8_t red, uint8_t green, uint8_t blue);
void flash(uint8_t red, uint8_t green, uint8_t blue);
void rainbow();
void rainbowCycle();
void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);

uint8_t estadoG = 6;
boolean bFlag = false;
void cleanVar();
void loop() {
  timer();
  if (digitalRead(BUTTON) == 1 && !bFlag){
    bFlag = true;
    estadoG++;
    estadoG%=5;
    cleanVar();
    delay(100);
  }
  else if (!digitalRead(BUTTON) && bFlag) {
    bFlag = false;
  }
}

void timer() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    timer_handler();
    //mostrar();
  }
}


void timer_handler() {
  //snake();
  //snake2();
  //mirror();
  //orbital(255,255,0);
  //flash(255,255,255);
  //rainbow();
  if(estadoG == 0) {
    snake();
  }
  else if (estadoG == 1) {
    mirror();
  }
  else if (estadoG == 2) {
    orbital(0,255,255);
  }
  else if (estadoG == 3) {
    flash(255,255,255);
  }
  else if (estadoG == 4) {
    rainbow();
  }
  else if (estadoG == 5) {
    rainbowCycle();
  }
  else if (estadoG == 6) {
    kitt(255,0,0,5,0);
  }
  else if (estadoG >= 7) {
    snake();
  }
}

uint8_t snakePos = 0;
uint8_t snakeR, snakeG, snakeB;
uint8_t flag = 1;
uint8_t y = 0;
uint8_t tira_r[NUM_LEDS] = {};
uint8_t tira_g[NUM_LEDS] = {};
uint8_t tira_b[NUM_LEDS] = {};
uint8_t count = 0;

void snake() {
  uint8_t i = 0;
  if (flag) {
    while(y < NUM_LEDS/8) {
      tira_r[y] = (y+1)*15;
      tira_g[y] = (y+1)*15;
      tira_b[y] = (y+1)*15;
      //Serial.print("WHILE 1 - ");
      //Serial.print("Y: ");
      //Serial.print(y);
      //Serial.print("- Tira R: ");
      //Serial.print(tira_r[y]);
      //Serial.print("- Tira G: ");
      //Serial.print(tira_g[y]);
      //Serial.print("- Tira B: ");
      //Serial.println(tira_b[y]);
      y++;
    }
    while(y < ((NUM_LEDS/4)-2) ) {
      tira_r[y] = tira_r[y-1]-15;
      tira_g[y] = tira_r[y-1]-15;
      tira_b[y] = tira_r[y-1]-15;
      //Serial.print("WHILE 2 - ");
      //Serial.print("Y: ");
      //Serial.print(y);
      //Serial.print("- Tira R: ");
      //Serial.print(tira_r[y]);
      //Serial.print("- Tira G: ");
      //Serial.print(tira_g[y]);
      //Serial.print("- Tira B: ");
      //Serial.println(tira_b[y]);
      y++;
    }
    while(y < NUM_LEDS) {
      tira_r[y] = 15;
      tira_g[y] = 15;
      tira_b[y] = 15;
      //Serial.print("WHILE 3 - ");
      //Serial.print("Y: ");
      //Serial.print(y);
      //Serial.print("- Tira R: ");
      //Serial.print(tira_r[y]);
      //Serial.print("- Tira G: ");
      //Serial.print(tira_g[y]);
      //Serial.print("- Tira B: ");
      //Serial.println(tira_b[y]);
      y++;
    }
    flag = 0;
  }
  if (count > 3) {
    uint8_t aux_r = tira_r[0];
    uint8_t aux_g = tira_g[0];
    uint8_t aux_b = tira_b[0];
    while (i < (NUM_LEDS-1)) {
      tira_r[i] = tira_r[i+1];
      tira_g[i] = tira_g[i+1];
      tira_b[i] = tira_b[i+1];
      i++;
    }
    tira_r[i] = aux_r;
    tira_g[i] = aux_g;
    tira_b[i] = aux_b; 
    count = 0;
  }
  count++;
}


uint8_t colorR[8]= {255, 255, 255, 0, 0, 255, 0};
uint8_t colorG[8]= {255, 0, 255, 0, 255, 0, 255};
uint8_t colorB[8]= {255, 0, 0, 255, 0, 255, 255};
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

uint16_t x = 0;
void mostrar() {
 x = 0;
 while(x < NUM_LEDS) {
    //Serial.print("X: ");
    //Serial.print(x);
    //Serial.print("- Tira R: ");
    //Serial.print(tira_r[x]);
    //Serial.print("- Tira G: ");
    //Serial.print(tira_g[x]);
    //Serial.print("- Tira B: ");
    //Serial.println(tira_b[x]);
    leds[x].r = tira_r[x];
    leds[x].g = tira_g[x];
    leds[x].b = tira_b[x];
    x++;
  }
  FastLED.show();
}


uint8_t mirrorPos = NUM_LEDS/2;
uint8_t mirrorColor[3] = {};
uint8_t set = 0;
uint8_t coloresR[] = {255, 0, 0, 255, 0, 255};
uint8_t coloresG[] = {0, 255, 0, 255, 255, 0};
uint8_t coloresB[] = {0, 0, 255, 0, 255, 255};
int8_t mirrorDir = -1;
void mirror() {
  if(mirrorPos > 0 && mirrorPos <= NUM_LEDS/2) {
    tira_r[mirrorPos] = mirrorColor[0];
    tira_g[mirrorPos] = mirrorColor[1];
    tira_b[mirrorPos] = mirrorColor[2];
    tira_r[NUM_LEDS-mirrorPos] = mirrorColor[0];
    tira_g[NUM_LEDS-mirrorPos] = mirrorColor[1];
    tira_b[NUM_LEDS-mirrorPos] = mirrorColor[2];
    mirrorPos+=mirrorDir;
    //Serial.print("mirrorPos: ");
    //Serial.print(mirrorPos);
    //Serial.println("CHAU");
    if(!mirrorPos) {
     tira_r[mirrorPos] = mirrorColor[0];
     tira_g[mirrorPos] = mirrorColor[1];
     tira_b[mirrorPos] = mirrorColor[2];
    }
  }
  else if (!mirrorPos || mirrorPos >= NUM_LEDS/2) {
    //Serial.print("mirrorColor[0]: ");
    //Serial.print(mirrorColor[0]);
    //Serial.println("HOLA");
    set++;
    set%=6;
    mirrorColor[0] = coloresR[set];
    mirrorColor[1] = coloresG[set];
    mirrorColor[2] = coloresB[set];
    tira_r[mirrorPos] = mirrorColor[0];
    tira_g[mirrorPos] = mirrorColor[1];
    tira_b[mirrorPos] = mirrorColor[2];
    mirrorDir*=-1;
    mirrorPos+=mirrorDir;
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
  else if( !auxR && !auxG && !auxB && stateOrb == 1) stateOrb = 2;

  while(orbCont < ORB_VEL) {
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
      if(orbDel == ORB_DEL) {
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

  i = NUM_LEDS-1;
  while (i > 0) {
    tira_r[i] = tira_r[i-1];
    tira_g[i] = tira_g[i-1];
    tira_b[i] = tira_b[i-1];
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
void rainbowCycle() {
  if (cycleCount == CYCLE_DEL) {
    cycleCount = 0;
    for (uint16_t i = 0; i < SPEED; i++) {
      update_rainbow();
    }
    tira_r[0] = rRed;
    tira_g[0] = rGreen;
    tira_b[0] = rBlue;
  
    i = NUM_LEDS-1;
    while (i > 0) {
      tira_r[i] = tira_r[i-1];
      tira_g[i] = tira_g[i-1];
      tira_b[i] = tira_b[i-1];
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
    for (byte i = NUM_LEDS-1; i > 0; i--) {
      leds[i].r = leds[i-1].r;
      leds[i].g = leds[i-1].g;
      leds[i].b = leds[i-1].b;  
    }
    kittCount2++;
    if (kittCount2 == NUM_LEDS) {
      kittCount2 = 0;
      kittState = 1;
    }
  }
  else if (kittState == 1) {
    for (byte i = 0; i < NUM_LEDS-1; i++) {
      leds[i].r = leds[i+1].r;
      leds[i].g = leds[i+1].g;
      leds[i].b = leds[i+1].b;  
    }
    setPixel(NUM_LEDS-1, 0, 0, 0);
    kittCount2++;
    if (kittCount2 == NUM_LEDS-eyeSize) {
      kittCount2 = 0;
      kittState = 2;
    }
  }
  else if (kittState == 2) {
    for (byte i = NUM_LEDS-1; i > 0; i--) {
      leds[i].r = leds[i-1].r;
      leds[i].g = leds[i-1].g;
      leds[i].b = leds[i-1].b;
      leds[NUM_LEDS-1-i].r = leds[i-1].r;
      //leds[NUM_LEDS-1-i].r = leds[i-1].r;
      //leds[NUM_LEDS-1-i].g = leds[i-1].g;
      //leds[NUM_LEDS-1-i].b = leds[i-1].b;
    }
    setPixel(0, 0, 0, 0);
    kittCount2++;
    if (kittCount2 == NUM_LEDS/2-eyeSize) {
      kittCount2 = 0;
      kittState = 3;
    }
  }
  else if (kittState == 3) {
    
  }
  Serial.print("State: ");
  Serial.print(kittState);
  Serial.print("- COUNT: ");
  Serial.print(kittCount);
  Serial.print("- COUNT2: ");
  Serial.print(kittCount2);
  Serial.println();
  showStrip();
}

void cleanVar() {
  snakePos = 0;
  snakeR = 0;
  snakeG = 0;
  snakeB = 0;
  flag = 1;
  y = 0;
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
 x = 0;
 mirrorPos = NUM_LEDS/2;
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
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
