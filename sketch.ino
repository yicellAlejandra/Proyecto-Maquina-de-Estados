/*!
\file   statemachine.ino
\date   2022-08-02
\author Alejandra Troyano <ytroyano@unicauca.edu.co>
\author Mary Montenegro <marymontenegro@unicauca.edu.co>
\author Liseth Rivera <lisethrivera@unicauca.edu.co>
\brief  State Machine on temperature control.

\par Copyright
Information contained herein is proprietary to and constitutes valuable
confidential trade secrets of Unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) Unicauca 2023. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
**************************
*/


/***************//*
 *  Libraries included
 *****************/
#include "StateMachineLib.h"
/***************//*
 *  implements a state machine that can run on a processor such as Arduino.
 *****************/ 
#include "AsyncTaskLib.h"
/***************//*
 *  implements a task that is executed after a certain period of microseconds.
 *****************/ 
#include "DHTStable.h"
/***************//*
 *  include functions for initializing sensors, reading sensor data,
    and handling sensor-related errors or issues.
 *****************/ 
#include <LiquidCrystal.h>
/***************//*
 *  includes functions for initializing LCD displays,
    printing text, controlling cursor positions, and managing display settings.
 *****************/ 
#include <Keypad.h>
/***************//*
 * Enable reading input from keypad devices and translating keypresses into usable data.
 *****************/ 


/***************//*
 *  Pin Definitions
 *****************/
#define LED_RED 10
#define LED_GREEN 8
#define LED_BLUE 9
#define BUZZER 7

/***************//*
 *  Temperature and Light sensor pins
 *****************/

const int TemperaturePin = A1;
#define beta 4090
#define resistencia 10

const int PHOTOCELL_PIN = A0;
#define ledPin = 13

// DHT es una instancia de la clase DHTStable
DHTStable DHT;

/***************//*
 *  Global Variables Declaration
 *****************/

char password[4] = {"4321"};
char inPassword[4];
int count = 0;
int trycount = 0;
const uint8_t ROWS = 4;
const uint8_t COLS = 4;
int luz = 0.0;
float temp = 0.0;
long a = 0;
boolean estado;

// Definición del teclado (keypad) y su configuración
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

uint8_t colPins[COLS] = {32, 34, 36, 38}; // Pines conectados a C1, C2, C3, C4
uint8_t rowPins[ROWS] = {24, 26, 28, 30}; // Pines conectados a R1, R2, R3, R4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configuración de la pantalla LCD (LiquidCrystal)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// Creación de una nueva instancia de la máquina de estados
StateMachine stateMachine(6, 10);

/***************//*
 *  Asynchronous Tasks for sensor data updates
 *****************/


/***************//*
 *  TimeOut Function
 *****************/

/**
 * @brief Function executed upon timeout completion in the AsyncTask instances.
 * ----------------------------------------------------------------------------
 * @details This function is invoked when the timeout period specified in the
 * AsyncTask instances (espera3s, espera4s, espera5s, espera6s, espera10s) elapses.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle actions or behaviors triggered upon the completion of a timeout
 *   period in the AsyncTask instances.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with asynchronous tasks' timeout completion and
 *   may execute specific actions, change states, or update variables accordingly.
 */

 // Creación de tareas asincrónicas para actualizar los datos de temperatura, humedad y luz en la pantalla LCD
void TimeOut(void);
AsyncTask espera1s(1000, false, TimeOut);
AsyncTask espera2s(2000, false, TimeOut);
AsyncTask espera3s(3000, false, TimeOut);
AsyncTask espera4s(4000, false, TimeOut);
AsyncTask espera5s(5000, false, TimeOut);
AsyncTask espera10s(10000, false, TimeOut);

/***************//*
 *  SisSeguridad Function
 *****************/

/**
 * @brief Function handling system security checks and user input processing.
 * ----------------------------------------------------------------------------
 * @details This function manages the system security, processes user inputs,
 * and verifies security credentials (e.g., password or access code).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage security-related operations such as processing user input to verify
 *   security credentials or perform system checks.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskSisSeguridad AsyncTask instance,
 *   where it handles security-related tasks, user inputs, and security checks.
 */
 void SisSeguridad(void);
AsyncTask TaskSisSeguridad(0, true, SisSeguridad);

/***************//*
 *  Bloqueo Function
 *****************/

