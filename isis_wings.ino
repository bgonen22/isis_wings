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

#define SWITCH_TIME 10 //number of secs between animations when auto change
#define NUM_OF_STRIPS 9
#define BUTTON 11
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      12

// How many colors do you want (more colors, more gradient)?
#define NUMOFCOLORS      6


// jump between to traces (head trace to head trace - min JUMP = 3)
#define JUMP 9

// traces power will be like wave or like teeth
#define FADE 0 //1 - traces power like a wave, 0 always first led max power

// start from the middle or from the beginning
#define FROM_MIDDLE 0 // 1 - start from the middle for both sides


// delay between iterations
int delayval =5; // delay im msec

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

Adafruit_NeoPixel pixels_arr[9] ={pixels_1,pixels_2, pixels_3,pixels_4,pixels_5,pixels_6,pixels_7,pixels_8,pixels_9};



int head_color = 0;
int debounce;
bool autoCycle = 0;
int animation =0;
bool phase;
uint32_t auto_time;
float pixel_num = 0;
void setup() {
 Serial.begin(9600); 
 pinMode(BUTTON, INPUT_PULLUP);     
 for (int i = 0; i<NUM_OF_STRIPS; ++i){
  pixels_arr[i].begin(); 
 }  
}

// ---------------------
// LOOP
// ---------------------
void loop() {
  //Serial.print("animation loop ");
  //Serial.println(animation);
  bool change = check_button();
  switch (animation) {
    case 0: {
      clear_all();
      phase = 0;
      trace(pixel_num);
      break;
    }
    case 1: {
      clear_all();
      phase = 1;
      trace(pixel_num);
      break;
    }
  }
  pixel_num = pixel_num+0.1; 
  // when pixel_num get large the animation start to be slow, need to go back and keep the head_color 
  if (pixel_num > JUMP + NUMPIXELS-1 ) {
    pixel_num -= JUMP;
    head_color = (head_color+1)%NUMOFCOLORS;        
  }
  
 
  
  delay(delayval); // Delay for a period of time (in milliseconds).
}

void clear_all() {
  for (int i=0; i<NUM_OF_STRIPS;++i) {
    pixels_arr[i].clear();
  }
}

void trace (float i) {
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  //float i = -1;
  int last_led;
  if (FROM_MIDDLE) {
    last_led = NUMPIXELS/2;  
  } else {
    last_led = NUMPIXELS -1;
  }   
  if (i == last_led) {
      i = LastLed(i);
     // continue;    
     return;
   }      
    //Serial.println(i);
    
    lightAllTraces(i, HIGHLEVEL); // the first led.  
    show_all();

}
void show_all() {
  for (int i = 0; i<NUM_OF_STRIPS; ++i) {
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
    if (autoCycle && (millis()- auto_time) >= (SWITCH_TIME * 1000L)) {
        auto_time = millis();
        next_animation();  
        return 1;      
    }
  } else {                             // Pressed...
    if(++debounce >= 1) {             // Debounce input
      next_animation();
      while(!digitalRead(BUTTON)); // Wait for release
      // If held 1+ sec, toggle auto-cycle mode on/off
      if((millis() - t) >= 500L) {
        autoCycle = !autoCycle;
        auto_time = millis();
        Serial.print("autoCycle: ");
        Serial.println(autoCycle);
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
  pixel_num = 0;
  Serial.print("animation ");
  Serial.println(animation);
}

int LastLed (float i) {
  lightAllTraces(i, HIGHLEVEL); 
  if (JUMP > 3) {
    lightAllTraces(i-3, 0); 
  }
  show_all(); // This sends the updated pixel color to the hardware.
  //delay(delayval); // Delay for a period of time (in milliseconds).

  if (JUMP > 3) {
   lightAllTraces(i-2, 0);                 
  }
  head_color = (head_color+1)%NUMOFCOLORS;        
  lightAllTraces(i-JUMP+1,HIGHLEVEL); 


  show_all(); // This sends the updated pixel color to the hardware.
  //delay(delayval); // Delay for a period of time (in milliseconds).

  lightAllTraces(i-JUMP+2,HIGHLEVEL); 
  head_color = (head_color-1)%NUMOFCOLORS;                      
  if (JUMP > 3) {
   lightAllTraces(i-1, 0);                                 
  }
  show_all(); // This sends the updated pixel color to the hardware.
  //delay(delayval); // Delay for a period of time (in milliseconds).
  if (JUMP > 3) { 
    lightAllTraces(i, 0);    
  }
  head_color = (head_color+1)%NUMOFCOLORS;                    
  lightAllTraces(i-JUMP+3, HIGHLEVEL); 
  
  pixel_num=i-JUMP+3;
  return pixel_num;
}

// ------------------
// lightAllTraces
// ------------------
void lightAllTraces(float i, float power) {
  int trace_num = 0;
   //Serial.println(i);
   while (i >= 0 ) {     
     int color = (head_color+trace_num)%NUMOFCOLORS;
     if (FROM_MIDDLE) {
      set_one_color_all_traces(NUMPIXELS/2+i, color,power);
      set_one_color_all_traces(NUMPIXELS/2-i, color,power);
     } else {       
       int led_ref = (int)(i+0.01);
       /*
       Serial.print("real led ");   
       Serial.println(i);
       Serial.print("ref led ");   
       Serial.println(led_ref);
       */
       // Light the trace consider the sidtance from the real source (i)
       for (int led = led_ref -1 ; led < led_ref + 3; ++led) {  // light 2 leds before and 2 leds after the the main light
          float dist = 1 + abs(led-i);          
          float power_update = power / pow(dist,2);
          /*
          Serial.print("led ");   
          Serial.print(led);
          Serial.print(" power ");   
          Serial.println(power_update);
          */          
          set_one_color_all_traces(led,color,power_update); 
       }     
  
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
  for (int i=0; i<NUM_OF_STRIPS;++i) {
    //int color_updatea = (phase*abs(i-5)+color)%NUMOFCOLORS; //when we want phase berween the strips 
    int pixel_num_update = pixel_num - (phase*abs(i-5));
    if (pixel_num_update < 0) {
      continue;
    }
    pixels_arr[i].setPixelColor(pixel_num_update, Wheel(color,power));         
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// the level is how brigt will be tghe light (0 to 255).
uint32_t Wheel(byte color, float level) {
  Adafruit_NeoPixel pixels ;
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
