//Xadow
#include "Sleep_x.h"
#include "debug_x.h"
#include "xadow.h"
#include "xadowDfs.h"

// Adafruit
#include "Adafruit_NeoPixel.h"

// The pixel
#define PIXEL_PIN 15 
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// DEBUG
//  uncomment this to displays debug messages in serial monitor
#define DEBUG 1 

// USE_BLE
//  comment this to use the controller's serial. this is useful while debugging commands
#define USE_BLE 1

// IS_BLE_SLAVE
//  uncomment this if you have the old BLE Slave module
#define IS_BLE_SLAVE 1


//The bluetooth command stack
char commands[20];
int commandCount = 0;

//
// Setup
//
void setup() 
{
    //Initialize controller serial
    Serial.begin(9600);
    
    #ifdef DEBUG    
    Serial.println("Setup -------------------");
    #endif
    
    //Initialize & reset pixel
    pixel.begin();
    resetPixel();
    
    //Initialize Bluetooth serial    
    Serial1.begin(38400);  
    #ifndef IS_BLE_SLAVE
    //Serial1.print("AT+ROLE0");
    #endif

    //Initialize Xadow
    Xadow.init();           
}

//
// The run loop will listen for any commands
// sent to the ble and then dispatch them to the executeCommands()
//
void loop() 
{
    #ifdef DEBUG  
    Serial.println("Run loop +++++++++++++++++++++++++++++++");
    #endif
    
    #ifdef USE_BLE  
    //Wait for bluetooth to send some data
    while(Serial1.available())
    {
        //Add data to queue        
        commands[commandCount++] = Serial1.read();

        #ifdef DEBUG
        Serial.print("Received Bluetooth data ");
        Serial.print(commands[commandCount - 1], HEX);
        Serial.println();
        #endif

        //Breathe...
        delay(3);
    }  
    #endif
    #ifndef USE_BLE
    //Wait for serial to send some data
    while(Serial.available())
    {
        //Add data to queue
        commands[commandCount++] = Serial.read();

        #ifdef DEBUG
        Serial.print("Received Serial data ");
        Serial.print(commands[commandCount - 1], HEX);
        Serial.println();
        #endif

        //Breathe...
        delay(5);
    }          
    #endif  

    //Run queued commands (if any)
    if (commandCount > 0)
    {
        executeCommands();
    }
    
    //Take a deep breath
    delay(2500);
}


//
// Execute all queued commands
//
void executeCommands()
{
    if (commandCount < 1)
    {
        return;
    }
    
    //Loop all received commands
    int commandIndex;
    for (commandIndex = 0; commandIndex < commandCount; commandIndex++)
    {
        char command = commands[commandIndex];
        
        #ifdef DEBUG        
        Serial.print("Executing command ");
        Serial.println(command);
        //Serial.println(' -------');   
        #endif
   
        switch(command)
        {
            //I
            case 0x49:
                sendId();            
                break;
    
            //B
            case 0x42:
                sendBatteryStatus();
                break;
            
            //V
            case 0x56:
                sendBatteryVolts();
                break;              
              
            //D
            case 0x44:
                playDualPulser(5, 2, 255, 255, 255, 0, 0, 255);
                break;           
              
            //F
            case 0x46:
                playFlash();
                break; 
              
            //P
            case 0x50:
                playPulser(5, 2, 255, 255, 255);
                break;           
              
            //R
            case 0x52:
                playRainbow(5, 10);
                break;    
          
            //S
            case 0x53:
              playStrobe(10, 30, 255, 255, 255);
              break;  
    
            //T
            case 0x54:
                playTremolo(5, 2, 50, 255, 255, 255, 0);
                break;                           
              
            default:
                #ifdef DEBUG
                Serial.println("Command not found");  
                #endif
                break;
        }            
    }
    
    //Reset count
    commandCount = 0;    
}


//
// Call this after each command to flush the queue
//
void flushCommand()
{
    Serial1.flush();  
}

//
// Resets the pixel state to off
//
void resetPixel()
{
    pixel.setBrightness(255);
    pixel.setPixelColor(0,0,0,0);
    pixel.show();
}


//
// Sends the application id to serial1
//
void sendId()
{
    #ifdef DEBUG
    Serial.println("Command : sendId");  
    #endif    
    
    //Prepare data packet
    char data[32];
    sprintf(data, "ID:WeTweetShirt|");      
    
    #ifdef DEBUG
    Serial.println("Command : sendId data = ");  
    //Serial.println(data);    
    #endif   
    
    //Send it
    Serial1.write(data);    
    flushCommand();
}


