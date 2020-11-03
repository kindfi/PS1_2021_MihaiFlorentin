#include <LiquidCrystal.h>
#include "DHT.h"
#define DHTPIN A1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
int led = 3;

byte degree[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};

const int pin_BL = 10;
float t = 0;
LiquidCrystal lcd( 8,  9,  4,  5,  6,  7);
int x = 0;
enum Buttons {
  EV_OK,
  EV_CANCEL,
  EV_NEXT,
  EV_PREV,
  EV_NONE,
  EV_MAX_NUM
};


enum Menus {
  MENU_MAIN = 0,
  MENU_KP,
  MENU_TEMP,
  MENU_ACTAUL,
  MENU_MAX_NUM
};








double temp = 30;
double kp = 20;
float temp_q = 0;
Menus scroll_menu = MENU_MAIN;
Menus current_menu =  MENU_MAIN;

void state_machine(enum Menus menu, enum Buttons button);
Buttons GetButtons(void);
void print_menu(enum Menus menu);

typedef void (state_machine_handler_t)(void);

void print_menu(enum Menus menu)
{
  lcd.clear();
  switch (menu)
  {
    case MENU_KP:
      lcd.print("KP = ");
      lcd.print(kp);
      break;
    case MENU_TEMP:
      lcd.print("TEMP = ");
      lcd.print(temp);

      break;
    case MENU_ACTAUL:
      lcd.setCursor(0, 0);
      lcd.print("Actual: ");
      lcd.print(t);
      lcd.write(byte(0));
      lcd.print("C");

      break;
    case MENU_MAIN:
    default:
      lcd.print("PS 2020");
      break;
  }
  if (current_menu != MENU_MAIN)
  {
    lcd.setCursor(0, 1);
    lcd.print("                         ");
    lcd.setCursor(0, 1);

    lcd.print("modifica");
  }
}

void enter_menu(void)
{
  current_menu = scroll_menu;
}

void go_home(void)
{
  scroll_menu = MENU_MAIN;
  current_menu = scroll_menu;
}

void go_next(void)
{
  scroll_menu = (Menus) ((int)scroll_menu + 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void go_prev(void)
{
  scroll_menu = (Menus) ((int)scroll_menu - 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void save_kp(void)
{
}

void inc_kp(void)
{
  kp++;
}

void dec_kp(void)
{
  kp--;
}

void save_temp(void)
{
}

void inc_temp(void)
{
  temp = temp + 15;
}

void dec_temp(void)
{
  temp = temp - 15;
}


state_machine_handler_t* sm[MENU_MAX_NUM][EV_MAX_NUM] =
{ //events: OK , CANCEL , NEXT, PREV
  {enter_menu, go_home, go_next, go_prev},  // MENU_MAIN
  {go_home, go_home, inc_kp, dec_kp},       // MENU_KP
  {go_home, go_home, inc_temp, dec_temp},   // MENU_TEMP
  {go_home, go_home, go_home, go_home},   // MENU_TEMP
};

void state_machine(enum Menus menu, enum Buttons button)
{
  sm[menu][button]();
}

Buttons GetButtons(void)
{
  enum Buttons ret_val = EV_NONE;
  if (x < 60)
  {
    ret_val = EV_OK;

  }
  else if (x < 200)
  {
    ret_val = EV_NEXT;
  }
  else if (x < 400)
  {
    ret_val = EV_PREV;
  }
  else if (x < 600)
  {
    ret_val = EV_CANCEL;
  }
  else if (x < 800)
  {
    ret_val = EV_OK;
  }

  Serial.print(ret_val);
  return ret_val;
}

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  lcd.createChar(0, degree);

}

void loop()
{
  x = analogRead (0);
  analogWrite(led, temp);
  t = dht.readTemperature();
  volatile Buttons event = GetButtons();
  if (event != EV_NONE)
  {
    state_machine(current_menu, event);
  }
  print_menu(scroll_menu);
  delay(500);
}
