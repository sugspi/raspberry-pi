#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdint.h>
#include <stdio.h>

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

void cs_select() {
    gpio_put(PIN_CS, 0);
}

void cs_deselect() {
    gpio_put(PIN_CS, 1);
}

void adxl367_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx[] = {CMD_WR, reg, value};
    cs_select();
    spi_write_blocking(SPI_PORT, tx, 3);
    cs_deselect();
}

uint8_t adxl367_read_register(uint8_t reg) {
    uint8_t tx[] = {CMD_RD, reg};
    uint8_t rx[1];
    cs_select();
    spi_write_blocking(SPI_PORT, tx, 2);
    spi_read_blocking(SPI_PORT, 0, rx, 1);
    cs_deselect();
    return rx[0];
}

void adxl367_init() {
    spi_init(SPI_PORT, 1000000);  // 1MHz
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    cs_deselect();

    adxl367_write_register(REG_SOFT_RESET, 0x52);  // ADXL367 ソフトリセットの実行 + 10ms 待機
    sleep_ms(10);
    
    adxl367_write_register(REG_FILTER_CTL, 0x20);  // ADXL367 ODR[2:0] 設定: 12.5Hz=000b 
    adxl367_write_register(REG_POWER_CTL, 0x02);  // DXL367 Measurement Mode に設定 + 100ms 待機
    sleep_ms(100);
}

void adxl367_read_xyz(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t xh = adxl367_read_register(REG_XDATA_H);
    uint8_t xl = adxl367_read_register(REG_XDATA_L);
    uint8_t yh = adxl367_read_register(REG_YDATA_H);
    uint8_t yl = adxl367_read_register(REG_YDATA_L);
    uint8_t zh = adxl367_read_register(REG_ZDATA_H);
    uint8_t zl = adxl367_read_register(REG_ZDATA_L);

    *x = (int16_t)((xh << 8) | xl);
    *y = (int16_t)((yh << 8) | yl);
    *z = (int16_t)((zh << 8) | zl);
}
