/*
  My name : Thuy Pham Xuan
  MICA Institute 
  Hanoi University of Science and Technology.

  This example is for Series 2 XBee Router radios.
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
  Modem Status frame return Status : 0x02 = Joined network (routers and end devices).
-------------------------------

  Using xbee-arduino library : AtCommandRequest Class; AtCommandResponse Class; ..
  XBee Class, ModemStatusResponse Class. 

  Modem Status = 0x02 <==> ASSOCIATED was defined on XBee.h file.

-------------------------------

  ZigBee Router : Discovering ZigBee network 
    To discover nearby Zigbee networks, the Router performs a PAN (or active) scan. 
  
  During the PAN scan, the Router sends a beacon request (broadcast) transmission on the..
  ..first channel in its scan channel list. All nearby Coordinators and Routers operating on..
  ..that channel respond to the beacon request by sending a beacon back to the Router. 
  ..The beacon contains information about the PAN the nearby device is on, including the PAN ID,..
  ..and whether or not joining is allowed. 

  When a ZB Router has successfully joined a network, it : 
    -Allows other devices to join the network for a time
    -Sets AI=0
    -Starts blinking the Associate LED
    -Sends an API modem status frame ("Associated = 0x02") out the serial port when using API mode.

  + Read the API Modem Status frame, "ASSOCIATED" = 0x02 is sent out the serial port.
    the Router successfully joined a network. 
  + AI command : association status , will return a value of 0, the Router has joined a PAN.
  + CH command : read the operating channel the router joined, 16 channels : 0-15 = 0x0B - 0x1A 
  + SH command : Serial Number High - Read high 32 bits of the RF module's unique IEEE 64 bit address.
  + SL command : Serial Number Low - Read low 32 bits of the RF module's unique IEEE 64 bit address.
  + OP command : read the operating 64-bit PAN ID
  + OI command : Read the operating 16-bit PAN ID
  + MY command : Get the 16 bit address of the module. 
  + NC command : Number of Children : Read the number of end device children that have..
                ..joined to the device. This command returns the number of child table entries currently in use.
  + PL command : Power level : select/read the power level at which the RF module transmits conducted.
-------------------------------

  64bit PAN ID was set to 2016. Checked by OP command 
  Using Arduino UNO for Router :  
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

// Create an XBee object at the top of sketch
XBee router = XBee();

// RF module status messages are sent from the module in response to specific conditions
ModemStatusResponse rStatus = ModemStatusResponse();
// AT Command Request - used to query or set module parameters.
// Reading parameter values
AtCommandRequest rAtRequest = AtCommandRequest();
// Response to an AT Command message
AtCommandResponse rAtResponse = AtCommandResponse(); 

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

  // Specify the serial port, for Arduino support multiple serial ports.
  router.setSerial(Serial);
  // starts the serail connection on the specify serial port. 
  router.begin(Serial); 
  // start soft serial
  nss.begin(9600);

  // Startup delay to wait for XBee radio to initialize.
  // you may need to increase this value if you are not getting a response
  delay(5000);
}

void loop(){
  // wait for XBee radio to initialize : Channel selection, PAN ID selection
  
//  if(router.readPacket(5000)){
//    if(router.getResponse().getApiId() == MODEM_STATUS_RESPONSE){
//    router.getResponse().getModemStatusResponse(rStatus);
//    if(rStatus.getStatus() == ASSOCIATED) 
//      { nss.println("Router joined! Welcom to XBee ZigBee Network!"); }
//    }
  

  // read association status 
  rAtRequest.setCommand(assocCmd);
  sendAtCommand();

  // read operating channel 
  rAtRequest.setCommand(channelCmd);
  sendAtCommand();

  // read serial high 
  rAtRequest.setCommand(shCmd);
  sendAtCommand();

  // read serial low 
  rAtRequest.setCommand(slCmd);
  sendAtCommand();

  // read operating 64-bit PAN ID
  rAtRequest.setCommand(panId64Cmd);
  sendAtCommand();

  // read operating 16-bit PAN ID
  rAtRequest.setCommand(panId16Cmd);
  sendAtCommand();

  // read 16 bit address
  rAtRequest.setCommand(netaddCmd);
  sendAtCommand();

  // read Number of Children
  rAtRequest.setCommand(numchilCmd);
  sendAtCommand();

  // read Power level 
  rAtRequest.setCommand(powlev);
  sendAtCommand();


  while(1){};
  
}


void sendAtCommand(){
  nss.println("Sending command to the XBee : ");

  // send the command 
  router.send(rAtRequest);

  // wait up to 5 seconds for status response
  if(router.readPacket(5000)){
    // got a response! 

    //should be an AT Command response 
    if(router.getResponse().getApiId() == AT_COMMAND_RESPONSE){
      router.getResponse().getAtCommandResponse(rAtResponse);

      if(rAtResponse.isOk()){
        nss.print("Command [");
        nss.print(rAtResponse.getCommand()[0], HEX);
        nss.print(rAtResponse.getCommand()[1], HEX);
        nss.println("] was successful!");

        if (rAtResponse.getValueLength() > 0) {
          nss.print("Command value length is ");
          nss.println(rAtResponse.getValueLength(), DEC);

          nss.print("Command value: ");
          
          for (int i = 0; i < rAtResponse.getValueLength(); i++) {
            nss.print(rAtResponse.getValue()[i], HEX);
            nss.print(" ");
          }

          nss.println("");
        }

      }
      else {
        // Status of At command != OK (0x00)
        nss.print("Command return error code: ");
        nss.println(rAtResponse.getStatus(), HEX);
      }
    }
    else{
      // ApiTd != AT_COMMAND_RESPONSE
      nss.print("Expected AT response but got ");
      nss.print(router.getResponse().getApiId(), HEX);
    }
  }
  else{
    // At command failed 
    if(router.getResponse().isError()){
      nss.print("Error reading packet.  Error code: ");  
      nss.println(router.getResponse().getErrorCode());
    }
    else{
      nss.println("No response from radio.");
    }
  }

}
