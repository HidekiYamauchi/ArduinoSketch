
// VNC1L tick test

#define VNC1L_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 9600

#define Debug Serial
#define VNC1L Serial1

bool debug = true;

char buff[128];
char response[64];
int cntTick = 0;

void setup() {
  Debug.begin(DEBUG_BAUD_RATE);
  VNC1L.begin(VNC1L_BAUD_RATE);

  Debug.println();
  Debug.println(":::::Setup START:::::");

  pinMode(8, OUTPUT);  // VNC1L RESET pin

  Debug.println(":::::VNC1L RESET:::::");

  // VNC1L RESET
  digitalWrite( 8, LOW );
  delay(1);
  digitalWrite( 8, HIGH );

  while ( ! checkOnline() ) {
    Debug.println(":::::VNC1L Starting:::::");
    delay(1000);
  }

  Debug.println(":::::VNC1L Online:::::");

  // send IPH
  VNC1L.write(0x91); // IPH
  VNC1L.write(0x0D);

  while ( ! returnCheckSCS() ) {
    Debug.println(":::send IPA/IPH:::");
    delay(500);
  }
  Debug.println(":::done IPA/IPH:::");

  Debug.println(":::::Setup DONE:::::");

  Debug.println();
  Debug.println(":::::Loop START:::::");
}

void loop() {
  int inputValue = 0;

  tick1();
  
  if(Debug.available() > 0) {
    inputValue = Debug.read();

    switch (inputValue) {
      case 100: // d: DIR
        scsDIR();
        break;
      case 102: // f: Firm Ver
        scsFWV();
        break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void usbReceive(char* message) {

  int strLen = strlen(message);
//  Debug.print(strLen);

  if ( strcmp(message, ">\r") == 0 ) {
    debugPrint("<<< SCS PROMPT >>>");
  } else {
    debugPrint(message);
  }

}

void tick1() {
  if ( VNC1L.available() > 0 ) {
    response[cntTick] = VNC1L.read();
    if ( cntTick > 30 || response[cntTick] == '\r' ) {
      response[cntTick+1] = 0;
      cntTick = 0;

      usbReceive(response);

    } else {
      cntTick++;
    }
  }
}



///////////////////////////////////////////////////////////////////////////////



void debugPrint(char* message) {
  if (debug) {
    Debug.print(">>>>> DEBUG: [");
    Debug.print(message);
    Debug.println("]");
  }
}

bool checkOnline() {
  bool flg = false;
  String strLog;
  
  while (VNC1L.available() > 0 ) {
    strLog = VNC1L.readString();
    debugPrint(strLog.c_str());
    if ( strLog.indexOf("On-Line:") > 0 ) {
      flg = true;
    }
  }

  return flg;
}

bool returnCheckSCS() {
  bool flg = false;

  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil(0x0d, buff, VNC1L.available());
    debugPrint(buff);
    if ( strcmp(buff, ">") == 0 ) {
      flg = true;
    }
    clearBuffer();
  }

  return flg;
}

void clearBuffer() {
  for ( int i = 0; i < (sizeof(buff)/sizeof(buff[0])); i++ ) {
    buff[i] = 0;
  }
}



///////////////////////////////////////////////////////////////////////////////



void scsFWV() {
  Debug.println();
  Debug.println("::: scsFWV() :::");
  Debug.println();

  VNC1L.write(0x13); // FWV
  VNC1L.write(0x0D);
}

void scsDIR() {
  Debug.println();
  Debug.println("::: scsDIR() :::");
  Debug.println();

  VNC1L.write(0x01); // DIR
  VNC1L.write(0x0D);
}

