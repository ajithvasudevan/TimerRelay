#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

//----------------------------------------------------------------------------
const char *ssid = "TIMER_RELAY";
const char *password = "timerrelay";
ESP8266WebServer server ( 80 );
//----------------------------------------------------------------------------

RTC_DS3231 rtc;
 
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int starthr1 =  18;
int startmin1 = 0;
int endhr1 = 6;
int endmin1 = 0;

int starthr2 =  18;
int startmin2 = 0;
int endhr2 = 6;
int endmin2 = 0;

uint32_t unixstart1, unixend1, unixstart2, unixend2;
int currstate1 = 0, prevstate1 = 0, currstate2 = 0, prevstate2 = 0;

//----------------------------------------------------------------------------
void handleRoot() {
  DateTime now = rtc.now();
  String y = String(now.year(), DEC);
  String M = pad(String(now.month(), DEC));
  String d = pad(String(now.day(), DEC));
  String h = pad(String(now.hour(), DEC));
  String m = pad(String(now.minute(), DEC));
  String s = pad(String(now.second(), DEC));
  String currdate = d + " / " + M + " / " + y;
  String currtime = h + " : " + m + " : " + s;
  DateTime s1 = DateTime(unixstart1);
  DateTime e1 = DateTime(unixend1);
  DateTime s2 = DateTime(unixstart2);
  DateTime e2 = DateTime(unixend2);
  String nextStart1 = getTimerDate(s1);
  String nextEnd1 = getTimerDate(e1);
  String nextStart2 = getTimerDate(s2);
  String nextEnd2 = getTimerDate(e2);
  
  server.send(200, "text/html",
    "<html><head><title>Timer Relay</title>"
    "<script>"
    "function setNow() { "
      "var dt = new Date(); "
      "document.forms[0].y.value = dt.getFullYear(); "
      "document.forms[0].M.value = (dt.getMonth() + 1); "
      "document.forms[0].d.value = dt.getDate(); "
      "document.forms[0].h.value = dt.getHours(); "
      "document.forms[0].m.value = dt.getMinutes(); "
      "document.forms[0].s.value = dt.getSeconds(); "
      "document.forms[0].submit(); "
    "} "
    "function pad(num, size) {"
        "var s = num+'';"
        "while (s.length < size) s = '0' + s;"
        "return s;"
    "} "
    "function init() {"
      "document.forms[0].M.value = pad(document.forms[0].M.value, 2);"
      "document.forms[0].d.value = pad(document.forms[0].d.value, 2);"
      "document.forms[0].h.value = pad(document.forms[0].h.value, 2);"
      "document.forms[0].m.value = pad(document.forms[0].m.value, 2);"
      "document.forms[0].s.value = pad(document.forms[0].s.value, 2);"
      "document.forms[1].sh1.value = pad(document.forms[1].sh1.value, 2);"
      "document.forms[1].sm1.value = pad(document.forms[1].sm1.value, 2);"
      "document.forms[1].eh1.value = pad(document.forms[1].eh1.value, 2);"
      "document.forms[1].em1.value = pad(document.forms[1].em1.value, 2);"
      "document.forms[1].sh2.value = pad(document.forms[1].sh2.value, 2);"
      "document.forms[1].sm2.value = pad(document.forms[1].sm2.value, 2);"
      "document.forms[1].eh2.value = pad(document.forms[1].eh2.value, 2);"
      "document.forms[1].em2.value = pad(document.forms[1].em2.value, 2);"
    "} "
    "</script>"
    "</head><body onLoad='init()'><a href='/'><h2>Timer Relay</h2></a>"
    "<pre>Unixtime:     "+ String(now.unixtime()) +"<BR>"
    "Current Date: " + currdate + "<BR>"+  "Current Time: " + currtime + "<BR></pre>"
    "<HR>"
    "<form method='POST' action='/t' enctype='multipart/form-data'>"
    "<pre>Set Date and Time: "
      "<BR>Date: <input type='number' name='d' min='1' max='31' size='2' value='"+ d +"'  placeholder='date'> / "
      "<input type='number' name='M' min='1' max='12' size='2' value='"+ M +"'  placeholder='month'> / "
      "<input type='number' name='y' min='2000' max='2165' size='2' value='"+ y +"'  placeholder='year'><BR>"
      "Time: <input type='number' name='h' min='0' max='23' size='2' value='"+ h +"'  placeholder='hr'> : "
      "<input type='number' name='m' min='0' max='59' size='2' value='"+ m +"'  placeholder='min'> : "
      "<input type='number' name='s' min='0' max='59' size='2' value='"+ s +"'  placeholder='sec'></pre>"
      " <input type='submit' value='Set Date and Time'>&nbsp;&nbsp;&nbsp;&nbsp;<input type='button' value='Set to Current Date-Time' onClick='setNow()'><BR><BR>"
    "</form>"
    "<HR>"
    "<form method='POST' action='/timer' enctype='multipart/form-data'>"
    "<pre>Set Timer1: <BR>Start: <input type='number' name='sh1' min='0' max='23' size='2' value='"+ starthr1 +"' placeholder='hr'> : "
    "<input type='number' name='sm1' min='0' max='59' size='2' value='"+ startmin1 +"' placeholder='min'>&nbsp;&nbsp;"+ nextStart1 +"<BR>"
    "End  : <input type='number' name='eh1' min='0' max='23' size='2' value='"+ endhr1 +"' placeholder='hr'> : "
    "<input type='number' name='em1' min='0' max='59' size='2' value='"+ endmin1 +"' placeholder='min'>&nbsp;&nbsp;"+ nextEnd1 +"<BR><BR>"
    "Set Timer2: <BR>Start: <input type='number' name='sh2' min='0' max='23' size='2' value='"+ starthr2 +"' placeholder='hr'> : "
    "<input type='number' name='sm2' min='0' max='59' size='2' value='"+ startmin2 +"' placeholder='min'>&nbsp;&nbsp;"+ nextStart2 +"<BR>"
    "End  : <input type='number' name='eh2' min='0' max='23' size='2' value='"+ endhr2 +"' placeholder='hr'> : "
    "<input type='number' name='em2' min='0' max='59' size='2' value='"+ endmin2 +"' placeholder='min'>&nbsp;&nbsp;"+ nextEnd2 +"</pre>"
    " <input type='submit' value='Set Timers'>"
    "</form>"
    "</body></html>");
}
//----------------------------------------------------------------------------
void handleSetStartEndTimes() {
  Serial.println("In handleSetTime( )");
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "sh1") {
      String sh1 = server.arg(i);
      starthr1 = atoi(sh1.c_str());
    }
    if (server.argName(i) == "sm1") {
      String sm1 = server.arg(i);
      startmin1 = atoi(sm1.c_str());
    }
    if (server.argName(i) == "eh1") {
      String eh1 = server.arg(i);
      endhr1 = atoi(eh1.c_str());
    }
    if (server.argName(i) == "em1") {
      String em1 = server.arg(i);
      endmin1 = atoi(em1.c_str());
    }
    if (server.argName(i) == "sh2") {
      String sh2 = server.arg(i);
      starthr2 = atoi(sh2.c_str());
    }
    if (server.argName(i) == "sm2") {
      String sm2 = server.arg(i);
      startmin2 = atoi(sm2.c_str());
    }
    if (server.argName(i) == "eh2") {
      String eh2 = server.arg(i);
      endhr2 = atoi(eh2.c_str());
    }
    if (server.argName(i) == "em2") {
      String em2 = server.arg(i);
      endmin2 = atoi(em2.c_str());
    }
  }
  
  EEPROM.write(0, starthr1);
  EEPROM.write(1, startmin1);
  EEPROM.write(2, endhr1);
  EEPROM.write(3, endmin1);
  EEPROM.write(4, starthr2);
  EEPROM.write(5, startmin2);
  EEPROM.write(6, endhr2);
  EEPROM.write(7, endmin2);
  EEPROM.commit();
  calcUnixTime(0);
  handleRoot();
}
//----------------------------------------------------------------------------
void handleSetTime() {
  Serial.println("In handleSetTime( )");
  int y, M, d, h, m, s;
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "y") {
      String y1 = server.arg(i);
      y = atoi(y1.c_str());
    }
    if (server.argName(i) == "M") {
      String M1 = server.arg(i);
      M = atoi(M1.c_str());
    }
    if (server.argName(i) == "d") {
      String d1 = server.arg(i);
      d = atoi(d1.c_str());
    }
    if (server.argName(i) == "h") {
      String h1 = server.arg(i);
      h = atoi(h1.c_str());
    }
    if (server.argName(i) == "m") {
      String m1 = server.arg(i);
      m = atoi(m1.c_str());
    }
    if (server.argName(i) == "s") {
      String s1 = server.arg(i);
      s = atoi(s1.c_str());
    }
    // Year, month, date, hour, min, sec
    rtc.adjust(DateTime(y, M, d, h, m, s));
  }
  calcUnixTime(0);
  handleRoot();
}
//----------------------------------------------------------------------------
/*

// Sample code for tokenizing a String

      String TIME = server.arg(i);
      Serial.print("New Start Time1 received: "); Serial.println(TIME);
      char buf[TIME.length()+1];
      TIME.toCharArray(buf, TIME.length()+1);
      char *p = buf;
      char *str;
      int y,M,d,h,m,s = 0, j = 0;
      while ((str = strtok_r(p, ",", &p)) != NULL) {// delimiter is the comma
        if(j == 0) y = atoi(str);
        if(j == 1) M = atoi(str);
        if(j == 2) d = atoi(str);
        if(j == 3) h = atoi(str);
        if(j == 4) m = atoi(str);
        if(j == 5) s = atoi(str);
        j++;
      }

*/
//----------------------------------------------------------------------------
String pad(String v) {
  if(v.length()<2) return ("0" + v);
  else return v;
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}
//----------------------------------------------------------------------------
void setup_wifi() {
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if ( MDNS.begin ( "timerrelay" ) ) {
    Serial.println ( "MDNS responder started" );
  }
}

