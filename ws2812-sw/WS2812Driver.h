#ifndef WS2812SW_WS2812DRIVER_H
#define WS2812SW_WS2812DRIVER_H
#include <mbed-drivers/DigitalOut.h>
#include <memory>
#include <vector>

namespace ws2812_sw
{
    class WS2812PlatformImpl;
    
    class WS2812Driver
    {
    public:   
        /** Create a driver for a WS2812 LED string connected to the specified pin
         *
         * @param output_pin WS2812 data pin
         */
        WS2812Driver(PinName output_pin, uint32_t n);    
           
        /** Initialize any hardware resources needed. On STM32, ensure timer clock
         *  is enabled before calling init().
         */     
        void init();

        void set_element(uint32_t n, uint8_t r, uint8_t g, uint8_t b);
        
        void refresh();
        
    private:
        mbed::DigitalOut _output;
        uint32_t _n_leds;
        std::vector<uint8_t> _data;
        std::unique_ptr<WS2812PlatformImpl> _platform_impl;
    };
}
#endif
