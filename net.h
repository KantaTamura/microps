#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

// interface name length
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

// interface types
#define NET_DEVICE_TYPE_DUMMY 0x0000
#define NET_DEVICE_TYPE_LOOPBACK 0x0001
#define NET_DEVICE_TYPE_ETHERNET 0x0002

// interface flags
#define NET_DEVICE_FLAG_UP 0x0001
#define NET_DEVICE_FLAG_LOOPBACK 0x0010
#define NET_DEVICE_FLAG_BROADCAST 0x0020
#define NET_DEVICE_FLAG_P2P 0x0040
#define NET_DEVICE_FLAG_NEED_ARP 0x0100

// interface address length
#define NET_DEVICE_ADDR_LEN 16

// check interface state macro
#define NET_DEVICE_IS_UP(x) ((x)->flags & NET_DEVICE_FLAG_UP)
#define NET_DEVICE_STATE(x) (NET_DEVICE_IS_UP(x) ? "up" : "down")

/* NOTE: use same value as the Ethernet types */
#define NET_PROTOCOL_TYPE_IP 0x0800
#define NET_PROTOCOL_TYPE_ARP 0x0806
#define NET_PROTOCOL_TYPE_IPV6 0x86dd

#define NET_IFACE_FAMILY_IP 1
#define NET_IFACE_FAMILY_IPV6 2

#define NET_IFACE(x) ((struct net_iface *)(x))

// interfaces
struct net_device {
    struct net_device *next;  // next device in the list
    struct net_iface
        *ifaces; /* NOTE: if you want to add/delete the entries after net_run(),
                    you need to protect ifaces with a mutex. */
    unsigned int index;   // interface index
    char name[IFNAMSIZ];  // interface name
    uint16_t type;        // interface type => NET_DEVICE_TYPE_XXX
    uint16_t mtu;         // interface MTU (maximum transmission unit)
    uint16_t flags;       // interface flags => NET_DEVICE_FLAG_XXX
    uint16_t hlen;        // header length
    uint16_t alen;        // address length
    // hardware address
    uint8_t addr[NET_DEVICE_ADDR_LEN];
    union {
        uint8_t peer[NET_DEVICE_ADDR_LEN];
        uint8_t broadcast[NET_DEVICE_ADDR_LEN];
    };
    // interface operations => open, close, transmit
    struct net_device_ops *ops;
    void *priv;  // private data
};

// interface operations
// transmit only nessesary for ethernet
// open and close are not called if it unnecessary
struct net_device_ops {
    int (*open)(struct net_device *dev);
    int (*close)(struct net_device *dev);
    int (*transmit)(struct net_device *dev, uint16_t type, const uint8_t *data,
                    size_t len, const void *dst);
};

struct net_iface {
    struct net_iface *next;
    struct net_device *dev; /* back pointer to parent */
    int family;
    /* depends on implementation of protocols. */
};

// prototype declarations
extern struct net_device *net_device_alloc(void);
extern int net_device_register(struct net_device *dev);
extern int net_device_output(struct net_device *dev, uint16_t type,
                             const uint8_t *data, size_t len, const void *dst);

extern int net_protocol_register(uint16_t type,
                                 void (*handler)(const uint8_t *data,
                                                 size_t len,
                                                 struct net_device *dev));
extern int net_device_add_iface(struct net_device *dev,
                                struct net_iface *iface);
extern struct net_iface *net_device_get_iface(struct net_device *dev,
                                              int family);

extern int net_timer_register(struct timeval interval, void (*handler)(void));
extern int net_timer_handler(void);

extern int net_input_handler(uint16_t type, const uint8_t *data, size_t len,
                             struct net_device *dev);
extern int net_softirq_handler(void);

extern int net_event_subscribe(void (*handler)(void *arg), void *arg);
extern int net_event_handler(void);
extern void net_raise_event(void);

extern int net_run(void);
extern void net_shutdown(void);
extern int net_init(void);

#endif
