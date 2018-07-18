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

#define REDBAR_MINX 80
#define GREENBAR_MINX 130
#define BLUEBAR_MINX 180
#define BAR_MINY 30
#define BAR_HEIGHT 250
#define BAR_WIDTH 30

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
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  tft.reset();

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
  identifier=0x9341;
  tft.begin(identifier);

  //Background color
  tft.fillScreen(LIGHTGREY);

  // Draw Main Menu
  tft.setRotation(2);
 // createButtons();

  
  tft.fillScreen(LIGHTGREY);


  currentpage = 0;

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
    tft.fillRect(BAR_MINY -10, BLUEBAR_MINX, i, 10, BLACK);
    delay(0.000000000000000000000000000000000000000000000000009);
  }

  tft.fillScreen(LIGHTGREY);

  drawHome();
  currentpage = 0;




  
  


}

void loop(){
retrieveTouch();
  
  //Homescreen
  if (currentpage == 0){
     //Auf eingabe warten
  if (Z > 5 && Z < 1000)
  { 
    // good for debuggin, prints out the x,y cordinates of the press
    tft.setTextSize(3);
    Serial.print("X = "); Serial.println(X);
    Serial.print("Y = "); Serial.println(Y);
    
     //Start angedrückt
    if(X>0 && X<230 && Y<230 && Y>160){
      tft.drawRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, WHITE);
      delay(90);
      currentpage = 1;
      }
      //Game Settings
      else if(X>0 && X<230 && Y<150 && Y>85){
      tft.drawRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, WHITE);

      delay(90);
      currentpage = 2;}
      //LED Settings
      else if(X>0 && X<230 && Y<70 && Y>5){
      tft.drawRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, WHITE);
      delay(90);
      currentpage = 3;}
      
    
  }
  }
  //Settings Screen
  if (currentpage == 1)
  {
    
    currentpage = 0;
    drawHome();

  }

    if (currentpage == 2)
  {
    
    currentpage = 0;
    drawHome();

  }

    if (currentpage == 3)
  {
    
    currentpage = 0;
    drawHome();

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
  //(initial x,initial y,width,height,color)
  double secondRowVertialAlign = secondRow + verticalAlign;
  double thirdRowVertialAlign = thirdRow + verticalAlign;
  double fourthRowVertialAlign = fourthRow + verticalAlign;

  /***Draw filled squares with specified dimensions and position***/
  //First Row
  tft.setCursor(30,20);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Pukemaster     5000");

  //Second Row 
  tft.fillRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);
  //Third Row
  tft.fillRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);
  //Fourth Row
  tft.fillRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding *2), BOXSIZE, GREY);

  /***Draw Borders around squares***/

  //Start Button
  tft.drawRect(leftColPositionX, secondRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);
  //Settings Button
  tft.drawRect(leftColPositionX, thirdRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);
  //Exit Button
  tft.drawRect(leftColPositionX, fourthRowVertialAlign, (BOXSIZE * 3) + (padding * 2), BOXSIZE, BLACK);




  tft.setTextSize(2);
  tft.setTextColor(BLACK);


  tft.setCursor(firstRowCursorY, secondRowCursorY);
  tft.println("Start");


  tft.setCursor(firstRowCursorY, thirdRowCursorY);
  tft.println("Game Settings");


  tft.setCursor(firstRowCursorY, fourthRowCursorY);
  tft.println("LED Settings");



  
}




