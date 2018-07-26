#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// calibration mins and max for raw data when touching edges of screen
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

//SPI Communication
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

//Color Definitons
#define BLACK     0x0000
#define BLUE      0x001F
#define GREY      0xCE79
#define LIGHTGREY 0xDEDB
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define Loading_MINX 180
#define Loading_MINY 30


#define MINPRESSURE 1
#define MAXPRESSURE 1000

//Size of key containers 70px
#define BOXSIZE 70

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //Touchscreen Object mit der Senibilität von 300 Mohm

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//Container variables for touch coordinates
int X, Y, Z;
//Variable für die aktuelle Seite im Menü
int currentpage;
int gamemode = 3;
boolean buttonactive = true;
int links = 44; //Motorsteuerung Pin 1
int rechts = 46; //Motorsteuerung Pin 2
int mintempo = 60; //Minimales Tempo
int maxtempo = 100; //Maximales Tempo
long randNumber; //Random Nummer für hard Mode
long roundCounter = 0; //RundenZähler für Random Mode




//Berechnungne für die Boxen
//Screen height without hidden pixel
double tHeight = tft.height() - 1;
//Centering the mid square
double center = (tft.width() / 2) - (BOXSIZE / 2);
//Space between squares
double padding = 10;
//Position of squares to the left and right of center
double fromCenter = BOXSIZE + padding;
//Second row Y-Axis position
double secondRow = BOXSIZE + padding;
//Third row Y-Axis position
double thirdRow = secondRow + BOXSIZE + padding;
//Fourth row Y-Axis position
double fourthRow = thirdRow + BOXSIZE + padding;
//Y-Axis align for all squares
double verticalAlign = (tHeight - ((BOXSIZE * 4) + (padding * 3))) / 2;
//Left column starting x posision
double leftColPositionX = center - fromCenter;
//Mid column starting x posision
double midColPositionX = center;
//Right column starting x posision
double rightColPositionX = center + fromCenter;

//Centers text horizontally on all three columns
double midColCursorX    = midColPositionX  + (BOXSIZE / 3);
//Centers text horizontally on all four rows
double firstRowCursorY  = verticalAlign + (BOXSIZE / 3);
double secondRowCursorY = secondRow + firstRowCursorY;
double thirdRowCursorY  = thirdRow  + firstRowCursorY;
double fourthRowCursorY = fourthRow + firstRowCursorY;
double secondRowVertialAlign = secondRow + verticalAlign;
double thirdRowVertialAlign = thirdRow + verticalAlign;
double fourthRowVertialAlign = fourthRow + verticalAlign;





void setup() {
  Serial.begin(9600);
  pinMode(links, OUTPUT);
  pinMode(rechts, OUTPUT);  
  analogWrite(links, 0);  
  analogWrite(rechts, 0); 
  tft.reset();
  #ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
  #else
  Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
  #endif
  tft.reset();
  uint16_t identifier = tft.readID();
  identifier=0x9341;
  tft.begin(identifier);
  //Background color
  tft.fillScreen(LIGHTGREY);
  //Display ausrichtung
  tft.setRotation(2);
  //Hauptansicht
  tft.fillScreen(LIGHTGREY);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(30, 140);
  tft.print("Loading...");
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.setCursor(30, 70);
  tft.print("Pukemaster");
  for (int i; i < 200; i++)
  {
    tft.fillRect(Loading_MINY -10, Loading_MINX, i, 10, BLACK);
    delay(1);
  }
  tft.fillScreen(LIGHTGREY);
  drawHome();
  currentpage = 0;

  randomSeed(analogRead(6));
  
}

