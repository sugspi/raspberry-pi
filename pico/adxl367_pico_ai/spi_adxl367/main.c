#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "spi_adxl367.h"


uint8_t adxl367_read_device_id() {

    uint8_t devid_ad = adxl367_read_register(REG_DEVID_AD);
    uint8_t devid_mst = adxl367_read_register(REG_DEVID_MST);
    uint8_t partid    = adxl367_read_register(REG_PARTID);
    uint8_t revid     = adxl367_read_register(REG_REVID);

    printf("DEVID_AD:  0x%02X\n", devid_ad);
    printf("DEVID_MST: 0x%02X\n", devid_mst);
    printf("PARTID:    0x%02X\n", partid);
    printf("REVID:     0x%02X\n", revid);
    printf("----------------------\n");

}

int main() {
    stdio_init_all();
    adxl367_init();

    sleep_ms(8000);  // シリアルモニターの準備時間
    adxl367_read_device_id();

    while (true) {
        int16_t x, y, z;
        adxl367_read_xyz(&x, &y, &z);
        printf("X: %d, Y: %d, Z: %d\n", x, y, z);
        sleep_ms(500);
    }
}
