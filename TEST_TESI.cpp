/*void create_packet(){
// programmino in c con main funzione void createpacket() printo interno pacchetto


    // TODO: capire come firmare il pacchetto
    // prendiamo l'ECU per autenticare parte del messaggio
    /* 
    char abs_path[MAX_PATH];
    if (!getcwd(abs_path, MAX_PATH)){
		perror("Error directory");
		return false;
     };

    string path = string(abs_path) + "/client_file/keys/" + client_session-> username + "_private_key.pem";
    EVP_PKEY *client_private_key = load_chiave_priv(path.c_str());
	if(client_private_key == nullptr){
		perror("Error loading client_private_key");
        return false;
    }

    // ELIMINABILE generazione chiave di sessione
    unsigned char *plaintext = new unsigned char[EVP_CIPHER_key_length(EVP_aes_256_gcm())];
    if(!RAND_bytes(plaintext, EVP_CIPHER_key_length(EVP_aes_256_gcm()))){
		perror("Error creating session key");
		delete_buffers(plaintext);
		return false;
    }

    //ELIMINABILE aggiungiamo la chiave di sessione alla sessione client
    memcpy(client_session->aes_key, plaintext, EVP_CIPHER_key_length(EVP_aes_256_gcm()));

    // ELIMINABILE cifriamo la chiave di sessione, usando la chiave pubblica effimera (eph_key_pub)
    unsigned char *ciphertext;
    int ciphertext_len;

    if(!rsaEncrypt(plaintext, EVP_CIPHER_key_length(EVP_aes_256_gcm()), client_session-> eph_key_pub, ciphertext, ciphertext_len)){
		perror("Error encrypting key session");
		delete_buffers(plaintext);
		return false;
     }

    //DIGITAL SIGNATURE
    //serializzare la lunghezza del ciphertext (cioè key_session cifrata)
    unsigned char *ciphertext_len_byte = new unsigned char[sizeof(long int)];
    serialize_longint(ciphertext_len, ciphertext_len_byte, sizeof(long int));

    //allocare il buffer per il blocchetto da firmare(quello giallo)
    //to_sign = key_session_len | ciphertext | nonce
    int to_sign_len = sizeof(long int) + ciphertext_len + NONCE_LEN;
    unsigned char *to_sign = new unsigned char[to_sign_len];

    //copiamo i dati nel buffer to_sign
    memcpy(to_sign, ciphertext_len_byte, sizeof(long int));
    memcpy(to_sign + sizeof(long int), ciphertext, ciphertext_len);
    memcpy(to_sign + sizeof(long int) + ciphertext_len, client_session-> nonceClient, NONCE_LEN);

    //creare la firma digitale
    int signature_len = EVP_PKEY_size(client_private_key);
    unsigned char *signature = new unsigned char[signature_len];
    if(create_digital_signature(client_private_key, to_sign, to_sign_len, signature) != signature_len){
		perror("Error: fail to create digital signature");
		delete_buffers(plaintext, ciphertext, to_sign, signature, ciphertext_len_byte);
		return false;
     }

    //allocare buffer per il payload
    //lunghezza in byte del blocco giallo
    unsigned char *to_sign_len_byte = new unsigned char[sizeof(int)];
    //lunghezza in byte della firma  
    unsigned char *signature_len_byte = new unsigned char[sizeof(int)];

    //serializzo la dimensione del payload
    serialize_int(to_sign_len, to_sign_len_byte);
	//serializza la dimensione della firma
    serialize_int(signature_len, signature_len_byte);
 
    //allochiamo la dimensione del blocco del messaggio M3
	// MESSAGE: to_sign_len | to_sign | signature_len | signature
    size_t message_size = sizeof(int) + to_sign_len + sizeof(int) + int_to_size_t(signature_len); 
    unsigned char *message = new unsigned char[message_size];

    //copiamo i dati nel buffer del messaggio
    //messaggio: to_sign_len, blocchetto giallo, signature_len, signature
    memcpy(message, to_sign_len_byte, sizeof(int));
    memcpy(message + sizeof(int), to_sign, to_sign_len);
    memcpy(message + sizeof(int) + to_sign_len, signature_len_byte, sizeof(int));
    memcpy(message + sizeof(int) + to_sign_len + sizeof(int), signature, signature_len);
	
	// invio messaggio M3
	if(send(client_session->socket, message, message_size,0) <0){
		perror("Error send M3");
		delete_buffers(plaintext, ciphertext, to_sign, signature, ciphertext_len_byte, signature_len_byte, message, to_sign_len_byte);
		return false;
	}

	delete_buffers(plaintext, ciphertext, to_sign, signature, ciphertext_len_byte, signature_len_byte, message, to_sign_len_byte);
	return true;
}*/


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

