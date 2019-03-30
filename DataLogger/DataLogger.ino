/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 11.02.2017 00:32:42
  Project     :
  Libraries   :
  Author      :
  Description :
******************************************************************/


#include <SPI.h>
#include <Ethernet2.h>
#include "Settimino.h"
#include "TimerOne.h"
#include <avr/wdt.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xED, 0x57};
IPAddress Local(163, 34, 25, 80); // Local Address
IPAddress PLC(163, 34, 25, 31); // PLC Address
IPAddress PLC2(163, 34, 25, 32); // PLC Address
IPAddress PLC3(163, 34, 25, 36); // PLC Address
IPAddress PLC4(163, 34, 25, 27); // PLC Address

// Following constants are needed if you are connecting via WIFI
// The ssid is the name of my WIFI network (the password obviously is wrong)
//char ssid[] = "DLINK";  // Your network password (if any)
IPAddress Gateway(163, 34, 25, 1);
IPAddress Subnet(255, 255, 255, 0);

int DBNum = 1000; // This DB must be present in your PLC
byte Buffer[1024];

// S7Client will create an EthernetClient as TCP Client
S7Client Client(_S7WIRED);

int prog;
unsigned long Elapsed; // To calc the execution time
int VAL2, VAL4, VAL6, VAL8, VAL14, VAL16, VAL18, VAL20, VAL22, VAL32, VAL34, VAL42, VAL48, VAL50, VAL52, VAL56, VAL182;

float VAL30R4, VAL30R8, VAL30R12, VAL30R24, VAL30R28, VAL30R40, VAL30R44, VAL30R48, VAL30R52, VAL30R68, VAL30R76;
float VAL1000R8, VAL1000R48, VAL1000R52, VAL1000R64, VAL1000R68, VAL1000R72, VAL1000R76, VAL1000R128, VAL1000R132;
float VAL409R20, VAL409R24, VAL409R28, VAL409R52, VAL409R56;

int trigger1;
float VALR44;
int Mouchard = 0;


//************
char octetReception;
char caractereReception;
char octetReceptionProc;
char caractereReceptionProc;
String chaineReception, Tram;
String chaineReceptionProc, TramProc, TramProcc;
char charVal[10];

int ValChg = 0;
char CR = 13;
int Lu = 0;

//**********************
int seq = 0;

//*********************
int T[10], TDN[10];

boolean CptT[10];
boolean FirstScan = false;

int B3[5]; // ONS
int P5VF4, P6VF4;
int NbrPLC = 4;

boolean lcdRetro = true;
// ***************************************************************************************************Setup
void setup() {


  Timer1.initialize(100000);
  Timer1.attachInterrupt(callback);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);


  //--------------------------------Wired Ethernet Shield Initialization
  //Start the Ethernet Library
  Ethernet.begin(mac, Local);
  delay(100);
  //Serial.println("");
  //Serial.println("Cable connected");
  //Serial.print("Local IP address : ");
  //Serial.println(Ethernet.localIP());
  delay(200);

  lcd.begin();              // initialize the lcd
}

//**************************************************************************************************callback
void callback()
{

  if (CptT[1]) {

    T[1] =  T[1] + 1.0;
    //Serial.println(T[1]);
  }

}


//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect()
{
  int Result = Client.ConnectTo(PLC,
                                0,  // Rack (see the doc.)
                                2); // Slot (see the doc.)
  //  Serial.print("Connecting to ");
  //  Serial.println(PLC);
  if (Result == 0)
  {
    //    Serial.print("Connected ! PDU Length = ");
    //    Serial.println(Client.GetPDULength());
  }
  else
    //    Serial.println("Connection error");
    return Result == 0;
}


//----------------------------------------------------------------------
// Connects to the PLC2
//----------------------------------------------------------------------
bool Connect2()
{
  int Result = Client.ConnectTo(PLC2,
                                0,  // Rack (see the doc.)
                                2); // Slot (see the doc.)
  //Serial.print("Connecting to ");
  //Serial.println(PLC);
  if (Result == 0)
  {
    //Serial.print("Connected ! PDU Length = ");
    //Serial.println(Client.GetPDULength());
  }
  else
    //Serial.println("Connection error");
    return Result == 0;
}

//----------------------------------------------------------------------
// Connects to the PLC3
//----------------------------------------------------------------------
bool Connect3()
{
  int Result = Client.ConnectTo(PLC3,
                                0,  // Rack (see the doc.)
                                2); // Slot (see the doc.)
  //Serial.print("Connecting to ");
  //Serial.println(PLC);
  if (Result == 0)
  {
    //Serial.print("Connected ! PDU Length = ");
    //Serial.println(Client.GetPDULength());
  }
  else
    //Serial.println("Connection error");
    return Result == 0;
}

