#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
// Ahora cambia, es un esp8266
// entrada d5 y d6
int input1 = D1;
int input2 = D2;

const char *ssid = "unoalocho";
const char *password = "12345678";
const unsigned long printInterval = 500;
unsigned long lastPrint = 0;
int steps = 0;
bool lastA0 = false;
bool lastA1 = false;

bool direction = true;
bool counted = false;
os_timer_t myTimer;
const int DELAY_MS = 1000;

// Callback function for the timer

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  Serial.print("Received from client: ");
  if (type == WStype_TEXT)
  {
    Serial.println((char *)payload);
    String message = (char *)payload;
    String mtype = message.substring(0, 3);
    float value = atof(message.substring(4).c_str());
    if (mtype == "set")
    {
      steps = value;
    }
  }
}

WebSocketsServer webSocket = WebSocketsServer(81);
void emitStepsWebsocket()
{
  String stepsString = String(steps);
  webSocket.broadcastTXT(stepsString);
}
void timerCallback(void *pArg)
{
  emitStepsWebsocket();
}

void setup()
{
  Serial.begin(9600);

  pinMode(input1, INPUT);
  pinMode(input2, INPUT);
  /* Conectar a wifi */
  WiFi.begin(ssid, password);
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  while (WiFi.status() != WL_CONNECTED)
  {

    Serial.print(".");
    delay(500);
  }
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, DELAY_MS, true /* repeat */);
}

void loop()
{
  webSocket.loop();
  int a0 = digitalRead(input1);
  int a1 = digitalRead(input2);
  // Direction
  if (lastA1 && !lastA0)
  {
    direction = true;
    counted = false;
  }
  if (lastA0 && !lastA1)
  {
    direction = false;
    counted = false;
  }
  // Count when both are +555
  if (lastA0 && lastA1)
  {
    if (counted == false)
    {
      counted = true;
      if (direction)
      {
        steps++;
      }
      else
      {
        steps--;
      }
    }
  }
  // States
  if (a0 == 1)
  {
    lastA0 = true;
  }
  else
  {
    lastA0 = false;
  }
  if (a1 == 1)
  {
    lastA1 = true;
  }
  else
  {
    lastA1 = false;
  }
}