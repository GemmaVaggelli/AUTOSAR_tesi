#include "aux_functions.h"
////////////////////////////////// Gemma Global Variables Definition -------------------------------

// Define named constants for magic numbers
const uint8_t PACKET_SIZE = 55; // 8 EVENT FRAME + 8 TIMESTAMP + 4 CDL + 32 CD + 2 SL + 1 SIGNATURE
const int TIMESTAMP_BYTES = 8;
const int TIMESTAMP_OFFSET = 8;
const int ANOMALY_INFO_LENGTH = 32;
const int MAX_PACKET_COUNT = 20;
File anomaly_file;
File filter_file;
uint8_t** packets = new uint8_t*[MAX_PACKET_COUNT]; 
int packetIndex = 0;
double filter_time = 0;

MCP2515 mcp2515(9);

//uint8_t** packets;
/////////////////////////////Ruled Based ----------------------------------------------------------------------------------------
            
/************* Global Variables Definition *************/

bool timing_training_complete=false;
struct can_frame canMsg;
bool mes_arr=false;
/***---- Read DBC file-----***/
File file;
int Num_ID;
int Num_ECU;
int Null_line=0;
String ECU[MaxNumEcu];
unsigned long ID_list[MaxNumID];

int DLC_list[MaxNumID];
String ECU_origin[MaxNumID];

/***-----Diagnostic variables-------***/
unsigned long int Diag_List[Num_Diagnostic_ID]={Diag_ID1,Diag_ID2,Diag_ID3,Diag_ID4,Diag_ID5,Diag_ID6,Diag_ID7,Diag_ID8,Diag_ID9,Diag_ID10,Diag_ID11,Diag_ID12,Diag_ID13,Diag_ID14,Diag_ID15,Diag_ID16,Diag_ID17,Diag_ID18,Diag_ID19,Diag_ID20,Diag_ID21,Diag_ID22,Diag_ID23,Diag_ID24,Diag_ID25,Diag_ID26};
bool Diagnostic_session = false; 

/****----Bus load variables-----*/
double bus_time=0;
int bitnumber=0;
double Bus_Load=0;

/***---Time variables---***/
int num_periodic_mess = 0;
double time_mess=0;
double arrival_time_array[MaxNumID];
double init_time=0;
unsigned long Periodic_ID_list[MaxNumID];
int Period_values[MaxNumID];

/*******------------Anomalies log counters---------********/
int Diagnostic_anomaly=0;
unsigned long Diagn_ID_log[LogLength];
unsigned long* P_Diagn_ID_log= &Diagn_ID_log[0];

int ID_anomaly=0;
unsigned long Anomal_ID_log[LogLength];
unsigned long* P_Anomal_ID_log= &Anomal_ID_log[0];

int DLC_anomaly=0;
unsigned long Anomal_DLC_log[LogLength];
unsigned long* P_Anomal_DLC_log= &Anomal_DLC_log[0];

int Rate_anomaly=0;
unsigned long Anomal_Rate_log[LogLength];
unsigned long* P_Anomal_Rate_log = &Anomal_Rate_log[0];


int BusLoad_anomaly = 0;
int velocity_increase_anomaly = 0;

int Counter_anomaly=0;
unsigned long Anomal_Counter_log[LogLength];
unsigned long* P_Anomal_Counter_log = &Anomal_Counter_log[0];

/****** Additional Checks  *****/

/**Vehicle speed**/
unsigned long VehicleSpeed_ID=0;
int BitStart;
int BitLength;
double Scale;
double offset;
int Range[2];   //min value, max value
String MeasureUnit;
int endian=0;
String sign;
int binary[8*maxDlcLength];
int index_array[8*maxDlcLength];
double vel=0;
double vel_prec=0;
double anomal_prev_vel=0;
double anomal_following_vel=0;

/** Messages counter  **/
int MessageCounters[MaxNumID];
int MessageCounters_prec[MaxNumID];
int NumCounters=0;
unsigned long IdCounter[MaxNumID];  //ID possessing counter signal
int BitStartCounter[MaxNumID];
int BitLengthCounter[MaxNumID];
int endianCounter[MaxNumID];
int maxRangeCounter[MaxNumID];


//////////////////////////Time Fingerprinting ----------------------------------------------------------------------------------------------------------

/************* Global Variables Definition *************/

int counterMin;
long int White_List_ID[Num_ID_TFP] ={0x4F4,0x1E2,0x5D6,0x2E0,0X4D8,0x4EE,0x6D6,0x4F0}; 
double my_time; 
double start_time;

int total_rec[Num_ID_TFP];
int total_rec_precedente[Num_ID_TFP];            
int ID_sospesi[Num_ID_TFP];                      
int ID_anomali_media[Num_ID_TFP];               
int ID_anomali_varianza[Num_ID_TFP];            
int* pID_anomali_media = &ID_anomali_media[0];
int* pID_anomali_varianza = &ID_anomali_varianza[0];

double Matrix_Training[Num_ID_TFP][N_Training]; //per accumulo campioni (tempi di arrivo) in fase di training
double Matrix_Test[Num_ID_TFP][N_Test]; //per accumulo campioni (tempi di arrivo) in fase di test

double* pMatrix_Training = &Matrix_Training[0][0];
double* pMatrix_Test     = &Matrix_Test[0][0];

int RecCounter[Num_ID_TFP]; // vettore le cui componenti indicano il numero di messaggi arrivati per ogni ID
int* pRecCounter = &RecCounter[0];

double MatriceDifferenze[Num_ID_TFP][N_Training];
double* pMatriceDifferenze = &MatriceDifferenze[0][0];

double MatrixDiff_Test[Num_ID_TFP][N_Test];
double* pMatrixDiff_Test = &MatrixDiff_Test[0][0];

double VettoreMedia[Num_ID_TFP];
double VettoreVarianza[Num_ID_TFP];

double* pVettoreMedia     = &VettoreMedia[0];
double* pVettoreVarianza  = &VettoreVarianza[0];

double VetMediaTest[Num_ID_TFP];
double VetVarianzaTest[Num_ID_TFP];

double* pVetMediaTest = &VetMediaTest[0];
double* pVetVarianzaTest = &VetVarianzaTest[0];




/////////////////////////////Ruled Based ----------------------------------------------------------------------------------------

/******* Functions definition ******/

void ReadDBC(){
  
   while(file.available()){
  String line = file.readStringUntil('\n');
  //SerialASC.println(line);
  
  String strs[250];
  int StringCount = 0;
  
  while (line.length() > 0){
    int index = line.indexOf(' ');
      
    if (index == -1){//no space found{
      strs[StringCount++] = line;
      break;
    }
    else{
      String auxstr = line.substring(0, index);
      strs[StringCount++]=auxstr;
      String auxstr2 = line.substring(index+1);
      line=auxstr2;
      
    }
  }
  
   if(strs[0]=="BU_:"){
    Num_ECU=StringCount-1;
    
    for(int j=0;j<Num_ECU;j++){
      ECU[j]=strs[j+1];               //centraline presenti nel file.dbc
      
      } 
    }

   if(strs[0]=="BO_"){
    ID_list[Num_ID]=strtoul(strs[1].c_str(),NULL,10);
    DLC_list[Num_ID]=strs[3].toInt();
    ECU_origin[Num_ID++]=strs[4];
    }


    if((strs[0]=="BA_")&&(strs[1]=="\"GenMsgCycleTime\"")){                    //period message values
      Periodic_ID_list[num_periodic_mess]=strtoul(strs[3].c_str(),NULL,10);
      Period_values[num_periodic_mess++]=strs[4].toInt();      // period in ms
      }

/********* Additional Checks **********/

    if((strs[1]=="SG_")&&(strs[2]=="VehicleSpeed")){    //Check for VehicleSpeed signal
      
      int slash = strs[4].indexOf('|'); 
      String string_BitStart=strs[4].substring(0,slash);
      BitStart=string_BitStart.toInt();
      //Serial.print("BitStart: ");Serial.println(BitStart);

      
      int at = strs[4].indexOf('@');
      String string_BitLength=strs[4].substring(slash+1,at);
      BitLength=string_BitLength.toInt();
      //Serial.print("BitLength: ");Serial.println(BitLength);

      String string_endian=strs[4].substring(at+1,at+2);
      endian=string_endian.toInt();

      String string_sign=strs[4].substring(at+2,at+3);
      sign=string_sign;
      
      int comma = strs[5].indexOf(',');
      String string_Scale=strs[5].substring(1,comma);
      Scale =string_Scale.toDouble();

      int parenthesis = strs[5].indexOf(')');
      String string_offset = strs[5].substring(comma+1,parenthesis);
      offset = string_offset.toDouble();
      
      slash = strs[6].indexOf('|');
      String string_RangeL=strs[6].substring(1,slash);
      Range[0]=string_RangeL.toInt();
     
      int bracket=strs[6].indexOf(']');
      String string_RangeU=strs[6].substring(slash+1,bracket);
      Range[1]=string_RangeU.toInt();

      MeasureUnit=strs[7];
      
      VehicleSpeed_ID=ID_list[Num_ID-1];
      }

      if( (strs[1]=="SG_")&&((strs[2]=="MessageCounter")||( strs[2]=="Counter"))  ){     //counter signal
        
         IdCounter[NumCounters]=ID_list[Num_ID-1];
         //Serial.println(IdCounter[NumCounters],HEX);
         int slash = strs[4].indexOf('|');
         String string_BitStart=strs[4].substring(0,slash);
         BitStartCounter[NumCounters]=string_BitStart.toInt();

         int at = strs[4].indexOf('@');
         String string_BitLength=strs[4].substring(slash+1,at);
         BitLengthCounter[NumCounters]=string_BitLength.toInt();
         
         String string_endian=strs[4].substring(at+1,at+2);
         endianCounter[NumCounters]=string_endian.toInt();


         slash = strs[6].indexOf('|');
         int bracket=strs[6].indexOf(']');
         String string_maxRangeCounter=strs[6].substring(slash+1,bracket);
         maxRangeCounter[NumCounters++]=string_maxRangeCounter.toInt();   
        
        }  
  }
}

