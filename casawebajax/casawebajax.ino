/*--------------------------------------------------------------
 Program:      eth_websrv_AJAX_IN

 Description:  Uses Ajax to update the state of two switches
 and an analog input on a web page. The Arduino
 web server hosts the web page.
 Does not use the SD card.

 Hardware:     Arduino Uno and official Arduino Ethernet
 shield. Should work with other Arduinos and
 compatible Ethernet shields.

 Software:     Developed using Arduino 1.0.3 software
 Should be compatible with Arduino 1.0 +

 References:   - WebServer example by David A. Mellis and
 modified by Tom Igoe
 - Ethernet library documentation:
 http://arduino.cc/en/Reference/Ethernet
 - Learning PHP, MySQL & JavaScript by
 Robin Nixon, O'Reilly publishers

 Date:         20 February 2013

 Author:       W.A. Smith, http://startingelectronics.com
 --------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include<dht11.h>
dht11 sensor;
#include <Servo.h>

Servo myservo;
int pos = 170;
float temperatura =-1;
float umidade = -1;

// MAC address from Ethernet shield sticker under board
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0, 177);
EthernetServer server(6565);

int pinIn2 = 2;
int pinIn3 = 3;
int pinOut5 = 5;
int pinOut7 = 7;
int pinOut8 = 8;
int pinOut9 = 9;
int pinOut12 = 12;

/*String r_pinIn2 = "GET /?PIN=2&STATUS=1 HTTP/1.1";
String r_pinIn3 = "GET /?PIN=3&STATUS=1 HTTP/1.1";
String r_pinOut5_high = "GET /?PIN=5&STATUS=1 HTTP/1.1";
String r_pinOut5_low = "GET /?PIN=5&STATUS=0 HTTP/1.1";
String r_pinOut7_high = "GET /?PIN=7&STATUS=1 HTTP/1.1";
String r_pinOut7_low = "GET /?PIN=7&STATUS=0 HTTP/1.1";
String r_pinOut8_high = "GET /?PIN=8&STATUS=1 HTTP/1.1";
String r_pinOut8_low = "GET /?PIN=8&STATUS=0 HTTP/1.1";
String r_pinOut9_high = "GET /?PIN=9&STATUS=1 HTTP/1.1";
String r_pinOut9_low = "GET /?PIN=9&STATUS=0 HTTP/1.1";
String r_pinOut12_high = "GET /?PIN=12&STATUS=1 HTTP/1.1";
String r_pinOut12_low = "GET /?PIN=12&STATUS=0 HTTP/1.1";
String r = "GET / HTTP/1.1";
String r_temp = "GET /?temp HTTP/1.1";
String r_janela = "GET /?janela HTTP/1.1";
*/

String HTTP_req;            // stores the HTTP request

void setup()
{
  pinMode(pinIn2, INPUT);
  pinMode(pinIn3, INPUT);
  pinMode(pinOut5, OUTPUT);
  pinMode(pinOut7, OUTPUT);
  pinMode(pinOut8, OUTPUT);
  pinMode(pinOut9, OUTPUT);


  digitalWrite(pinIn2, LOW);
  digitalWrite(pinIn3, LOW);
  digitalWrite(pinOut5, LOW);
  digitalWrite(pinOut7, LOW);
  digitalWrite(pinOut8, LOW);
  digitalWrite(pinOut9, LOW);


  myservo.attach(pinOut12);
  myservo.write(170);

  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  Serial.begin(9600);       // for diagnostics
}

