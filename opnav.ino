#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <U8g2lib.h>

const char* ssid = "Garv's Pixel";
const char* password = "asustufa15";

WiFiServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

String directions;
String distance;
bool directionsReceived = false;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(" ");
  Serial.println("Starting setup");

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB12_tr);
  u8g2.drawStr(0, 15, "Starting setup");
  u8g2.sendBuffer();

  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "Connection Successful!");
  u8g2.sendBuffer();
  
  timeClient.begin();
  timeClient.setTimeOffset(0); // Adjust time offset as per your timezone

  server.begin();
  Serial.println("Server started");

  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  timeClient.update();

  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/data/") != -1) {
    int jsonStart = request.indexOf("/data/") + 6; // Length of "/data/"
    int jsonEnd = request.indexOf(" HTTP/1.1");
    String jsonString = request.substring(jsonStart, jsonEnd);

    jsonString.replace("%22", "\"");
    jsonString.replace("%20", " ");
    jsonString.replace("%7B", "{");
    jsonString.replace("%7D", "}");

    Serial.println("JSON data:");
    Serial.println(jsonString);

    DynamicJsonDocument doc(256);
    deserializeJson(doc, jsonString);

    String packageName = doc["packageName"];
    String title = doc["title"];
    String text = doc["text"];
    Serial.println("Application Name: " + packageName);
    Serial.println("Title: " + title);
    Serial.println("Text: " + text);

    if (packageName.indexOf("maps") != -1) {
      directions = text;
      distance = "10m";
      directionsReceived = true;
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 8, directions.c_str());
      u8g2.drawStr(0, 25, distance.c_str()); // Draw distance
      u8g2.drawTriangle(105, 15, 110, 10, 115, 15); // Up arrow
        u8g2.drawLine(110, 15, 110, 20);
      if (directions.indexOf("north") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 30); // Up arrow (bigger)
        u8g2.drawLine(115, 30, 115, 40); // Up arrow (longer line)
      } else if (directions.indexOf("south") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 10); // Down arrow (bigger)
        u8g2.drawLine(115, 10, 115, 0); // Down arrow (longer line)
      } else if (directions.indexOf("east") != -1) {
        u8g2.drawTriangle(125, 25, 115, 15, 115, 35); // Right arrow (bigger)
        u8g2.drawLine(115, 25, 105, 25); // Right arrow (longer line)
      } else if (directions.indexOf("west") != -1) {
        u8g2.drawTriangle(105, 25, 115, 15, 115, 35); // Left arrow (bigger)
        u8g2.drawLine(115, 25, 125, 25); // Left arrow (longer line)
      } else if (directions.indexOf("northwest") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 30); // Northwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 30); // Northwest diagonal arrow (longer line)
      } else if (directions.indexOf("northeast") != -1) {
        u8g2.drawTriangle(125, 10, 115, 20, 105, 30); // Northeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 30); // Northeast diagonal arrow (longer line)
      } else if (directions.indexOf("southwest") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 10); // Southwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 10); // Southwest diagonal arrow (longer line)
      } else if (directions.indexOf("southeast") != -1) {
        u8g2.drawTriangle(125, 30, 115, 20, 105, 10); // Southeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 10); // Southeast diagonal arrow (longer line)
      }
      u8g2.sendBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 50, "Current Time:");
      u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
      u8g2.sendBuffer();
    } else {
      if (!directionsReceived) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 15, "Navigation is off");
        u8g2.sendBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 50, "Current Time:");
        u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
        u8g2.sendBuffer();
      }
    }

    if (packageName.indexOf("whatsapp") != -1 && text.indexOf("urgent") != -1) {
      u8g2.clearBuffer();
      if (directionsReceived == true) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 8, directions.c_str());
        u8g2.drawStr(0, 25, distance.c_str()); // Draw distance
        u8g2.drawTriangle(105, 15, 110, 10, 115, 15); // Up arrow
        u8g2.drawLine(110, 15, 110, 20);
        if (directions.indexOf("north") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 30); // Up arrow (bigger)
        u8g2.drawLine(115, 30, 115, 40); // Up arrow (longer line)
      } else if (directions.indexOf("south") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 10); // Down arrow (bigger)
        u8g2.drawLine(115, 10, 115, 0); // Down arrow (longer line)
      } else if (directions.indexOf("east") != -1) {
        u8g2.drawTriangle(125, 25, 115, 15, 115, 35); // Right arrow (bigger)
        u8g2.drawLine(115, 25, 105, 25); // Right arrow (longer line)
      } else if (directions.indexOf("west") != -1) {
        u8g2.drawTriangle(105, 25, 115, 15, 115, 35); // Left arrow (bigger)
        u8g2.drawLine(115, 25, 125, 25); // Left arrow (longer line)
      } else if (directions.indexOf("northwest") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 30); // Northwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 30); // Northwest diagonal arrow (longer line)
      } else if (directions.indexOf("northeast") != -1) {
        u8g2.drawTriangle(125, 10, 115, 20, 105, 30); // Northeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 30); // Northeast diagonal arrow (longer line)
      } else if (directions.indexOf("southwest") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 10); // Southwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 10); // Southwest diagonal arrow (longer line)
      } else if (directions.indexOf("southeast") != -1) {
        u8g2.drawTriangle(125, 30, 115, 20, 105, 10); // Southeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 10); // Southeast diagonal arrow (longer line)
      }
        u8g2.sendBuffer();
      } else if (!directionsReceived) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 15, "Navigation is off");
        u8g2.sendBuffer();
      }
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 50, ("Sender: " + title).c_str());
      u8g2.drawStr(0, 60, ("Message: " + text).c_str());
      u8g2.sendBuffer();
      delay(5000);
      u8g2.clearBuffer();
      if (directionsReceived == true) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 8, directions.c_str());
        u8g2.drawStr(0, 25, distance.c_str()); // Draw distance
        u8g2.drawTriangle(105, 15, 110, 10, 115, 15); // Up arrow
        u8g2.drawLine(110, 15, 110, 20);
        if (directions.indexOf("north") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 30); // Up arrow (bigger)
        u8g2.drawLine(115, 30, 115, 40); // Up arrow (longer line)
      } else if (directions.indexOf("south") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 10); // Down arrow (bigger)
        u8g2.drawLine(115, 10, 115, 0); // Down arrow (longer line)
      } else if (directions.indexOf("east") != -1) {
        u8g2.drawTriangle(125, 25, 115, 15, 115, 35); // Right arrow (bigger)
        u8g2.drawLine(115, 25, 105, 25); // Right arrow (longer line)
      } else if (directions.indexOf("west") != -1) {
        u8g2.drawTriangle(105, 25, 115, 15, 115, 35); // Left arrow (bigger)
        u8g2.drawLine(115, 25, 125, 25); // Left arrow (longer line)
      } else if (directions.indexOf("northwest") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 30); // Northwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 30); // Northwest diagonal arrow (longer line)
      } else if (directions.indexOf("northeast") != -1) {
        u8g2.drawTriangle(125, 10, 115, 20, 105, 30); // Northeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 30); // Northeast diagonal arrow (longer line)
      } else if (directions.indexOf("southwest") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 10); // Southwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 10); // Southwest diagonal arrow (longer line)
      } else if (directions.indexOf("southeast") != -1) {
        u8g2.drawTriangle(125, 30, 115, 20, 105, 10); // Southeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 10); // Southeast diagonal arrow (longer line)
      }
        u8g2.sendBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 50, "Current Time:");
        u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
        u8g2.sendBuffer();
      } else if (!directionsReceived) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 15, "Navigation is off");
        u8g2.sendBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 50, "Current Time:");
        u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
        u8g2.sendBuffer();
      }

    } else if (packageName.indexOf("dialer") != -1 && text.indexOf("Incoming call") != -1) {
      u8g2.clearBuffer();
      if (directionsReceived == true) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 8, directions.c_str());
        u8g2.drawStr(0, 25, distance.c_str()); // Draw distance
        u8g2.drawTriangle(105, 15, 110, 10, 115, 15); // Up arrow
        u8g2.drawLine(110, 15, 110, 20);
        if (directions.indexOf("north") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 30); // Up arrow (bigger)
        u8g2.drawLine(115, 30, 115, 40); // Up arrow (longer line)
      } else if (directions.indexOf("south") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 10); // Down arrow (bigger)
        u8g2.drawLine(115, 10, 115, 0); // Down arrow (longer line)
      } else if (directions.indexOf("east") != -1) {
        u8g2.drawTriangle(125, 25, 115, 15, 115, 35); // Right arrow (bigger)
        u8g2.drawLine(115, 25, 105, 25); // Right arrow (longer line)
      } else if (directions.indexOf("west") != -1) {
        u8g2.drawTriangle(105, 25, 115, 15, 115, 35); // Left arrow (bigger)
        u8g2.drawLine(115, 25, 125, 25); // Left arrow (longer line)
      } else if (directions.indexOf("northwest") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 30); // Northwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 30); // Northwest diagonal arrow (longer line)
      } else if (directions.indexOf("northeast") != -1) {
        u8g2.drawTriangle(125, 10, 115, 20, 105, 30); // Northeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 30); // Northeast diagonal arrow (longer line)
      } else if (directions.indexOf("southwest") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 10); // Southwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 10); // Southwest diagonal arrow (longer line)
      } else if (directions.indexOf("southeast") != -1) {
        u8g2.drawTriangle(125, 30, 115, 20, 105, 10); // Southeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 10); // Southeast diagonal arrow (longer line)
      }
        u8g2.sendBuffer();
      } else if (!directionsReceived) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 15, "Navigation is off");
        u8g2.sendBuffer();
      }
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 50, "Incoming call from:");
      u8g2.drawStr(0, 60, title.c_str());
      u8g2.sendBuffer();
      delay(5000);
      u8g2.clearBuffer();
      if (directionsReceived == true) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 8, directions.c_str());
        u8g2.drawStr(0, 25, distance.c_str()); // Draw distance
        u8g2.drawTriangle(105, 15, 110, 10, 115, 15); // Up arrow
        u8g2.drawLine(110, 15, 110, 20);
       if (directions.indexOf("north") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 30); // Up arrow (bigger)
        u8g2.drawLine(115, 30, 115, 40); // Up arrow (longer line)
      } else if (directions.indexOf("south") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 10); // Down arrow (bigger)
        u8g2.drawLine(115, 10, 115, 0); // Down arrow (longer line)
      } else if (directions.indexOf("east") != -1) {
        u8g2.drawTriangle(125, 25, 115, 15, 115, 35); // Right arrow (bigger)
        u8g2.drawLine(115, 25, 105, 25); // Right arrow (longer line)
      } else if (directions.indexOf("west") != -1) {
        u8g2.drawTriangle(105, 25, 115, 15, 115, 35); // Left arrow (bigger)
        u8g2.drawLine(115, 25, 125, 25); // Left arrow (longer line)
      } else if (directions.indexOf("northwest") != -1) {
        u8g2.drawTriangle(105, 10, 115, 20, 125, 30); // Northwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 30); // Northwest diagonal arrow (longer line)
      } else if (directions.indexOf("northeast") != -1) {
        u8g2.drawTriangle(125, 10, 115, 20, 105, 30); // Northeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 30); // Northeast diagonal arrow (longer line)
      } else if (directions.indexOf("southwest") != -1) {
        u8g2.drawTriangle(105, 30, 115, 20, 125, 10); // Southwest diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 125, 10); // Southwest diagonal arrow (longer line)
      } else if (directions.indexOf("southeast") != -1) {
        u8g2.drawTriangle(125, 30, 115, 20, 105, 10); // Southeast diagonal arrow (bigger)
        u8g2.drawLine(115, 20, 105, 10); // Southeast diagonal arrow (longer line)
      }
        u8g2.sendBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 50, "Current Time:");
        u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
        u8g2.sendBuffer();
      } else if (!directionsReceived) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 15, "Navigation is off");
        u8g2.sendBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 50, "Current Time:");
        u8g2.drawStr(0, 60, timeClient.getFormattedTime().c_str());
        u8g2.sendBuffer();
      }
    }
  }
}
