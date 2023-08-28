#include "dummy.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "util.h"
#include "net.h"

// dummy device MTU
#define DUMMY_MTU UINT16_MAX

// dummy device transmit function (for testing)
// => drop data
static int dummy_transmit(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst) {
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    return 0;
}

// dummy device operations
static struct net_device_ops dummy_ops = {
    .transmit = dummy_transmit,
};

// dummy device initialization
struct net_device *dummy_init(void) {
    net_device *dev;

    // allocate dummy device
    if ((dev = net_device_alloc()) == NULL) {
        errorf("net_device_alloc() failure");
        return NULL;
    }
    // setting dummy device
    dev->type = NET_DEVICE_TYPE_DUMMY;
    dev->mtu = DUMMY_MTU;
    dev->hlen = 0;      // no header
    dev->alen = 0;      // no address
    dev->ops = &dummy_ops;
    // register dummy device
    if (net_device_register(dev) == -1) {
        errorf("net_device_register() failure");
        return NULL;
    }
    debugf("dummy device initialized, dev=%s", dev->name);
    return dev;
}
