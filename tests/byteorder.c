#include "util.h"

int main() {
    infof("host byteorder and network byteorder conversion (32bit)");
    uint32_t x32 = 0x12345678;
    uint32_t y32 = hton32(x32);
    uint32_t z32 = ntoh32(y32);
    debugf("host byteorder    = 0x%08x to network byteorder = 0x%08x", x32, y32);
    debugf("network byteorder = 0x%08x to host byteorder    = 0x%08x", y32, z32);
    printf("\n");
    infof("host byteorder and network byteorder conversion (16bit)");
    uint16_t x16 = 0x1234;
    uint16_t y16 = hton16(x16);
    uint16_t z16 = ntoh16(y16);
    debugf("host byteorder    = 0x%04x to network byteorder = 0x%04x", x16, y16);
    debugf("network byteorder = 0x%04x to host byteorder    = 0x%04x", y16, z16);
    return 0;
}