/**
 * @brief Function handling system locking and timeout operations.
 * ----------------------------------------------------------------------------
 * @details This function manages the system locking mechanism and operations
 * triggered by timeout events.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle system locking and timeout-related operations when specific
 *   conditions, such as timeout events, occur.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskBloqueo AsyncTask instance and
 *   manages system locking and timeout events.
 */


void Bloqueo(void);
AsyncTask TaskBloqueo(0, true, Bloqueo);

/***************//*
 *  monitoreoTemperatura Function
 *****************/

/**
 * @brief Function responsible for monitoring temperature readings.
 * ----------------------------------------------------------------------------
 * @details This function monitors and handles temperature readings obtained
 * from sensors or specific input sources.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage and process temperature readings, performing actions based on
 *   the obtained temperature data.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskMonitoreoTemperatura AsyncTask
 *   instance and is responsible for monitoring temperature readings.
 */

void monitoreoTemperatura(void);
AsyncTask TaskMonitoreoTemperatura(500, true, monitoreoTemperatura);

/***************//*
 *  monitoreoLuz Function
 *****************/

/**
 * @brief Function responsible for monitoring light intensity.
 * ----------------------------------------------------------------------------
 * @details This function monitors and manages the light intensity readings
 * obtained from sensors or specific input sources.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle and process light intensity readings, performing actions based
 *   on the obtained light data.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskMonitoreoLuz AsyncTask instance
 *   and is responsible for monitoring light intensity readings.
 */

void monitoreoLuz(void);
AsyncTask TaskMonitoreoLuz(500, true, monitoreoLuz);

/***************//*
 *  AlertaLuz Function
 *****************/

/**
 * @brief Function responsible for handling low light intensity alerts.
 * ----------------------------------------------------------------------------
 * @details This function manages the process of alerting or responding to low
 * light intensity situations, performing specific actions or triggering alerts.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle low light intensity alerts and respond accordingly.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskAlertaLuz AsyncTask instance and
 *   is responsible for handling low light intensity alert scenarios.
 */

void AlertaLuz(void);
AsyncTask TaskAlertaLuz(0, true, AlertaLuz);

/***************//*
 *  AlertaTemp Function
 *****************/

/**
 * @brief Function responsible for handling high temperature alerts.
 * ----------------------------------------------------------------------------
 * @details This function manages the process of alerting or responding to high
 * temperature situations, performing specific actions or triggering alerts.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle high temperature alerts and respond accordingly.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is associated with the TaskAlertaTemp AsyncTask instance and
 *   is responsible for handling high temperature alert scenarios.
 */

void AlertaTemp(void);
AsyncTask TaskAlertaTemp(0, true, AlertaTemp);

/***************//*
 *  Enumerations for States and Inputs
 *****************/

// Enumeración para representar los estados del programa
enum State
{
    Init = 0,                 // Estado de inicio de sesión
    BLoqueo = 1,
    MonitoreoLuz = 2,         // Estado de monitoreo luz
    MonitoreoTemperatura = 3, // Estado de monitoreo Temperatura
    AlertaLed = 4,            // Estado de alerta de seguridad
    AlertaBuzzer = 5
};

// Enumeración para representar las entradas del programa
enum Input
{
  SisBloqueado = 0,
  clvCorrecta = 1, // Entrada para una clave correcta
  LuzBaja = 2,     // Entrada de luz menor a 40
  temp30 = 3,      // Entrada de temperatura mayor a 30°C
  timeOut = 4,     // Entrada de tiempo de espera
  Unknown = 5      // Entrada desconocida
};

Input currentInput = Input::Unknown;

/***************//*
 *  State Machine Configuration
 *****************/

