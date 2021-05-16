#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define pino_sinal_analogico A0

#define pino_led_vermelho 13 //5
#define pino_led_verde 12 //7
#define pino_bomba 14 //8 14

//Variáveis
int valor_analogico;
String strMSG = "0";
char mensagem[30];


const char* ssid = "casazul";   
const char* password =  "internetdanieldantas";

const char* mqttServer = "driver.cloudmqtt.com";
const char* mqttUser = "urxhvxbl";              
const char* mqttPassword = "9qbWP_T0cJ96";      
const int mqttPort = 18728;                     
const char* mqttTopicSub = "jardim/bomba";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);
  pinMode(pino_sinal_analogico, INPUT);
  pinMode(pino_led_vermelho, OUTPUT);
  pinMode(pino_led_verde, OUTPUT);
  pinMode(pino_bomba, OUTPUT);

  WiFi.begin(ssid, password);

  Serial.print("Entrando no Setup");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.println("Aguarde! Conectando ao WiFi...");
#endif
  }
#ifdef DEBUG
  Serial.println("Conectado na rede WiFi com sucesso!");
#endif

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
#ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
#endif

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
#ifdef DEBUG
      Serial.println("Conectado com sucesso");
#endif

    } else {
#ifdef DEBUG
      Serial.print("falha estado  ");
      Serial.print(client.state());
#endif
      delay(2000);
    }
  }

  
  client.subscribe(mqttTopicSub);

}

void loop() {

  if (!client.connected()) {
    Serial.print("Passou no Connected");
    reconect();

  }
 
  Serial.print(valor_analogico);
  //Lendo o valor do pino A0 do sensor para detecção
  valor_analogico = analogRead(pino_sinal_analogico);

  
  Serial.print("Porta analogica: ");
  Serial.print(valor_analogico);

  
    if (valor_analogico >= 0 && valor_analogico < 500)
    {
      Serial.println("Status: Solo umido");
    
      digitalWrite (pino_led_vermelho, LOW);
      digitalWrite (pino_led_verde, HIGH);
      digitalWrite (pino_bomba, HIGH);
  
      
      sprintf(mensagem, "0");
      Serial.print("Mensagem enviada: ");
      Serial.println(mensagem);
      client.publish("jardim/bomba", mensagem);
      Serial.println("Mensagem enviada com sucesso...");
  
    }

 
  
    if (valor_analogico >= 500 && valor_analogico <= 1024)
    {
      Serial.println(" Status: Solo seco");
      
      digitalWrite (pino_led_verde, LOW);
      digitalWrite (pino_led_vermelho, HIGH);
      digitalWrite (pino_bomba, LOW);
  
      
      sprintf(mensagem, "1"); 
      Serial.print("Mensagem enviada: ");
      Serial.println(mensagem); 
      client.publish("jardim/bomba", mensagem);
      Serial.println("Mensagem enviada com sucesso...");
  
    }
  delay(10000);

  client.loop();

}

void callback(char* topic, byte* payload, unsigned int length) {

  
  payload[length] = '\0';
  strMSG = String((char*)payload);

#ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
#endif

}


void reconect() {
  
  while (!client.connected()) {
#ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
#endif

    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if (conectado) {
#ifdef DEBUG
      Serial.println("Conectado!");
#endif
      
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
#ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
#endif
      
      delay(5000);
    }
  }
}