//----------------------------------------------------------------------
// Connects to the PLC4
//----------------------------------------------------------------------
bool Connect4()
{
  int Result = Client.ConnectTo(PLC4,
                                0,  // Rack (see the doc.)
                                2); // Slot (see the doc.)
  //Serial.print("Connecting to ");
  //Serial.println(PLC);
  if (Result == 0)
  {
    //Serial.print("Connected ! PDU Length = ");
    //Serial.println(Client.GetPDULength());
  }
  else
    //Serial.println("Connection error");
    return Result == 0;
}

//----------------------------------------------------------------------
// Dumps a buffer (a very rough routine)
//----------------------------------------------------------------------
void Dump(void *Buffer, int Length)
{
  int i, cnt = 0;
  pbyte buf;

  if (Buffer != NULL)
    buf = pbyte(Buffer);
  else
    buf = pbyte(&PDU.DATA[0]);

  //  Serial.print("[ Dumping "); //Serial.print(Length);
  //  Serial.println(" bytes ]===========================");
  for (i = 0; i < Length; i++)
  {
    cnt++;
    if (buf[i] < 0x10)
      //      Serial.print("0");
      //    Serial.print(buf[i], HEX);
      //    Serial.print(" ");
      if (cnt == 16)
      {
        cnt = 0;
        //      Serial.println();
      }
  }
  //  Serial.println("===============================================");
}
//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  //Serial.print("Error No. 0x");
  //Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    //Serial.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect();
  }
}
//----------------------------------------------------------------------
// Profiling routines
//----------------------------------------------------------------------
void MarkTime()
{
  Elapsed = millis();
}
//----------------------------------------------------------------------
void ShowTime()
{
  // Calcs the time
  Elapsed = millis() - Elapsed;
  //Serial.print("Job time (ms) : ");
  // Serial.println(Elapsed);
}



