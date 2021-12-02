//Librerias

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

/*-------------------------------TECLADO---------------------------------------*/

const byte numRows = 4; //numero de filas del teclado
const byte numCols = 4; //numero de columnas del teclado
char keypressed;
char keymap[numRows][numCols] =
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
//Codigo que muestra la conexion del teclado hacia las terminales del arduino
byte rowPins[numRows] = {A5, A4, A3, A2}; //filas 0 al 3
byte colPins[numCols] = {13, 12, 11, 10}; //columnas 0 al 3             
//Inicializamos la instancia de la clase del teclado
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

/*-------------------------------CONSTANTES------------------------------------*/

LiquidCrystal lcd(8, 7, 6, 5, 4, 3); //LCD
const int buzzer = A1; //Buzzer/speaker pequeño
const int lock = 9; //Cerradura eléctrica

/*-------------------------------VARIABLES------------------------------------*/

String password = "2580"; //Variable que almacena la contraseña actual
int PW; //Almacenar la contraseña.
String tempPassword = ""; //Variable que almacena el input de la contraseña
int doublecheck; //Verificar dos  veces la nueva contraseña
boolean armed = false; //Variable para el estado del sistema (armed:true / unarmed:false)
boolean input_pass; //Variable para la entrada del sistema (correct:true / wrong:false)
boolean storedPassword = true;
boolean changedPassword = false;
boolean checkPassword = false;
int i = 1; //Variabe del index del array

/*----------------------------------------------------------------------------*/

void setup(){
  pinMode(lock, OUTPUT);
  lcd.begin(16, 2); //Inicializar el numero de filas y columnas del LCD 
  //Imprimir mensaje de bienvenida
  lcd.setCursor(0, 0);
  lcd.print("Sistema");
  lcd.setCursor(0, 1);
  lcd.print("  de seguridad  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Abrir sistema ");
  lcd.setCursor(0, 1);
  lcd.print("Digitar pin:");
  delay(2000);
  lcd.clear();
  EEPROM.get(0, PW); //Almacenar la contraseña en la variable PW
  if(PW != password.toInt() && PW>0){
    password = String(PW); //Seteamos la contraseña almacenada en el EEPROM en la variable password
  }
}

void loop() { //Loop principal
  unlockTheDoor();
}

/********************************FUNCTIONS*************************************/

void unlockTheDoor(){
  lockAgain: //goto label
  tempPassword = "";
  lcd.clear();
  i = 6;
  noTone(buzzer);
  digitalWrite(lock, LOW);
  while(!checkPassword){
    lcd.setCursor(0, 0);
    lcd.print("Abrir puerta:");
    lcd.setCursor(0, 1);
    lcd.print("PIN:");
    keypressed = myKeypad.getKey(); //Leer tecla presionada
    if (keypressed != NO_KEY){ //Aceptar solo numeros y * del teclado
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i, 1);
        lcd.print("*"); //Escribir * en el LCD
        i++;
        tone(buzzer, 800, 200); //Tono del boton
      }else if (keypressed == 'A'){
        changePassword();
        goto lockAgain;
      }else if (keypressed == '#'){
        break;
      }else if (keypressed == '*'){ //Revisar la contraseña
        if (password == tempPassword){ //Si es correcta...
          lcd.clear();      
          lcd.setCursor(0, 0);
          lcd.print("Correcto...");
          lcd.setCursor(0, 1);
          lcd.print("Abierto");
          tone(buzzer, 100); //Reproducir un sonido mientras la cerradura esta abiera
          digitalWrite(lock, HIGH); //Desbloquear la puerta por 5 segundos
          delay(5000);
          goto lockAgain;
        }else{ //Si es falso reintentar
          tempPassword = "";
          tone(buzzer, 500, 200);
          delay(300);
          tone(buzzer, 500, 200);
          delay(300);
          goto lockAgain;
        }
      }
    }
  }
}

//Cambiar la contraseña actual
void changePassword(){
  retry: //goto label
  tempPassword = "";
  lcd.clear();
  i = 1;
  while(!changedPassword){ //Esperando la contraseña actual
    keypressed = myKeypad.getKey(); //Leer tecla presionada
    lcd.setCursor(0, 0);
    lcd.print("Pin actual");
    lcd.setCursor(0, 1);
    lcd.print(":");
    if (keypressed != NO_KEY){
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i, 1);
        lcd.print("*");
        i++;
        tone(buzzer, 800, 200);       
      }else if (keypressed == '#'){
        break;
      }else if (keypressed == '*'){
        i = 1;
        if (password == tempPassword){
          storedPassword = false;
          tone(buzzer, 500, 200);
          newPassword(); //Contraseña correcta, llamar metodo de nueva contraseña
          break;
        }else{ //Intentar denuevo
          tempPassword = "";
          tone(buzzer, 500, 200);
          delay(300);
          tone(buzzer, 500, 200);
          delay(300);
          goto retry;
        }
      }
    }
  }
}

//Configurar nueva contraseña
String firstpass;
void newPassword(){
  tempPassword = "";
  changedPassword = false;
  lcd.clear();
  i=1;
  while(!storedPassword){
    keypressed = myKeypad.getKey(); //Leer presionada
    if (doublecheck == 0){
      lcd.setCursor(0,0);
      lcd.print("Nuevo pin");
      lcd.setCursor(0,1);
      lcd.print(":");
    }else{
      lcd.setCursor(0,0);
      lcd.print("De nuevo...");
      lcd.setCursor(0, 1);
      lcd.print(":");
    }
    if (keypressed != NO_KEY){
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i, 1);
        lcd.print("*");
        i++;
        tone(buzzer, 800, 200);
      }else if (keypressed == '#'){
        break;
      }else if (keypressed == '*'){
        if (doublecheck == 0){
          firstpass = tempPassword;
          doublecheck = 1;
          newPassword();
        }
        if (doublecheck == 1){
          doublecheck = 0;
          if (firstpass == tempPassword){
            i = 1;
            firstpass = "";
            password = tempPassword; //Nueva contraseña guardada
            tempPassword = ""; //Borrar la contraseña temporal
            PW = password.toInt();
            EEPROM.put( 0, PW );
            lcd.setCursor(0, 0);
            lcd.print("Cambio correcto");
            lcd.setCursor(0, 1);
            lcd.print("----------------");
            storedPassword = true;
            tone(buzzer, 500, 400);
            delay(2000);
            lcd.clear();
            break;
          }else{
            firstpass = "";
            newPassword();
          }
        }
      } 
    }
  }
}
