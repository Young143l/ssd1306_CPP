#include "ssd1306.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <iostream>

SSD1306::SSD1306(const char* device, int addr) : i2c_fd(-1) {
    i2c_fd = open(device, O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "Can't open I2C device\n";
        return;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
        std::cerr << "Can't set I2C address\n";
        close(i2c_fd);
        i2c_fd = -1;
        return;
    }

    memset(buffer, 0, sizeof(buffer));
}

SSD1306::~SSD1306() {
    if (i2c_fd >= 0) {
        close(i2c_fd);
    }
}

bool SSD1306::writeCommand(unsigned char cmd) {
    unsigned char buf[] = {0x00, cmd}; // 控制字节 0x00 = 命令
    return write(i2c_fd, buf, 2) == 2;
}

bool SSD1306::writeData(const unsigned char* data, int len) {
    unsigned char* buf = new unsigned char[len + 1];
    buf[0] = 0x40; // 控制字节 0x40 = 数据
    memcpy(buf + 1, data, len);
    bool result = write(i2c_fd, buf, len + 1) == len + 1;
    delete[] buf;
    return result;
}

bool SSD1306::init() {
    const unsigned char init_seq[] = {
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
        if (!writeCommand(init_seq[i])) {
            std::cerr << "Init error: Command " << std::hex << init_seq[i] << "\n";
            return false;
        }
    }

    clear();
    display();
    return true;
}

void SSD1306::clear() {
    memset(buffer, 0, sizeof(buffer));
}

void SSD1306::setPixel(int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    int idx = (y / 8) * 128 + x;
    if (on) {
        buffer[idx] |= (1 << (y % 8));
    } else {
        buffer[idx] &= ~(1 << (y % 8));
    }
}

void SSD1306::display() {
    // 设置起始页和列地址
    writeCommand(0x21); // 设置列地址
    writeCommand(0x00); // 起始列
    writeCommand(127);  // 结束列

    writeCommand(0x22); // 设置页地址
    writeCommand(0x00); // 起始页
    writeCommand(0x07); // 结束页 (64/8 = 8 页)

    writeData(buffer, sizeof(buffer));
}
