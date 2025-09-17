#include "ssd1306.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdint>

#ifdef DEBUG
#define LOG(a) std::clog<<a;
#else
#define LOG(a)
#endif

SSD1306::SSD1306(const char* device, int addr) : i2c_fd(-1) {
    i2c_fd = open(device, O_RDWR);
    if (i2c_fd < 0) {
        LOG("ERROR:Can't open I2C device.\n");
        return;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
        LOG("ERROE:Can't set I2C address.\n");
        close(i2c_fd);
        i2c_fd = -1;
        return;
    }
    LOG("OK:Open I2C device and set it's address successfully.\n");

    memset(buffer, 0, sizeof(buffer));
}

SSD1306::~SSD1306() {
    if (i2c_fd >= 0) {
        close(i2c_fd);
        LOG("OK:Close I2C device successfully.\n");
    }
}

bool SSD1306::write_command(uint8_t cmd) {
    uint8_t buf[] = {0x00, cmd}; // 控制字节 0x00 = 命令
    if(write(i2c_fd, buf, 2) == 2){
        LOG("OK:Write command 0x"
                 <<std::hex<< std::setfill('0') << std::setw(2)
                 <<(int)cmd<<" successfully.\n");
        return 1;
    }else{    
        LOG("ERROR:Can't write command 0x"
                 <<std::hex<< std::setfill('0') << std::setw(2)
                 <<(int)cmd<<"\n";)
        return 0;
    }
}

bool SSD1306::write_data(const uint8_t* data, int len) {
    uint8_t* buf = new uint8_t[len + 1];
    buf[0] = 0x40; // 控制字节 0x40 = 数据
    memcpy(buf + 1, data, len);
    bool result = write(i2c_fd, buf, len + 1) == len + 1;
    delete[] buf;
    if(result){
        LOG("OK:Write data successfully.\n");
    }else{
        LOG("ERROR:Can't write data.\n");
    }
    return result;
}

bool SSD1306::init() {
    const uint8_t init_seq[] = {
        0xAE,       // 关闭显示
        0xD5, 0x80, // 设置时钟分频
        0xA8, 0x3F, // 设置多路复用比 (1/64 duty)
        0xD3, 0x00, // 设置显示偏移：0
        0x40,       // 设置显示起始行
        0x8D, 0x14, // 启用电荷泵
        0x20, 0x00, // 设置内存寻址模式：水平模式
        0xA1,       // 设置段重映射 (段0 = 最左边)
        0xC8,       // 设置 COM 输出扫描方向
        0xDA, 0x12, // 设置 COM 引脚硬件配置
        0x81, 0xCF, // 设置对比度
        0xD9, 0xF1, // 设置预充电周期
        0xDB, 0x40, // 设置 VCOMH 取消选择级别
        0xA4,       // 禁用整个显示开启
        0xA6,       // 设置正常显示 (非反色)
        0xAF        // 开启显示
    };

    for (size_t i = 0; i < sizeof(init_seq); i++) {
        if (!write_command(init_seq[i])) {
            LOG("ERROR:Can't init in sending command 0x" 
                      <<std::hex<< std::setfill('0') << std::setw(2)
                      <<(int)init_seq[i] << ".\n");
            return false;
        }
    }

    clear();
    LOG("OK:Init successfully.\n");
    return true;
}

void SSD1306::clear() {
    memset(buffer, 0, sizeof(buffer));
    this->display();
    LOG("OK: Clear the screen seccessfully.\n");
}

void SSD1306::set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    int idx = (y / 8) * 128 + x;
    if (on) {
        buffer[idx] |= (1 << (y % 8));
    } else {
        buffer[idx] &= ~(1 << (y % 8));
    }
    LOG("OK:Set pixel at ("<<std::dec<<x<<","<<y<<") successfully.\n");
}

bool SSD1306::display() {
    // 设置起始页和列地址
    bool ok=1;
    ok&=write_command(0x21); // 设置列地址
    ok&=write_command(0x00); // 起始列
    ok&=write_command(127);  // 结束列

    ok&=write_command(0x22); // 设置页地址
    ok&=write_command(0x00); // 起始页
    ok&=write_command(0x07); // 结束页 (64/8 = 8 页)

    if(write_data(buffer, sizeof(buffer))){
        LOG("OK:The screen has been displayed.\n");
        ok=1;
    }else{
        LOG("ERROR:Can't display the screen.\n");
        ok=0;
    }
    return ok;
}
