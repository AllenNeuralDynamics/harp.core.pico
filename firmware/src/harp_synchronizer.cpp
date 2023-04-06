#include <harp_synchronizer.h>


HarpSynchronizer::HarpSynchronizer(uart_inst_t* uart_id, uint8_t uart_rx_pin)
:uart_id_{uart_id}, packet_index_{0}, sync_data_{0, 0, 0, 0},
 state_{RECEIVE_HEADER_0}, new_timestamp_{false}
{
    // Create a pointer to the first (and one-and-only) instance created.
    if (self == nullptr)
        self = this;
    // Setup uart.
    uart_init(uart_id_, HARP_SYNC_BAUDRATE);
    // Disable hardware flow control.
    uart_set_hw_flow(uart_id_, false, false);
    // Set data format
    uart_set_format(uart_id_, HARP_SYNC_DATA_BITS, HARP_SYNC_STOP_BITS,
                    HARP_SYNC_PARITY);
    // Setup the RX pin by using the function select on the GPIO
    gpio_set_function(uart_rx_pin, GPIO_FUNC_UART);
    // Turn off internal FIFO. (FIFO set to size 1.)
    uart_set_fifo_enabled(uart_id, false);
    // Select correct interrupt handler for the UART we are using.
    int uart_irq = (uart_id == uart0) ? UART0_IRQ : UART1_IRQ;
    // Attach the static callback function.
    irq_set_exclusive_handler(uart_irq, uart_rx_callback);
    irq_set_enabled(uart_irq, true);
    // Enable RX-based interrupts.
    uart_set_irq_enables(uart_id_, true, false);
}

HarpSynchronizer::~HarpSynchronizer(){self = nullptr;}

HarpSynchronizer& HarpSynchronizer::init(uart_inst_t* uart_id,
                                         uint8_t uart_rx_pin)
{
    static HarpSynchronizer synchronizer(uart_id, uart_rx_pin);
    return synchronizer;
}

void HarpSynchronizer::uart_rx_callback()
{
    SyncState next_state_{self->state_}; // Init next state at curr state value.
    uint8_t new_byte;
    // This State machine "ticks" every time we receive at least one new byte.
    // Handle next-state/output logic.
    while (uart_is_readable(self->uart_id_) and not self->new_timestamp_)
    {
        new_byte = uart_getc(self->uart_id_);
        #ifdef DEBUG
        printf("state: %d | byte: 0x%x\r\n", self->state_, new_byte);
        #endif
        switch (self->state_)
        {
            case RECEIVE_HEADER_0:
                if (new_byte == 0xAA)
                    next_state_ = RECEIVE_HEADER_1;
                break;
            case RECEIVE_HEADER_1:
                if (new_byte == 0xAF)
                    next_state_ = RECEIVE_TIMESTAMP;
                else
                    next_state_ = RECEIVE_HEADER_0;
                break;
            case RECEIVE_TIMESTAMP:
                self->sync_data_[self->packet_index_++] = new_byte;
                if (self->packet_index_ == 4)
                {
                    next_state_ = RECEIVE_HEADER_0;
                    self->packet_index_ = 0;
                    self->new_timestamp_ = true;
                }
                else
                    next_state_ = RECEIVE_TIMESTAMP;
                break;
        }
        self->state_ = next_state_;
    }
    if (not self->new_timestamp_)
        return;
    // Cleanup first, in case changing time registers has adverse behavior.
    self->new_timestamp_ = false;
    // Apply new timestamp data.
    // Interpret 4-byte sequence as a little-endian uint32_t.
    //uint32_t sec = *((uint16_t*)(self->sync_data_)); // works.
    //uint32_t sec = *((uint32_t*)(self->sync_data_)); // This breaks.
    // TODO: why can't we just reinterpret cast? Something about being volatile?
    uint32_t sec;
    memcpy(&sec, (const void*)&(self->sync_data_[0]), 4);
    uint64_t curr_us = uint64_t(sec) * 1000000 - HARP_SYNC_OFFSET_US;
    #ifdef DEBUG
    printf("time is: %llu [us]\r\n", curr_us);
    #endif
    timer_hw->timelw = (uint32_t)curr_us;
    timer_hw->timehw = (uint32_t)(curr_us >> 32);
}

