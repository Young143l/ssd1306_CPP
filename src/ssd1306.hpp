#ifndef SSD1306_H
#define SSD1306_H

#include <cstdint>

class SSD1306 {
private:
    int i2c_fd;
    uint8_t buffer[128 * 64 / 8]; // 128x64

    bool write_command(uint8_t cmd);
    bool write_data(const uint8_t* data, int len);

public:
    SSD1306(const char* i2c_device = "/dev/i2c-1", int addr = 0x3C);
    ~SSD1306();

    bool init();
    void clear();
    void set_pixel(int x, int y, bool on = true);
    bool display(); 

};

#endif
