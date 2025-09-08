#ifndef SSD1306_H
#define SSD1306_H

class SSD1306 {
private:
    int i2c_fd;
    unsigned char buffer[128 * 64 / 8]; // 128x64

    bool writeCommand(unsigned char cmd);
    bool writeData(const unsigned char* data, int len);

public:
    SSD1306(const char* i2c_device = "/dev/i2c-1", int addr = 0x3C);
    ~SSD1306();

    bool init();
    void clear();
    void setPixel(int x, int y, bool on = true);
    void display(); 

};

#endif