//----------------------------------------------------------------------------

void calcUnixTime(int p)
{
  DateTime now = rtc.now();

  if(p == 0 || p == 1) {
    DateTime startdt1 = DateTime(now.year(), now.month(), now.day(), starthr1, startmin1);
    DateTime enddt1   = DateTime(now.year(), now.month(), now.day(), endhr1,   endmin1);
  
    unixstart1 = startdt1.unixtime();  
    unixend1   = enddt1.unixtime();
    if(unixend1 < unixstart1) unixend1 += (60 * 60 * 24);
  
    while(now.unixtime() > unixend1 && now.unixtime() > unixstart1) {
      unixstart1 += (60 * 60 * 24);
      unixend1 += (60 * 60 * 24);
    }
    DateTime s1 = DateTime(unixstart1);
    DateTime e1 = DateTime(unixend1);
  
    Serial.print("Now : "); printDate(now); Serial.println("");
    Serial.print("Sta1: "); printDate(s1);
    Serial.print("End1: "); printDate(e1); Serial.println("");
  }

  if(p == 0 || p == 2) {
    DateTime startdt2 = DateTime(now.year(), now.month(), now.day(), starthr2, startmin2);
    DateTime enddt2   = DateTime(now.year(), now.month(), now.day(), endhr2,   endmin2);
  
    unixstart2 = startdt2.unixtime();  
    unixend2   = enddt2.unixtime();
    if(unixend2 < unixstart2) unixend2 += (60 * 60 * 24);
  
    while(now.unixtime() > unixend2) {
      unixstart2 += (60 * 60 * 24);
      unixend2 += (60 * 60 * 24);
    }
  
    DateTime s2 = DateTime(unixstart2);
    DateTime e2 = DateTime(unixend2);
  
    Serial.print("Now : "); printDate(now); Serial.println("");
    Serial.print("Sta2: "); printDate(s2);
    Serial.print("End2: "); printDate(e2);
  }
}
//----------------------------------------------------------------------------
void printDate(DateTime d) {
  Serial.print(d.year()); Serial.print("/"); Serial.print(d.month()); Serial.print("/");
  Serial.print(d.day()); Serial.print("  "); Serial.print(d.hour()); Serial.print(":");
  Serial.print(d.minute()); Serial.print(":"); Serial.println(d.second());
}
//----------------------------------------------------------------------------
String getTimerDate(DateTime d) {
  return String(d.year()) + "/" + pad(String(d.month())) + "/" + pad(String(d.day())) + "  " + pad(String(d.hour())) + ":" + pad(String(d.minute()));
}


