#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Which pin on the Arduino is connected to the NeoPixels?
//2-10
#define LED_PIN1 2          
#define LED_PIN2 3          
#define LED_PIN3 4          
#define LED_PIN4 5          
#define LED_PIN5 6          
#define LED_PIN6 7          
#define LED_PIN7 8          
#define LED_PIN8 9          
#define LED_PIN9 10          


#define BUTTON 11
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60

// How many colors do you want (more colors, more gradient)?
#define NUMOFCOLORS      6


// jump between to traces (head trace to head trace - min JUMP = 3)
#define JUMP 5

// traces power will be like wave or like teeth
#define FADE 0 //1 - traces power like a wave, 0 always first led max power

// start from the middle or from the beginning
#define FROM_MIDDLE 0 // 1 - start from the middle for both sides


// delay between iterations
int delayval = 100; // delay for half a second

// light level 1=255
float HIGHLEVEL=0.4;
float MEDIUMLEVEL=0.2;
float LOWLEVEL=0.06;



// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

Adafruit_NeoPixel pixels_1 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_2 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_3 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_4 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_5 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_6 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_7 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_8 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_9 = Adafruit_NeoPixel(NUMPIXELS, LED_PIN9, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixels_dummy = Adafruit_NeoPixel(NUMPIXELS, 18, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixels_arr[9] ={pixels_1,pixels_2, pixels_3,pixels_4,pixels_5,pixels_6,pixels_7,pixels_8,pixels_9};



int head_color = 0;
int debounce;
bool autoCycle = 0;
int animation;
bool phase;
uint32_t auto_time;

void setup() {
 Serial.begin(9600); 
 for (int i = 0; i<10; ++i){
  pixels_arr[i].begin(); 
 }  
}

// ---------------------
// LOOP
// ---------------------
void loop() {
  Serial.print("animation b ");
  Serial.println(animation);
  
  switch (animation) {
    case 0: {
      phase = 0;
      trace();
      break;
    }
    case 1: {
      phase = 1;
      trace();
      break;
    }
  }
  
  delay(delayval); // Delay for a period of time (in milliseconds).
}

void trace () {
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  int i = -1;
  int last_led;
  if (FROM_MIDDLE) {
    last_led = NUMPIXELS/2;  
  } else {
    last_led = NUMPIXELS -1;
  }
  while(1){
    bool change = check_button();
    if (change) return;
    i++;
//    Serial.println(head_color);
    
    if (i == last_led) {
      i = LastLed(i);
      continue;    
    }      
    //Serial.println(i);
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    lightAllTraces(i, HIGHLEVEL); // the first led.
    if (i>0) {
        lightAllTraces(i-1, MEDIUMLEVEL); 
        if (i>1) {
           lightAllTraces(i-2, LOWLEVEL); 
           if(i>2 && JUMP >3) {
                lightAllTraces(i-3, 0); // turn off the distant led.            
           }
        }        
    }  
    show_all();
  }
}
void show_all() {
  for (int i = 0; i<10; ++i) {
    pixels_arr[i].show(); // This sends the updated pixel color to the hardware.
  }

}
bool check_button() {
  //Serial.println("HERE ");
  //int readb = digitalRead(BUTTON);
  //Serial.println (readb);
  uint32_t t = millis();               // Current time, milliseconds  
  if(digitalRead(BUTTON)) {        // Image select?
    debounce = 0;                      // Not pressed -- reset counter
    if (autoCycle && (millis()- auto_time) >= 10000L) {
        auto_time = millis();
        next_animation();  
        return 1;      
    }
  } else {                             // Pressed...
    if(++debounce >= 2) {             // Debounce input
      next_animation();
      while(!digitalRead(BUTTON)); // Wait for release
      // If held 1+ sec, toggle auto-cycle mode on/off
      if((millis() - t) >= 1000L) {
        autoCycle = !autoCycle;
        auto_time = millis();
        //Serial.print("Time pressed: ");
        //uint32_t time_pressed = millis() - t;
        //Serial.println(time_pressed);        
      }
      debounce = 0;
      return 1;
    }
  } 
  return 0; 
}


void next_animation() {
  animation = (animation+1)%2;   // Switch to next image
  Serial.print("animation ");
  Serial.println(animation);
}

int LastLed (int i) {
  lightAllTraces(i, HIGHLEVEL); 
  lightAllTraces(i-1, MEDIUMLEVEL);
  lightAllTraces(i-2, LOWLEVEL); 
  if (JUMP > 3) {
    lightAllTraces(i-3, 0); 
  }
  show_all(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).

  
  lightAllTraces(i, MEDIUMLEVEL);                     
  lightAllTraces(i-1,LOWLEVEL);
  if (JUMP > 3) {
   lightAllTraces(i-2, 0);                 
  }
  head_color = (head_color+1)%NUMOFCOLORS;                    
  lightAllTraces(i-JUMP+1,HIGHLEVEL); 


  show_all(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).

  lightAllTraces(i-JUMP+2,HIGHLEVEL); 
  lightAllTraces(i-JUMP+1, MEDIUMLEVEL);                     
  head_color = (head_color-1)%NUMOFCOLORS;                    
  lightAllTraces(i, LOWLEVEL);
  if (JUMP > 3) {
   lightAllTraces(i-1, 0);                                 
  }
  show_all(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).
  if (JUMP > 3) { 
    lightAllTraces(i, 0);    
  }
  head_color = (head_color+1)%NUMOFCOLORS;                    
  lightAllTraces(i-JUMP+3, HIGHLEVEL); 
  lightAllTraces(i-JUMP+2, MEDIUMLEVEL);                     
  lightAllTraces(i-JUMP+1, LOWLEVEL);

  i=i-JUMP+3;
  return i;
}
void lightAllTraces(int i, float power) {
  int trace_num = 0;
   //Serial.println(i);
   while (i >= 0 ) {     
     int color = (head_color+trace_num)%NUMOFCOLORS;
     if (FROM_MIDDLE) {
     // set_one_color_all_traces(NUMPIXELS/2+i, color,power);
      //set_one_color_all_traces(NUMPIXELS/2-i, color,power);
     } else {
       set_one_color_all_traces(i,color,power);     
     }
      i-=JUMP;     
      trace_num++;
      if (FADE) {
        if (power == HIGHLEVEL) {
          power = LOWLEVEL;      
        } else if (power == LOWLEVEL) {
          power = HIGHLEVEL;      
        }
      }
   } 
}

void set_one_color_all_traces (int pixel_num,int color,float power) {
  for (int i=0; i<10;++i) {
    int color_update = (phase*abs(i-5)+color)%NUMOFCOLORS; //when we want phase berween the strips
    pixels_arr[i].setPixelColor(pixel_num, Wheel(color_update,power));         
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// the level is how brigt will be tghe light (0 to 255).
uint32_t Wheel(byte color, float level) {
  Adafruit_NeoPixel pixels = pixels_dummy;
  if (level==0) return pixels.Color(0, 0, 0);
  //level=100;   
  float power;
 // Serial.println(color);
  if(color < NUMOFCOLORS/3) {
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*(255 - power), 0, level*power); 
  } else if(color < 2*NUMOFCOLORS/3) {
      color -= NUMOFCOLORS/3;
      power=1.0*color/(NUMOFCOLORS/3)*255;
      return pixels.Color(0, level*power, level*(255 - power));
  } else {
     color -= 2*NUMOFCOLORS/3;
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*power, level*(255 - power), 0);
  }
}

