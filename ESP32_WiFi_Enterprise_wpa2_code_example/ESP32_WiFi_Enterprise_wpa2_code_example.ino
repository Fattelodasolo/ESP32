#include <WiFi.h>
#include <WiFiClientSecure.h>
#if __has_include ("esp_eap_client.h")
#include "esp_eap_client.h"
#else
#include "esp_wpa2.h"
#endif
#include <Wire.h>
#define EAP_ANONYMOUS_IDENTITY "username" //ID e Username sono gli stessi
#define EAP_IDENTITY "username"
#define EAP_PASSWORD "password"
const char* ssid = "ssid";

int counter = 0;
const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh\n" \
                           "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
                           "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
                           "QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT\n" \
                           "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
                           "b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n" \
                           "AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n\n" \
                           "+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28\n" \
                           "OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK\n" \
                           "MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki\n" \
                           "f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik\n" \
                           "KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw\n" \
                           "HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW\n" \
                           "0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n" \
                           "BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n" \
                           "MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n" \
                           "OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n" \
                           "YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH\n" \
                           "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG\n" \
                           "BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc\n" \
                           "YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ\n" \
                           "/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG\n" \
                           "hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry\n" \
                           "qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ\n" \
                           "ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX\n" \
                           "TVyMnGo=\n" \
                           "-----END CERTIFICATE-----\n";

/*const char* test_client_key = \
                                "-----BEGIN RSA PRIVATE KEY-----\n" \
                                "MIIEpAIBAAKCAQEA0tIJrvzDItZly3zdktL0LQTabQi02RwECXr0csKwqa9T2Iw4\n" \
                                "nAnpfMsnHX7YVpUwKCfPq99DziiwaHmTteitN9y8PmLg3ULhyR+6RLfYBm0MhgIu\n" \
                                "JBOuVHH8cBV+Sc32nPzP+X/fqFaJkiGcy1Zo0C9lQlxIovGelPb0WiyRrYRCmp+k\n" \
                                "e49GdLWS4OfA/gRTlQKwIcPtrA4VT8fzo/ma6j35ZqIbYf6MlJmOeQGnqyGQfjBh\n" \
                                "79O4vrAUOO3ZU+v31ADwoUwxt2kCot+iwm75bBkcH7rHHoTHPerGutPJ/Zqg9iZQ\n" \
                                "twmb3uB6F2GkmfitMDWHJCHxWy29A4aBXe33aQIDAQABAoIBAQCUFgvNSil8CCPb\n" \
                                "CyvNHbd9BAK2Qj5enypKyn55Oj7EAq8f1mdRXDkRetZQBrDKKON54XUj4qGLZ9N6\n" \
                                "n6PdTMKYEcp8/BG9+mQFVe8OpCDvapw4acGlf9md4a+Tv9VE3JNVdNRWCre9VIQw\n" \
                                "DMJJprnbOt+i/JXdN2HQe6Pqz5HI0m8eB82u+NzTa2PsXGY5sJLIYkeaJW6+IOFU\n" \
                                "H0mkLK+6IVP2b9eUrla2xzGcsdddcChVCX6W7jfG6T/uYIanrrIaAf3VylZZ4Fzj\n" \
                                "Wtbf0eUSouEthXbr/Rq4Ejl0NcU3eZ8iDmySZSvLvSxlMpIusn3zBp8Oht8lr6db\n" \
                                "E+pGs0KxAoGBAPHy/3GptKI2b4JjJt1h/WD3maMUFGprGF17mzvAJXcmi2Lnx8Gz\n" \
                                "Mp/lXS39sO4goO3zaqya8Kaa57+PUj5Pg90sYWs8zt79xxZTcv2C5QYukTmVTKpH\n" \
                                "OA0NjZTRTAaTi/XI0cOv+8XdHEz3oa93y+VFUCVempKTU+8WLiqNNxrdAoGBAN8Q\n" \
                                "QZR19hd7ZK5p2r6SpC0pQ2RepGnhpB0Wuw8C6D5DeNprMLwNwaVJTx2AD9grXLqa\n" \
                                "d2O0f4OfQi8VXbeWoX6kmPF7jjjhzRVNRhhwu44ELjy/s2fYVufQ2rs3U8hNEC71\n" \
                                "IidbZ45MaV7Dz2uoq0/gD19aC+PuCJxp4cEy9+f9AoGALSO72rXhuH3VcgsTniwA\n" \
                                "/M1VAaLmaDBQ3GgKPyyLEN7FFI/PqejH4iMFrmXtU5AM1jgax9ckXMD7ybZwjqWB\n" \
                                "IYt9JWez5NCOCzFIe9mm5u9bMtYS9pXQ9EZ6CrC9sWBntVRhJlQ38eNdu1Jb/QOp\n" \
                                "q72xxTcQOZZ5C1slA9VS2ZUCgYBDubHPm4keVA6bjfQmTGugcpjNjpaotyOW2WHu\n" \
                                "CtnsnZvB5S2kyyBySLHGqS0mWvQKi/+XvCPDGMWFeh08MnXR97OgDf1+c8BKqW2Q\n" \
                                "2hl34vkO5L1Z2Ct6ssTWdtPvSsaRG0hFXtEwOS784MLHUyzAUa0ZLEPT7l+a7eLn\n" \
                                "P8xZGQKBgQDGQrCf80OD2xhPfngpqCmGNDbOMCtKKkD55f202cVQzm25i/Ye4WcF\n" \
                                "WxE4xhpSGLvDLITkaXXkRqtAQUjJrFXt8SjJ+24BVRlvtHby9yuEZZKwmzjhRcln\n" \
                                "c62/pov7m2Ni8HOc/ANvejW9aMOrdNdACGMhKYAOIvKPS+GmA0Nl4Q==\n" \
                                "-----END RSA PRIVATE KEY-----\n"; */
