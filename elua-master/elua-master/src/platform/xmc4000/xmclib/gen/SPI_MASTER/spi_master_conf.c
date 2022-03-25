/*********************************************************************************************************************
* DAVE APP Name : SPI_MASTER       APP Version: 4.3.22
*
* NOTE:
* This file is generated by DAVE. Any manual modification done to this file will be lost when the code is regenerated.
*********************************************************************************************************************/

/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-02-16:
 *     - Initial version<br>
 *
 * 2015-02-20:
 *     - Guard is generated for dx0_input. This helps to cross check the dynamic mode change support<br>
 *
 * 2015-05-08:
 *     - Bug fix for code generation if advanced pin configuration is not enabled.<br>
 *     - Seperate code generation for full duplex mode<br>
 *     - Conditional code generation for parity error<br>
 *     - XMC_SPI_CH_EnableFEM() and XMC_SPI_CH_SetBitOrderLsbFirst() are generated conditionally<br>
 *     - Configuration for Leading/Training is added<br>
 *
 * 2015-06-20:
 *     - Code generation for parity error is generated after selecting the parity type and enabling the callback in 
 *        interrupt settings tab.<br>
 *     - word_length is added to SPI_MASTER_DYNAMIC_CONFIG_t, to support runtime change.<br>
 *     - Clock settings are updated with combo. So defined a map locally for clcok settings.
 *
 * 2015-07-07:
 *     - DYNAMIC_CONFIG_t is renamed as RUNTIME_t
 *
 * 2015-09-01:
 *     - 'gpio_mode_alternate_function' is renamed as 'gpio_output_alternate_function' and extended to support XMC14 devices.
 *
 * 2015-09-30:
 *     - dummy read are added for setena bit field.
 *
 * 2015-10-14:
 *     - new variable "spi_master_config_mode" to store the actual mode generated during initialisation.
 *     - bug fix: node pointer assignment for protocol event is generated always.  
 *     - XMC_USIC_CH_TriggerServiceRequest() is generated during init,  
 *     - receive start disable event is removed from the DMA callback to support the higher baudrate.
 *
 * 2016-01-20:
 *     - In DMA mode, TDV flag polling is added to ensure last byte is shifted out from the buffer.
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "spi_master.h"

/***********************************************************************************************************************
* LOCAL ROUTINES
***********************************************************************************************************************/
#if (SPI_MASTER_INTERRUPT_TRANSMIT_MODE == 1U)
/*
 * Function implements the data transmission. It is called from the transmit interrupt service handler.
 * Function pushes data to the output block and releases control. It is called again when the previous data is
 * transmitted. When transmit FIFO is used, the function sets the trigger limit based on the size of data to be
 * transmitted. This makes sure that the CPU usage is optimum.
 *
 * Note: The API is only meant for internal use. The implementation should not be modified.
 */
extern void SPI_MASTER_lTransmitHandler(const SPI_MASTER_t * const handle);
#endif
#if (SPI_MASTER_INTERRUPT_RECEIVE_MODE == 1U)
/*
 * Function implements the data reception. It is called from the receive interrupt service handler.
 * Function reads data from the receive block and updates the receive buffer. It is called again when the data is
 * received again. When receive FIFO is used, the function sets the trigger limit based on the size of data to be
 * received. This makes sure that the CPU usage is optimum.
 *
 * Note: The API is only meant for internal use. The implementation should not be modified.
 */
extern void SPI_MASTER_lReceiveHandler(const SPI_MASTER_t * const handle);
#endif
#if (SPI_MASTER_PARITY_ERROR == 1U)
/*
 * Function monitors the configured protocol interrupt flags. It is called from the protocol interrupt service handler.
 * Function reads the status of the SPI_MASTER channel and checks for configured flags in the APP GUI. If any callback
 * function is provided in the APP UI, it will be called when the selected flag is set.
 *
 * Note: The API is only meant for internal use. The implementation should not be modified.
 */
