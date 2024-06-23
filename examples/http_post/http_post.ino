/**
 * @file http_post.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief TimerCAM HTTP Post Test
 * @version 0.1
 * @date 2023-12-28
 *
 *
 * @Hardwares: TimerCAM
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * TimerCam-arduino: https://github.com/m5stack/TimerCam-arduino
 * ArduinoHttpClient: https://github.com/arduino-libraries/ArduinoHttpClient
 */

#include "M5TimerCAM.h"
#include <WiFi.h>
#include <ArduinoHttpClient.h>

#define ssid     "ssid"
#define password "password"

#define SERVER "httpbin.org"

WiFiClient wifi;
HttpClient client = HttpClient(wifi, SERVER);

void setup() {
  Serial.begin(115200); // Set baud rate for serial communication

  TimerCAM.begin();

  if (!TimerCAM.Camera.begin()) {
    Serial.println("Camera Init Fail");
    return;
  }
  Serial.println("Camera Init Success");

  TimerCAM.Camera.sensor->set_pixformat(TimerCAM.Camera.sensor,
                                          PIXFORMAT_JPEG);
  TimerCAM.Camera.sensor->set_framesize(TimerCAM.Camera.sensor,
                                          FRAMESIZE_QVGA);

  TimerCAM.Camera.sensor->set_vflip(TimerCAM.Camera.sensor, 1);
  TimerCAM.Camera.sensor->set_hmirror(TimerCAM.Camera.sensor, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (TimerCAM.Camera.get()) {
    Serial.println("Image captured");

    // Print captured image information
    Serial.println("Image data size: " + String(TimerCAM.Camera.fb->len));
    Serial.println("Image width: " + String(TimerCAM.Camera.fb->width));
    Serial.println("Image height: " + String(TimerCAM.Camera.fb->height));

    Serial.println("making POST request");

    String contentType = "image/jpeg";

    // client.post("/post", contentType, postData);
    client.post("/post", contentType.c_str(), TimerCAM.Camera.fb->len,
                TimerCAM.Camera.fb->buf);

    // read the status code and body of the response
    int statusCode  = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("WiFi connection status: ");
    Serial.println(WiFi.status());

    Serial.print("HTTP request URL: ");
    Serial.println(SERVER);

    Serial.print("HTTP request content length: ");
    Serial.println(TimerCAM.Camera.fb->len);

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    if (client.responseBody()) {  // Call responseBody() to get the String object
      Serial.print("Response body: ");
      Serial.println(client.responseBody());
      Serial.print("Response body length: ");
      Serial.println(client.responseBody().length());  // Access length directly
    }

    Serial.println("Wait five seconds");
    TimerCAM.Camera.free();
    delay(5000);
  }
}