//
// Sends the battery status to serial1
//
void sendBatteryStatus()
{
    #ifdef DEBUG
    Serial.println("Command : sendBatteryStatus");  
    #endif
    
    //Get state
    char chargeState = 1;    
    switch(Xadow.getChrgState())
    {
        case CHARGING:
            chargeState = 2;
            break;

        case CHARGDONE:
            chargeState = 3;
            break;
    }
    
    //Prepare data packet
    char data[32];
    sprintf(data, "BS:%d|", chargeState);    

    #ifdef DEBUG
    Serial.print("Command : sendBatteryStatus data = ");  
    Serial.println(data);
    #endif

    //Send it
    Serial1.write(data);
    flushCommand();
}


//
// Sends the current volts of the battery to serial1
//
void sendBatteryVolts()
{
    #ifdef DEBUG
    Serial.println("Command : sendBatteryVolts");  
    #endif
    
    //Prepare data packet
    char data[32];
    sprintf(data, "BV:%d|", int(Xadow.getBatVol() * 100));    

    #ifdef DEBUG
    Serial.print("Command : sendBatteryVolts data = ");  
    Serial.println(data);
    #endif

    //Send it
    Serial1.write(data);
    flushCommand();
}

//
// Returns the color of a color wheel at the given angle
//
//  angle - the angle of the colorwheel
//
int getColorForAngle(byte angle) 
{
    if (angle < 85) 
    {
        return pixel.Color(angle * 3, 255 - angle * 3, 0);
    } 
    else if(angle < 170) 
    {
        angle -= 85;
        return pixel.Color(255 - angle * 3, 0, angle * 3);
    } 
    else 
    {
        angle -= 170;
        return pixel.Color(0, angle * 3, 255 - angle * 3);
    }
}


//
// Plays a flash animation
//
void playFlash()
{
    #ifdef DEBUG
    Serial.println("Command : playFlash");  
    #endif
    
    for (int i = 0; i < 256; i++)
    {
        pixel.setPixelColor(0, i, 0, 0);
        pixel.show();
        delay(1);
    }

    for (int i = 255; i >= 0; i--)
    {
        pixel.setPixelColor(0, i, 0, 0);
        pixel.show();
        delay(1);
    }

    pixel.setPixelColor(0, 255, 255, 255);
    pixel.show();
    delay(50);

    pixel.setPixelColor(0, 0, 0, 0);
    pixel.show();
    delay(25);

    pixel.setPixelColor(0, 255, 255, 255);
    pixel.show();
    delay(275);

    resetPixel();
    flushCommand();
}


//
// Plays a rainbow animation
//
//  reps - repetitions
//  pace - delay between animation steps
//
void playRainbow(byte reps, byte pace)
{
    #ifdef DEBUG
    Serial.print("Command : playRainbow(");  
    Serial.print(reps);  
    Serial.print(", ");
    Serial.print(pace);  
    Serial.println(")");    
    #endif
    
    int i, j;
    for(j = 0; j < 256 * reps; j++) 
    { 
        for(i = 0; i < pixel.numPixels(); i++) 
        {
            pixel.setPixelColor(i, getColorForAngle(((i * 256 / pixel.numPixels()) + j) & 255));
        }
        pixel.show();
        delay(pace);
    }
    
    //Fadeout
    for (i = 255; i >= 0; i--)
    {
        pixel.setBrightness(i);
        pixel.show();
        delay(2);
    }  

    //Done here    
    resetPixel();
    flushCommand();
}


//
// plays a pulse animation
//
//  reps - repetitions
//  pace - delay between animation steps
//  r, g, b - color of the pulse
//
void playPulser(byte reps, byte pace, byte r, byte g, byte b)
{
    #ifdef DEBUG
    Serial.print("Command : playPulser(");  
    Serial.print(reps);  
    Serial.print(", ");
    Serial.print(pace);  
    Serial.print(", ");
    Serial.print(r);  
    Serial.print(", ");
    Serial.print(g);  
    Serial.print(", ");
    Serial.print(b);      
    Serial.println(")");    
    #endif
    
    int i, j, k;
    for (i = 0; i < reps; i++)
    {
        for (j = 0; j < 255; j++)
        {
            pixel.setPixelColor(0, r, g, b);
            pixel.setBrightness(j);
            pixel.show();
            delay(pace);
        }
        for (k = 255; k > 0; k--)
        {
            pixel.setPixelColor(0, r, g, b);
            pixel.setBrightness(k);
            pixel.show();
            delay(pace);
        }
    }

    //Done here
    resetPixel();
    flushCommand();    
}


