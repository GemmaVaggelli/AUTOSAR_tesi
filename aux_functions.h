#ifndef AUX_FUNCTIONS.H
#define AUX_FUNCTIONS.H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <mcp2515.h>

#include <stddef.h>
#include <stdlib.h>

#define SerialBaude 1000000
#define CSPin 4
#define Serial SerialASC
extern MCP2515 mcp2515;

///////////////////////////////////////Ruled Based ------------------------------------------------------------------

/****** UDS(Unified Diagnostic Services) *****/
#define Diag_ID1 0x10
#define Diag_ID2 0x11
#define Diag_ID3 0x27
#define Diag_ID4 0x28
#define Diag_ID5 0x29
#define Diag_ID6 0x3E
#define Diag_ID7 0x84
#define Diag_ID8 0x85
#define Diag_ID9 0x86
#define Diag_ID10 0x87
#define Diag_ID11 0x22
#define Diag_ID12 0x23
#define Diag_ID13 0x24
#define Diag_ID14 0x2A
#define Diag_ID15 0x2C
#define Diag_ID16 0x2E
#define Diag_ID17 0x3D
#define Diag_ID18 0x14
#define Diag_ID19 0x19
#define Diag_ID20 0x2F
#define Diag_ID21 0x31
#define Diag_ID22 0x34
#define Diag_ID23 0x35
#define Diag_ID24 0x36
#define Diag_ID25 0x37
#define Diag_ID26 0x38

#define Num_Diagnostic_ID 26

#define FileName1 "alf.dbc"
#define FileName2 "for.dbc"
//#define FileName3 "j19.dbc"
#define FileName5 "bmw.dbc"
#define FileName4 "kia.dbc"

#define MaxNumEcu 80
#define MaxNumID 500

#define LogLength 100

#define Bitrate 500000

#define maxDlcLength 64

/************* Global Variables Declaration *************/

extern struct can_frame canMsg;
extern bool mes_arr;

/***---- Read DBC file-----***/
extern File file;
extern int Num_ID;
extern int Num_ECU;
extern int Null_line;
extern String ECU[MaxNumEcu];
extern unsigned long ID_list[MaxNumID];

extern int DLC_list[MaxNumID];
extern String ECU_origin[MaxNumID];

/***-----Diagnostic variables-------***/
extern unsigned long int Diag_List[Num_Diagnostic_ID];
extern bool Diagnostic_session; 

/****----Bus load variables-----*/
extern double bus_time;
extern int bitnumber;
extern double Bus_Load;

/***---Time variables---***/
extern int num_periodic_mess;
extern double time_mess;
extern double arrival_time_array[MaxNumID];
extern double init_time;
extern unsigned long Periodic_ID_list[MaxNumID];
extern int Period_values[MaxNumID];

/*******------------Anomalies log counters---------********/
extern int Diagnostic_anomaly;
extern unsigned long Diagn_ID_log[LogLength];
extern unsigned long* P_Diagn_ID_log;

extern int ID_anomaly;
extern unsigned long Anomal_ID_log[LogLength];
extern unsigned long* P_Anomal_ID_log;

extern int DLC_anomaly;
extern unsigned long Anomal_DLC_log[LogLength];
extern unsigned long* P_Anomal_DLC_log;

extern int Rate_anomaly;
extern unsigned long Anomal_Rate_log[LogLength];
extern unsigned long* P_Anomal_Rate_log;

extern int BusLoad_anomaly;
extern int velocity_increase_anomaly;

extern int Counter_anomaly;
extern unsigned long Anomal_Counter_log[LogLength];
extern unsigned long* P_Anomal_Counter_log;

/** Messages counter  **/
extern int MessageCounters[MaxNumID];
extern int MessageCounters_prec[MaxNumID];
extern int NumCounters;
extern unsigned long IdCounter[MaxNumID];
extern int BitStartCounter[MaxNumID];
extern int BitLengthCounter[MaxNumID];
extern int endianCounter[MaxNumID];
extern int maxRangeCounter[MaxNumID];


/****** Additional Checks  *****/

/**Vehicle speed**/
extern unsigned long VehicleSpeed_ID;
extern int BitStart;
extern int BitLength;
extern double Scale;
extern double offset;
extern int Range[2];   //min value, max value
extern String MeasureUnit;
extern int endian;
extern String sign;
extern int binary[8*maxDlcLength];
extern int index_array[8*maxDlcLength];
extern double vel;
extern double vel_prec;
extern double anomal_prev_vel;
extern double anomal_following_vel;

/************* Functions Declaration *************/