void setupStateMachine()
{
    // Agregar transiciones
    stateMachine.AddTransition(Init, Bloqueo, []() { return currentInput == SisBloqueado; });

    stateMachine.AddTransition(Init, Bloqueo, []() { return currentInput == timeOut; });

    stateMachine.AddTransition(Init, MonitoreoLuz, []() { return currentInput == clvCorrecta; });

    stateMachine.AddTransition(Bloqueo, Init, []() { return currentInput == timeOut; });

    stateMachine.AddTransition(MonitoreoLuz, MonitoreoTemperatura, []() { return currentInput == timeOut; });

    stateMachine.AddTransition(MonitoreoLuz, AlertaLed, []() { return currentInput == LuzBaja; });

    stateMachine.AddTransition(MonitoreoTemperatura, MonitoreoLuz, []() { return currentInput == timeOut; });

    stateMachine.AddTransition(MonitoreoTemperatura, AlertaBuzzer, []() { return currentInput == temp30; });

    stateMachine.AddTransition(AlertaLed, MonitoreoLuz, []() { return currentInput == timeOut; });

    stateMachine.AddTransition(AlertaBuzzer, MonitoreoTemperatura, []() { return currentInput == timeOut; });


    // Asignar acciones
    stateMachine.SetOnEntering(Init, outputInit);
    stateMachine.SetOnLeaving(Init, onLeavingInit);
    stateMachine.SetOnEntering(Bloqueo, outputBloqueo);
    stateMachine.SetOnLeaving(Bloqueo, onLeavingBloqueo);
    stateMachine.SetOnEntering(MonitoreoLuz, outputMonitoreoLuz);
    stateMachine.SetOnLeaving(MonitoreoLuz, onLeavingMonitoreoLuz);
    stateMachine.SetOnEntering(MonitoreoTemperatura, outputMonitoreoTemperatura);
    stateMachine.SetOnLeaving(MonitoreoTemperatura, onLeavingMonitoreoTemperatura);
    stateMachine.SetOnEntering(AlertaLed, outputAlertaLed);
    stateMachine.SetOnLeaving(AlertaLed, onLeavingAlertaLed);
    stateMachine.SetOnEntering(AlertaBuzzer, outputAlertaBuzzer);
    stateMachine.SetOnLeaving(AlertaBuzzer, onLeavingAlertaBuzzer);
}

/***************//*
 *  ClaveCorrecta Function
 *****************/

/**
 * @brief Function to handle successful password input.
 * ----------------------------------------------------------------------------
 * @details This function manages the actions and visual indications upon
 * successful password entry, including UI updates and auditory feedback.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle and indicate successful password input through LEDs, LCD, and
 *   audible feedback.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is responsible for resetting the try count, displaying a
 *   successful message on the LCD screen, activating LEDs, and producing
 *   confirmation tones via the buzzer.
 * - It includes visual and auditory cues indicating the correct password
 *   entry.
 */


void claveCorrecta(){
  trycount=0;
  digitalWrite(LED_GREEN, HIGH);
  lcd.clear();
  lcd.print("Clave");
  lcd.setCursor(0, 1);
  lcd.print("Correcta");

  // Reproducir tono de confirmación
  tone(BUZZER, 1800, 100);
  delay(100);
  tone(BUZZER, 2000, 100);
  delay(100);
  tone(BUZZER, 2200, 100);
  delay(100);
  tone(BUZZER, 2500, 350);
  delay(350);
  noTone(BUZZER);

  delay(1000);
  digitalWrite(LED_RED, LOW);
  lcd.clear();
}

/***************//*
 *  ClaveIncorrecta Function
 *****************/

/**
 * @brief Function to handle incorrect password entries.
 * ----------------------------------------------------------------------------
 * @details This function manages the actions and visual indications upon an
 * incorrect password entry, including UI updates and auditory feedback.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle and indicate an incorrect password entry through LEDs, LCD,
 *   and audible feedback.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function is responsible for incrementing the try count, displaying
 *   an incorrect message on the LCD screen, activating an LED, and producing
 *   error tones via the buzzer.
 * - It includes visual and auditory cues indicating an incorrect password
 *   entry.
 */

void claveIncorrecta()
{
    digitalWrite(LED_BLUE, HIGH);
    lcd.print("Clave");
    lcd.setCursor(0, 1);
    lcd.print("Incorrecta");
    trycount++;

    // Reproducir tono de error
    tone(BUZZER, 1000, 200);
    delay(200);
    noTone(BUZZER);

    delay(1000);
    digitalWrite(LED_BLUE, LOW);
    lcd.clear();
    lcd.print("Ingrese Clave:");
    lcd.setCursor(0, 1);
}

/***************//*
 *  compararPalabras Function
 *****************/

/**
 * @brief Function to compare two strings character by character.
 * ----------------------------------------------------------------------------
 * @details This function compares two arrays of characters (strings)
 * character-by-character to determine if they are equal.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To compare two strings and check for equality between them.
 * ----------------------------------------------------------------------------
 * @param longitud Length of the strings to be compared.
 * @param pal1 First string to compare.
 * @param pal2 Second string to compare.
 * @return bool True if the strings are equal; otherwise, false.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - This function iterates through the characters of the strings to check for
 *   equality and returns a boolean indicating whether they are the same.
 */