extern void SPI_MASTER_lProtocolHandler(const SPI_MASTER_t * const handle);
#endif
/***********************************************************************************************************************
* DATA STRUCTURES
***********************************************************************************************************************/

static SPI_MASTER_STATUS_t spi_uSD_sdram_kit_lInit(void);
/* Data Transmit pin from SPI_MASTER */
const SPI_MASTER_GPIO_t spi_uSD_sdram_kit_MOSI = 
{
  .port = (XMC_GPIO_PORT_t *)PORT3_BASE,
  .pin  = (uint8_t)15
};         
                                      
SPI_MASTER_GPIO_CONFIG_t spi_uSD_sdram_kit_MOSI_Config = 
{ 
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
  },
  .hw_control = XMC_GPIO_HWCTRL_DISABLED
};

/* Data Receive pin for SPI_MASTER */
const SPI_MASTER_GPIO_t spi_uSD_sdram_kit_MISO = 
{
  .port = (XMC_GPIO_PORT_t *)PORT3_BASE,
  .pin  = (uint8_t)14
};

SPI_MASTER_GPIO_CONFIG_t spi_uSD_sdram_kit_MISO_Config = 
{
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_INPUT_TRISTATE,
  },
                        
};

const SPI_MASTER_GPIO_t spi_uSD_sdram_kit_SCLKOUT = 
{
  .port = (XMC_GPIO_PORT_t *)PORT0_BASE,
  .pin  = (uint8_t)13
};     

const SPI_MASTER_GPIO_CONFIG_t spi_uSD_sdram_kit_SCLKOUT_Config = 
{ 
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
}
};

const SPI_MASTER_GPIO_t spi_uSD_sdram_kit_SS_0 =
{
.port = (XMC_GPIO_PORT_t *)PORT3_BASE,
.pin  = (uint8_t)4
};

const SPI_MASTER_GPIO_CONFIG_t spi_uSD_sdram_kit_SS_0_Config =
{
.port_config =
{
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
  },
  .slave_select_ch = XMC_SPI_CH_SLAVE_SELECT_2
};

XMC_SPI_CH_CONFIG_t spi_uSD_sdram_kit_Channel_Config =
{
  .baudrate = 2000000U,
  .bus_mode = (XMC_SPI_CH_BUS_MODE_t)XMC_SPI_CH_BUS_MODE_MASTER,
  .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
  .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE
};
              