void PrintFeaturesFromDBC(){
  
   SerialASC.println("--------From dbc file--------");
   SerialASC.print("Number of ECU: ");
   SerialASC.println(Num_ECU);
   SerialASC.print("ECU list: ");
   for(int j=0;j<Num_ECU;j++){
      SerialASC.print(ECU[j]);SerialASC.print("  ");
      }
   SerialASC.println("");
   SerialASC.print("Number of ID: ");  
   SerialASC.println(Num_ID);
   SerialASC.print("ID list: ");
   for(int j=0;j<Num_ID;j++){
      SerialASC.print(ID_list[j],HEX);SerialASC.print("  ");
      }
   SerialASC.println("");   
   SerialASC.print("DLC list: ");
   for(int j=0;j<Num_ID;j++){
      SerialASC.print(DLC_list[j]);SerialASC.print("  ");
      } 
   SerialASC.println("");   
   SerialASC.print("ECU origin list: ");
   for(int j=0;j<Num_ID;j++){
      SerialASC.print(ECU_origin[j]);SerialASC.print("  ");
      }
   SerialASC.println("");
   if(NumCounters!=0){   
     SerialASC.println("ID possessing the counter signal:");
     for(int i=0;i<NumCounters;i++){
       SerialASC.print("0x");SerialASC.print(IdCounter[i],HEX);SerialASC.print(" ");
     }
   }
   SerialASC.println("");
   if(VehicleSpeed_ID != 0){
    SerialASC.print("ID possessing the speed vehicle signal: 0x");SerialASC.println(VehicleSpeed_ID,HEX);
    }
  SerialASC.println("Period specified for ID:");
   for(int p=0;p<num_periodic_mess;p++){
      SerialASC.print("0x");SerialASC.print(Periodic_ID_list[p],HEX);SerialASC.print(" Period: ");SerialASC.print(Period_values[p]);SerialASC.println("ms");
      }

  
 }


void Check_Diagnostic_anomaly(unsigned long canId, unsigned char buf[8] ){
  
  if(Diagnostic_session==false){
    for(int j = 0; j < Num_Diagnostic_ID; j++){
      if(canId==Diag_List[j]){                         //Diagnostic ID occurs during NOT Diagnostic session
      Diagnostic_anomaly++;
      
      /* Creazione pacchetto */
      String anomaly_info = (String)canId;
      uint8_t* packet = create_packet(0x8000,0b1000, anomaly_info, packet);
      insert_packet(packet);
      
      if(Diagnostic_anomaly%LogLength==0){
        *(P_Diagn_ID_log)=canId;
        P_Diagn_ID_log=&Diagn_ID_log[0];
        }
        else{
            *(P_Diagn_ID_log)=canId;
            P_Diagn_ID_log++;
           }  
      }
    }
   } 
 }


void Check_ID_anomaly(unsigned long canId){

    for(int i = 0; i < Num_ID; i++){
      if(canId==ID_list[i]||canId==0){    //ID 0x00 is not considered anomalous
        return;// nullptr;
        }
      }
     for(int j = 0; j < Num_Diagnostic_ID; j++){
       if(canId==Diag_List[j]){  
        return; // nullptr;
        }
      }
   ID_anomaly++;
   Serial.println(canId, BIN);
   
   /* Creazione pacchetto */
   String anomaly_info = (String)canId;
   uint8_t* packet = create_packet(0x8001,0b1000, anomaly_info, packet);
   insert_packet(packet);

   if(ID_anomaly%LogLength==0){
     *(P_Anomal_ID_log)=canId;
     P_Anomal_ID_log=&Anomal_ID_log[0];
    }
    else{
        *(P_Anomal_ID_log)=canId;
        P_Anomal_ID_log++;
       }  
 }

void Check_DLC_anomaly(unsigned long canId,byte len){

  if(ID_anomaly !=0 ){
    return;// nullptr;
    }
  
  for(int j = 0; j < Num_Diagnostic_ID; j++){
       if(canId==Diag_List[j]){                 //if diagnostic ID the DLC is not checked
        return;// nullptr;
        }
   } 
     
  int current_DLC=len;
  int index=0;
  while(canId != ID_list[index]){
    index++;
    if(index==Num_ID){
      return;// nullptr;
      }
    }
    
    if(current_DLC != DLC_list[index]){
      DLC_anomaly++;

      /* Creazione pacchetto */
      String anomaly_info = (String)canId;
      uint8_t* packet = create_packet(0x8002,0b1000, anomaly_info, packet);
      insert_packet(packet);
      
      if(DLC_anomaly%LogLength==0){
        *(P_Anomal_DLC_log)=canId;
        P_Anomal_DLC_log=&Anomal_DLC_log[0];
      }
      else{
          *(P_Anomal_DLC_log)=canId;
          P_Anomal_DLC_log++;
         }
     }
     
  }

void Check_Rate_anomaly(unsigned long canId,double time_mess){

/*

  if(ID_anomaly !=0 ){return;}
    //Serial.print("Rate check ");Serial.println(canId,HEX);
  for(int j = 0; j < Num_Diagnostic_ID; j++){
       if(canId==Diag_List[j]){                 //if diagnostic ID the rate is not checked
        return;
        }
   }
  */
    int index=0;
    while(Periodic_ID_list[index] != canId){
      index++;
      if(index==num_periodic_mess+1){return; //nullptr;
      }
      }

     double time_elapsed=0;
      
     if(arrival_time_array[index]==0){
      arrival_time_array[index]=time_mess;
      //return nullptr;
      return;
      }
      else{
        time_elapsed=time_mess-arrival_time_array[index];
        //mSerial.print(canId,HEX);Serial.print(" time_elapsed: ");Serial.println(time_elapsed);
        arrival_time_array[index]=time_mess;
        }

        if (Period_values[index] !=0 ){    //check only periodic messages

         if(Period_values[index]>30){
          if((time_elapsed<(1-0.2)*Period_values[index])||(time_elapsed>(1+0.2)*Period_values[index])){  // +/-20% of nominal value
            Rate_anomaly++;
            
            /* Crezione Pacchetto */
            String time_elapsed_str = (String)time_elapsed;
            String index_str = String(Periodic_ID_list[index],HEX);
            String anomaly_info = time_elapsed_str + " and 0x" + index_str;
            uint8_t* packet = create_packet(0x8004, 0b1000, anomaly_info, packet);
            insert_packet(packet);

            if(Rate_anomaly%LogLength==0){
               *(P_Anomal_Rate_log)=canId;
               P_Anomal_Rate_log=&Anomal_Rate_log[0];
              }
              else{
                  *(P_Anomal_Rate_log)=canId;
                  P_Anomal_Rate_log++;
                }       
            }//if(time_elapsed..)
          }//if(Period_values[index]>30)
          
          else{        //if period is lower than 30ms threshold is bigger 
            
            if((time_elapsed<(1-0.4)*Period_values[index])||(time_elapsed>(1+0.4)*Period_values[index])){  // +/-40% of nominal value
            Rate_anomaly++;

            /* Creazione Pacchetto */
            String time_elapsed_str = (String)time_elapsed;
            String index_str = String(Periodic_ID_list[index],HEX);
            String anomaly_info = time_elapsed_str + " and 0x"+ index_str;
            uint8_t* packet = create_packet(0x8004, 0b1000, anomaly_info, packet);
            insert_packet(packet);
            
            if(Rate_anomaly%LogLength==0){
               *(P_Anomal_Rate_log)=canId;
               P_Anomal_Rate_log=&Anomal_Rate_log[0];
              }
              else{
                  *(P_Anomal_Rate_log)=canId;
                  P_Anomal_Rate_log++;
                } 
                    //return packet;    
            }//if(time_elapsed..)   
           }//else
            
         }//if(Period_values[index] !=0)

      //Serial.print("ID received: 0x");Serial.print(canId,HEX);Serial.print(" time_elapsed: ");Serial.print(time_elapsed);Serial.print("Period expected: ");Serial.println(Period_values[index]);    

  }


void Check_BusLoad_anomaly(){

  //Serial.print("bitnumber: ");Serial.println(bitnumber);
  Bus_Load=((float)bitnumber/Bitrate)*100; //percentage of Bus Load
  if(Bus_Load > 80){ //80% max
    BusLoad_anomaly++;

    /* Creazione Pacchetto */
    String anomaly_info = (String)Bus_Load ;
    uint8_t* packet = create_packet(0x8005, 0b1000, anomaly_info,  packet);
    insert_packet(packet);
    
    }
    
  bitnumber=0;
  bus_time=millis();

  
  }