#include <cstdint>
#include <iostream>
using namespace std;
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

#include <iostream>
#include <ctime>
using namespace std;

/* funzione che controlla che l'eventi_ID sia del formato corretto (HEX) e che sia del valore compreso tra 8000 e 8009 altrimenti assegno invalid_ID*/
uint16_t check_event_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8009) {
        return event_id;
    } else {
        return 0xFFFF;
    }
}
// MANCA CASO IN CUI EVENT_ID NON VALIDO ANCHE IL SENSORE DEVE AVERE IL VALORE 1111
uint8_t set_sensor_value(uint16_t event_id, uint8_t anomal_classe) {
    // uint8_t idsm_id = (event_id >= 0x8000 && event_id <= 0x8008) ? 0 : 1;
    //uint8_t sensor_value = (event_id >= 0x8000 && event_id <= 0x8008) ? 0b1111 : anomal_classe;
    //return sensor_value;
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0b1111;
    } else if(event_id == 0x8009){
        return anomal_classe;
    }else return 0b11111111;
}

uint8_t assign_idsm_id(uint16_t event_id) {
    if (event_id >= 0x8000 && event_id <= 0x8008) {
        return 0;
    } else if(event_id == 0x8009){
        return 1;
    }else return 255;
}

template <typename T>
void print_binary(T num, int num_bits) {
  for (int i = num_bits - 1; i >= 0; i--) {
    cout << ((num >> i) & 1);
  }
  cout << "\n";
}

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
     Undefined ECU: 1111
  */
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
     Invalid_ID: 0xFFFF --> 1111111111111111 */
  
  uint8_t byte3 = (custom_event_id >> 8) & 0xFF;
  uint8_t byte4 = custom_event_id & 0xFF;
  custom_event_id = (byte3 << 8) + byte4;
  custom_event_id = check_event_id(custom_event_id);
  cout << "Event ID: ";
  print_binary(custom_event_id,16);
/*
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
  // codice errore e poi uno fa lookup oppure stringa 
  unsigned int length_format = 0b1;
  uint32_t context_data_length = 0b0000000000000000000000000000;
  cout << "Context Data Length Format: ";
  print_binary(length_format,1);
  cout << "Context Data Length: ";
  print_binary(context_data_length,32);
  
  // CONTEXT DATA, n byte
  uint8_t contex_data = 0b00000001;
  cout << "Context Data: ";
  print_binary(contex_data,8);*/
}

int main(){
  // uint8_t s_ID = 5;
  uint8_t anomal_classe = 8;
  uint16_t e_ID = 0x8006;
  uint16_t e_ID3 = 0x8009;
  uint16_t e_ID2 = 0X80FF;
  cout << "Pacchetto legit del core 0:\n";
  create_packet(e_ID,anomal_classe);
  cout << "Pacchetto legit del core 2:\n";
  create_packet(e_ID3,anomal_classe);
  cout << "Pacchetto con event_ID inesistente:\n";
  create_packet(e_ID2,anomal_classe);

}