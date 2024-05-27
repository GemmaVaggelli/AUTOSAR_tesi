
/**----- Multi Feature IDS Unipi  -------**/

#include "aux_functions.h"
MCP2515 mcp2515(9);

/* +++++++++++++++++++++++++ ++++++++++++++++++ +++++++++++++++++++  ++++++++++++++++++++*/
/*** Core 0 ***///Rule Based & Time Fingerprinting

void setup() {
  
  SerialASC.begin(SerialBaude);
  SD.begin(10);  //CS for SD memory 10
  file = SD.open(FileName1, FILE_READ); 
  if(file){SerialASC.println("microSD letta");}

  ReadDBC();  //Read dbc file
  file.close();
  PrintFeaturesFromDBC(); //Print Features from dbc file
  
  /*** --------init can bus -------- ***********/
  SD.begin(BOARD_SPI_SS0_S1);    //79
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS);
  mcp2515.setNormalMode();
  /*****/
  for(int r=1; r <=maxDlcLength; r++){
      for(int i=0; i<8; i++){             //index array to obtain bit order
      index_array[i+8*(r-1)]=8*r-1-i;
      }
   }
   
  for(int p=0; p <MaxNumID; p++){   //initialization to beginning value
    MessageCounters_prec[p]=-2;
   } 
   
//////Trigghero le centraline    
  pinMode(31,OUTPUT);
  digitalWrite(31,HIGH); //ECU 1 
  
  pinMode(33,OUTPUT);
  digitalWrite(33,HIGH); //ECU 2 

  pinMode(35,OUTPUT);
  digitalWrite(35,HIGH); //ECU 3 
  
  pinMode(37,OUTPUT);
  digitalWrite(37,HIGH); //ECU 4 
  
  pinMode(39,OUTPUT);
  digitalWrite(39,HIGH); //ECU 5 
  
  pinMode(41,OUTPUT);
  digitalWrite(41,HIGH); //ECU 6 
  
  pinMode(43,OUTPUT);
  digitalWrite(43,HIGH); //ECU 7 
  
  pinMode(45,OUTPUT);
  digitalWrite(45,HIGH); //ECU 8 
  
  init_time=millis(); 
  bus_time=millis();      
}


