/*
void create_packet(){
  // ------------------------------------------------------------------------- EVENT FRAME HEADER --------------------------------------------------------------------------
  // BYTE 0 --> PROTOCOL VERSION & PROTOCOL HEADER
  uint8_t byte0 = 0b00010111;
  unsigned int protocol_version = ((byte0 & 0xF0) >> 4);
  cout << protocol_version;
  unsigned int protocol_header = byte0 & 0x0F;
  cout << protocol_header;

  // BYTE 1, 0: core0 1: core2  
  // TODO: c'è modo di capire da quale core proviene l'anomalia (risposta: sì)
  // BYTE 2, sensorID è l'identificativo della ECU da cui proviene l'anomalia
  uint8_t byte1 = 0b00000000;
  uint8_t byte2 = 0b01000001;
  unsigned int idsm_ID = ((byte2 & 0xC0) >> 6) |((byte1 << 2));
  unsigned int sensor_ID = (byte2 & 0x3F);

  // BYTE 3 E BYTE 4, tipo di evento avvenuto (es. quale rule è stata violata, che tipo di anomalia sul tempo) Customer specific ID: 0x8000-0xFFFE e Invalid ID: 0xFFFF
  /* Rule 1: diagnostic anomaly --> 8000
     Rule 2: ID anomaly --> 8001 TODO CONVERTIRE IN BIT
     Rule 3: rate anomaly --> 8002
     Rule 4: DLC anomaly --> 011
     Rule 5: counter anomaly --> 100
     *Rule 6: additional checks --> 101* CAPIRE SE TENERE
     Time 1: suspension anomaly --> 110
     Time 2: bus load anomaly --> 111
     Voltage 1: Voltage anomaly --> 101 
  uint8_t byte3 = 0b00000000;
  uint8_t byte4 = 0b00000000;
  // unsigned int event_ID = ;

  // BYTE 5 E BYTE 6, count inizializzato a 1, con aggregation_filter() aumenta
  uint8_t byte5 = 0b00000000 ;
  uint8_t byte6 = 0b00000001;
  // unsigned int count = ;

  // BYTE 7, reserved settato a 0
  uint8_t byte7 = 0b00000000;

  // ---------------------------------------------------------------------------- TIMESTAMP --------------------------------------------------------------------------------
   unsigned int source = 0b1; // 0 AUTOSAR STANDARD, 1 CUSTOM
   unsigned int timestamp = millis();
   // devo convertire timestamp (capiamo) da unsigned int a uint8_t per 7  (reserved che ci metto?) cambiare tipo della source, c++ compiler per test

  // -------------------------------------------------------------------------- CONTEXT DATA FRAME ------------------------------------------------------------------------
  // CONTEXT DATA LENGTH, capire in che formato comunicare la context data length
  // codice errore e poi uno fa lookup oppure stringa 
  unsigned int length_format = 0b0;
  unsigned int context_data_length = 0b0000000;
  
  // CONTEXT DATA, 7 byte
  uint8_t contex_data = 0b00000000;

  // ---------------------------------------------------------------------------- SIGNATURE -----------------------------------------------------------------------------------

  // TODO: capire con cosa firmare wolfssl(?) 

  unsigned int sign_len = 0b0000000000000000;
  unsigned int signature;

  // int IDS_message_size = 
  unsigned char IDS_message = new unsigned char[];
  //memcpy()


}
*/