void ReadDBC();
void PrintFeaturesFromDBC();
void Check_Diagnostic_anomaly(unsigned long canId, unsigned char buf[8] );
void Check_ID_anomaly(unsigned long canId);
void Check_DLC_anomaly(unsigned long canId,byte len);
void Check_Rate_anomaly(unsigned long canId,double time_mess);
void Check_BusLoad_anomaly();
void Check_Counter_anomaly(unsigned long canId,byte len, unsigned char buf[8]);
void Additional_Checks(unsigned long canId,byte len, unsigned char buf[8]);
void PrintAnomalies();

////////////////////////////////////////////////////Time Fingerprinting ------------------------------------------------------------------------------

#define NOT_RECOGNISE -1

#define N_Training 100
#define N_Test 100
#define Num_ID_TFP 8


/************* Global Variables Declaration *************/
extern bool timing_training_complete;
extern int counterMin;
extern long int White_List_ID[Num_ID_TFP];
extern double my_time; 
extern double start_time;
extern int total_rec[Num_ID_TFP];
extern int total_rec_precedente[Num_ID_TFP];            
extern int ID_sospesi[Num_ID_TFP];                      
extern int ID_anomali_media[Num_ID_TFP];               
extern int ID_anomali_varianza[Num_ID_TFP];            
extern int* pID_anomali_media;
extern int* pID_anomali_varianza;

extern double Matrix_Training[Num_ID_TFP][N_Training]; //per accumulo campioni (tempi di arrivo) in fase di training
extern double Matrix_Test[Num_ID_TFP][N_Test]; //per accumulo campioni (tempi di arrivo) in fase di test

extern double* pMatrix_Training ;
extern double* pMatrix_Test ;

extern int RecCounter[Num_ID_TFP]; // vettore le cui componenti indicano il numero di messaggi arrivati per ogni ID
extern int* pRecCounter ;

extern double MatriceDifferenze[Num_ID_TFP][N_Training];
extern double* pMatriceDifferenze ;

extern double MatrixDiff_Test[Num_ID_TFP][N_Test];
extern double* pMatrixDiff_Test ;

extern double VettoreMedia[Num_ID_TFP];
extern double VettoreVarianza[Num_ID_TFP];

extern double* pVettoreMedia;
extern double* pVettoreVarianza ;

extern double VetMediaTest[Num_ID_TFP];
extern double VetVarianzaTest[Num_ID_TFP];

extern double* pVetMediaTest;
extern double* pVetVarianzaTest;


/************* Functions Declaration *************/
int FindIndexInList(long int List_ID[Num_ID_TFP], unsigned long int msg_id);
void CounterUpdate(int indexID, int* pRecCounter, int N);
int trovaMinimo(int VettoreContatori[Num_ID_TFP]);  
void creaMatriceDifferenze(double* MatriceDifferenze, double* MatriceDati);
void calcoloStatistiche(double* pMatriceDifferenze, double* pVettoreMedia, double* pVettoreVarianza);
void stampaStatistiche(double VettoreMedia[Num_ID_TFP], double VettoreVarianza[Num_ID_TFP], long int White_List_ID[Num_ID_TFP]);
void riempiRigaMatrice(int riga, int colonna, double my_time, double* pMatrix_Training);
void aggiornaRigaDiff(int rigaID, int dimCol, double* pMatrix, double* pMatrixDiff);
void calcolaStatisticheRiga(int index_riga, int dimCol, double* pMatrixDiff, double* pVetMedia, double* pVetVarianza);
int CheckMediaRangeRaw(double mu_train, double mu_test,int IDtrovato);
int CheckVarianzaRangeRaw(double var_train, double var_test);
void Period_Check(int IDtrovato, int checkmedia, int checkvarianza);
void azzeraRigaMatrice(int index_riga, double* pMatrix, int dimCol);
void Suspension_Check();

////////////////////////////////// FUNCTION DEFINITION GEMMA ------------------------------------------
#define FileNameIDS "IDS.txt"
extern File anomaly_file;
extern File filter_file;
extern uint8_t**  packets;
extern double filter_time;
extern int packetIndex;

uint16_t check_event_id(uint16_t event_id);
uint8_t set_sensor_value(uint16_t event_id, uint8_t anomal_classe);
uint8_t assign_idsm_id(uint16_t event_id);
uint8_t calculate_checksum(uint8_t* data, size_t length);
uint8_t* create_packet(uint16_t custom_event_id, uint8_t anomal_classe, String anomaly_info, uint8_t* &packet);
void filter_chain(uint8_t** packets, size_t packets_size);
//void save_packet_to_SD(uint8_t* packet);
void save_packets_to_SD(uint8_t** packets, int size);
void save_IDSpackets_to_SD(uint8_t** packets, int size);
void insert_packet(uint8_t* packet);
/************* Filter Chain Function Declaration *****************/



