



#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define FASTLED_ALLOW_INTERRUPTS 0
#define DATA_PIN            5
#define NUM_LEDS            300
#define MAX_POWER_MILLIAMPS 2100
#define LED_TYPE            WS2813
#define COLOR_ORDER         GRB

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          250
#define FRAMES_PER_SECOND   30
#define ZOOMING_BEATS_PER_MINUTE 61



#define BLENDING NOBLEND




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
          // Read signal
          int temp = analogRead(A0);

          // Sum up all the signals
          currThresh = currThresh + temp;

          // Get the loudest signal so far
          if (currPeak < temp) { currPeak = temp; }
        }

        // Get the average of the signals, then add a magic number (3) to set this as your sound threshold
        currThresh = currThresh/200 + 3;
      
        // Use the loudest signal to check with the threshold (for chicking the BEAT)
        currValue = currPeak;

      

        
        // Every 16 seconds, select a new display MODE
        unsigned long time = millis();                         
        const unsigned long timeCheck = 16 * 1000;      // 16 seconds
        static unsigned long lastSampleTime = 0 - timeCheck; 
        if (time - lastSampleTime >= timeCheck){        
            lastSampleTime = lastSampleTime + timeCheck;

            currMode = random(1,20);
        }

        // Apply the display MODE if it's different from the currrent (in this case, PREVIOUS) mode...
        if (currMode!=prevMode)
          {
           
            prevMode=currMode;
            ChangePaletteMode(currMode);
            
          }
        
        // ...But if the new MODE is a BEAT MODE, display the BEAT mode ONLY if there is a big change in the sound
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
        //  Otherwise, just show the other display MODE
        else
        {
          static uint8_t startIndex = 0;
          startIndex = startIndex + 1; /* motion speed */
          FillLEDsFromPaletteColors(startIndex);
        }


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