/*
template <typename T>
void print_binary(T num) {
  for (int i = sizeof(T) * 8 - 1; i >= 0; i--) {
    cout << ((num >> i) & 1);
  }
  cout << "\n";
}
void create_packet(){
     // ------------------------------------------------------------------------- EVENT FRAME HEADER --------------------------------------------------------------------------
  // BYTE 0 --> PROTOCOL VERSION & PROTOCOL HEADER
  uint8_t byte0 = 0b00010111;
  unsigned int protocol_version = ((byte0 & 0xF0) >> 4);

  unsigned int protocol_header = byte0 & 0x0F;
  cout << "Protocol Header: "<< protocol_header << "\nProtocol Version: " << protocol_version<<  " \n" ;

  // BYTE 1 IsdMID
  // 0: core0 
  // 1: core2  
  // BYTE 2 sensorID
  /* ECU A: 0000
     ECU B: 0001
     ECU C: 0010
     ECU D: 0011
     ECU E: 0100
     ECU F: 0101
     ECU G: 0110
     ECU H: 0111
     Undefined ECU: 1111
  
  uint8_t byte1 = 0b00000000;
  uint8_t byte2 = 0b01000001;
  unsigned int idsm_ID = ((byte2 & 0xC0) >> 6) |((byte1 << 2));
  unsigned int sensor_ID = (byte2 & 0x3F);
  cout <<"IdsM ID: " <<idsm_ID << "\nSensor ID: " << sensor_ID <<  "\n";

  // BYTE 3 E BYTE 4, tipo di evento avvenuto (es. quale rule è stata violata, che tipo di anomalia sul tempo)
  /* Rule 1: diagnostic anomaly --> 8000 --> 1000000000000000
     Rule 2: ID anomaly --> 8001 --> 1000000000000001
     Rule 3: rate anomaly --> 8002 --> 1000000000000010
     Rule 4: DLC anomaly --> 8003 --> 1000000000000011
     Rule 5: counter anomaly --> 8004 --> 1000000000000100
     Rule 6: additional checks --> 8005 --> 1000000000000101
     Time 1: suspension anomaly --> 8006 --> 1000000000000110
     Time 2: bus load anomaly --> 8007 --> 1000000000000111
     Voltage 1: Voltage anomaly --> 8008 --> 1000000000001000
     Invalid_ID: 0xFFFF 
  uint8_t byte3 = 0b10000000;
  uint8_t byte4 = 0b00000000;
  uint16_t event_ID = (byte3 << 8) + byte4;
  cout << "Event ID: " << event_ID << "\n";

  // BYTE 5 E BYTE 6, count inizializzato a 1, con aggregation_filter() aumenta
  uint8_t byte5 = 0b00000000 ;
  uint8_t byte6 = 0b00000001;
  uint16_t count =  (byte5 << 8) + byte6;
  cout << "Count: " << count << "\n";

  // BYTE 7, reserved settato a 0
  uint8_t byte7 = 0b00000000;
  cout << "Reserved: " << static_cast<int>(byte7) << "\n";

  // ---------------------------------------------------------------------------- TIMESTAMP --------------------------------------------------------------------------------
   unsigned int source = 0b1; // 0 AUTOSAR STANDARD, 1 CUSTOM
   time_t timestamp = time(nullptr); // convert to millis() su arduino
   cout << "Timestamp: "<< timestamp << "\n";
   //unsigned int timestamp = millis();
   // devo convertire timestamp (capiamo) da unsigned int a uint8_t per 7  (reserved che ci metto?) cambiare tipo della source, c++ compiler per test

  // -------------------------------------------------------------------------- CONTEXT DATA FRAME ------------------------------------------------------------------------
  // CONTEXT DATA LENGTH, capire in che formato comunicare la context data length
  // codice errore e poi uno fa lookup oppure stringa 
  int length_format = 0b1;
  uint32_t context_data_length = 0b0000000000000000000000000000;
  cout << "Context Data Length Format: " << length_format << "\nContext Data Length: " << context_data_length <<  "\n";
  
  // CONTEXT DATA, n byte
  uint8_t contex_data = 0b00000001;
  cout << "Context Data: " << static_cast<int>(contex_data);
}*/


// ------------------------------------------------------- QUA PRINTA BIT A BIT DI LUNGHEZZA GIUSTA -----------------------------------------

//#include "aux_functions.h"
//#include <wolfssl/ssl.h>
//#include <wolfssl/cert_data.h>
// #include <ed25519/src/ed25519.h>
#include <iostream>
#include <string.h>
#include <string>
#include <cstddef> 
#include <ctime>
#include <vector>
#include <bitset>
#include <iomanip>
using namespace std;

