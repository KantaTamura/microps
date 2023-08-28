#ifndef MICROPS_NET_H
#define MICROPS_NET_H

#include <stddef.h>
#include <stdint.h>

#define NAME_SIZE 16

#define NET_DEVICE_TYPE_DUMMY     0x0000
#define NET_DEVICE_TYPE_LOOPBACK  0x0001
#define NET_DEVICE_TYPE_ETHERNET  0x0002

#define NET_DEVICE_FLAG_UP        0x0001
#define NET_DEVICE_FLAG_LOOPBACK  0x0010
#define NET_DEVICE_FLAG_BROADCAST 0x0020
#define NET_DEVICE_FLAG_P2P       0x0040
#define NET_DEVICE_FLAG_NEED_ARP  0x0100

#define NET_DEVICE_ADDR_LEN 6

typedef struct net_device net_device;
typedef struct net_device_ops net_device_ops;

/// network device structure
struct net_device {
    struct net_device *next;    // next device
    unsigned int index;         // device index
    char name[NAME_SIZE];       // device name e.g. "eth0"
    uint16_t type;  // device type e.g. NET_DEVICE_TYPE_ETHERNET
    uint16_t mtu;   // maximum transmission unit
    uint16_t flags; // device flags e.g. NET_DEVICE_FLAG_UP
    uint16_t hlen;  // header length
    uint16_t alen;  // address length
    uint8_t addr[NET_DEVICE_ADDR_LEN];
    union {
        uint8_t peer[NET_DEVICE_ADDR_LEN];
        uint8_t broadcast[NET_DEVICE_ADDR_LEN];
    };
    net_device_ops *ops;    // device operations
    void *priv;             // private data
};

/// network device operations.
/// open, close is not necessary, but transmit is required
struct net_device_ops {
    int (*open)(net_device *dev);
    int (*close)(net_device *dev);
    // type: packet type e.g. NET_PACKET_TYPE_IP
    // data, size: packet payload data and size
    // dst: destination address
    int (*transmit)(net_device *dev, uint16_t type, const uint8_t *data, size_t size, const void *dst);
};

#endif //MICROPS_NET_H
