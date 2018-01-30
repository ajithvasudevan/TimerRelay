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

const byte TIMER_MODE = 1; 
const byte MANUAL_MODE = 2;
byte MODE1 = TIMER_MODE;
byte MODE2 = TIMER_MODE;
byte STATE1 = 0;
byte STATE2 = 0;

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
  server.send(200, "text/html",
    "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Timer Relay</title>"
    "<script>"
    "var server = '192.168.4.1';"
    "var xhttp = new XMLHttpRequest();"
    "xhttp.onreadystatechange = function() {"
    "    if (this.readyState == 4 && this.status == 200) {"
    "       var resp = xhttp.responseText.split(',');"
    "       document.getElementById('mode1').innerHTML = resp[0];"
    "       document.getElementById('mode2').innerHTML = resp[1];"
    "       if(resp[0] == 'TIMER') document.getElementsByName('m1')[0].checked = true;"
    "       else document.getElementsByName('m1')[1].checked = true;"
    "       if(resp[1] == 'TIMER') document.getElementsByName('m2')[0].checked = true;"
    "       else document.getElementsByName('m2')[1].checked = true;"
    "       if(resp[2] == '0') document.getElementsByName('r1')[0].checked = true;"
    "       else document.getElementsByName('r1')[1].checked = true;"
    "       if(resp[3] == '0') document.getElementsByName('r2')[0].checked = true;"
    "       else document.getElementsByName('r2')[1].checked = true;"
    "       document.getElementById('s1').value = pad(resp[4], 2) + ':' + pad(resp[5], 2);"    
    "       document.getElementById('e1').value = pad(resp[6], 2) + ':' + pad(resp[7], 2);"    
    "       document.getElementById('s2').value = pad(resp[8], 2) + ':' + pad(resp[9], 2);"    
    "       document.getElementById('e2').value = pad(resp[10], 2) + ':' + pad(resp[11], 2);"    
    "       document.getElementById('ns1').innerHTML = '&nbsp;&nbsp;' + pad(resp[12], 2);"
    "       document.getElementById('ne1').innerHTML = '&nbsp;&nbsp;' + pad(resp[13], 2);"
    "       document.getElementById('ns2').innerHTML = '&nbsp;&nbsp;' + pad(resp[14], 2);"
    "       document.getElementById('ne2').innerHTML = '&nbsp;&nbsp;' + pad(resp[15], 2);"
    "       document.getElementById('dtl').value = resp[18] + '-' + resp[17] + '-' + resp[16] + 'T' + resp[19] + ':' + resp[20];"
    "    }"
    "  var dt = new Date(); "
    "  document.getElementById('dtl2').value = dt.getFullYear() + '-' + pad((dt.getMonth() + 1), 2) + '-' + pad(dt.getDate(), 2) + 'T' + pad(dt.getHours(), 2) + ':' + pad(dt.getMinutes(), 2);"
    "document.getElementById('dtl2').disabled = true;"
    "};"
    "function manual(chk) {"
    "	 xhttp.open('GET', 'http://'+ server +'/manual?' + chk.name + '=' + chk.value, true);"
    "	 xhttp.send();"
    "}"
    "function setTimer(n) {"
      "var url = 'http://'+ server + '/timer?n=' + n;"
      "if(n == 1) {"
        "url += '&sh1=' + document.getElementById('s1').value.split(':')[0];"
        "url += '&sm1=' + document.getElementById('s1').value.split(':')[1];"
        "url += '&eh1=' + document.getElementById('e1').value.split(':')[0];"
        "url += '&em1=' + document.getElementById('e1').value.split(':')[1];"
      "} else {"
        "url += '&sh2=' + document.getElementById('s2').value.split(':')[0];"
        "url += '&sm2=' + document.getElementById('s2').value.split(':')[1];"
        "url += '&eh2=' + document.getElementById('e2').value.split(':')[0];"
        "url += '&em2=' + document.getElementById('e2').value.split(':')[1];"
      "}"
    "  xhttp.open('GET', url, true);"
    "	 xhttp.send();"
    "}"
    "function setMode(n, m) {"
    "  var url = 'http://'+ server + '/mode?t=' + n + '&m=' + m.value;"
    "  xhttp.open('GET', url, true);"
    "  xhttp.send();"
    "}"
    "function setDateTime() { "
      "var dt = document.getElementById('dtl').value;"
      "console.log(dt);"
      "var d = dt.split('T')[0].split('-')[2];"
      "var M = dt.split('T')[0].split('-')[1];"
      "var y = dt.split('T')[0].split('-')[0];"
      "var h = dt.split('T')[1].split(':')[0];"
      "var m = dt.split('T')[1].split(':')[1];"
      "var s = dt.split('T')[1].split(':')[2];"
      "var url = 'http://'+ server +'/t?d=' + d + '&M=' + M + '&y=' + y;"
      "url += '&h=' + h + '&m=' + m + '&s=' + s;"
      "xhttp.open('GET', url, true);"
      "xhttp.send();"      
    "} "
    "function setNow() { "
      "var dt = new Date();"
      "var url = 'http://'+ server +'/t?d=' + dt.getDate();"
      "url += '&M=' + (dt.getMonth() + 1) + '&y=' + dt.getFullYear();"
      "url += '&h=' + dt.getHours() + '&m=' + dt.getMinutes() + '&s=' + dt.getSeconds();"
      "xhttp.open('GET', url, true);"
      "xhttp.send();"      
    "} "
    "function pad(num, size) {"
        "var s = num+'';"
        "while (s.length < size) s = '0' + s;"
        "return s;"
    "} "
    "function init() {"
      "xhttp.open('GET', 'http://'+ server +'/manual', true);"
      "xhttp.send();"
    "} "
    "</script>"
    "</head><body onLoad='init()'><h3 style='color:green'>Timer Relay</h3>"
    "<table>"
    "<tr><td><B>Current Time</B></td><td> : <input type='datetime-local' name='dtl2' id='dtl2'></td></tr>"
    "<tr><td><B>Relay Time</B></td><td> : <input type='datetime-local' name='dtl' id='dtl' onChange='setDateTime();'></td></tr>"
    "<tr><td></td><td><input type='button' value='Set to Current Date and Time' onClick='setNow()'></td></tr>"
    "</table><BR>"
    "<HR>"

    "<table border='0'>"
    "<tr><td><B>Relay 1 </B></td><td>: <input type='radio' name='r1' value='0' checked onclick='manual(this);'> OFF  &nbsp;</td><td><input type='radio' name='r1' value='1' onclick='manual(this);'> ON </td></tr>"
    "<tr><td><B>Mode </B></td><td>: <input type='radio' name='m1' value='1' checked onclick='setMode(1, this);'> TIMER  &nbsp;</td><td><input type='radio' name='m1' value='2' onclick='setMode(1, this);'> MANUAL </td></tr>"
    "</table>"
    "<table  border='0'>"
    "<tr><td colspan='2' id='mode1' style='color:red;font-weight:bold'>TIMER</td><td><B>&nbsp;&nbsp;Next start/end</B></td></tr>"
    "<tr><td><B>Start</B> </td><td>: <input type='time' name='s1' id='s1' onChange='setTimer(1)'></td><td id='ns1'>nextStart1</td></tr>"
    "<tr><td><B>End</B> </td><td>: <input type='time' name='e1' id='e1' onChange='setTimer(1)'></td><td id='ne1'>nextEnd1</td></tr>"
    "</table>"
    "<BR><HR>"
    "<table border='0'>"
    "<tr><td><B>Relay 2 </B></td><td>: <input type='radio' name='r2' value='0' checked onclick='manual(this);'> OFF  &nbsp;</td><td><input type='radio' name='r2' value='1' onclick='manual(this);'> ON </td></tr>"
    "<tr><td><B>Mode </B></td><td>: <input type='radio' name='m2' value='1' checked onclick='setMode(2, this);'> TIMER  &nbsp;</td><td><input type='radio' name='m2' value='2' onclick='setMode(2, this);'> MANUAL </td></tr>"
    "</table>"
    "<table  border='0'>"
    "<tr><td colspan='2' id='mode2' style='color:red;font-weight:bold'>TIMER</td><td><B>&nbsp;&nbsp;Next start/end</B></td></tr>"
    "<tr><td><B>Start</B> </td><td>: <input type='time' name='s2' id='s2' onChange='setTimer(2)'></td><td id='ns2'>nextStart2</td></tr>"
    "<tr><td><B>End</B> </td><td>: <input type='time' name='e2' id='e2' onChange='setTimer(2)'></td><td id='ne2'>nextEnd2</td></tr>"
    "</table>"
    "<BR><hr>"
    "</body></html>");
}
//----------------------------------------------------------------------------
void handleManualONOFF() {
  Serial.println("In handleManualONOFF( )");
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "r1") {
      MODE1 =  MANUAL_MODE;
      EEPROM.write(10, MODE1);
      Serial.print("MODE1 set to "); Serial.println(MODE1);
      String r1 = server.arg(i);
      STATE1 = atoi(r1.c_str());
      EEPROM.write(12, STATE1);
    }
    if (server.argName(i) == "r2") {
      MODE2 =  MANUAL_MODE;
      EEPROM.write(11, MODE2);
      Serial.print("MODE2 set to "); Serial.println(MODE2);
      String r2 = server.arg(i);
      STATE2 = atoi(r2.c_str());
      EEPROM.write(13, STATE2);
    }
  }
  server.send(200, "text/html", getResp());
}
//----------------------------------------------------------------------------
void handleMode() {
  Serial.println("In handleMode( )");
  int m = 0;
  String t = "0";
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "t") {
        t = server.arg(i);
    }
    if (server.argName(i) == "m") {
        m = atoi(server.arg(i).c_str());
    }
  }
  if(t.equals("1")) MODE1 = m;
  else if(t.equals("2")) MODE2 = m;
  server.send(200, "text/html", getResp());
}
//----------------------------------------------------------------------------
void handleSetStartEndTimes() {
  int n = 0;
  Serial.println("In handleSetStartEndTimes( )"); 
  for (uint16_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "n") {
      String nStr = server.arg(i);
      n = atoi(nStr.c_str());
    }
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
  if(n == 1) {  MODE1 =  TIMER_MODE;  EEPROM.write(10, MODE1); Serial.print("MODE1 set to "); Serial.println(MODE1); }
  else { MODE2 = TIMER_MODE; EEPROM.write(11, MODE2);  Serial.print("MODE2 set to "); Serial.println(MODE2); }


  EEPROM.write(0, starthr1);
  EEPROM.write(1, startmin1);
  EEPROM.write(2, endhr1);
  EEPROM.write(3, endmin1);
  EEPROM.write(4, starthr2);
  EEPROM.write(5, startmin2);
  EEPROM.write(6, endhr2);
  EEPROM.write(7, endmin2);
  EEPROM.commit();
  calcUnixTime();
  server.send(200, "text/html", getResp());
}
//----------------------------------------------------------------------------
void handleSetTime() {
  Serial.println("In handleSetTime( )");
  int y, M, d, h, m, s=0;
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
  calcUnixTime();
  server.send(200, "text/html", getResp());
}
//----------------------------------------------------------------------------
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
String getResp() {
  DateTime now = rtc.now();
  String y = String(now.year(), DEC);
  String M = pad(String(now.month(), DEC));
  String d = pad(String(now.day(), DEC));
  String h = pad(String(now.hour(), DEC));
  String m = pad(String(now.minute(), DEC));
  String s = pad(String(now.second(), DEC));
  String currdate = d + " / " + M + " / " + y;
  String currtime = h + " : " + m + " : " + s;
  String strMode1 = "TIMER";
  String strMode2 = "TIMER";
  if(MODE1 == MANUAL_MODE) strMode1 = "MANUAL";
  if(MODE2 == MANUAL_MODE) strMode2 = "MANUAL";

  DateTime s1 = DateTime(unixstart1);
  DateTime e1 = DateTime(unixend1);
  DateTime s2 = DateTime(unixstart2);
  DateTime e2 = DateTime(unixend2);
  String nextStart1 = getDateString(s1);
  String nextEnd1 = getDateString(e1);
  String nextStart2 = getDateString(s2);
  String nextEnd2 = getDateString(e2);
  setStates();
  String resp = "";
  resp += strMode1 + "," + strMode2 + "," + String(STATE1) + "," + String(STATE2) + "," ;
  resp += String(starthr1) + "," + String(startmin1) + "," + String(endhr1) + "," + String(endmin1) + "," ;
  resp += String(starthr2) + "," + String(startmin2) + "," + String(endhr2) + "," + String(endmin2) + "," ;
  resp += nextStart1 + "," + nextEnd1 + "," + nextStart2 + "," + nextEnd2 + "," ;
  resp += d + "," + M + "," + y + "," + h + "," + m + "," + s + "," + currdate + "," + currtime;
  return resp;
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

void calcUnixTime()
{
  DateTime now = rtc.now();

  DateTime startdt1 = DateTime(now.year(), now.month(), now.day(), starthr1, startmin1);
  DateTime enddt1   = DateTime(now.year(), now.month(), now.day(), endhr1,   endmin1);

  unixstart1 = startdt1.unixtime();  
  unixend1   = enddt1.unixtime();
  if(unixend1 < unixstart1) unixend1 += (60 * 60 * 24);
  if(unixend1 <= now.unixtime()) {
    unixstart1 += (60 * 60 * 24);
    unixend1 += (60 * 60 * 24);
  }

  DateTime s1 = DateTime(unixstart1);
  DateTime e1 = DateTime(unixend1);

  Serial.print("Now : "); Serial.println(getDateString(now));
  Serial.print("Sta1: "); Serial.println(getDateString(s1));
  Serial.print("End1: "); Serial.println(getDateString(e1));

  DateTime startdt2 = DateTime(now.year(), now.month(), now.day(), starthr2, startmin2);
  DateTime enddt2   = DateTime(now.year(), now.month(), now.day(), endhr2,   endmin2);

  unixstart2 = startdt2.unixtime();  
  unixend2   = enddt2.unixtime();
  if(unixend2 < unixstart2) unixend2 += (60 * 60 * 24);
  if(unixend2 <= now.unixtime()) {
    unixstart2 += (60 * 60 * 24);
    unixend2 += (60 * 60 * 24);
  }    

  DateTime s2 = DateTime(unixstart2);
  DateTime e2 = DateTime(unixend2);

  Serial.print("Now : "); Serial.println(getDateString(now)); 
  Serial.print("Sta2: "); Serial.println(getDateString(s2));
  Serial.print("End2: "); Serial.println(getDateString(e2));

}

//----------------------------------------------------------------------------
String getDateString(DateTime d) {
  return pad(String(d.day())) + "/" + pad(String(d.month())) + "/" + String(d.year()) + "  " + pad(String(d.hour())) + ":" + pad(String(d.minute()));
}
//----------------------------------------------------------------------------
String pad(String v) {
  if(v.length()<2) return ("0" + v);
  else return v;
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
  MODE1 = EEPROM.read(10);
  MODE2 = EEPROM.read(11);
  STATE1 = EEPROM.read(12);
  STATE2 = EEPROM.read(13);
  
  calcUnixTime();
//----------------------------------------------------------------------------
  server.on("/", handleRoot);
  server.on("/t", handleSetTime);
  server.on("/manual", handleManualONOFF);
  server.on("/timer", handleSetStartEndTimes);
  server.on("/mode", handleMode);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
//----------------------------------------------------------------------------
void setStates() {
  DateTime now = rtc.now();
  uint32_t unixtime =  now.unixtime();   

  prevstate1 = currstate1;
  prevstate2 = currstate2;

  if(MODE1 == TIMER_MODE) {
    if(unixtime >= unixstart1 && unixtime < unixend1) { 
      digitalWrite( D3, HIGH);
      currstate1 = HIGH;
      STATE1 = 1; 
    }
    else { 
      digitalWrite( D3, LOW); 
      currstate1 = LOW; 
      STATE1 = 0;
    }
    EEPROM.write(12, STATE1);
    EEPROM.write(8, currstate1);
  }
  else digitalWrite( D3, STATE1);

  if(MODE2 == TIMER_MODE) {
    if(unixtime >= unixstart2 && unixtime < unixend2) { 
      digitalWrite( D4, HIGH); 
      currstate2 = HIGH;
      STATE2 = 1; 
    }
    else { 
      digitalWrite( D4, LOW); 
      currstate2 = LOW; 
      STATE2 = 0;
    }
    EEPROM.write(13, STATE2);
    EEPROM.write(9, currstate2);
  }
  else digitalWrite( D4, STATE2);

}



void loop () {
    server.handleClient();

    setStates(); 

    if(prevstate1 == HIGH && currstate1 == LOW) {
      Serial.println("Advancing unixtimes by 24 hrs for Timer1");
      unixstart1 += (60 * 60 * 24);
      unixend1   += (60 * 60 * 24);
    }
    if(prevstate2 == HIGH && currstate2 == LOW) {
      Serial.println("Advancing unixtimes by 24 hrs for Timer2");
      unixstart2 += (60 * 60 * 24);
      unixend2   += (60 * 60 * 24);
    }

    EEPROM.write(8, currstate1);
    EEPROM.write(9, currstate2);
    EEPROM.commit();

    delay(100);
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
