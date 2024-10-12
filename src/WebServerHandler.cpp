#include <WebServer.h>
#include <Update.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Wi-Fi and WebServer settings
extern WebServer server;  // External reference to the web server
extern String version;    // Firmware version string

extern Adafruit_ST7789 tft;  // External reference to the display object

void setupWebServer(const char* webAuthUser, const char* webAuthPass) {
    server.on("/", HTTP_GET, []() {
        String html = "<h1>ESP32 Firmware</h1>";
        html += "<p>Current Firmware Version: " + version + "</p>";
        html += "<form method='POST' action='/update' enctype='multipart/form-data'>";
        html += "<input type='file' name='firmware'>";
        html += "<input type='submit' value='Update Firmware'>";
        html += "</form>";
        server.send(200, "text/html", html);
    });

    server.on("/update", HTTP_POST, []() {
        String message = Update.hasError() ? "Update Failed!" : "Update Success! Rebooting...";
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", message);
        ESP.restart();
    }, [webAuthUser, webAuthPass]() {
        if (!server.authenticate(webAuthUser, webAuthPass)) {
            return server.requestAuthentication();
        }

        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            } else {
                tft.fillScreen(ST77XX_BLACK);
                tft.setCursor(10, 10);
                tft.println("Updating...");
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u bytes\n", upload.totalSize);
                tft.fillScreen(ST77XX_BLACK);
                tft.setCursor(10, 10);
                tft.print("Update Successful!");
                delay(1000);
            } else {
                Update.printError(Serial);
            }
        }
    });

    server.begin();
}

void handleWebServer() {
    server.handleClient();
}
