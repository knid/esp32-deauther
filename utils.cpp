#include "utils.h"
#include "esp_wifi.h"

char *get_auth_mode(int authmode) {
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        return "OPEN";
        break;
    case WIFI_AUTH_OWE:
        return "OWE";
        break;
    case WIFI_AUTH_WEP:
        return "WEP";
        break;
    case WIFI_AUTH_WPA_PSK:
        return "WPA_PSK";
        break;
    case WIFI_AUTH_WPA2_PSK:
        return "WPA2_PSK";
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WPA_WPA2_PSK";
        break;
    case WIFI_AUTH_ENTERPRISE:
        return "ENTERPRISE";
        break;
    case WIFI_AUTH_WPA3_PSK:
        return "WPA3_PSK";
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WPA2_WPA3_PSK";
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        return "WPA3_ENT_192";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}
