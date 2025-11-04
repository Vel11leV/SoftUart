# Software UART 
The protocol is implemented for using on different platforms and controllers. 
It has a set of interfaces that should be implemented dependin on the platform:
'''
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
'''
Also it has 2 callback functions, that shouldbe plased to the propper handlers: 
		void rx_start_event_pin_handler(void);
		void rx_bit_timer_handler(bool pinValue);
		void tx_bit_timer_handler(void);
Uart uses 2 timers (one for RX one for TX) and two pins RX (input pull-up pin) and TX (output pin).

