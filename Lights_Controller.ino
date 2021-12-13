/* Optimizaciones:
 *  CRITICOS:
 *  0 - SIN SERIAL PRINTS EL PROGRAMA NO ANDA ????
 *  1 - sacar todos los serial print
 *  2 - cambiar el bt a un hardware serial
 *  LEVES:
 *  1 - cambiar variables a static y volatile
 *  2 - mover codigo a archivos .c y .h
 *  3 - debounce de boton con timers?
 *  4 - mejorar la decodificacion de la comunicacion bt
 */

/*  Autor: Ezequiel Ledesma
 *  Fecha: 04/2021
 *  MCU: Arduino Nano
 *  Modulo BT: HC-05
 *  
 *  Notas: Funciona con libreria FastLED o Adafruit Neopixel
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


// Inicializaciones
void leds_init();
void timer2_init();


// Funciones
void update_bt();
void update_state();
void update_leds();
void fade_color(byte red, byte green, byte blue, byte vel);
void orbital_1(byte red, byte green, byte blue, byte background);
void orbital_2(byte red, byte green, byte blue);
void update_rainbow(byte *red, byte *green, byte *blue);
void rainbow(byte vel = 5, byte del = 1);
void rainbow_cycle(byte colorVel = 1, byte moveVel = 0);
void mirror();
void flash(byte red, byte green, byte blue, byte flashON = 2, byte flashOFF = 1, byte times = 5, byte timeOFF = 50);
void kitt (byte red, byte green, byte blue, byte LEN = 6, uint8_t speedDelay = 0, uint8_t vel = 1);
void theater_rainbow(byte colorVel = 1, byte moveVel = 0);

// Drivers
void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);
byte get_brightness();
void set_brightness(byte brightness);

// Variables de Estado
#define REST 0
#define STATIC_COLOR 1
#define FADE_COLOR 2
#define ORBITAL_1 3
#define ORBITAL_2 4
#define MIRROR 5
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

// Variables Globales
byte colorRGB[3] = {255, 0, 1};
byte ant_colorRGB[3] = {0, 0, 0};
uint8_t state = REST;
uint8_t timer2_ticks = 10;                  // Frecuencia de actualizacion (1 ~ 1ms)
uint16_t timer2_count = 0;
boolean timer2_flag = false;


#ifdef ADAFRUIT_NEOPIXEL_H
 // NeoPixel
 Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
#endif


void setup() {
  delay(100);
  leds_init();
  timer2_init();
  btSerial.begin(19200);
  Serial.begin(9600);                       // Serial monitor para debugging
}


void leds_init() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.begin();
   strip.setBrightness(BRIGHTNESS);
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
 #endif
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
  update_bt();
  update_state();
    
  Serial.print(" // STATE: "); Serial.println(state);
  /*
  Serial.print(" - BRIGTHNESS: "); Serial.print(FastLED.getBrightness());
  Serial.print(" - RED: "); Serial.print(colorRGB[0]);
  Serial.print(" - GREEN: "); Serial.print(colorRGB[1]);
  Serial.print(" - BLUE: "); Serial.print(colorRGB[2]);
  Serial.print(" - REDant: "); Serial.print(ant_colorRGB[0]);
  Serial.print(" - GREENant: "); Serial.print(ant_colorRGB[1]);
  Serial.print(" - BLUEant: "); Serial.print(ant_colorRGB[2]);
  Serial.println("");
  */
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
    case FADE_COLOR:
      fade_color(colorRGB[0], colorRGB[1], colorRGB[2], 5);
      break;
    case ORBITAL_1:
      orbital_1(colorRGB[0], colorRGB[1], colorRGB[2], 30);
      break;
    case ORBITAL_2:
      orbital_2(colorRGB[0], colorRGB[1], colorRGB[2]);       // falta
      break;
    case MIRROR:
      mirror();                                               // falta
      break;
    case FLASH:
      flash(colorRGB[0], colorRGB[1], colorRGB[2]);
      break;
    case RAINBOW:
      rainbow();
      break;
    case RAINBOW_CYCLE:
      rainbow_cycle(13, 3);
      break;
    case KITT:
      kitt(colorRGB[0], colorRGB[1], colorRGB[2], 6, 0, 1);
      break;
    case OFF:
      setAll(0, 0, 0);
      break;
    case THEATER_RAINBOW:
      theater_rainbow(10, 2);
      break;
    default:
      state = REST;
      break;
  }
  showStrip();
}


// dejar estas variables hasta re hacer el codigo de todos los efectos
byte tira_r[NUM_LEDS] = {};
byte tira_g[NUM_LEDS] = {};
byte tira_b[NUM_LEDS] = {};


