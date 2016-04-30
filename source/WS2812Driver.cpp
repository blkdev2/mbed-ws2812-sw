#include <ws2812-sw/WS2812Driver.h>

#include <ws2812-sw/platforms/stm32f4.h>
#ifndef WS2812_SW_PLATFORM_IMPL
#error Unsupported target
#endif

using namespace ws2812_sw;

WS2812Driver::WS2812Driver(PinName output_pin, uint32_t n)
    : _output(output_pin),
      _n_leds(n),
      _data(3 * n)
{
    _platform_impl = new WS2812PlatformImpl(_output);
    
    // Initialize element data
    for (unsigned int i=0; i < 3 * _n_leds; ++i)
        _data[i] = 0;        
}

WS2812Driver::~WS2812Driver()
{
    delete _platform_impl;
}

void WS2812Driver::init()
{
    _platform_impl->init();
}

void WS2812Driver::set_element(uint32_t n, uint8_t r, uint8_t g, uint8_t b)
{
    if (n >= _n_leds) return;
    
    _data[3*n] = g;
    _data[3*n + 1] = r;
    _data[3*n + 2] = b;
}


void WS2812Driver::refresh()
{
    _platform_impl->start_burst();
    for (unsigned int i=0; i < 3 * _n_leds; ++i)
    {
        uint8_t b = _data[i];            
        for (int j=0; j < 8; ++j)
        {
            if (b & 0x80) {
                _platform_impl->transmit_one();
            }
            else {
                _platform_impl->transmit_zero();
            }
            
            b <<= 1;
        }
    }
    _platform_impl->end_burst();
    _platform_impl->transmit_reset();
}
