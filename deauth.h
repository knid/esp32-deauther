#ifndef DEAUTH
#define DEAUTH

#include "esp_wifi_types.h"

void send_deauth_frame_to_ap(wifi_ap_record_t ap_record);

#endif // !DEAUTH
