#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <SNMP.h>

// Define o tipo do sensor DHT11 e o pino ao qual está conectado
#define DHTTYPE DHT11
#define DHTPIN 2

// Cria o objeto do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Define o endereço IP e a porta do servidor web
IPAddress ip(192, 168, 0, 10);
EthernetServer server(80);

// Define o endereço IP e as informações de comunidade para o agente SNMP
IPAddress snmpAddress(192, 168, 0, 5);
char* snmpCommunity = "public";

void setup() {
  // Inicializa o sensor DHT e o Ethernet Shield
  dht.begin();
  Ethernet.begin(mac);
  server.begin();

  // Configura as informações do agente SNMP
  SNMP.begin(snmpAddress, snmpCommunity);

  // Registra as variáveis SNMP para temperatura e umidade
  SNMP.variable("temp", 0, INTEGER);
  SNMP.variable("hum", 0, INTEGER);
}

void loop() {
  // Lê a temperatura e a umidade do sensor DHT
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Se a leitura falhar, exibe uma mensagem de erro
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Erro ao ler o sensor DHT!");
    return;
  }

  // Exibe os valores da temperatura e umidade no monitor serial
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print(" °C, Umidade: ");
  Serial.print(hum);
  Serial.println("%");

  // Atualiza as variáveis SNMP com os valores da temperatura e umidade
  SNMP.update("temp", int(temp));
  SNMP.update("hum", int(hum));

  // Cria uma página da web com os valores da temperatura e umidade
  String webpage = "<html><head><title>Temperatura e Umidade</title></head><body>";
  webpage += "<h1>Temperatura: ";
  webpage += temp;
  webpage += " &#8451;</h1>";
  webpage += "<h1>Umidade: ";
  webpage += hum;
  webpage += " %</h1>";
  webpage += "</body></html>";

  // Exibe a página da web para o cliente
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Nova requisição HTTP!");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(webpage);
    delay(1);
    client.stop();
    Serial.println("Requisição HTTP atendida.");
  }

  // Espera por alguns segundos antes de ler os valores do sensor novamente
  delay(5000);
}
