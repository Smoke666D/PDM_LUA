/*
 * platform_init.h
 *
 *  Created on: Mar 7, 2023
 *      Author: igor.dymov
 */

#ifndef PLATFORM_INIT_H_
#define PLATFORM_INIT_H_





//#define PDM
#define PCM


#ifdef PDM
    #define BAT_INDEX           3U
    #define TEMP_INDEX           4U
    #define ADC1_CHANNELS      9U
    #define ADC2_CHANNELS      7U
    #define ADC3_CHANNELS      9U
    #define AIN_NUMBER          3U        //Количесвто аналоговых входов
#endif
#ifdef PCM
    #define TEMP_INDEX         13U
    #define BAT_INDEX          12U
    #define AIN_NUMBER             12U        //Количесвто аналоговых входов
#endif


#ifdef PDM
#define OUT1_PIN  InCH20_1_Pin
#define OUT1_PORT InCH20_1_GPIO_Port
#define OUT2_PIN  InCH20_2_Pin
#define OUT2_PORT InCH20_2_GPIO_Port
#define OUT3_PIN  InCH20_3_Pin
#define OUT3_PORT InCH20_3_GPIO_Port
#define OUT4_PIN  InCH20_4_Pin
#define OUT4_PORT InCH20_4_GPIO_Port
#define OUT5_PIN  InCH20_5_Pin
#define OUT5_PORT InCH20_5_GPIO_Port
#define OUT6_PIN  InCH20_6_Pin
#define OUT6_PORT InCH20_6_GPIO_Port
#define OUT7_PIN  InCH20_7_Pin
#define OUT7_PORT InCH20_7_GPIO_Port
#define OUT8_PIN  InCH20_8_Pin
#define OUT8_PORT InCH20_8_GPIO_Port
#define OUT9_PIN  InCH8_9_Pin
#define OUT9_PORT InCH8_9_GPIO_Port
#define OUT10_PIN  InCH8_10_Pin
#define OUT10_PORT InCH8_10_GPIO_Port
#define OUT11_PIN  InCH8_11_Pin
#define OUT11_PORT InCH8_11_GPIO_Port
#define OUT12_PIN  InCH8_12_Pin
#define OUT12_PORT InCH8_12_GPIO_Port
#define OUT13_PIN  InCH8_13_Pin
#define OUT13_PORT InCH8_13_GPIO_Port
#define OUT14_PIN  InCH8_14_Pin
#define OUT14_PORT InCH8_14_GPIO_Port
#define OUT15_PIN  InCH8_15_Pin
#define OUT15_PORT InCH8_15_GPIO_Port
#define OUT16_PIN  InCH8_16_Pin
#define OUT16_PORT InCH8_16_GPIO_Port
#define OUT17_PIN  InCH8_17_Pin
#define OUT17_PORT InCH8_17_GPIO_Port
#define OUT18_PIN  InCH8_18_Pin
#define OUT18_PORT InCH8_18_GPIO_Port
#define OUT19_PIN  InCH8_19_Pin
#define OUT19_PORT InCH8_19_GPIO_Port
#define OUT20_PIN  InCH8_20_Pin
#define OUT20_PORT InCH8_20_GPIO_Port
#endif






#ifdef PCM
#define OUT1_PIN  DR1_Pin
#define OUT1_PORT DR1_GPIO_Port
#define OUT2_PIN  DR2_Pin
#define OUT2_PORT DR2_GPIO_Port
#define OUT3_PIN  DR3_Pin
#define OUT3_PORT DR3_GPIO_Port
#define OUT4_PIN  DR4_Pin
#define OUT4_PORT DR4_GPIO_Port
#define OUT5_PIN  DR5_Pin
#define OUT5_PORT DR5_GPIO_Port
#define OUT6_PIN  DR6_Pin
#define OUT6_PORT DR6_GPIO_Port
#define OUT7_PIN  DR7_Pin
#define OUT7_PORT DR7_GPIO_Port
#define OUT8_PIN  DR8_Pin
#define OUT8_PORT DR8_GPIO_Port
#define OUT9_PIN  DR9_Pin
#define OUT9_PORT DR9_GPIO_Port
#define OUT10_PIN  DR10_Pin
#define OUT10_PORT DR10_GPIO_Port
#define OUT11_PIN  DR11_Pin
#define OUT11_PORT DR11_GPIO_Port
#define OUT12_PIN  DR12_Pin
#define OUT12_PORT DR12_GPIO_Port
#define OUT13_PIN  DR13_Pin
#define OUT13_PORT DR13_GPIO_Port
#define OUT14_PIN  DR14_Pin
#define OUT14_PORT DR14_GPIO_Port
#define OUT15_PIN  DR15_Pin
#define OUT15_PORT DR15_GPIO_Port
#define OUT16_PIN  DR16_Pin
#define OUT16_PORT DR16_GPIO_Port
#define OUT17_PIN  DR17_Pin
#define OUT17_PORT DR17_GPIO_Port
#define OUT18_PIN  DR18_Pin
#define OUT18_PORT DR18_GPIO_Port
#define OUT19_PIN  DR19_Pin
#define OUT19_PORT DR19_GPIO_Port
#define OUT20_PIN  DR20_Pin
#define OUT20_PORT DR20_GPIO_Port
#endif

#endif /* PLATFORM_INIT_H_ */