void loop(){
retrieveTouch();
  
  //Homescreen
  if (currentpage == 0){
  //Auf eingabe warten
  if (Z > 5 && Z < 1000)
  { 
    // good for debuggin, prints out the x,y cordinates of the press
    Serial.print("X = "); Serial.println(X);
    Serial.print("Y = "); Serial.println(Y);
    
     //Start angedrückt
      if(X>0 && X<230 && Y<230 && Y>160){
        tft.drawRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, LIGHTGREY);
        delay(90);
        currentpage = 1;
        drawGameMenu();
           if(gamemode == 1) //Easy Slow Forward
           {
            easy();

            }
            else if (gamemode == 2) //Normal Faster Forward
            {
    
             normal();

            }
            else if (gamemode == 3) //Hard Faster Random
            {
            hard();       
            }
      }
      //Game Settings
      else if(X>0 && X<230 && Y<150 && Y>85){
        tft.drawRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, LIGHTGREY);
        delay(90);
        currentpage = 2;
        X = 0;
        Y = 0;
        Z = 0;
        drawGameSettings();
      }
      //LED Settings
        else if(X>0 && X<230 && Y<70 && Y>5){
        tft.drawRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, LIGHTGREY);
        delay(90);
        currentpage = 3;
        X = 0;
        Y = 0;
        Z = 0;
        drawLEDSettings();
        } 
  }
  }
  //Start Screen
  if (currentpage == 1)
  {
    
    retrieveTouch();

      //Auf eingabe warten
       if (Z > 5 && Z < 1000)
       {  
        // good for debuggin, prints out the x,y cordinates of the press
        Serial.print("X = "); Serial.println(X);
        Serial.print("Y = "); Serial.println(Y);
      if (X>45 && X<185 && Y<150 && Y>85)
      {
        tft.fillRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, WHITE);
        delay(70);
        tft.fillRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, BLACK);
        tft.drawRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, WHITE);
        tft.setCursor(midColCursorX-32,187);
        tft.setTextColor(WHITE);
        tft.print("Stop");


        
        delay(70);
        currentpage = 0;
        analogWrite(links, 0);  
        analogWrite(rechts, 0); 
        drawHome();
      }
      
  }
  else
  {
    if(gamemode == 1) //Easy Slow Forward
           {
            easy();

            }
            else if (gamemode == 2) //Normal Faster Forward
            {
    
             normal();

            }
            else if (gamemode == 3) //Hard Faster Random
            {
              if(roundCounter < 3500)
              {
                roundCounter = roundCounter + 1;
              }
              else
              {
                hard();  
                roundCounter = 0;
              }
            }
  }
  }
   //Game Settings
    if (currentpage == 2)
  {
       retrieveTouch();
       //Auf eingabe warten
       if (Z > 5 && Z < 1000)
       {  
        // good for debuggin, prints out the x,y cordinates of the press
        Serial.print("X = "); Serial.println(X);
        Serial.print("Y = "); Serial.println(Y);
      if (X>5 && X<40 && Y<305 && Y>275)
      {
        tft.fillRoundRect(padding,padding, 35, 30, 8, WHITE);
        delay(70);
        tft.fillRoundRect(padding,padding, 35, 30, 8, BLACK);
        tft.drawRoundRect(padding,padding, 35, 30, 8, WHITE);
        tft.setCursor(15, 18);
        tft.print("<-");
        delay(70);
        currentpage = 0;
        drawHome();
      }
            else if (X>5 && X<70 && Y<210 && Y>145) //Easy Button (Gamemode 1)
      {
        tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN);
        tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        //Disable Other
        tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        gamemode = 1;
      }
            else if (X>80 && X<155 && Y<210 && Y>145) //Normal Button (Gamemode 2)
      {
        tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN); 
        tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        //Disable Other
        tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        gamemode = 2;
      }
            else if (X>170 && X<235 && Y<210 && Y>145) //Hard Button (Gamemode 3)
      {
        tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN);
        tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        //Disable Other
        tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        gamemode = 3;

      }
            else if (X>5 && X<70 && Y<75 && Y>5) //Boost On Button
      {
          tft.fillRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREEN);
          tft.drawRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
          //Disable Other
          tft.fillRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREY);
          tft.drawRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
          buttonactive = true;
          
      }
            else if (X>165 && X<235 && Y<75 && Y>5) //Boost Off Button
      {

          tft.fillRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, RED);
          tft.drawRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
          //Disable Other
          tft.fillRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREY);
          tft.drawRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
          buttonactive = false;
      }
      



      
    }

  }

  //LED Settings
    if (currentpage == 3)
  {
       retrieveTouch();
       //Auf eingabe warten
       if (Z > 5 && Z < 1000)
       {  
        // good for debuggin, prints out the x,y cordinates of the press
        Serial.print("X = "); Serial.println(X);
        Serial.print("Y = "); Serial.println(Y);
      if (X>5 && X<40 && Y<305 && Y>275)
      {
        tft.fillRoundRect(padding,padding, 35, 30, 8, WHITE);
        delay(70);
        tft.fillRoundRect(padding,padding, 35, 30, 8, BLACK);
        tft.drawRoundRect(padding,padding, 35, 30, 8, WHITE);
        tft.setCursor(15, 18);
        tft.print("<-");
        delay(70);
        currentpage = 0;
        drawHome();
      }
    }

  }


    
}
 



/////////////////
//
//
//    Functions
//
//
//
////////////////



void retrieveTouch()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  //If sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  Z = p.z;
  X = map(p.x, TS_MAXX, TS_MINX, 0, 240);
  Y = map(p.y, TS_MAXY, TS_MINY, 0, 320);




}