bool compararPalabras(int longitud, char pal1[], char pal2[]) {
  int igualdad = 0;
  for (int i = 0; i < longitud; i++) {
    if (pal1[i] != pal2[i]) {
      igualdad = 1;
      break;
    }
  }

  if (igualdad == 0)
    return true;
  else
    return false;
}

/***************//*
 *  TimeOut Function
 *****************/

/**
 * @brief Function to handle timeout event.
 * ----------------------------------------------------------------------------
 * @details This function is invoked upon a timeout event to update the 
 * currentInput variable to indicate a timeout occurrence.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the timeout event.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - The function updates the global variable currentInput to represent a 
 *   timeout occurrence.
 */

void TimeOut(void) {
  currentInput = timeOut;
}

/***************//*
 *  Setup Function
 *****************/

/**
 * @brief Function to initialize the system setup.
 * ----------------------------------------------------------------------------
 * @details This function initializes the serial port, configures pins for LEDs 
 * (indicating system status), sets up the state machine, sets the initial state 
 * of the system, and initializes the LCD screen.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To initialize system components and setup state machine.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Initializes serial communication at 9600 baud rate.
 * - Configures LED pins (LED_RED, LED_BLUE, LED_GREEN, BUZZER) as OUTPUT.
 * - Invokes setupStateMachine() to configure the state machine.
 * - Sets the initial state of the system to "Init" using stateMachine.SetState().
 * - Initializes the LCD screen with 16 columns and 2 rows using lcd.begin(16, 2).
 */

void setup()
{
     // Inicialización del puerto serie
    Serial.begin(9600);

    // Configuración de pines
    pinMode(LED_RED, OUTPUT);   // LED indicador de sistema bloqueado
    pinMode(LED_BLUE, OUTPUT);  // LED indicador de clave incorrecta
    pinMode(LED_GREEN, OUTPUT); // LED indicador de clave correcta
    pinMode(BUZZER, OUTPUT);    // Buzzer
    // Configurar la máquina de estados
    setupStateMachine();

    // Estado inicial
    stateMachine.SetState(Init, false, true);

    // Inicializar pantalla LCD
    lcd.begin(16, 2);
    lcd.print("Bienvenido!"); // Mostrar mensaje de bienvenida al encender
    delay(2000); // Mostrar el mensaje de bienvenida por 2 segundos
    lcd.clear();
    lcd.print("Ingrese la clave");
    lcd.setCursor(0, 1);
}

/***************//*
 *  Loop Function
 *****************/

/**
 * @brief Function to run repetitive tasks in the system.
 * ----------------------------------------------------------------------------
 * @details This function updates asynchronous tasks, such as timers and 
 * security-related tasks, along with the state machine of the system.
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To handle the execution of asynchronous tasks and update the state machine.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Updates asynchronous tasks (espera3s, espera4s, espera5s, espera6s, espera10s,
 *   TaskSisSeguridad, TaskBloqueo, TaskMonitoreoTemperatura, TaskMonitoreoLuz,
 *   TaskAlertaLuz, TaskAlertaTemp) by invoking their respective Update() methods.
 * - Updates the state machine (stateMachine) by calling stateMachine.Update().
 * - Resets the currentInput to 'Unknown' for the next state transition.
 */

void loop()
{
   //Actualizar Tareas Asincronicas
  espera1s.Update();
  espera2s.Update();
  espera3s.Update();
  espera4s.Update();
  espera5s.Update();
  espera10s.Update();

  TaskSisSeguridad.Update();
  TaskBloqueo.Update();
  TaskMonitoreoTemperatura.Update();
  TaskMonitoreoLuz.Update();
  TaskAlertaLuz.Update();
  TaskAlertaTemp.Update();

  // Actualizar la máquina de estados
  stateMachine.Update();
  currentInput =Unknown;
}

/***************//*
 *  Output Initialization Function
 *****************/

/**
 * @brief Function to handle initialization output state.
 * ----------------------------------------------------------------------------
 * @details This function clears the LCD screen and prompts the user to enter 
 * the password. It starts a 10-second timer (espera10s) and initiates the 
 * security system task (TaskSisSeguridad).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To initialize the output state by prompting the user to enter the password 
 *   on the LCD screen and initiating relevant system tasks.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Clears the LCD screen using lcd.clear().
 * - Prints "Ingrese la" on the LCD screen at position (0, 0) and "contrasenia" 
 *   at position (0, 1).
 * - Starts a 10-second timer (espera10s) and begins the security system task 
 *   (TaskSisSeguridad) by calling their respective Start() methods.
 */

