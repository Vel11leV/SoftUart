#include "stdint.h"
#include <cstddef>
#include "SoftUart.hpp"

/*UART packet bits*/
#define SOFT_UART_DATA_BITS_NUMBER         8
#define SOFT_UART_END_BITS_NUMBER          1        
#define SOFT_UART_PARITY_BIT               1 
#define SOFT_UART_TRANSACTION_START        1 
/*UART timing */
#define SOFT_UART_4800_TIMEOUT             198
#define SOFT_UART_9600_TIMEOUT             94
#define SOFT_UART_19200_TIMEOUT            44
#define SOFT_UART_57600_TIMEOUT            9
#define SOFT_UART_115200_TIMEOUT           2
/*UART TX statuses*/
#define SOFT_UART_TX_READY_TO_SEND         0
#define SOFT_UART_TX_IN_PROGRESS           1
#define SOFT_UART_RX_ACTIVE                0
#define SOFT_UART_RX_IN_PROGRESS           1
#define SOFT_UART_RX_INACTIVE              2

/**
* @brief   Sends one byte
* @param   dataByte: one byte for sending.
* @retval  uint8_t:  SOFT_UART_TX_IN_PROGRESS - sending in process, SOFT_UART_TX_READY_TO_SEND - next byte can be sent.
*/
uint8_t SoftUart::uart_send(uint8_t dataByte) {
    if (sendingStatus == SOFT_UART_TX_IN_PROGRESS) {
        return sendingStatus;
    }
    sendingStatus = SOFT_UART_TX_IN_PROGRESS;
    uint8_t inputByte = dataByte;
    sendDataBufer = 0;
    timerTxInit(RxTxTimeout);
    for(int i = 0; i < 8; i++) {
        sendDataBufer <<= 1;
        sendDataBufer |= dataByte & 0x0001;
        dataByte >>= 1;
    }
	if (parity==SOFT_UART_PARITY_OFF) {
	    sendDataBufer <<= SOFT_UART_END_BITS_NUMBER;
	    if (SOFT_UART_END_BITS_NUMBER == 1)
	    {
	        sendDataBufer |= 0x0001;
	    } else if (SOFT_UART_END_BITS_NUMBER == 2) {
	    	sendDataBufer |= 0x0003;
		}
	    sendDataByteMask = 1<<(SOFT_UART_DATA_BITS_NUMBER + SOFT_UART_END_BITS_NUMBER);
	    startSending();
	} else {
		uint16_t parity = 0;
		while (inputByte){
		    parity ^= (inputByte & 0x0001);
		    inputByte >>= 1;
       	}
		sendDataBufer <<= (SOFT_UART_PARITY_BIT + SOFT_UART_END_BITS_NUMBER);
	    if (SOFT_UART_END_BITS_NUMBER == 1)
	    {
	        sendDataBufer |= 0x0001;
	    } else if (SOFT_UART_END_BITS_NUMBER == 2) {
	    	sendDataBufer |= 0x0003;
		}
	    sendDataBufer |= (parity<<SOFT_UART_END_BITS_NUMBER);
	    sendDataByteMask = 1<<( SOFT_UART_DATA_BITS_NUMBER + SOFT_UART_PARITY_BIT + SOFT_UART_END_BITS_NUMBER);
	    startSending();
	}
    return sendingStatus;
}

/**
* @brief   Initiates sending process.
* @param   none.
* @retval  none.
*/
void SoftUart::startSending(void) {
    bool bitValue = sendDataByteMask & sendDataBufer;
	outputTxBitSet(bitValue);
	sendDataByteMask >>= 1;
	(*timerTxStart)();
	return;
}

/**
* @brief   Callback of the trainsmit timer timeout handler.
* @param   none.
* @retval  none.
*/
void SoftUart::tx_bit_timer_handler(void) {
    timerTxInit(RxTxTimeout);
	if (sendDataByteMask) {
		startSending();
	} else {
	    sendingStatus = SOFT_UART_TX_READY_TO_SEND;
	}
}