string diagnostic_anomaly_info = "";
string id_anomaly_info = "";
string dlc_anomaly_info = "";
string rate_anomaly_info = "";
string bus_load_anomaly_info = "";
string counter_anomaly_info = "";
string additional_checks_anomaly_info = "";
string suspension_anomaly_info = "";
string period_anomaly_info = "";
string voltage_anomaly_info = "";

string anomaly_info = "";
/*
// FUNZIONI PER RACCOGLIERE I DATI DI CONTESTO DA METTERE NEL PACCHETTO 
string get_diagn_anomaly_info(){
  if(Diagnostic_anomaly<LogLength){
    for(int p = 0; p < Diagnostic_anomaly; p++){
      diagnostic_anomaly_info = "0x" + string(Diagn_ID_log[p],HEX);
    }
  }
  else{
    for(int p = 0; p < LogLength; p++){
      diagnostic_anomaly_info = "0x" + string(Diagn_ID_log[p],HEX) ;
    }
  }
  Diagnostic_anomaly=0;
  P_Diagn_ID_log=&Diagn_ID_log[0];
  for(int r = 0; r < LogLength; r++){
    Diagn_ID_log[r]=0;
  } return diagnostic_anomaly_info;
}

string get_id_anomaly_info(){
  if(ID_anomaly<LogLength){
    for(int s = 0; s < ID_anomaly; s++){
      id_anomaly_info = "0x" + string(Anomal_ID_log[s],HEX);
    }
  }
  else{
    for(int s = 0; s < LogLength; s++){
      id_anomaly_info = "0x"+ string(Anomal_ID_log[s],HEX);
    }
  }
  ID_anomaly=0;
  P_Anomal_ID_log=&Anomal_ID_log[0];
  for(int r = 0; r < LogLength; r++){
    Anomal_ID_log[r]=0;
  }return id_anomaly_info;
}

string get_dlc_anomaly_info(){
  if(DLC_anomaly<LogLength){
    for(int c = 0; c < DLC_anomaly; c++){
      dlc_anomaly_info = "0x" + string(Anomal_DLC_log[c] ,HEX);
    }
  }
  else{
    for(int c = 0; c < LogLength; c++){
      dlc_anomaly_info = "0x" + string(Anomal_DLC_log[c] ,HEX);
    }
  }
  DLC_anomaly=0;
  P_Anomal_DLC_log=&Anomal_DLC_log[0];
  for(int r = 0; r < LogLength; r++){
    Anomal_DLC_log[r]=0;
  }return dlc_anomaly_info;
}

string get_rate_anomaly_info(){
  if(Rate_anomaly<LogLength){
    for(int d = 0; d < Rate_anomaly; d++){
      rate_anomaly_info = "0x"+ string(Anomal_Rate_log[d],HEX);
    }
  }
  else{
    for(int d = 0; d < LogLength; d++){
      rate_anomaly_info = "0x" + string(Anomal_Rate_log[d],HEX);
    }
  }
  Rate_anomaly=0;
  P_Anomal_Rate_log=&Anomal_Rate_log[0];
  for(int r = 0; r < LogLength; r++){
    Anomal_Rate_log[r]=0;
  }return rate_anomaly_info;
}

string get_bus_load_anomaly_info(){
  if(BusLoad_anomaly !=0 ){
    bus_load_anomaly_info = "Bus Load greater than 80%";
  }
  BusLoad_anomaly=0;
  return bus_load_anomlay_info;
}

string get_counter_anomaly_info(){
  if(Counter_anomaly<LogLength){
         for(int c = 0; c < Counter_anomaly; c++){
          counter_anomaly_info = "0x"+ string(Anomal_Counter_log[c],HEX);
         }
       }
       else{
            for(int c = 0; c < LogLength; c++){
             counter_anomaly_info = "0x" + string(Anomal_Counter_log[c],HEX);
        }
       }
       Counter_anomaly=0;
       P_Anomal_Counter_log=&Anomal_Counter_log[0];
       return counter_anomaly_info;
}

string get_additional_anomaly_info(){
      if (VehicleSpeed_ID != 0){
      if(velocity_increase_anomaly != 0){
        additional_checks_anomaly_info = "Previous speed: " + string(anomal_prev_vel) + "  Following speed:"+ string(anomal_following_vel);
        }
      velocity_increase_anomaly=0;
      }
      return additional_checks_anomaly_info;
    // init_time=millis();
  }

// TODO: vedere dove si possono prendere dati per queste anomalie
string get_suspension_anomaly_info(){
  for(int i=0;i<Num_ID_TFP;i++){
        if(ID_sospesi[i]!=0){
          suspension_anomaly_info = "Suspended ID 0x" + string(White_List_ID[i],HEX);
          ID_sospesi[i]=0;
          }
        }
  return suspension_anomaly_info;
}

string get_period_anomaly_info(){
  for (int j=0;j<Num_ID_TFP;j++){
    if (*(pID_anomali_media + j)>=3){
      if(IDtrovato==j){
        period_anomaly_info = "Period anomaly detected for ID 0x" + string(White_List_ID[j],HEX) + "Average value measured = " + string(VetMediaTest[j]);
        if (*(pID_anomali_varianza + j)>=3){
           period_anomaly_info =+"Variance is anomalous as well";
        }
      }
    }
  }
  return period_anomaly_info;
}

string get_voltage_anomaly_info(){
  voltage_anomaly_info = "Voltage anomaly!";
  return voltage_anomaly_info;
  }
*/
string get_voltage_anomaly_info_dummy(){
  voltage_anomaly_info = "Voltage anomaly!";
  return voltage_anomaly_info;
  }

