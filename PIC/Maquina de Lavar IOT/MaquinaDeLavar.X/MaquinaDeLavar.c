//Maquina de lavar iot
//Carlos Henrique Gomes 1760092
//Silas Leme Silverio 1760343
//Eng Computação 171


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pic18f4520.h>
#include "configura.h"
#include "lcd.h"


#define _XTAL_FREQ 4000000
#define botaoLigaDes PORTCbits.RC1
#define botaoAvanca PORTCbits.RC2
#define botaoSensorA PORTCbits.RC3
#define botaoSensorB PORTCbits.RC0

#define ledLiga PORTBbits.RB0
#define ledSensorNivel PORTBbits.RB5
#define ledValvula PORTBbits.RB6
#define ledMotorBomb PORTBbits.RB7


int  tempo=0, estado=0;
char estadoMes[15],tempoDis[5];

void escreve(void);
void escreve2(void);
void estadosMaquina(void);
void tempo1(void);
    
void main(void) {
   
    config();
    lcd_init();
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    TMR0=6;
    estadosMaquina();
    while (1) {
        if (!botaoLigaDes) { //Botão Liga e Desliga
            __delay_ms(5);
            if (!botaoLigaDes) {
                while (!botaoLigaDes);
                
                //LATA5 =! LATA5;
                if(estado == 0){
                    estado = 1;
                }else {
                    estado = 0;
                }
                estadosMaquina();
            }
        }
        
       if (!botaoAvanca) { //Botão avança estado
            __delay_ms(5);
            if (!botaoAvanca) {
                while (!botaoAvanca);
                if(estado!=0)
                estado = estado + 1;
                estadosMaquina();
                
            }
        }
        if(estado != 0) // transição de estados, checagem continua
        {
            estadosMaquina();
        }       
    }
}

void __interrupt(high_priority) tmr (void)
{ 
    if(RCIF)
    {
        RCIF=0;
            if (RCREG=='T')  //T causa a interupção,simulando a abertura da tampa
            {
                while (RCREG=='T'){
                }  
            }else if(RCREG=='Q'){ //Q causa a interupção,simulando fechar a tampa
                estadosMaquina();
            }
            else if(RCREG=='C'){ //C causa a interupção,e manda o estado atual pro java
                estadosMaquina(); 
            }else if(RCREG=='L'){ //L causa a interupção,Liga maquina
                estado = 1;
                estadosMaquina();
            }else if(RCREG=='D'){ //D causa a interupção,Desliga maquina
                estado = 9;
                estadosMaquina();
            }else if(RCREG=='A'){ //A causa a interupção,Avança etapa da maquina
                estado = estado + 1;
                estadosMaquina();
            }
    }       
}

void tempo1(void)
{
    for(tempo=5;tempo>-1;tempo--)
    {   if(estado!=0){ //Tratamento para caso a maquina seja desligada pelo Java
            __delay_ms(1000);
            sprintf(tempoDis, "%i", tempo);
            escreve2();}
        }
}

void estadosMaquina(void)
{       
        if (estado == 0) {
            ledMotorBomb =0; //desliga motor centrifuga 
            ledSensorNivel =0; //Desliga sensores
            ledValvula =0; //Desliga valvulas
            ledLiga = 0; //desliga maquina
            TXREG = 0;
            sprintf(estadoMes, "Desligado");
            escreve();
        }
        else if (estado == 1) {
            sprintf(estadoMes, "Encher");
            ledLiga = 1;
            ledMotorBomb =1; //Liga bomba de agua
            ledValvula =1; //Liga valvula de sabão
            TXREG = 1;  
            estado = estado + 1; // avança para o próximo estado
            escreve();
        }
        else if( (estado == 2) && (botaoSensorA == 0)){
            ledSensorNivel =1; // mostra que maquina está cheia
            ledMotorBomb =0; //´desliga bomba de agua
            ledValvula =0; //Desliga valvula de sabão
            __delay_ms(500);
            ledMotorBomb = 1; //Liga motor agitador
            sprintf(estadoMes, "Agitar");
            tempo1(); //chama rotina para contagem de tempo
            TXREG = 2;
            estado = estado + 1; // avança para o próximo estado
            escreve();
        }
        else if (estado == 3){
            ledSensorNivel =0; // mostra que maquina está esvaziando, sensor nivel alto desligado
            ledMotorBomb =0; //´desliga motor agitador
            __delay_ms(500);
            ledValvula =1; //aciona a valvula de drenagem
            sprintf(estadoMes, "Drenar");
            TXREG = 3;
            estado = estado + 1; // avança para o próximo estado
            escreve();
        }
        else if( (estado == 4) && (botaoSensorB == 0 )){
             ledSensorNivel =1; // acende mostrando que a maquina está sem agua, sensor nivel minimo.
             ledMotorBomb =1; //liga o motor da centrifuga
             sprintf(estadoMes, "Centrif.");
             tempo1();
             TXREG = 4;
             estado = estado + 1;// avança para o próximo estado
             escreve();
        }
        else if(estado == 5){
            ledMotorBomb =0; // desliga motor centrifuga
            ledValvula =0; // desliga valvula de drenagem 
             ledSensorNivel =0; //apaga sensor de baixo, está enchendo
            __delay_ms(500);
             ledMotorBomb =1;  // liga bomba d agua
             ledValvula =1; //aciona a valvula de amaciante
            sprintf(estadoMes, "Enxaguar");
            TXREG = 5;
            estado = estado + 1;// avança para o próximo estado
            escreve();
        }
        else if((estado == 6) && (botaoSensorA == 0)){
            ledMotorBomb =0; //desliga bomba d agua
            ledValvula =0;  // desliga valvula 
            __delay_ms(500);
            ledMotorBomb =1;//liga motor agitador
            ledSensorNivel =1; //acende mostrando que maquina encheu
            sprintf(estadoMes, "Agitar");
            tempo1();
            TXREG = 6;
            estado = estado + 1;// avança para o próximo estado
            escreve();
        }
        else if (estado == 7){
            ledMotorBomb =0; //desliga motor agitador
            ledSensorNivel =0; //desliga mostrando que está esvaziando
            __delay_ms(500);
            ledValvula =1; //liga valvula de drenagem
            sprintf(estadoMes, "Drenar");
            TXREG = 7;
            estado = estado + 1; // avança para o próximo estado
            escreve();
        }
        else if((estado == 8) && (botaoSensorB == 0 )){
            ledMotorBomb =1; //aciona motor centrifuga 
            ledSensorNivel =1; //ativa sensor baixo nivel
            sprintf(estadoMes, "Centrif.");
            tempo1();
            TXREG = 8;
            estado = estado + 1; // avança para o próximo estado
            escreve();
        }else if (estado == 9){
            ledMotorBomb =0; //desliga motor centrifuga 
            ledSensorNivel =0; //Desliga sensores
            ledValvula =0; //Desliga valvulas
            ledLiga = 0; //desliga maquina
            estado = 0; // vai para o estado desligado
            TXREG = 0;
            sprintf(estadoMes, "Desligado");
            escreve();
        }       
}




void escreve(void)
{
    
    lcd_clear();
    lcd_goto (0);
    lcd_puts ( "Estado:");
    lcd_puts (estadoMes);
    
}
void escreve2(void)
{
    
    lcd_clear();
    lcd_goto (0);
    lcd_puts ( "Estado:");
    lcd_puts (estadoMes);
    lcd_goto (40);
    lcd_puts ("Tempo:");
    lcd_puts (tempoDis);

}
