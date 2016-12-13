#include <SPI.h>
#include <avr/pgmspace.h>

////////////////////// Registers //////////////////////
#define REG_Product_ID                           0x00
#define REG_Revision_ID                          0x01
#define REG_Motion                               0x02
#define REG_Delta_X_L                            0x03
#define REG_Delta_X_H                            0x04
#define REG_Delta_Y_L                            0x05
#define REG_Delta_Y_H                            0x06
#define REG_SQUAL                                0x07
#define REG_Pixel_Sum                            0x08
#define REG_Maximum_Pixel                        0x09
#define REG_Minimum_Pixel                        0x0a
#define REG_Shutter_Lower                        0x0b
#define REG_Shutter_Upper                        0x0c
#define REG_Frame_Period_Lower                   0x0d
#define REG_Frame_Period_Upper                   0x0e
#define REG_Configuration_I                      0x0f
#define REG_Configuration_II                     0x0
#define REG_Frame_Capture                        0x2
#define REG_SROM_Enable                          0x3
#define REG_Run_Downshift                        0x4
#define REG_Rest1_Rate                           0x5
#define REG_Rest1_Downshift                      0x6
#define REG_Rest2_Rate                           0x7
#define REG_Rest2_Downshift                      0x8
#define REG_Rest3_Rate                           0x9
#define REG_Frame_Period_Max_Bound_Lower         0xa
#define REG_Frame_Period_Max_Bound_Upper         0xb
#define REG_Frame_Period_Min_Bound_Lower         0xc
#define REG_Frame_Period_Min_Bound_Upper         0xd
#define REG_Shutter_Max_Bound_Lower              0xe
#define REG_Shutter_Max_Bound_Upper              0xf
#define REG_LASER_CTRL0                          0x20
#define REG_Observation                          0x24
#define REG_Data_Out_Lower                       0x25
#define REG_Data_Out_Upper                       0x26
#define REG_SROM_ID                              0x2a
#define REG_Lift_Detection_Thr                   0x2e
#define REG_Configuration_V                      0x2f
#define REG_Configuration_IV                     0x39
#define REG_Power_Up_Reset                       0x3a
#define REG_Shutdown                             0x3b
#define REG_Inverse_Product_ID                   0x3f
#define REG_Motion_Burst                         0x50
#define REG_SROM_Load_Burst                      0x62
#define REG_Pixel_Burst                          0x64

byte initComplete = 0;
byte testctr = 0;
unsigned long currTime;
unsigned long timer;

byte buffer1 [14];

int16_t * xburst = (int16_t *) &buffer1[2];
int16_t * yburst = (int16_t *) &buffer1[4];

const int sensors = 3; //number of sensors
const int ncs1 = 1;
const int ncs2 = 2;
const int ncs3 = 3;

extern const unsigned short firmware_length;
extern const unsigned char firmware_data[];

long lasttime;
////////////////////// Setup //////////////////////
void setup() {

  pinMode(13, OUTPUT);  // Test LED declaration

  analogWriteResolution(12);

  Serial.begin(9600);


  pinMode(ncs1, OUTPUT);
  pinMode(ncs2, OUTPUT);
  pinMode(ncs3, OUTPUT);

  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3)); //  differente way to express it in teensy and arduino: SPI.setDataMode(SPI_MODE3); SPI.setBitOrder(MSBFIRST);SPI.setClockDivider(8);
  SPI.setSCK(14);

  performStartup(sensors);
//  dispRegisters(sensors);
  delay(100);
  initComplete = 9;
  lasttime = micros();

}


////////////////////// FUNCTIONS //////////////////////
void adns_com_begin(int ncs) {
  switch (ncs) {
    case 1:
      digitalWrite(ncs1, LOW);
      break;
    case 2:
      digitalWrite(ncs2, LOW);
      break;
    case 3:
      digitalWrite(ncs3, LOW);
      break;
  }
}

void adns_com_end(int ncs) {
  switch (ncs) {
    case 1:
      digitalWrite(ncs1, HIGH);
      break;
    case 2:
      digitalWrite(ncs2, HIGH);
      break;
    case 3:
      digitalWrite(ncs3, HIGH);
      break;
  }
}

byte adns_read_reg(int ncs, byte reg_addr){
  adns_com_begin(ncs);
  
  // send adress of the register, with MSBit = 0 to indicate it's a read
  SPI.transfer(reg_addr & 0x7f);
  delayMicroseconds(100); // tSRAD
  // read data
  byte data = SPI.transfer(0);
  
  delayMicroseconds(1); // tSCLK-NCS for read operation is 120ns
  adns_com_end(ncs);
  delayMicroseconds(19); //  tSRW/tSRR (=20us) minus tSCLK-NCS
  return data;
}

void adns_write_reg(int ncs, byte reg_addr, byte data) {
  adns_com_begin(ncs);

  //send adress of the register, with MSBit = 1 to indicate it's a write
  SPI.transfer(reg_addr | 0x80 );
  //sent data
  SPI.transfer(data);

  delayMicroseconds(20); // tSCLK-NCS for write operation
  adns_com_end(ncs);
  delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound
}

