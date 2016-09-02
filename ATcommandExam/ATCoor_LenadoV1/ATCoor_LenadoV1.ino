/*
  My name : Thuy Pham Xuan
  MICA Institute 
  Hanoi University of Science and Technology.

  This example is for Series 2 XBee Coordinator radios.
  Send a few *AT command queries* to the radio and checks the status response for success.
--------------------------------
  Using API packets : 

  AT Command packet
  Frame type : 0x08
  Used to query or set module parameters on the local device. 
  This API command applies changes after executing the command.

  AT command - Queue Parameter Value packet 
  Frame type : 0x09 
  this API type allows module parameters to be queried or set. 
  In contrast to the "AT command" API type, new parameter values are queued and not applied..
  ..until either the "AT Command"(0x08) API type or the AC (Apply Changes) command is issued.
  Register queries (Reading parameter values) are returned immediately.

  Sending a AT Command Request (0x08 or 0x09) to XBee module, then a AT Command Response (0x88) is..
  ..sent out the serial port on this XBee module.

  AT Command Response packet 
  Frame type : 0x88
  In response to an AT Command message, the module will send and AT Command Response message. 
  Some commands will send back multiple frames (for example, the ND (Node Discover) command).

  Modem Status packet 
  Frame type : 0x8A
  RF module status messages are sent from the module in response to specific conditions. 
  Example : when an API Coordinator forms a network, 
  Modem Status frame return Status : 0x06 = Coordinator started. 
-------------------------------

  Using xbee-arduino library : AtCommandRequest Class; AtCommandResponse Class; ..
  XBee Class, ModemStatusResponse Class.

  Modem Status = 0x06 <==> COORDINATOR_STARTED was defined on XBee.h file. 

-------------------------------

  ZigBee Coordinator : Forming a network 
  + Channel selection : 
    When starting a network, the coordinator must select a "good" channel..
    ..for the network to operate on.
    To do this, it performs an energy scan on ultiple channels to detect energy levels..
    ..on each channel.

  + PAN ID selection : 
    After completing the energy scan, the coordinator scans its list of potential channels..
    ..to obtain a list of neighboring PANs.
    To do this, the coordinator sends a beacon request (broadcast) transmission on each potential channel.
    All nearby coordinators and routers (that have already joined a ZigBee network) will respond..
    ..to the beacon request by sending a beacon back to the coordinator. The beacon contains information..
    ..about the PAN the device is on, including the PAN identifiers (16 bit and 64 bit).
    After the coordinator completes the channel and PAN scan, it selects a random channel and unused..
    ..16-bit PAN ID to start on.

  + When the Coordinator has successfully started a network, it : 
    -Allow other devices to join the network for a time
    -Sets AI = 0
    -Starts blinking the Associate LED
    -Sends an API modem status frame ("Coordinator started = 0x06") out the serial port when..
     ..using API mode. 


  + Read the API Modem Status frame, "COORDINATOR_STARTED" = 0x06 is sent out the serial port.
    the Coordinator has selected a channel and PAN ID. 
  + AI command : association status , will return a value of 0, indicating a successful startup.
  + CH command : read the operating channel, 16 channels : 0-15 = 0x0B - 0x1A 
  + SH command : Serial Number High - Read high 32 bits of the RF module's unique IEEE 64 bit address.
  + SL command : Serial Number Low - Read low 32 bits of the RF module's unique IEEE 64 bit address.
  + OP command : read the operating 64-bit PAN ID
  + OI command : Read the operating 16-bit PAN ID
  + MY command : 16 bit address, will return a value of 0, the ZigBee defined 16-bit address..
                 ..of the Coordinator.
  + NC command : Number of Children : Read the number of end device children that have..
                ..joined to the device. This command returns the number of child table entries currently in use.
  + PL command : Power level : select/read the power level at which the RF module transmits conducted. 
-------------------------------

  64bit PAN ID was set to 2016. Checked by OP command 
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
// AT Command Request - used to query or set module parameters.
// Reading parameter values
AtCommandRequest cAtRequest = AtCommandRequest();
// Response to an AT Command message
AtCommandResponse cAtResponse = AtCommandResponse(); 

//--- set data for AT Command 
// association status
uint8_t assocCmd[] = {'A','I'};
// the operating channel
uint8_t channelCmd[] = {'C','H'};
// serial high
uint8_t shCmd[] = {'S','H'}; 
// serial low
uint8_t slCmd[] = {'S','L'};
// operating 64-bit PAN ID
uint8_t panId64Cmd[] = {'O','P'};
// operating 16-bit PAN ID
uint8_t panId16Cmd[] = {'O','I'};
// 16 bit address
uint8_t netaddCmd[] = {'M','Y'};
// Number of Children 
uint8_t numchilCmd[] = {'N','C'};
// Power level
uint8_t powlev[] = {'P','L'};

void setup()
{
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

void loop(){
  // wait for XBee radio to initialize : Channel selection, PAN ID selection
  
//  if(coordinator.readPacket(5000)){
//    if(coordinator.getResponse().getApiId() == MODEM_STATUS_RESPONSE){
//    coordinator.getResponse().getModemStatusResponse(cStatus);
//    if(cStatus.getStatus() == COORDINATOR_STARTED) 
//      { Serial.println("Coordinator Started! Welcom to XBee ZigBee Network!"); }
//    }
  

  // read association status 
  cAtRequest.setCommand(assocCmd);
  sendAtCommand();

  // read operating channel 
  cAtRequest.setCommand(channelCmd);
  sendAtCommand();

  // read serial high 
  cAtRequest.setCommand(shCmd);
  sendAtCommand();

  // read serial low 
  cAtRequest.setCommand(slCmd);
  sendAtCommand();

  // read operating 64-bit PAN ID
  cAtRequest.setCommand(panId64Cmd);
  sendAtCommand();

  // read operating 16-bit PAN ID
  cAtRequest.setCommand(panId16Cmd);
  sendAtCommand();

  // read 16 bit address
  cAtRequest.setCommand(netaddCmd);
  sendAtCommand();

  // read Number of Children
  cAtRequest.setCommand(numchilCmd);
  sendAtCommand();

  // read Power level 
  cAtRequest.setCommand(powlev);
  sendAtCommand();

  while(1){};
  
}

  


void sendAtCommand(){
  Serial.println("Sending command to the XBee : ");

  // send the command 
  coordinator.send(cAtRequest);

  // wait up to 5 seconds for status response
  if(coordinator.readPacket(5000)){
    // got a response! 

    //should be an AT Command response 
    if(coordinator.getResponse().getApiId() == AT_COMMAND_RESPONSE){
      coordinator.getResponse().getAtCommandResponse(cAtResponse);

      if(cAtResponse.isOk()){
        Serial.print("Command [");
        Serial.print(cAtResponse.getCommand()[0], HEX);
        Serial.print(cAtResponse.getCommand()[1], HEX);
        Serial.println("] was successful!");

        if (cAtResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(cAtResponse.getValueLength(), DEC);

          Serial.print("Command value: ");
          
          for (int i = 0; i < cAtResponse.getValueLength(); i++) {
            Serial.print(cAtResponse.getValue()[i], HEX);
            Serial.print(" ");
          }

          Serial.println("");
        }

      }
      else {
        // Status of At command != OK (0x00)
        Serial.print("Command return error code: ");
        Serial.println(cAtResponse.getStatus(), HEX);
      }
    }
    else{
      // ApiTd != AT_COMMAND_RESPONSE
      Serial.print("Expected AT response but got ");
      Serial.print(coordinator.getResponse().getApiId(), HEX);
    }
  }
  else{
    // At command failed 
    if(coordinator.getResponse().isError()){
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(coordinator.getResponse().getErrorCode());
    }
    else{
      Serial.println("No response from radio.");
    }
  }

}
