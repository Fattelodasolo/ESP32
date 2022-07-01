#include "esp_camera.h"
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#endif

#include <ESP_Mail_Client.h>

#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

const char *ssid_Router     = "SSID_NAME";  //input your wifi name
const char *password_Router = "PASSWORD";  //input your wifi passwords
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_465

#define AUTHOR_EMAIL "emailmittente"
#define AUTHOR_PASSWORD "passworddellapp"

SMTPSession smtp;

camera_config_t config;

void smtpCallback(SMTP_Status status);

void startCameraServer();
void config_init();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  config_init();
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 0);
  s->set_brightness(s, 1);
  s->set_saturation(s, -1);

  WiFi.begin(ssid_Router, password_Router);
  while (WiFi.isConnected() != true) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

      smtp.debug(1);
      
    smtp.callback(smtpCallback);

    ESP_Mail_Session session;

    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;
    session.login.user_domain = F("");

    session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
    session.time.gmt_offset = 3;
    session.time.day_light_offset = 0;

    SMTP_Message message;

    message.enable.chunking = true;

    message.sender.name = F("ESP Mail");
    message.sender.email = AUTHOR_EMAIL;

    message.subject = F("Invio immagine da camera");
    message.addRecipient(F("ESP32-CAM"), F("emaildeldestinatario"));

    message.html.content = F("<span style=\"color:#ff0000;\">L'immagine della camera.</span><br/><br/><img src=\"cid:image-001\" alt=\"esp32 cam image\"  width=\"800\" height=\"600\">");

    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    message.html.charSet = F("utf-8");

    camera_fb_t *fb = esp_camera_fb_get();

    SMTP_Attachment att;

    att.descr.filename = F("camera.jpg");
    att.descr.mime = F("image/jpg");

   att.blob.data = fb->buf;
    att.blob.size = fb->len;

    att.descr.content_id = F("image-001"); //The content id (cid) of camera.jpg image in the src tag

    att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;

    message.addInlineImage(att);

    if (!smtp.connect(&session))
        return;

    if (!MailClient.sendMail(&smtp, &message, true))
        Serial.println("Error sending Email, " + smtp.errorReason());

    ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
}

void loop() {

}

void config_init() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_count = 1;
}

void smtpCallback(SMTP_Status status)
{
    Serial.println(status.info());

    if (status.success())
    {
        Serial.println("----------------");
        ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
        ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
        Serial.println("----------------\n");
        struct tm dt;

        for (size_t i = 0; i < smtp.sendingResult.size(); i++)
        {
            SMTP_Result result = smtp.sendingResult.getItem(i);
            time_t ts = (time_t)result.timestamp;
            localtime_r(&ts, &dt);

            ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
            ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
            ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
            ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
            ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
        }
        Serial.println("----------------\n");

        smtp.sendingResult.clear();
    }
}