/**
* @brief   Get raw last input data byte.
* @param   p_receivedByte:  output data byte pointer.
* @retval  uint8_t:  0 - no data, 1 - success.
*/
uint8_t  SoftUart::uart_receive(uint8_t *p_receivedByte) {
	if(p_receivedByte == NULL || !receiveDataByteStatus)
	{
		return 0;
	} else {
		*p_receivedByte = receivedDataByte;
		receiveDataByteStatus = false;
		return 1;
	}	
}

/**
* @brief  RX pin handler callback.
* @param  none.
* @retval none.
*/
void  SoftUart::rx_start_event_pin_handler(void) {
    (*inputRxPinInterruptOFF)();
    receivingStatus = SOFT_UART_RX_IN_PROGRESS;
	if (parity==SOFT_UART_PARITY_OFF) {
		receiveDataByteMask = 1 << (SOFT_UART_DATA_BITS_NUMBER + SOFT_UART_END_BITS_NUMBER);
	} else {
		receiveDataByteMask = 1 << (SOFT_UART_DATA_BITS_NUMBER + SOFT_UART_PARITY_BIT + SOFT_UART_END_BITS_NUMBER);
	}
	receiveDataBufer = 0;
	receiveDataByteStatus = false;
	RxParityCheckError = false;	
	RxStopBitsError = false;
	(*timerRxInit)(RxTxTimeout/2);
	(*timerRxStart)();
}

/**
* @brief  RX read at RX timer timeout handler.
* @param  pinValue: RX pin value.
* @retval none.
*/
void SoftUart::rx_bit_timer_handler(bool pinValue) {
	(*timerRxInit)(RxTxTimeout);
	if(pinValue == true) {
		receiveDataBufer |= receiveDataByteMask;
    }
	receiveDataByteMask >>= 1;

	if (receiveDataByteMask == 0) {
		/*stop bit check*/
		if ( receiveDataBufer & 0x0001) {
		    receiveDataBufer >>= SOFT_UART_END_BITS_NUMBER;
		} else {
		    receiveDataBufer >>= SOFT_UART_END_BITS_NUMBER;
			RxStopBitsError = true;
		}
		/*parity check if used*/
		if (parity == SOFT_UART_PARITY_ON) {
			inputParityValue = receiveDataBufer & 0x0001;
			receiveDataBufer >>= 1;
		}
		receivedDataByteSwapped = (uint8_t)(receiveDataBufer & 0xFF);
		for (int i = 0; i<8; i++)
		{
		     calculatedParityValue ^= (receiveDataBufer & 0x0001);
		     receiveDataBufer >>= 1;
       	}
       	    
       	if (calculatedParityValue != inputParityValue) {
       	    RxParityCheckError = true;
		}
	    if (!RxStopBitsError && !RxParityCheckError){
	        receiveDataByteStatus = true;
	    }
		/*swapping bits*/
        receivedDataByte = 0;
	    for(int i = 0; i < 8; i++) {
	        receivedDataByte <<= 1;
	        receivedDataByte |= receivedDataByteSwapped & 0x01;
	        receivedDataByteSwapped >>= 1;
	    }

		if (receiveDataByteStatus) {
	        //stop interruption
		    dataBuffer.put(receivedDataByte);
	        //start interruption
		}
		(*timerRxStop)();
		(*inputRxPinInterruptON)();
		receivingStatus = SOFT_UART_RX_ACTIVE;

	} else {
		(*timerRxStart)();
	}	
}