void outputInit()
{
  lcd.clear();
  lcd.print("Bienvenido!");
  delay(2000); // Mostrar el mensaje de bienvenida por 2 segundos
  lcd.clear();
  lcd.print("Ingrese la clave");
  lcd.setCursor(0, 1);

  // Aumento del tiempo para ingresar la contraseña
  espera10s.Start();
  TaskSisSeguridad.Start();
}

/***************//*
 *  Output Blocking Function
 *****************/

/**
 * @brief Function to handle output when the system is blocked.
 * ----------------------------------------------------------------------------
 * @details This function stops the 10-second timer (espera10s) and halts the 
 * security system task (TaskSisSeguridad). It starts a 5-second timer 
 * (espera5s) and triggers the blocking task (TaskBloqueo).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the output state when the system is blocked.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 10-second timer (espera10s) and halts the security system task 
 *   (TaskSisSeguridad) by calling their respective Stop() methods.
 * - Initiates a 5-second timer (espera5s) and triggers the blocking task 
 *   (TaskBloqueo) by calling their respective Start() methods.
 */


void outputBloqueo()
{
  espera10s.Stop();
  TaskSisSeguridad.Stop();

  espera5s.Start();
  TaskBloqueo.Start();
}

/***************//*
 *  Output Light Monitoring Function
 *****************/

/**
 * @brief Function to handle output during light monitoring.
 * ----------------------------------------------------------------------------
 * @details This function stops the 10-second timer (espera10s) and halts the 
 * security system task (TaskSisSeguridad). It stops both temperature 
 * monitoring (TaskMonitoreoTemperatura) and light alert (TaskAlertaLuz) tasks. 
 * It then starts a 3-second timer (espera3s) and triggers the light monitoring 
 * task (TaskMonitoreoLuz).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the output state during light monitoring.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 10-second timer (espera10s) and halts the security system task 
 *   (TaskSisSeguridad) by calling their respective Stop() methods.
 * - Stops both the temperature monitoring task (TaskMonitoreoTemperatura) and 
 *   light alert task (TaskAlertaLuz) by calling their respective Stop() methods.
 * - Initiates a 3-second timer (espera3s) and triggers the light monitoring 
 *   task (TaskMonitoreoLuz) by calling their respective Start() methods.
 */

void outputMonitoreoLuz()
{
  espera10s.Stop();
  TaskSisSeguridad.Stop();

  TaskMonitoreoTemperatura.Stop();
  TaskAlertaLuz.Stop();

  espera3s.Start();
  TaskMonitoreoLuz.Start();
}

/***************//*
 *  Output Temperature Monitoring Function
 *****************/

/**
 * @brief Function to handle output during temperature monitoring.
 * ----------------------------------------------------------------------------
 * @details This function stops the light monitoring task (TaskMonitoreoLuz) 
 * and the temperature alert task (TaskAlertaTemp). Then, it starts a 4-second 
 * timer (espera4s) and triggers the temperature monitoring task 
 * (TaskMonitoreoTemperatura).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the output state during temperature monitoring.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the light monitoring task (TaskMonitoreoLuz) and the temperature 
 *   alert task (TaskAlertaTemp) by calling their respective Stop() methods.
 * - Initiates a 4-second timer (espera4s) and triggers the temperature 
 *   monitoring task (TaskMonitoreoTemperatura) by calling their respective 
 *   Start() methods.
 */

void outputMonitoreoTemperatura()
{
  TaskMonitoreoLuz.Stop();
  TaskAlertaTemp.Stop();

  espera4s.Start();
  TaskMonitoreoTemperatura.Start();

}

/***************//*
 *  Output Alert LED Function
 *****************/

/**
 * @brief Function to manage output during LED alert.
 * ----------------------------------------------------------------------------
 * @details This function stops the 4-second timer (espera4s) and the light 
 * monitoring task (TaskMonitoreoLuz). Then, it starts a new 4-second timer 
 * (espera4s) and triggers the light alert task (TaskAlertaLuz).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the output state during an LED alert.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 4-second timer (espera4s) and the light monitoring task 
 *   (TaskMonitoreoLuz) by calling their respective Stop() methods.
 * - Initiates a new 4-second timer (espera4s) and triggers the light 
 *   alert task (TaskAlertaLuz) by calling their respective Start() methods.
 */