string get_anomaly_info(uint16_t event_id){
  if(event_id < 0x8000 && event_id > 0x8009){
    return "Invalid Event ID";
  }/*
  else if(event_id == 0x8000){
    anomaly_info = get_diagn_anomaly_info();
  }else if(event_id == 0x8001){
    anomaly_info = get_id_anomaly_info();
  }else if(event_id == 0x8002){
    anomaly_info = get_dlc_anomaly_info();
  }else if(event_id == 0x8003){
    anomaly_info = get_rate_anomaly_info();
  }else if(event_id == 0x8004){
    anomaly_info = get_counter_anomaly_info();
  }else if(event_id == 0x8005){
    anomaly_info = get_bus_load_anomaly_info();
  }else if(event_id == 0x8006){
    anomaly_info = get_additional_anomaly_info();
  }else if(event_id == 0x8007){
    anomaly_info = get_suspension_anomaly_info();
  }else if(event_id == 0x8008){
    anomaly_info = get_period_anomaly_info();
  }*/else if(event_id == 0x8009){
    //anomaly_info = get_voltage_anomaly_info();
    anomaly_info = get_voltage_anomaly_info_dummy();
  }
  return anomaly_info;
}
/*
vector<uint8_t> sign(vector<uint8_t> packet){
    // Load the CA certificate
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new();
    wolfSSL_CTX_load_verify_locations(ctx, "ca-cert.pem", NULL);

    // Create a new certificate
    WOLFSSL_CERT* cert = wolfSSL_cert_new();
    wolfSSL_cert_set_version(cert, 3); // Version 3
    wolfSSL_cert_set_serial_number(cert, 0); // Randomly generated serial number
    wolfSSL_cert_set_sig_type(cert, CTC_SHAwRSA); // Signature type

    // Set the subject information
    strncpy(cert->subject.country, "IT", CTC_NAME_SIZE);
    strncpy(cert->subject.state, "IT", CTC_NAME_SIZE);
    strncpy(cert->subject.locality, "Pisa", CTC_NAME_SIZE);
    strncpy(cert->subject.org, "yaSSL", CTC_NAME_SIZE);
    strncpy(cert->subject.unit, "Development", CTC_NAME_SIZE);
    strncpy(cert->subject.commonName, "www.wolfssl.com", CTC_NAME_SIZE);
    strncpy(cert->subject.email, "info@wolfssl.com", CTC_NAME_SIZE);

    // Load the private key
    WOLFSSL_KEY* key = wolfSSL_key_new();
    wolfSSL_key_load(key, "private-key.pem", NULL);

    // Sign the binary blob
    unsigned char* blob = packet.data();
    int blob_len = packet.size();
    unsigned char* signature = malloc(blob_len + 256); // Allocate space for the signature
    uint16_t sig_len = wolfSSL_sign(blob, blob_len, signature, key, cert);
    if (sig_len < 0) {
      cout << "Error in signing the packet";
        // Error signing the blob
    }
    packet.push_back(sig_len);
    packet.push_back(signature);

    // Free the resources
    wolfSSL_key_free(key);
    wolfSSL_cert_free(cert);
    wolfSSL_CTX_free(ctx);
    return packet;
}*/