//
// Plays a pulse animation
//
//  reps - repetitions
//  pace - delay between animation steps
//  r, g, b - color of the pulse
//
void playDualPulser(byte reps, byte pace, byte r, byte g, byte b, byte r2, byte g2, byte b2)
{
    #ifdef DEBUG
    Serial.print("Command : playDualPulser(");  
    Serial.print(reps);  
    Serial.print(", ");
    Serial.print(pace);  
    Serial.print(", ");
    Serial.print(r);  
    Serial.print(", ");
    Serial.print(g);  
    Serial.print(", ");
    Serial.print(b);     
    Serial.print(", ");
    Serial.print(r2);  
    Serial.print(", ");
    Serial.print(g2);  
    Serial.print(", ");
    Serial.print(b2);     
    Serial.println(")");    
    #endif

    int i, j, k, l, m;
    for (i = 0; i < reps; i++)
    {
        pixel.setPixelColor(0, r, g, b);
        
        for (j = 0; j < 255; j++)
        {
           pixel.setPixelColor(0, r, g, b);
           pixel.setBrightness(j);
           pixel.show();
           delay(pace);
        }

        for (k = 255; k > 0; k--)
        {
            pixel.setPixelColor(0, r, g, b);
            pixel.setBrightness(k);
            pixel.show();
            delay(pace);
        }

        pixel.setPixelColor(0, r2, g2, b2);
            
        for (l = 0; l < 255; l++)
        {
            pixel.setPixelColor(0, r2, g2, b2);
            pixel.setBrightness(l);
            pixel.show();
            delay(pace);
        }

        for (m = 255; m > 0; m--)
        {
            pixel.setPixelColor(0, r2, g2, b2);
            pixel.setBrightness(m);
            pixel.show();
            delay(pace);
        }       
    }
    
    //Done here
    resetPixel();
    flushCommand();    
}


//
// plays a tremolo animation
//
//  reps - repetitions
//  pace - delay between repetitions
//  minBrightness - the minimum brightness
//  maxBrightness - the maximum brightness
//  r, g, b - color of the pulse
//
void playTremolo(byte reps, byte pace, byte minBrightness, byte maxBrightness, byte r, byte g, byte b)
{
    #ifdef DEBUG
    Serial.print("Command : playTremolo(");  
    Serial.print(reps);  
    Serial.print(", ");
    Serial.print(pace);  
    Serial.print(", ");
    Serial.print(minBrightness); 
    Serial.print(", ");
    Serial.print(maxBrightness);     
    Serial.print(", ");
    Serial.print(r);  
    Serial.print(", ");
    Serial.print(g);  
    Serial.print(", ");
    Serial.print(b);      
    Serial.println(")");    
    #endif
    
    pixel.setPixelColor(0, r, g, b);
    int i, j, k;
    for (i = 0; i < reps; i++)
    {
        for (j = 0; j < 255; j++)
        {
            pixel.setBrightness(int(minBrightness + ((maxBrightness - minBrightness) / 255 * j)));
            pixel.show();
            delay(pace);
        }
        for (k = 0; k < 255; k++)
        {
            pixel.setBrightness(int(maxBrightness - ((maxBrightness - minBrightness) / 255 * k)));
            pixel.show();
            delay(pace);
        }
    }
    
    //Fadeout
    for (i = minBrightness; i >= 0; i--)
    {
        pixel.setBrightness(i);
        pixel.show();
        delay(2);
    }  

    //Done here
    resetPixel();
    flushCommand(); 
}


//
// play a strobe animation
//
//  reps - repetitions
//  pace - delay between repetitions
//
void playStrobe(byte reps, byte pace, byte r, byte g, byte b)
{   
    #ifdef DEBUG
    Serial.print("Command : playStrobe(");  
    Serial.print(reps);  
    Serial.print(", ");
    Serial.print(pace);  
    Serial.print(", ");
    Serial.print(r);  
    Serial.print(", ");
    Serial.print(g);  
    Serial.print(", ");
    Serial.print(b);      
    Serial.println(")");    
    #endif
    
    int i;
    for (i = 0; i < reps; i++)
    {
        pixel.setPixelColor(0, r, g, b);
        pixel.show();
        delay(pace);

        pixel.setPixelColor(0, 0, 0, 0);
        pixel.show();
        delay(pace);
    }
    
    resetPixel();
    flushCommand(); 
}