//******************************************************************************************LOOP
void loop() {

  if (Lu == 0) {

    //lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("HELLO");
    delay(2000);
    lcd.clear();
  } else {
    //lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(T[1] );
    lcd.setCursor(0, 0);
    lcd.print("Seq:     ");
    lcd.setCursor(5, 0);
    lcd.print(seq);
  }


  if (Lu == 1) {
    //***************TEMPO
    //T1 2s
    if (TDN[1] == 0.0 & seq == 0) {
      CptT[1] = true;
    }
    else {
      T[1] =  0.0;
      TDN[1] =  0.0;
      CptT[1] = false;
    }

    if (T[1] >= 50.0) // présélection 20= 2 secondes METTRE 600
    {
      TDN[1] =  1.0;   // bit down
      T[1] = 0.0;
      //Mouchard=Mouchard+1;

    }


    if ( TDN[1] == 1.0) {

      seq = 1;
    }



    // --------------------------------------------------------------------SEQUENCE
    //***************PLC
    switch (seq)
    {
      case 1: //****** ETHERNET INITIALISATION
        delay(1000);
        seq = 2;
        break;


      case 2: //****** LECTURE DB SIEMENS 1er ADRESSE IP
        int Size, Result;
        void *Target;

        //byte mybuffer[1024];

        Size = 184;
        Target = &Buffer; // Uses a larger buffer
        // Connection

        while (!Client.Connected)
        {
          if (!Connect())
            delay(500);

        }
        //Serial.print("Reading ");Serial.print(Size);Serial.print(" bytes from DB");Serial.println(DBNum);
        // Get the current tick
        //Serial.print("Reading ");
        MarkTime();
        Client.ReadArea(S7AreaDB, 45, 0, 184, Target);

        if (Result == 0)
        {
          ShowTime();
          Dump(Target, Size);
        }
        else
          CheckError(Result);

        delay(500);
        seq = 3;
        break;

      //*******DB45
      case 3:  //***** FORMATAGE DES DONNEES
        VAL2 = S7.IntegerAt(2);// INT
        //Serial.println(VAL2);
        VAL4 = S7.IntegerAt(4);// INT
        //Serial.println(VAL4);
        VAL6 = S7.IntegerAt(6); // INT
        //Serial.println(VAL6);
        VAL8 = S7.IntegerAt(8); // INT
        //Serial.println(VAL8);
        VAL14 = S7.IntegerAt(14); // INT
        //Serial.println(VALR6);
        VAL16 = S7.IntegerAt(16); // INT
        //Serial.println(VALR6);
        VAL18 = S7.IntegerAt(18); // INT
        //Serial.println(VALR6);
        VAL20 = S7.IntegerAt(20); // INT
        //Serial.println(VALR6);
        VAL22 = S7.IntegerAt(22); // INT
        //Serial.println(VALR6);
        VAL32 = S7.IntegerAt(32); // INT
        //Serial.println(VALR6);
        VAL34 = S7.IntegerAt(34); // INT
        //Serial.println(VALR6);
        VAL42 = S7.IntegerAt(42); // INT
        //Serial.println(VALR6);
        VAL48 = S7.IntegerAt(48); // INT
        //Serial.println(VALR6);
        VAL50 = S7.IntegerAt(50); // INT
        //Serial.println(VALR6);
        VAL52 = S7.IntegerAt(52); // INT
        //Serial.println(VALR6);
        VAL56 = S7.IntegerAt(56); // INT
        //Serial.println(VALR6);
        VAL182 = S7.IntegerAt(182); // INT
        //Serial.println(VALR6);
        VALR44 = S7.FloatAt(44);// REEL
        //Serial.println(VALR10);

        seq = 4;
        break;
      //***************DB46
      case 4:
        Client.ReadArea(S7AreaDB, 46, 0, 8, Target);
        if (Result == 0)
        {
          ShowTime();
          Size = 2;
          Dump(Target, Size);
        }

        else
          CheckError(Result);

        trigger1 = PDU.DATA[7] & 1; //Buffer[0];//= S7.IntegerAt(0);
        seq = 5;
        break;

      case 5:  //***** ENVOI VERS PROCESSING DES DONNEES
        //Serial.println(trigger1);
        if (trigger1 != 1)
        {
          //AffichageParam();
        }
        seq = 6;
        break;

      case 6:  //***** TEST PLC SUIVANT
        //Serial.println(trigger1);
        if (NbrPLC == 1)
        {
          Client.Disconnect();
          seq = 0;

        } else {
          Client.Disconnect();
          seq = 7;
        }

        break;

      //*******DB30
      case 7:  //***** TRAITEMENT PLC2

        Size = 184;
        Target = &Buffer; // Uses a larger buffer
        // Connection

        while (!Client.Connected)
        {
          if (!Connect2())
            delay(500);

        }
        //Serial.print("Reading ");Serial.print(Size);Serial.print(" bytes from DB");Serial.println(DBNum);
        // Get the current tick
        //Serial.print("Reading ");
        MarkTime();
        Client.ReadArea(S7AreaDB, 30, 0, 78, Target);

        if (Result == 0)
        {
          ShowTime();
          Dump(Target, Size);
        }
        else
          CheckError(Result);

        delay(500);
        seq = 8;
        break;

      case 8:  //***** FORMATAGE DES DONNEES
        VAL30R4 = S7.FloatAt(4);// REEL
        VAL30R8 = S7.FloatAt(8);// REEL
        VAL30R12 = S7.FloatAt(12);// REEL
        VAL30R24 = S7.FloatAt(24);// REEL
        VAL30R28 = S7.FloatAt(28);// REEL
        VAL30R40 = S7.FloatAt(40);// REEL
        VAL30R44 = S7.FloatAt(44);// REEL
        VAL30R48 = S7.FloatAt(48);// REEL
        VAL30R52 = S7.FloatAt(52);// REEL
        VAL30R68 = S7.FloatAt(68);// REEL
        VAL30R76 = S7.FloatAt(76);// REEL
        seq = 9;
        break;

      case 9:  //***** ENVOI VERS PROCESSING DES DONNEES
        if (trigger1 == 1)
        {
          AffichageParam30();
        }
        seq = 10;
        break;

      case 10:  //***** TEST PLC SUIVANT
        //Serial.println(trigger1);
        if (NbrPLC == 2)
        {
          Client.Disconnect();
          seq = 0;

        } else {
          Client.Disconnect();
          seq = 11;
        }

        break;

      //*******DB1000
      case 11:  //***** TRAITEMENT PLC3

        Size = 184;
        Target = &Buffer; // Uses a larger buffer
        // Connection

        while (!Client.Connected)
        {
          if (!Connect3())
            delay(500);

        }
        //Serial.print("Reading ");Serial.print(Size);Serial.print(" bytes from DB");Serial.println(DBNum);
        // Get the current tick
        //Serial.print("Reading ");
        MarkTime();
        Client.ReadArea(S7AreaDB, 1000, 0, 136, Target);

        if (Result == 0)
        {
          ShowTime();
          Dump(Target, Size);
        }
        else
          CheckError(Result);

        delay(500);
        seq = 12;
        break;


      case 12:  //***** FORMATAGE DES DONNEES
        VAL1000R8 = S7.FloatAt(8);// REEL
        VAL1000R48 = S7.FloatAt(48);// REEL
        VAL1000R52 = S7.FloatAt(52);// REEL
        VAL1000R64 = S7.FloatAt(64);// REEL
        VAL1000R68 = S7.FloatAt(68);// REEL
        VAL1000R72 = S7.FloatAt(72);// REEL
        VAL1000R76 = S7.FloatAt(76);// REEL
        VAL1000R128 = S7.FloatAt(128);// REEL
        VAL1000R132 = S7.FloatAt(132);// REEL

        seq = 13;
        break;

      case 13:  //***** ENVOI VERS PROCESSING DES DONNEES

        //AffichageParam1000();

        seq = 14;
        break;

      case 14:  //***** TEST PLC SUIVANT
        //Serial.println(trigger1);
        if (NbrPLC == 3)
        {
          Client.Disconnect();
          seq = 0;

        } else {
          Client.Disconnect();
          seq = 15;
        }

        break;


      //*******DB409
      case 15:  //***** TRAITEMENT PLC4

        Size = 184;
        Target = &Buffer; // Uses a larger buffer
        // Connection

        while (!Client.Connected)
        {
          if (!Connect4())
            delay(500);

        }
        //Serial.print("Reading ");Serial.print(Size);Serial.print(" bytes from DB");Serial.println(DBNum);
        // Get the current tick
        //Serial.print("Reading ");
        MarkTime();
        Client.ReadArea(S7AreaDB, 409, 0, 58, Target);

        if (Result == 0)
        {
          ShowTime();
          Dump(Target, Size);
        }
        else
          CheckError(Result);

        delay(500);
        seq = 16;
        break;


      case 16:  //***** FORMATAGE DES DONNEES
        VAL409R20 = S7.FloatAt(20);// REEL
        VAL409R24 = S7.FloatAt(24);// REEL
        VAL409R28 = S7.FloatAt(28);// REEL
        VAL409R52 = S7.FloatAt(52);// REEL
        VAL409R56 = S7.FloatAt(56);// REEL

        seq = 17;
        break;

      case 17:  //***** ENVOI VERS PROCESSING DES DONNEES

        AffichageParam409();
        delay(5000);
        seq = 18;
        break;

      case 18:  //***** TEST PLC SUIVANT
        //Serial.println(trigger1);
        if (NbrPLC == 4)
        {
          Client.Disconnect();
          seq = 0;

        } else {
          Client.Disconnect();
          seq = 0;
        }
        FirstScan = true;
        delay(5000);
        break;

      default:
        break;

    }
    //************************************************************************************************************DISPLAY
    //    lcd.clear();
    //    lcd.setCursor(3, 1);
    //    lcd.print(Lu);
    //    lcd.setCursor(0, 1);
    //    lcd.print("Lu");
    //    lcd.setCursor(5, 1);
    //    lcd.print("SEQ:");
    //    lcd.setCursor(9, 1);
    //    lcd.print(seq);
    //    lcd.setCursor(0, 0);
    //    lcd.print (T[1]);
  }
}

