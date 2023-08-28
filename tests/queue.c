#include "util.h"

typedef struct data data;
struct data {
    uint16_t x;
};

void add(void *arg, void *data) {
    ((struct data *)data)->x += ((struct data *)arg)->x;
    debugf("new queue data: 0x%04x", ((struct data *)data)->x);
    return;
}

int main() {
    queue_head queue;
    queue_init(&queue);
    debugf("queue push 0x0001 ~ 0x0004");
    queue_push(&queue, &(data){0x0001});
    queue_push(&queue, &(data){0x0002});
    queue_push(&queue, &(data){0x0003});
    queue_push(&queue, &(data){0x0004});
    debugf("queue pop head: 0x%04x", ((data *)queue_pop(&queue))->x);
    debugf("queue pop head: 0x%04x", ((data *)queue_pop(&queue))->x);
    debugf("queue peek head: 0x%04x", ((data *)queue_peek(&queue))->x);
    queue_foreach(&queue, add, &(data){0x0005});
}
