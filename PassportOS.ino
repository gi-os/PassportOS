#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <SoftwareSerial.h>//serial communication to the EsP8266
#include <Fonts/FreeSans18pt7b.h>//font
#include <Fonts/FreeSans9pt7b.h>//font
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
#define MINPRESSURE 10
#define MAXPRESSURE 10000
int countup, data, time1 = 12, time2 = 35, time21, time22, time3, ticksec, date1, date2, date3, weatherdigit = 69; //ints delivered by ESP8266
int scrollcur = 0, slidepage;
int ticksecpastsmall, ticksecpastlarge, ticksecpast;
int page = 0;
int homepasty2, homepasty, px, pxpre = 0, memopastx, memopasty;
int letter = 0, stroke = 0, curbox, pastbox, i2, i, i3, i4, i5, i6, i7,i4past, i3past;
int characterlist[16][50];
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
}
void homelayout() {
  page = 0;
  //images
  tft.drawBitmap(256, 20, glogo, 50, 79, FONT); //g logo image
  tft.drawBitmap(256, 130, calendaricon, 50, 50, FONT); //calendar icon
  tft.drawBitmap(256, 205, calculatoricon, 50, 50, FONT); //calculator icon
  tft.drawBitmap(256, 280, weathericon, 50, 50, FONT); //weather icon
  tft.drawBitmap(256, 355, memoicon, 50, 50, FONT); //memo icon
  tft.drawBitmap(0, 0, wave, 240, 480, NAVY3); // cool wave background
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
  //Flavor text
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(18, 30); //setting cursor for text
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.println("Welcome Back Giovanni!");

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
  statusbar();
  homelayout();
}
void calendarlayout() {
  tft.setTextColor(NAVY3);
  tft.fillRect(0, slidepage + 10, 340, 800, FONT); //cool bg
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, slidepage + 15); //set cursor
  //tft.setFont();
  tft.println("  0:00\n\n\n\n  1:00\n\n\n\n  2:00\n\n\n\n  3:00\n\n\n\n  4:00\n\n\n\n  5:00\n\n\n\n  6:00\n\n\n\n  7:00\n\n\n\n  8:00\n\n\n\n  9:00\n\n\n\n 10:00\n\n\n\n 11:00\n\n\n\n 12:00\n\n\n\n 13:00\n\n\n\n 14:00\n\n\n\n 15:00\n\n\n\n 16:00\n\n\n\n 17:00\n\n\n\n 18:00\n\n\n\n 19:00\n\n\n\n 20:00\n\n\n\n 21:00\n\n\n\n 22:00\n\n\n\n 23:00\n");
  tft.fillRect(0, slidepage - 1 + (32 * time1), 260, 2, RED); //marker saying where you are in claendar
  char ENG[28] = "Intro to Engineering Design";
  char EXAM[19] = "Common Exam Period";
  char CALCULUS[23] = "Calculus and Functions";
  char CHEM[18] = "General Chemistry";
  if        (date1 == 0) {
    eventload(8, EXAM);
    eventload(10, CALCULUS);
    event2hour(12, "Composition and Rhetoric");
    eventload(14, ENG);
    eventload(15, ENG);
  } else if (date1 == 1) {
    event2hour(10, ENG);
    eventload(12, CHEM);
  } else if (date1 == 2) {
    eventload(8, EXAM);
    eventload(10, CALCULUS);
    eventload(13, CHEM);
    eventload(16, "The Drexel Experience");
    eventload(18, EXAM);
  } else if (date1 == 3) {
    eventload(10, CALCULUS);
    eventload(12, CHEM);
    event2hour(14, CHEM);
  } else if (date1 == 4) {
    eventload(8, EXAM);
    eventload(10, CALCULUS);
  }
}
void calendarscrollgetto() {
  while (480 - px / 2 != pxpre) {
    if (pxpre >= 480) {
      pxpre = 0;
    }
    if (px > pxpre) {
      tft.vertScroll(10, 480, pxpre);
      pxpre++;
      delay(1);
    } else if (px < pxpre) {
      tft.vertScroll(10, 480, 480 - pxpre);
      pxpre++;
      delay(1);
    }
  }
}
void scrollcalendar() {
  px = 480 - px / 2;
  while (px != pxpre) {
    tft.vertScroll(10, 480, pxpre);
    pxpre++;
  }
  if (px == pxpre) {
    if (px < 480) {
      tft.vertScroll(10, 480, px);
      scrollcur = scrollcur + 4;
      tft.fillRect(10, px, 240, 20, NAVY1); //draw bg as scroll
      tft.fillRect(240, px, 80, 20, NAVY3); //draw sidebar as scroll
    }
  }
  /*
    char ENG[28] ="Intro to Engineering Design";
    char EXAM[19] = "Common Exam Period";
    char CALCULUS[23] ="Calculus and Functions";
    char CHEM[18] = "General Chemistry";
    GFXcanvas1 *_scroll;
    _scroll = new GFXcanvas1(320, 480-px/2);
    _scroll->setTextColor(BLACK);
    _scroll->fillRect(0, slidepage + 10, 340, 800, FONT); //cool bg
    _scroll->setTextColor(BLACK);
    _scroll->setTextSize(1);
    _scroll->setCursor(0, slidepage + 15); //set cursor
    _scroll->println("  0:00\n\n\n\n  1:00\n\n\n\n  2:00\n\n\n\n  3:00\n\n\n\n  4:00\n\n\n\n  5:00\n\n\n\n  6:00\n\n\n\n  7:00\n\n\n\n  8:00\n\n\n\n  9:00\n\n\n\n 10:00\n\n\n\n 11:00\n\n\n\n 12:00\n\n\n\n 13:00\n\n\n\n 14:00\n\n\n\n 15:00\n\n\n\n 16:00\n\n\n\n 17:00\n\n\n\n 18:00\n\n\n\n 19:00\n\n\n\n 20:00\n\n\n\n 21:00\n\n\n\n 22:00\n\n\n\n 23:00\n");
    _scroll->fillRect(0, slidepage - 1 + (32 * time1), 260, 2, RED); //marker saying where you are in claendar
    //_scroll->print(time21);
    //_scroll->print(time22);
    tft.drawBitmap(0, 0,_scroll->getBuffer(),320, 480-px/2, FONT, NAVY3);
    delete _scroll;

    //clocklarge();
    tft.setTextColor(NAVY3);
    tft.fillRect(0, slidepage + 10, 340, 800, FONT); //cool bg
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, slidepage + 15); //set cursor
    //tft.setFont();
    tft.println("  0:00\n\n\n\n  1:00\n\n\n\n  2:00\n\n\n\n  3:00\n\n\n\n  4:00\n\n\n\n  5:00\n\n\n\n  6:00\n\n\n\n  7:00\n\n\n\n  8:00\n\n\n\n  9:00\n\n\n\n 10:00\n\n\n\n 11:00\n\n\n\n 12:00\n\n\n\n 13:00\n\n\n\n 14:00\n\n\n\n 15:00\n\n\n\n 16:00\n\n\n\n 17:00\n\n\n\n 18:00\n\n\n\n 19:00\n\n\n\n 20:00\n\n\n\n 21:00\n\n\n\n 22:00\n\n\n\n 23:00\n");
    tft.fillRect(0, slidepage - 1 + (32 * time1), 260, 2, RED); //marker saying where you are in claendar
  */
  /*
    if (date1 == 0) {
    canvaseventload(8, EXAM,_scroll);
    canvaseventload(10, CALCULUS,_scroll);
    canvasevent2hour(12, "Composition and Rhetoric",_scroll);
    canvaseventload(14, ENG,_scroll);
    canvaseventload(15, ENG,_scroll);
    } else if (date1 == 1) {
    canvasevent2hour(10, ENG,_scroll);
    canvaseventload(12, CHEM,_scroll);
    } else if (date1 == 2) {
    canvaseventload(8, EXAM,_scroll);
    canvaseventload(10, CALCULUS,_scroll);
    canvaseventload(13, CHEM,_scroll);
    canvaseventload(16, "The Drexel Experience",_scroll);
    canvaseventload(18, EXAM,_scroll);
    } else if (date1 == 3) {
    canvaseventload(10, CALCULUS,_scroll);
    canvaseventload(12, CHEM,_scroll);
    canvasevent2hour(14, CHEM,_scroll);
    } else if (date1 == 4) {
    canvaseventload(8, EXAM,_scroll);
    canvaseventload(10, CALCULUS,_scroll);
    }
  */
}
/*
  void canvasevent2hour(float eventtime, char eventname[28],GFXcanvas1 _scroll) {
  _scroll->fillRect(40, slidepage + 4 + (32 * eventtime), 230, 60, NAVY1); //marker saying where you are in claendar
  _scroll->setTextColor(FONT); tft.setTextSize(1);
  _scroll->setCursor(45, slidepage + 5 + (32 * eventtime)); //setting cursor for text
  _scroll->println(eventname);
  }
  void canvaseventload(float eventtime, char eventname[28],GFXcanvas1 _scroll) {
  _scroll->fillRect(40, slidepage + 4 + (32 * eventtime), 230, 26, NAVY1); //marker saying where you are in claendar
  _scroll->setTextColor(FONT); tft.setTextSize(1);
  _scroll->setCursor(45, slidepage + 5 + (32 * eventtime)); //setting cursor for text
  _scroll->println(eventname);
  }
*/
void event2hour(float eventtime, char eventname[28]) {
  tft.fillRect(40, slidepage + 4 + (32 * eventtime), 230, 60, NAVY1); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(45, slidepage + 5 + (32 * eventtime)); //setting cursor for text
  tft.println(eventname);
}
void eventload(float eventtime, char eventname[28]) {
  tft.fillRect(40, slidepage + 4 + (32 * eventtime), 230, 26, NAVY1); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(45, slidepage + 5 + (32 * eventtime)); //setting cursor for text
  tft.println(eventname);
}
void memolayout() {
  tft.fillScreen(BLACK);
  tft.fillRoundRect(0, 10, 320, 480, 30, WHITE); //cool bg
  tft.drawFastHLine(0, 40, 320, RED);
  tft.drawFastHLine(0, 70, 320, RED);
  tft.drawFastHLine(0, 100, 320, RED);
  tft.drawFastHLine(0, 130, 320, RED);
  tft.drawFastHLine(0, 160, 320, RED);
  tft.drawFastHLine(0, 190, 320, RED);
  tft.drawFastHLine(0, 220, 320, RED);
  tft.drawFastHLine(0, 250, 320, RED);
  tft.drawFastHLine(0, 280, 320, RED);
  drawpastmemo();
  tft.fillRoundRect(0, 290, 320, 250, 30, NAVY3); //cool bg
  tft.fillRoundRect(15, 310, 90, 140, 15, FONT); //cool bg
  tft.fillRoundRect(115, 310, 90, 140, 15, FONT); //cool bg
  tft.fillRoundRect(215, 310, 90, 140, 15, FONT); //cool bg
  tft.fillRoundRect(0, 460, 320, 80, 15, NAVY1); //cool bg
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.setCursor(65, 475); //set cursor
  tft.println("SAVE   LOAD   DELETE");
}
void drawpastmemo() {
  i2 = 1;
  i = 4;
  while (i2 <= 64) {
    while (i <= 48) {//198
      i3 = characterlist[i2][i];
      i4 = characterlist[i2][i + 1];
      i3past = characterlist[i2][i-2];
      i4past = characterlist[i2][i - 1];
      if (i3 !=0){
        if (i2 <=15){
          tft.fillCircle(i3 / 4 + (i2 * 17) - 10, i4 / 4-70, 1, BLACK);
          //tft.drawLine(i3/4 +(i2*17)-10  , i4/4,  i3past/4 +(i2*17)-10  , i4past/4,GREEN);
        }else if (i2 <=30){
          tft.fillCircle(i3 / 4 + ((i2-15) * 17) - 10, i4 / 4-(70+(i2*10)), 1, BLACK);
        }
      }

      Serial.print("i2-");
      Serial.print(i2);
      Serial.print("| i-");
      Serial.print(i);
      Serial.print("| 0-");
      Serial.print(characterlist[i2][i]);
      Serial.print("| 1-");
      Serial.println(characterlist[i2][i + 1]);
      //Serial.println("-");
      i = i + 2;
    }
    i2++;
    i = 4;
  }
}
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0) ;
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
  long result = (high << 8) | low;
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
void setup() {
  Serial.begin(115200);
  esp8266.begin(9600);
  Serial.println(data);
  if (data == 200) {
    countup = 1;
  } else if (data != 200) {
    if (countup == 1) {
      weatherdigit = data;
      countup++;
      Serial.print("The weather is");
      Serial.println(weatherdigit);
    } else if (countup == 2) {
      time1 = data;
      countup++;
      Serial.print("The weekday is ");
      Serial.println(date1);
    } else if (countup == 3) {
      time1 = data;
      countup++;
      Serial.print("The hour is ");
      Serial.println(time1);
    } else if (countup == 4) {
      time2 = data;
      countup = 0;
      Serial.print("The min is ");
      Serial.println(time2);
      time22 = time2 / 10; //X0 hand
      time21 = time2 % 10; //0X hand
    }
  }
  Serial.println("Gi-OS v4 Started");
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.setRotation(0);
  tft.invertDisplay(1);
  tft.fillScreen(NAVY1);
  tft.fillRect(240, 0, 80, 500, NAVY3); //sidebar
  homelayout();
  statusbar();
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
      if ((0 < p.y && p.y < 150) || homepasty2 != 0) { //check if in top of Screen
        if ((0 < homepasty && homepasty < 150)) { //check if in a few secs you still there
          tft.fillRect(0, 0, 320, 10, GREEN); //say you good to go
          if ((150 < homepasty2 && homepasty2 < 480)) { //check if in a few secs you moved down
            homepasty = 0;
            homepasty2 = 0;
            Serial.print("going home...");
            cover();
          } else if ((0 < homepasty2 && homepasty2 < 150)) { //check if you havent moved
            homepasty = 0;
            homepasty2 = 0;
            statusbar();
            //tft.fillRect(0, 472, 320, 8, RED); //draw bg as scroll
            delay(1000);
          } else {
            delay(600);
            TSPoint p = ts.getPoint();//get touch point
            p.x = p.x + p.y;
            p.y = p.x - p.y;
            p.x = p.x - p.y;

            p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
            p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
            homepasty2 = p.y; //set pastx to p.x to compare
          }
        } else {
          delay(300);
          TSPoint p = ts.getPoint();//get touch point
          p.x = p.x + p.y;
          p.y = p.x - p.y;
          p.x = p.x - p.y;

          p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
          p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
          homepasty = p.y; //set pastx to p.x to compare
        }
      } else if ((0 < homepasty && homepasty < 150 && homepasty2 <= 150)) { //if you arent there anymore set pastx to 0
        homepasty = 0;
        homepasty2 = 0;
        statusbar();
      }
    }
    if (page == 0) { //home page
      if ((256 < p.x && p.x < 320)) {//check in bar
        if ((130 < p.y && p.y < 190)) {//calendar!
          page = 1;
          calendarlayout();
          statusbar();
        }
        if ((355 < p.y && p.y < 410)) {//memo!
          page = 4;
          memolayout();
          statusbar();
        }
      }
    } else if (page == 1) { //calendar
      if ((0 < p.x && p.x < 200)) { //check if in x
        if (p.y != 0) {
          px = p.y;
          calendarscrollgetto();
          //tft.vertScroll(10, 480, 480-p.x/2);

          //scrollcalendar();
        }

        //calendarlayout();
      }
    } else if (page == 2) { //calculator

    } else if (page == 3) { //weather

    } else if (page == 4) { //memo
      /*
        if (p.x != memopastx && p.y != memopasty){

        while(ticksec>ticksecpastsmall&&ticksec<ticksecpastlarge){
          Serial.print("works!");
          ticksec++;
          TSPoint p = ts.getPoint();//get touch point
          tft.fillCircle(360-p.y/2.4, p.x/2, 6, BLACK);
        }
        Serial.print("fuck!!");

        }
      */



      //ark[letter] =  0;
      if (p.y>characterlist[letter][stroke+1] && p.y-characterlist[letter][stroke+1] >4 || p.y<characterlist[letter][stroke+1] && characterlist[letter][stroke+1]-p.y >4|| p.x>characterlist[letter][stroke] && p.x-characterlist[letter][stroke] >4 || p.x<characterlist[letter][stroke] && characterlist[letter][stroke]-p.x >4){
        i5 = 0;

        if (p.x < 107 && p.x > 16) { //check which box it is in
          characterlist[letter][stroke] =  p.x-16;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        } else if (p.x < 215 && p.x > 115) { //check which box it is in
          characterlist[letter][stroke] =  p.x-115;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        } else if (p.x < 320 && p.x > 215) { //check which box it is in
          characterlist[letter][stroke] =  p.x-215;
          characterlist[letter][stroke + 1] =  p.y;
          stroke = stroke + 2;
          Serial.println("WROTE!");
        }

      }

      /*
      if (i5 == 2) {
        i5 = 0;
        characterlist[letter][stroke] =  p.x;
        characterlist[letter][stroke + 1] =  p.y;
        stroke = stroke + 2;
        Serial.println("WROTE!");
      }
      i5++;
      */
      /*
        Serial.print(" letter-");
        Serial.print(letter);
        Serial.print(" stroke-");
        Serial.print(stroke);
        Serial.print(" 0-");
        Serial.print(characterlist[letter][stroke]);
        Serial.print(" 1-");
        Serial.println(characterlist[letter][stroke+1]);
      */
      //Serial.println(ark[letter][stroke]);
      //Serial.println(ark[letter]);

      //drawpastmemo();
      if (p.x < 107 && p.x > 16) { //check which box it is in
        tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write

        //tft.fillCircle(p.x / 4 + (i6 * 17) - 10, p.y / 4+(i7*10)-50, 1, BLACK);
        pastbox = curbox;
        curbox = 1;
      } else if (p.x < 215 && p.x > 115) { //check which box it is in
        tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
        //tft.fillCircle(p.x / 4 + (i6 * 17) - 10, p.y / 4+(i7*10)-50, 1, BLACK);
        pastbox = curbox;
        curbox = 2;
      } else if (p.x < 320 && p.x > 215) { //check which box it is in
        tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
        //tft.fillCircle(p.x / 4 + (i6 * 17) - 10, p.y / 4+(i7*10)-50, 1, BLACK);
        pastbox = curbox;
        curbox = 3;
      }



      if (pastbox != curbox) {
        if (pastbox == 0 && curbox == 1 || pastbox == 1 && curbox == 2 || pastbox == 2 && curbox == 3 || pastbox == 3 && curbox == 1) { //next letter
          letter++;
          stroke = 0;
          if (p.x < 107 && p.x > 16) { //check which box it is in
            tft.fillRoundRect(215, 310, 90, 140, 15, FONT); //cool bg

          } else if (p.x < 215 && p.x > 115) { //check which box it is in
            tft.fillRoundRect(15, 310, 90, 140, 15, FONT); //cool bg
          } else if (p.x < 320 && p.x > 215) { //check which box it is in
            tft.fillRoundRect(115, 310, 90, 140, 15, FONT); //cool bg


          }
        } else if (pastbox == 1 && curbox == 3 || pastbox == 2 && curbox == 1 || pastbox == 3 && curbox == 2) { //jump a letter (space!)
          letter = letter + 2;
          stroke = 0;
          if (p.x < 107 && p.x > 16) { //check which box it is in

            tft.fillRoundRect(115, 310, 90, 140, 15, FONT); //cool bg
          } else if (p.x < 215 && p.x > 115) { //check which box it is in
            tft.fillRoundRect(215, 310, 90, 140, 15, FONT); //cool bg

          } else if (p.x < 320 && p.x > 215) { //check which box it is in
            tft.fillRoundRect(15, 310, 90, 140, 15, FONT); //cool bg
          }
        }
      }

      //tft.fillCircle(p.x/3, p.y/3,2,BLACK);
      //tft.fillCircle(380-p.y/2.4, p.x/2,4,BLACK);
      //if (p.y<870 && p.y>670){

      //}

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
    statusbar();
    if (page == 0) {
      GFXcanvas1 *_canvas;
      _canvas = new GFXcanvas1(76, 128);
      _canvas->fillScreen(WHITE);
      _canvas->setFont(&FreeSans18pt7b);
      _canvas->setTextColor(BLACK);
      _canvas->setTextSize(2);
      _canvas->setCursor(0, 54);
      _canvas->println(time1);
      _canvas->setCursor(0, 119);
      _canvas->print(time21);
      _canvas->print(time22);
      tft.drawBitmap(82, 176, _canvas->getBuffer(), 76, 128, NAVY2, FONT);
      delete _canvas;
    }
  }
  delay(1);
  ticksec++;
  ticksecpastlarge = ticksecpastlarge = ticksec + 500;
  ticksecpastsmall = ticksecpastsmall = ticksec - 500;
  ticksecpast = ticksec;
}