//*********************************************************************************************************VERS PROCESSING
void AffichageParam409() {
  TramProcc = "";
  TramProcc = "CDB1000-DB409 ;";
  TramProcc = TramProcc + VAL1000R8 + ";";
  TramProcc = TramProcc + VAL1000R48 + ";";
  TramProcc = TramProcc + VAL1000R52 + ";";
  TramProcc = TramProcc + VAL1000R64 + ";";
  TramProcc = TramProcc + VAL1000R68 + ";";
  TramProcc = TramProcc + VAL1000R72 + ";";
  TramProcc = TramProcc + VAL1000R76 + ";";
  TramProcc = TramProcc + VAL1000R128 + ";";
  TramProcc = TramProcc + VAL1000R132 + ";";
  TramProcc = TramProcc + VAL409R20 + ";";
  TramProcc = TramProcc + VAL409R24 + ";";
  TramProcc = TramProcc + VAL409R28 + ";";
  TramProcc = TramProcc + VAL409R52 + ";";
  TramProcc = TramProcc + VAL409R56 + ";" + "*";
  if (FirstScan) {
    Serial.println(TramProcc);

  }
}


//*********************************************************************************************************VERS PROCESSING
void AffichageParam1000() {

  TramProcc = "";
  TramProcc = "C  DB1000: ;";
  TramProcc = TramProcc + VAL1000R8 + ";";
  TramProcc = TramProcc + VAL1000R48 + ";";
  TramProcc = TramProcc + VAL1000R52 + ";";
  TramProcc = TramProcc + VAL1000R64 + ";";
  TramProcc = TramProcc + VAL1000R68 + ";";
  TramProcc = TramProcc + VAL1000R72 + ";";
  TramProcc = TramProcc + VAL1000R76 + ";";
  TramProcc = TramProcc + VAL1000R128 + ";";
  TramProcc = TramProcc + VAL1000R132 + ";" + "*";
  if (FirstScan) {
    Serial.println(TramProcc);
  }

}

