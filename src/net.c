#include "net.h"

#include <stdio.h>
#include <stddef.h>

#include "platform.h"
#include "util.h"

/// network device list
net_device *devices;

// allocate a new network device
net_device *net_device_alloc() {
    net_device *dev;
    if ((dev = memory_alloc(sizeof(*dev))) == NULL) {
        errorf("memory_alloc() failure");
        return NULL;
    }
    return dev;
}

// register a network device
// NOTE: this function must not be call after net_run()
int net_device_register(net_device *dev) {
    // It's global variable for device index
    static unsigned int index = 0;

    // set device index and name
    // (index, name) : (0, "net0"), (1, "net1"), ...
    dev->index = index++;
    snprintf(dev->name, sizeof(dev->name), "net%d", dev->index);
    // add list head device
    dev->next = devices;
    devices = dev;

    infof("registered, dev=%s, type=0x%04x", dev->name, dev->type);
    return 0;
}

// device open : set flag "NET_DEVICE_FLAG_UP"
static int net_device_open(net_device *dev) {
    // require device close
    if (NET_DEVICE_IS_UP(dev)) {
        errorf("already opened, dev=%s", dev->name);
        return -1;
    }
    // if device has not open operation, do not execute
    if (dev->ops->open && dev->ops->open(dev) == -1) {
        errorf("failure device open operation, dev=%s", dev->name);
        return -1;
    }
    // set flag "NET_DEVICE_FLAG_UP"
    dev->flags |= NET_DEVICE_FLAG_UP;
    infof("dev=%s, state=%s", dev->name, NET_DEVICE_STATE(dev));
    return 0;
}

// device close : clear flag "NET_DEVICE_FLAG_UP"
static int net_device_close(net_device *dev) {
    // require device open
    if (!NET_DEVICE_IS_UP(dev)) {
        errorf("already closed, dev=%s", dev->name);
        return -1;
    }
    // if device has not close operation, do not execute
    if (dev->ops->close && dev->ops->close(dev) == -1) {
        errorf("failure device close operation, dev=%s", dev->name);
        return -1;
    }
    // clear flag "NET_DEVICE_FLAG_UP"
    dev->flags &= ~NET_DEVICE_FLAG_UP;
    infof("dev=%s, state=%s", dev->name, NET_DEVICE_STATE(dev));
    return 0;
}

// output data from network device
int net_device_output(net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst) {
    // require device open
    if (!NET_DEVICE_IS_UP(dev)) {
        errorf("device is down, dev=%s", dev->name);
        return -1;
    }
    // packet size must be less than MTU
    if (len > dev->mtu) {
        errorf("too large packet, dev=%s, len=%d, mtu=%d", dev->name, len, dev->mtu);
        return -1;
    }
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    // transmit packet
    if (dev->ops->transmit(dev, type, data, len, dst) == -1) {
        errorf("failure device transmit operation, dev=%s, len=%zu", dev->name, len);
        return -1;
    }
    return 0;
}

// passes packets to the protocol stack received by the network device
int net_input_handler(uint16_t type, const uint8_t *data, size_t len, net_device *dev) {
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    return 0;
}

// run all network devices of device list
int net_run() {
    net_device *dev;

    debugf("open all devices...");
    for (dev = devices; dev; dev = dev->next) {
        net_device_open(dev);
    }
    debugf("running...");
    return 0;
}

// shutdown all network devices of device list
void net_shutdown() {
    net_device *dev;

    debugf("close all devices...");
    for (dev = devices; dev; dev = dev->next) {
        net_device_close(dev);
    }
    debugf("shutdown");
}

// initialize network devices
int net_init() {
    infof("initialized");
    return 0;
}