void outputAlertaLed()
{
  espera4s.Stop();
  TaskMonitoreoLuz.Stop();

  espera4s.Start();
  TaskAlertaLuz.Start();
}

/***************//*
 *  Output Alert Buzzer Function
 *****************/

/**
 * @brief Function to manage output during buzzer alert.
 * ----------------------------------------------------------------------------
 * @details This function stops the 5-second timer (espera5s) and the 
 * temperature monitoring task (TaskMonitoreoTemperatura). Then, it starts a 
 * new 5-second timer (espera5s) and triggers the temperature alert task 
 * (TaskAlertaTemp).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To manage the output state during a buzzer alert.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 5-second timer (espera5s) and the temperature monitoring 
 *   task (TaskMonitoreoTemperatura) by calling their respective Stop() methods.
 * - Initiates a new 5-second timer (espera5s) and triggers the temperature 
 *   alert task (TaskAlertaTemp) by calling their respective Start() methods.
 */

void outputAlertaBuzzer()
{
  espera5s.Stop();
  TaskMonitoreoTemperatura.Stop();

  espera5s.Start();
  TaskAlertaTemp.Start();
}

/***************//*
 *  On Leaving Initialization Function
 *****************/

/**
 * @brief Function executed upon leaving the initialization state.
 * ----------------------------------------------------------------------------
 * @details This function stops the 10-second timer (espera10s) and halts the 
 * system security task (TaskSisSeguridad).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the initialization state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 10-second timer (espera10s) by calling its Stop() method.
 * - Halts the system security task (TaskSisSeguridad) by calling its Stop() method.
 */

void onLeavingInit() {
    espera10s.Stop();
    TaskSisSeguridad.Stop();
}

/***************//*
 *  On Leaving Bloqueo Function
 *****************/

/**
 * @brief Function executed upon leaving the Bloqueo state.
 * ----------------------------------------------------------------------------
 * @details This function stops the 10-second timer (espera10s), halts the 
 * system security task (TaskSisSeguridad), stops the 5-second timer (espera5s),
 * and stops the Bloqueo task (TaskBloqueo).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the Bloqueo state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 10-second timer (espera10s) by calling its Stop() method.
 * - Halts the system security task (TaskSisSeguridad) by calling its Stop() method.
 * - Stops the 5-second timer (espera5s) by calling its Stop() method.
 * - Stops the Bloqueo task (TaskBloqueo) by calling its Stop() method.
 */

void onLeavingBloqueo() {
    espera10s.Stop();
    TaskSisSeguridad.Stop();
    espera5s.Stop();
    TaskBloqueo.Stop();
}

/***************//*
 *  On Leaving MonitoreoLuz Function
 *****************/

/**
 * @brief Function executed upon leaving the MonitoreoLuz state.
 * ----------------------------------------------------------------------------
 * @details This function stops the 10-second timer (espera10s), halts the 
 * system security task (TaskSisSeguridad), stops the temperature monitoring task 
 * (TaskMonitoreoTemperatura), stops the light alert task (TaskAlertaLuz), stops 
 * the 3-second timer (espera3s), and stops the light monitoring task (TaskMonitoreoLuz).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the MonitoreoLuz state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 10-second timer (espera10s) by calling its Stop() method.
 * - Halts the system security task (TaskSisSeguridad) by calling its Stop() method.
 * - Stops the temperature monitoring task (TaskMonitoreoTemperatura) by calling its Stop() method.
 * - Stops the light alert task (TaskAlertaLuz) by calling its Stop() method.
 * - Stops the 3-second timer (espera3s) by calling its Stop() method.
 * - Stops the light monitoring task (TaskMonitoreoLuz) by calling its Stop() method.
 */

void onLeavingMonitoreoLuz() {
    espera10s.Stop();
    TaskSisSeguridad.Stop();
    TaskMonitoreoTemperatura.Stop();
    TaskAlertaLuz.Stop();
    espera3s.Stop();
    TaskMonitoreoLuz.Stop();
}

/***************//*
 *  On Leaving MonitoreoTemperatura Function
 *****************/

/**
 * @brief Function executed upon leaving the MonitoreoTemperatura state.
 * ----------------------------------------------------------------------------
 * @details This function stops the light monitoring task (TaskMonitoreoLuz), 
 * halts the temperature alert task (TaskAlertaTemp), stops the 4-second timer (espera4s), 
 * and stops the temperature monitoring task (TaskMonitoreoTemperatura).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the MonitoreoTemperatura state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the light monitoring task (TaskMonitoreoLuz) by calling its Stop() method.
 * - Halts the temperature alert task (TaskAlertaTemp) by calling its Stop() method.
 * - Stops the 4-second timer (espera4s) by calling its Stop() method.
 * - Stops the temperature monitoring task (TaskMonitoreoTemperatura) by calling its Stop() method.
 */