void drawHome() {
  tft.fillScreen(LIGHTGREY);
  //Draw Headline
  tft.setCursor(30,20);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Pukemaster     5000");

  //Draw Start Button
  tft.fillRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);
  tft.drawRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(firstRowCursorY, secondRowCursorY);
  tft.println("Start");

  //Draw Game Settings Button
  tft.fillRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);
  tft.drawRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(firstRowCursorY, thirdRowCursorY);
  tft.println("Game Settings");
  
  //Draw LED Settings
  tft.fillRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);
  tft.drawRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(firstRowCursorY, fourthRowCursorY);
  tft.println("LED Settings"); 

}

void drawGameSettings()  //Draw Game Settings Menu
{
  tft.fillScreen(LIGHTGREY);
  tft.setCursor(55,20);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Game Settings");


  tft.setCursor(80,50);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Modus:");
  tft.setCursor(20,70);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Easy/Normal/Hard");
//Gamemode 1= Easy pre select Gamemode 2 = Normal pre select Gamemode 3 = Hard pre select
  if(gamemode==1){
      tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN);
      tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
      tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
      tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
    }
  else if (gamemode==2){
      tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
      tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN);
      tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
      tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
      tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
  }  
  else if (gamemode==3){
        tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
        tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREEN);
        tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
        tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
  }
  /* Erste Reihe
  tft.fillRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
  tft.fillRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
  tft.fillRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, GREY);
  tft.drawRect(leftColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
  tft.drawRect(midColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
  tft.drawRect(rightColPositionX, secondRowVertialAlign+20, BOXSIZE, BOXSIZE, BLACK);
  */

  
  tft.setCursor(55,220);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Boost ON/OFF?");


  if (buttonactive == true){
    tft.fillRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREEN);
    tft.fillRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREY);
    tft.drawRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
    tft.drawRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);

  }else if (buttonactive == false){
    tft.fillRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, GREY);
    tft.fillRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, RED);
    tft.drawRect(leftColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);
    tft.drawRect(rightColPositionX, fourthRowVertialAlign, BOXSIZE, BOXSIZE, BLACK);

  }



  //Back Button
  tft.fillRoundRect(padding,padding, 35, 30, 8, BLACK);
  tft.drawRoundRect(padding,padding, 35, 30, 8, WHITE);
  tft.setCursor(15, 18);
  tft.setTextColor(WHITE);
  tft.print("<-");
  delay(300);

}

void drawLEDSettings()  //Draw LED Settings Menu
{
  tft.fillScreen(LIGHTGREY);
  tft.setCursor(55,20);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("LED Settings");

  //
  //Define Settings Here
  //


  //Back Button
  tft.fillRoundRect(padding,padding, 35, 30, 8, BLACK);
  tft.drawRoundRect(padding,padding, 35, 30, 8, WHITE);
  tft.setCursor(15, 18);
  tft.setTextColor(WHITE);
  tft.print("<-");
  delay(300);

}


void drawGameMenu()  //Draw Start Display
{
  
    tft.fillScreen(LIGHTGREY);
    //Draw Headline
    tft.setCursor(30,20);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Pukemaster     5000");
    for (int i=3; i >= 0; i--)
    {
    tft.fillRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, RED);
    tft.drawRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, WHITE);
    tft.setCursor(midColCursorX,187);
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.print(i);
    delay(1000);
    } 



  //
  //Define Settings Here
  //

  //Stop Button
  tft.fillRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, BLACK);
  tft.drawRoundRect(midColPositionX-35,thirdRowVertialAlign, BOXSIZE*2, BOXSIZE, 8, WHITE);
  tft.setCursor(midColCursorX-32,187);
  tft.setTextColor(WHITE);
  tft.print("Stop");

}


   void easy()
   {

     analogWrite(links, 0);  
     analogWrite(rechts, mintempo);

   }

   void normal()
   {
     analogWrite(links, 0);  
     analogWrite(rechts, maxtempo);

   }
   void hard()
   {

       randNumber = random(0, 6);
       if(randNumber == 0)
       {
        analogWrite(links, 0);  
       analogWrite(rechts, mintempo);
       }
       else if(randNumber == 1)
       {
        analogWrite(links, 0);  
       analogWrite(rechts, mintempo + 10);
       }
       else if(randNumber == 2)
       {
        analogWrite(links, 0);  
       analogWrite(rechts, mintempo + 20);
       }
       else if(randNumber == 3)
       {
        analogWrite(links, 0);  
       analogWrite(rechts, mintempo + 30);
       }
       else if(randNumber == 4)
       {
        analogWrite(links, 0);  
       analogWrite(rechts, maxtempo);
       }
       else if(randNumber == 5)
       {
        analogWrite(rechts, 0);
        analogWrite(links, mintempo);
       }

   }
   




