//Bibliotecas
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <AltSoftSerial.h>
//Para o modulo wifi ESP8266
AltSoftSerial esp8266;

#define DEBUG true

/**************SENSORES ************************/
byte statusLed    = 13;
byte sensor1Interrupt = 0;  // 0 = digital pin 2
byte sensor2Interrupt = 1;  // 1 = digital pin 3
byte sensor3Interrupt = 5;  // 5 = digital pin 18
byte sensor4Interrupt = 4;  // 4 = digital pin 19

byte sensor1Pin       = 2; //SENSOR VAZAO 1 (MASTER)
byte sensor2Pin       = 3; //SENSOR VAZAO 2
byte sensor3Pin       = 18; //SENSOR VAZAO 3
byte sensor4Pin       = 19; //SENSOR VAZAO 4

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5; 

//*************************SENSOR1
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;
//**************************SENSOR2
volatile byte pulseCount2;
float flowRate2;
unsigned int flowMilliLitres2;
unsigned long totalMilliLitres2;
unsigned long oldTime2;
//**************************SENSOR3
volatile byte pulseCount3;
float flowRate3;
unsigned int flowMilliLitres3;
unsigned long totalMilliLitres3;
unsigned long oldTime3;
//**************************SENSOR4
volatile byte pulseCount4;
float flowRate4;
unsigned int flowMilliLitres4;
unsigned long totalMilliLitres4;
unsigned long oldTime4;

void setup()
{
 Serial.begin(9600); //para o arduino
 esp8266.begin(19200); //para o esp8266
 
 /*******************SENSOR VAZAO*****************************/   
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  //Setando os pinos dos sensores para HIGH e INPUT
  pinMode(sensor1Pin, INPUT);
  digitalWrite(sensor1Pin, HIGH);
  
  pinMode(sensor2Pin, INPUT);
  digitalWrite(sensor2Pin, HIGH);

  pinMode(sensor3Pin, INPUT);
  digitalWrite(sensor3Pin, HIGH);

  pinMode(sensor4Pin, INPUT);
  digitalWrite(sensor4Pin, HIGH);

  //Variáveis para os sensores
  //**********************SENSOR1 
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  //**********************SENSOR2 
  pulseCount2        = 0;
  flowRate2          = 0.0;
  flowMilliLitres2   = 0;
  totalMilliLitres2  = 0;
  oldTime2           = 0;
  //**********************SENSOR3 
  pulseCount3        = 0;
  flowRate3          = 0.0;
  flowMilliLitres3   = 0;
  totalMilliLitres3  = 0;
  oldTime3           = 0;
  //**********************SENSOR4 
  pulseCount4        = 0;
  flowRate4          = 0.0;
  flowMilliLitres4   = 0;
  totalMilliLitres4  = 0;
  oldTime4           = 0; 
  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensor1Interrupt, pulseCounter, FALLING);
  attachInterrupt(sensor2Interrupt, pulseCounter2, FALLING);
  attachInterrupt(sensor2Interrupt, pulseCounter3, FALLING);
  attachInterrupt(sensor2Interrupt, pulseCounter4, FALLING);
 /************************************************/
 Serial.println("***INICIALIZANDO***\n");

 sendData("AT+RST\r\n", 5000, DEBUG); // rst
 // Conecta a rede wireless
 //sendData("AT+CWJAP=\"SSID\",\"SENHA\"\r\n", 2000, DEBUG);
 sendData("AT+CWJAP=\"iPhone Marco Herberty\",\"herberty123\"\r\n", 9000, DEBUG);

 delay(3000);
 sendData("AT+CWMODE=1\r\n", 9000, DEBUG);
 // Mostra o endereco IP
 sendData("AT+CIFSR\r\n", 9000, DEBUG);
 // Configura para multiplas conexoes
 sendData("AT+CIPMUX=1\r\n", 9000, DEBUG);
 
 //Inicia o web server na porta 80
 sendData("AT+CIPSERVER=1,80\r\n", 9000, DEBUG);

 Serial.println("***Fim Setup***\n");
}

