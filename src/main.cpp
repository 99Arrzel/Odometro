#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
const char *ssid = "unoalocho";
const char *password = "12345678";
#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 26
#define ROTARY_ENCODER_BUTTON_PIN 21
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 1
// instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
WebSocketsServer webSocket = WebSocketsServer(81);
void rotary_onButtonClick()
{
  static unsigned long lastTimePressed = 0; // Soft debouncing
  if (millis() - lastTimePressed < 500)
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    String message = (char *)payload;
    String mtype = message.substring(0, 3);
    float value = atof(message.substring(4).c_str());
    if (mtype == "set")
    {
      rotaryEncoder.setEncoderValue(value);
    }
  }
}
void emitStepsWebsocket(int steps)
{
  String stepsString = String(steps);
  webSocket.broadcastTXT(stepsString);
}
void rotary_loop()
{
  // dont print anything unless value changed
  if (rotaryEncoder.encoderChanged())
  {
    emitStepsWebsocket(rotaryEncoder.readEncoder());
  }
  if (rotaryEncoder.isEncoderButtonClicked())
  {
    rotary_onButtonClick();
  }
}
void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}
void setup()
{
  Serial.begin(115200);
  // we must initialize rotary encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  // set boundaries and if values should cycle or not
  // in this example we will set possible values between 0 and 1000;
  bool circleValues = true;
  rotaryEncoder.setBoundaries(INT32_MIN, INT32_MAX, circleValues); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  rotaryEncoder.disableAcceleration(); // acceleration is now enabled by default - disable if you dont need it
  // rotaryEncoder.setAcceleration(250); // or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  webSocket.begin();
}
void loop()
{
  // in loop call your custom function which will process rotary encoder values
  rotary_loop();
  webSocket.loop();
}