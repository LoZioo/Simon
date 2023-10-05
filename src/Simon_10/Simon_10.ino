#include <EEPROM.h>
#define PIEZO 6
        //Pin di controllo per il piezo (cicalino)
        //NB: Il pin da utilizzare deve necessariamente essere un pin
        //utilizzabile in modalità PWM
#define LIVELLI 10 //Lv. massimo
#define LEN 4 //Num. di pulsanti e led

const int NOTE[LEN]={123,247,494,988};
const int LED[LEN]={7,8,9,10};
        //Pin di controllo per i led
const int BTN[LEN]={5,2,1,0};
        //Pin di controllo per i pulsanti
        //NB: Controllare se il proprio microcontrollore Arduino è dotato di
        //resistori interni di pullup, il caso contrario, sarà necessario
        //inserire esternamente 4 resistori di pullup, pena, il danneggiamento e
        //malfunzionamento del microcontrollore;
        //Nel secondo caso, sarà necessario cambiare l'istruzione
        //pinMode(BTN[i],INPUT_PULLUP); in pinMode(BTN[i],INPUT);
        //Non mi ritengo responsabile per qualsiasi danno arrecato
        //al microcontrollore.
const int LEVEL[LEN]={4,6,8,10};

bool errore=false;
int k, l=EEPROM.read(0), a[LIVELLI]; //k=contatore livello attuale, l=livello di partenza, a[]=sequenza del memo

void setup(){
  pinMode(PIEZO,OUTPUT);
  for(int i=0;i<LEN;i++){
    pinMode(LED[i],OUTPUT);
    pinMode(BTN[i],INPUT_PULLUP);
  }
  
  //EEPROM.write(0,4); //Programmare la prima volta con questa istruzione
  //Poi commentarla alla seconda programmazione
  
  randomSeed(analogRead(A3));
  k=0;
  for(int i=0;i<l;i++)
    a[i]=random(LEN);
  
  intro();
}

void loop(){
  show();
  
  int i=0;
  while(i<k && !errore){
    if(a[i]!=btn(false))
      errore=true;
    i++;
  }
  
  if(errore)
    led(-2); //Sconfitta
  
  if(k==l)
    led(-1); //Vittoria
}

void intro(){
  for(int i=0;i<LEN;i++)
    led(i);
  
  for(int i=LEN-1;i>=0;i--)
    led(i);
}

void (*rst)(void)=0;

void show(){
  delay(500);
  for(int i=0;i<k+1;i++)
    led(a[i]);
  k++;
}

int btn(bool wait){ //wait serve per disabilitare il timeout
  int input=-1, t=millis()+5000, tLv=millis()+2000;
  
  while(input==-1){
    input=scan();
    
    if(!wait){
      if(millis()>=t) //Timeout
        led(-2);
      
      if(input>-1){ //Stampa tasto premuto
          led(input);
          
          while(digitalRead(BTN[input])==LOW) //Impostazione livello
            if(millis()>=tLv)
              level();
      }
    }
    
    delay(1);
  }
  delay(100);
  return input;
}

void level(){
  bool done=false;
  int input, t;
  
  for(int i=0;i<LEN;i++) //Per stampare la difficoltà attuale
    if(l==LEVEL[i])
      input=i;
  
  while(!done){
    while(input==-1){
      input=scan();
      delay(1);
    }
    
    for(int i=0;i<input+1;i++)
      digitalWrite(LED[i],HIGH);
    if(input>-1)
      for(int i=input+1;i<LEN;i++)
        digitalWrite(LED[i],LOW);
    tone(PIEZO,NOTE[input]);
    delay(500);
    noTone(PIEZO);
    
    l=LEVEL[input];
    t=millis()+1000;
    while(digitalRead(BTN[input])==LOW && !done)
      if(millis()>=t)
        done=true;
    
    input=-1;
  }
  
  for(int i=0;i<LEN;i++)
    digitalWrite(LED[i],LOW);
  
  EEPROM.update(0,l);
  rst();
}

int scan(){
  for(int i=0;i<LEN;i++) //Controllo dei tasti
      if(digitalRead(BTN[i])==LOW)
        return i;
  return -1;
}

void led(int n){
  switch(n){
    case -1: //Vittoria
      delay(500);
      for(int i=0;i<l;i++){
          for(int j=0;j<LEN;j++)
            digitalWrite(LED[j],HIGH);
        tone(PIEZO,NOTE[a[i]]);
        delay(100);
          for(int j=0;j<LEN;j++)
            digitalWrite(LED[j],LOW);
        noTone(PIEZO);
        delay(100);
      }
      delay(1000);
      btn(true);
      rst();
      break;
      
    case -2: //Sconfitta
      tone(PIEZO,60);
      for(int i=0;i<4;i++){
        for(int j=0;j<LEN;j++)
            digitalWrite(LED[j],HIGH);
        delay(100);
        for(int j=0;j<LEN;j++)
          digitalWrite(LED[j],LOW);
        delay(100);
      }
      noTone(PIEZO);
      delay(1000);
      rst();
      break;
      
    default:
      tone(PIEZO,NOTE[n]);
      digitalWrite(LED[n],HIGH);
      delay(500);
      noTone(PIEZO);
      digitalWrite(LED[n],LOW);
      delay(100);
      break;
  }
}
