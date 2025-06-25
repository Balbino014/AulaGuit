#include <Arduino.h>
#include <WiFi.h>
#include "internet.h"
#include "senhas.h"


const unsigned long tempoEsperaConexao = 15000;
const unsigned long tempoEsperaReconexao = 5000;


void conectaWiFi()
{
  Serial.printf("conectando ao WiFi: %s", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoInicialWiFi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoInicialWiFi < tempoEsperaConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Conectado com sucesso! ");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
  }

  else
  {
    Serial.println("\nFalha ao conectar WiFi. VErifique o nome da rede e a senha");
  }
}


void checkWiFi()
{
  unsigned long tempoAtual = millis();
  static unsigned long tempoUltimaConexao = 0;
  if (tempoAtual - tempoUltimaConexao > tempoEsperaConexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("\n Conexao perdida! Tentando reconectar...");
      conectaWiFi();
    }
      tempoUltimaConexao = tempoAtual;
  }
}
