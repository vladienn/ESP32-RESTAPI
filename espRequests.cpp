#include <WiFi.h>
#include <HTTPClient.h>
#include <AceButton.h>
#include <Adafruit_NeoPixel.h>

using namespace ace_button;

const int BUTTON_PIN = 35;
const int LED_PIN = 33;
const int STRIP_SIZE = 16;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

AceButton button(BUTTON_PIN);
void handleEvent(AceButton*, uint8_t, uint8_t);
void restPOST(bool);
void restGET(void* par);
void btn_check(void* par);
void colorWipe(uint32_t c, uint8_t wait);


//WiFi credentials
const char* wifi_name = "*****";
const char* wifi_password = "*****";

bool tmp = true;
int led_color = 1;

void setup() {
  strip.begin();
  strip.setBrightness(25);  // Lower brightness and save eyeballs!
  strip.show();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);


  //WiFi
  Serial.begin(115200);
  WiFi.begin(wifi_name, wifi_password);
  //  while(WiFi.status() != WL_CONNECTED){
  //      delay(500);
  //      Serial.println("Connnecting...");
  //    }
  //  Serial.println("Connected to ");
  //  Serial.print(wifi_name);

  xTaskCreate(
    restGET,
    "restGET",
    10000,
    NULL,
    1,
    NULL);

  xTaskCreate(
    btn_check,
    "btn_check",
    10000,
    NULL,
    1,
    NULL);

}

void loop() {
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void btn_check(void* par) {
  while (true) {
    button.check();
  }
}

void restGET(void* par) {
  while (true) {
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

      HTTPClient http;

      http.begin("http://78.90.138.71:5001/"); //Specify the URL

      int httpCode = http.GET();                                        //Make the request

      if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();

        led_color = int(payload[16]) - '0';

        switch (led_color) {
          case 0:
            colorWipe(strip.Color(0, 0, 0), 20); //off
            tmp = true;
            break;
          case 1:
            colorWipe(strip.Color(255, 0, 0), 20); //red
            tmp = false;
            break;
          case 2:
            colorWipe(strip.Color(255, 0, 128), 20); //pink
            break;
          case 3:
            colorWipe(strip.Color(128, 0, 255), 20); //purple
            break;
          case 4:
            colorWipe(strip.Color(250, 255, 0), 20); //yellow
            break;
          case 5:
            colorWipe(strip.Color(255, 128, 0), 20); //orange
            break;
          case 6:
            colorWipe(strip.Color(0, 255, 0), 20); //green
            break;
          case 7:
            colorWipe(strip.Color(255, 255, 255), 20); //white
            break;
          case 8:
            colorWipe(strip.Color(20, 255, 255), 20); //cyan
            break;
          case 9:
            colorWipe(strip.Color(0, 0, 255), 20); //blue
            break;
        }

        led_color++;
        if (led_color == 10) {
          led_color = 1;
        }
      }
      http.end(); //Free the resources
    }
    vTaskDelay(1000);
  }
}


void restPOST(bool light_state) {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("http://78.90.138.71:5001/"); //Specify the URL
    http.addHeader("Content-Type", "text/plain");

    String text_state = "{\"light_state\":\"";

    if (light_state == false) {
      if (tmp == true) {
        text_state += "1\"}";
        tmp = false;
      } else  {
        text_state += "0\"}";
        tmp = true;
      }
    } else if (light_state == true && tmp == false) {
      text_state += led_color;
      text_state += "\"}";
    } else text_state = "{\"light_state\":\"0\"}";

    int httpCode = http.POST(text_state);

    http.end(); //Free the resources
  }
}

void handleEvent(AceButton* /* button */, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      restPOST(true);
      break;
    case AceButton::kEventDoubleClicked:
      restPOST(false);
      break;

  }
}