void Check_Counter_anomaly(unsigned long canId,byte len, unsigned char buf[8] ){

  int indexID=0;
   while(IdCounter[indexID] != canId){
    indexID++;
    if(indexID ==(NumCounters+1)){
      return;// nullptr;
      }
    }
    if(BitStartCounter[indexID]>(len*8)){   //dlc anomalous 
      return;// nullptr;
      }
    
    for(int p=0; p<len; p++){
          for(int s=0; s<8 ;s++){
            binary[s+p*8]=bitRead(buf[p], 7-s); //reading dataframe bits of the message
            //Serial.print(binary[s+p*8]);
          }
          //Serial.print("|");
          }
          //Serial.println(" ");
           
   int index=0;
   while (BitStartCounter[indexID] != index_array[index] ){
    index++;
   }
 
   //Serial.print("indexID: ");Serial.println(indexID);
   //Serial.print("BitStart:");Serial.print(BitStartCounter[indexID]); Serial.print(" BitLength:");Serial.println(BitLengthCounter[indexID]);

   if(endian==0){       //most significant byte first
   for(int j=0; j<BitLengthCounter[indexID] ;j++){
     MessageCounters[indexID]=MessageCounters[indexID]+binary[index+j]*pow(2,BitLengthCounter[indexID]-1-j);
     //Serial.print(binary[index+j]);Serial.print("-");
   } 
   //Serial.print("counter attuale: ");  
   //Serial.println(MessageCounters[indexID]);
  }

if(endian==1){    //least significant byte first

   int bitLengthCounterTmp = BitLengthCounter[indexID];
   int esp=0;

    while(bitLengthCounterTmp != 0){
      
        //Serial.print(" indice:");Serial.print(index);
        if(index %8 ==0){
          MessageCounters[indexID]=MessageCounters[indexID]+binary[index]*pow(2,esp++);
          index=index+(2*8)-1;
          }
          else{
            MessageCounters[indexID]=MessageCounters[indexID]+binary[index--]*pow(2,esp++);
            }

      bitLengthCounterTmp--;
      }
         
   } 

  if(MessageCounters_prec[indexID]== -2){
    MessageCounters_prec[indexID]=MessageCounters[indexID];
    MessageCounters[indexID]=0;
    return;// nullptr;
    }
 
  if((MessageCounters_prec[indexID]==maxRangeCounter[indexID])&&(MessageCounters[indexID]!=0)){  //max value of the counter

      Counter_anomaly++;

      /* Creazione Pacchetto */
      String anomaly_info = String(indexID,HEX);
      uint8_t* packet = create_packet(0x8006, 0b1000, anomaly_info, packet);
      insert_packet(packet);
      
      if(Counter_anomaly%LogLength==0){
        *(P_Anomal_Counter_log)=canId;
        P_Anomal_Counter_log=&Anomal_Counter_log[0];
        }
      else{
        *(P_Anomal_Counter_log)=canId;
        P_Anomal_Counter_log++;
        }     
    }
  else if(  MessageCounters[indexID]!=(MessageCounters_prec[indexID]+1) &&(MessageCounters_prec[indexID]!=maxRangeCounter[indexID])  ){
    //Serial.print("Messagecounter prec: ");Serial.println(MessageCounters_prec[indexID]);
    //Serial.print("Messagecounter:" );Serial.println(MessageCounters[indexID]);

      Counter_anomaly++;

      /* Creazione Pacchetto */
      String anomaly_info = String(indexID,HEX);
      uint8_t* packet = create_packet(0x8006, 0b1000, anomaly_info, packet);
      insert_packet(packet);
      
      if(Counter_anomaly%LogLength==0){
        *(P_Anomal_Counter_log)=canId;
        P_Anomal_Counter_log=&Anomal_Counter_log[0];
      }
      else{
        *(P_Anomal_Counter_log)=canId;
        P_Anomal_Counter_log++;
        }
    }

    MessageCounters_prec[indexID]=MessageCounters[indexID];
    MessageCounters[indexID]=0;
  
  }


void Additional_Checks(unsigned long canId,byte len, unsigned char buf[8]){
  
  if (VehicleSpeed_ID != 0){   //if this ID is == 0 there is no information about speed on DBC file
    
      if(canId==VehicleSpeed_ID){
        for(int p=0; p<len; p++){
          for(int s=0; s<8 ;s++){
            binary[s+p*8]=bitRead(buf[p], 7-s);
            //Serial.print(binary[s+p*8]);
          }
          //Serial.print("|");
          }
          //Serial.println(" ");
           
   int index=0;
   while (BitStart != index_array[index] ){
    index++;
   }
   //Serial.print("index: ");Serial.println(index);
   //Serial.print("BitStart:");Serial.print(BitStart); Serial.print(" BitLength:");Serial.println(BitLength);

  if(endian==0){       //most significant byte first
   for(int j=0; j<BitLength ;j++){
     vel=vel+binary[index+j]*pow(2,BitLength-1-j);
     //Serial.print(binary[index+j]);Serial.print("-");
   } 
   //Serial.println();
     
  } 

  if(endian==1){    //least significant byte first

   int bitLengthCounter = BitLength;
   int esp=0;

    while(bitLengthCounter != 0){
      
        //Serial.print(" indice:");Serial.print(index);

        if(index %8 ==0){
          vel=vel+binary[index]*pow(2,esp++);
          index=index+(2*8)-1;
          }
          else{
            vel=vel+binary[index--]*pow(2,esp++);
            }
      bitLengthCounter--;
      }      
   }
 //Serial.print("velocità: ");Serial.println(vel*Scale+offset);

 if(((vel*Scale+offset)-vel_prec)>50){
  velocity_increase_anomaly++;

  /* Creazione Pacchetto */
  String anomaly_info = (String)(vel*Scale+offset);
  uint8_t* packet = create_packet(0x8006, 0b1000, anomaly_info , packet);
  insert_packet(packet);

  anomal_prev_vel = vel_prec;
  anomal_following_vel = vel*Scale+offset;
  //return packet; 
  //Serial.println("Anomalous speed increase!");
  //Serial.print("Previous speed: ");Serial.print(vel_prec);Serial.print(" Current speed:");Serial.println(vel*Scale+offset);
  }
 vel_prec = vel*Scale+offset;
 vel=0;
 
   }
 }  
   
}


void PrintAnomalies(){
      Serial.println("[RULE BASED]");
      
      Serial.println("1)Diagnostic ID received during NOT diagnostic session: ");
      if(Diagnostic_anomaly<LogLength){
        for(int p = 0; p < Diagnostic_anomaly; p++){
          Serial.print("0x");
          Serial.println(Diagn_ID_log[p],HEX);
         }
       }
       else{
        for(int p = 0; p < LogLength; p++){
          Serial.print("0x");
          Serial.println(Diagn_ID_log[p],HEX);
         }
        }
       Diagnostic_anomaly=0;
       P_Diagn_ID_log=&Diagn_ID_log[0];
  
       for(int r = 0; r < LogLength; r++){
          Diagn_ID_log[r]=0;
         }
                    
      Serial.println("2)ID not belonging to white list received:");
      if(ID_anomaly<LogLength){
        for(int s = 0; s < ID_anomaly; s++){
          Serial.print("0x");
          Serial.println(Anomal_ID_log[s],HEX);
          }
       }
      else{
       for(int s = 0; s < LogLength; s++){
          Serial.print("0x");
          Serial.println(Anomal_ID_log[s],HEX);
          }
        }
       ID_anomaly=0;      
       P_Anomal_ID_log=&Anomal_ID_log[0];
         
      Serial.println("3)Data length code anomalous for ID:");
      if(DLC_anomaly<LogLength){
        for(int c = 0; c < DLC_anomaly; c++){
          Serial.print("0x");
          Serial.println(Anomal_DLC_log[c] ,HEX);
         }
      }
      else{
            for(int c = 0; c < LogLength; c++){
          Serial.print("0x");
          Serial.println(Anomal_DLC_log[c] ,HEX);
         }
        }
        DLC_anomaly=0;
        P_Anomal_DLC_log=&Anomal_DLC_log[0];
      
        for(int r = 0; r < LogLength; r++){
          Anomal_DLC_log[r]=0;
         }
      
        
      Serial.println("4)Rate anomaly for ID:");
      if(Rate_anomaly<LogLength){
        for(int d = 0; d < Rate_anomaly; d++){
         Serial.print("0x");
         Serial.println(Anomal_Rate_log[d],HEX);
         }
      }
      else{
        for(int d = 0; d < LogLength; d++){
         Serial.print("0x");
         Serial.println(Anomal_Rate_log[d],HEX);
         }
       } 
       Rate_anomaly=0;
       P_Anomal_Rate_log=&Anomal_Rate_log[0];
       
       
       for(int r = 0; r < LogLength; r++){
          Anomal_Rate_log[r]=0;
         }
      
     
      Serial.println("5)Bus Load anomaly:");
      if(BusLoad_anomaly !=0 ){
        Serial.println("Bus Load greater than 80%");
        }
      BusLoad_anomaly=0;
      
       Serial.println("6)Counter anomaly for ID:");
       if(Counter_anomaly<LogLength){
         for(int c = 0; c < Counter_anomaly; c++){
          Serial.print("0x");
          Serial.println(Anomal_Counter_log[c],HEX);
         }
       }
       else{
            for(int c = 0; c < LogLength; c++){
             Serial.print("0x");
             Serial.println(Anomal_Counter_log[c],HEX);
        }
       }
       Counter_anomaly=0;
       P_Anomal_Counter_log=&Anomal_Counter_log[0];


      if (VehicleSpeed_ID != 0){
      Serial.println("7)Vehicle speed signal anomaly:");
      if(velocity_increase_anomaly != 0){
        Serial.print("Previous speed: ");Serial.print(anomal_prev_vel);Serial.print("  Following speed:");Serial.println(anomal_following_vel);
        }
      velocity_increase_anomaly=0;
      } 
    Serial.println("-----------------");
    init_time=millis();
  }

//////////////////////////Time Fingerprinting ----------------------------------------------------------------------------------------------------------

/******* Functions definition ******/


int FindIndexInList(long int List_ID[Num_ID_TFP], unsigned long int msg_id){
  
  for(int i = 0; i < Num_ID_TFP; i++){
    if(msg_id == List_ID[i]){
      return i;
    }
  }
  return NOT_RECOGNISE;   
}

void CounterUpdate(int indexID, int* pRecCounter, int N){
  if((*(pRecCounter + indexID)) < N){
    (*(pRecCounter + indexID))++;
  }else{
    (*(pRecCounter + indexID)) = N + 1; // così non stampa all'infinito nella fase di debug
  }
}


int trovaMinimo(int VettoreContatori[Num_ID_TFP]){
  int valMin = VettoreContatori[0];
  for(int i = 0; i< Num_ID_TFP; i++){
    if(VettoreContatori[i] < valMin){
      valMin = VettoreContatori[i];
    }
  }
  return valMin;
}

void creaMatriceDifferenze(double* MatriceDifferenze, double* MatriceDati){
  for(int i = 0; i < Num_ID_TFP; i++){
    for(int j = 0; j < N_Training; j++){
      *(MatriceDifferenze + N_Training*i + j) = *(MatriceDati + N_Training*i + j) - *(MatriceDati + N_Training*i + (j-1));
    }
  }
}

void calcoloStatistiche(double* pMatriceDifferenze, double* pVettoreMedia, double* pVettoreVarianza){
  int j_start = 10;

  /* calcolo delle medie campionarie per ogni ID */
  
  for(int i = 0; i < Num_ID_TFP; i++){
    for(int j = j_start; j < N_Training; j++){
      *(pVettoreMedia + i) = *(pVettoreMedia + i) + *(pMatriceDifferenze + N_Training*i + j)/(N_Training - j_start);
    }
  }

  /* calcolo varianze campionarie per ogni ID */

  for(int i = 0; i < Num_ID_TFP; i++){
    for(int j = j_start; j < N_Training; j++){
      *(pVettoreVarianza + i) = *(pVettoreVarianza + i) + (pow(*(pMatriceDifferenze + N_Training*i + j) - *(pVettoreMedia + i) ,2))/(N_Training - j_start - 1);
    }
  }
}

