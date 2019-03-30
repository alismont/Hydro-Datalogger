//************************************** PORT SERIE
import processing.serial.*;


Serial myPort; //myPortGE;
int baudRate = 9600;
int lf = 10;


String[] tag = new String[110];
char octetReception;
char caractereReception;
String chaineReception;
int Cont=0;

PrintWriter output, output2;
int cmpt_reception = 0;
PGraphics pg;
String chaineReceptionEnTete, chaineReceptionEnTeteC;
int d ;    // Values from 1 - 31
int m ;  // Values from 1 - 12
int y ;   // 2003, 2004, 2005, etc.
int s ;  // Values from 0 - 59
int mi ;  // Values from 0 - 59
int h ;    // Values from 0 - 23

//***************************************
void setup() {

  background(#FEFFFC);
  size(1000, 900);
  frameRate(30);




  println(Serial.list());
  //exemple de liste de ports: COM3 COM24 COM25 COM26 COM30 COM31
  myPort = new Serial(this, Serial.list()[2], baudRate); 

  myPort.buffer(1000);
  myPort.clear();

  frameRate(60);

  //*****************************************************************************FICHIER


  output = createWriter("data/ValTrigger.csv"); 
  output2 = createWriter("data/ValCont.csv"); 
  myPort.write("Lu");

  rect(30, 20, 500, 55);
  textSize(32);

  fill(0, 102, 153);
  text("ENREGISTREMENT DES FICHIERS", 35, 60);
  fill(0, 102, 153, 51);
  pg = createGraphics(100, 160);

  chaineReceptionEnTete=" ;ARGONFLOW;PV_OXYGENFLOW;PV_ARGON_PRS;PV_CAST_SPEED;PV_CAST_LENG;TEMP_ON_TABLE;COOLING_WATER_FLOW;COOL_WATER_TEMP;WATER_LEVEL_PIT;CV_ARGON_PRESS_CONT;SP_OXYGENE_FLOW;SP_ARGON_PRESS;SP_CASTING_SPEED;SP_WATER_FLOW_CASTING;PV_ROD_SPEED;PV_METAL_LEVEL_CASTING;METAL_LEVEL_LAUNDER;CHARGE_NUMBER;PV_FLOW_ARGONR1;PV_FLOW_ARGONR2;PV_PRES_REACTOR;PV_LEVEL_LAUNDER;PV_LEVEL_REACTOR;PV_PRES_AIR_EJECTOR;PV_PRES_EJECTOR;SP_FLOW_ARGONR1;SP_FLOW_ARGONR2;SP_PRES_EJECTOR;SP_LEVEL_REAC_OPERATION;";
  output.print(chaineReceptionEnTete);
  output.flush(); // Writes the remaining data to the file
  chaineReceptionEnTeteC=" ;PV CHIMNEY WASTE GAS TEMP;PV_MIDLEROOF_TEMP;PV_LEFT_ROOF_TEMP;PV_ROOF_TEMP;PV_LIMIT_TEMP;PV1_BATH_TEMP;PV2_BATH_TEMP;FURNACE_PRESS_PV;FURNACE_PRESS_PV;CF2_TEMP_PLAF1;CF2_TEMP_PLAF2;CF2_TEMP_BAIN;CF2_GAZ_TOT;CPT_GAZ_CF2_ACT;";    
  output2.print(chaineReceptionEnTeteC);
  output.flush(); // Writes the remaining data to the file
}

//******************************** 
void draw()
{
  d = day();    // Values from 1 - 31
  m = month();  // Values from 1 - 12
  y = year();   // 2003, 2004, 2005, etc.
  s = second();  // Values from 0 - 59
  mi = minute();  // Values from 0 - 59
  h = hour();    // Values from 0 - 23



  lectureBuffer();
}


//**********************
void lectureBuffer() {

  while (myPort.available () > 0) { 
    text("Reception.......", 100, 160);
    octetReception = myPort.readChar(); 

    if (char(octetReception)=='C') Cont=1;

    //println(octetReception);
    if (octetReception == '*') { 


      String sd = String.valueOf(d);
      String sm = String.valueOf(m);
      String sy = String.valueOf(y);
      String ss = String.valueOf(s);
      String smi = String.valueOf(mi);
      String sh = String.valueOf(h);

      chaineReception= chaineReception + sd+"/"+sm+"/"+sy+";"+sh+":"+smi+":"+ss;
      print(chaineReception);


      if (Cont==1) {     
        output2.print(chaineReception);
        output2.flush(); // Writes the remaining data to the file
      } else {
        output.print(chaineReception);
        output.flush(); // Writes the remaining data to the file
      }

      chaineReception = "";
      Cont=0;
    } else { 
      caractereReception = char(octetReception); 
      chaineReception = chaineReception + caractereReception; 
      delay(1); // laisse le temps au caractères d'arriver
    }
  }
}
//*****************************
void AffichageTags(String[] tags) {  // This function also does not return a value

  if (tags[0] !=null) {
  }
}
//*********************************
void mouseClicked() {
  if (mouseX > 32 & mouseY>19 & mouseX<530 & mouseY<73) {
    exit();
    output.flush(); 
    output2.flush();
  }
}

