#include "mbed.h"
 
#define SHT31_DEFAULT_ADDR          0x45
#define SHT31_MEAS_HIGHREP_STRETCH  0x2C06
#define SHT31_MEAS_MEDREP_STRETCH   0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH   0x2C10
#define SHT31_MEAS_HIGHREP          0x2400
#define SHT31_MEAS_MEDREP           0x240B
#define SHT31_MEAS_LOWREP           0x2416
#define SHT31_READSTATUS            0xF32D
#define SHT31_CLEARSTATUS           0x3041
#define SHT31_SOFTRESET             0x30A2
#define SHT31_HEATEREN              0x306D
#define SHT31_HEATERDIS             0x3066



class Sht31 {
public:
    Sht31(PinName sda, PinName scl) : _i2c(sda, scl) {
    _i2caddr = (0x45 << 1);
    reset();
    readStatus();
    }
    float readTemperature(void) {
        if (! readTempHum()) return NAN;
        return temp;
}
 
float readHumidity(void) {
    if (! readTempHum()) return NAN;
    return humidity;
}
 
private:
    void reset(void){
    writeCommand(SHT31_SOFTRESET);
    wait_ms(10);
}
    
    uint16_t readStatus(void){
        writeCommand(SHT31_READSTATUS);
    char val[1];
    _i2c.read(_i2caddr, val, 1);
    uint16_t stat = val[0];
    stat <<= 8;
    _i2c.read(_i2caddr, val, 1);
    stat |= val[0];
    // printf("0x%X\r\n", stat);
    return stat;
    }
        
    
    void writeCommand(uint16_t cmd){
         char buf[2];
    buf[0] = (cmd >> 8);
    buf[1] = (cmd & 0xFF);
    _i2c.write(_i2caddr, buf, 2);
    }
    
    
    bool readTempHum(void){
        char readbuffer[6];
 
    writeCommand(SHT31_MEAS_HIGHREP);
 
    wait_ms(500);
    _i2c.read(_i2caddr, readbuffer, 6);
    
    uint16_t ST, SRH;
    ST = readbuffer[0];
    ST <<= 8;
    ST |= readbuffer[1];
 
    if (readbuffer[2] != crc8((uint8_t *) readbuffer, 2)) {
        return false;
    }
 
    SRH = readbuffer[3];
    SRH <<= 8;
    SRH |= readbuffer[4];
 
    if (readbuffer[5] != crc8((uint8_t *) readbuffer+3, 2)) {
        return false;
    }
 
    double stemp = ST;
    stemp *= 175;
    stemp /= 0xffff;
    stemp = -45 + stemp;
    temp = stemp;
 
    double shum = SRH;
    shum *= 100;
    shum /= 0xFFFF;
 
    humidity = shum;
 
    return true;
}
    
    uint8_t crc8(const uint8_t *data, int len){
    const uint8_t POLYNOMIAL(0x31);
    uint8_t crc(0xFF);
 
    for ( int j = len; j; --j ) {
        crc ^= *data++;
 
        for ( int i = 8; i; --i ) {
            crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
        }
    }
    return crc;
}
 
    I2C _i2c;
    int _i2caddr;
    float humidity, temp;
};
Sht31 sht_31(I2C_SDA, I2C_SCL);
Serial pc(SERIAL_TX, SERIAL_RX);

int main(){
    while(1){
		float temp = sht_31.readTemperature();
		float humidity = sht_31.readHumidity();
		pc.printf("T%0.1fH%0.1f\n",temp,humidity);
		wait(10);
    }
}