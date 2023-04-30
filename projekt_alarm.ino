#include <Keypad.h>  // biblioteka do klawiatury
#include <Wire.h>   // biblioteka do I2C
#include <LiquidCrystal_I2C.h>  // biblioteka do wyświetlacza
#include <SPI.h>   // bibiloteka do SPI
#include <MFRC522.h> // biblioteka do RFID

#define RST_PIN 9 
#define SS_PIN 10
#define czujnik_ruchu 8
#define buzzer A3


LiquidCrystal_I2C lcd(0x27, 16, 2); // deklaracja wyświetlacza
MFRC522 rfid(SS_PIN, RST_PIN); // deklaracja czytnika RFID

const byte brelok[] = {0x0C,0x4B,0x06,0x49};  // UID posiadanego breloka
const byte karta[] = {0x43,0x6A,0xCC,0x14};   // UID posiadanej karty
int wlam = 0;
char pin[4]={'1','2','5','8'};
char wpisane[4];
char klawisz='0';
char kratka='0';
int cyfrapinu=1;
int sygnal_wykrycia =0;
int alarm_rozpoczety=0;
int wyswietl_nieuzb=0;
int wyswietl_uzb=0;
int wyswietl_rozbr=0;
int rozb_start=0;
int syrena_start=0;
unsigned long syrena_czasstart = 0;
unsigned long syrena_czasakt = 0;
unsigned long syrena_odestep = 370;
unsigned long rozb_czasstart = 0;
unsigned long rozb_czasakt = 0;
unsigned long czasnapin = 10000;
const byte ROWS = 4; // ile wierszy
const byte COLS = 4; //ile kolumn
 
byte rowPins[ROWS] = {7, 6, 5, 4}; //piny wierszy
byte colPins[COLS] = {3, 2, 1, 0}; //piny kolum
 
char keys[ROWS][COLS] = { //mapowanie klawiatury
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
Keypad klaw = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); // deklaracja klawiatury

