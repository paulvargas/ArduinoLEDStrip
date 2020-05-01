
int sensor_value = 0;
int threshold = 430; //Enter Your threshold value here
int abs_value = 0;
int ledCount = 200; //number of Bargraph LEDs
int bargraph[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // Bargraph pins



#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define DATA_PIN           5
#define NUM_LEDS            300
#define MAX_POWER_MILLIAMPS 2000
#define LED_TYPE            WS2811
#define COLOR_ORDER         GRB

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          250
#define FRAMES_PER_SECOND   30
#define ZOOMING_BEATS_PER_MINUTE 61



// you could also use any other pre-defined or custom 
// color palette, e.g., 
//CRGBPalette16 gCurrentPalette( PartyColors_p );
// CRGBPalette16 gCurrentPalette( CloudColors_p );
// CRGBPalette16 gCurrentPalette( LavaColors_p );

// disable palette color 'blending'; i.e., we don't want to
// blend between purple and orange (for example), we just want 
// to use those raw colors directly.
#define BLENDING NOBLEND


int  workLED   = 50;
int  ledStep   = 3;


float currentAvg = 0;
float prevValue = 0;
float currValue = 0;
float currThresh = 0;
float currMin = 0;
int   currMode = 0;
int   prevMode = 0;
int   prevSecond=0;
#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

int currPeak = 0;

void setup() {

    delay(3000);
    Serial.begin(115200); // setup serial
    
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setDither(BRIGHTNESS < 255);
    //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip).setDither(BRIGHTNESS < 255);

    FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
    
     for( int i = 0; i < NUM_LEDS; i++) {
                leds[i] = CRGB::Black;
              }
                FastLED.show();
             delay(300);

    
      

 
}


void loop() {

       
      currThresh = 0;currPeak =0;
        for (int i=1;i<=200;i++)
        {
          int temp = analogRead(A0);
          currThresh = currThresh + temp;
          if (currPeak < temp) { currPeak = temp; }
        }
        currThresh = currThresh/200 + 3;
      
      //  sensor_value = analogRead(A0);
        currValue = currPeak;

      

        uint8_t currSecond = (millis() / 1000) % 60;
        static uint8_t lastSecond = 99;
        
        unsigned long time = millis();                         
        const unsigned long timeCheck = 16 * 1000;      // 16 seconds
        static unsigned long lastSampleTime = 0 - timeCheck; 
        if (time - lastSampleTime >= timeCheck){        
            lastSampleTime = lastSampleTime + timeCheck;

            currMode = random(1,20);
        }

        if (currMode!=prevMode)
          {
           
            prevMode=currMode;
            ChangePaletteMode(currMode);
            
          }
          
         
        if (currMode>10)  //BEAT MODE
        {
          if (currValue > currThresh )      //HAS BEAT
          {
            
            FillLEDsFromPaletteColors(0);
           
          }
          else
          {
              for( int i = 0; i < NUM_LEDS; i++) {
                leds[i] = CRGB::Black;
              }
          }
        }
        else
        {
          static uint8_t startIndex = 0;
          startIndex = startIndex + 1; /* motion speed */
          FillLEDsFromPaletteColors(startIndex);
        }
      

          
        Serial.print(currValue);
       // Serial.print("    ");
       // Serial.print(prevValue);
       // Serial.print("    ");
       // Serial.print(currentAvg);
        Serial.print("    ");
        Serial.println(currThresh);

//
//        currentAvg = (currValue + prevValue) / 2;
//        
//        currThresh = currentAvg + abs(currentAvg-currValue) ;
//        
//        prevValue = currValue;

      
       FastLED.show();
        FastLED.delay(500 / UPDATES_PER_SECOND);

}
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


void ChangePaletteMode(int mode)
{


    
        if( mode == 1)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( mode == 2)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( mode == 3)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( mode == 4)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( mode == 5)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( mode == 6)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( mode == 7)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( mode == 8)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
      
        if( mode == 9)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( mode == 10)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
        if( mode > 10)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}


// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

