/*
  My name : Thuy Pham Xuan
  MICA Institute 
  Hanoi University of Science and Technology.

  This example is for Series 2 XBee transmit radios.
  Sends a ZB Transmit Request  and checks the response for success with ZigBee Transmit Status
 -------------------
  Using API packets :

  Zigbee Transmit Request packet
  Frame type : 0x10 
  An Transmit Request API frame causes the module to send data as an RF packet to..
  ..the specified destination.
  
  Sending a Zigbee Transmit Request (0x10) to XBee module, then a ZigBee Transmit Status (0x8B) is..
  ..sent out the serial port on this XBee module.
  
  ZigBee Transmit Status packet
  Frame type : 0x8B
  When a TX Request is completed, the module sends a TX Status message. This message will indicate if..
  ..the packet was transmitted successfully or it there was a failure. 
  Status = 0x00 <==> Success 

 -------------------
  Using xbee-arduino library : XBee Class, ZBTxRequest Class, ZBTxStatusResponse Class. 
  XBeeAddress64 Class. 
  Delivery status = 0x00 <==> SUCCESS was defined on XBee.h file. 
 -------------------
  Using Arduino UNO for End Device :  
        The Arduino UNO board with SoftwareSerial uses : 
        + Serial to communicate via TTL (5V) serial on pins 0 (RX) and 1 (TX). 
        + Define SoftSerial TX/RX pins : pin 8 to TX, pin 9 to RX of usb-serial device
*/

#include <XBee.h> 
#include <SoftwareSerial.h>

// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

// send data led
uint8_t sendDataLed = 13; 

// Create an XBee object at the top of sketch
XBee endDevice = XBee();

// payload1 - string : "TxData" = 0x54, 0x78, 0x44, 0x61, 0x74, 0x61
uint8_t payload1[] = {0x54, 0x78, 0x44, 0x61, 0x74, 0x61};
// payload2 - number : 25 = 0x19 
uint8_t payload2[] = {0x19};

// SH + SL Address of Receiving Xbee : Coordinator 
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x40CADFA6); 
ZBTxRequest zbTx1 = ZBTxRequest(addr64, payload1, sizeof(payload1));
ZBTxRequest zbTx2 = ZBTxRequest(addr64, payload2, sizeof(payload2));
ZBTxStatusResponse txStatus = ZBTxStatusResponse(); 

void setup(){
	pinMode(sendDataLed, OUTPUT);

	Serial.begin(9600);

    // Specify the serial port, for Arduino support multiple serial ports.
    endDevice.setSerial(Serial);
    // starts the serail connection on the specify serial port. 
    endDevice.begin(Serial); 
    // start soft serial
    nss.begin(9600);

    // Startup delay to wait for XBee radio to initialize.
    // you may need to increase this value if you are not getting a response
    delay(5000);
}

void loop(){

// Sending payload1 data 	
	endDevice.send(zbTx1);
	digitalWrite(sendDataLed, HIGH);
	delay(1000);
	digitalWrite(sendDataLed, LOW);
    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (endDevice.readPacket(500)){
    	// got a response!

    	// should be a znet tx status
    	if (endDevice.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
    		endDevice.getResponse().getZBTxStatusResponse(txStatus);

    	// get the delivery status
        if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success 
        nss.println("Zigbee Transmit status = Success!");
      } else {
        // the remote XBee did not receive our packet. 
        nss.println("Zigbee Transmit status = Error!");
      }	

    	}

    } 
    
  delay(1000);

// Sending payload2 data 	
	endDevice.send(zbTx2);
	digitalWrite(sendDataLed, HIGH);
	delay(1000);
	digitalWrite(sendDataLed, LOW);
    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (endDevice.readPacket(500)){
    	// got a response!

    	// should be a znet tx status
    	if (endDevice.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
    		endDevice.getResponse().getZBTxStatusResponse(txStatus);

    	// get the delivery status
        if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success 
        nss.println("Zigbee Transmit status = Success!");
      } else {
        // the remote XBee did not receive our packet. 
        nss.println("Zigbee Transmit status = Error!");
      }	

    	}

    } 
    
  delay(1000);
}
