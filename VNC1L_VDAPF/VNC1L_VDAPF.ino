
// VNC1L VDAPF


#define VNC1L_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 9600

#define Debug Serial
#define VNC1L Serial1


bool debug = true;
char buff[128];



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

  // send IPA
//  VNC1L.print("IPH");
//  VNC1L.print("IPA");
  VNC1L.write(0x91); // IPH
  VNC1L.write(0x0D);

  while ( ! returnCheckSCS() ) {
    Debug.println(":::send IPA/IPH:::");
    delay(500);
  }
  Debug.println(":::done IPA/IPH:::");

// using SCS default firmware
//  // send SCS
//  VNC1L.print("SCS");
//  VNC1L.write(0x0D);
//
//  while ( ! returnCheckSCS() ) {
//    Debug.println(":::send SCS:::");
//    delay(500);
//  }
//  Debug.println(":::done SCS:::");

  Debug.println(":::::Setup DONE:::::");

  Debug.println();
  Debug.println(":::::Loop START:::::");
}

void loop() {
  int inputValue = 0;

  if(Debug.available() > 0) {
    inputValue = Debug.read();

    if ( inputValue == 65 ) {  // A
      // Test
    } else if ( inputValue ==  68 ) {  // D
      // test
    } else if ( inputValue ==  69 ) {  // E
      //test

    } else if ( inputValue ==  97 ) {  // a
      // test
    } else if ( inputValue == 100 ) {  // d
      scsDIR();
    } else if ( inputValue == 101 ) {  // e
      scsE();
    } else if ( inputValue == 102 ) {  // f
      scsFWV();
    } else if ( inputValue == 103 ) {  // g
      // test
    } else if ( inputValue == 109 ) {  // m
      scsMKD("SAMPLE");
    }
  }

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////



void debugPrint(char* message) {
  if (debug) {
    Debug.print(">>>>> DEBUG: ");
    Debug.println(message);
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

bool returnCheckECS() {
  bool flg = false;

  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil(0x0d, buff, VNC1L.available());
    debugPrint(buff);
    if ( strcmp(buff, "D:\\>") == 0 ) {
      flg = true;
    }
    clearBuffer();
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

void clearSerialBuffer() {
  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil('\r', buff, VNC1L.available());

    if ( strcmp(buff, "DD2") == 0 ) {
      debugPrint("Disk Detected");
    } else if ( strcmp(buff, "DR2") == 0 ) {
      debugPrint("Disk Removed");
    } else if ( strcmp(buff, "ND") == 0 ) {
      debugPrint("No Disk");
    } else if ( strcmp(buff, ">") == 0 ) {
      debugPrint("<<< SCS PROMPT >>>");
    } else {
      debugPrint(buff);
    }
    clearBuffer();
  }
}

void retOutDump() {  // for DEBUG
  while (VNC1L.available() > 0 ) {
    Debug.print("available(): ");
    Debug.println(VNC1L.available());

    int readLen = VNC1L.readBytes(buff, 128);
    Debug.print("readLen: ");
    Debug.println(readLen);

//    for ( int i = 0; i < (sizeof(buff)/sizeof(buff[0])); i++ ) {
    for ( int i = 0; i < readLen; i++ ) {
      Debug.print(buff[i]);
      Debug.print("[");
      Debug.print(buff[i], HEX);
      Debug.print("]");
      Debug.println();
    }
    clearBuffer();
  }
}

void retOut() {
  int readLen;

  while (VNC1L.available() > 0 ) {
    readLen = VNC1L.readBytesUntil('\r', buff, 128);
    if ( strcmp(buff, "D:\\>") == 0 ) {
      Debug.println("<<< ECS PROMPT >>>");
    } else if ( strcmp(buff, ">") == 0 ) {
      Debug.println("<<< SCS PROMPT >>>");
    } else if ( strcmp(buff, "BC") == 0 ) {
      Debug.println("<<< Bad Command >>>");
    } else if ( strcmp(buff, "CF") == 0 ) {
      Debug.println("<<< Command Failed >>>");
    } else if ( strcmp(buff, "DF") == 0 ) {
      Debug.println("<<< Disk Full >>>");
    } else if ( strcmp(buff, "FI") == 0 ) {
      Debug.println("<<< Filename Invalid >>>");
    } else if ( strcmp(buff, "RO") == 0 ) {
      Debug.println("<<< Read Only >>>");
    } else if ( strcmp(buff, "FO") == 0 ) {
      Debug.println("<<< File Open >>>");
    } else if ( strcmp(buff, "NE") == 0 ) {
      Debug.println("<<< Dir Not Empty >>>");
    } else if ( strcmp(buff, "FN") == 0 ) {
      Debug.println("<<< Filename Invalid >>>");
    } else if ( strcmp(buff, "NU") == 0 ) {
      Debug.println("<<< No Upgrade >>>");
    } else if ( strcmp(buff, "DD2") == 0 ) {
      Debug.println("<<< Device inserted in USB Port 2 >>>");
    } else if ( strcmp(buff, "DR2") == 0 ) {
      Debug.println("<<< Device removed from USB Port 2 >>>");
    } else if ( strcmp(buff, "ND") == 0 ) {
      Debug.println("<<< No Disk >>>");
    } else {
      debugPrint(buff);
    }
    clearBuffer();
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////



void scsFWV() {
  Debug.println();
  Debug.println("::: scsFWV() :::");
  clearSerialBuffer();

  VNC1L.write(0x13); // FWV
  VNC1L.write(0x0D);
  delay(100);
  retOut();
}

void scsDIR() {
  Debug.println();
  Debug.println("::: scsDIR() :::");
  clearSerialBuffer();

  VNC1L.write(0x01); // DIR
  VNC1L.write(0x0D);
  delay(100);
  retOut();
//  retOutDump();
}

void scsMKD(char* filename) {
  Debug.println();
  Debug.println("::: scsMKD() :::");
  clearSerialBuffer();

  VNC1L.write(0x06); // MKD
  VNC1L.write(0x20);
  VNC1L.print(filename);
  VNC1L.write(0x0D);
  delay(100);
  retOut();
}

void scsE() {
  Debug.println();
  Debug.println("::: scsE() :::");
  clearSerialBuffer();

  VNC1L.write(0x45);  // E
  VNC1L.write(0x0D);
  delay(100);
  retOut();
}

