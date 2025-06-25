#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "internet.h"
#include "certificados.h"
#include "botao.h"
#include <Adafruit_NeoPixel.h>
#include <ezTime.h>

const int mqtt_port = 8883;
const char *mqtt_id = "esp32Diogo";
const char *mqtt_SUB = "senai134/mesa03";
const char *mqtt_PUB = "senai134/mesa03";

bool estadoAnterior = false;
bool estadoAtual = false;
bool mensagemmqtt = false;
int hora = 0;
int minuto[2] = {0, 0};

Timezone tempo;
WiFiClientSecure espClient;
PubSubClient mqtt(espClient);
Boot boot (0);
Adafruit_NeoPixel ledRGB(1, 48, NEO_GRB);

void ConectaMQTT();
void Callback(char *topic, byte *payload, unsigned int length);

void setup()
{
    Serial.begin(9600);
    conectaWiFi();
    
    espClient.setCACert(AWS_ROOT_CA);
    espClient.setCertificate(AWS_CERT);
    espClient.setPrivateKey(AWS_KEY);

    tempo.setLocation("America/Sao_Paulo");
    waitForSync();
    Serial.print("horario srincronizado");

    mqtt.setServer(AWS_BROKER, mqtt_port);
    mqtt.setCallback(Callback);

    ledRGB.begin();
    ledRGB.setBrightness(150); //intensidade de brilho
    ledRGB.setPixelColor(0, ledRGB.Color(255, 221, 0)); //cor do led do esp
    ledRGB.show();
}

void loop()
{
    checkWiFi();
    boot.atualizar();
    if (!mqtt.connected())
    {
        ConectaMQTT();
    }

    mqtt.loop();

    static unsigned long tempo_pub = 0;
    unsigned long agora = millis();

    estadoAtual = boot.pressionado();

    hora = tempo.hour();
    minuto[1] = tempo.minute();

    // Formata a hora no formato HH:MM
    String horaFormatada = (hora < 10) ? "0" + String(hora) : String(hora);
    String minutoFormatado = (minuto[1] < 10) ? "0" + String(minuto[1]) : String(minuto[1]);
    String horario = horaFormatada + ":" + minutoFormatado;

    if (estadoAtual && !estadoAnterior)
    {
        mensagemmqtt = !mensagemmqtt;
        tempo_pub = agora;

        JsonDocument doc;
        if (mensagemmqtt)
        {
            doc["estado"] = "iniciado ";
            doc["hora"] = horario;

            ledRGB.setPixelColor(0, ledRGB.Color(255, 0, 8));
            String mensagem;
            serializeJson(doc, mensagem);
            mqtt.publish(mqtt_PUB, mensagem.c_str());
        }
        else
        {
            doc["estado"] = "livre";
            doc["hora"] = horario;

            ledRGB.setPixelColor(0, ledRGB.Color(46, 163, 10));
            String mensagem;
            serializeJson(doc, mensagem);
            mqtt.publish(mqtt_PUB, mensagem.c_str());
        }
        ledRGB.show();
    }

    estadoAnterior = estadoAtual;
}

void Callback(char *topic, byte *payload, unsigned int length)
{
    String msg((char *)payload, length);
    Serial.printf("Mensagem recebida (tópico: [%s]): %s \n\r", topic, msg.c_str());
}

void ConectaMQTT()
{
    while (!mqtt.connected())
    {
        Serial.println("Conectando ao AWS IoT Core ...");
        if (mqtt.connect(mqtt_id))
        {
            Serial.println("Conectado.");
            mqtt.subscribe(mqtt_SUB);
        }
        else
        {
            Serial.printf("Falhou (%d). Tentando novamente em 5s \n\r", mqtt.state());
            delay(5000);
        }
    }
}