void onLeavingMonitoreoTemperatura() {
    TaskMonitoreoLuz.Stop();
    TaskAlertaTemp.Stop();
    espera4s.Stop();
    TaskMonitoreoTemperatura.Stop();
}

/***************//*
 *  On Leaving AlertaLed Function
 *****************/

/**
 * @brief Function executed upon leaving the AlertaLed state.
 * ----------------------------------------------------------------------------
 * @details This function stops the 4-second timer (espera4s), halts the light monitoring 
 * task (TaskMonitoreoLuz), stops the 4-second timer again (espera4s), and stops 
 * the light alert task (TaskAlertaLuz).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the AlertaLed state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 4-second timer (espera4s) by calling its Stop() method.
 * - Halts the light monitoring task (TaskMonitoreoLuz) by calling its Stop() method.
 * - Stops the 4-second timer again (espera4s) by calling its Stop() method.
 * - Stops the light alert task (TaskAlertaLuz) by calling its Stop() method.
 */

void onLeavingAlertaLed() {
    espera4s.Stop();
    TaskMonitoreoLuz.Stop();
    espera4s.Stop();
    TaskAlertaLuz.Stop();
}

/***************//*
 *  On Leaving AlertaBuzzer Function
 *****************/

/**
 * @brief Function executed upon leaving the AlertaBuzzer state.
 * ----------------------------------------------------------------------------
 * @details This function stops the 5-second timer (espera5s), halts the temperature 
 * monitoring task (TaskMonitoreoTemperatura), stops the 5-second timer again 
 * (espera5s), and stops the temperature alert task (TaskAlertaTemp).
 * ----------------------------------------------------------------------------
 * PURPOSE:
 * - To perform actions when leaving the AlertaBuzzer state.
 * ----------------------------------------------------------------------------
 * @param None.
 * @return None.
 * ----------------------------------------------------------------------------
 * NOTE:
 * - Stops the 5-second timer (espera5s) by calling its Stop() method.
 * - Halts the temperature monitoring task (TaskMonitoreoTemperatura) by calling 
 * its Stop() method.
 * - Stops the 5-second timer again (espera5s) by calling its Stop() method.
 * - Stops the temperature alert task (TaskAlertaTemp) by calling its Stop() method.
 */

void onLeavingAlertaBuzzer() {
    espera5s.Stop();
    TaskMonitoreoTemperatura.Stop();
    espera5s.Stop();
    TaskAlertaTemp.Stop();
}


/***************//*
 * Lock Function
 *******************/

/**
 * @brief Function to activate the system lock.
 * ------------------------------------------------- ---------------------------
 * @details This function is responsible for activating the system lock when
 * reaches the limit of failed attempts or when the timeout exceeds the limit.
 * ------------------------------------------------- ---------------------------
 *PURPOSE:
 * - Activate the system lock and perform the corresponding actions.
 * ------------------------------------------------- ---------------------------
 * @param None.
 * @return None.
 * ------------------------------------------------- ---------------------------
 *NOTE:
 * - If the number of failed attempts is exceeded, a message is displayed indicating the system will crash.
 */

void SisSeguridad(void)
{
  digitalWrite(LED_RED, LOW);

  char key = keypad.getKey();
  if(key)
  {
    lcd.print("*");
    inPassword[count] = key;
    count++;
  }

  if(trycount == 3)
  {
    currentInput = SisBloqueado;
    tone(BUZZER, 100, 150);
    delay(150);
    noTone(BUZZER);
    return false;
  }

  if(count == 4)
  {
    count = 0;
    lcd.clear();

    if(compararPalabras(4, inPassword, password))
    {
      claveCorrecta();
      lcd.clear();
      currentInput=static_cast<Input>(Input::clvCorrecta);
      return true;
    }else
    {
      claveIncorrecta();
    }
  }
  return false;
  
}
void Bloqueo(void) {
  digitalWrite(LED_RED, HIGH);

  if (trycount < 3) 
  {
    
    lcd.clear();
    lcd.print("Tiempo");
    lcd.setCursor(0, 1);
    lcd.print("Agotado");
    delay(2000); // Ajuste del tiempo en que aparece el mensaje de tiempo agotado
    lcd.clear();
  } 
  else 
  {
    lcd.clear();
    lcd.print("Demasiados ");
    lcd.setCursor(0, 1);
    lcd.print("Intentos");
    delay(2000); // Ajuste del tiempo en que aparece el mensaje de intentos agotados
    lcd.clear();

    lcd.print("Sistema");
    lcd.setCursor(0, 1);
    lcd.print("Bloqueado");
    delay(1000);
    lcd.clear();
  }
  
  count = 0;
  trycount = 0;
}

