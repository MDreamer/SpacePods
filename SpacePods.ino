/*
// Based on Adafruit Neopixel example https://github.com/adafruit/Adafruit_NeoPixel 

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

/*
State machine:
Pulsing - random period & duration every 30 sec
every 15 sec - random Start-End -> crawl there
*/


#include <Adafruit_NeoPixel.h>

//just for the beginning, later I'll might divide it to 3 pins so
//we won't have data wires between the led strips
#define PIN 23
#define pirPin 15

#define RED    0xFF0000
#define BLCK   0x000000
#define PUR    0xFF00FF
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE1 0xFFFFFF

//number of LED per srip is 180 - 
// 30 LED per meter, each arc is 3 meters, 3 strips in total = 9 meters = 270 LEDs
const int ledsPerStrip = 270;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledsPerStrip, PIN, NEO_GRB + NEO_KHZ800);

//catterpilar dimensions
int min_cat = 30;
int max_cat = 100;

int max_led;
int min_led;

int dest_max_led;
int dest_min_led;

int periodePulse = random(3000,6000);
int displacePulse = random(1000,3000);     
long time=0;

uint32_t color_value;

bool movementDetected = false;

byte ambientR;
byte ambientG;
byte ambientB;

unsigned long randomTimeDelaysec = 10000; // 10 sec
unsigned long randomCountTime = 0;

unsigned long pirTimeDelaysec = 100; // 10 sec
unsigned long pirCountTime = 0;

unsigned long movementTimeDelaysec = 10000; // 10 sec
unsigned long movementCountTime = 0;

unsigned long changeColor = 0;

unsigned long periodeAmbientR = 0;
unsigned long displaceAmbientR = 0; 
unsigned long periodeAmbientG = 0;
unsigned long displaceAmbientG = 0; 
unsigned long periodeAmbientB = 0;
unsigned long displaceAmbientB = 0; 

void colorWipe(int startLED, int endLED, int colorR,int colorG,int colorB)
{
  //kill all the leds
  for (int i=0; i < ledsPerStrip; i++)
    strip.setPixelColor(i, BLCK);
  //light just one one we need
  for (int i=startLED; i <= endLED; i++)
    strip.setPixelColor(i, colorR,colorG,colorB);
}

void randomPeriodDisplace()
{
  periodeAmbientR = random(3000,6000);
  displaceAmbientR = random(1000,3000);
  periodeAmbientG = random(3000,6000);
  displaceAmbientG = random(1000,3000);
  periodeAmbientB = random(3000,6000);
  displaceAmbientB = random(1000,3000);
}

void setup() 
{
  //create a "real" random numbers
  randomSeed(analogRead(0));

  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  
  digitalWrite(pirPin, LOW);

  //first run on random
  randomPeriodDisplace();
  
  //starting the program - kill all the lights - make them dark (0,0,0)
  colorWipe(0,ledsPerStrip-1, 0, 0, 0);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  
}

void loop() 
{

  if ((abs(millis() - randomCountTime) > randomTimeDelaysec))
  {
    randomPeriodDisplace();
    randomCountTime = millis();
    randomTimeDelaysec = random(6000,20000);
  }

  //if during the last pirTimeDelaysec movement detected shine bright, if not just check
  if ((abs(millis() - pirCountTime) > pirTimeDelaysec) and movementDetected == false)
  {
    if (digitalRead(pirPin) == LOW)
    {
      movementDetected = true;
    }
    
    randomCountTime = millis();
  }

  //10 seconds after movement check set the checking var to false in order to be able to check again
  if ((abs(millis() - movementCountTime) > movementTimeDelaysec) and movementDetected == true)
  {
    movementDetected = false;
    movementCountTime = millis();
  }

  //smart cos fade of the leds
  time=millis();
  
  //when movement is detected raise the brightness
  if (movementDetected)
    color_value = 128+127*cos(2*PI/periodePulse*(displacePulse-time))/2;
  else
    color_value = 128+127*cos(2*PI/periodePulse*(displacePulse-time));

  //constant ambeint random light
  ambientR = (128+127*cos(2*PI/periodeAmbientR*(displaceAmbientR-time)))/divider;
  ambientG = (128+127*cos(2*PI/periodeAmbientG*(displaceAmbientG-time)))/divider;
  ambientB = (128+127*cos(2*PI/periodeAmbientB*(displaceAmbientB-time)))/divider;

  //write the colors
  colorWipe(0,ledsPerStrip-1,ambientR,ambientG,ambientB);    

  //brightness
  strip.setBrightness(color_value);
  strip.show();

  

}