/* funzione che controlla che l'eventi_ID sia del formato corretto (HEX) e che sia del valore compreso tra 8000 e 8009 altrimenti assegno invalid_ID*/
uint16_t check_event_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8009) {
        return event_id;
    } else {
        return 0xFFFF;
    }
}

uint8_t set_sensor_value(uint16_t event_id, uint8_t anomal_classe) {
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0b1000;
    } else if(event_id == 0x8009){
        return anomal_classe;
    }else return 0b1111;
}

uint8_t assign_idsm_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0;
    } else if(event_id == 0x8009){
        return 1;
    }else return 255; // undefined core
}

template <typename T>
void print_binary(T num, int num_bits) {
  for (int i = num_bits - 1; i >= 0; i--) {
    cout << ((num >> i) & 1);
  }
  cout << "";
}

void printAnomalyBitstream(const vector<uint8_t> anomaly_bitstream) {
  for (size_t i = 0; i < anomaly_bitstream.size(); i++) {
    print_binary(anomaly_bitstream[i], 8); // print each byte as 8-bit binary
  }
}

void print_binary_blob(const vector<uint8_t>& blob) {
    for (size_t i = 0; i < blob.size(); ++i) {
        bitset<8> b(blob[i]);
        cout << setw(2) << i << ": " << b.to_string() << " ";

        if ((i + 1) % 8 == 0) {
            cout << endl;
        }
    }
    cout << endl;
}