void stampaStatistiche(double VettoreMedia[Num_ID_TFP], double VettoreVarianza[Num_ID_TFP], long int White_List_ID[Num_ID_TFP]){
  for(int i = 0; i < Num_ID_TFP; i++){
    SerialASC.print("--- ID 0x"); SerialASC.print(White_List_ID[i], HEX);  SerialASC.println(" ---");
    SerialASC.print(" Average value: "); SerialASC.print(VettoreMedia[i]); SerialASC.print(" -- "); SerialASC.print(" Variance: "); SerialASC.println(VettoreVarianza[i]);
  }
}

void riempiRigaMatrice(int riga, int colonna, double my_time, double* pMatrix_Training){
  *(pMatrix_Training + N_Training*riga + colonna) = my_time;
}

void aggiornaRigaDiff(int rigaID, int dimCol, double* pMatrix, double* pMatrixDiff){
  for(int j = 0; j < dimCol; j++){
    *(pMatrixDiff + dimCol*rigaID + j) = *(pMatrix + dimCol*rigaID + j) - *(pMatrix + dimCol*rigaID + (j-1));
  }
}

void calcolaStatisticheRiga(int index_riga, int dimCol, double* pMatrixDiff, double* pVetMedia, double* pVetVarianza){
  double media = 0.0; // queste 2 sono solo var. di "appoggio"
  double varianza = 0.0;
  int j_start = 10; // per sicurezza escludo le prime misure
  for(int j = j_start; j < dimCol; j++){
    media = media +  *(pMatrixDiff + dimCol*index_riga + j)/(dimCol - j_start);
  }
  *(pVetMedia + index_riga) = media; // aggiorno la componente del vettore delle medie relativo all'ID indicato da index_riga
  for(int j = j_start; j < dimCol; j++){
    varianza = varianza + pow( *(pMatrixDiff + dimCol*index_riga + j) - media , 2)/(dimCol - j_start - 1);
  }
  *(pVetVarianza + index_riga) = varianza; // aggiorno vettore varianze
}

int CheckMediaRangeRaw(double mu_train, double mu_test,int IDtrovato){            //ritorna 1 se sta dentro il range

  if((mu_test >= mu_train*(1 - 0.1))&&(mu_test <= mu_train*(1 + 0.1))){
    return 1;
  }                             //intervallo al 10% del valore di training
  else{
    return 0;
  }
}

int CheckVarianzaRangeRaw(double var_train, double var_test){     //ritorna 1 se sta dentro il range

  if((var_test >= var_train*(1 - 0.1))&&(var_test <= var_train*(1 + 0.1))){
    return 1;
  }
  else{
    return 0;
  }
}

void Period_Check(int IDtrovato, int checkmedia, int checkvarianza){
    if(checkmedia==0){
      *(pID_anomali_media + IDtrovato)= *(pID_anomali_media + IDtrovato)+1;
    }
    else{ 
      *(pID_anomali_media + IDtrovato)=0;
    }
    if(checkvarianza==0){
      *(pID_anomali_varianza + IDtrovato)= *(pID_anomali_varianza + IDtrovato)+1;
    }
    else{ 
      *(pID_anomali_varianza + IDtrovato)=0;
    }
     for (int j=0;j<Num_ID_TFP;j++){
    if (*(pID_anomali_media + j)>=3){
      if(IDtrovato==j){
        SerialASC.println("[TIME FINGERPRINTING]");
        SerialASC.print("Period anomaly detected for ID 0x");SerialASC.println(White_List_ID[j],HEX);
        
        /* Creazione Pacchetto */
        String anomaly_info = String(White_List_ID[j],HEX);
        uint8_t* packet = create_packet(0x8007, 0b1000, anomaly_info, packet);
        insert_packet(packet);
        
        SerialASC.print("Average value measured = ");SerialASC.println(VetMediaTest[j]);
        if (*(pID_anomali_varianza + j)>=3){
           SerialASC.println("Variance is anomalous as well");SerialASC.println();

          /* Creazione Pacchetto */
          uint8_t* packet = create_packet(0x8007, 0b1000, "Variance is anomalous too", packet);
          insert_packet(packet);
          
         }
        }
      }
    }
  }


void azzeraRigaMatrice(int index_riga, double* pMatrix, int dimCol){
  for(int j = 0; j < dimCol; j++){
    *(pMatrix + dimCol*index_riga + j) = 0.00;  
  }
}

void Suspension_Check(){
  for(int i=0;i<Num_ID_TFP;i++){
        if(total_rec_precedente[i]==total_rec[i]){  //ID sospeso poichè il contatore non è stato incrementato
          ID_sospesi[i]++;
        }
      }
      for(int i=0;i<Num_ID_TFP;i++){
        total_rec_precedente[i]=total_rec[i];
        }
      start_time=millis();  
      for(int i=0;i<Num_ID_TFP;i++){
        if(ID_sospesi[i]!=0){
          SerialASC.println("[TIME FINGERPRINTING check]");
          SerialASC.print("Suspended ID 0x");SerialASC.println(White_List_ID[i],HEX);SerialASC.println();
          
          /* Creazione Pacchetto */
          String anomaly_info = String(White_List_ID[i],HEX);
          uint8_t* packet = create_packet(0x8008, 0b1000, anomaly_info , packet);
          insert_packet(packet);
          
          ID_sospesi[i]=0;
          }
        }
  }

//////////////////////////Voltage Fingerprinting ----------------------------------------------------------------------------------------------------------

float misure_CANH[400];
float misure_CANL[400];
float misure_DIFF[400];

float dominanti_H[400];
float dominanti_L[400];
float dominanti_D[400];

float CANH_tmp = 0.0;
float CANL_tmp = 0.0;
float DIFF_tmp = 0.0;

float CANH_avg=0;
float CANL_avg=0;
float diff_avg=0;

float CANH_ECUA_training=0;
float CANL_ECUA_training=0;
float diff_ECUA_training=0;

float CANH_ECUB_training=0;
float CANL_ECUB_training=0;
float diff_ECUB_training=0;

float CANH_ECUC_training=0;
float CANL_ECUC_training=0;
float diff_ECUC_training=0;

float CANH_ECUD_training=0;
float CANL_ECUD_training=0;
float diff_ECUD_training=0;

float CANH_ECUE_training=0;
float CANL_ECUE_training=0;
float diff_ECUE_training=0;

float CANH_ECUF_training=0;
float CANL_ECUF_training=0;
float diff_ECUF_training=0;

float CANH_ECUG_training=0;
float CANL_ECUG_training=0;
float diff_ECUG_training=0;

float CANH_ECUH_training=0;
float CANL_ECUH_training=0;
float diff_ECUH_training=0;




int actual_measurements = 0;
int maxValue = 0;
int maxIndex = 0;
int idle_cnt = 0; // mi serve per contare quanti campioni di idle leggo, e se questo numero è >= a una certa soglia, riconosco l'idle


double features[15] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; 
unsigned int id[8] = {0x4F4,0x1E2,0x5D6,0x2E0,0x4D8,0x4EE,0x6D6,0x4F0};
int voto[8];
byte ID_A_NOM[35] = {0,0, 1,1,1, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 0,0,0, 0,0,0}; //Arduino vecchio 0x4F4 
byte ID_B_NOM[35] = {0,0, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 0,0,0, 0,0,0, 1,1,1, 0,0,0}; //Arduino nuovo 0x1E2
byte ID_C_NOM[35] = {0,0, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 0,0,0}; 
byte ID_D_NOM[35] = {0,0, 0,0,0, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0};
byte ID_E_NOM[35] = {0,0, 1,1,1, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 0,0,0, 0,0,0, 0,0,0}; 
byte ID_F_NOM[35] = {0,0, 1,1,1, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 0,0,0}; 
byte ID_G_NOM[35] = {0,0, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 0,0,0, 1,1,1, 0,0,0, 1,1,1, 1,1,1, 0,0,0}; 
byte ID_H_NOM[35] = {0,0, 1,1,1, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0};

int DIFF_app[35];
int cnt = 0;
byte err_cnt_A = 0;
byte err_cnt_B = 0;
byte err_cnt_C = 0;
byte err_cnt_D = 0;
byte err_cnt_E = 0;
byte err_cnt_F = 0;
byte err_cnt_G = 0;
byte err_cnt_H = 0;

char classe;
double score[8];

byte centralina = 1;  

bool measures_ready=false;

float dist[8];
  

float CANH_test=0;
float CANL_test=0;
float diff_ECU_test=0;

int anomalies_A=0;
char log_A[ANOMALY_THRESHOLD];
int anomalies_B=0;
char log_B[ANOMALY_THRESHOLD];
int anomalies_C=0;
char log_C[ANOMALY_THRESHOLD];
int anomalies_D=0;
char log_D[ANOMALY_THRESHOLD];
int anomalies_E=0;
char log_E[ANOMALY_THRESHOLD];
int anomalies_F=0;
char log_F[ANOMALY_THRESHOLD];
int anomalies_G=0;
char log_G[ANOMALY_THRESHOLD];
int anomalies_H=0;
char log_H[ANOMALY_THRESHOLD];

char anomal_classe=0;

//ECU distances during traing
float dist_AB=0;
float dist_AC=0;
float dist_AD=0;
float dist_AE=0;
float dist_AF=0;
float dist_AG=0;
float dist_AH=0;

float dist_BC=0;
float dist_BD=0;
float dist_BE=0;
float dist_BF=0;
float dist_BG=0;
float dist_BH=0;

float dist_CD=0;
float dist_CE=0;
float dist_CF=0;
float dist_CG=0;
float dist_CH=0;

float dist_DE=0;
float dist_DF=0;
float dist_DG=0;
float dist_DH=0;

float dist_EF=0;
float dist_EG=0;
float dist_EH=0;

float dist_FG=0;
float dist_FH=0;