/*const char* test_client_cert = \
                                 "-----BEGIN CERTIFICATE-----\n" \
                                 "MIID6DCCAtCgAwIBAgIBAjANBgkqhkiG9w0BAQsFADB2MQswCQYDVQQGEwJJTjEL\n" \
                                 "MAkGA1UECAwCTUgxDTALBgNVBAcMBENJVFkxDDAKBgNVBAoMA09SRzENMAsGA1UE\n" \
                                 "CwwEVU5JVDELMAkGA1UEAwwCQ0ExITAfBgkqhkiG9w0BCQEWEmNhY2VydEBleGFt\n" \
                                 "cGxlLmNvbTAeFw0yMjA2MTUwNTQ2NTRaFw0zMjA2MTIwNTQ2NTRaMHwxCzAJBgNV\n" \
                                 "BAYTAklOMQswCQYDVQQIDAJNSDENMAsGA1UEBwwEQ0lUWTENMAsGA1UECgwET1JH\n" \
                                 "MTEOMAwGA1UECwwFVU5JVDExDzANBgNVBAMMBkNMSUVOVDEhMB8GCSqGSIb3DQEJ\n" \
                                 "ARYSY2xpZW50QGV4YW1wbGUuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
                                 "CgKCAQEA0tIJrvzDItZly3zdktL0LQTabQi02RwECXr0csKwqa9T2Iw4nAnpfMsn\n" \
                                 "HX7YVpUwKCfPq99DziiwaHmTteitN9y8PmLg3ULhyR+6RLfYBm0MhgIuJBOuVHH8\n" \
                                 "cBV+Sc32nPzP+X/fqFaJkiGcy1Zo0C9lQlxIovGelPb0WiyRrYRCmp+ke49GdLWS\n" \
                                 "4OfA/gRTlQKwIcPtrA4VT8fzo/ma6j35ZqIbYf6MlJmOeQGnqyGQfjBh79O4vrAU\n" \
                                 "OO3ZU+v31ADwoUwxt2kCot+iwm75bBkcH7rHHoTHPerGutPJ/Zqg9iZQtwmb3uB6\n" \
                                 "F2GkmfitMDWHJCHxWy29A4aBXe33aQIDAQABo3sweTAJBgNVHRMEAjAAMCwGCWCG\n" \
                                 "SAGG+EIBDQQfFh1PcGVuU1NMIEdlbmVyYXRlZCBDZXJ0aWZpY2F0ZTAdBgNVHQ4E\n" \
                                 "FgQUFVW72Au/k1EUkawGOKwXOVREZP0wHwYDVR0jBBgwFoAUe5BHvV/opTgq28V/\n" \
                                 "4NC2FyWxG2wwDQYJKoZIhvcNAQELBQADggEBAKRGwhMhxxA5t8OV84Hj6S58Xb8Z\n" \
                                 "0u3Bfc4RFMR9OHJVuirEJPUeGmoLN1fWIs0xWzMdfie2s9nAtTBLGA6J1d/P4Vwv\n" \
                                 "I/AdChs+iAPjCvzC9Pg6phdC6llrd9wqZMAHD5RhTFmENHWOK5djqxG2Fm6D/efq\n" \
                                 "gL0jWLAQf9Fd86/yPWAUbdBRjHAtWs918IKBZDJ1x7Y97IKC3XMbiqL8p4lpW7xx\n" \
                                 "lCtclbPEPuDjtJ5nj5+50d1oMsEzK5vpladUb5bz8eK3agHZSETVHD1A8ZhBHYiO\n" \
                                 "lsmBxhaWUYo+3FnqoJuRCtKy6UX7fIGumoKGINmfr5Spl8MlHS5jzxBTfPs=\n" \
                                 "-----END CERTIFICATE-----\n";*/
WiFiClientSecure client;
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Connessione alla rete: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  // WiFi si deve disconnettere
  WiFi.mode(WIFI_STA); //inizializza wifi mode Station
#if __has_include ("esp_eap_client.h")
  esp_eap_client_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_eap_client_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_enterprise_enable();
#else
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
#endif
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) { //in caso di insuccesso, dopo 30 secondi riavvia la ESP32
      ESP.restart();
    }
  }
  client.setCACert(test_root_ca); // verifica con Public Certificate Authority
  //client.setCertificate(test_client_cert); // verifica certificato cliente
  //client.setPrivateKey(test_client_key);  // verifica con chiave privata
  Serial.println("");
  Serial.println("Connesso alla rete Wi-Fi");
  Serial.println("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
}
void loop() {
 
}