//*********************************************************************************************************VERS PROCESSING
void AffichageParam30() {

  TramProcc = "";
  TramProc = "DB45-DB30 ;";
  TramProc = TramProc + VAL2 + ";";
  TramProc = TramProc + VAL4 + ";";
  TramProc = TramProc + VAL6 + ";";
  TramProc = TramProc + VAL8 + ";";
  TramProc = TramProc + VAL14 + ";";
  TramProc = TramProc + VAL16 + ";";
  TramProc = TramProc + VAL18 + ";";
  TramProc = TramProc + VAL20 + ";";
  TramProc = TramProc + VAL22 + ";";
  TramProc = TramProc + VAL32 + ";";
  TramProc = TramProc + VAL34 + ";";
  TramProc = TramProc + VAL42 + ";";
  TramProc = TramProc + VAL48 + ";";
  TramProc = TramProc + VAL50 + ";";
  TramProc = TramProc + VAL52 + ";";
  TramProc = TramProc + VAL56 + ";";
  TramProc = TramProc + VAL182 + ";";
  TramProc = TramProc + VALR44 + ";";
  TramProc = TramProc + VAL30R4 + ";";
  TramProc = TramProc + VAL30R8 + ";";
  TramProc = TramProc + VAL30R12 + ";";
  TramProc = TramProc + VAL30R24 + ";";
  TramProc = TramProc + VAL30R28 + ";";
  TramProc = TramProc + VAL30R40 + ";";
  TramProc = TramProc + VAL30R44 + ";";
  TramProc = TramProc + VAL30R48 + ";";
  TramProc = TramProc + VAL30R52 + ";";
  TramProc = TramProc + VAL30R68 + ";";
  TramProc = TramProc + VAL30R76 + ";" + "*";
  if (FirstScan) {
    Serial.println(TramProc);
  }

}

//*********************************************************************************************************VERS PROCESSING
void AffichageParam() {

  TramProcc = "";
  TramProc = "DB45: ;";
  TramProc = TramProc + VAL2 + ";";
  TramProc = TramProc + VAL4 + ";";
  TramProc = TramProc + VAL6 + ";";
  TramProc = TramProc + VAL8 + ";";
  TramProc = TramProc + VAL14 + ";";
  TramProc = TramProc + VAL16 + ";";
  TramProc = TramProc + VAL18 + ";";
  TramProc = TramProc + VAL20 + ";";
  TramProc = TramProc + VAL22 + ";";
  TramProc = TramProc + VAL32 + ";";
  TramProc = TramProc + VAL34 + ";";
  TramProc = TramProc + VAL42 + ";";
  TramProc = TramProc + VAL48 + ";";
  TramProc = TramProc + VAL50 + ";";
  TramProc = TramProc + VAL52 + ";";
  TramProc = TramProc + VAL56 + ";";
  TramProc = TramProc + VAL182 + ";";
  TramProc = TramProc + VALR44 + ";" + "*";
  if (FirstScan) {
    Serial.println(TramProc);
  }

}


//**************************************************************************************************Soft Reset
void software_Reboot()
{
  wdt_enable(WDTO_15MS);

  while (1)
  {

  }
}
//*********************************************************************************************DE PROCESSING
void Reception() {
  //Lu = 1;
  while (Serial.available() > 0) {

    Lu = 1;
    Serial.print ("dfhyiuiogdfsjslkmgmm");
    Serial.read();
    Serial.flush();
  }
}

//*****************************************************************************************************************
void serialEvent() {
  if (Serial.available() > 0) {

    Lu = 1;

  }
}
