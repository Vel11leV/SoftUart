
#include "RingBuffer.hpp"

/**
* @brief  The constructor of RingBuffer class
* @param  bufferSize: uint16_t value of ring buffer size
* @retval none
*/
RingBuffer::RingBuffer()
{
    this->m_bufferSize = sizeof(m_buffer)/sizeof(m_buffer[0]);
    this->m_getIndex = 0;
    this->m_putIndex = 0;
    this->m_dataLength = 0;
}

/**
* @brief   Put new element to ring buffer. 
* @param   element:  new element.
* @retval  none.
*/
void RingBuffer::put(uint8_t element)
{
    //if buffer is not empty
    if(this->m_dataLength != 0){
        //if buffer is full
		if(this->m_putIndex == this->m_getIndex){
			this->m_getIndex++;
            this->m_dataLength--;
			if(this->m_getIndex == this->m_bufferSize)
				this->m_getIndex = 0;
		}
	}
    //put element to buffer
    this->m_buffer[this->m_putIndex++] = element;
    if(this->m_putIndex == this->m_bufferSize)
        this->m_putIndex = 0;
    this->m_dataLength++;
}

/**
* @brief   Get first element from ring buffer. 
*          You must check data length before calling this function.
*          If data length is 0 - You shouldn't call this function.
*          In other way there may be problems.
* @retval  none.
*/
inline bool RingBuffer::get(uint8_t &byte)
{
    if (m_dataLength == 0) {

        return false;
    }

    byte = this->m_buffer[this->m_getIndex++];
    if(this->m_getIndex == this->m_bufferSize)
        this->m_getIndex = 0;
    this->m_dataLength--;
    
    return true;
}

/**
* @brief   Get a number of elements from ring buffer. 
* @param   data:     Specify the pointer to array, where to save data.
* @param   length:   Specify the link to variable, where to set length of
*                    read data from ring buffer.
* @param   maxSize:  Specify  number of data to read from buffer.
* @retval  none.
*/
void RingBuffer::get(uint8_t* data, uint16_t& length, uint16_t maxSize)
{
    bool l_flag = false;

    length = 0;
    if (this->m_dataLength == 0) {
        return;
    }
    
    //Protects against sudden change of this->m_dataLength
    uint32_t dataLength = this->m_dataLength;
    
    if(maxSize > dataLength)
        maxSize = dataLength;
    
    for(uint32_t i = 0; i < maxSize; i++) {

        l_flag  = this->get(data[i]);
        length++;

        if (l_flag == false) {

            break;
        }
    }
}

/**
* @brief   Get a length of received elements in ring buffer. 
* @param   none.
* @retval  uint16_t: length of received elements.
*/
uint16_t RingBuffer::getDataLenght()
{
    return this->m_dataLength;
}
/**
* @brief   Clear the ring buffer. 
* @param   none.
* @retval  none.
*/
void RingBuffer::clear()
{
    this->m_getIndex = this->m_putIndex = this->m_dataLength = 0;
}
