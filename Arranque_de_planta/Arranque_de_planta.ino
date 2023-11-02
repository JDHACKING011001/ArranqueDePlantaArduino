#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

RTC_DS3231 rtc;

int signal_planta = 11;
int signal_luz = 10;
int select_man = 9;
int select_aut = 8;
int arranque = 7;
int planta_off = 6;


int zellerDia;
//variables para la congruencia de zeller

void setup() {

  pinMode(signal_luz, INPUT_PULLUP);
  pinMode(signal_planta, INPUT_PULLUP);
  pinMode(select_man, INPUT_PULLUP);
  pinMode(select_aut, INPUT_PULLUP);
  pinMode(arranque, OUTPUT);
  pinMode(planta_off, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);

  if (!rtc.begin()) {                              // si falla la inicializacion del modulo
    Serial.println("Modulo RTC no encontrado !");  // muestra mensaje de error
    while (1)
      ;  // bucle infinito que detiene ejecucion del programa
  }
}

void loop() {
  DateTime fecha = rtc.now();

  /*Serial.print(fecha.year());
  Serial.print(fecha.month());
  Serial.println(fecha.day());*/

  zellerDia = zeller(fecha.year(), fecha.month(), fecha.day());

  Serial.println(zellerDia);
  delay(1000);

/*  Congruencia de Zeller Tabla
  0=Domingo;1=Lunes;2=Martes
*/
  // MODO MANUAL//
  if (digitalRead(select_man) == LOW) {
    modo_manual();
  }

  // MODO AUTOMATICO//
  if (digitalRead(select_aut) == LOW and ) { 
    modo_auto();
  }

  lcd.clear();
}

void modo_manual() {
  lcd.setCursor(0, 0);
  lcd.print("Modo Manual      ");
}

void modo_auto() {



  lcd.setCursor(0, 0);
  lcd.print("Modo Automatico   ");

  if (digitalRead(signal_luz) == LOW and digitalRead(signal_planta) == HIGH) {  //si no hay luz, la planta esta apagada y
    Serial.println("Modo Automatico");

    for (int i = 0; i < 3; i++) {  //Inteneta arrancar la planta 3veces

      digitalWrite(arranque, HIGH);
      delay(1500);
      digitalWrite(arranque, LOW);
      delay(1500);
      Serial.print("Intentos de Arranque");
      Serial.println(i);

      if (digitalRead(signal_planta) == LOW) {  //si arranca deja de hacer más intentos
        digitalWrite(arranque, LOW);
        Serial.println("Planta encendida");
        break;
      }

      else {  //si no arranca manda un mensaje de error y espera que alguien pre
        while (true) {
          lcd.setCursor(0, 0);
          lcd.print("Error");
          if (digitalRead(signal_planta) == LOW) {
            lcd.setCursor(0, 0);
            lcd.print("Error");
            break;
          }
        }
      }
    }
  }

  if (digitalRead(signal_luz) == HIGH and digitalRead(signal_planta) == LOW) {  //si llega la luz y la plnata este encendida apaga la plnata
    digitalWrite(planta_off, HIGH);
    delay(2000);
    digitalWrite(planta_off, LOW);
  }
}

// congrunecia de zeller(los numros del mes los pasa a dias de la semema (lunes, martes....) por medio de un algoritmo)
int zeller(int ano, int mes, int dia) {
  int a = (14 - mes) / 12;
  int y = ano - a;
  int m = mes + 12 * a - 2;

  int d = ((dia + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7);

  return d;//retorrna un numero que equivale a un dia de la semana ejem(1=Lunes, 2=Martes.....)
}