void loop(){
  
/********************** SENSORES **************************/

    // Only process counters once per second
    if((millis() - oldTime) > 1000 && (millis() - oldTime2) > 1000 && (millis() - oldTime3) > 1000 && (millis() - oldTime4) > 1000)
   {
    /************************   SENSOR 1   *********************************************/ 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensor1Interrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate SENSOR 1: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensor1Interrupt, pulseCounter, FALLING);
    
    /*************************************************************************************/
    
    /************************   SENSOR 2   *********************************************/ 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensor2Interrupt);
    flowRate2 = ((1000.0 / (millis() - oldTime2)) * pulseCount2) / calibrationFactor;
    oldTime2 = millis();
    flowMilliLitres2 = (flowRate2 / 60) * 1000;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres2 += flowMilliLitres2;
      
    unsigned int frac2;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate SENSOR 2: ");
    Serial.print(int(flowRate2));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac2 = (flowRate2 - int(flowRate2)) * 10;
    Serial.print(frac2, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres2);
    Serial.print("mL/Sec");
    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres2);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount2 = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensor2Interrupt, pulseCounter2, FALLING);
    /*************************************************************************************/

    /************************   SENSOR 3   *********************************************/ 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensor3Interrupt);
    flowRate3 = ((1000.0 / (millis() - oldTime3)) * pulseCount3) / calibrationFactor;
    oldTime3 = millis();
    flowMilliLitres3 = (flowRate3 / 60) * 1000;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres3 += flowMilliLitres3;
      
    unsigned int frac3;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate SENSOR 3: ");
    Serial.print(int(flowRate3));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac3 = (flowRate3 - int(flowRate3)) * 10;
    Serial.print(frac3, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres3);
    Serial.print("mL/Sec");
    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres3);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount3 = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensor3Interrupt, pulseCounter3, FALLING);

    /************************   SENSOR 4   *********************************************/ 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensor4Interrupt);
    flowRate4 = ((1000.0 / (millis() - oldTime4)) * pulseCount4) / calibrationFactor;
    oldTime4 = millis();
    flowMilliLitres4 = (flowRate4 / 60) * 1000;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres4 += flowMilliLitres4;
      
    unsigned int frac4;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate SENSOR 4: ");
    Serial.print(int(flowRate4));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac4 = (flowRate4 - int(flowRate4)) * 10;
    Serial.print(frac4, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres4);
    Serial.print("mL/Sec");
    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres4);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount4 = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensor4Interrupt, pulseCounter4, FALLING); 
    
    }

/**************** WIFI ********************************/
 // Verifica se o ESP8266 esta enviando dados
 if (esp8266.available())
 {
   Serial.println("ESP disponivel.\n");

   if (esp8266.find("+IPD,"))
   {
     Serial.println("ESP recebeu dados.\n");
     delay(300);
     int connectionId = esp8266.read() - 48;

    //String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
    String webpage = "<head>";
     webpage += "</head><h1></u>ESP8266 - LOCAL WEBSERVER WATER FLOW SENSOR</u></h1>";
     webpage += "<h2>SENSOR 1 (MASTER): ";
     webpage += totalMilliLitres;;
     webpage += " mL";
     webpage += "<h2>SENSOR 2: ";
     webpage += totalMilliLitres2;
     webpage += " mL";
     webpage += "</h2>";
     webpage += "<h2>SENSOR 3: ";
     webpage += totalMilliLitres3;
     webpage += " mL";
     webpage += "</h2>";
     webpage += "<h2>SENSOR 4: ";
     webpage += totalMilliLitres4;
     webpage += " mL";
     webpage += "</h2>";
 
 
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend += webpage.length();
     cipSend += "\r\n";

     sendData(cipSend, 5000, DEBUG);
     sendData(webpage, 5000, DEBUG);

     String closeCommand = "AT+CIPCLOSE=";
     closeCommand += connectionId; // append connection id
     closeCommand += "\r\n";

     sendData(closeCommand, 9000, DEBUG);
   }
  }
  
 }

//Rotinas de Interrupção
void pulseCounter()
{
  // Increment the pulse counter sensor 1
  pulseCount++;
}
void pulseCounter2()
{
  // Increment the pulse counter sensor 2
  pulseCount2++;
}
void pulseCounter3()
{
  // Increment the pulse counter sensor 3
  pulseCount3++;
}
void pulseCounter4()
{
  // Increment the pulse counter sensor 4
  pulseCount4++;
}
 
String sendData(String command, const int timeout, boolean debug)
{
 // Envio dos comandos AT para o modulo
 String response = "";
 esp8266.print(command);
 long int time = millis();
 while ( (time + timeout) > millis())
 {
   while (esp8266.available())
   {
     // The esp has data so display its output to the serial window
     char c = esp8266.read(); // read the next character.
     response += c;
   }
 }
 if (debug)
 {
   Serial.print(response);
 }
 return response;
}
