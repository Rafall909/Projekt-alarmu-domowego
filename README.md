# Projekt-alarmu-domowego
Projekt alarmu wykonany w ramach przedmiotu "Mechatronika w systemach nadzoru i bezpieczeństwa"

Kod realizujący system alarmu bazuje na 5 stanach:
1 - alarm nieuzbrojony
2 - alarm uzbrojony
3 - stan krótkiego oczekiwania na rozbrojenie po wykryciu obecności
4 - alarm włączony (aktywna syrena + sygnały świetlne)
5 - stan rozbjarania alarmu w trakcie włączonych sygnałów dźwiękowo-wizualnych

Uzbrojony alarm reaguje na sygnały z następujących czujników:
1 - czujnik ruchu PIR
2 - czujnik otwarcia drzwi - kontaktron


Rozbrojenie alarmu może nastąpić na 2 sposoby:
1 - wpisanie odpowiedniego kodu PIN na klawiaturze
2 - przyłożenie przypisanej karty lub breloku do czytnika RFID

Filmik pokazujący działanie alarmu zamontowanego na makiecie:
https://drive.google.com/file/d/1h-231ImY-cU6UckkB7-FXfa9oa_Lae5M/view?usp=sharing
