#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
// Ahora cambia, es un esp8266
// entrada D32 y D33
int input1 = 12;
int input2 = 13;

const char *ssid = "unoalocho";
const char *password = "12345678";
const unsigned long printInterval = 500;
unsigned long lastPrint = 0;
int steps = 0;
bool lastA0 = false;
bool lastA1 = false;

bool direction = true;
bool counted = false;
// os_timer_t myTimer;
hw_timer_t *timer = NULL;
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

void IRAM_ATTR onTimer()
{
  emitStepsWebsocket();
}

void setup()
{
  Serial.begin(9600);

  pinMode(input1, INPUT);
  pinMode(input2, INPUT);
  /* Conectar a wifi */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  while (WiFi.status() != WL_CONNECTED)
  {

    Serial.print(".");
    delay(500);
  }
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, DELAY_MS * 1000, true);
  timerAlarmEnable(timer);
}

void loop()
{
  webSocket.loop();
  int a0 = digitalRead(input1);
  int a1 = digitalRead(input2);
  // Direction
  if (lastA1 && !lastA0) // 0 1
  {
    direction = true;
  }
  if (lastA0 && !lastA1) // 1 0
  {
    direction = false;
  }
  if (!lastA0 && !lastA1) // 0 0 (Paso por un puro blanco)
  {
    counted = false;
  }
  // Count when both are +555
  if (lastA0 && lastA1) // 1 1 (Paso por un puro negro)
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