void loop() {

     
   SerialASC.println("-------- Time Fingerprinting Training Phase Starts ----------");
   delay(1000);
    while(counterMin < N_Training){

    SerialASC.println(counterMin);
    /*
    for(int i=0;i<Num_ID_TFP;i++){
     SerialASC.println(RecCounter[i]);
    }
    SerialASC.println("****");
    */
    if(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK){
      //Serial.println(canMsg.can_id, HEX); // print ID
      my_time = millis();
   
    int IDtrovato = FindIndexInList(White_List_ID, canMsg.can_id); // riga matrice
    CounterUpdate(IDtrovato, pRecCounter, N_Training);
 
    int samplePos = RecCounter[IDtrovato]; // colonna matrice
    Matrix_Training[IDtrovato][samplePos] = my_time; // nuovo elemento matrice
    
    counterMin = trovaMinimo(RecCounter);
    
    }
  }
  
  creaMatriceDifferenze(pMatriceDifferenze, pMatrix_Training);
  calcoloStatistiche(pMatriceDifferenze, pVettoreMedia, pVettoreVarianza);
 
  stampaStatistiche(VettoreMedia, VettoreVarianza, White_List_ID);

  for(int i = 0; i < Num_ID_TFP; i++){ // azzero tutti i contatori (così riuso lo stesso vettore)
    RecCounter[i] = 0;
  }
  timing_training_complete=true;
  SerialASC.println("-------- Voltage Training Starts ----------");
  Serial.println("Trigghero ECU A");
  pinMode(31,OUTPUT);  
  digitalWrite(31,HIGH); 
  pinMode(33,OUTPUT);   //ECU B
  digitalWrite(33,LOW);
  pinMode(35,OUTPUT);  //ECU C
  digitalWrite(35,LOW); 
  pinMode(37,OUTPUT);  //ECU D
  digitalWrite(37,LOW); 
  pinMode(39,OUTPUT);  //ECU E
  digitalWrite(39,LOW); 
  pinMode(41,OUTPUT);  //ECU F
  digitalWrite(41,LOW); 
  pinMode(43,OUTPUT);  //ECU G
  digitalWrite(43,LOW); 
  pinMode(45,OUTPUT); //ECU H
  digitalWrite(45,LOW); 
  while(voltage_training_complete==false){
    delay(1000);
    } 
  SerialASC.println("-------- Test Phase Starts ----------");
  online_phase=true;
  start_time=millis();    //per valutare sospensioni

/*****************************************************************************/
 
  while(1){

   //Serial.print("online_phase fuori if ");Serial.println(online_phase);
   if(online_phase==true){
   //Serial.print("online_phase dentro if ");Serial.println(online_phase);
   if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    
     time_mess = millis();
     bitnumber = bitnumber + canMsg.can_dlc*8+44; //standard frame

   /***  ---------RULE BASED check functions------------   ***/
    Check_Diagnostic_anomaly(canMsg.can_id, canMsg.data);
    Check_ID_anomaly(canMsg.can_id);
    Check_Rate_anomaly(canMsg.can_id,time_mess);
    Check_DLC_anomaly(canMsg.can_id,canMsg.can_dlc);
    Check_Counter_anomaly(canMsg.can_id,canMsg.can_dlc, canMsg.data);
    Additional_Checks(canMsg.can_id,canMsg.can_dlc,canMsg.data); //custom checks
    
    /***  ---------TIME FINGERPRINTING check functions------------   ***/
     my_time = time_mess;
     int IDtrovato = FindIndexInList(White_List_ID, canMsg.can_id);
     
      if(IDtrovato != -1){
               
      total_rec[IDtrovato] = total_rec[IDtrovato] + 1;
      CounterUpdate(IDtrovato, pRecCounter, N_Test);
      int samplePos = RecCounter[IDtrovato];

      if(samplePos < N_Test){
        riempiRigaMatrice(IDtrovato, samplePos, my_time, pMatrix_Test);
      }
      else if((samplePos >= N_Test)){
        aggiornaRigaDiff(IDtrovato, N_Test, pMatrix_Test, pMatrixDiff_Test); 
        calcolaStatisticheRiga(IDtrovato, N_Test, pMatrixDiff_Test, pVetMediaTest, pVetVarianzaTest); 
        int checkmedia = CheckMediaRangeRaw(VettoreMedia[IDtrovato], VetMediaTest[IDtrovato],IDtrovato);
        int checkvarianza= CheckVarianzaRangeRaw(VettoreVarianza[IDtrovato],VetVarianzaTest[IDtrovato]);
        Period_Check(IDtrovato,checkmedia,checkvarianza);
        azzeraRigaMatrice(IDtrovato, pMatrix_Test, N_Test);
        azzeraRigaMatrice(IDtrovato, pMatrixDiff_Test, N_Test);
        RecCounter[IDtrovato] = 0;
      }

   /***  ---------------------------------------------------   ***/
     }//if(IDtrovato != -1)
   }/*readMessage*/

   
   
   if ((millis() - start_time) >= 10000){ //ogni 10 secondi controllo se i contatori di tutti i messaggi si sono incrementati
      Suspension_Check();
      }
   if((millis() - bus_time) >= 1000){ // Check bus load every 1 second
      Check_BusLoad_anomaly();
      }
   if ((millis() - init_time) >= 5000){ //Log Ruled based report is printed every 5 seconds 
     PrintAnomalies();
     }   
   }
  }/*while(1)*/
}/*loop()*/

/* +++++++++++++++++++++++++ ++++++++++++++++++ +++++++++++++++++++  ++++++++++++++++++++*/
/*** Core 1 ***/

void setup1() {
	SerialASC.begin(SerialBaude);
  
}

void loop1() {
   //Serial.println("loop1");
   //delay(1000);             
}


/* +++++++++++++++++++++++++ ++++++++++++++++++ +++++++++++++++++++  ++++++++++++++++++++*/
/*** Core 2 ***/


void setup2() {
	SerialASC.begin(SerialBaude);
  analogReadResolution (12u);

}