//DO THAT FOR ONE SENSOR
void adns_upload_firmware(int ncs) {
  // send the firmware to the chip, cf p.18 of the datasheet
  Serial.println("Uploading firmware...");

  // set the configuration_IV register in 3k firmware mode
  adns_write_reg(ncs, REG_Configuration_IV, 0x02); // bit 1 = 1 for 3k mode, other bits are reserved

  // write 0xd in SROM_enable reg for initializing
  adns_write_reg(ncs, REG_SROM_Enable, 0xd);

  // wait for more than one frame period
  delay(10); // assume that the frame rate is as low as 100fps... even if it should never be that low

  // write 0x8 to SROM_enable to start SROM download
  adns_write_reg(ncs, REG_SROM_Enable, 0x8);

  // write the SROM file (=firmware data)
  adns_com_begin(ncs);
  SPI.transfer(REG_SROM_Load_Burst | 0x80); // write burst destination adress
  delayMicroseconds(15);

  // send all bytes of the firmware
  unsigned char c;
  for (int i = 0; i < firmware_length; i++) {
    c = (unsigned char)pgm_read_byte(firmware_data + i);
    SPI.transfer(c);
    delayMicroseconds(15);
  }
  adns_com_end(ncs);
}


//DO THAT FOR ALL THE SENSORS
void performStartup(int sensors) {

  //do that to each sensor connected. We have "s" sensors
  for (int s = 1; s <= sensors; s++) 
  {
    adns_com_begin(s); // ensure that the Serial port is reset
    adns_com_end(s); // ensure that the Serial port is reset
    adns_write_reg(s, REG_Power_Up_Reset, 0x5a); // force reset
    adns_write_reg(s, REG_Maximum_Pixel, 0xFF);  // to have the highest movement range
    delay(50); // wait for it to reboot
    // read registers 0x02 to 0x06 (and discard the data)
    adns_read_reg(s, REG_Motion);
    adns_read_reg(s, REG_Delta_X_L);
    adns_read_reg(s, REG_Delta_X_H);
    adns_read_reg(s, REG_Delta_Y_L);
    adns_read_reg(s, REG_Delta_Y_H);
    // upload the firmware
    adns_upload_firmware(s);
    delay(10);
    adns_write_reg(s, REG_Configuration_I, 0x29); // 8200 cpi (counts per inch; cf p.29 doc)
    delay(10);

    //enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
    // reading the actual value of the register is important because the real
    // default value is different from what is said in the datasheet, and if you
    // change the reserved bytes (like by writing 0x00...) it would not work.
    byte laser_ctrl0 = adns_read_reg(s, REG_LASER_CTRL0);
    adns_write_reg(s, REG_LASER_CTRL0, laser_ctrl0 & 0xf0 );

    delay(1);

  }
  
  Serial.println("Optical Chip Initialized");

}

//DO THAT FOR ONE SENSOR
void UpdatePointer(int ncs) {
  if (initComplete == 9) {

    //    //burstMode
    adns_com_begin(ncs);
    SPI.transfer(REG_Motion_Burst & 0x7f); //command line with adress
    //waite one frame
    delayMicroseconds(100); // tSRAD, waite one frame, if period 12000 frames/second, then it is 83 microseconds
    //     byte buffer1[14];  //32 bits of data
    uint8_t size1 = 14;   //number of bytes
    SPI.transfer(buffer1, size1);
    adns_com_end(ncs);
    delayMicroseconds(1); //  tBEXIT is 500ns, to keep NCS HIGH
    adns_write_reg(ncs, REG_Motion, 0); //write any value tu Motion register


    switch (ncs) {
      case 1:
        Serial.print(String(float(*xburst) / 8200 * 2.54 *1000) + "\t" + String(float(*yburst) / 8200 * 2.54*1000 )+"\t");
        break;
      case 2:
        Serial.print(String(float(*xburst) / 8200 * 2.54 *1000) + "\t" + String(float(*yburst) / 8200 * 2.54 *1000)+"\t");
        break;
      case 3:
        Serial.println(String(float(*xburst) / 8200 * 2.54 *1000) + "\t" + String(float(*yburst) / 8200 * 2.54 *1000));
        Serial.send_now();
        break;

    }
  }
}

  void dispRegisters(int sensors) {
    byte oreg[7] = {
      0x00, 0x3F, 0x2A, 0x02, 0x0F
    };
    char* oregname[] = {
      "Product_ID", "Inverse_Product_ID", "SROM_Version", "Motion", "Configuration_I"
    };
    byte regres;

    //do that to each sensor connected. We have "s" sensors
    for (int s = 1; s <= sensors; s++) {
      adns_com_begin(s);
      int rctr = 0;
      for (rctr = 0; rctr < 5; rctr++) {
        SPI.transfer(oreg[rctr]);
        delay(1);
        Serial.println("---");
        Serial.println(oregname[rctr]);
        Serial.println(oreg[rctr], HEX);
        regres = SPI.transfer(0);
        Serial.println(regres, BIN);
        Serial.println(regres, HEX);
        delay(1);
      }
      adns_com_end(s);
    }
  }

  ////////////////////// LOOP //////////////////////
  void loop() {

    //to check the Teensy frequency with the LED
    digitalWrite(13, HIGH);

    //LaserPosition
    
    //do that to each sensor connected.
    for (int s = 1; s <= sensors; s++)
    {
    UpdatePointer(s);
    }
     while ((micros()-lasttime) <= 2000) {
      digitalWrite(13,LOW);
      }
      lasttime = micros();
    

  }