/***************//*
 * Light Intensity Monitoring
 *******************/

/**
 * @brief Function to monitor ambient light intensity.
 * ------------------------------------------------- ---------------------------
 * @details This function reads the light sensor and displays on the
 *LCD display the current value of light intensity. If the intensity of
 * light is less than a certain threshold, triggers a specific input
 * in the state machine.
 * ------------------------------------------------- ---------------------------
 *PURPOSE:
 * - Monitor the light intensity and activate an input in the state machine if it is low.
 * ------------------------------------------------- ---------------------------
 * @param None.
 * @return None.
 * ------------------------------------------------- ---------------------------
 *NOTE:
 * - Read the light sensor value.
 * - Shows the value read on the LCD screen.
 * - If the light intensity is less than the threshold, activates a specific input.
 */

void monitoreoLuz(void)
{
  luz = analogRead(PHOTOCELL_PIN);

  lcd.clear();
  lcd.print("Luz: ");
  lcd.print(luz);

  if(luz < 40)
    currentInput = LuzBaja;
}

/***************//*
 * Low Lighting Alert
 *******************/

/**
 * @brief Function to activate low lighting alert.
 * ------------------------------------------------- ---------------------------
 * @details This function displays a message on the LCD screen indicating that
 * lighting is low. Additionally, it activates a tone and flashes the red LED.
 * ------------------------------------------------- ---------------------------
 *PURPOSE:
 * - Notify about low lighting conditions.
 * ------------------------------------------------- ---------------------------
 * @param None.
 * @return None.
 * ------------------------------------------------- ---------------------------
 *NOTE:
 * - Displays a message on the LCD screen indicating low lighting.
 * - Generates a tone and flashes the red LED as a visual and auditory alert.
 */

void monitoreoTemperatura(void)
{
  a = 1023 - analogRead(TemperaturePin);
  temp = beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;

  lcd.clear();
  lcd.print("Temperatura: ");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.print("C");

  if(temp > 30)
    currentInput = temp30;
}

/********************************************//**
 * Low Lighting Alert
 *********************************************************/

/**
 * @brief Function to activate low lighting alert.
 * ------------------------------------------------- ---------------------------
 * @details This function displays a message on the LCD screen indicating that
 * lighting is low. Additionally, it activates a tone and flashes the red LED.
 * ------------------------------------------------- ---------------------------
 *PURPOSE:
 * - Notify about low lighting conditions.
 * ------------------------------------------------- ---------------------------
 * @param None.
 * @return None.
 * ------------------------------------------------- ---------------------------
 *NOTE:
 * - Displays a message on the LCD screen indicating low lighting.
 * - Generates a tone and flashes the red LED as a visual and auditory alert.
 */

void AlertaLuz(void)
{
  lcd.clear(); 
  lcd.print("Iluminacion baja");

  tone(BUZZER, 100, 150);
  delay(150);;
  noTone(BUZZER);

  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(200);
}

/********************************************//**
 * High Temperature Alert
 *********************************************************/

/**
 * @brief Function to activate high temperature alert.
 * ------------------------------------------------- ---------------------------
 * @details This function displays a message on the LCD screen indicating that
 * the temperature is high. Additionally, it activates a tone and flashes the red LED.
 * ------------------------------------------------- ---------------------------
 *PURPOSE:
 * - Notify about high temperature conditions.
 * ------------------------------------------------- ---------------------------
 * @param None.
 * @return None.
 * ------------------------------------------------- ---------------------------
 *NOTE:
 * - Displays a message on the LCD screen indicating high temperature.
 * - Generates a tone and flashes the red LED as a visual and auditory alert.
 */
void AlertaTemp(void)
{
  lcd.clear();
  lcd.print("Temperatura Alta ");

  tone(BUZZER, 1000, 5000); 
  delay(500);
  noTone(BUZZER);

  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(200);
}