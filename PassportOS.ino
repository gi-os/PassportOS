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
float versnum = 0.54;
int countup, data, time1 = 12, time2 = 35, time21, time22, time3, ticksec, date1, date2=12, date3=1, date4=2019, weatherdigit = 69,viewdate, initclock=1; //ints delivered by ESP8266
int scrollcur = 0, slidepage;
int ticksecpastsmall, ticksecpastlarge, ticksecpast;
int page = 0;
int homepasty2, homepasty, px, pxpre = 0, memopastx, memopasty;
int letter = 0, stroke = 0, curbox, pastbox, i2, i, i3, i4, i5, i6, i7, i4past, i3past;
int characterlist[16][180];
unsigned int sundaycolor,mondaycolor,tuesdaycolor,wednesdaycolor,thursdaycolor,fridaycolor,saturdaycolor;
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
  tft.print(versnum);
}
void homelayout() {
  //tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  //Serial.print("TFT Has been rebooted ");
  //tft.setRotation(0);
  tft.invertDisplay(1);

  tft.fillScreen(NAVY1);
  tft.fillRect(240, 0, 80, 480, NAVY3); //draw sidebar as scroll
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
  slidepage=80;
  viewdate = date1;
  tft.fillScreen(NAVY3);
  tft.setTextColor(FONT);
  tft.setTextSize(1);
  tft.setCursor(90, 30);
  tft.setFont(&FreeSans18pt7b);
  tft.print(date2);
  tft.print("/");
  tft.print(date3);
  tft.print("/");
  tft.print(date4);
  tft.setFont();
  drawcalendar();
}
void drawcalendar(){
  if (viewdate == 0){//monday
    sundaycolor=RED;  mondaycolor=FONT;  tuesdaycolor=NAVY1;  wednesdaycolor=NAVY1;  thursdaycolor=NAVY1;  fridaycolor=NAVY1;  saturdaycolor=RED;
  }else if (viewdate == 1){//tuesday
    sundaycolor=RED;  mondaycolor=NAVY1;  tuesdaycolor=FONT;  wednesdaycolor=NAVY1;  thursdaycolor=NAVY1;  fridaycolor=NAVY1;  saturdaycolor=RED;
  }else if (viewdate == 2){//wednesday
    sundaycolor=RED;  mondaycolor=NAVY1;  tuesdaycolor=NAVY1;  wednesdaycolor=FONT;  thursdaycolor=NAVY1;  fridaycolor=NAVY1;  saturdaycolor=RED;
  }else if (viewdate == 3){//thursday
    sundaycolor=RED;  mondaycolor=NAVY1;  tuesdaycolor=NAVY1;  wednesdaycolor=NAVY1;  thursdaycolor=FONT;  fridaycolor=NAVY1;  saturdaycolor=RED;
  }else if (viewdate == 4){//friday
    sundaycolor=RED;  mondaycolor=NAVY1;  tuesdaycolor=NAVY1;  wednesdaycolor=NAVY1;  thursdaycolor=NAVY1;  fridaycolor=FONT;  saturdaycolor=RED;
  }else if (viewdate == 5){//saturday
    sundaycolor=RED;  mondaycolor=NAVY1;  tuesdaycolor=NAVY1;  wednesdaycolor=NAVY1;  thursdaycolor=NAVY1;  fridaycolor=NAVY1;  saturdaycolor=FONT;
  }else if (viewdate == 6){//sunday
    sundaycolor=FONT;  mondaycolor=NAVY1;  tuesdaycolor=NAVY1;  wednesdaycolor=NAVY1;  thursdaycolor=NAVY1;  fridaycolor=NAVY1;  saturdaycolor=RED;
  }
  tft.fillRect(0,60, 47, 30, sundaycolor); 
  tft.fillRect(47,60, 45, 30, mondaycolor); 
  tft.fillRect(92,60, 45, 30, tuesdaycolor); 
  tft.fillRect(137,60, 45, 30, wednesdaycolor); 
  tft.fillRect(182,60, 45, 30, thursdaycolor); 
  tft.fillRect(227,60, 45, 30, fridaycolor); 
  tft.fillRect(270,60, 50, 30, saturdaycolor); 

  tft.drawLine(47,60, 47, 90, WHITE); 
  tft.drawLine(92,60, 92, 90, WHITE); 
  tft.drawLine(137,60, 137, 90, WHITE); 
  tft.drawLine(182,60, 182, 90, WHITE); 
  tft.drawLine(227,60, 227, 90, WHITE); 
  tft.drawLine(270,60, 270, 90, WHITE);
  tft.drawLine(0,60, 320, 60, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(6, 68); tft.print("SUN"); tft.setCursor(53, 68); tft.print("MON"); tft.setCursor(97, 68); tft.print("TUE"); tft.setCursor(143, 68); tft.print("WED"); tft.setCursor(187, 68); tft.print("THU"); tft.setCursor(233, 68); tft.print("FRI"); tft.setCursor(278, 68); tft.print("SAT");
  
  tft.setTextColor(NAVY3);
  tft.fillRect(0, slidepage + 10, 340, 390, FONT); 
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, slidepage + 17); //set cursor
  tft.setFont();
  tft.println("  0:00\n\n  1:00\n\n  2:00\n\n  3:00\n\n  4:00\n\n  5:00\n\n  6:00\n\n  7:00\n\n  8:00\n\n  9:00\n\n 10:00\n\n 11:00\n\n 12:00\n\n 13:00\n\n 14:00\n\n 15:00\n\n 16:00\n\n 17:00\n\n 18:00\n\n 19:00\n\n 20:00\n\n 21:00\n\n 22:00\n\n 23:00\n");
  tft.fillRect(0, slidepage - 1 + (16 * time1)+6, 340, 2, RED); //marker saying where you are in claendar
  char ENG[28] = "Intro to Engineering Design";
  char EXAM[19] = "Common Exam Period";
  char CALCULUS[23] = "Calculus and Functions";
  char CHEM[18] = "General Chemistry";
  if (viewdate == 0) {
    eventload(8, EXAM," 8");
    eventload(10, CALCULUS,"10");
    event2hour(12, "Composition and Rhetoric","12","13");
    eventload(14, ENG,"14");
    eventload(15, ENG,"15");
  } else if (viewdate == 1) {
    event2hour(10, ENG,"10","11");
    eventload(12, CHEM,"12");
  } else if (viewdate == 2) {
    eventload(8, EXAM," 8");
    eventload(10, CALCULUS,"10");
    eventload(13, CHEM,"13");
    eventload(16, "The Drexel Experience","10");
    eventload(18, EXAM,"18");
  } else if (viewdate == 3) {
    eventload(10, CALCULUS,"10");
    eventload(12, CHEM,"12");
    event2hour(14, CHEM,"14","15");
  } else if (viewdate == 4) {
    eventload(8, EXAM," 8");
    eventload(10, CALCULUS,"10");
  }
}
/*
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
*/
/*
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

  char ENG[28] = "Intro to Engineering Design";
  char EXAM[19] = "Common Exam Period";
  char CALCULUS[23] = "Calculus and Functions";
  char CHEM[18] = "General Chemistry";
  GFXcanvas1 *_scroll;

    _scroll = new GFXcanvas1(320, 480-px/2);
    _scroll->setTextColor(BLACK);
    _scroll->fillRect(0, slidepage + 10, 340, 800, FONT); 
    _scroll->setTextColor(BLACK);
    _scroll->setTextSize(1);
    _scroll->setCursor(0, slidepage + 15); //set cursor
    _scroll->println("  0:00\n\n  1:00\n\n\n\n  2:00\n\n\n\n  3:00\n\n\n\n  4:00\n\n\n\n  5:00\n\n\n\n  6:00\n\n\n\n  7:00\n\n\n\n  8:00\n\n\n\n  9:00\n\n\n\n 10:00\n\n\n\n 11:00\n\n\n\n 12:00\n\n\n\n 13:00\n\n\n\n 14:00\n\n\n\n 15:00\n\n\n\n 16:00\n\n\n\n 17:00\n\n\n\n 18:00\n\n\n\n 19:00\n\n\n\n 20:00\n\n\n\n 21:00\n\n\n\n 22:00\n\n\n\n 23:00\n");
    _scroll->fillRect(0, slidepage - 1 + (32 * time1), 260, 2, RED); //marker saying where you are in claendar
    //_scroll->print(time21);
    //_scroll->print(time22);
    tft.drawBitmap(0, 0,_scroll->getBuffer(),320, 480-px/2, FONT, NAVY3);
    delete _scroll;

  //clocklarge();
  tft.setTextColor(NAVY3);
  tft.fillRect(0, slidepage + 10, 340, 800, FONT); 
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, slidepage + 15); //set cursor
  //tft.setFont();
  tft.println("  0:00\n\n\n\n  1:00\n\n\n\n  2:00\n\n\n\n  3:00\n\n\n\n  4:00\n\n\n\n  5:00\n\n\n\n  6:00\n\n\n\n  7:00\n\n\n\n  8:00\n\n\n\n  9:00\n\n\n\n 10:00\n\n\n\n 11:00\n\n\n\n 12:00\n\n\n\n 13:00\n\n\n\n 14:00\n\n\n\n 15:00\n\n\n\n 16:00\n\n\n\n 17:00\n\n\n\n 18:00\n\n\n\n 19:00\n\n\n\n 20:00\n\n\n\n 21:00\n\n\n\n 22:00\n\n\n\n 23:00\n");
  tft.fillRect(0, slidepage - 1 + (32 * time1), 260, 2, RED); //marker saying where you are in claendar

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

  }
*/
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
void event2hour(float eventtime, char eventname[28],char eventtimeread[2],char eventtimeread2[2]) {
  tft.fillRect(40, slidepage + 4 + (16 * eventtime)+10, 230, 30, NAVY1); //marker saying where you are in claendar
  tft.fillRect(0, slidepage + 4 + (16 * eventtime)+10, 40, 30, NAVY3); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(0, slidepage + 6 + (16 * eventtime)+11); //setting cursor for text
  tft.print(" ");tft.print(eventtimeread);tft.print(":00");
  tft.setCursor(0, slidepage + 6 + (16 * (eventtime+1))+11);
  tft.print(" ");tft.print(eventtimeread2);tft.print(":00");
  tft.setCursor(45, slidepage + 16 + (16 * eventtime)+10); //setting cursor for text
  tft.println(eventname);
}
void eventload(float eventtime, char eventname[28],char eventtimeread[2]) {
  tft.fillRect(40, slidepage + 4 + (16 * eventtime)+10, 230, 14, NAVY1); //marker saying where you are in claendar
  tft.fillRect(0, slidepage + 4 + (16 * eventtime)+10, 40, 14, NAVY3); //marker saying where you are in claendar
  tft.setTextColor(FONT); tft.setTextSize(1);
  tft.setCursor(0, slidepage + 6 + (16 * eventtime)+11); //setting cursor for text
  tft.print(" ");tft.print(eventtimeread);tft.print(":00");
  tft.setCursor(45, slidepage + 6 + (16 * eventtime)+11); //setting cursor for text
  tft.println(eventname);
}
void memolayout() {
  tft.fillScreen(BLACK);
  tft.fillRoundRect(0, 10, 320, 480, 30, WHITE); 
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
  while (i2 <= 16) {
    while (i <= 198) {//198
      i3 = characterlist[i2][i];
      i4 = characterlist[i2][i + 1];
      i3past = characterlist[i2][i - 2];
      i4past = characterlist[i2][i - 1];
      if (i3 != 0) {
        if (i2 <= 15) {
          tft.fillCircle(i3 / 4 + (i2 * 17) - 10, i4 / 4, 1, BLACK);
          tft.fillCircle(i3 / 4 + (i2 * 17) - 10, i4 / 4 - 70, 1, BLACK);
          //tft.drawLine(  i3 / 4 + (i2 * 17) - 10, i4 / 4 - 70, i3past / 4 + (i2 * 17) - 10,i4past / 4 - 70,BLACK);
          //tft.drawLine(  i3 / 4 + (i2 * 17) - 10+1, i4 / 4 - 70+1, i3past / 4 + (i2 * 17) - 10+1,i4past / 4 - 70+1,BLACK);
          //tft.drawLine(  i3 / 4 + (i2 * 17) - 10-1, i4 / 4 - 70-1, i3past / 4 + (i2 * 17) - 10-1,i4past / 4 - 70-1,BLACK);
        } else if (i2 <= 30) {
          tft.fillCircle(i3 / 4 + ((i2 - 15) * 17) - 10, i4 / 4 - (70 + (i2 * 10)), 1, BLACK);
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
  /*
    FILE *f = fopen("memo1.psprt", "wb");//creates writeable file
    fwrite(characterlist, sizeof(char), sizeof(characterlist), f);// writes array to file
    fclose(f);//coses file
  */
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
  Serial.println("PassportOS Started");
  Serial.print("Version: ");
  Serial.println(versnum);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.setRotation(0);
  tft.invertDisplay(1);
  //tft.fillScreen(NAVY1);
  //tft.fillRect(240, 0, 80, 500, NAVY3); //sidebar
  
  homelayout();
  //statusbar();
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
    if ((0 < p.y && p.y < 20) || homepasty2 != 0) { //check if in top of Screen
      if ((0 < homepasty && homepasty < 20)) { //check if in a few secs you still there
        tft.fillRect(0, 0, 320, 10, GREEN); //say you good to go
        if ((20 < homepasty2 && homepasty2 < 480)) { //check if in a few secs you moved down
          homepasty = 0;
          homepasty2 = 0;
          Serial.print("going home...");
          cover();
        } else if ((0 < homepasty2 && homepasty2 < 20)) { //check if you havent moved
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
    } else if ((0 < homepasty && homepasty < 20 && homepasty2 <= 20)) { //if you arent there anymore set pastx to 0
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
      if ((60 < p.y && p.y < 100)) { 
        if ((0 < p.x && p.x < 47)) { 
          viewdate=6;
          drawcalendar();
        }else if ((47 < p.x && p.x < 92)) { 
          viewdate=0;
          drawcalendar();
        }else if ((92 < p.x && p.x < 137)) { 
          viewdate=1;
          drawcalendar();
        }else if ((137 < p.x && p.x < 182)) { 
          viewdate=2;
          drawcalendar();
        }else if ((182 < p.x && p.x < 227)) { 
          viewdate=3;
          drawcalendar();
        }else if ((227 < p.x && p.x < 270)) { 
          viewdate=4;
          drawcalendar();
        }else if ((270 < p.x && p.x < 320)) { 
          viewdate=5;
          drawcalendar();
        }
      }
    } else if (page == 2) { //calculator

    } else if (page == 3) { //weather

    } else if (page == 4) { //memo
      //ark[letter] =  0;
      /*
        FILE *ifp = fopen("memo1.psprt", "rb"); //opens file
        fread(characterlist, sizeof(char), sizeof(characterlist), ifp);//converts file to characterlist array
      */
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
        tft.fillCircle((p.x - 16) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
        pastbox = curbox;
        curbox = 1;
      } else if (p.x < 215 && p.x > 115) { //check which box it is in
        tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
        tft.fillCircle((p.x - 115) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
        pastbox = curbox;
        curbox = 2;
      } else if (p.x < 320 && p.x > 215) { //check which box it is in
        tft.fillCircle(p.x, p.y, 4, BLACK);//draw when you write
        tft.fillCircle((p.x - 215) / 4 + (i6 * 17) + (letter * 17), p.y / 4 + (i7 * 10) - 70, 1, BLACK);
        pastbox = curbox;
        curbox = 3;
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
    
    if (page == 0) {
      
      //delete _canvas;
      tft.fillRect(82,176,76,128, NAVY2);
      tft.setFont(&FreeSans18pt7b);
      tft.setCursor(82, 230); //setting cursor for text
      tft.setTextColor(FONT);
      tft.setTextSize(2);
      tft.println(time1);
      tft.setCursor(82, 295); //setting cursor for text
      tft.print(time21);
      tft.print(time22);
    }else{
      statusbar();
    }
  }
  delay(1);
  ticksec++;
  ticksecpastlarge = ticksecpastlarge = ticksec + 500;
  ticksecpastsmall = ticksecpastsmall = ticksec - 500;
  ticksecpast = ticksec;
}