const SPI_MASTER_CONFIG_t spi_uSD_sdram_kit_Config  = 
{
  .channel_config          = &spi_uSD_sdram_kit_Channel_Config,
  .fptr_spi_master_config  = spi_uSD_sdram_kit_lInit,
  /* FIFO configuration */
  .tx_fifo_size            = (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
  .rx_fifo_size            = (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
  /* Clock Settings */
  .shift_clk_passive_level = XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_DISABLED, 
  .slave_select_lines      = (uint8_t)1,
  .leading_trailing_delay  = (uint8_t)2,
  .spi_master_config_mode  = XMC_SPI_CH_MODE_STANDARD, /* spi master initial mode configured mode */
  .transmit_mode           = SPI_MASTER_TRANSFER_MODE_INTERRUPT,
  .receive_mode            = SPI_MASTER_TRANSFER_MODE_INTERRUPT,
   
  .tx_cbhandler            = NULL,
  .rx_cbhandler            = NULL,
  .parity_cbhandler        = NULL,
  .mosi_0_pin              = &spi_uSD_sdram_kit_MOSI, /*!< mosi0 pin pointer*/
  .mosi_0_pin_config       = &spi_uSD_sdram_kit_MOSI_Config,
  .mosi_1_pin              = &spi_uSD_sdram_kit_MISO,
  .mosi_1_pin_config       = &spi_uSD_sdram_kit_MISO_Config,
  .mosi_2_pin              = NULL,
  .mosi_2_pin_config       = NULL,
  .mosi_3_pin              = NULL,
  .mosi_3_pin_config       = NULL,
  .sclk_out_pin_config     = &spi_uSD_sdram_kit_SCLKOUT_Config,
  .sclk_out_pin            = &spi_uSD_sdram_kit_SCLKOUT,
  .slave_select_pin        = {&spi_uSD_sdram_kit_SS_0, NULL,
                              NULL, NULL,
                              NULL, NULL,
                              NULL, NULL
                             },
  .slave_select_pin_config = {&spi_uSD_sdram_kit_SS_0_Config, NULL,
                              NULL, NULL,
                              NULL, NULL,
                              NULL, NULL
                             },

  .tx_sr   = (SPI_MASTER_SR_ID_t)SPI_MASTER_SR_ID_1,
  .rx_sr   = (SPI_MASTER_SR_ID_t)SPI_MASTER_SR_ID_0,
};
                           
SPI_MASTER_RUNTIME_t spi_uSD_sdram_kit_runtime =
{
  .spi_master_mode = XMC_SPI_CH_MODE_STANDARD, /* spi master transmission mode */
  .word_length     = 8U,
                       
  #ifdef USIC1_C1_DX0_P3_14
  .dx0_input = SPI_MASTER_INPUT_B,
  #else
  .dx0_input = SPI_MASTER_INPUT_INVALID,
  #endif

  #ifdef USIC1_C1_DX0_P3_15
  .dx0_input_half_duplex = SPI_MASTER_INPUT_B,
  #else
  .dx0_input_half_duplex = SPI_MASTER_INPUT_INVALID,
  #endif
                           
  .tx_data_dummy = false,
  .rx_data_dummy = true,
  .tx_busy = false,
  .rx_busy = false
};
                  
SPI_MASTER_t spi_uSD_sdram_kit =
{
  .channel = XMC_SPI1_CH1, /* USIC channel */
  .config  = &spi_uSD_sdram_kit_Config, /* spi master configuration structure pointer */
  .runtime = &spi_uSD_sdram_kit_runtime,
};
                          
/*
 * @brief Configure the port registers and data input registers of SPI channel
 *
 * @param[in] handle Pointer to an object of SPI_MASTER configuration
 */
static SPI_MASTER_STATUS_t spi_uSD_sdram_kit_lInit(void)
{
  SPI_MASTER_STATUS_t status;
  status = SPI_MASTER_STATUS_SUCCESS; 
  /* LLD initialization */
  XMC_SPI_CH_Init(XMC_SPI1_CH1, &spi_uSD_sdram_kit_Channel_Config);
                             
  XMC_SPI_CH_DisableFEM(XMC_SPI1_CH1);
                              
  XMC_SPI_CH_SetBitOrderMsbFirst(XMC_SPI1_CH1);
          
  XMC_SPI_CH_SetWordLength(XMC_SPI1_CH1, (uint8_t)8);

  XMC_SPI_CH_SetFrameLength(XMC_SPI1_CH1, (uint8_t)64);

  /* Configure the clock polarity and clock delay */
  XMC_SPI_CH_ConfigureShiftClockOutput(XMC_SPI1_CH1,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_DISABLED,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
  /* Configure Leading/Trailing delay */
  XMC_SPI_CH_SetSlaveSelectDelay(XMC_SPI1_CH1, 2U);

               
  /* Configure the input pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)14, &spi_uSD_sdram_kit_MISO_Config.port_config);

  /* Configure the data input line selected */
  XMC_SPI_CH_SetInputSource(XMC_SPI1_CH1, XMC_SPI_CH_INPUT_DIN0, (uint8_t)SPI_MASTER_INPUT_B);
  /* Start the SPI_Channel */
  XMC_SPI_CH_Start(XMC_SPI1_CH1);

  /* Configure the output pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)15, &spi_uSD_sdram_kit_MOSI_Config.port_config);
    
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT0_BASE, (uint8_t)13, &spi_uSD_sdram_kit_SCLKOUT_Config.port_config);

  /* Configure the pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)4, &spi_uSD_sdram_kit_SS_0_Config.port_config);
  XMC_SPI_CH_EnableSlaveSelect(XMC_SPI1_CH1, XMC_SPI_CH_SLAVE_SELECT_2);

  XMC_USIC_CH_SetInterruptNodePointer(XMC_SPI1_CH1,
                                      XMC_USIC_CH_INTERRUPT_NODE_POINTER_PROTOCOL,
                                      (uint32_t)SPI_MASTER_SR_ID_2);
            
  /* Configure transmit FIFO settings */
  XMC_USIC_CH_TXFIFO_Configure(XMC_SPI1_CH1,
                               16U,
                               (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
                               1U);

  /* Configure the service interrupt nodes for standard transmit FIFO events */
               
  XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(XMC_SPI1_CH1,
                                             XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             (uint32_t)SPI_MASTER_SR_ID_1);
  /* Configure receive FIFO settings */
  XMC_USIC_CH_RXFIFO_Configure(XMC_SPI1_CH1,
                               0U,
                               (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
                               0U);
             
  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_SPI1_CH1,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             (uint32_t)SPI_MASTER_SR_ID_0);
  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_SPI1_CH1,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE,
                                             (uint32_t)SPI_MASTER_SR_ID_0);
  /* Set priority of the Transmit interrupt */
  NVIC_SetPriority((IRQn_Type)91, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U, 0U));
     
  /* Enable Transmit interrupt */
  NVIC_EnableIRQ((IRQn_Type)91);
             
  /* Set priority of the Receive interrupt */
  NVIC_SetPriority((IRQn_Type)90, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 62U, 0U));
    
  /* Enable Receive interrupt */
  NVIC_EnableIRQ((IRQn_Type)90);
    
  return status;
}
/*Transmit ISR*/
void spi_uSD_sdram_kit_tx_handler()
{
  SPI_MASTER_lTransmitHandler(&spi_uSD_sdram_kit);
}

/*Receive ISR*/
void spi_uSD_sdram_kit_rx_handler()
{
  SPI_MASTER_lReceiveHandler(&spi_uSD_sdram_kit);
}

static SPI_MASTER_STATUS_t SPI_MASTER_0_lInit(void);
/* Data Transmit pin from SPI_MASTER */
const SPI_MASTER_GPIO_t SPI_MASTER_0_MOSI = 
{
  .port = (XMC_GPIO_PORT_t *)PORT3_BASE,
  .pin  = (uint8_t)13
};         
                                      
SPI_MASTER_GPIO_CONFIG_t SPI_MASTER_0_MOSI_Config = 
{ 
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
  },
  .hw_control = XMC_GPIO_HWCTRL_DISABLED
};

