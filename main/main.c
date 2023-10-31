#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"
#include "protocol_examples_common.h"

static const char *TAG = "HTTP_CLIENT";

// Your Account SID and Auth Token from twilio.com/console
char ifttt_api_key[] = "replace_with_your_key";
char ifttt_event_name[] = "sensor_readings";

void twilio_send_sms(void *pvParameters)
{

    char twilio_url[200];
    snprintf(twilio_url,
             sizeof(twilio_url),
             "%s%s%s%s",
             "https://maker.ifttt.com/trigger/",
             ifttt_event_name,
             "/with/key/",
             ifttt_api_key);

    char post_data[200];

    snprintf(post_data, sizeof(post_data), "{\"value1\":\"%d\",\"value2\":\"%d\",\"value3\":\"%d\"}", 5, 5, 5);


    esp_http_client_config_t config = {
        .url = twilio_url,
        .method = HTTP_METHOD_POST,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    ESP_LOGI(TAG, "post = %s", post_data);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 201)
        {
            ESP_LOGI(TAG, "Message sent Successfully");
        }
        else
        {
            ESP_LOGI(TAG, "Message sent Failed");
        }
    }
    else
    {
        ESP_LOGI(TAG, "Message sent Failed");
    }
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	//connect_wifi();

    
    ESP_ERROR_CHECK(example_connect());
    ESP_LOGI(TAG, "Connected to AP, begin http example");

	
	xTaskCreate(&twilio_send_sms, "twilio_send_sms", 8192, NULL, 6, NULL);
	
}