/*
void create_packet(uint16_t custom_event_id, uint8_t anomal_classe){
// BYTE 0 --> PROTOCOL VERSION & PROTOCOL HEADER ------------------------------------------------------ OK
  uint8_t byte0 = 0b00010111;
  unsigned int protocol_version = ((byte0 & 0xF0) >> 4);
  unsigned int protocol_header = byte0 & 0x0F;
  cout << "Protocol Version: ";
  print_binary(protocol_version,4);
  cout << "Protocol Header: ";
  print_binary(protocol_header,4);

  // BYTE 1 IsdMID
  // 0: core0 
  // 1: core2  
  // BYTE 2 sensorID
  /* ECU A: 0000
     ECU B: 0001
     ECU C: 0010
     ECU D: 0011
     ECU E: 0100
     ECU F: 0101
     ECU G: 0110
     ECU H: 0111
     CAN: 1000
     Undefined ECU: 1111
  
  uint8_t custom_sensor_id = set_sensor_value(custom_event_id, anomal_classe);
  uint8_t byte1 = (custom_sensor_id >> 6) & 0x03;
  uint8_t byte2 = custom_sensor_id & 0x3F;
  // uint8_t idsm_ID = ((byte2 & 0xC0) >> 6) |((byte1 << 2));
  custom_sensor_id = (byte2 & 0x3F);
  uint8_t idsm_ID = assign_idsm_id(custom_event_id);
  cout <<"IdsM ID: ";
  print_binary(idsm_ID,10);
  //cout <<"byte1: ";
  //print_binary(byte1,8);
  cout << "Sensor ID: ";
  print_binary(custom_sensor_id,6);
  //cout <<"byte2: ";
  //print_binary(byte2,8);

  // BYTE 3 E BYTE 4, tipo di evento avvenuto (es. quale rule è stata violata, che tipo di anomalia sul tempo)
  /* Rule 1: diagnostic anomaly --> 0x8000 --> 1000000000000000 
     Rule 2: ID anomaly --> 8001 --> 1000000000000001
     Rule 3: rate anomaly --> 8002 --> 1000000000000010
     Rule 4: DLC anomaly --> 8003 --> 1000000000000011
     Rule 5: counter anomaly --> 8004 --> 1000000000000100
     Rule 6: bus load anomaly --> 8005 --> 1000000000000101
     Rule 7: additional checks --> 8006 --> 1000000000000110
     Time 1: suspension anomaly --> 8007 --> 1000000000000111
     Time 2: Period check anomaly --> 8008 --> 1000000000001000
     Voltage 1: Voltage anomaly --> 8009 --> 1000000000001001
     Invalid_ID: 0xFFFF --> 1111111111111111 
  
  uint8_t byte3 = (custom_event_id >> 8) & 0xFF;
  uint8_t byte4 = custom_event_id & 0xFF;
  custom_event_id = (byte3 << 8) + byte4;
  custom_event_id = check_event_id(custom_event_id);
  cout << "Event ID: ";
  print_binary(custom_event_id,16);

  // BYTE 5 E BYTE 6, count inizializzato a 1, con aggregation_filter() aumenta
  uint8_t byte5 = 0b00000000 ;
  uint8_t byte6 = 0b00000001;
  uint16_t count =  (byte5 << 8) + byte6;
  cout << "Count: ";
  print_binary(count,16);

  // BYTE 7, reserved settato a 0 ------------------------------------------ OK
  uint8_t byte7 = 0b00000000;
  cout << "Reserved: "; 
  print_binary(byte7,8);

  // ---------------------------------------------------------------------------- TIMESTAMP --------------------------------------------------------------------------------
   unsigned int source = 0b1; // 0 AUTOSAR STANDARD, 1 CUSTOM
   cout << "Timestamp format: ";
   print_binary(source,1);
   time_t timestamp = time(nullptr);
   cout << "Timestamp: ";
   print_binary(timestamp,63); //custom_timestamp instead of timestamp
   //unsigned int timestamp = millis();
   // devo convertire timestamp (capiamo) da unsigned int a uint8_t per 7 cambiare tipo della source, c++ compiler per test

  // -------------------------------------------------------------------------- CONTEXT DATA FRAME ------------------------------------------------------------------------
  // CONTEXT DATA LENGTH, capire in che formato comunicare la context data length
  unsigned int length_format = 0b1;
  cout << "Context Data Length Format: ";
  print_binary(length_format,1);
  string context_data = get_anomaly_info(custom_event_id);
  vector<uint8_t> anomaly_bitstream(context_data.begin(), context_data.end());
  // byte anomaly_bitstream[context_data.length()];
 // anomaly_bitstream contains the bitstream representation of the anomaly info string
  size_t anomaly_bitstream_length = anomaly_bitstream.size();
  cout << "Context Data Length: ";
  print_binary(anomaly_bitstream_length, 32);
  // CONTEXT DATA, n byte chiamerò get_anomaly_info(custom_event_id);
  cout << "Context Data: ";
  printAnomalyBitstream(anomaly_bitstream);
}*/

