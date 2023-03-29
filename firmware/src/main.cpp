#include <harp_core.h>
#include <harp_synchronizer.h>
#include <harp_message.h>
#include <pico/stdlib.h>
#include <cstdio>
#include <cstring>


// Create device name array.
const uint16_t who_am_i = 1216;
const uint8_t hw_version_major = 1;
const uint8_t hw_version_minor = 0;
const uint8_t assembly_version = 2;
const uint8_t harp_version_major = 2;
const uint8_t harp_version_minor = 0;
const uint8_t fw_version_major = 3;
const uint8_t fw_version_minor = 0;

// Create Core and synchronizer.
HarpCore& core = HarpCore::init(who_am_i, hw_version_major, hw_version_minor,
                                assembly_version,
                                harp_version_major, harp_version_minor,
                                fw_version_major, fw_version_minor,
                                "Pico Harp");
HarpSynchronizer& sync = HarpSynchronizer::init(uart0, 1);

// Core0 main.
int main()
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

#ifdef DEBUG
    stdio_uart_init_full(uart0, 115200, 0, -1); // use uart0 tx only.
    printf("Hello, from a Pi Pico!\r\n");
#endif
    while(true)
    {
        core.run(); // call this in a loop.
        // run() will:
        // 1. parse new messages into a buffer.
        // 2. Handle register reads and write messages to core registers.
        // 3. Reply with the appropriate harp reply for reads/writes to core
        //    registers.
        // Optional. Handle msgs outside the range of the core registers here.
        if (not core.new_msg())
            continue;
        // Handle the message here.
        core.clear_msg();
    }
    return 0;
}