//----------------------------------------------------------------------------

void setup () {
 
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif
 
  Serial.begin(115200);
//----------------------------------------------------------------------------
  setup_wifi();
//----------------------------------------------------------------------------
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite( D3, LOW);
  digitalWrite( D4, LOW);
  
  delay(3000); // wait for console opening
//----------------------------------------------------------------------------
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  EEPROM.begin(512);
  starthr1 = EEPROM.read(0);
  startmin1 = EEPROM.read(1);
  endhr1 = EEPROM.read(2);
  endmin1 = EEPROM.read(3);
  starthr2 = EEPROM.read(4);
  startmin2 = EEPROM.read(5);
  endhr2 = EEPROM.read(6);
  endmin2 = EEPROM.read(7);
  currstate1 = EEPROM.read(8);
  currstate2 = EEPROM.read(9);
  
  calcUnixTime(0);
//----------------------------------------------------------------------------
  server.on("/", handleRoot);
  server.on("/t", handleSetTime);
//  server.on("/t1", handleSetTime1);
  server.on("/timer", handleSetStartEndTimes);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
//----------------------------------------------------------------------------

void loop () {
    server.handleClient();
    DateTime now = rtc.now();
    uint32_t unixtime =  now.unixtime();   
    
    
 /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
*/

/* 
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
 
  // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
 
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
 
    Serial.println();
*/
    prevstate1 = currstate1;
    if(unixtime >= unixstart1 && unixtime < unixend1) { digitalWrite( D3, HIGH); currstate1 = HIGH; }
    else { digitalWrite( D3, LOW); currstate1 = LOW; }
    if(prevstate1 == HIGH && currstate1 == LOW) {
      Serial.println("Advancing unixtimes by 24 hrs for Timer1");
      unixstart1 += (60 * 60 * 24);
      unixend1   += (60 * 60 * 24);
//        calcUnixTime(1);
    }

    prevstate2 = currstate2;
    if(unixtime >= unixstart2 && unixtime < unixend2) { digitalWrite( D4, HIGH); currstate2 = HIGH; }
    else { digitalWrite( D4, LOW); currstate2 = LOW; }
    if(prevstate2 == HIGH && currstate2 == LOW) {
      Serial.println("Advancing unixtimes by 24 hrs for Timer2");
      unixstart2 += (60 * 60 * 24);
      unixend2   += (60 * 60 * 24);
//      calcUnixTime(2);
    }

    EEPROM.write(8, currstate1);
    EEPROM.write(9, currstate2);
    EEPROM.commit();
    delay(100);
}