void loop2() {          ////////Voltage Fingerprinting 

   while(Diagnostic_anomaly !=0 ||ID_anomaly != 0){
    delay(5000);
    }
           
 if(timing_training_complete==true){
  
 //training_again:


  /***************** Training Phase **********************/
  switch(centralina){
    case 1:
    measure_features();
    if(measures_ready==true){
      for(int j=0;j<actual_measurements;j++){
        CANH_ECUA_training = CANH_ECUA_training +dominanti_H[j];
        CANL_ECUA_training = CANL_ECUA_training +dominanti_L[j];
        }
        CANH_ECUA_training = CANH_ECUA_training/actual_measurements;
        CANL_ECUA_training = CANL_ECUA_training/actual_measurements;
        diff_ECUA_training= CANH_ECUA_training-CANL_ECUA_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(31,LOW);
        delay(1000);
        Serial.println("Trigghero ECU B");
        digitalWrite(33,HIGH);
        delay(500);
       } 
     break;
     case 2:
     measure_features();
     if(measures_ready==true){
      for(int j=0;j<actual_measurements;j++){
        CANH_ECUB_training = CANH_ECUB_training + dominanti_H[j];
        CANL_ECUB_training = CANL_ECUB_training + dominanti_L[j];
        }
        CANH_ECUB_training = CANH_ECUB_training/actual_measurements;
        CANL_ECUB_training = CANL_ECUB_training/actual_measurements;
        diff_ECUB_training= CANH_ECUB_training-CANL_ECUB_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(33,LOW);
        delay(1000);
        Serial.println("Trigghero ECU C");
        digitalWrite(35,HIGH);
        delay(500);
       } 
       break;
       case 3:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUC_training = CANH_ECUC_training +dominanti_H[j];
           CANL_ECUC_training = CANL_ECUC_training +dominanti_L[j];
           }
          CANH_ECUC_training = CANH_ECUC_training/actual_measurements;
          CANL_ECUC_training = CANL_ECUC_training/actual_measurements;
          diff_ECUC_training= CANH_ECUC_training-CANL_ECUC_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(35,LOW);
        delay(1000);
        Serial.println("Trigghero ECU D");
        digitalWrite(37,HIGH);
        delay(500);
       } 
       break;
       case 4:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUD_training = CANH_ECUD_training +dominanti_H[j];
           CANL_ECUD_training = CANL_ECUD_training +dominanti_L[j];
           }
          CANH_ECUD_training = CANH_ECUD_training/actual_measurements;
          CANL_ECUD_training = CANL_ECUD_training/actual_measurements;
          diff_ECUD_training= CANH_ECUD_training-CANL_ECUD_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(37,LOW);
        delay(1000);
        Serial.println("Trigghero ECU E");
        digitalWrite(39,HIGH);
        delay(500);
       } 
       break;
       case 5:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUE_training = CANH_ECUE_training +dominanti_H[j];
           CANL_ECUE_training = CANL_ECUE_training +dominanti_L[j];
           }
          CANH_ECUE_training = CANH_ECUE_training/actual_measurements;
          CANL_ECUE_training = CANL_ECUE_training/actual_measurements;
          diff_ECUE_training= CANH_ECUE_training-CANL_ECUE_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(39,LOW);
        delay(1000);
        Serial.println("Trigghero ECU F");
        digitalWrite(41,HIGH);
        delay(500);
       } 
       break;       
       case 6:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUF_training = CANH_ECUF_training +dominanti_H[j];
           CANL_ECUF_training = CANL_ECUF_training +dominanti_L[j];
           }
          CANH_ECUF_training = CANH_ECUF_training/actual_measurements;
          CANL_ECUF_training = CANL_ECUF_training/actual_measurements;
          diff_ECUF_training= CANH_ECUF_training-CANL_ECUF_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(41,LOW);
        delay(1000);
        Serial.println("Trigghero ECU G");
        digitalWrite(43,HIGH);
        delay(500);
       } 
       break;
       case 7:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUG_training = CANH_ECUG_training +dominanti_H[j];
           CANL_ECUG_training = CANL_ECUG_training +dominanti_L[j];
           }
          CANH_ECUG_training = CANH_ECUG_training/actual_measurements;
          CANL_ECUG_training = CANL_ECUG_training/actual_measurements;
          diff_ECUG_training= CANH_ECUG_training-CANL_ECUG_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(43,LOW);
        delay(1000);
        Serial.println("Trigghero ECU H");
        digitalWrite(45,HIGH);
        delay(500);
       } 
       break;              
       case 8:
        measure_features();
        if(measures_ready==true){
          for(int j=0;j<actual_measurements;j++){
           CANH_ECUH_training = CANH_ECUH_training +dominanti_H[j];
           CANL_ECUH_training = CANL_ECUH_training +dominanti_L[j];
           }
          CANH_ECUH_training = CANH_ECUH_training/actual_measurements;
          CANL_ECUH_training = CANL_ECUH_training/actual_measurements;
          diff_ECUH_training= CANH_ECUH_training-CANL_ECUH_training;
        
        actual_measurements=0;
        measures_ready=false;
        centralina++;
        digitalWrite(45,LOW);
        delay(1000);
        training_ECU_dist();
        Serial.println("Fine Training -> Inizio Test:");
        delay(500);
       } 
       break;
  
       default:
      
       /*
       Serial.print("CANH_ECUA_training: ");Serial.print(CANH_ECUA_training,4);Serial.print("\t");
       Serial.print("CANL_ECUA_training: ");Serial.print(CANL_ECUA_training,4);Serial.print("\t");
       Serial.print("diff_ECUA_training: ");Serial.println(diff_ECUA_training,4);

       Serial.print("CANH_ECUB_training: ");Serial.print(CANH_ECUB_training,4);Serial.print("\t");
       Serial.print("CANL_ECUB_training: ");Serial.print(CANL_ECUB_training,4);Serial.print("\t");
       Serial.print("diff_ECUB_training: ");Serial.println(diff_ECUB_training,4);
       
       Serial.print("CANH_ECUC_training: ");Serial.print(CANH_ECUC_training,4);Serial.print("\t");
       Serial.print("CANL_ECUC_training: ");Serial.print(CANL_ECUC_training,4);Serial.print("\t");
       Serial.print("diff_ECUC_training: ");Serial.println(diff_ECUC_training,4);

       Serial.print("CANH_ECUD_training: ");Serial.print(CANH_ECUD_training,4);Serial.print("\t");
       Serial.print("CANL_ECUD_training: ");Serial.print(CANL_ECUD_training,4);Serial.print("\t");
       Serial.print("diff_ECUD_training: ");Serial.println(diff_ECUD_training,4);
       
       Serial.print("CANH_ECUE_training: ");Serial.print(CANH_ECUE_training,4);Serial.print("\t");
       Serial.print("CANL_ECUE_training: ");Serial.print(CANL_ECUE_training,4);Serial.print("\t");
       Serial.print("diff_ECUE_training: ");Serial.println(diff_ECUE_training,4);

       Serial.print("CANH_ECUF_training: ");Serial.print(CANH_ECUF_training,4);Serial.print("\t");
       Serial.print("CANL_ECUF_training: ");Serial.print(CANL_ECUF_training,4);Serial.print("\t");
       Serial.print("diff_ECUF_training: ");Serial.println(diff_ECUF_training,4);
       
       Serial.print("CANH_ECUG_training: ");Serial.print(CANH_ECUG_training,4);Serial.print("\t");
       Serial.print("CANL_ECUG_training: ");Serial.print(CANL_ECUG_training,4);Serial.print("\t");
       Serial.print("diff_ECUG_training: ");Serial.println(diff_ECUG_training,4);

       Serial.print("CANH_ECUH_training: ");Serial.print(CANH_ECUH_training,4);Serial.print("\t");
       Serial.print("CANL_ECUH_training: ");Serial.print(CANL_ECUH_training,4);Serial.print("\t");
       Serial.print("diff_ECUH_training: ");Serial.println(diff_ECUH_training,4);
       */

       
       digitalWrite(31,HIGH); //ECU1 A
       digitalWrite(33,HIGH); //ECU2 B
       digitalWrite(35,HIGH); //ECU3 C
       digitalWrite(37,HIGH); //ECU4 D   //a volte confuso con classe C  
       digitalWrite(39,HIGH); //ECU5 E
       digitalWrite(41,HIGH); //ECU6 F
       digitalWrite(43,HIGH); //ECU7 G
       digitalWrite(45,HIGH); //ECU8 H         
       delay(1000); 
       voltage_training_complete=true;
       online_phase=true;
           
       while(voltage_training_complete){

          if(Serial.available()){
            //Serial.println("prima read");
            incoming_char=Serial.read();
            //Serial.println("dopo read");
            if(incoming_char=='t'){
              online_phase=false;
              Serial.println("Voltage Training starts again");
              voltage_training_complete=false;
              centralina=1;
              Serial.println("Trigghero ECU A");
              pinMode(31,OUTPUT);  
              digitalWrite(31,HIGH); 
              pinMode(33,OUTPUT);   //ECU B
              digitalWrite(33,LOW);
              pinMode(35,OUTPUT);  //ECU C
              digitalWrite(35,LOW); 
              pinMode(37,OUTPUT);  //ECU D
              digitalWrite(37,LOW); 
              pinMode(39,OUTPUT);  //ECU E
              digitalWrite(39,LOW); 
              pinMode(41,OUTPUT);  //ECU F
              digitalWrite(41,LOW); 
              pinMode(43,OUTPUT);  //ECU G
              digitalWrite(43,LOW); 
              pinMode(45,OUTPUT); //ECU H
              digitalWrite(45,LOW); 
              delay(500);

              //goto training_again;
              }
                   
           }
      
        measure_features();
        if(measures_ready==true){
          for(int x = 0; x < 35; x++){
           if(misure_DIFF[x] > 1 ){
           DIFF_app[x] = 0.0;
           }
           else{
           DIFF_app[x] = 1.0;
           }
      }
// ADESSO VADO FARE IL VOTING PER CAPIRE QUALE ID E' PIU' PROBABILE

      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_A_NOM[x] ){
          voto[0] = voto[0] + 1;
        }
      }
      //Serial.print("voto[0]:");Serial.println(voto[0]);  
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_B_NOM[x] ){
          voto[1] = voto[1] + 1;
        }
      }
      //Serial.print("voto[1]:");Serial.println(voto[1]);
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_C_NOM[x] ){
          voto[2] = voto[2] + 1;
        }
      }
      //Serial.print("voto[2]:");Serial.println(voto[2]);  
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_D_NOM[x] ){
          voto[3] = voto[3] + 1;
        }
      }
      //Serial.print("voto[3]:");Serial.println(voto[3]);      

      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_E_NOM[x] ){
          voto[4] = voto[4] + 1;
        }
      }
      //Serial.print("voto[4]:");Serial.println(voto[4]);  
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_F_NOM[x] ){
          voto[5] = voto[5] + 1;
        }
      }
      //Serial.print("voto[5]:");Serial.println(voto[5]);
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_G_NOM[x] ){
          voto[6] = voto[6] + 1;
        }
      }
      //Serial.print("voto[6]:");Serial.println(voto[6]);  
      for(int x = 0; x < 35; x++){
        if(DIFF_app[x] == ID_H_NOM[x] ){
          voto[7] = voto[7] + 1;
        }
      }
      //Serial.print("voto[7]:");Serial.println(voto[7]);  
      
      maxValue = voto[0];
      maxIndex = 0;

      for (int i = 0; i < 8; i++) {
        if (voto[i] > maxValue) {
          maxValue = voto[i];
          maxIndex = i;
        }
      }
      for (int i = 0; i < 8; i++) {
        voto[i]=0;
        }
   
      
      //Serial.println(id[maxIndex],HEX);
      

      
      for(int j=0;j<actual_measurements;j++){
        CANH_test = CANH_test + dominanti_H[j];
        CANL_test = CANL_test + dominanti_L[j];
        }
        CANH_test = CANH_test/actual_measurements;
        CANL_test = CANL_test/actual_measurements;
        diff_ECU_test = CANH_test-CANL_test;

        check_classe();
          
        print_anomalies();

  
      actual_measurements=0;
      measures_ready=false; 
      CANH_test=0;
      CANL_test=0;
      diff_ECU_test=0;

                         
          }//measure_ready               
        }//while(1)
        
       break;   
    }//switch

 }
                 
}//loop2