void setup() {
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(buzzer, OUTPUT);
  pinMode(czujnik_ruchu, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(9600);
}


void loop() {
klawisz=klaw.getKey();

switch(wlam){

  case 0: //wlam 0 - alarm nieuzbrojony
    if(wyswietl_nieuzb==0){
      wyswietl_nieuzb=1;
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Alarm");
    lcd.setCursor(1,1);
    lcd.print("nieuzbrojony");}
    if(klawisz)
    {
      if (klawisz == keys[3][0]) {wlam=1; wyswietl_nieuzb=0;}
    }
  break;

  case 1: //wlam 1 - alarm uzbrojony
    if(wyswietl_uzb==0){
      wyswietl_uzb=1;
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Alarm");
    lcd.setCursor(1,1);
    lcd.print("uzbrojony");}
  if(digitalRead(czujnik_ruchu)== HIGH) {
  wlam=2;
  wyswietl_uzb=0;
   }
  break;

  case 2:      //wlam 2 - daje chwile na rozbrojenie po wykryciu obecności 
    if(sygnal_wykrycia==0){
      sygnal_wykrycia=1;
      tone(buzzer, 2500);
      delay(100);
      noTone(buzzer);
      tone(buzzer, 3000);
      delay(100);
      noTone(buzzer);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wykryto wlamanie");
      delay(1000);}
    rozbrajanie();
  break;

  case 3:      //wlam 3 - alarm włączony
    if(alarm_rozpoczety==0){
      alarm_rozpoczety=1;
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("!! WLAMANIE !!");
      lcd.setCursor(1,1);
      lcd.print("!! WLAMANIE !!");}
    syrena();
  break;

  case 4:  // rozbrajanie, w trakcie uruchomionego alarmu
    syrena_czasstart=millis();
    if(syrena_start==0){
      syrena_czasakt=syrena_czasstart;
      syrena_start=1;}
    if((syrena_czasstart-syrena_czasakt)<=syrena_odestep){
      tone(buzzer, 3500);}
    if((syrena_czasstart-syrena_czasakt)>syrena_odestep && (syrena_czasstart-syrena_czasakt)<=(2*syrena_odestep)) {
      tone(buzzer, 4200);}
    if((syrena_czasstart-syrena_czasakt)>(2*syrena_odestep)){
      noTone(buzzer); syrena_start=0;
    }
    rozbrajanie();
  break;

}
}


void syrena() {
syrena_czasstart=millis();
if(syrena_start==0){
  syrena_czasakt=syrena_czasstart;
  syrena_start=1;}
if((syrena_czasstart-syrena_czasakt)<=syrena_odestep){
    tone(buzzer, 3500);}
if((syrena_czasstart-syrena_czasakt)>syrena_odestep && (syrena_czasstart-syrena_czasakt)<=(2*syrena_odestep)) {
    tone(buzzer, 4200);}
if((syrena_czasstart-syrena_czasakt)>(2*syrena_odestep)){
     syrena_start=0;}
kratka=klaw.getKey();
  if (kratka) {
    if (kratka == keys[3][1]) {
      wlam=4;
      noTone(buzzer);}
      }
}

void czytnikrfid() {
  
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  if (rfid.uid.uidByte[0] == brelok[0] || 
      rfid.uid.uidByte[1] == brelok[1] || 
      rfid.uid.uidByte[2] == brelok[2] || 
      rfid.uid.uidByte[3] == brelok[3] ) 
     {
      lcd.clear();
      wlam=0;
      rozb_start=0;
      lcd.println("Poprawny brelok ");
      lcd.setCursor(1,1);
      lcd.println("Witaj Rafal         ");
      noTone(buzzer);
      delay(3000);
      sygnal_wykrycia =0;
      alarm_rozpoczety=0;}

  if (rfid.uid.uidByte[3] == karta[0] || 
      rfid.uid.uidByte[2] == karta[1] || 
      rfid.uid.uidByte[2] == karta[2] || 
      rfid.uid.uidByte[3] == karta[3] ) 
     {
      lcd.clear();
      wlam=0;
      rozb_start=0;
      lcd.println("Poprawna kartaa ");
      lcd.setCursor(1,1);
      lcd.println("Witaj Rafal         ");
      noTone(buzzer);
      delay(3000);
      sygnal_wykrycia =0;
      alarm_rozpoczety=0;}

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

}
void rozbrajanie(){
  if(wyswietl_rozbr==0){
      wyswietl_rozbr=1;
      lcd.clear();
      lcd.print("Wpisz PIN     ");
      lcd.setCursor(0,1);
      lcd.print("lub uzyj breloka");}
  rozb_czasstart=millis();
  if(rozb_start==0){
    rozb_czasakt=rozb_czasstart;
    rozb_start=1;}
  if((rozb_czasstart-rozb_czasakt)<=czasnapin){
    if (rfid.PICC_IsNewCardPresent()) {czytnikrfid();}
  klawisz=klaw.getKey();
  if (klawisz) {
    if (cyfrapinu == 1 && klawisz == pin[0]) { cyfrapinu++;lcd.clear();lcd.print("*");}
    else if (cyfrapinu == 2 && klawisz == pin[1]) {cyfrapinu++;lcd.print("*");}
    else if (cyfrapinu == 3 && klawisz == pin[2]) {cyfrapinu++;lcd.print("*");}
    else if (cyfrapinu == 4 && klawisz == pin[3]) { 
      wlam = 0;
      noTone(buzzer);
      sygnal_wykrycia=0;
      wyswietl_rozbr=0; 
      rozb_start=0;
      alarm_rozpoczety=0;
      cyfrapinu = 1;
      klawisz='0';
      lcd.clear();
      lcd.print("PIN DOBRY");
      delay(1000);}
      else {
        wlam=3;
        sygnal_wykrycia =0;
        rozb_start=0;
        wyswietl_rozbr=0;
        alarm_rozpoczety=0;
        cyfrapinu = 1;
        klawisz='0';
        lcd.clear();
        lcd.print("PIN ZLY :( ");
        delay(1000);   
        }
    }
} else {wlam=3;
        sygnal_wykrycia =0;
        rozb_start=0;
        wyswietl_rozbr=0;
        alarm_rozpoczety=0;
        cyfrapinu = 1;
        klawisz='0';
        lcd.clear();
        lcd.print("Czas minal");
        delay(1000);}
}