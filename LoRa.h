#ifndef LORA_H
#define LORA_H

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "SEGGER_RTT.h"

#include "app_scheduler.h"


// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS_MASK 			 0x11
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_PKT_SNR_VALUE        0x1b
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_MODEM_CONFIG_3       0x26
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// PA config
#define PA_BOOST                 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40
#define IRQ_VALID_HEDER_MASK			 0x10

#define MAX_PKT_LENGTH           255


#define LORA_DEFAULT_RESET_PIN 31
#define LORA_DEFAULT_DIO0_PIN 6 

#define MAP_DIO0_LORA_RXDONE   0x00  // 00------
#define MAP_DIO0_LORA_TXDONE   0x40  // 01------

//#define PA_OUTPUT_RFO_PIN      17 
//#define PA_OUTPUT_PA_BOOST_PIN 18

//#define MAX_POWER //uncoment to set max power mode
#define SUPER_MAX_POWER //DANGER

typedef enum
{
	TX_DONE,
	RX_DONE
}lora_event_t;

typedef	void (*p_lora_hendler_t)(uint8_t * p_arr, uint8_t, lora_event_t);

static uint8_t rx_buf[2];
static uint8_t tx_buf[2];
static long _frequency;
static bool irq_flag;

static nrf_drv_spi_t _spi;
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */
static nrf_drv_spi_xfer_desc_t nrf_drv_spi_xfer_desc;

void lora_recive(void);
void beginPacket(void);
void lora_write(const uint8_t *buffer, uint16_t size);
void endPacket(void);
uint8_t lora_init(nrf_drv_spi_t, long, p_lora_hendler_t);
int rssi(void);

#endif

