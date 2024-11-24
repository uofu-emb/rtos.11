/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "btstack.h"
#include "btstack_event.h"
#include "pico/cyw43_arch.h"
#include "picow_bt_example_common.h"

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define BTSTACK_TASK_PRIORITY				( tskIDLE_PRIORITY + 2UL )

int btstack_main(int argc, const char * argv[]);
static btstack_packet_callback_registration_t hci_event_callback_registration;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch(hci_event_packet_get_type(packet)){
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
            break;
        case BTSTACK_EVENT_NR_CONNECTIONS_CHANGED:
            uint8_t num;
            if (num = btstack_event_nr_connections_changed_get_number_connections(packet)) {
                printf("CONNECTED %d\n", num);
            } else {
                printf("NO CONNECTED\n");
            }
            break;
        default:
            break;
    }
}

void main_task(__unused void *params)
{
    // initialize CYW43 driver architecture
    // (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
    } else {
        // inform about BTstack state
        hci_event_callback_registration.callback = &packet_handler;
        hci_add_event_handler(&hci_event_callback_registration);
        btstack_main(0, NULL);
    }

    while(true) {
        vTaskDelay(1000);
    }
}

int main()
{
    stdio_init_all();
    TaskHandle_t task;
    xTaskCreate(main_task, "TestMainThread", 1024, NULL, BTSTACK_TASK_PRIORITY, &task);
    vTaskStartScheduler();
    return 0;
}
