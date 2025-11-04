#ifndef __TOOLS_RINGBUFFER_H
#define __TOOLS_RINGBUFFER_H

#include <stdint.h>

    class RingBuffer {
    
        public:
            RingBuffer();
            virtual ~RingBuffer(){}
        
            void put(uint8_t element);
            void get(uint8_t* data, uint16_t& length, uint16_t maxSize);
            uint16_t getDataLenght();
            void clear();
        
        private:
            
            uint8_t m_buffer[512];
            uint16_t m_bufferSize;
            uint16_t m_putIndex;
            uint16_t m_getIndex;
            uint16_t m_dataLength;
        
            inline bool get(uint8_t &byte);
    };

#endif

