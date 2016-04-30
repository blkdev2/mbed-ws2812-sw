#ifdef TARGET_LIKE_STM32F4
#ifdef WS2812_SW_PLATFORM_IMPL
#error Conflicting target implementations
#endif
#define WS2812_SW_PLATFORM_IMPL

#include <mbed-drivers/mbed.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_tim.h>

namespace ws2812_sw
{
    class WS2812PlatformImpl
    {
    public:
        WS2812PlatformImpl(DigitalOut &output)
            : _output(output)
        {
            // stm32_timer: Should be a GP timer, e.g. TIM4
            TIM_TypeDef *stm32_timer = TIM4;
            __HAL_RCC_TIM4_CLK_ENABLE();
            _timer.Instance = stm32_timer;       
            // Create 2.4 MHz time base (84 MHz / 35), 3 ticks = 1 NZR bit window
            _timer.Init.Prescaler = 34;
            // Count up
            _timer.Init.CounterMode = TIM_COUNTERMODE_UP;
            // Free running -- explicit reset
            _timer.Init.Period = 0xFFFF;
            _timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
            _timer.Init.RepetitionCounter = 0;        
        }               
        
        void init()
        {
            HAL_TIM_Base_Init(&_timer);
            HAL_TIM_Base_Start(&_timer);
        }

        void transmit_zero()
        {
            uint32_t t0 = await_transition();
            _output = 1;
            while (get_counter() - t0 < 1); // 2.4 MHz: 1, 4 MHz: 1
            _output = 0;
            while (get_counter() - t0 < 3); // 2.4 MHz: 3, 4 MHz: 5
        }
        
        void transmit_one()
        {
            uint32_t t0 = await_transition();
            _output = 1;
            while (get_counter() - t0 < 2); // 2.4 MHz: 2, 4 MHz: 3
            _output = 0;
            while (get_counter() - t0 < 3); // 2.4 MHz: 3, 4 MHz: 5
        }
        
        void transmit_reset()
        {
            _output = 0;
            while (get_counter() < 120); // 2.4 MHz: 120, 4 Mhz: 200
        }
        
        void start_burst()
        {
            __HAL_TIM_SET_COUNTER(&_timer, 0);
            __disable_irq();       
        }
        
        void end_burst()
        {
            __enable_irq();
        }
        
    private:
        inline uint32_t get_counter()
        {
            return __HAL_TIM_GET_COUNTER(&_timer);
        }
        
        inline uint32_t await_transition()
        {
            uint32_t t0 = get_counter();
            uint32_t t1;
        while ((t1 = get_counter()) == t0);
        return t1;
        }
        
        DigitalOut &_output;
        TIM_HandleTypeDef _timer;
    };
}
#endif
