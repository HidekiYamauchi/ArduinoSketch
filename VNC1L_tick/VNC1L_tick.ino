
// VNC1L Test tick

#define VNC1L_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 9600

#define Debug Serial
#define VNC1L Serial1

char buff[65];
char response[64];
int cnti;

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

  tick1();

  
  if(Debug.available() > 0) {
    int inputValue = Debug.read();

    if ( inputValue == 102 ) {  // f
      getFirmwareVersion();

    } else if ( inputValue == 100 ) {  // d
      dir();
    } else if ( inputValue == 101 ) {  // e
      dir2();
    }
  }

}

///////////////////////////////////////////////////////////////////


void usbReceive(char* message) {
  Debug.println(message);

}

void tick1() {
  if ( VNC1L.available() > 0 ) {
    response[cnti] = VNC1L.read();
    if ( cnti > 30 || response[cnti] == 13 ) {
      response[cnti+1] = 0;
      char msg[cnti+1];
      strcpy(msg, response);
      cnti = 0;
      usbReceive(msg);

    } else {
      cnti++;
    }
  }
}

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
    Debug.println(buff);
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
      Debug.println("OK");
    } else if ( strcmp(buff, ">") == 0 ) {
      Debug.println("OK");
    } else {
      Debug.println(buff);
    }
    clearBuffer();
  }
}

void getFirmwareVersion() {

  Debug.println();
  Debug.println("[[[ getFirmwareVersion() ]]]");

//  VNC1L.write("FWV"); // ECS command
  VNC1L.write(0x13); // SCS command
  VNC1L.write(13);
//  delay(100);
//  retOut();
}

void dir() {

  Debug.println();
  Debug.println("[[[ dir() ]]]");

//  VNC1L.write("DIR"); // ECS command
  VNC1L.write(0x01); // SCS command
  VNC1L.write(13);
//  delay(100);
//  retOut();
}

void dir2() {
  Debug.println();
  Debug.println("[[[ dir2() ]]]");

  VNC1L.write(0x01); // SCS command
  VNC1L.write(0x20);
  VNC1L.print("FABOTEST.TXT");
  VNC1L.write(13);
//  delay(100);
//  retOut();
}


