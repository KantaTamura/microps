#include <unistd.h>
#include <signal.h>

#include "net.h"
#include "util.h"
#include "dummy.h"
#include "test.h"

static volatile sig_atomic_t terminate = 0;

static void on_signal(int signal) {
    (void)signal;
    terminate = 1;
}

int main() {
    net_device *dev;

    signal(SIGINT, on_signal);
    // initial network device
    if (net_init() == -1) {
        errorf("net_init() failure");
        return -1;
    }
    // register network device
    if ((dev = dummy_init()) == NULL) {
        errorf("dummy_init() failure");
        return -1;
    }
    // open all network devices
    if (net_run() == -1) {
        errorf("net_run() failure");
        return -1;
    }
    // wait for termination (triggered by signal Ctrl+C)
    while (!terminate) {
        if (net_device_output(dev, 0x0800, test_data, sizeof(test_data), NULL) == -1) {
            errorf("net_device_output() failure");
            return -1;
        }
        sleep(1);
    }
    // close all network devices
    net_shutdown();
    return 0;
}
