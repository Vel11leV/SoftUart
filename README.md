# Software UART 
The protocol is implemented for using on different platforms and controllers. 
It has a set of interfaces that should be implemented dependin on the platform:
```
bool (*inputRxBitRead)(void) = NULL;
void (*inputRxPinInterruptON)(void) = NULL;
void (*inputRxPinInterruptOFF)(void) = NULL;
void (*timerRxInit)(uint16_t timeout) = NULL;
void (*timerRxStart)(void) = NULL;
void (*timerRxStop)(void) = NULL;
void (*outputTxBitSet)(bool) = NULL;
void (*timerTxInit)(uint16_t timeout) = NULL;	
void (*timerTxStart)(void) = NULL;
void (*timerTxStop)(void) = NULL;
```
Also it has 2 callback functions, that shouldbe plased to the propper handlers: 
```
void rx_start_event_pin_handler(void);
void rx_bit_timer_handler(bool pinValue);
void tx_bit_timer_handler(void);
```
Uart uses 2 timers (one for RX one for TX) and two pins RX (input pull-up pin) and TX (output pin).


```
#include "SoftUart.hpp"

SoftUart *softuart1 = new SoftUart(
        inputRxBitReadFunc,
        inputRxPinInterruptSetFunc,
        timerRxInitFunc,
        timerRxStartFunc,
        stopRxInterruptionsFunc,
        outputTxBitSetFunc,
        timerTxInitFunc,
        timerTxStartFunc,
        timerTxStopFunc,
        timerRxStopFunc
);
 SoftUart::SoftUartParity_t parity = SoftUart::SOFT_UART_PARITY_ON;
 SoftUart::SoftUartSpeed_t speed = SoftUart::SOFT_UART_9600;
 softuart1->uart_init(speed, parity);
 softuart1->uart_send(0b11111111);

 uint8_t inputByte;
 uint8_t status;
 status = uart_receive(&inputByte);
```