/* Data Receive pin for SPI_MASTER */
const SPI_MASTER_GPIO_t SPI_MASTER_0_MISO = 
{
  .port = (XMC_GPIO_PORT_t *)PORT2_BASE,
  .pin  = (uint8_t)5
};

SPI_MASTER_GPIO_CONFIG_t SPI_MASTER_0_MISO_Config = 
{
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_INPUT_TRISTATE,
  },
                        
};

const SPI_MASTER_GPIO_t SPI_MASTER_0_SCLKOUT = 
{
  .port = (XMC_GPIO_PORT_t *)PORT3_BASE,
  .pin  = (uint8_t)6
};     

const SPI_MASTER_GPIO_CONFIG_t SPI_MASTER_0_SCLKOUT_Config = 
{ 
  .port_config = 
  {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
}
};

const SPI_MASTER_GPIO_t SPI_MASTER_0_SS_0 =
{
.port = (XMC_GPIO_PORT_t *)PORT3_BASE,
.pin  = (uint8_t)12
};

const SPI_MASTER_GPIO_CONFIG_t SPI_MASTER_0_SS_0_Config =
{
.port_config =
{
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
    .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE
  },
  .slave_select_ch = XMC_SPI_CH_SLAVE_SELECT_1
};

XMC_SPI_CH_CONFIG_t SPI_MASTER_0_Channel_Config =
{
  .baudrate = 19200U,
  .bus_mode = (XMC_SPI_CH_BUS_MODE_t)XMC_SPI_CH_BUS_MODE_MASTER,
  .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
  .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE
};
              

