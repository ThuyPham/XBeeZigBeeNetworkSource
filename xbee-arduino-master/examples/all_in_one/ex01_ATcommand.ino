/* 
	In this example, sends a few AT command queries to the radio and checks the status response for success.
	Using Arduino Lenado for Coordinator 
*/
#include <XBee.h>

// Create an XBee object at the top of your sketch
XBee xbee = XBee();

// Create an array for holding the data you want to send
// 1 - Serial number High : Read the high 32 bits of the module's unique 64-bit address
uint8_t shCmd[] = {'S','H'};
// 2 - Serial Number Low : Read the low 32 bits of the module's unique 64-bit address
uint8_t slCmd[] = {'S','L'}; 
// 3 - Association Indication : Read information regarding last node join request 
uint8_t assocCmd[] = {'A','I'}; 


AtCommandRequest atRequest = AtCommandRequest(shCmd);

AtCommandResponse atResponse = AtCommandResponse(); 

void setup()
{
	Serial.begin(9600);
	// tell XBee to use hardware serial1
	
	Serial1.begin(9600);
	xbee.setSerial(Serial);

	// Startup delay to wait for XBee radio to initialize.
  	// you may need to increase this value if you are not getting a response
  	delay(5000);

}

void loop() {

  // get SH
  sendAtCommand();
  
  // set command to SL
  atRequest.setCommand(slCmd);  
  sendAtCommand();

  // set command to AI
  atRequest.setCommand(assocCmd);  
  sendAtCommand();
  
  // we're done.  Hit the Arduino reset button to start the sketch over
  while (1) {};
}

void sendAtCommand() {
  Serial.println("Sending command to the XBee");

  // send the command
  xbee.send(atRequest);

  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        Serial.print("Command [");
        Serial.print(atResponse.getCommand()[0]);
        Serial.print(atResponse.getCommand()[1]);
        Serial.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(atResponse.getValueLength(), DEC);

          Serial.print("Command value: ");
          
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Serial.print(atResponse.getValue()[i], HEX);
            Serial.print(" ");
          }

          Serial.println("");
        }
      } 
      else {
        Serial.print("Command return error code: ");
        Serial.println(atResponse.getStatus(), HEX);
      }
    } else {
      Serial.print("Expected AT response but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // at command failed
    if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(xbee.getResponse().getErrorCode());
    } 
    else {
      Serial.print("No response from radio");  
    }
  }
}