//////////////////////////Voltage Fingerprinting ----------------------------------------------------------------------------------------------------------

#define VIN       (5)
#define IDLE_THRESHOLD  (50)  // SOGLIA DA CUI RICONOSCO SE SONO IN IDLE
#define ANOMALY_THRESHOLD 30

extern bool online_phase;


extern float misure_CANH[400];
extern float misure_CANL[400];
extern float misure_DIFF[400];

extern float dominanti_H[400];
extern float dominanti_L[400];
extern float dominanti_D[400];

extern float CANH_tmp;
extern float CANL_tmp;
extern float DIFF_tmp;

extern float CANH_avg;
extern float CANL_avg;
extern float diff_avg;

extern float CANH_ECUA_training;
extern float CANL_ECUA_training;
extern float diff_ECUA_training;

extern float CANH_ECUB_training;
extern float CANL_ECUB_training;
extern float diff_ECUB_training;

extern float CANH_ECUC_training;
extern float CANL_ECUC_training;
extern float diff_ECUC_training;

extern float CANH_ECUD_training;
extern float CANL_ECUD_training;
extern float diff_ECUD_training;

extern float CANH_ECUE_training;
extern float CANL_ECUE_training;
extern float diff_ECUE_training;

extern float CANH_ECUF_training;
extern float CANL_ECUF_training;
extern float diff_ECUF_training;

extern float CANH_ECUG_training;
extern float CANL_ECUG_training;
extern float diff_ECUG_training;

extern float CANH_ECUH_training;
extern float CANL_ECUH_training;
extern float diff_ECUH_training;




extern int actual_measurements;
extern int maxValue;
extern int maxIndex ;
extern int idle_cnt; 


extern double features[15]; 
extern unsigned int id[8];
extern int voto[8];
extern byte ID_A_NOM[35]; 
extern byte ID_B_NOM[35]; 
extern byte ID_C_NOM[35]; 
extern byte ID_D_NOM[35];
extern byte ID_E_NOM[35]; 
extern byte ID_F_NOM[35]; 
extern byte ID_G_NOM[35]; 
extern byte ID_H_NOM[35];

extern int DIFF_app[35];
extern int cnt;
extern byte err_cnt_A;
extern byte err_cnt_B;
extern byte err_cnt_C;
extern byte err_cnt_D;
extern byte err_cnt_E;
extern byte err_cnt_F;
extern byte err_cnt_G;
extern byte err_cnt_H;

extern char classe;
extern double score[8];

extern byte centralina;  

extern bool measures_ready;

extern float dist[8];
  

extern float CANH_test;
extern float CANL_test;
extern float diff_ECU_test;

extern int anomalies_A;
extern char log_A[ANOMALY_THRESHOLD];
extern int anomalies_B;
extern char log_B[ANOMALY_THRESHOLD];
extern int anomalies_C;
extern char log_C[ANOMALY_THRESHOLD];
extern int anomalies_D;
extern char log_D[ANOMALY_THRESHOLD];
extern int anomalies_E;
extern char log_E[ANOMALY_THRESHOLD];
extern int anomalies_F;
extern char log_F[ANOMALY_THRESHOLD];
extern int anomalies_G;
extern char log_G[ANOMALY_THRESHOLD];
extern int anomalies_H;
extern char log_H[ANOMALY_THRESHOLD];

extern char anomal_classe;


//ECU distances during traing
extern float dist_AB;
extern float dist_AC;
extern float dist_AD;
extern float dist_AE;
extern float dist_AF;
extern float dist_AG;
extern float dist_AH;

extern float dist_BC;
extern float dist_BD;
extern float dist_BE;
extern float dist_BF;
extern float dist_BG;
extern float dist_BH;

extern float dist_CD;
extern float dist_CE;
extern float dist_CF;
extern float dist_CG;
extern float dist_CH;

extern float dist_DE;
extern float dist_DF;
extern float dist_DG;
extern float dist_DH;

extern float dist_EF;
extern float dist_EG;
extern float dist_EH;

extern float dist_FG;
extern float dist_FH;

extern float dist_GH;

extern char incoming_char;
extern bool voltage_training_complete;

/************* Functions Declaration *************/
void measure_features();
void check_classe();
void print_anomalies();
void training_ECU_dist();
void Log_queue_check(char log_array[ANOMALY_THRESHOLD]);




#endif
