/*
  My name : Thuy Pham Xuan
  MICA Institute 
  Hanoi University of Science and Technology.

  This example is for Series 2 XBee receive radios.
  Receives a ZB RX packet from xbee module other. 
------------------------
  Using API packets :

  Modem Status packet 
  Frame type : 0x8A
  RF module status messages are sent from the module in response to specific conditions. 
  Example : when an API Coordinator forms a network, 
  Modem Status frame return Status : 0x02 = Joined network (routers and end devices).
  									 0x03 = Disassociated 

  ZigBee Receive packet 
  Frame type : 0x90
  When the module receive an RF packet (frame type = 0x10 or 0x11), it is sent out..
  ..the serial port using this message type.

 -----------------------
  Using xbee-arduino library : XBee Class, ModemStatusResponse Class, ZBRxResponse Class,..
  ..RxDataResponse Class.

  Modem Status = 0x02 <==> ASSOCIATED was defined on XBee.h file.
  Modem Status = 0x03 <==> DISASSOCIATED was defined on XBee.h file.
 -----------------------

  Using Arduino Lenado for Coordinator :  
        The Arduino Leonardo board uses 
        + Serial1 to communicate via TTL (5V) serial on pins 0 (RX) and 1 (TX). 
        + Serial is reserved for USB CDC communication. 

  */

#include <XBee.h>

// Create an XBee object at the top of sketch
XBee coordinator = XBee();

// RF module status messages are sent from the module in response to specific conditions
ModemStatusResponse cStatus = ModemStatusResponse();
// create response objects 
ZBRxResponse rx = ZBRxResponse();

uint8_t receiveLed = 13; 

void setup()
{
	pinMode(receiveLed, OUTPUT);

	Serial.begin(9600);
    Serial1.begin(9600);
    while (!Serial) {
            ; // wait for serial port to connect. Needed for Arduino Lenado
    }

    // Specify the serial port, for Arduino support multiple serial ports.
    coordinator.setSerial(Serial1);
    // starts the serail connection on the specify serial port. 
    coordinator.begin(Serial1); 

    // Startup delay to wait for XBee radio to initialize.
    // you may need to increase this value if you are not getting a response
    delay(5000);
}

void loop() {
	coordinator.readPacket(); 

	if (coordinator.getResponse().isAvailable()){
		// got something
		if (coordinator.getResponse().getApiId() == ZB_RX_RESPONSE){
			// got a zb rx packet
			digitalWrite(receiveLed, HIGH);
			delay(1000);
			digitalWrite(receiveLed, LOW);

			// now fill our zb rx class
            coordinator.getResponse().getZBRxResponse(rx);
            if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED){
            	// the sender got an ACK
            	Serial.println("Receive an ZB Xbee packet!");
            } else{
            	//sender didn't get an ACK
            	Serial.println("Receive an ZB Xbee packet!");
            }

            //read data payload
            uint8_t dataLength = rx.getDataLength();
            Serial.print("Data length = ");
            Serial.println(dataLength);

            Serial.print("Data = ");
            for(int i =0; i<dataLength; i++ ){
            Serial.print(rx.getData(i), HEX);
            Serial.print(" ");
          }

            Serial.println(" ");

		} else if (coordinator.getResponse().getApiId() == MODEM_STATUS_RESPONSE){
			coordinator.getResponse().getModemStatusResponse(cStatus);

			if (cStatus.getStatus() == COORDINATOR_STARTED) {  
            Serial.println("Coordinator started!");
          } else if (cStatus.getStatus() == DISASSOCIATED) {
            Serial.println("Coordinator Disassociated!");
          } else {
            // another status
            Serial.println("Another status!");
          }
		}
	} else if (coordinator.getResponse().isError()) {
	  // Error codes include: CHECKSUM_FAILURE, PACKET_EXCEEDS_BYTE_ARRAY_LENGTH, UNEXPECTED_START_BYTE 
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(coordinator.getResponse().getErrorCode());
    }
}
