#ifndef __TOOLS_SOFTUART_H
#define __TOOLS_SOFTUART_H
#include "RingBuffer.hpp"

class SoftUart
{
	public:
		typedef enum {
    	    	SOFT_UART_4800 = 0,
				SOFT_UART_9600,
				SOFT_UART_19200,
				SOFT_UART_57600,
				SOFT_UART_115200
    	} SoftUartSpeed_t;
    	
    	typedef enum {
    	    	SOFT_UART_PARITY_ON = 0,
				SOFT_UART_PARITY_OFF,
    	} SoftUartParity_t;

		SoftUart(
		         bool (*inputRxBitReadFunc)(void),
		         void (*inputRxPinInterruptSetFunc)(void),
		         void (*timerRxInitFunc)(uint16_t timeout),
		         void (*timerRxStartFunc)(void),
		         void (*stopRxInterruptionsFunc)(void),
		         void (*outputTxBitSetFunc)(bool),
		         void (*timerTxInitFunc)(uint16_t timeout),
		         void (*timerTxStartFunc)(void),
		         void (*timerTxStopFunc)(void),
		         void (*timerRxStopFunc)(void)
		        );
		uint8_t uart_pop_data(uint8_t* data, uint16_t& length, uint16_t maxSize);
		void uart_init(SoftUartSpeed_t speed, SoftUartParity_t parity);
		uint8_t uart_send(uint8_t dataByte);
        uint8_t uart_receive(uint8_t* p_receivedByte);
        uint8_t uart_get_RX_state(void);
        uint8_t uart_get_TX_state(void);

		void start_rx_uart(void);
		void stop_rx_uart(void);
		
		void rx_start_event_pin_handler(void);
		void rx_bit_timer_handler(bool pinValue);
		void tx_bit_timer_handler(void);
		
	private:
		uint8_t receivedDataByte = 0;
		uint8_t receivedDataByteSwapped = 0;
		RingBuffer dataBuffer;
		bool receiveDataByteStatus = false;
		bool RxStopBitsError = false;
		bool RxParityCheckError = false;
		uint8_t receivingStatus = 0;

        uint16_t inputParityValue = 0;
        uint16_t calculatedParityValue = 0;
		
		uint16_t receiveDataByteMask = 0;
		uint16_t receiveDataBufer = 0;
		
		uint16_t sendDataByteMask = 0;
		uint16_t sendDataBufer = 0;
		uint8_t sendingStatus = 0;
		
		SoftUartParity_t parity = SOFT_UART_PARITY_OFF;
		SoftUartSpeed_t speed = SOFT_UART_9600;
		
		uint16_t initialRxTimeout;
		uint16_t RxTxTimeout;

		bool (*inputRxBitRead)(void) = NULL;
		void (*inputRxPinInterruptON)(void) = NULL;
        void (*inputRxPinInterruptOFF)(void) = NULL;
		void (*timerRxInit)(uint16_t timeout) = NULL;
		void (*timerRxStart)(void) = NULL;
        void (*timerRxStop)(void) = NULL;
		
		void startSending(void);
		void (*outputTxBitSet)(bool) = NULL;
		void (*timerTxInit)(uint16_t timeout) = NULL;	
		void (*timerTxStart)(void) = NULL;
		void (*timerTxStop)(void) = NULL;
};


#endif

