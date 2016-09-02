

#define VNC1L_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 9600

#define Debug Serial
#define VNC1L Serial1

char buff[65];

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

  // send IPA
  VNC1L.print("IPA");
  VNC1L.write(13);

  while ( ! returnCheckECS() ) {
    Debug.println(":::::send IPA:::::");
    delay(500);
  }
  Debug.println(":::::done IPA:::::");

  // send SCS
  VNC1L.print("SCS");
  VNC1L.write(13);

  while ( ! returnCheckSCS() ) {
    Debug.println(":::::send SCS:::::");
    delay(500);
  }
  Debug.println(":::::done SCS:::::");

  Debug.println(":::::Setup DONE:::::");

  Debug.println();
  Debug.println(":::::show Version:::::");
  // firmware version
  getFirmwareVersion();

  Debug.println();
  Debug.println(":::::Loop START:::::");
}

void loop() {
  
  if(Debug.available() > 0) {
    int inputValue = Debug.read();

    if ( inputValue == 102 ) {  // f
      getFirmwareVersion();

    } else if ( inputValue == 111 ) {  // o
      testOpen();
    } else if ( inputValue ==  99 ) {  // c
      testClose();
    } else if ( inputValue == 119 ) {  // w
      tWri1("ABC");
    } else if ( inputValue == 100 ) {  // d
      dir();
    } else if ( inputValue == 122 ) {  // z
      VNC1L.write(13);
      delay(100);
      retOut();
    }
  }

}

///////////////////////////////////////////////////////////////////



bool checkOnline() {
  bool flg = false;
  String strLog;
  
  while (VNC1L.available() > 0 ) {
    strLog = VNC1L.readString();
    Debug.println(strLog);
    if ( strLog.indexOf("On-Line:") > 0 ) {
      flg = true;
    }
  }

  return flg;
}


bool returnCheck(int checkType) {
  bool flg = false;
  char chkStr[10];
  
  switch(checkType) {
    case 0:
      strcpy(chkStr, "D:\\>");
      break;
    case 1:
      strcpy(chkStr, ">");
      break;
    case 2:
      strcpy(chkStr, "On-Line:");
      break;
  }

  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil(0x0d, buff, VNC1L.available());
    Debug.println(buff);
    if ( strcmp(buff, chkStr) == 0 ) {
      flg = true;
    }
    clearBuffer();
  }

  return flg;
}



bool returnCheckECS() {
  bool flg = false;

  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil(0x0d, buff, VNC1L.available());
    Debug.println(buff);
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
//    Debug.println(buff);
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

void retOut() {
  while (VNC1L.available() > 0 ) {
    VNC1L.readBytesUntil(0x0d, buff, VNC1L.available());
    if ( strcmp(buff, "D:\\>") == 0 ) {
      // Nothing
    } else if ( strcmp(buff, ">") == 0 ) {
      // Nothing
    } else {
      Debug.println(buff);
    }
    clearBuffer();
  }
}

void getFirmwareVersion() {
//  VNC1L.write("FWV"); // ECS command
  VNC1L.write(0x13); // SCS command
  VNC1L.write(13);
  delay(100);
  retOut();
}

void dir() {
//  VNC1L.write("DIR"); // ECS command
  VNC1L.write(0x01); // SCS command
  VNC1L.write(13);
  delay(100);
  retOut();
}

void tWri1(char* data) {
  Debug.println(":::::testWrite1 START:::::");

  int len = strlen(data);

  Debug.print(":::::[");
  Debug.print(data);
  Debug.print("]");
  Debug.print(len);
  Debug.print(":::::");
  Debug.println();

  VNC1L.print("WRF ");
  VNC1L.print(len);
  VNC1L.write(13);
  VNC1L.print(data);

  Debug.println(":::::testWrite1 DONE:::::");
}

void testWrite() {
  Debug.println(":::::testWrite START:::::");

  // write to file
  VNC1L.print("WRF 3");
  VNC1L.write(13);
  VNC1L.print("ABC");

  Debug.println(":::::testWrite DONE:::::");

  return;
}

void testClose() {
  Debug.println(":::::testClose START:::::");

  // close file
  VNC1L.print("CLF TEST.TXT");
  VNC1L.write(13);

  Debug.println(":::::testClose DONE:::::");

  return;
}

void testOpen() {
  Debug.println(":::::testOpen START:::::");

  // file open
  VNC1L.print("OPW TEST.TXT");
  VNC1L.write(13);

  Debug.println(":::::testOpen DONE:::::");

  return;
}

void testDir() {
  Debug.println(":::::testDir START:::::");

  VNC1L.print("DIR");
  VNC1L.write(13);

  Debug.println(":::::testDir DONE:::::");

  return;
}

