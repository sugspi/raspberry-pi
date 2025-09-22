#ifndef SPI_ADXL367_H_
#define SPI_ADXL367_H_

#include <stdint.h>
#define SPI_PORT spi1
#define PIN_SCK 10
#define PIN_MOSI 11
#define PIN_MISO 12
#define PIN_CS 13

// ADXL367 レジスタアドレス定義
#define REG_DEVID_AD    0x00
#define REG_DEVID_MST   0x01
#define REG_PARTID      0x02
#define REG_REVID       0x03
#define REG_STATUS      0x0B
#define REG_XDATA_H     0x0E
#define REG_XDATA_L     0x0F
#define REG_YDATA_H     0x10
#define REG_YDATA_L     0x11
#define REG_ZDATA_H     0x12
#define REG_ZDATA_L     0x13
#define REG_SOFT_RESET  0x1F
#define REG_INTMAP2     0x2B
#define REG_FILTER_CTL  0x2C
#define REG_POWER_CTL   0x2D

// ADXL367 コマンド定義
#define CMD_WR          0x0A
#define CMD_RD          0x0B
#define CMD_RD_FIFO     0x0D

void adxl367_init();;
void adxl367_read_xyz(int16_t* x, int16_t* y, int16_t* z);
void adxl367_write_register(uint8_t reg, uint8_t value);
uint8_t adxl367_read_register(uint8_t reg);

#endif  // SPI_ADXL367_H_
