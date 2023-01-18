#include <Arduino.h>
#include "WiFi.h"
#include <esp_now.h>

//parametros del hc sr501
const int led = 2;
const int pir = 14;
int det = 0; //si detecta envia un uno
//parametros del envio
uint8_t broadcastAddress[] = {0xc0, 0x49, 0x0f, 0x00, 0x00, 0x00};

typedef struct struct_message {
  char a[32];
  int b;
} struct_message;


struct_message myData;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup(){
  Serial.begin(9200);
//parametros del hc sr501
pinMode(led, OUTPUT);
pinMode(pir, INPUT);
digitalWrite(led, LOW);

  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK){
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Fallo al añadir peer");
    return;
  }
}

void loop(){
  if (digitalRead(pir) == LOW){
    det = 0;
    digitalWrite(led,LOW);
  }
  if (digitalRead(pir) == HIGH){
    det = 1;
    digitalWrite(led,HIGH);
  }
  strcpy(myData.a, "Enviando datos");
  myData.b = det;

  esp_now_register_send_cb(OnDataSent);
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if(result == ESP_OK){
    Serial.println("enviado con exito");
  }
  else{
    Serial.println("fallo en el envio");
  }
  
  delay(2000);
}
