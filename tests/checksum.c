#include "util.h"

int main() {
    uint16_t data[] = { 0x0000, 0x0001, 0x0002, 0x0003 };
    uint16_t checksum = checksum16(data, countof(data), 0);
    debugf("checksum = 0x%04x", checksum);
    return 0;
}