float dist_GH=0;

char incoming_char;
bool voltage_training_complete=false;

bool online_phase=false;

/******* Functions definition ******/
void measure_features(){
  CANH_tmp = (((float)(ReadAD0()))/4095)*VIN; 
  CANL_tmp = (((float)(ReadAD1()))/4095)*VIN;
  DIFF_tmp = CANH_tmp - CANL_tmp;

  if(DIFF_tmp < 1){ // vuol dire che sono in idle
    idle_cnt++;
    if(idle_cnt == IDLE_THRESHOLD){ // Ho contato abbastanza campioni di IDLE per poter dire che sono effettivamente in idle/EOF => il prossimo valore alto sarà un SOF 
      idle_cnt = 0;
      do{
        CANH_tmp = (((float)(ReadAD0()))/4095)*VIN; // leggo dagli ADC
        CANL_tmp = (((float)(ReadAD1()))/4095)*VIN;
        DIFF_tmp = CANH_tmp - CANL_tmp;
      }
      while(DIFF_tmp < 1);          
      for(int x = 0; x < 400; x++){
        misure_CANH[x] = (((float)(ReadAD0()))/4095)*VIN; // leggo dagli ADC
        misure_CANL[x] = (((float)(ReadAD1()))/4095)*VIN;
        misure_DIFF[x] = misure_CANH[x] - misure_CANL[x];
      }
      for(int i = 0; i < 400;i++){
        if( (misure_CANH[i] < 3.5)&&(misure_CANH[i] > 2.8 ) && (misure_CANL[i] < 2)&&(misure_CANL[i] > 1) ){ //filtraggio dei dominanti
          dominanti_H[actual_measurements] = misure_CANH[i];
          dominanti_L[actual_measurements] = misure_CANL[i];
          dominanti_D[actual_measurements] = misure_DIFF[i];
          actual_measurements++;
        }
      }
      measures_ready=true;      
            
    }//idle threshold
  }//diff_TMP<1 
 }

void check_classe(){
  
   dist[0]=sqrt(  pow((CANH_test-CANH_ECUA_training),2) + pow((CANL_test-CANL_ECUA_training),2) + pow((diff_ECU_test-diff_ECUA_training),2)   );
   dist[1]=sqrt(  pow((CANH_test-CANH_ECUB_training),2) + pow((CANL_test-CANL_ECUB_training),2) + pow((diff_ECU_test-diff_ECUB_training),2)   );
   dist[2]=sqrt(  pow((CANH_test-CANH_ECUC_training),2) + pow((CANL_test-CANL_ECUC_training),2) + pow((diff_ECU_test-diff_ECUC_training),2)   );
   dist[3]=sqrt(  pow((CANH_test-CANH_ECUD_training),2) + pow((CANL_test-CANL_ECUD_training),2) + pow((diff_ECU_test-diff_ECUD_training),2)   );
   dist[4]=sqrt(  pow((CANH_test-CANH_ECUE_training),2) + pow((CANL_test-CANL_ECUE_training),2) + pow((diff_ECU_test-diff_ECUE_training),2)   );
   dist[5]=sqrt(  pow((CANH_test-CANH_ECUF_training),2) + pow((CANL_test-CANL_ECUF_training),2) + pow((diff_ECU_test-diff_ECUF_training),2)   );
   dist[6]=sqrt(  pow((CANH_test-CANH_ECUG_training),2) + pow((CANL_test-CANL_ECUG_training),2) + pow((diff_ECU_test-diff_ECUG_training),2)   );
   dist[7]=sqrt(  pow((CANH_test-CANH_ECUH_training),2) + pow((CANL_test-CANL_ECUH_training),2) + pow((diff_ECU_test-diff_ECUH_training),2)   );

  /*
  Serial.print("dist_A: ");Serial.println(dist[0],4);
  Serial.print("dist_B: ");Serial.println(dist[1],4);
  Serial.print("dist_C: ");Serial.println(dist[2],4);
  Serial.print("dist_D: ");Serial.println(dist[3],4);
  Serial.print("dist_E: ");Serial.println(dist[4],4);
  Serial.print("dist_F: ");Serial.println(dist[5],4);
  Serial.print("dist_G: ");Serial.println(dist[6],4);
  Serial.print("dist_H: ");Serial.println(dist[7],4);
  */

 int min_index=0;
 float min_dist=dist[0];

for(int j=0; j<8; j++){
  if(dist[j] < min_dist){
    min_dist = dist[j];
    min_index = j;
    }
  }

switch(min_index){
  case 0:
  classe='A';
  break;
  case 1:
  classe='B';
  break;
  case 2:
  classe='C';
  break;
  case 3:
  classe='D';
  break;
  case 4:
  classe='E';
  break;
  case 5:
  classe='F';
  break;
  case 6:
  classe='G';
  break;
  case 7:
  classe='H';
  break;  
  }
//Serial.print("Classe: ");Serial.println(classe);
}

void print_anomalies(){
/*  
Serial.print("err_cnt_A: ");Serial.println(err_cnt_A);
Serial.print("err_cnt_B: ");Serial.println(err_cnt_B);
Serial.print("err_cnt_C: ");Serial.println(err_cnt_C);
Serial.print("err_cnt_D: ");Serial.println(err_cnt_D);
Serial.print("err_cnt_E: ");Serial.println(err_cnt_E);
Serial.print("err_cnt_F: ");Serial.println(err_cnt_F);
Serial.print("err_cnt_G: ");Serial.println(err_cnt_G);
Serial.print("err_cnt_H: ");Serial.println(err_cnt_H);
Serial.print("classe: ");Serial.println(classe);
Serial.println("----------");
*/


if((id[maxIndex] == 0x4F4) && (classe != 'A')){
        log_A[err_cnt_A++]=classe;
        if (err_cnt_A == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_A);
          if(anomal_classe!=0){
            Serial.println("[VOLTAGE FINGERPRINTING]");
            Serial.print("Rilevato ID 0x4F4 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU A");
            Serial.println();
            
            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x4F4: ECU A", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_A[p]=0;
            }
                     
            err_cnt_A = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x4F4) && (classe == 'A')){
        err_cnt_A = 0;
      }


      
      if((id[maxIndex] == 0x1E2) && (classe != 'B')){
        log_B[err_cnt_B++]=classe;
        if (err_cnt_B == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_B);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x1E2 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU B");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x1E2: ECU B", packet );
            insert_packet(packet);
           
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_B[p]=0;
            }
                     
            err_cnt_B = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x1E2) && (classe == 'B')){
        err_cnt_B = 0;
      }


      if((id[maxIndex] == 0x5D6) && (classe != 'C')){
        log_C[err_cnt_C++]=classe;
        if (err_cnt_C == ANOMALY_THRESHOLD){
        
          Log_queue_check(log_C);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x5D6 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU C");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x5D6: ECU C", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_C[p]=0;
            }
                     
            err_cnt_C = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x5D6) && (classe == 'C')){
        err_cnt_C = 0;
      }


      if((id[maxIndex] == 0x2E0) && (classe != 'D')){
        log_D[err_cnt_D++]=classe;
        if (err_cnt_D == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_D);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x2E0 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU D");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x2E0: ECU D", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_D[p]=0;
            }
                     
            err_cnt_D = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x2E0) && (classe == 'D')){
        err_cnt_D = 0;
      }


      if((id[maxIndex] == 0x4D8) && (classe != 'E')){
        log_E[err_cnt_E++]=classe;
        if (err_cnt_E == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_E);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x4D8 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU E");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x4D8: ECU E" , packet);
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_E[p]=0;
            }
                     
            err_cnt_E = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x4D8) && (classe == 'E')){
        err_cnt_E = 0;
      }



      if((id[maxIndex] == 0x4EE) && (classe != 'F')){
        log_F[err_cnt_F++]=classe;
        if (err_cnt_F == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_F);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x4EE proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU F");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x4EE: ECU F", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_F[p]=0;
            }
                     
            err_cnt_F = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x4EE) && (classe == 'F')){
        err_cnt_F = 0;
      }



      if((id[maxIndex] == 0x6D6) && (classe != 'G')){
        log_G[err_cnt_G++]=classe;
        if (err_cnt_G == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_G);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x6D6 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU G");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x6D6: ECU G", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_G[p]=0;
            }
                    
            err_cnt_G = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x6D6) && (classe == 'G')){
        err_cnt_G = 0;
      }


      if((id[maxIndex] == 0x4F0) && (classe != 'H')){
        log_H[err_cnt_H++]=classe;
        if (err_cnt_H == ANOMALY_THRESHOLD){
          
          Log_queue_check(log_H);
          if(anomal_classe!=0){
            Serial.println("VOLTAGE FINGERPRINTING:");
            Serial.print("Rilevato ID 0x4F0 proveniente da ECU ");Serial.print(anomal_classe);
            Serial.println(" invece che da ECU H");
            Serial.println();

            /*Creazione Pacchetto */
            uint8_t* packet = create_packet(0x8009, anomal_classe,  "ID 0x4F0: ECU H", packet );
            insert_packet(packet);
            
            anomal_classe=0;
            }
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            log_H[p]=0;
            }
                     
            err_cnt_H = 0;
            delay(4000);
        }
      }
      else if((id[maxIndex] == 0x4F0) && (classe == 'H')){
        err_cnt_H = 0;
      }

  }




