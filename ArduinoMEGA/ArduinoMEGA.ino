#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <SoftwareSerial.h>//serial communication to the EsP8266
#include <Fonts/FreeSansBold24pt7b.h>//font
#include <Fonts/FreeSans24pt7b.h>//font
#include <Fonts/FreeSans18pt7b.h>//font
#include <Fonts/FreeSans9pt7b.h>//font
#include <Fonts/FreeMono12pt7b.h>//font
#include <Fonts/FreeMono24pt7b.h>//font
#include "images.h"
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define RX 10
#define TX 11
SoftwareSerial esp8266(RX, TX);
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define NAVY1   0x182F//least dark
#define NAVY2   0x100D//not that dark
#define NAVY3   0x100A//darkest
#define FONT    0xE71C//gray used for fonts (looks sexier)
#define GREY1   0x52AA
#define GREY2   0x39C7
#define GREY3   0x7BCF
#define TAN     0xF425
#define MINPRESSURE 10
#define MAXPRESSURE 10000
float versnum = 0.57;
int countup, data, time1 = 12, time2 = 35, time21, time22, date1, date2 = 12, date3 = 1, date4 = 2019, weatherdigit = 69, humidity=66, percipitation=31, wind=2, weathertype=1; //ints delivered by ESP8266
int time3=0, ticksec=0, viewdate, initclock = 1; //data not gathered from esp
int scrollcur = 0, slidepage;
int ticksecpastsmall, ticksecpastlarge, ticksecpast;
int page = 0;
int homepasty2, homepasty, px, pxpre = 0, memopastx, memopasty;
int letter = 0, stroke = 0, curbox, pastbox, i2, i, i3, i4, i5, i6, i7, i4past, i3past;
int characterlist[54][60];
unsigned int sundaycolor, mondaycolor, tuesdaycolor, wednesdaycolor, thursdaycolor, fridaycolor, saturdaycolor;
unsigned long int digitadd, calcvaluecur, calcvalue1, calcvalue2, calcaction =0, activatecalc, addzero=0;
void statusbar() {
  tft.fillRect(0, 0, 320, 10, BLACK); //bg
  tft.setFont();
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.setCursor(2, 2); //setting cursor for text
  tft.print(time1);
  tft.print(":");
  tft.print(time21);
  tft.print(time22);
  tft.setCursor(285, 2); //setting cursor for text
  tft.print("v");
  tft.print(versnum);
}
void homelayout() {
  page = 0;
  //images
  tft.drawBitmap(256, 20, glogo, 50, 79, FONT); //g logo image
  tft.drawBitmap(256, 130, calendaricon, 50, 50, FONT); //calendar icon
  tft.drawBitmap(256, 205, calculatoricon, 50, 50, FONT); //calculator icon
  tft.drawBitmap(256, 280, weathericon, 50, 50, FONT); //weather icon
  tft.drawBitmap(256, 355, memoicon, 50, 50, FONT); //memo icon
  tft.drawBitmap(0, 0, wave, 240, 480, FONT); // cool wave background
  tft.fillRect(68, 172, 104, 136, FONT); //clock square 1
  tft.fillRect(72, 176, 96, 128, NAVY2); //clock square 2
  tft.setFont(&FreeSans18pt7b);
  tft.setCursor(82, 230); //setting cursor for text
  tft.setTextColor(FONT);
  tft.setTextSize(2);
  tft.println(time1);
  tft.setCursor(82, 295); //setting cursor for text
  tft.print(time21);
  tft.print(time22);
}
void cover() {
  while (scrollcur < 480) {
    if (scrollcur <= 4) {
      tft.vertScroll(0, 480, 480 - 8);
      tft.fillRect(0, 472, 320, 8, FONT); //draw bg as scroll
      scrollcur = scrollcur + 8;
    } else if (scrollcur < 480) {
      tft.vertScroll(0, 480, 480 - scrollcur - 4);
      scrollcur = scrollcur + 4;
      tft.fillRect(0, 480 - scrollcur, 240, 4, NAVY1); //draw bg as scroll
      tft.fillRect(240, 480 - scrollcur, 80, 4, NAVY3); //draw sidebar as scroll
    }
  }
  tft.fillRect(0, 472, 240, 8, NAVY1); //draw bg as scroll
  tft.fillRect(240, 472, 80, 8, NAVY3); //draw sidebar as scroll
  scrollcur = 0;
  //statusbar();
  homelayout();
}
void calendarlayout() {
  slidepage = 90;
  viewdate = date1;
  tft.fillScreen(NAVY3);
  statusbar();
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.setCursor(15, 50);
  tft.setFont(&FreeSansBold24pt7b);
  tft.print(date2);
  tft.print("/");
  tft.print(date3);
  tft.print("/");
  tft.print(date4);
  tft.drawBitmap(255, 15, calendaricon, 50, 50, FONT); //calendar icon
  tft.setFont();
  drawcalendar();
}
void drawcalendar() {
  if (viewdate == 0) { //monday
    sundaycolor = RED;  mondaycolor = FONT;  tuesdaycolor = NAVY1;  wednesdaycolor = NAVY1;  thursdaycolor = NAVY1;  fridaycolor = NAVY1;  saturdaycolor = RED;
  } else if (viewdate == 1) { //tuesday
    sundaycolor = RED;  mondaycolor = NAVY1;  tuesdaycolor = FONT;  wednesdaycolor = NAVY1;  thursdaycolor = NAVY1;  fridaycolor = NAVY1;  saturdaycolor = RED;
  } else if (viewdate == 2) { //wednesday
    sundaycolor = RED;  mondaycolor = NAVY1;  tuesdaycolor = NAVY1;  wednesdaycolor = FONT;  thursdaycolor = NAVY1;  fridaycolor = NAVY1;  saturdaycolor = RED;
  } else if (viewdate == 3) { //thursday
    sundaycolor = RED;  mondaycolor = NAVY1;  tuesdaycolor = NAVY1;  wednesdaycolor = NAVY1;  thursdaycolor = FONT;  fridaycolor = NAVY1;  saturdaycolor = RED;
  } else if (viewdate == 4) { //friday
    sundaycolor = RED;  mondaycolor = NAVY1;  tuesdaycolor = NAVY1;  wednesdaycolor = NAVY1;  thursdaycolor = NAVY1;  fridaycolor = FONT;  saturdaycolor = RED;
  } else if (viewdate == 5) { //saturday
    sundaycolor = RED;  mondaycolor = NAVY1;  tuesdaycolor = NAVY1;  wednesdaycolor = NAVY1;  thursdaycolor = NAVY1;  fridaycolor = NAVY1;  saturdaycolor = FONT;
  } else if (viewdate == 6) { //sunday
    sundaycolor = FONT;  mondaycolor = NAVY1;  tuesdaycolor = NAVY1;  wednesdaycolor = NAVY1;  thursdaycolor = NAVY1;  fridaycolor = NAVY1;  saturdaycolor = RED;
  }
  tft.fillRect(0, 70, 47, 30, sundaycolor);  tft.fillRect(47, 70, 45, 30, mondaycolor);  tft.fillRect(92, 70, 45, 30, tuesdaycolor);  tft.fillRect(137, 70, 45, 30, wednesdaycolor);  tft.fillRect(182, 70, 45, 30, thursdaycolor);  tft.fillRect(227, 70, 45, 30, fridaycolor);  tft.fillRect(270, 70, 50, 30, saturdaycolor);
  tft.drawLine(47, 70, 47, 100, WHITE);  tft.drawLine(92, 70, 92, 100, WHITE);  tft.drawLine(137, 70, 137, 100, WHITE);  tft.drawLine(182, 70, 182, 100, WHITE);
  tft.drawLine(227, 70, 227, 100, WHITE);  tft.drawLine(270, 70, 270, 100, WHITE);
  tft.drawLine(0, 70, 320, 70, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(6, 78); tft.print("SUN"); tft.setCursor(53, 78); tft.print("MON"); tft.setCursor(97, 78); tft.print("TUE"); tft.setCursor(143, 78); tft.print("WED"); tft.setCursor(187, 78); tft.print("THU"); tft.setCursor(233, 78); tft.print("FRI"); tft.setCursor(278, 78); tft.print("SAT");
  tft.setTextColor(NAVY3);
  tft.fillRect(0, slidepage + 10, 340, 390, FONT);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, slidepage + 17); //set cursor
  tft.setFont();
  tft.println("  0:00\n\n  1:00\n\n  2:00\n\n  3:00\n\n  4:00\n\n  5:00\n\n  6:00\n\n  7:00\n\n  8:00\n\n  9:00\n\n 10:00\n\n 11:00\n\n 12:00\n\n 13:00\n\n 14:00\n\n 15:00\n\n 16:00\n\n 17:00\n\n 18:00\n\n 19:00\n\n 20:00\n\n 21:00\n\n 22:00\n\n 23:00\n");
  tft.fillRect(0, slidepage - 1 + (16 * time1) + 6, 340, 2, RED); //marker saying where you are in claendar
  char ENG[28] = "Intro to Engineering Design";
  char EXAM[19] = "Common Exam Period";
  char CALCULUS[23] = "Calculus and Functions";
  char CHEM[18] = "General Chemistry";
  if (viewdate == 0) {
    eventload(8, EXAM, " 8");
    eventload(10, CALCULUS, "10");
    event2hour(12, "Composition and Rhetoric", "12", "13");
    eventload(14, ENG, "14");
    eventload(15, ENG, "15");
  } else if (viewdate == 1) {
    event2hour(10, ENG, "10", "11");
    eventload(12, CHEM, "12");
  } else if (viewdate == 2) {
    eventload(8, EXAM, " 8");
    eventload(10, CALCULUS, "10");
    eventload(13, CHEM, "13");
    eventload(16, "The Drexel Experience", "10");
    eventload(18, EXAM, "18");
  } else if (viewdate == 3) {
    eventload(10, CALCULUS, "10");
    eventload(12, CHEM, "12");
    event2hour(14, CHEM, "14", "15");
  } else if (viewdate == 4) {
    eventload(8, EXAM, " 8");
    eventload(10, CALCULUS, "10");
  }
}
void event2hour(float eventtime, char eventname[28], char eventtimeread[2], char eventtimeread2[2]) {
  tft.fillRect(40, slidepage + 4 + (16 * eventtime) + 10, 230, 30, NAVY1); //marker saying where you are in claendar
  tft.fillRect(0, slidepage + 4 + (16 * eventtime) + 10, 40, 30, NAVY3); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(0, slidepage + 6 + (16 * eventtime) + 11); //setting cursor for text
  tft.print(" "); tft.print(eventtimeread); tft.print(":00");
  tft.setCursor(0, slidepage + 6 + (16 * (eventtime + 1)) + 11);
  tft.print(" "); tft.print(eventtimeread2); tft.print(":00");
  tft.setCursor(45, slidepage + 16 + (16 * eventtime) + 10); //setting cursor for text
  tft.println(eventname);
}
void eventload(float eventtime, char eventname[28], char eventtimeread[2]) {
  tft.fillRect(40, slidepage + 4 + (16 * eventtime) + 10, 230, 14, NAVY1); //marker saying where you are in claendar
  tft.fillRect(0, slidepage + 4 + (16 * eventtime) + 10, 40, 14, NAVY3); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(0, slidepage + 6 + (16 * eventtime) + 11); //setting cursor for text
  tft.print(" "); tft.print(eventtimeread); tft.print(":00");
  tft.setCursor(45, slidepage + 6 + (16 * eventtime) + 11); //setting cursor for text
  tft.println(eventname);
}
void calculatorlayout() {
  drawcalculator();
  tft.fillRect(0,110,320,430,GREY1);
  tft.fillRoundRect(2  , 114, 76, 88, 2,GREY2);  tft.fillRoundRect(82 , 114, 76, 88, 2,GREY2);  tft.fillRoundRect(162, 114, 76, 88, 2,GREY2);  tft.fillRoundRect(242, 114, 76, 88, 2,GREY2);
  tft.fillRoundRect(2  , 206, 76, 88, 2,GREY2);  tft.fillRoundRect(82 , 206, 76, 88, 2,GREY2);  tft.fillRoundRect(162, 206, 76, 88, 2,GREY2);  tft.fillRoundRect(242, 206, 76, 88, 2,GREY2);
  tft.fillRoundRect(2  , 298, 76, 88, 2,GREY2);  tft.fillRoundRect(82 , 298, 76, 88, 2,GREY2);  tft.fillRoundRect(162, 298, 76, 88, 2,GREY2);  tft.fillRoundRect(242, 298, 76, 88, 2,GREY2);
  tft.fillRoundRect(2  , 390, 76, 88, 2,GREY2);  tft.fillRoundRect(82 , 390, 76, 88, 2,GREY2);  tft.fillRoundRect(162, 390, 76, 88, 2,GREY2);  tft.fillRoundRect(242, 390, 76, 88, 2,GREY2);
  tft.setTextColor(GREY1);
  tft.setTextSize(1);
  tft.setFont(&FreeMono24pt7b);
  tft.setCursor(50, 194); tft.print("1");  tft.setCursor(130, 194); tft.print("2");  tft.setCursor(206, 194); tft.print("3");
  tft.setCursor(50, 286); tft.print("4");  tft.setCursor(130, 286); tft.print("5");  tft.setCursor(206, 286); tft.print("6");
  tft.setCursor(50, 378); tft.print("7");  tft.setCursor(130, 378); tft.print("8");  tft.setCursor(206, 378); tft.print("9");
  tft.setCursor(130, 470); tft.print("0");
  tft.setTextColor(GREY3);
  tft.setCursor(50, 470); tft.print("C");  tft.setCursor(206, 470); tft.print("=");
  tft.setCursor(286, 194); tft.print("/"); tft.setCursor(286, 286); tft.print("X"); tft.setCursor(286, 378); tft.print("-"); tft.setCursor(286, 470); tft.print("+");
  tft.setFont();
}
void drawcalculator(){
  tft.fillRect(0,00,320,110,BLACK);
  tft.setTextColor(GREY1);
  tft.setTextSize(1);
  tft.setFont(&FreeMono12pt7b);
  tft.setCursor(0, 40);
  if (calcvalue1 != 0){
    tft.print(calcvalue1);
  }
  if (calcaction == 1){
    tft.print("/");
  } else if (calcaction == 2){
    tft.print("X");
  } else if (calcaction == 3){
    tft.print("-");
  } else if (calcaction == 4){
    tft.print("+");
  }
  if (calcvalue2 != 0){
    tft.print(calcvalue2);
  }
  tft.setTextColor(GREY3);
  tft.setTextSize(1);
  tft.setFont(&FreeMono24pt7b);
  tft.setCursor(0, 80); tft.print(calcvaluecur);
  statusbar();
}
void weatherlayout(){
  //tft.fillScreen(0x07E0);
  /*
  tft.fillRect(0,0  ,320,10, 0x0000);
  tft.fillRect(0,50 ,320,10, 0x0022);
  tft.fillRect(0,60 ,320,10, 0x0042);
  tft.fillRect(0,70 ,320,10, 0x0062);
  tft.fillRect(0,80 ,320,10, 0x0082);
  tft.fillRect(0,90,320,10, 0x00A9);
  tft.fillRect(0,100,320,10, 0x00C9);
  tft.fillRect(0,110,320,10, 0x00EA);
  tft.fillRect(0,120,320,10, 0x0043);
  tft.fillRect(0,180,320,10, 0x0043);
  tft.fillRect(0,200,320,10, 0x0043);
  tft.fillRect(0,220,320,10, 0x0043);
  tft.fillRect(0,240,320,10, 0x0043);
  tft.fillRect(0,260,320,10, 0x0043);
  tft.fillRect(0,280,320,10, 0x0043);
  tft.fillRect(0,300,320,10, 0x0043);
  tft.fillRect(0,320,320,10, 0x0043);
  tft.fillRect(0,340,320,10, 0x0043);
  tft.fillRect(0,360,320,10, 0x0043);
  tft.fillRect(0,380,320,10, 0x0043);
  tft.fillRect(0,400,320,10, 0x0043);
  tft.fillRect(0,420,320,10, 0x0043);
  tft.fillRect(0,440,320,10, 0x0043);
  tft.fillRect(0,460,320,10, 0x0043);
  */
  uint8_t aspect=0;
  int16_t colormask[] = { 0x001F, 0x07E0, 0xF800, 0xFFFF };
  uint16_t dx=2, rgb, n, wid, ht;
  for (n = 0; n < 256; n++) {
                rgb = n*1;
                rgb = tft.color565(0, 255, rgb);
                tft.fillRect(0, n * dx, 320, dx, rgb);
  }
  rgb = tft.color565(255, 0, 255);
  //tft.fillRect(0,255,320,225,rgb);
  /*

  for (byte x = 0; x < 200; x++) { // generate a simple grayscale gradient
   uint16_t color = (uint16_t)(255 * x) / 200;
   pixels[x] = tft.color565(000, 000, color);
  }
  int z;
  for (z <48){
    z++;
    Serial.println(z);
    Serial.println(pixels[z*4]);
    tft.fillRect(0,z*10,320,10,pixels[z*4]);
    delay(10);
  }
  */
  /*
  for (byte y = 0; y < 3; y++) { // 3 lines
     tft.pushColors(pixels, 0, 200); // draw the generated colors
  }
  */
  tft.fillRect(30,30,320,10,WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.setFont(&FreeSans24pt7b);
  tft.setCursor(20, 200); tft.print(weatherdigit);
  tft.setFont(&FreeSans18pt7b);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 300);  tft.print("Humidity");
  tft.setCursor(20, 380);  tft.print("Wind Speed");
  tft.setTextColor(WHITE);
  tft.setCursor(20, 340);  tft.print(humidity);  tft.print("%");
  tft.setCursor(20, 420);  tft.print(wind); tft.print("mph");

}
void memolayout() {
  tft.fillRect(0,0,320,50,BLACK);
  tft.fillRoundRect(0, 10, 320, 65, 30, WHITE);
  tft.fillRect(0,35,320,450,WHITE);
  int drawlinememo = 9;
  int curdrawline = 1;
  while (drawlinememo >= curdrawline ) {
    tft.drawFastHLine(0, 10 + curdrawline * 30, 320, RED);
    curdrawline++;
  }
  drawpastmemo();
  tft.fillRoundRect(0, 290, 320, 250, 30, NAVY3);
  tft.fillRoundRect(15, 310, 90, 140, 15, FONT);
  tft.fillRoundRect(115, 310, 90, 140, 15, FONT);
  tft.fillRoundRect(215, 310, 90, 140, 15, FONT);
  tft.fillRoundRect(0, 460, 320, 80, 15, NAVY1);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.setCursor(65, 475); //set cursor
  tft.println("SAVE   LOAD   DELETE");
}
void drawpastmemo() {
  i2 = 1;
  i = 4;
  while (i2 <= 54) {
    if (characterlist[i2][i] == 0){
      i=180;
    }
    while (i <= 68) {//198
      i3 = characterlist[i2][i];
      i4 = characterlist[i2][i + 1];
      i3past = characterlist[i2][i - 2];
      i4past = characterlist[i2][i - 1];
      if (i3 != 0) {
        if (i2 <= 18) {
          //tft.fillCircle(i3 / 4 + (i2 * 17) - 10, i4 / 4, 1, BLACK);
          tft.fillCircle(i3 / 4 + (i2 * 17) - 10, i4 / 4 - 70, 1, BLACK);
        } else if (i2 <= 36) {
          i5=i2-18;
          tft.fillCircle(i3 / 4 + (i5 * 17) - 10, i4 / 4 - 30, 1, BLACK);
        } else if (i2 <= 54) {
          i5=i2-36;
          tft.fillCircle(i3 / 4 + (i5 * 17) - 10, i4 / 4 - 10, 1, BLACK);
        }
      }
      Serial.print("i2-");  Serial.print(i2);  Serial.print("| i-");  Serial.print(i);  Serial.print("| 0-");  Serial.print(characterlist[i2][i]);  Serial.print("| 1-");  Serial.println(characterlist[i2][i + 1]);
      i = i + 2;
    }
    i2++;
    i = 4;
  }
}
void rebootscreen(){
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.invertDisplay(1);
  tft.fillScreen(NAVY1);
  tft.fillRect(240, 0, 80, 480, NAVY3); //draw sidebar as scroll
  homelayout();
}
void errorscreen(int errornumber){
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(0);
  tft.invertDisplay(1);
  tft.fillScreen(RED);
  tft.setTextSize(4);
  tft.setCursor(0, 0); //set cursor
  tft.println("ERROR");
  tft.setTextSize(1);
  tft.setCursor(0, 30); //set cursor
  if (errornumber ==11) {
    tft.println("ESP8266 was unable to be reached.");
  } else if (errornumber ==12) {
    tft.println("ESP8266 was unable to connect to the internet.");
  }  else {
      tft.println("Unknown error.");
    }
  tft.setTextSize(2);
  tft.println("\nREBOOTING IN 5 SECONDS.");
  delay(5000);
  setup();
}
void setup() {
  Serial.begin(115200);
  esp8266.begin(9600);
  Serial.println(data);
  if (data == 4000) {
    countup = 1;
  }  else if (data == 4069) {
    errorscreen(12);
  }  else if (data == 4070) {//when ESP8266 is installed make 0!
    errorscreen(11);
  }  else if (data != 200) {
    if (countup == 1) {
      weatherdigit = data;
      countup++;
      Serial.print("The weather is");
      Serial.println(weatherdigit);
    } else if (countup == 2) {
      humidity = data;
      countup++;
      Serial.print("The humidity is");
      Serial.println(humidity);
    } else if (countup == 3) {
      percipitation = data;
      countup++;
      Serial.print("The percipitation is");
      Serial.println(percipitation);
    } else if (countup == 4) {
      wind = data;
      countup++;
      Serial.print("The wind is");
      Serial.println(wind);
    } else if (countup == 5) {
      weathertype = data;
      countup++;
      Serial.print("The weather type is");
      Serial.println(weatherdigit);
    } else if (countup == 6) {
      date1 = data;
      countup++;
      Serial.print("The weekday is ");
      Serial.println(date1);
    } else if (countup == 7) {
      date2 = data;
      countup++;
      Serial.print("The month is ");
      Serial.println(date2);
    } else if (countup == 8) {
      date3 = data;
      countup++;
      Serial.print("The day is ");
      Serial.println(date3);
    } else if (countup == 9) {
      date4 = data;
      countup++;
      Serial.print("The year is ");
      Serial.println(date4);
    } else if (countup == 10) {
      time1 = data;
      countup++;
      Serial.print("The hour is ");
      Serial.println(time1);
    } else if (countup == 11) {
      time2 = data;
      countup = 0;
      Serial.print("The min is ");
      Serial.println(time2);
      time22 = time2 / 10; //X0 hand
      time21 = time2 % 10; //0X hand
    }
  }
  Serial.println("PassportOS Started");
  Serial.print("Version: ");  Serial.println(versnum);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.setRotation(0);
  tft.invertDisplay(1);
  tft.fillScreen(NAVY1);
  tft.fillRect(240, 0, 80, 480, NAVY3); //draw sidebar as scroll
  homelayout();
}
void loop() {
  digitalWrite(13, HIGH);//hell if i know
  TSPoint p = ts.getPoint();//get touch point
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {//touch points
    p.x = p.x + p.y;
    p.y = p.x - p.y;
    p.x = p.x - p.y;
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    Serial.print("x-");
    Serial.print(p.x);
    Serial.print(" y- ");
    Serial.println(p.y);
    if ((0 < p.x && p.x < 275)) { //go home code
      if ((0 < p.y && p.y < 40) || homepasty2 != 0) { //check if in top of Screen
        if ((0 < homepasty && homepasty < 40)) { //check if in a few secs you still there
          tft.fillRect(0, 0, 320, 10, GREEN); //say you good to go
          if ((40 < homepasty2 && homepasty2 < 480)) { //check if in a few secs you moved down
            homepasty = 0;
            homepasty2 = 0;
            Serial.print("going home...");
            cover();
          } else if ((0 < homepasty2 && homepasty2 < 40)) { //check if you havent moved
            homepasty = 0;
            homepasty2 = 0;
            statusbar();
            delay(600);
          } else {
            delay(400);
            TSPoint p = ts.getPoint();//get touch point
            p.x = p.x + p.y;
            p.y = p.x - p.y;
            p.x = p.x - p.y;
            p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
            p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
            homepasty2 = p.y; //set pastx to p.x to compare
          }
        } else {
          delay(200);
          TSPoint p = ts.getPoint();//get touch point
          p.x = p.x + p.y;
          p.y = p.x - p.y;
          p.x = p.x - p.y;
          p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
          p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
          homepasty = p.y; //set pastx to p.x to compare
        }
      } else if ((0 < homepasty && homepasty < 20 && homepasty2 <= 20)) { //if you arent there anymore set pastx to 0
        homepasty = 0;
        homepasty2 = 0;
        statusbar();
      }
    }
    if (page == 0) { //home page
      if ((256 < p.x && p.x < 320)) {//check in bar
        if ((10 < p.y && p.y < 110)) {//reboot system
          rebootscreen();
        }
        if ((130 < p.y && p.y < 190)) {//calendar!
          page = 1;
          calendarlayout();
          statusbar();
        }
        if ((205 < p.y && p.y < 255)) {//calculator~
            page = 2;
            calculatorlayout();
            statusbar();
        }
        if ((280 < p.y && p.y < 330)) {//calculator~
            page = 3;
            weatherlayout();
            statusbar();
        }
        if ((355 < p.y && p.y < 410)) {//memo!
          page = 4;
          memolayout();
          statusbar();
        }
      }
    } else if (page == 1) { //calendar
      if ((70 < p.y && p.y < 110)) {
        if ((0 < p.x && p.x < 47)) {
          viewdate = 6;
          drawcalendar();
        } else if ((47 < p.x && p.x < 92)) {
          viewdate = 0;
          drawcalendar();
        } else if ((92 < p.x && p.x < 137)) {
          viewdate = 1;
          drawcalendar();
        } else if ((137 < p.x && p.x < 182)) {
          viewdate = 2;
          drawcalendar();
        } else if ((182 < p.x && p.x < 227)) {
          viewdate = 3;
          drawcalendar();
        } else if ((227 < p.x && p.x < 270)) {
          viewdate = 4;
          drawcalendar();
        } else if ((270 < p.x && p.x < 320)) {
          viewdate = 5;
          drawcalendar();
        }
      }
    } else if (page == 2) { //calculator
      if (p.y >114 && p.y <466){
        if (p.y >114 && p.y <200){
          if (p.x >2 && p.x <78){
            digitadd = 1;
          } else if (p.x >82 && p.x <158){
            digitadd = 2;
          } else if (p.x >162 && p.x <238){
            digitadd = 3;
          } else if (p.x >242 && p.x <316){
            calcaction=1;
          }
        } else if (p.y >206 && p.y <294){
          if (p.x >2 && p.x <78){
            digitadd = 4;
          } else if (p.x >82 && p.x <158){
            digitadd = 5;
          } else if (p.x >162 && p.x <238){
            digitadd = 6;
          } else if (p.x >242 && p.x <316){
            calcaction=2;
          }
        } else if (p.y >298 && p.y <386){
          if (p.x >2 && p.x <78){
            digitadd = 7;
          } else if (p.x >82 && p.x <158){
            digitadd = 8;
          } else if (p.x >162 && p.x <238){
            digitadd = 9;
          } else if (p.x >242 && p.x <316){
            calcaction=3;
          }
        } else if (p.y >390 && p.y <466){
          if (p.x >2 && p.x <78){
            calcaction=6;
          } else if (p.x >82 && p.x <158){
            digitadd = 0;
            addzero = 1;
          } else if (p.x >162 && p.x <238){
            activatecalc=1;
          } else if (p.x >242 && p.x <316){
            calcaction=4;
          }
        }
        if (calcaction != 0){//check if an action has been requested
          if (calcvalue2 == 0 && calcvalue1 ==0){//check if 1st box has been written in
            calcvalue1 = calcvaluecur;
            calcvaluecur = 0;
          } if (calcvalue2 != 0 && calcvalue1 !=0){//check if both boxes have been filled
            calcvalue1 = calcvaluecur;
            calcvalue2 = 0;
            calcvaluecur = 0;
          }
          if (activatecalc== 1){//if equals was clicked
            calcvalue2 = calcvaluecur;
            calcvaluecur = 0;
            if (calcaction ==1){//division
              calcvaluecur = calcvalue1/calcvalue2;
            } else if (calcaction ==2){//multiplication
              calcvaluecur = calcvalue1*calcvalue2;
            } else if (calcaction ==3){//subtraction
              calcvaluecur = calcvalue1-calcvalue2;
            } else if (calcaction ==4){//addition
              calcvaluecur = calcvalue1+calcvalue2;
            }
            if (calcvaluecur > 1000000000){//stop overflow errors
              calcvaluecur=0;
            }
            //calcaction=0;
            activatecalc=0;
            //digitadd=0;
          } else {
            if (digitadd !=0 || addzero==1){
              calcvaluecur = calcvaluecur*10+digitadd;
              digitadd=0;
              addzero=0;
            }
          }
          if (calcaction == 6){
            calcvalue2 = 0;
            calcvalue1 = 0;
            calcvaluecur = 0;
            calcaction=0;
            digitadd=0;
          }
        } else {
          if (digitadd !=0 || addzero==1){
            calcvaluecur = calcvaluecur*10+digitadd;
            digitadd=0;
            addzero=0;
          }
        }

        drawcalculator();
      }
    } else if (page == 3) { //weather

    } else if (page == 4) { //memo
      if (p.x < 107 && p.x > 16) { //check which box it is in
        px = p.x - 16;
      } else if (p.x < 215 && p.x > 115) { //check which box it is in
        px = p.x - 115;
      } else if (p.x < 320 && p.x > 215) { //check which box it is in
        px = p.x - 215;
      }
      int memomindist = 3;//distance between dots
      if (p.y > characterlist[letter][stroke - 1] && p.y - characterlist[letter][stroke - 1] > memomindist || p.y < characterlist[letter][stroke - 1] && characterlist[letter][stroke - 1] - p.y > memomindist || px > characterlist[letter][stroke - 2] && px - characterlist[letter][stroke - 2] > memomindist || px < characterlist[letter][stroke - 2] && characterlist[letter][stroke - 2] - px > memomindist) {
        i5 = 0;
        Serial.println(p.y - characterlist[letter][stroke - 1]);
        Serial.println(characterlist[letter][stroke - 1] - p.y);
        Serial.println(px - characterlist[letter][stroke - 2]);
        Serial.println(characterlist[letter][stroke - 2] - px);
        if (p.x < 107 && p.x > 16) { //check which box it is in
          px = p.x - 16;
          characterlist[letter][stroke] =  px;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        } else if (p.x < 215 && p.x > 115) { //check which box it is in
          px = p.x - 115;
          characterlist[letter][stroke] =  px;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        } else if (p.x < 320 && p.x > 215) { //check which box it is in
          px = p.x - 215;
          characterlist[letter][stroke] =  px;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        }
      }
      if (p.y > 310 && p.y <450){
        if (p.x < 107 && p.x > 16) { //check which box it is in
          tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
          if (letter <=18){
            tft.fillCircle((p.x - 16) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
          } else if (letter <=36){
            tft.fillCircle((p.x - 16) / 4 + (i6 * 17) + ((letter-18) * 17), p.y / 4 + (i7 * 10) - 40, 1, BLACK);
          } else if (letter <=54){
            tft.fillCircle((p.x - 16) / 4 + (i6 * 17) + ((letter-36) * 17), p.y / 4 + (i7 * 10) - 10, 1, BLACK);
          }
          pastbox = curbox;
          curbox = 1;
        } else if (p.x < 215 && p.x > 115) { //check which box it is in
          tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
          if (letter <=18){
            tft.fillCircle((p.x - 115) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
          } else if (letter <=36){
            tft.fillCircle((p.x - 115) / 4 + (i6 * 17) + ((letter-18) * 17), p.y / 4 + (i7 * 10) - 40, 1, BLACK);
          } else if (letter <=54){
            tft.fillCircle((p.x - 115) / 4 + (i6 * 17) + ((letter-36) * 17), p.y / 4 + (i7 * 10) - 10, 1, BLACK);
          }
          pastbox = curbox;
          curbox = 2;
        } else if (p.x < 320 && p.x > 215) { //check which box it is in
          tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
          if (letter <=18){
            tft.fillCircle((p.x - 215) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
          } else if (letter <=36){
            tft.fillCircle((p.x - 215) / 4 + (i6 * 17) + ((letter-18) * 17), p.y / 4 + (i7 * 10) - 40, 1, BLACK);
          } else if (letter <=54){
            tft.fillCircle((p.x - 215) / 4 + (i6 * 17) + ((letter-36) * 17), p.y / 4 + (i7 * 10) - 10, 1, BLACK);
          }
          pastbox = curbox;
          curbox = 3;
        }
      }
      if (pastbox != curbox) {
        if (pastbox == 0 && curbox == 1 || pastbox == 1 && curbox == 2 || pastbox == 2 && curbox == 3 || pastbox == 3 && curbox == 1) { //next letter
          characterlist[letter][stroke - 2] =  characterlist[letter][stroke - 4];
          characterlist[letter][stroke - 1] =  characterlist[letter][stroke - 3];
          letter++;
          stroke = 0;

          if (p.x < 107 && p.x > 16) { //check which box it is in
            tft.fillRoundRect(215, 310, 90, 140, 15, FONT);
          } else if (p.x < 215 && p.x > 115) { //check which box it is in
            tft.fillRoundRect(15, 310, 90, 140, 15, FONT);
          } else if (p.x < 320 && p.x > 215) { //check which box it is in
            tft.fillRoundRect(115, 310, 90, 140, 15, FONT);
          }
        } else if (pastbox == 1 && curbox == 3 || pastbox == 2 && curbox == 1 || pastbox == 3 && curbox == 2) { //jump a letter (space!)
          letter = letter + 2;
          stroke = 0;
          if (p.x < 107 && p.x > 16) { //check which box it is in
            tft.fillRoundRect(115, 310, 90, 140, 15, FONT); //cover past box
          } else if (p.x < 215 && p.x > 115) { //check which box it is in
            tft.fillRoundRect(215, 310, 90, 140, 15, FONT); //cover past box
          } else if (p.x < 320 && p.x > 215) { //check which box it is in
            tft.fillRoundRect(15, 310, 90, 140, 15, FONT); //cover past box
          }
        }
      }
      memopastx = p.x;
      memopasty = p.y;
    }
  }
  if (ticksec == 500) { //convert ticks to seconds
    time3++;
    ticksec = 0;
  }
  if (time3 == 60) { //converts ticks to seconds (1 sec ~=~ 300 tick)36000
    time22 = time22 + 1;
    time3 = 0;
    if (time22 == 10) {
      time21++;
      time22 = 0;
    }
    if (time21 == 6) {
      time1++;
      time21 = 0;
    }
    if (time1 == 23) {
      time1 = 0;
    }
    if (page == 0) {
      tft.fillRect(82, 176, 76, 128, NAVY2);
      tft.setFont(&FreeSans18pt7b);
      tft.setCursor(82, 230); //setting cursor for text
      tft.setTextColor(FONT);
      tft.setTextSize(2);
      tft.println(time1);
      tft.setCursor(82, 295); //setting cursor for text
      tft.print(time21);
      tft.print(time22);
    } else {
      statusbar();
    }
  }
  delay(1);
  ticksec++;
  ticksecpastlarge = ticksecpastlarge = ticksec + 500;
  ticksecpastsmall = ticksecpastsmall = ticksec - 500;
  ticksecpast = ticksec;
}