/**
* @brief  UART speed and parity bit settings function.
* @param  speedValue: UART speed value.
* @param  parityFlag: Parity flag.
* @retval none.
*/
void  SoftUart::uart_init(SoftUartSpeed_t speedValue, SoftUartParity_t parityFlag) {
	switch (speedValue) {
		case SOFT_UART_4800:
		    RxTxTimeout = SOFT_UART_4800_TIMEOUT;
			initialRxTimeout = SOFT_UART_4800_TIMEOUT + SOFT_UART_4800_TIMEOUT/2 ;
			break;
		case SOFT_UART_9600:
		    RxTxTimeout = SOFT_UART_9600_TIMEOUT;
			initialRxTimeout = SOFT_UART_9600_TIMEOUT + SOFT_UART_9600_TIMEOUT/2;
			break;
		case SOFT_UART_19200:
		    RxTxTimeout = SOFT_UART_19200_TIMEOUT;
			initialRxTimeout = SOFT_UART_19200_TIMEOUT + SOFT_UART_19200_TIMEOUT/2;
			break;
		case SOFT_UART_57600:
		    RxTxTimeout = SOFT_UART_57600_TIMEOUT;
			initialRxTimeout = SOFT_UART_57600_TIMEOUT + SOFT_UART_57600_TIMEOUT/2;
		    break;
	    case SOFT_UART_115200:
	        RxTxTimeout = SOFT_UART_115200_TIMEOUT;
	    	initialRxTimeout = SOFT_UART_115200_TIMEOUT + SOFT_UART_115200_TIMEOUT/2;
	    	break;			
	}

    receiveDataBufer = 0;
    receiveDataByteStatus = false;
    RxParityCheckError = false;
    RxStopBitsError = false;

	parity = parityFlag;
    speed = speedValue;

    (*timerTxInit)(RxTxTimeout);
    (*timerRxInit)(RxTxTimeout);
    (*inputRxPinInterruptOFF)();
    receivingStatus = SOFT_UART_RX_INACTIVE;
}

/**
* @brief  Stop RX.
* @param  none.
* @retval none.
*/
void SoftUart::stop_rx_uart(void) {
    receivingStatus = SOFT_UART_RX_INACTIVE;
	(*inputRxPinInterruptOFF)();
}

/**
* @brief  Start RX.
* @param  none.
* @retval none.
*/
void SoftUart::start_rx_uart(void) {
    receivingStatus = SOFT_UART_RX_ACTIVE;
    (*inputRxPinInterruptON)();
}

/**
* @brief  The constructor of the software implementation class.
* @param  inputRxBitReadFunc:
* @param  inputRxPinInterruptSetFunc:
* @param  timerRxInitFunc:
* @param  itimerRxStartFunc:
* @param  stopRxInterruptionsFunc:
* @param  outputTxBitSetFunc:
* @param  timerTxStartFunc:
* @param  timerTxStopFunc:
* @param  timerRxStopFunc:
*/
SoftUart::SoftUart ( bool (*inputRxBitReadFunc)(void),
		  void (*inputRxPinInterruptSetFunc)(void),
		  void (*timerRxInitFunc)(uint16_t timeout),
		  void (*timerRxStartFunc)(void),
		  void (*stopRxInterruptionsFunc)(void),
		  void (*outputTxBitSetFunc)(bool),
		  void (*timerTxInitFunc)(uint16_t timeout),
		  void (*timerTxStartFunc)(void) ,
		  void (*timerTxStopFunc)(void),
		  void (*timerRxStopFunc)(void)) {
		 	
		inputRxBitRead = inputRxBitReadFunc;
		inputRxPinInterruptON = inputRxPinInterruptSetFunc;
		timerRxInit = timerRxInitFunc;
		timerRxStart = timerRxStartFunc;
		inputRxPinInterruptOFF = stopRxInterruptionsFunc;
		outputTxBitSet = outputTxBitSetFunc;
		timerTxInit = timerTxInitFunc;	
		timerTxStart = timerTxStartFunc;
		timerTxStop = timerTxStopFunc;
		timerRxStop = timerRxStopFunc;
}

/**
* @brief  Pop the data from the ring buffer for RX.
* @param  data:    data container output pointer.
* @param  length:  actually read data length.
* @param  maxSize: max data length for reading.
* @retval none.
*/
uint8_t SoftUart::uart_pop_data(uint8_t* data, uint16_t& length, uint16_t maxSize){
    //stop interruption
    dataBuffer.get(data, length, maxSize);
    //start interruption
    return 1;
}

/**
* @brief  Check RX process status.
* @retval TX status.
*/
uint8_t SoftUart::uart_get_RX_state(void) {
    return receivingStatus;
}

/**
* @brief  Check TX process status.
* @retval TX status.
*/
uint8_t SoftUart::uart_get_TX_state(void) {
    return sendingStatus;
}