/*********old function
 
void print_anomalies(){
  
//Serial.print("err_cnt_A: ");Serial.println(err_cnt_A);
//Serial.print("err_cnt_B: ");Serial.println(err_cnt_B);
//Serial.print("err_cnt_C: ");Serial.println(err_cnt_C);
//Serial.print("err_cnt_D: ");Serial.println(err_cnt_D);
//Serial.print("err_cnt_E: ");Serial.println(err_cnt_E);
//Serial.print("err_cnt_F: ");Serial.println(err_cnt_F);
//Serial.print("err_cnt_G: ");Serial.println(err_cnt_G);
//Serial.print("err_cnt_H: ");Serial.println(err_cnt_H);
//Serial.print("classe: ");Serial.println(classe);
//Serial.println("----------");



if((id[maxIndex] == 0x4F4) && (classe != 'A')){
        log_A[err_cnt_A++]=classe;
        if (err_cnt_A == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x4F4 NON proveniente da ECU A ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_A[p]);Serial.print("\t");
            log_A[p]=0;
            }
            Serial.println();
            err_cnt_A = 0;
            delay(5000);
        }
      }
      else if((id[maxIndex] == 0x4F4) && (classe == 'A')){
        err_cnt_A = 0;
      }


      
      if((id[maxIndex] == 0x1E2) && (classe != 'B')){
        log_B[err_cnt_B++]=classe;
        if (err_cnt_B == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x1E2 NON proveniente da ECU B ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_B[p]);Serial.print("\t");
            log_B[p]=0;
            }
            Serial.println();         
          err_cnt_B = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x1E2) && (classe == 'B')){
        err_cnt_B = 0;
      }


      if((id[maxIndex] == 0x5D6) && (classe != 'C')){
        log_C[err_cnt_C++]=classe;
        if (err_cnt_C == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x5D6 NON proveniente da ECU C ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_C[p]);Serial.print("\t");
            log_C[p]=0;
            }
            Serial.println();          
          err_cnt_C = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x5D6) && (classe == 'C')){
        err_cnt_C = 0;
      }


      if((id[maxIndex] == 0x2E0) && (classe != 'D')){
        log_D[err_cnt_D++]=classe;
        if (err_cnt_D == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x2E0 NON proveniente da ECU D ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_D[p]);Serial.print("\t");
            log_D[p]=0;
            }
            Serial.println();          
          err_cnt_D = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x2E0) && (classe == 'D')){
        err_cnt_D = 0;
      }


      if((id[maxIndex] == 0x4D8) && (classe != 'E')){
        log_E[err_cnt_E++]=classe;
        if (err_cnt_E == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x4D8 NON proveniente da ECU E ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_E[p]);Serial.print("\t");
            log_E[p]=0;
            }
            Serial.println();          
          err_cnt_E = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x4D8) && (classe == 'E')){
        err_cnt_E = 0;
      }



      if((id[maxIndex] == 0x4EE) && (classe != 'F')){
        log_F[err_cnt_F++]=classe;
        if (err_cnt_F == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x4EE NON proveniente da ECU F ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_F[p]);Serial.print("\t");
            log_F[p]=0;
            }
            Serial.println();          
          err_cnt_F = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x4EE) && (classe == 'F')){
        err_cnt_F = 0;
      }



      if((id[maxIndex] == 0x6D6) && (classe != 'G')){
        log_G[err_cnt_G++]=classe;
        if (err_cnt_G == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x6D6 NON proveniente da ECU G ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_G[p]);Serial.print("\t");
            log_G[p]=0;
            }
            Serial.println();          
          err_cnt_G = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x6D6) && (classe == 'G')){
        err_cnt_G = 0;
      }


      if((id[maxIndex] == 0x4F0) && (classe != 'H')){
        log_H[err_cnt_H++]=classe;
        if (err_cnt_H == ANOMALY_THRESHOLD){
          Serial.println("VOLTAGE FINGERPRINTING:");
          Serial.println("Rilevato ID 0x4F0 NON proveniente da ECU H ");
          for(int p=0;p<ANOMALY_THRESHOLD;p++){
            Serial.print(log_H[p]);Serial.print("\t");
            log_H[p]=0;
            }
            Serial.println();          
          err_cnt_H = 0;
          delay(5000);
        }
      }
      else if((id[maxIndex] == 0x4F0) && (classe == 'H')){
        err_cnt_H = 0;
      }

 }

*******end old function********/

  
void training_ECU_dist(){
  
dist_AB=sqrt(  pow((CANH_ECUA_training-CANH_ECUB_training),2) + pow((CANL_ECUA_training-CANL_ECUB_training),2) + pow((diff_ECUA_training-diff_ECUB_training),2)   );
dist_AC=sqrt(  pow((CANH_ECUA_training-CANH_ECUC_training),2) + pow((CANL_ECUA_training-CANL_ECUC_training),2) + pow((diff_ECUA_training-diff_ECUC_training),2)   );
dist_AD=sqrt(  pow((CANH_ECUA_training-CANH_ECUD_training),2) + pow((CANL_ECUA_training-CANL_ECUD_training),2) + pow((diff_ECUA_training-diff_ECUD_training),2)   );
dist_AE=sqrt(  pow((CANH_ECUA_training-CANH_ECUE_training),2) + pow((CANL_ECUA_training-CANL_ECUE_training),2) + pow((diff_ECUA_training-diff_ECUE_training),2)   );
dist_AF=sqrt(  pow((CANH_ECUA_training-CANH_ECUF_training),2) + pow((CANL_ECUA_training-CANL_ECUF_training),2) + pow((diff_ECUA_training-diff_ECUF_training),2)   );
dist_AG=sqrt(  pow((CANH_ECUA_training-CANH_ECUG_training),2) + pow((CANL_ECUA_training-CANL_ECUG_training),2) + pow((diff_ECUA_training-diff_ECUG_training),2)   ); 
dist_AH=sqrt(  pow((CANH_ECUA_training-CANH_ECUH_training),2) + pow((CANL_ECUA_training-CANL_ECUH_training),2) + pow((diff_ECUA_training-diff_ECUH_training),2)   );

dist_BC=sqrt(  pow((CANH_ECUB_training-CANH_ECUC_training),2) + pow((CANL_ECUB_training-CANL_ECUC_training),2) + pow((diff_ECUB_training-diff_ECUC_training),2)   );
dist_BD=sqrt(  pow((CANH_ECUB_training-CANH_ECUD_training),2) + pow((CANL_ECUB_training-CANL_ECUD_training),2) + pow((diff_ECUB_training-diff_ECUD_training),2)   ); 
dist_BE=sqrt(  pow((CANH_ECUB_training-CANH_ECUE_training),2) + pow((CANL_ECUB_training-CANL_ECUE_training),2) + pow((diff_ECUB_training-diff_ECUE_training),2)   );
dist_BF=sqrt(  pow((CANH_ECUB_training-CANH_ECUF_training),2) + pow((CANL_ECUB_training-CANL_ECUF_training),2) + pow((diff_ECUB_training-diff_ECUF_training),2)   );
dist_BG=sqrt(  pow((CANH_ECUB_training-CANH_ECUG_training),2) + pow((CANL_ECUB_training-CANL_ECUG_training),2) + pow((diff_ECUB_training-diff_ECUG_training),2)   );
dist_BH=sqrt(  pow((CANH_ECUB_training-CANH_ECUH_training),2) + pow((CANL_ECUB_training-CANL_ECUH_training),2) + pow((diff_ECUB_training-diff_ECUH_training),2)   );

dist_CD=sqrt(  pow((CANH_ECUC_training-CANH_ECUD_training),2) + pow((CANL_ECUC_training-CANL_ECUD_training),2) + pow((diff_ECUC_training-diff_ECUD_training),2)   );
dist_CE=sqrt(  pow((CANH_ECUC_training-CANH_ECUE_training),2) + pow((CANL_ECUC_training-CANL_ECUE_training),2) + pow((diff_ECUC_training-diff_ECUE_training),2)   );
dist_CF=sqrt(  pow((CANH_ECUC_training-CANH_ECUF_training),2) + pow((CANL_ECUC_training-CANL_ECUF_training),2) + pow((diff_ECUC_training-diff_ECUF_training),2)   );
dist_CG=sqrt(  pow((CANH_ECUC_training-CANH_ECUG_training),2) + pow((CANL_ECUC_training-CANL_ECUG_training),2) + pow((diff_ECUC_training-diff_ECUG_training),2)   );
dist_CH=sqrt(  pow((CANH_ECUC_training-CANH_ECUH_training),2) + pow((CANL_ECUC_training-CANL_ECUH_training),2) + pow((diff_ECUC_training-diff_ECUH_training),2)   );

dist_DE=sqrt(  pow((CANH_ECUD_training-CANH_ECUE_training),2) + pow((CANL_ECUD_training-CANL_ECUE_training),2) + pow((diff_ECUD_training-diff_ECUE_training),2)   );
dist_DF=sqrt(  pow((CANH_ECUD_training-CANH_ECUF_training),2) + pow((CANL_ECUD_training-CANL_ECUF_training),2) + pow((diff_ECUD_training-diff_ECUF_training),2)   );
dist_DG=sqrt(  pow((CANH_ECUD_training-CANH_ECUG_training),2) + pow((CANL_ECUD_training-CANL_ECUG_training),2) + pow((diff_ECUD_training-diff_ECUG_training),2)   );
dist_DH=sqrt(  pow((CANH_ECUD_training-CANH_ECUH_training),2) + pow((CANL_ECUD_training-CANL_ECUH_training),2) + pow((diff_ECUD_training-diff_ECUH_training),2)   );

dist_EF=sqrt(  pow((CANH_ECUE_training-CANH_ECUF_training),2) + pow((CANL_ECUE_training-CANL_ECUF_training),2) + pow((diff_ECUE_training-diff_ECUF_training),2)   );
dist_EG=sqrt(  pow((CANH_ECUE_training-CANH_ECUG_training),2) + pow((CANL_ECUE_training-CANL_ECUG_training),2) + pow((diff_ECUE_training-diff_ECUG_training),2)   );
dist_EH=sqrt(  pow((CANH_ECUE_training-CANH_ECUH_training),2) + pow((CANL_ECUE_training-CANL_ECUH_training),2) + pow((diff_ECUE_training-diff_ECUH_training),2)   );

dist_FG=sqrt(  pow((CANH_ECUF_training-CANH_ECUG_training),2) + pow((CANL_ECUF_training-CANL_ECUG_training),2) + pow((diff_ECUF_training-diff_ECUG_training),2)   );
dist_FH=sqrt(  pow((CANH_ECUF_training-CANH_ECUH_training),2) + pow((CANL_ECUF_training-CANL_ECUH_training),2) + pow((diff_ECUF_training-diff_ECUH_training),2)   );

dist_GH=sqrt(  pow((CANH_ECUG_training-CANH_ECUH_training),2) + pow((CANL_ECUG_training-CANL_ECUH_training),2) + pow((diff_ECUG_training-diff_ECUH_training),2)   );

Serial.println("ECU training distances:");
Serial.print("  A");Serial.print("\t");Serial.print("  B");Serial.print("\t");Serial.print("  C");Serial.print("\t");Serial.print("  D");Serial.print("\t");Serial.print("  E");Serial.print("\t");Serial.print("  F");Serial.print("\t");Serial.print("  G");Serial.print("\t");Serial.println("  H");
Serial.print('A');Serial.print("\t");Serial.print(dist_AB,4);Serial.print("\t");Serial.print(dist_AC,4);Serial.print("\t");Serial.print(dist_AD,4);Serial.print("\t");Serial.print(dist_AE,4);Serial.print("\t");Serial.print(dist_AF,4);Serial.print("\t");Serial.print(dist_AG,4);Serial.print("\t");Serial.println(dist_AH,4);
Serial.print('B');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(dist_BC,4);Serial.print("\t");Serial.print(dist_BD,4);Serial.print("\t");Serial.print(dist_BE,4);Serial.print("\t");Serial.print(dist_BF,4);Serial.print("\t");Serial.print(dist_BG,4);Serial.print("\t");Serial.println(dist_BH,4);
Serial.print('C');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(dist_CD,4);Serial.print("\t");Serial.print(dist_CE,4);Serial.print("\t");Serial.print(dist_CF,4);Serial.print("\t");Serial.print(dist_CG,4);Serial.print("\t");Serial.println(dist_CH,4);
Serial.print('D');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(dist_DE,4);Serial.print("\t");Serial.print(dist_DF,4);Serial.print("\t");Serial.print(dist_DG,4);Serial.print("\t");Serial.println(dist_DH,4);
Serial.print('E');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(dist_EF,4);Serial.print("\t");Serial.print(dist_EG,4);Serial.print("\t");Serial.println(dist_EH,4);
Serial.print('F');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(dist_FG,4);Serial.print("\t");Serial.println(dist_FH,4);
Serial.print('G');Serial.print(" ");Serial.print("\t");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.print(" ");Serial.print("\t");Serial.println(dist_GH,4);
Serial.println();

}

