// O ModbusSlave implementa parte do protocolo Modbus no
// Arduino. Apenas holding registers sao usados
#include <ModbusSlave.h>

#define PINLED 13
#define PINBOT 4
#define PINPOT A5

#define BOMBA 0
#define VAZAO 1
#define NIVEL 2
#define SAIDA 3
#define REALVAZAO 4
#define TAGQTE  5

#define CAPACIDADETANQUE 1000
#define NIVELINICIAL 750
#define VAZAOENTRADA 1000
#define VAZAOSAIDA 500

#define ESCALA 100.0

int tag[TAGQTE]; // Array com os holding registers
ModbusSlave mb;  // mb eh o objeto da classe Modbus
unsigned long t, tant; // indicadores de tempo

int primeiraVez; // Variavel auxiliar

double nivel; // nivel em mililitros

void setup() {                
  pinMode (PINBOT, INPUT_PULLUP);  
  pinMode (PINLED, OUTPUT);     
  pinMode (PINPOT, OUTPUT);     
  
  tag[BOMBA] = LOW; // Inicia desligada
  tag[VAZAO] = 0;   // Inicia fechada
  tag[NIVEL] = NIVELINICIAL; // Volume inicial
  tag[SAIDA] = LOW; // Valvula de saida fechada
  tag[REALVAZAO] = 0.0;
  
  nivel = NIVELINICIAL*1000.0;
  primeiraVez = 1;
  
  digitalWrite (PINLED, tag[BOMBA]);    
  
  // Configura e inicia o gerenciador de protocolo
  mb.configure(1, 9600, 'n', 0); //(Endereco escravo, Taxa de transmissao, Paridade (8 bits sem paridade), Pino de enderecamento (0 ou 1 não contam));
  tant = millis(); // Inicio dos tempos
}


void loop() {
  // Sincroniza com o supervisorio
  mb.update(tag, TAGQTE);
  
  // Calcula o nivel atual
  if (tag[BOMBA] == HIGH)
      nivel += (ESCALA*(double)tag[REALVAZAO] * (millis()-tant)) / 3600.0;
  if (tag[SAIDA])
      nivel -= (ESCALA*(double)VAZAOSAIDA * (millis()-tant)) / 3600.0;
  tag[NIVEL] = (int)(nivel/1000.0);
  
  // Verifica se o botao foi pressionado e atualiza
  if (digitalRead(PINBOT) == LOW) { // Botao pressionado
     if (primeiraVez) {
        tag[BOMBA] = !tag[BOMBA];
	primeiraVez = 0;
     }
  } else {
     primeiraVez = 1;  
  }
//  tag[VAZAO] = tag[BOMBA]?map(analogRead(PINPOT), 0,1023, 0,VAZAOENTRADA):0; 
  tag[REALVAZAO] = tag[BOMBA]?tag[VAZAO]:0; 
  delay(5);

  // Atualiza as saidas
  digitalWrite (PINLED, tag[BOMBA]);    
  tant = millis();
  
  
  delay(10); // Importante para dar tempo da comunicação se completar
}


