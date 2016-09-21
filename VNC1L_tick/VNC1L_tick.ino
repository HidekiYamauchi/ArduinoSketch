
// VNC1L tick test

#define VNC1L_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 115200

#define Debug Serial
#define VNC1L Serial2

bool debug = true;

char buff[128];
char response[64];
int cntTick = 0;

bool flgDisk = false;

void setup() {
  Debug.begin(DEBUG_BAUD_RATE);
  VNC1L.begin(VNC1L_BAUD_RATE);

  Debug.println();
  Debug.println(":::::Setup START:::::");

  pinMode(8, OUTPUT);  // VNC1L RESET pin

  Debug.println(":::VNC1L RESET:::");

  // VNC1L RESET
  digitalWrite( 8, LOW );
  delay(1);
  digitalWrite( 8, HIGH );

  while ( ! checkOnline() ) {
    Debug.println(":::VNC1L Starting:::");
    delay(1000);
  }

  Debug.println(":::VNC1L Online:::");

  // send SCS
  VNC1L.print("SCS"); // SCS
  VNC1L.write(0x0D);
  while ( ! returnCheckSCS() ) {
    Debug.println(":::send SCS:::");
    delay(500);
  }
  Debug.println(":::done SCS:::");

  // send IPA
  VNC1L.write(0x90); // IPA
  VNC1L.write(0x0D);

  while ( ! returnCheckSCS() ) {
    Debug.println(":::send IPA:::");
    delay(500);
  }
  Debug.println(":::done IPA:::");

  // check Disk
  while ( ! checkDisk() ) {
    Debug.println(":::check Disk:::");
    delay(1000);
  }
  Debug.println(":::Disk OK:::");



  Debug.println(":::::Setup DONE:::::");
}

void loop() {
  int inputValue = 0;

  tick1();
  
  if(Debug.available() > 0) {
    inputValue = Debug.read();

    switch (inputValue) {
      case  98: // b: CD ..
        scsCD();
        break;
      case  99: // c: CD filename
        scsCD("SAMPLE");
        break;
      case 100: // d: DIR
        scsDIR();
        break;
      case 102: // f: Firm Ver
        scsFWV();
        break;
      case 105: // i: IDD
        scsIDD();
        break;
      case 109: // m: mkdir filename
        scsMKD("SAMPLE");
        break;
      case 111: // o: open filename for write
        scsOPW("FABOTEST.TXT");
        break;
      case 119: // w: write data
        scsWRF("FaBo Test !");
        break;
      case 120: // x: close filename
        scsCLF("FABOTEST.TXT");
        break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void usbReceive(char* message) {

  int strLen = strlen(message);
  Debug.print(strLen);

  if ( strcmp(message, ">\r") == 0 ) {
    debugPrint("<<< SCS PROMPT >>>");
  } else if ( strcmp(message, "DD2\r") == 0 ) {
    debugPrint("<<< Disk Detected >>>");
  } else if ( strcmp(message, "DR2\r") == 0 ) {
    debugPrint("<<< Disk Removed >>>");
  } else if ( strcmp(message, "ND\r") == 0 ) {
    debugPrint("<<< No Disk >>>");
  } else if ( strcmp(message, "CF\r") == 0 ) {
    debugPrint("<<< Command Failed >>>");
  } else if ( strcmp(message, "BC\r") == 0 ) {
    debugPrint("<<< Bad Command >>>");
  } else if ( strcmp(message, "DF\r") == 0 ) {
    debugPrint("<<< Disk Full >>>");
  } else if ( strcmp(message, "FI\r") == 0 ) {
    debugPrint("<<< Filename Invalid >>>");
  } else if ( strcmp(message, "RO\r") == 0 ) {
    debugPrint("<<< Read Only >>>");
  } else if ( strcmp(message, "FO\r") == 0 ) {
    debugPrint("<<< File Opened >>>");
  } else if ( strcmp(message, "NE\r") == 0 ) {
    debugPrint("<<< Dir Not Empty >>>");
  } else if ( strcmp(message, "FN\r") == 0 ) {
    debugPrint("<<< Filename Invalid >>>");
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
//    debugPrint(strLog.c_str());
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
//    debugPrint(buff);
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

bool checkDisk() {
  bool flg = false;
  String strLog;

  VNC1L.write(0x01); // DIR
  VNC1L.write(0x0D);

  while (VNC1L.available() > 0 ) {
    strLog = VNC1L.readStringUntil(0x0d);
//    Debug.print(">>>>> DEBUG: [");
//    Debug.print(strLog);
//    Debug.println("]");
    if ( strLog == ">" ) {
      flg = true;
    }
  }

  return flg;
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

void scsCD() {
  Debug.println();
  Debug.println("::: scsCD(..) :::");
  Debug.println();

  VNC1L.write(0x02); // CD .. / up directory
  VNC1L.write(0x20);
  VNC1L.write(0x2E);
  VNC1L.write(0x2E);
  VNC1L.write(0x0D);
}

void scsCD(char* filename) {
  Debug.println();
  Debug.print("::: scsCD(");
  Debug.print(filename);
  Debug.println(") :::");
  Debug.println();

  VNC1L.write(0x02); // CD
  VNC1L.write(0x20);
  VNC1L.print(filename);
  VNC1L.write(0x0D);
}

void scsMKD(char* filename) {
  Debug.println();
  Debug.print("::: scsMKD(");
  Debug.print(filename);
  Debug.println(") :::");
  Debug.println();

  VNC1L.write(0x06); // MKD
  VNC1L.write(0x20);
  VNC1L.print(filename);
  VNC1L.write(0x0D);
}

void scsOPW(char* filename) {
  Debug.println();
  Debug.print("::: scsOPW(");
  Debug.print(filename);
  Debug.println(") :::");
  Debug.println();

  VNC1L.write(0x09); // OPW
  VNC1L.write(0x20);
  VNC1L.print(filename);
  VNC1L.write(0x0D);
}

void scsWRF(char* data) {
  Debug.println();
  Debug.print("::: scsWRF(");
  Debug.print(data);
  Debug.println(") :::");
  Debug.println();

  int dataLen = strlen(data);

  VNC1L.write(0x08); // WRF
  VNC1L.write(0x20);
  VNC1L.print(dataLen);
  VNC1L.write(0x0D);
  VNC1L.print(data);
}

void scsCLF(char* filename) {
  Debug.println();
  Debug.print("::: scsCLF(");
  Debug.print(filename);
  Debug.println(") :::");
  Debug.println();

  VNC1L.write(0x0A); // CLF
  VNC1L.write(0x20);
  VNC1L.print(filename);
  VNC1L.write(0x0D);
}

void scsIDD() {
  Debug.println();
  Debug.println("::: scsIDD() :::");
  Debug.println();

  VNC1L.write(0x0F); // IDD
  VNC1L.write(0x0D);
}