void Log_queue_check(char log_array[ANOMALY_THRESHOLD]){
  int occurencies_array[8];
   for(int y=0;y<8;y++){
     occurencies_array[y]=0;
  }
  for(int p=0;p<ANOMALY_THRESHOLD;p++){
    if(log_array[p]=='A'){
      occurencies_array[0]= occurencies_array[0]+1;    
      }
    if(log_array[p]=='B'){
      occurencies_array[1]= occurencies_array[1]+1;    
      } 
    if(log_array[p]=='C'){
      occurencies_array[2]= occurencies_array[2]+1;    
      }
    if(log_array[p]=='D'){
      occurencies_array[3]= occurencies_array[3]+1;    
      }
    if(log_array[p]=='E'){
      occurencies_array[4]= occurencies_array[4]+1;    
      }
    if(log_array[p]=='F'){
      occurencies_array[5]= occurencies_array[5]+1;    
      } 
    if(log_array[p]=='G'){
      occurencies_array[6]= occurencies_array[6]+1;    
      }
    if(log_array[p]=='H'){
      occurencies_array[7]= occurencies_array[7]+1;    
      }              
    }


 int max_index=0;
 float max_occur=occurencies_array[0];

for(int j=0; j<8; j++){
  if(occurencies_array[j] > max_occur){
     max_occur = occurencies_array[j];
     max_index = j;
    }
  }




 if(occurencies_array[max_index]>=(ANOMALY_THRESHOLD)*0.9){
  switch(max_index){
    case 0:
    anomal_classe='A';
    break;
    case 1:
    anomal_classe='B';
    break;    
    case 2:
    anomal_classe='C';
    break;
    case 3:
    anomal_classe='D';
    break;    
    case 4:
    anomal_classe='E';
    break;
    case 5:
    anomal_classe='F';
    break;    
    case 6:
    anomal_classe='G';
    break;
    case 7:
    anomal_classe='H';
    break;     
    }
  }

 for(int y=0;y<8;y++){
  //Serial.println(occurencies_array[y]);
  occurencies_array[y]=0;
  }
  
  
}

//////////////////////////////////////// FUNCTIONS AUTOSAR ---------------------------------------------------
/************* FUNZIONI DI SETTAGGIO *************/

/* funzione che controlla che l'eventi_id passato alla funzione create_packet sia valido, altrimenti gli assegna il valore di invalid_event_id */
uint16_t check_event_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8009) {
        return event_id;
    } else {
        return 0xFFFF;
    }
}

/* funzione che imposta il valore del sensor_id in base all'event_id, se è un'anomalia voltage assegna l'ECU di riferimento*/
uint8_t set_sensor_value(uint16_t event_id, uint8_t anomal_classe) {
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0b1000;
    } else if(event_id == 0x8009){
        return anomal_classe;
    }else return 0b1111;
}

/* funzione che assegna il core di provenienza in base all'event_id */
uint8_t assign_idsm_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0;
    } else if(event_id == 0x8009){
        return 1;
    }else return 255; // undefined core
}

/* Funzione che fa il checksum del pacchetto come firma */
uint8_t calculate_checksum(uint8_t* data, size_t length) {
  uint8_t checksum = 0;
  for (size_t i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum;
}


void insert_packet(uint8_t* packet) {
  if (packetIndex < MAX_PACKET_COUNT) {
    packets[packetIndex] = packet;
    packetIndex++;
  } else {
    // Shift the packets array to the left
    for (int i = 0; i < MAX_PACKET_COUNT - 1; i++) {
      packets[i] = packets[i + 1];
    }
    packets[MAX_PACKET_COUNT - 1] = packet;
  }

  // Check if the Filter Chain is applicable
  if (packetIndex == MAX_PACKET_COUNT) {
    packetIndex = 0; // Reset the packetIndex
  }
}
/*
void save_packet_to_SD(uint8_t* packet) {
  for (int i = 0; i < PACKET_SIZE; i++) {
    for (int j = 7; j >= 0; --j) {
      anomaly_file.print(((packet[i] >> j) & 1));
    }
    anomaly_file.print(" ");
  }
  anomaly_file.println(); // Add a newline at the end
}*/

void save_IDSpackets_to_SD(uint8_t** packets, int size) {
    for (int i = 0; i < size; i++) {
        SerialASC.print("Processing packet ");
        SerialASC.println(i);
        for (int j = 0; j < PACKET_SIZE; j++) {
              SerialASC.print("Byte ");
              SerialASC.print(j);
              SerialASC.print(": ");
              SerialASC.println(String(packets[i][j]));
            for (int k = 7; k >= 0; --k) {
                //anomaly_file.println("Test write to file");
                anomaly_file.print(((packets[i][j] >> k) & 1));
            }
            anomaly_file.print(" ");
        }
        anomaly_file.println(); // Add a newline at the end
    }
}

// DA RIVEDERE QUANDO AGGIUNGO I FILTRI POTREBBE DIVENTARE UNA FUNZIONE UNICA CON QUELLA SOPRA (?)
void save_packets_to_SD(uint8_t** packets, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < PACKET_SIZE; j++) {
            for (int k = 7; k >= 0; --k) {
                filter_file.print(((packets[i][j] >> k) & 1));
            }
            filter_file.print(" ");
        }
        filter_file.println(); // Add a newline at the end
    }
}

/*************** FUNZIONI DI DEBUG *************** */

void print_binary(uint8_t* blob, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        for (int j = 7; j >= 0; --j) {
            Serial.print(((blob[i] >> j) & 1));
        }
        Serial.print(" ");

        if ((i + 1) % 8 == 0) {
            Serial.println();
        }
    }
    Serial.println(); // Add a newline at the end
}
void print_uint64_t(uint64_t num) {
  for (int i = 63; i >= 0; i--) {
    Serial.print((num >> i) & 1? '1' : '0');
  }
}
/******************** FUNZIONE CREAZIONE PACCHETTO *********************** */


uint8_t* create_packet(uint16_t custom_event_id, uint8_t anomal_classe, String anomaly_info, uint8_t* &packet) {

   uint8_t packet_size = PACKET_SIZE; // 55 bytes
   packet = new uint8_t[packet_size]; // EF + TS + CD + SIGN
   
  // Initialize packet array with correct values
  for (int i = 0; i < packet_size; i++) {
    packet[i] = 0; // initialize with 0
  }


  // BYTES 0-7 EVENT FRAME ---------------------------------------
  packet[0] = 0b00010111; // protocol version and protocol header
  uint8_t custom_sensor_id = set_sensor_value(custom_event_id, anomal_classe);
  uint8_t idsm_ID = assign_idsm_id(custom_event_id);
  packet[1] = idsm_ID; // idsm_ID
  packet[2] = custom_sensor_id & 0x3F;
  packet[3] = (custom_event_id >> 8) & 0xFF; // event_id
  packet[4] = custom_event_id & 0xFF; // event_id
  packet[5] = 0b00000000; // counter
  packet[6] = 0b00000001; // counter
  packet[7] = 0b00000000; // reserved
  
  
  
  // BYTES 8 - 15 TIMESTAMP ------------------------------------------------
  unsigned long timestamp = millis();
  Serial.print("Timestamp: ");
  Serial.println(timestamp);
  uint8_t timestamp_bytes[TIMESTAMP_BYTES]; // 64 bit
  for (int t = TIMESTAMP_BYTES - 1 ; t >= 0; t--) {
    timestamp_bytes[t] = (timestamp >> (t * 8)) & 0xFF;
  } 
    
  // Write timestamp to the packet
  for (int i = 0; i < TIMESTAMP_BYTES; i++) {
      packet[15 - i] = timestamp_bytes[i];
  }

  packet[8] |= 0x80; // added source = 1 CUSTOM overwrite of MSB of timestamp

  //  BYTES 16 - 19 CONTEXT DATA LENGHT -----------------------------------------
  uint8_t anomaly_bitstream_length = anomaly_info.length();
  for (int i = 3; i >= 0; --i) {
    uint8_t byte = (anomaly_bitstream_length >> (i * 8)) & 0xFF;
    if (i == 3) {
      byte |= 0x80; // Context Data Length format, set to 1, 4 bytes long
    }
    packet[19 - i] = byte;
  } 
  // BYTES 20 - 51 CONTEXT DATA -------------------------------------------------
  // Add anomaly info to packet
  for (int i = 0; i < ANOMALY_INFO_LENGTH; i++) {
    packet[20 + i] = anomaly_info[i];
  }
  // BYTES 52 - 54 SIGNATURE LENGHT E SIGNATURE WITH checksum ---------------------
 
  uint8_t checksum = calculate_checksum(packet, PACKET_SIZE);
  packet[52] = 0;
  packet[53] = 1;
  packet[54] = checksum;
  //print_binary(packet, PACKET_SIZE);
  /*SD.begin(10);
  anomaly_file = SD.open("IDS.txt", FILE_WRITE); 
  if (anomaly_file) {
    SerialASC.println("File aperto");
  } 
  else {
    SerialASC.println("ERRORE AD APRIRE");
    return nullptr;
  } 
  save_packet_to_SD(packet);
  
SerialASC.println("CHIUDO IDS.TXT ");
  anomaly_file.close();
SD.begin(BOARD_SPI_SS0_S1);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS);
  mcp2515.setNormalMode();*/
  
return packet;
}