// ------------------------------------------------------------------------- CREAZIONE PACCHETTO COME BINARY BLOB ----------------------------------------
vector<uint8_t> create_packet(uint16_t custom_event_id, uint8_t anomal_classe){//, string anomaly_info) {
    vector<uint8_t> packet;

    // BYTE 0 --> PROTOCOL VERSION & PROTOCOL HEADER ------------------------------------------------------ OK
    
    uint8_t byte0 = 0b00010111;
    packet.push_back(byte0);
    uint8_t custom_sensor_id = set_sensor_value(custom_event_id, anomal_classe);
    uint8_t idsm_ID = assign_idsm_id(custom_event_id);
    packet.push_back(idsm_ID);
    packet.push_back(custom_sensor_id & 0x3F);
    packet.push_back((custom_event_id >> 8) & 0xFF);
    packet.push_back(custom_event_id & 0xFF);
    packet.push_back(0x00); // counter
    packet.push_back(0x01); // counter
    packet.push_back(0x00); //reserverd 


    vector<uint8_t> timestamp_vec;
    time_t timestamp = time(nullptr); // get the current timestamp

    // Convert the timestamp to a 64-bit integer (assuming time_t is 32-bit or 64-bit)
    uint64_t timestamp_64 = static_cast<uint64_t>(timestamp);

    // Convert the 64-bit integer to a byte array (big-endian)
    for (int i = sizeof(uint64_t) - 1; i >= 0; --i) {
    timestamp_vec.push_back(static_cast<uint8_t>((timestamp_64 >> (i * 8)) & 0xFF));
    }
    timestamp_vec[0] = timestamp_vec[0] | 0x80; // added source = 1 CUSTOM 
    // Print the byte array
    for (uint8_t t : timestamp_vec) {
        packet.push_back(t);
        cout << static_cast<int>(t) << " ";
    }
    
    // uint8_t length_format = 0b10000000; // 4byte long context data length
    // packet.push_back(0x80); 

    string context_data = "Mattia sei bellissimo VIVALATOPA";//get_anomaly_info(custom_event_id);
    vector<uint8_t> anomaly_bitstream(context_data.begin(), context_data.end()); 
    size_t anomaly_bitstream_length = anomaly_bitstream.size(); //context data length
    /*
    packet.push_back(((anomaly_bitstream_length >> 24) & 0xFF) | 0x80);
    packet.push_back((anomaly_bitstream_length >> 16) & 0xFF);
    packet.push_back((anomaly_bitstream_length >> 8) & 0xFF); // Upper byte
    packet.push_back(anomaly_bitstream_length & 0xFF); // Lower byte
    */
   // itera le righe sopra per creare 4 byte di context data length, i=3 fa l'OR con il formato (1 per 4 byte lunghezza)
   for (int i = 3; i >= 0; --i) {
    uint8_t byte = (anomaly_bitstream_length >> (i * 8)) & 0xFF;
    if (i == 3) {
        byte |= 0x80;
    }
    packet.push_back(byte);
}

    // CONTEXT DATA, n byte chiamerò get_anomaly_info(custom_event_id);
    for (uint8_t byte : anomaly_bitstream) {
        packet.push_back(byte);
    }

/*
    // SIGNATURE 
    ed25519_keypair keypair;
    ed25519_keypair_from_seed(&keypair, my_secret_key, sizeof(my_secret_key)); // replace with your secret key

    size_t signature_length = ed25519_signature_size();
    vector<uint8_t> signature(signature_length);

    ed25519_sign(&keypair, packet.data(), packet.size(), signature.data());

    // APPEND SIGNATURE LENGTH AND SIGNATURE TO PACKET
    packet.push_back((signature_length >> 8) & 0xFF);
    packet.push_back(signature_length & 0xFF);
    packet.insert(packet.end(), signature.begin(), signature.end());
*/
    return packet;
}

int main(){
  uint8_t anomal_classe = 8; // ECU H
  uint16_t e_ID3 = 0x8009; // VOLTAGE ANOMALY
  vector<uint8_t> packet = create_packet(e_ID3, anomal_classe);//,context_data);
  unsigned char* blob = packet.data();
  size_t blob_size = packet.size();
  cout << "Pacchetto legit del core 2:";
  print_binary_blob(packet);
  //uint16_t e_ID = 0x8005; // RULE ADDITIONAL CHECKS
  //uint16_t e_ID2 = 0X80FF; // INVALID EVENT_ID
  //cout << "Pacchetto legit del core 0:\n";
  //create_packet(e_ID,anomal_classe);
  //cout << "Pacchetto legit del core 2:\n";
  //create_packet(e_ID3,anomal_classe);
  //cout << "Pacchetto con event_ID inesistente:\n";
  //create_packet(e_ID2,anomal_classe);

}