const SPI_MASTER_CONFIG_t SPI_MASTER_0_Config  = 
{
  .channel_config          = &SPI_MASTER_0_Channel_Config,
  .fptr_spi_master_config  = SPI_MASTER_0_lInit,
  /* FIFO configuration */
  .tx_fifo_size            = (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
  .rx_fifo_size            = (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
  /* Clock Settings */
  .shift_clk_passive_level = XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_DISABLED, 
  .slave_select_lines      = (uint8_t)1,
  .leading_trailing_delay  = (uint8_t)2,
  .spi_master_config_mode  = XMC_SPI_CH_MODE_STANDARD, /* spi master initial mode configured mode */
  .transmit_mode           = SPI_MASTER_TRANSFER_MODE_INTERRUPT,
  .receive_mode            = SPI_MASTER_TRANSFER_MODE_INTERRUPT,
   
  .tx_cbhandler            = NULL,
  .rx_cbhandler            = NULL,
  .parity_cbhandler        = NULL,
  .mosi_0_pin              = &SPI_MASTER_0_MOSI, /*!< mosi0 pin pointer*/
  .mosi_0_pin_config       = &SPI_MASTER_0_MOSI_Config,
  .mosi_1_pin              = &SPI_MASTER_0_MISO,
  .mosi_1_pin_config       = &SPI_MASTER_0_MISO_Config,
  .mosi_2_pin              = NULL,
  .mosi_2_pin_config       = NULL,
  .mosi_3_pin              = NULL,
  .mosi_3_pin_config       = NULL,
  .sclk_out_pin_config     = &SPI_MASTER_0_SCLKOUT_Config,
  .sclk_out_pin            = &SPI_MASTER_0_SCLKOUT,
  .slave_select_pin        = {&SPI_MASTER_0_SS_0, NULL,
                              NULL, NULL,
                              NULL, NULL,
                              NULL, NULL
                             },
  .slave_select_pin_config = {&SPI_MASTER_0_SS_0_Config, NULL,
                              NULL, NULL,
                              NULL, NULL,
                              NULL, NULL
                             },

  .tx_sr   = (SPI_MASTER_SR_ID_t)SPI_MASTER_SR_ID_3,
  .rx_sr   = (SPI_MASTER_SR_ID_t)SPI_MASTER_SR_ID_5,
};
                           
SPI_MASTER_RUNTIME_t SPI_MASTER_0_runtime =
{
  .spi_master_mode = XMC_SPI_CH_MODE_STANDARD, /* spi master transmission mode */
  .word_length     = 8U,
                       
  #ifdef USIC0_C1_DX0_P2_5
  .dx0_input = SPI_MASTER_INPUT_B,
  #else
  .dx0_input = SPI_MASTER_INPUT_INVALID,
  #endif

  #ifdef USIC0_C1_DX0_P3_13
  .dx0_input_half_duplex = SPI_MASTER_INPUT_B,
  #else
  .dx0_input_half_duplex = SPI_MASTER_INPUT_INVALID,
  #endif
                           
  .tx_data_dummy = false,
  .rx_data_dummy = true,
  .tx_busy = false,
  .rx_busy = false
};
                  
SPI_MASTER_t SPI_MASTER_0 =
{
  .channel = XMC_SPI0_CH1, /* USIC channel */
  .config  = &SPI_MASTER_0_Config, /* spi master configuration structure pointer */
  .runtime = &SPI_MASTER_0_runtime,
};
                          
/*
 * @brief Configure the port registers and data input registers of SPI channel
 *
 * @param[in] handle Pointer to an object of SPI_MASTER configuration
 */
static SPI_MASTER_STATUS_t SPI_MASTER_0_lInit(void)
{
  SPI_MASTER_STATUS_t status;
  status = SPI_MASTER_STATUS_SUCCESS; 
  /* LLD initialization */
  XMC_SPI_CH_Init(XMC_SPI0_CH1, &SPI_MASTER_0_Channel_Config);
                             
  XMC_SPI_CH_SetBitOrderMsbFirst(XMC_SPI0_CH1);
          
  XMC_SPI_CH_SetWordLength(XMC_SPI0_CH1, (uint8_t)8);

  XMC_SPI_CH_SetFrameLength(XMC_SPI0_CH1, (uint8_t)64);

  /* Configure the clock polarity and clock delay */
  XMC_SPI_CH_ConfigureShiftClockOutput(XMC_SPI0_CH1,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_DISABLED,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
  /* Configure Leading/Trailing delay */
  XMC_SPI_CH_SetSlaveSelectDelay(XMC_SPI0_CH1, 2U);

               
  /* Configure the input pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT2_BASE, (uint8_t)5, &SPI_MASTER_0_MISO_Config.port_config);

  /* Configure the data input line selected */
  XMC_SPI_CH_SetInputSource(XMC_SPI0_CH1, XMC_SPI_CH_INPUT_DIN0, (uint8_t)SPI_MASTER_INPUT_B);
  /* Start the SPI_Channel */
  XMC_SPI_CH_Start(XMC_SPI0_CH1);

  /* Configure the output pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)13, &SPI_MASTER_0_MOSI_Config.port_config);
    
  /* Initialize SPI SCLK out pin */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)6, &SPI_MASTER_0_SCLKOUT_Config.port_config);

  /* Configure the pin properties */
  XMC_GPIO_Init((XMC_GPIO_PORT_t *)PORT3_BASE, (uint8_t)12, &SPI_MASTER_0_SS_0_Config.port_config);
  XMC_SPI_CH_EnableSlaveSelect(XMC_SPI0_CH1, XMC_SPI_CH_SLAVE_SELECT_1);

  XMC_USIC_CH_SetInterruptNodePointer(XMC_SPI0_CH1,
                                      XMC_USIC_CH_INTERRUPT_NODE_POINTER_PROTOCOL,
                                      (uint32_t)SPI_MASTER_SR_ID_0);
            
  /* Configure transmit FIFO settings */
  XMC_USIC_CH_TXFIFO_Configure(XMC_SPI0_CH1,
                               16U,
                               (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
                               1U);

  /* Configure the service interrupt nodes for standard transmit FIFO events */
               
  XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(XMC_SPI0_CH1,
                                             XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             (uint32_t)SPI_MASTER_SR_ID_3);
  /* Configure receive FIFO settings */
  XMC_USIC_CH_RXFIFO_Configure(XMC_SPI0_CH1,
                               0U,
                               (XMC_USIC_CH_FIFO_SIZE_t)XMC_USIC_CH_FIFO_SIZE_16WORDS,
                               0U);
             
  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_SPI0_CH1,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             (uint32_t)SPI_MASTER_SR_ID_5);
  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_SPI0_CH1,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE,
                                             (uint32_t)SPI_MASTER_SR_ID_5);
  /* Set priority of the Transmit interrupt */
  NVIC_SetPriority((IRQn_Type)87, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63U, 0U));
     
  /* Enable Transmit interrupt */
  NVIC_EnableIRQ((IRQn_Type)87);
             
  /* Set priority of the Receive interrupt */
  NVIC_SetPriority((IRQn_Type)89, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 62U, 0U));
    
  /* Enable Receive interrupt */
  NVIC_EnableIRQ((IRQn_Type)89);
    
  return status;
}
/*Transmit ISR*/
void SPI_MASTER_0_tx_handler()
{
  SPI_MASTER_lTransmitHandler(&SPI_MASTER_0);
}

/*Receive ISR*/
void SPI_MASTER_0_rx_handler()
{
  SPI_MASTER_lReceiveHandler(&SPI_MASTER_0);
}