/********************** FILTER CHAIN **************************** */

uint8_t** sampling_filter(uint8_t** packets, int packets_length, int n) {
  uint8_t** sampled_packets = nullptr;
  int sampled_packet_size = packets_length / n;
  if (packets_length % n!= 0) {
    sampled_packet_size++; // account for remainder
  }
  sampled_packets = (uint8_t**) malloc(sampled_packet_size * sizeof(uint8_t*));

  int j = 0;
  for (int i = 0; i < packets_length; i++) {
    if ((i + 1) % n == 1) { // forward every n-th packet
      sampled_packets[j] = new uint8_t[PACKET_SIZE];
      memcpy(sampled_packets[j], packets[i], PACKET_SIZE);
      j++;
    }
  }
  return sampled_packets;
}

uint8_t** aggregation_filter(uint8_t** packets, uint64_t time_interval, int packets_size, int* aggregated_packets_size) {
  const int max_aggregated_packets = 25; // adjust this value based on your needs
  uint8_t** aggregated_packets = new uint8_t*[max_aggregated_packets];
  int packets_aggregated_size = 0;
  
  Serial.println("Entering aggregation_filter with packets_size = ");
  Serial.println(packets_size);
  //print_binary(packets[i], PACKET_SIZE);

  for (int i = 0; i < max_aggregated_packets; i++) {
    aggregated_packets[i] = new uint8_t[PACKET_SIZE];
     
  }
 
  for (int i = 0; i < packets_size; i++){/*
    Serial.print("Processing packet ");
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < PACKET_SIZE; j++) {
      Serial.print(packets[i][j], BIN);
      Serial.print(" ");
    }
    Serial.println();*/
    bool found = false;
    for (int j = 0; j < packets_aggregated_size; j++) {
      if (packets[i][3] == aggregated_packets[j][3] && packets[i][4] == aggregated_packets[j][4]) {
        Serial.println("Packets have same event id");
        uint64_t time_x = (packets[i][8] << 56) | packets[i][9] << 48 | packets[i][10] << 40 | packets[i][11] << 32 | packets[i][12] << 24 | packets[i][13] << 16 | packets[i][14] << 8 | packets[i][15];
        uint64_t time_y = (aggregated_packets[j][8] << 56) | aggregated_packets[j][9] << 48 | aggregated_packets[j][10] << 40 | aggregated_packets[j][11] << 32 | aggregated_packets[j][12] << 24 | aggregated_packets[j][13] << 16 | aggregated_packets[j][14] << 8 | aggregated_packets[j][15];

        if ((time_x - time_y) < time_interval) {
          aggregated_packets[j][6]++; // increment counter
          Serial.println("The two packets respect the time interval");
          found = true;
          break;
        }
      }else {Serial.println("Packets DO NOT have same event id");}
    }

    if (!found) {
      if (packets_aggregated_size < max_aggregated_packets) {
        memcpy(aggregated_packets[packets_aggregated_size], packets[i], PACKET_SIZE);
        aggregated_packets[packets_aggregated_size][6] = 1; // initialize counter
        packets_aggregated_size++; // increment packets_aggregated_size here
      } else {
        Serial.println("Maximum number of aggregated packets reached. Discarding packet.");
      }
    }
  }
  *aggregated_packets_size = packets_aggregated_size;
  Serial.print("Number of aggregated packets:");
  Serial.println(packets_aggregated_size);
  Serial.println("Exiting aggregation_filter");
  return aggregated_packets;
}

uint8_t** threshold_filter(uint8_t** packets, int threshold, uint64_t time_interval, int packets_size, int* threshold_packets_size){
  const int max_threshold_packets = 25;
  uint8_t** threshold_packets = new uint8_t*[max_threshold_packets];
  int threshold_packets_index = 0;
  int counter = 0;
  bool packet_meets_threshold = false;
  Serial.print("Inizio filtro threshold with number of packets: ");
  Serial.println(packets_size);

  if(packets_size == 1){
   counter = packets[0][6];
   packet_meets_threshold = false;
   if(counter >= threshold){
    packet_meets_threshold = true;
    Serial.print("Packet with size 1 meets threshold condition with counter ");
    Serial.println(counter);
    }
   }

  for(int i = 0; i < packets_size; i++){ 
    if(packets_size != 1){
    uint64_t time_x = (packets[i][8] << 56) | packets[i][9] << 48 | packets[i][10] << 40 | packets[i][11] << 32  | packets[i][12] << 24  | packets[i][13] <<16 | packets[i][14] << 8 | packets[i][15] ;
    //Serial.println("estratto timestamp di primo elemento");
    counter = packets[i][6];
    Serial.print("counter value before checks: ");
    Serial.println(counter);
    packet_meets_threshold = false;
    for(int j= packets_size - 1 ; j>=0 ; j--){
      uint64_t time_y = (packets[j][8] << 56) | packets[j][9] << 48 | packets[j][10] << 40 | packets[j][11] << 32  | packets[j][12] << 24  | packets[j][13] <<16 | packets[j][14] << 8 | packets[j][15];
      //Serial.println("estratto timestamp del secondo elemento");
      if(packets[i][3] == packets[j][3] && packets[i][4] == packets[j][4]){
        Serial.println("i pacchetti hanno stesso event id");
        counter = packets[j][6];
        Serial.print("counter value after evevent_id checks: ");
        Serial.println(counter);
        if(counter >= threshold && (time_x - time_y) < time_interval){
          packet_meets_threshold = true;
          Serial.print("Packet ");
          Serial.print(i);
          Serial.print(" meets threshold condition with counter: and respect the time interval ");
          Serial.println(counter);
        }
      }
    }
    }
  
  if(packet_meets_threshold){
  bool packet_exists = false;
  for (int k = 0; k < threshold_packets_index; k++) {
    if (packets[i][3] == threshold_packets[k][3] && packets[i][4] == threshold_packets[k][4]) {
      threshold_packets[k][6] = counter;
      packet_exists = true;
      break;
    }
  }
  if (!packet_exists) {
    threshold_packets[threshold_packets_index] = new uint8_t[PACKET_SIZE];
    memcpy(threshold_packets[threshold_packets_index], packets[i], PACKET_SIZE);
    threshold_packets[threshold_packets_index][6] = counter;
    threshold_packets_index++;
        Serial.print("Packet ");
        Serial.print(i);
        Serial.print(" added to threshold packets with counter ");
        Serial.println(counter);
      }
    }
  }
  Serial.println("Threshold Packets inside filter: ");
  Serial.println(threshold_packets_index);
  *threshold_packets_size = threshold_packets_index;
  return threshold_packets;
}



void filter_chain(uint8_t** packets, size_t packets_size){

 int n = 2; // sampling rate con n = 1 si rompe
 int threshold = 3; // si rompe con threshold = 2 o 1 
 uint64_t time_interval = 4000;
 
 int packets_sampling_size = packets_size/n;
 int packets_aggregated_size;
 int threshold_packets_size;

 if (packets == nullptr || packets_size == 0) {
      Serial.println("ERRORE: filter chain vuota! ");
    }
  
        uint8_t** s_packets = sampling_filter(packets, packets_size, n);
        uint8_t** a_packets = aggregation_filter(s_packets, time_interval, packets_sampling_size, &packets_aggregated_size);
        uint8_t** t_packets = threshold_filter(a_packets, threshold, time_interval, packets_aggregated_size, &threshold_packets_size);
    

      

  for(int i = 0; i < packets_sampling_size; i++){
  if (s_packets != nullptr) {
    Serial.print("Filtered packets from sampling: ");
    print_binary(s_packets[i], PACKET_SIZE); 
    Serial.println();
  }
}

  for(int i = 0; i < packets_aggregated_size; i++){
  if (a_packets != nullptr) {
    Serial.print("Filtered packets from aggregation: ");
    print_binary(a_packets[i], PACKET_SIZE); 
    Serial.println();
  }
}

  for(int i = 0; i < threshold_packets_size; i++){
  if (t_packets != nullptr) {
    Serial.print("Filtered packets from thershold: ");
    print_binary(t_packets[i], PACKET_SIZE); 
    Serial.println();
  }
}
  filter_file = SD.open("sat.txt", FILE_WRITE); 
  if (filter_file) {
    SerialASC.println("File filtri aperto");
  } 
  else {
    SerialASC.println("ERRORE AD APRIRE");
    return;
  } 
  save_packets_to_SD(t_packets, threshold_packets_size);
  filter_file.close();
  
    // Free memory allocated for filtered packets
    for (int i = 0; i < threshold_packets_size; i++) {
        delete[] t_packets[i];
    }
    delete[] t_packets;

    // Free memory allocated for aggregated packets
    for (int i = 0; i < packets_aggregated_size; i++) {
        delete[] a_packets[i];
    }
    delete[] a_packets;

    // Free memory allocated for sampled packets
    for (int i = 0; i < packets_sampling_size; i++) {
        delete[] s_packets[i];
    }
    delete[] s_packets;
}
