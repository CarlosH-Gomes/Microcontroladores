// Projeto 43 - Módulo de RF 433Mhz com arduino - porta 5
#include <DS3232RTC.h>
#include <Streaming.h>
#include <Time.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <VirtualWire.h> // Biblioteca necessário para comunicação
// Um buffer para guardar as mensagens recebidas

//Pino CS do modulo cartao SD
int CS_PIN = 4;
 
File file;

char message[VW_MAX_MESSAGE_LEN]; 
byte messageLength = VW_MAX_MESSAGE_LEN; // Tamanho da mensagem 
void setup()
{
Serial.begin(9600);
vw_set_rx_pin(5);
inicia_SD();

setSyncProvider(RTC.get);
  Serial << F("Sincronizando com o RTC...");
  if (timeStatus() != timeSet) Serial << F(" Falha!");
  Serial << endl;

// Se comunicação entre o receptor e transmissor estiver correta irá...  
//aparecer essa mensagem de confirmação no serial monitor
Serial.println("Dispositivo pronto! Recebendo dados: "); 
// Inicializa o IO e ISR
vw_setup(2000); // Bits por segundo
vw_rx_start(); // Inicializa o receptor
}
void loop()
{

static time_t tLast;
  time_t t;
  tmElements_t tm;
 
  //Verifica se foi setado um novo horario
  //Formato: ano, mês, dia, hora, minuto, segundo
  if (Serial.available() >= 12) {
    int y = Serial.parseInt();
    if (y >= 100 && y < 1000)
      Serial<<F("Erro: Ano deve ter dois ou quatro digitos!") <<endl;
    else {
      if (y >= 1000)
        tm.Year = CalendarYrToTm(y);
      else    //(y < 100)
        tm.Year = y2kYearToTm(y);
      tm.Month = Serial.parseInt();
      tm.Day = Serial.parseInt();
      tm.Hour = Serial.parseInt();
      tm.Minute = Serial.parseInt();
      tm.Second = Serial.parseInt();
      t = makeTime(tm);
      RTC.set(t);
      setTime(t);
      Serial << F("Horario modificado para: ");
      printDateTime(t);
      Serial << endl;
      while (Serial.available() > 0) Serial.read();
    }
  }
 
  t = now();
  if (t != tLast) {
    tLast = t;
    printDateTime(t);
    //Grava dados no cartao a cada 1 minuto
    if (second(t) == 0)
    {
      
      Serial.println("nGravando dados no cartao SD...");
      grava_SD(t);
    }
    Serial << endl;
  }
}
void printDateTime(time_t t)
{
  printI00(day(t), 0);
  Serial << monthShortStr(month(t)) << _DEC(year(t));
  Serial << ' ';
  //printTime(t);
  printI00(hour(t), ':');
  printI00(minute(t), ':');
  printI00(second(t), ' ');
  if (vw_get_message(message, &messageLength)) // Sem bloqueio
      {
      Serial.print(message); 
      }
}
void grava_SD(time_t t)
{
  abre_arquivo_gravacao("data.txt");
  file.print("Data: ");
  file.print(day(t));
  file.print("/");
  if (month(t) < 10)
  {
    file.print("0");
  }
  file.print(month(t));
  file.print("/");
  file.print(year(t));
  file.print(" Hora: ");
  if (hour(t) < 10)
  {
    file.print("0");
  }
  file.print(hour(t));
  file.print(":");
  if (minute(t) < 10)
  {
    file.print("0");
  }
  file.print(minute(t));
  file.print(":");
  if (second(t) < 10)
  {
    file.print("0");
  }
  file.print(second(t));
  file.print(" Temperatura: ");
  if (vw_get_message(message, &messageLength)) // Sem bloqueio
  {
    file.print(message);  // Irá mostrar a menssagem "Dados enviados com sucesso!" 
  }
  fecha_arquivo();
}
 
//Correcao para imprimir "00" ao inves de "0" caso
//o valor seja menor do que 10
void printI00(int val, char delim)
{
  if (val < 10) Serial << '0';
  Serial << _DEC(val);
  if (delim > 0) Serial << delim;
  return;
}
 
void inicia_SD()
{
  pinMode(CS_PIN, OUTPUT);
 
  if (SD.begin())
  {
  } else
  {
    return;
  }
}
 
int abre_arquivo_gravacao(char filename[])
{
  file = SD.open(filename, FILE_WRITE);
 
  if (file)
  {
    return 1;
  } else
  {
    return 0;
  }
}
 
void fecha_arquivo()
{
  if (file)
  {
    file.close();
  }
}
