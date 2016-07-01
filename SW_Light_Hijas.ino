
// NODO 1 del Cuarto de las hijas, switch que prende y apaga cada cama, recibe estado inicial y prende leds 
// Sensor de movimiento para activar leds


// MySensosrs
#include <MySensor.h>
#include <SPI.h>
#define NODE_ID 1     // Nodo: Numero de nodo fijo para MySensors
#define CH_ID_BT1 1   // Boton: Luz Eli
#define CH_ID_BT2 2   // Boton: Luz Naty
#define CH_ID_LD1 3   // LED: Luz Eli
#define CH_ID_LD2 4   // LED: Luz Nati
#define CH_ID_PR1 5   // PIR: Movimiento
MySensor gw;
MyMessage msgOUT(CH_ID_BT1,V_STATUS);

// Asignacion de Pin
int btn1Pin = 2;          // Button 1 (Interrupt 0)
int btn2Pin = 3;          // Button 2 (Interrupt 1)
int sen1Pin = 4;          // PIR 1
int led1Pin = 5;          // Led 1
int led2Pin = 6;          // Led 2

// Variables de estado
boolean lgt1State = 0;        // Status de Luz 1 actual (0 = Encendido por el pullup)
boolean lgt1PrevState = 0;    // Status de Luz 1 Anterior (Asume apagado en reboot por default)
boolean lgt2State = 0;        // Status de Luz 2 actual (0 = Encendido por el pullup)
boolean lgt2PrevState = 0;    // Status de Luz 2 Anterior (Asume apagado en reboot por default)
boolean hwAck;                // Aknnowledge de Mysensors


void btn1Interrupt() // Rutina de Interrupcion 0, PIN2
  {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // Hace debounce por 200ms
    if (interrupt_time - last_interrupt_time > 200) 
      {
        if (lgt1State == 0) // Hace que funcione como un toggle switch
          lgt1State = 1;  
        else
          lgt1State = 0;
       }
    last_interrupt_time = interrupt_time;
  }

void btn2Interrupt() // Rutina de Interrupcion 1, PIN3
  {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // Hace debounce por 200ms
    if (interrupt_time - last_interrupt_time > 200) 
      {
        if (lgt2State == 0) // Hace que funcione como un toggle switch
          lgt2State = 1;  
        else
          lgt2State = 0;
       }
    last_interrupt_time = interrupt_time;
  }  

  
void setup()
  {
    // Inicia Serial Debug
    Serial.begin(115200);
    delay(100);
    Serial.println("Serial Started");
    // Inicializa Pins
    pinMode(btn1Pin, INPUT_PULLUP);
    pinMode(btn2Pin, INPUT_PULLUP);
    pinMode(sen1Pin, INPUT);  
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    Serial.println("Pins Initialized");
    // Inicializa Interrupciones 
    attachInterrupt(0,btn1Interrupt, FALLING);
    attachInterrupt(1,btn2Interrupt, FALLING);
    Serial.println("Interruptions Attached");
    // Inicia MySensor GW
    gw.begin(msgIN, NODE_ID);
    gw.wait(1200);
    gw.sendSketchInfo("Nodo: RMHijas_ND01", "Version: 1.0");
    gw.wait(1200);
    gw.present(CH_ID_BT1, S_BINARY,"RMHijas_ND01_BT01",1);
    gw.wait(1200);
    gw.present(CH_ID_BT2, S_BINARY,"RMHijas_ND01_BT02",1);
    gw.wait(1200);
    gw.present(CH_ID_PR1, S_MOTION,"RMHijas_ND01_PR01",1);
    gw.wait(1200);
    gw.present(CH_ID_LD1, S_LIGHT,"RMHijas_ND01_LD01",1);
    gw.wait(1200);
    gw.present(CH_ID_LD2, S_LIGHT,"RMHijas_ND01_LD02",1);
    gw.wait(1200);
    Serial.println("My Sensors Node and Sensors Started");
    // Get Led State from Controller
    gw.request(CH_ID_LD1,V_STATUS);
    gw.wait(1200);
    Serial.println("Status LED 1 Requested");
    gw.request(CH_ID_LD2,V_STATUS);
    gw.wait(1200);
    Serial.println("Status LED 2 Requested");
  }

void loop()
  {
    gw.process();
    // Verifica cambio de Estado
    if (lgt1State != lgt1PrevState) 
      {
        // Envia Estado a Gateway 
        msgOUT.setSensor(CH_ID_BT1);
        hwAck = gw.send(msgOUT.set(lgt1State), true);
        lgt1PrevState = lgt1State;
        Serial.print("Envio Luz 1 State: ");
        Serial.println(lgt1State);
      }
    if (lgt2State != lgt2PrevState) 
      {
        // Envia Estado a Gateway 
        msgOUT.setSensor(CH_ID_BT2);
        hwAck = gw.send(msgOUT.set(lgt2State), true);
        lgt2PrevState = lgt2State;
        Serial.print("Envio Luz 2 State: ");
        Serial.println(lgt2State);
      }  
  }

void msgIN(const MyMessage &message) 
  {
    Serial.print("Message received ledstate: ");
    Serial.println(lgt1State);
    int comando = mGetCommand(message); // No lo uso por el momento****************
    if (message.isAck())
      {
        Serial.print("Acknowledge from gateway to Sensor ");
        Serial.print(message.sensor);
        Serial.print(": " );
        Serial.println(message.getString());
      }
    
    switch (message.sensor)
      {
        case CH_ID_BT1:
          // Mensaje Recibido para Boton
        break;
        //**************** falta decidir Color de LED para Flag 1,2 y ON/OFF********************************
        case CH_ID_LD1:
         // Mensaje Recibido para LED 1
          if (message.type==V_STATUS)     
            {
              Serial.print("Incoming change for sensor:");
              Serial.print(message.sensor);
              Serial.print(", New status: ");
              Serial.println(message.getBool());
              lgt1State = message.getBool();
              lgt1PrevState = lgt1State;
              if (message.getBool() == 0)
                {
                  // Luz apagada LED 1
                  digitalWrite(led1Pin, LOW);
                }
              if (message.getBool() == 1)
                {
                  // Luz encendida LED 1
                  digitalWrite(led1Pin, HIGH);
                }
            }
          break;
          case CH_ID_LD2:
         // Mensaje Recibido para LED 
          if (message.type==V_STATUS)     
            {
              Serial.print("Incoming change for sensor:");
              Serial.print(message.sensor);
              Serial.print(", New status: ");
              Serial.println(message.getBool());
              lgt2State = message.getBool();
              lgt2PrevState = lgt2State;
              if (message.getBool() == 0)
                {
                  // Luz apagada LED 2
                  digitalWrite(led2Pin, LOW);
                }
              if (message.getBool() == 1)
                {
                  // Luz encendida LED 2
                  digitalWrite(led2Pin, HIGH);
                }
            }
          break;
        }
    }