// FADE_COLOR
// Posibles mejoras: transiciones mas fluidas
void fade_color(byte red, byte green, byte blue, byte vel) {
  static byte red_aux = ant_colorRGB[0];
  static byte green_aux = ant_colorRGB[1];
  static byte blue_aux = ant_colorRGB[2];
  for (uint16_t i = 0; i < vel; i++) {
    if (red > red_aux) red_aux++;
    else if (red < red_aux) red_aux--;
    if (green > green_aux) green_aux++;
    else if (green < green_aux) green_aux--;
    if (blue > blue_aux) blue_aux++;
    else if (blue < blue_aux) blue_aux--; 
  }
  setAll(red_aux, green_aux, blue_aux);
}


// ORBITAL_1
// Posibles mejoras: poder seleccionar para que lado ir, velocidad y ancho
void orbital_1(byte red, byte green, byte blue, byte background) {
  static uint16_t y = 0;
  static uint16_t newpos = 0;
  y = 0;
  while ( y < (NUM_LEDS/8) ) {
    setPixel((y+newpos)%NUM_LEDS,
             (byte)(red*background/100) + (red-(byte)(red*background/100))*(y+1)/(NUM_LEDS/8),
             (byte)(green*background/100) + (green-(byte)(green*background/100))*(y+1)/(NUM_LEDS/8),
             (byte)(blue*background/100) + (blue-(byte)(blue*background/100))*(y+1)/(NUM_LEDS/8));
    y++;
  }
  while ( y < ((NUM_LEDS/4)-2) ) {
    setPixel((y+newpos)%NUM_LEDS,
              red - (red-(byte)(red*background/100))*(y+1)/((NUM_LEDS/4)-2),
              green - (green-(byte)(green*background/100))*(y+1)/((NUM_LEDS/4)-2),
              blue - (blue-(byte)(blue*background/100))*(y+1)/((NUM_LEDS/4)-2));
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


// ORBITAL_2
// Posibles mejoras: poder seleccionar para que lado ir, tiempo de full encendido, control de tamanio del degrade
void orbital_2(uint8_t red, uint8_t green, uint8_t blue) {
  static uint8_t o2_state = 0;
  static uint16_t o2_count = 0;
  static byte auxR = 0, auxG = 0, auxB = 0;
  static byte auxR2 = 0, auxG2 = 0, auxB2 = 0;
  for(uint8_t j = 0; j < 2; j++) {
    if(!o2_state) {
      if( (auxR==red) && (auxG==green) && (auxB==blue)) o2_count++;
      if(auxR < red) auxR++;
      if(auxG < green) auxG++;
      if(auxB < blue) auxB++;
      auxR2 = auxR;
      auxG2 = auxG;
      auxB2 = auxB;
      for(uint16_t i = o2_count; i < NUM_LEDS; i++) {
        setPixel(i, auxR, auxG, auxB);
        if(i==(NUM_LEDS-1) && (auxR == red) && (auxG == green) && (auxB == blue)) {
          o2_count = 0;
          o2_state = 1;
        }
        if(auxR) auxR--;
        if(auxG) auxG--;
        if(auxB) auxB--;
      }
      auxR = auxR2;
      auxG = auxG2;
      auxB = auxB2;
    }
    else if (o2_state == 1) {
      if(auxR) auxR--;
      if(auxG) auxG--;
      if(auxB) auxB--;
      if(!auxR && !auxG && !auxB) o2_count++;
      auxR2 = auxR;
      auxG2 = auxG;
      auxB2 = auxB;
      for(uint16_t i = o2_count; i < NUM_LEDS; i++) {
        setPixel(i, auxR, auxG, auxB);
        if(i==(NUM_LEDS-1) && !auxR && !auxG && !auxB ) {
          o2_count = 0;
          o2_state = 0;
        }
        if(auxR<red) auxR++;
        if(auxG<green) auxG++;
        if(auxB<blue) auxB++;
      }
      auxR = auxR2;
      auxG = auxG2;
      auxB = auxB2;
    }
  }
}



void mirror() {
  static byte mir_r[NUM_LEDS] = {};
  static byte mir_g[NUM_LEDS] = {};
  static byte mir_b[NUM_LEDS] = {};
  static uint16_t mirrorPos = 0;
  //static uint8_t mirrorPos = NUM_LEDS / 2;
  static uint8_t mirrorColor[3] = {};
  static uint8_t set = 0;
  static uint8_t coloresR[] = {255, 0, 0, 255, 0, 255};
  static uint8_t coloresG[] = {0, 255, 0, 255, 255, 0};
  static uint8_t coloresB[] = {0, 0, 255, 0, 255, 255};
  static int8_t mirrorDir = -1;
  //(byte)NUM_LEDS/2;
  /*
  if (mirrorPos < (byte)NUM_LEDS/2) {
    for (uint16_t i = 0; i <
  }*/
  
  if (mirrorPos > 0 && mirrorPos <= NUM_LEDS / 2) {
    mir_r[mirrorPos] = mirrorColor[0];
    mir_g[mirrorPos] = mirrorColor[1];
    mir_b[mirrorPos] = mirrorColor[2];
    mir_r[NUM_LEDS - mirrorPos] = mirrorColor[0];
    mir_g[NUM_LEDS - mirrorPos] = mirrorColor[1];
    mir_b[NUM_LEDS - mirrorPos] = mirrorColor[2];
    //Serial.print("mirrorPos: ");
    //Serial.print(mirrorPos);
    //Serial.println("CHAU");
    if (!mirrorPos) {
      mir_r[mirrorPos] = mirrorColor[0];
      mir_g[mirrorPos] = mirrorColor[1];
      mir_b[mirrorPos] = mirrorColor[2];
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
    mir_r[mirrorPos] = mirrorColor[0];
    mir_g[mirrorPos] = mirrorColor[1];
    mir_b[mirrorPos] = mirrorColor[2];
    mirrorDir *= -1;
  }
  setPixel(mirrorPos, mir_r[mirrorPos], mir_g[mirrorPos], mir_b[mirrorPos]);
  setPixel(NUM_LEDS-mirrorPos, mir_r[NUM_LEDS-mirrorPos], mir_g[NUM_LEDS-mirrorPos], mir_b[NUM_LEDS-mirrorPos]);
  mirrorPos += mirrorDir;
}



// FLASH
// Posibles mejoras: mejores opciones para flashON y flashOFF, times y timeOff valores altos tardan mucho
void flash(uint8_t red, uint8_t green, uint8_t blue, byte flashON, byte flashOFF, byte times, byte timeOFF) {
  static byte flash_state = 0;
  static uint16_t flash_count = 0;
  static uint16_t times_count = 0;
  if (times_count == times) {
    if (flash_count == timeOFF) {
      flash_count = 0;
      times_count = 0;
    }
    flash_count++;
    return;
  }
  if (flash_state == 0) {
    setAll(red, green, blue);
    flash_count++;
    if (flash_count == flashON) {
      flash_count = 0;
      flash_state = 1;
    }
  }
  else if (flash_state == 1) {
    setAll(0, 0, 0);
    flash_count++;
    if (flash_count == flashOFF) {
      flash_count = 0;
      flash_state = 0;
      times_count++;
    }
  }
}


// RAINBOW
void rainbow(byte vel, byte del) {
  static uint8_t rainbow_del = 0;
  if(rainbow_del < del) {
    rainbow_del++;
    return;
  }
  rainbow_del = 0;
  static byte red = 0, green = 0, blue = 0;
  for (uint16_t i = 0; i < vel; i++) {
     update_rainbow(&red, &green, &blue);
  }
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
  }
}


// UPDATE_RAINBOW
void update_rainbow(byte *red, byte *green, byte *blue) {
  #define RED     0
  #define D_RED   1
  #define GREEN   2
  #define D_GREEN 3
  #define BLUE    4
  #define D_BLUE  5
  static uint8_t r_estado = RED;
  if (r_estado == RED) {
    if ((*red) == 255) r_estado = D_BLUE;
    else (*red)++;
  }
  if (r_estado == D_BLUE) {
    if (!(*blue)) r_estado = GREEN;
    else (*blue)--;
  }
  if (r_estado == GREEN) {
    if ((*green) == 255) r_estado = D_RED;
    else (*green)++;
  }
  if (r_estado == D_RED) {
    if (!(*red)) r_estado = BLUE;
    else (*red)--;
  }
  if (r_estado == BLUE) {
    if ((*blue) == 255) r_estado = D_GREEN;
    else (*blue)++;
  }
  if (r_estado == D_GREEN) {
    if (!(*green)) r_estado = RED;
    else (*green)--;
  }
}


// UPDATE_RAINBOW v2
void update_rainbowv2(byte *red, byte *green, byte *blue) {
  #define RED     0
  #define D_RED   1
  #define GREEN   2
  #define D_GREEN 3
  #define BLUE    4
  #define D_BLUE  5
  uint8_t r_estado = RED;
  if( (*red)<255 && !(*green) && (*blue)==255) r_estado == RED;
  else if ( (*red)==255 && !(*green) && (*blue)<255 && (*blue) ) r_estado = D_BLUE;
  else if ( (*red)==255 && (*green)<255 && !(*blue) ) r_estado = GREEN;
  else if ( (*red)<255 && (*green)==255 && !(*blue) && (*red)) r_estado = D_RED;
  else if ( !(*red) && (*green)==255 && (*blue)<255 ) r_estado = BLUE;
  else if ( !(*red) && (*green)<255 && (*blue)==255 && (*green)) r_estado = D_GREEN;
  if (r_estado == RED) {
    (*red)++;
    if ((*red) == 255) (*blue)--;
  }
  if (r_estado == D_BLUE) {
    (*blue)--;
    if (!(*blue)) (*green)++;
  }
  if (r_estado == GREEN) {
    (*green)++;
    if ((*green) == 255) (*red)--;
  }
  if (r_estado == D_RED) {
    (*red)--;
    if (!(*red)) (*blue)++;
  }
  if (r_estado == BLUE) {
    (*blue)++;
    if ((*blue) == 255) (*green)--;
  }
  if (r_estado == D_GREEN) {
    (*green)--;
    if (!(*green)) (*red)++;
  }
}


// RANBOW_CYCLE
// Posibles mejoras: fluidez de velocidad y movimiento
void rainbow_cycle(byte colorVel, byte moveVel) {
  static byte red = 255, green = 0, blue = 0;
  static uint16_t cycleCount = 0;
  if (cycleCount == moveVel) {
    cycleCount = 0;
    for (uint16_t j = 0; j < NUM_LEDS; j++) {
      for (uint16_t i = 0; i < colorVel; i++) {
        update_rainbow(&red, &green, &blue);
      }
      setPixel(j, red, green, blue);
    }
  }
  else {
    cycleCount++;
  }
}


// Lineas tipo KITT
void kitt (byte red, byte green, byte blue, byte LEN, uint8_t speedDelay, uint8_t vel) {
  #define RIGHT 0
  #define LEFT 1
  #define MIDDLE 2
  //#define LEN 6
  #define REP 1
  static uint8_t p3_state = RIGHT;
  static int16_t l_count = 0;
  static int16_t i = 0;
  static uint8_t kitt_del = 0;
for(uint8_t x = 0; x < vel; x++) {
  if(kitt_del < speedDelay) {
    kitt_del++;
    return;
  }
  else kitt_del = 0;
  if(p3_state == RIGHT) {
    if(i < (NUM_LEDS)) {
      for(int16_t j = 0; j < LEN; j++) {
        if((i-j)>=0) {
          setPixel(i-j, red, green, blue);
        }
        if((i-j-1)>=0) {
          setPixel(i-j-1, 0, 0, 0);
        }
      }
      i++;
    }
    else {
      p3_state = LEFT;
      i-=LEN;
    }
  }
  else if (p3_state == LEFT) {
    if(i >= 0) {
      for(int16_t j = 0; j < LEN; j++) {
        if((i+j)<NUM_LEDS) {
          setPixel(i+j, red, green, blue);
        }
        if((i+j+1)<NUM_LEDS) {
          setPixel(i+j+1, 0, 0, 0);
        }
      }
      i--;
    }
    else {
      p3_state = RIGHT;
      i+=LEN;
      l_count++;
      if(l_count == REP) {
        l_count = 0;
        p3_state = MIDDLE;
        i = i-(LEN-1);
      }
    }
  }
  else if (p3_state == MIDDLE) {
    static bool mid_dir = true;
    static bool turn = true;
    if(mid_dir) {
      if(i<(NUM_LEDS/2)) {
        for(int16_t j = 0; j < LEN/2; j++) {
          if((i-j)>=0) {
            setPixel(i-j, red, green, blue);
            setPixel(NUM_LEDS-1-(i-j), red, green, blue);
          }
          if((i-j-1)>=0) {
            setPixel(i-j-1, 0, 0, 0);
            setPixel(NUM_LEDS-1-(i-j-1), 0, 0, 0);
          }
         }
        i++;
      }
      else mid_dir = false;  
    }
    else {
      if(i>=0) {
        for(int16_t j = 0; j < LEN/2; j++) {
          if((i+j)<NUM_LEDS) {
            setPixel(i+j, red, green, blue);
            setPixel(NUM_LEDS-1-(i+j), red, green, blue);
          }
          if((i+j+1)<NUM_LEDS) {
            setPixel(i+j+1, 0, 0, 0);
            setPixel(NUM_LEDS-1-(i+j+1), 0, 0, 0);
          }
        }
        i--;
      }
      else {
        mid_dir = true;
        turn = !turn;
        if(turn) {
          p3_state = LEFT;
          for(int16_t j = 0; j<LEN/2; j++) { // Fix
            setPixel(0+j, 0, 0, 0);
          }
          i = NUM_LEDS-1;
          l_count = -1;
        }
        else {
          p3_state = RIGHT;
          for(int16_t j = 0; j<LEN/2; j++) { // Fix
            setPixel(NUM_LEDS-1-j, 0, 0, 0);
          }
          i = 0;
        }
      }
    }
  }
}
}


byte sen(byte x) {
  if(x%5) {
    return 0;
  }
  else return 1;
}

void theater_rainbow(byte colorVel, byte moveVel) {
  /*static byte red = 255, green = 0, blue = 0;  
  static int Position=0;
      Position++;
      for(int i=0; i<NUM_LEDS; i++) {
        for (uint16_t i = 0; i < 2; i++) {
          update_rainbow(&red, &green, &blue);
        }
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
                   //Serial.print("TH: ");
                   //Serial.println(((sin(i+Position) * 127 + 128)/255)*red);
      }
  return;*/
  static byte red = 255, green = 0, blue = 1;
  static byte auxRed = 0, auxGreen = 0, auxBlue = 0;
  static uint16_t cycleCount = 0;
  static int Position=0;
  Position++;
  if (cycleCount == moveVel) {
    cycleCount = 0;
    for (uint16_t j = 0; j < NUM_LEDS; j++) {
      for (uint16_t i = 0; i < colorVel; i++) {
        update_rainbowv2(&red, &green, &blue);
      }
      if (!j) {
        auxRed = red;
        auxGreen = green;
        auxBlue = blue;
      }
      /*
      setPixel(j,((sin(j+Position) * 127 + 128)/255)*red,
                 ((sin(j+Position) * 127 + 128)/255)*green,
                 ((sin(j+Position) * 127 + 128)/255)*blue);
                 */
      //*
      setPixel(j,sen(j+Position)*red,
                 sen(j+Position)*green,
                 sen(j+Position)*blue);
       //          */
    }
    red = auxRed;
    green = auxGreen;
    blue = auxBlue;
  }
  else {
    cycleCount++;
  }
}


/////////////////////
// BT TRANSMISSION //
/////////////////////

#define LEIDO 0
#define SIN_LEER 1
uint8_t bt = LEIDO;

void update_bt() {
  static uint8_t change = 0;
  static boolean change_flag = false;
  static byte colorRGB_aux[3] = {};
  while (btSerial.available()) {
    buff = btSerial.read();
    Serial.print(" // BUFF: ");
    Serial.print(buff);
    Serial.println("");
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
  else if (code == "fc") state = FADE_COLOR;
  else if (code == "as") state = RAINBOW;
  else if (code == "ac") state = RAINBOW_CYCLE;
  else if (code == "o1") state = ORBITAL_1;
  else if (code == "o2") state = ORBITAL_2;
  else if (code == "th") state = THEATER_RAINBOW;
  else if (code == "mi") state = MIRROR;
  else if (code == "kt") state = KITT;
  else if (code == "fs") state = FLASH;
  else if (code == "l+" && (get_brightness() <= 225)) set_brightness(get_brightness() + 30);
  else if (code == "l-" && (get_brightness() >= 31)) set_brightness(get_brightness() - 30);
  else if (code == "l0") set_brightness(255);
  else if (code == "p-" && (timer2_ticks <= 45)) timer2_ticks += 5;
  else if (code == "p+" && (timer2_ticks >= 10)) timer2_ticks -= 5;
  else if (code == "p0") timer2_ticks = 10;
  bt = LEIDO;
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
    strip.setPixelColor(Pixel, strip.Color(pgm_read_byte(&gamma8[red]), pgm_read_byte(&gamma8[green]), pgm_read_byte(&gamma8[blue])));
  #endif
  #ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[Pixel].r = pgm_read_byte(&gamma8[red]);
    leds[Pixel].g = pgm_read_byte(&gamma8[green]);
    leds[Pixel].b = pgm_read_byte(&gamma8[blue]);
  #endif
  //////////////////
  // EXPERIMENTAL //
  //////////////////
  //**Para poder utilizar cambio de colores con fade**//
  ant_colorRGB[0]=red;
  ant_colorRGB[1]=green;
  ant_colorRGB[2]=blue;
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
}

byte get_brightness() {
  #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    return strip.getBrightness();
  #endif
  #ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    return FastLED.getBrightness();
  #endif
}

void set_brightness(byte brightness) {
  #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.setBrightness(brightness);
  #endif
  #ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    FastLED.setBrightness(brightness);
  #endif
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
