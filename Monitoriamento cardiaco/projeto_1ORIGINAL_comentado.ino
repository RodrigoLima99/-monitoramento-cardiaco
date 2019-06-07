//Nesse projeto pode ser impremenatdo um botão panico porém não impremenetei,masai deixei no codigo, rods perfeitamente sem ou com o botão
//boa parte do codigo está comentado o restante são comando delacrações. Esse codigo se comunica com um site o hingspeak.com por isso o ip do site e no final do codigo a parte do fild.
//Boa execusão 
// linlk da biblioteca PulseSensorPlayground.h:   https://github.com/WorldFamousElectronics/PulseSensorPlayground 
// linlk da biblioteca timer.h: https://github.com/JChristensen/Timer



#define USE_ARDUINO_INTERRUPTS true                           //Configure interrupções de baixo nível para a correspondência mais precisa do BPM e 
#define DEBUG true                                            //ative o DEBUG para mostrar comandos contínuos no monitor serial. 
#define SSID "PETAutoNET"                                     //Nome do seu wifi        
#define PASS "PET@utonet"                                     //Senha do wifi
#define IP "184.106.153.149"                                  //Ip do thingspeak.com

//Primeiro, incluímos todas as bibliotecas. Estamos usando o software serial para se comunicar com o esp8266.

#include <SoftwareSerial.h>                                   // Incluindo a Biblioteca da serial.
#include "Timer.h"                                            // Incluindo a Biblioteca do time.
#include <PulseSensorPlayground.h>                            // Incluindo a Biblioteca do sensor de pulso.

//Como vou chamar as bibliotecas o programa.

Timer t;                                                      
PulseSensorPlayground pulseSensor;


String msg = "GET /update?key=1L98R8B341V3HCHZ";               // chave pessoal do thingspeak 
SoftwareSerial esp8266(10,11);                                 // SoftwareSerial esp8266 10 = rx, 11 = tx.

//Variables

const int PulseWire = A0;                                     // PulseSensor fio de sinal dpo sensor conectado ao pino analogico 0.
const int LED13 = 13;                                         // Led  do arduino on-board, perto do pino 13.  
int Threshold = 550;                                          //fThreshold = 550, limiti do sensor de pulso.
float myTemp;                                                 //Variavel da temperatura myTemp.                 
int myBPM;                                                    //Variavel do BPM myBPM.
String BPM;                                                   //Variavel da palavra BPM.
String temp;                                                  //Variavel da palavra temp.
int error;                                                    //Variavel do erro.
int panic;                                                    //Variavel do botão pânico
int raw_myTemp;                                               //Variavel tempo Bruto.
float Voltage;                                                //Variavel da voltagem.
float tempC;                                                  //Variavel da temperatura em Celcius.
void setup()
{
 
  Serial.begin(9600);                                         //velocidade da serial begin.                    
  esp8266.begin(115200);                                      //velocidade da serial begin dp esp.
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);                            //automagicamente pisca LED Arduino com pulsação.
  pulseSensor.setThreshold(Threshold);                        //fThreshold = 550, limiti do sensor de pulso.

  // Verifique novamente se o objeto "Sensor de pulso" foi criado e "começou" a ver um sinal.
  
   if (pulseSensor.begin()) {
    Serial.println("pulseSensor Object !");                  // "pulseSensor Object !" Isso é impresso uma vez na inicialização do Arduino ou na redefinição do Arduino.  
  }
  Serial.println("AT");                                      //Comfirmação que esta lendo a serail AT = OK.                                    
  esp8266.println("AT");                                     //Comfirmação que esta lendo a serail do esp AT = OK.

  delay(3000);                                               //Delay de 3000 milisegundos.

  if(esp8266.find("OK"))                                     // se encontrar o esp manda OK.
  {
    Serial.println("RESPONDEU");                             //se ler a serial rwsponda RESPONDEU.
    connectWiFi();                                           //se tudo der certo conet ao  wifi.
    
  }else{                                                     //senão                                                
    Serial.println("N RESPONDEU");}                          //senão responder responda "N RESPONDEU".                    
  t.every(10000, getReadings);                               //obeter leiura
   t.every(10000, updateInfo);                               //obeter leiura
}
 

//Na função loop , chame panic_button () e temporizadores usando a função t.update () .
void loop()
{
  panic_button();                                            //Botão Pânico.
start:                                                       //Rótulo.
    error=0;  
   t.update();                                               //Atualizar.
   
    //Reenviar se a transmissão não for concluída
    
    if (error==1)                                            // se erro                                     
    {
      goto start; //go to label "start"                      //vai começar; vá para o rótulo "start".
    } 
 delay(4000);                                                //Delay de 4000 milisegundos.
}

void updateInfo()                                           //atualização de informação 
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";                   
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  if(esp8266.find("Error"))
  {
    return;
  }
  cmd = msg ;
  cmd += "&field1=";    //field 1 for BPM
  cmd += BPM;
  cmd += "&field2=";  //field 2 for temperature
  cmd += temp;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find(">"))
  {
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}
//Faça função para connectWiFi () que irá retornar True ou False dependendo do Wi-Fi conectado ou não. AT + CWMODE = 1 fará o ESP8266 funcionar no modo de estação .
AT + CWJAP = \, comando, usado nesta função, é conectar ao seu Ponto de Acesso (seu roteador Wi-Fi).

boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(5000);
  if(esp8266.find("OK"))
  {
    Serial.println("CONETADO");
    return true;
  }
  else
  {
    Serial.println("N CONECTADO");
    return false;
  }
}
//Faça getReadings (); função para levar sensor de pulso e leituras LM35 e convertê-las em string usando dtostrf (); função .

void getReadings(){
  raw_myTemp = analogRead(A1);                         // sensor de temperayura lm 35 conectado ao pino analogico A1 do arduino 
  Voltage = (raw_myTemp / 1023.0) * 5000;              // para obter 5000 milivots. pois o lm 35 é 5v, e ele não mede temperatura e sim volts e cada mili volts é representado  em graus  verifica datasheet
  tempC = Voltage * 0.1;                               //passando de volts para graus 
  myTemp = (tempC * 1.8) + 32;                         //Convertendo em fahrenheit °F
  myTemp = (myTemp - 32) * 5/9;                        //Convertendo em velcius °C
  Serial.println(myTemp);
  int myBPM = pulseSensor.getBeatsPerMinute();        // Chama a função em nosso objeto pulseSensor que retorna o BPM como um "int".
                                                      // "myBPM" mantém este valor de BPM agora. 
if (pulseSensor.sawStartOfBeat()) {                   // Constantemente testar para ver se "uma batida aconteceu". 
Serial.println(myBPM);                                // Imprima o valor dentro do myBPM. 
}

  delay(20);            
    char buffer1[10];
     char buffer2[10];
    BPM = dtostrf(myBPM, 4, 1, buffer1);
    temp = dtostrf(myTemp, 4, 1, buffer2);  
  }
//Da mesma forma, faça a função para panic_button . Quando o botão vai para ALTO, esp8266 envia as informações para o servidor usando os comandos AT + CIPSTART e AT + CIPSEN

void panic_button(){
 
  panic = digitalRead(8);
    if(panic == HIGH){
    Serial.println(panic);
      String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  if(esp8266.find("Error"))
  {
    return;
  }
  //Anexe esta informação a "& field3 =".
  cmd = msg ;
  cmd += "&field3=";    
  cmd += panic;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find(">"))
  {
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}
}