void loop()
{
  EthernetClient client = server.available();  // try to get client

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        HTTP_req += c;  // save the HTTP request 1 char at a time
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: keep-alive");
          client.println();
          // AJAX request for switch state
          if(HTTP_req.indexOf("Janela") > -1) {
            janela(client);
          }
          else if(HTTP_req.indexOf("Temp") > -1) {
            temp(client);
          }
          else if(HTTP_req.indexOf("PIN=5") > -1) {
            if(digitalRead(pinOut5)){
              digitalWrite(pinOut5,LOW);
            }
            else {
              digitalWrite(pinOut5,HIGH);
            }
          }

          else if(HTTP_req.indexOf("PIN=7") > -1) {
            if(digitalRead(pinOut7)){
              digitalWrite(pinOut7,LOW);
            }
            else {
              digitalWrite(pinOut7,HIGH);
            }
          }
          else if(HTTP_req.indexOf("PIN=8") > -1) {
            if(digitalRead(pinOut8)){
              digitalWrite(pinOut8,LOW);
            }
            else {
              digitalWrite(pinOut8,HIGH);
            }
          }
          else if(HTTP_req.indexOf("PIN=9") > -1) {
            if(digitalRead(pinOut9)){
              digitalWrite(pinOut9,LOW);
            }
            else {
              digitalWrite(pinOut9,HIGH);
            }
          }
          else if(HTTP_req.indexOf("PIN=12") > -1) {
            Serial.println(pos);
            if(pos == 170){
              for(; pos>51; pos-=1)
              {
                myservo.write(pos);
                delay(15);
              }
            }
            else
              for(; pos < 170; pos += 1)
              {
                myservo.write(pos);
                delay(15);
              }
          }

          else {  // HTTP request for web page
            // send web page - contains JavaScript with AJAX calls
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>Arduino Web Page</title>");
            client.println("<script>");
            client.println("var request = new XMLHttpRequest();");

            client.println("function GetJanela() {");
            //  client.println(
            //"nocache = \"&nocache=\" + Math.random() * 1000000;");
            client.println("var request = new XMLHttpRequest();");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("document.getElementById(\"sw_an_data\").innerHTML = this.responseText;");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"Janela\", true);");
            client.println("request.send(null);");
            client.println("setTimeout('GetJanela()', 300);");
            client.println("}");

            client.println("function GetTemp() {");
            //  client.println(
            // "nocache = \"&nocache=\" + Math.random() * 1000000;");
            client.println("var request = new XMLHttpRequest();");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("document.getElementById(\"sw_an_data1\").innerHTML = this.responseText;");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"Temp\", true);");
            client.println("request.send(null);");
            client.println("setTimeout('GetTemp()', 2500);");
            client.println("}");

            client.println("function SetSala() {");
            client.println("var request = new XMLHttpRequest(); ");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"PIN=8\", true);");
            client.println("request.send(null);");
            client.println("}");


            client.println("function SetQuarto() {");
            client.println("var request = new XMLHttpRequest(); ");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"PIN=5\", true);");
            client.println("request.send(null);");
            client.println("}");

            client.println("function SetCozinha() {");
            client.println("var request = new XMLHttpRequest(); ");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"PIN=7\", true);");
            client.println("request.send(null);");
            client.println("}");

            client.println("function SetPortao() {");
            client.println("var request = new XMLHttpRequest(); ");
            client.println("request.onreadystatechange = function() {");
            client.println("if (this.readyState == 4) {");
            client.println("if (this.status == 200) {");
            client.println("if (this.responseText != null) {");
            client.println("}}}}");
            client.println("request.open(\"GET\", \"PIN=12\", true);");
            client.println("request.send(null);");
            client.println("}");

            client.println("</script>");
            client.println("</head>");
            client.println("<body onload=\"GetJanela();GetTemp()\">");
            client.println("<h1>Projeto Dom&oacutetico</h1>");

            client.println("<form action='/' method='GET'><br />");
            client.println("<input id='btn12' type='button' value=' PORTAO ' onClick='SetPortao()'><br /><br />");
            client.println("<input id='btn5' type='button' value=' QUARTO ' onClick='SetQuarto()'><br /><br />");
            client.println("<input id='btn7' type='button' value=' COZINHA ' onClick='SetCozinha()'><br /><br />");
            client.println("<input id='btn8' type='button' value=' SALA ' onClick='SetSala()'><br /><br />");
            client.println("</form>");

            client.println("<div id=\"sw_an_data\">");
            client.println("</div>");

            client.println("<div id=\"sw_an_data1\">");
            client.println("</div>");

            client.println("</body>");
            client.println("</html>");
          }
          // display received HTTP request on serial port
          Serial.print(HTTP_req);
          HTTP_req = "";            // finished with request, empty string
          break;
        }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}

// send the state of the switch to the web browser
void temp(EthernetClient c)
{
  Serial.print("Lendo sensor: ");
  int chk = sensor.read(pinIn2);
  switch(chk) {
  case DHTLIB_OK:
    Serial.println("OK");
    temperatura = (float)sensor.temperature;
    umidade = (float)sensor.humidity;
    break;
  case DHTLIB_ERROR_CHECKSUM:
    Serial.println("Erro no checksum");
    break;
  case DHTLIB_ERROR_TIMEOUT:
    Serial.println("Tempo esgotado");
    break;
  default:
    Serial.println("Erro desconhecido");
  }
  c.print("<br />Umidade: ");
  c.println((float)sensor.humidity, 2);
  c.print(" %<br />Temperatura: ");
  c.println((float)sensor.temperature, 2);
  c.print("&#176C<br />");

  Serial.print("Umidade (%): ");
  Serial.println((float)sensor.humidity, 2);
  Serial.print("Temperatura (graus Celsius): ");
  Serial.println((float)sensor.temperature, 2);

}

void janela(EthernetClient c){
  c.print("<br />");
  c.println("JANELA SALA: ");
  if(!digitalRead(pinIn3)){
    //client.print((float)1);
    c.print(" Aberta </form> ");
  }
  else {
    //client.print((float)0);
    c.print(" Fechada </form> ");
  }
}













