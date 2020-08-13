// Projeto 43 - Módulo de RF 433Mhz com arduino - porta 12 emissor - 

#include <VirtualWire.h>  // Biblioteca necessário para comunicação
//sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature barramento(&oneWire);

char data[5]; //conversão float para string
 
void setup()
{
//sensor
Serial.begin(9600);
  barramento.begin();

  
// Initializa o IO e ISR
vw_setup(2000); // Bits por segundo


}
void loop()
{
  //sensor
barramento.requestTemperatures(); 
  Serial.print("\ntemperatura: ");
  float temperatura = barramento.getTempCByIndex(0);
  Serial.print(temperatura);
  char temp[8];
  dtostrf(temperatura,8,4,temp);
// Mensagem a ser enviada para o receptor
send(temp); 
delay(200);

}
void send (char *message)
{
vw_send(message, strlen(message));
vw_wait_tx(); // Aguarde até que toda a mensagem seja enviada 
}
