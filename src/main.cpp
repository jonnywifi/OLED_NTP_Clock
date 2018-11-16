#include <Arduino.h>
#include "WiFi.h"
#include <WiFiUdp.h>
#include <NTP.h>
#include <U8g2lib.h>


//char ssid[] = "RedHatN";
//char password[] = "HeatherDriveN";
//char ssid[] = "AC-T50";
//char password[] = "TeamH!r3familyT50";

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 4, /* data=*/ 5);   // Lolin32 with integrated OLED
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 15, /* data=*/ 4);   // Smaller ESP32 with integrated OLED
WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

void display_text(char *text) {
 u8g2.firstPage();
   do {
     //u8g2.clearBuffer();					// clear the internal memory
     u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
     u8g2.drawStr(0,20,text);	// write something to the internal memory
     //u8g2.sendBuffer();					// transfer internal memory to the display
     } while ( u8g2.nextPage() );
//  delay(1000);  
}


void setup() {
  Serial.begin(9600);
  u8g2.begin();  
  u8g2.setDisplayRotation(U8G2_R0);
  u8g2.setFlipMode(1);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    display_text("Connecting ...");
    delay (500);
    }
  display_text("Connected");
  delay (500);
  ntp.ruleDST("EDT", Second, Sun, Mar, 2, -240); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
  ntp.ruleSTD("EST", First, Sun, Nov, 2, -300); // last sunday in october 3:00, timezone +60min (+1 GMT)
  ntp.begin();
  display_text("Start NTP");
  delay (500);
  }

bool tick = false;
uint8_t lastSeconds = 0;
uint8_t xPos = 0;

void loop() {
  ntp.update();
  u8g2.firstPage();
  xPos = 126 * ntp.seconds() / 59;
   do {
        if (tick == true ) {
          u8g2.drawBox(xPos - 5, 0, 5, 3);
        }
        else {
          u8g2.drawBox(126 - xPos, 0, 5, 3);
        }
        u8g2.setFont(u8g2_font_fur35_tn);
        u8g2.setFontPosCenter();
        u8g2.drawStr(0,32,ntp.formattedTime("%l:%M"));
        u8g2.setFont(u8g2_font_pcsenior_8r);
        u8g2.setFontPosBaseline();
        u8g2.drawStr(64 - u8g2.getStrWidth(ntp.formattedTime("%A, %b %d")) / 2,61,ntp.formattedTime("%A, %b %d"));
     } while ( u8g2.nextPage() );
  delay(1000);
  if (( ntp.seconds() == 0 ) && (lastSeconds = 59 )) tick = !tick;
  lastSeconds = ntp.seconds();
  }
