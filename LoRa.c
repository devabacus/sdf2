#include "LoRa.h"

p_lora_hendler_t _p_lora_hendler;
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
	if(p_event->type ==NRF_DRV_SPI_EVENT_DONE){
    spi_xfer_done = true;
	}
}

uint8_t singleTransfer(uint8_t address, uint8_t value)
{
	
 nrf_drv_spi_config_t spi_config = 
		{                                                          
//			.sck_pin      = SPI_SCK_PIN,                
//			.mosi_pin     = SPI_MOSI_PIN,                
//			.miso_pin     = SPI_MISO_PIN,                
//			.ss_pin       = SPI_SS_PIN,  
			.sck_pin      = 19,                
			.mosi_pin     = 16,                
			.miso_pin     = 20,                
			.ss_pin       = 18,  			
			.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,         
			.orc          = 0x00,                                    
			.frequency    = NRF_DRV_SPI_FREQ_8M,                     
			.mode         = NRF_DRV_SPI_MODE_0,                      
			.bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,         
		};
		
	APP_ERROR_CHECK(nrf_drv_spi_init(&_spi, &spi_config, spi_event_handler, NULL));	
	
	rx_buf[1] = 0;
	tx_buf[0] = address;
	tx_buf[1] = value;
	nrf_drv_spi_xfer_desc.p_rx_buffer = rx_buf;
	nrf_drv_spi_xfer_desc.p_tx_buffer	=	tx_buf;
	nrf_drv_spi_xfer_desc.rx_length		=	2;
	nrf_drv_spi_xfer_desc.tx_length		=	2;
	spi_xfer_done = false;
	
	APP_ERROR_CHECK(nrf_drv_spi_xfer(&_spi, &nrf_drv_spi_xfer_desc, NULL));
	
	 while (!spi_xfer_done)
        {
            UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        }
		
	nrf_drv_spi_uninit(&_spi);
	return rx_buf[1];
	
}

uint8_t writeRegister(uint8_t address, uint8_t value)
{
	uint8_t	ret;
  ret = singleTransfer(address | 0x80, value);
	return ret;
}

uint8_t readRegister(uint8_t address)
{
	return singleTransfer(address & 0x7f, 0x00);
}


void setFrequency(long frequency)
{
	_frequency = frequency;
	NRF_LOG_DEBUG("Frequency = %d", frequency);
  	
	union{
		uint64_t freg64;
		struct{
			uint8_t lsb;
			uint8_t	mid;
			uint8_t	msb;
		}byte;
	}freg;
	
	freg.freg64 = ((uint64_t)frequency << 19) / 32000000;

	NRF_LOG_DEBUG("Frequency MSB = %x", (freg.byte.msb));
  writeRegister(REG_FRF_MSB, freg.byte.msb);
	NRF_LOG_DEBUG("Frequency MID = %x", (freg.byte.mid));
  writeRegister(REG_FRF_MID, freg.byte.mid);
	NRF_LOG_DEBUG("Frequency LSB = %x", (freg.byte.lsb));
  writeRegister(REG_FRF_LSB, freg.byte.lsb);
}

static void serial_scheduled_ex (void * p_event_data, uint16_t event_size)
{
	uint8_t adr = 0;
	uint8_t packetLength = 0;
	uint8_t rx_byte =	0;
	uint8_t * arr;
	
	uint8_t event = readRegister(REG_IRQ_FLAGS);
	if(event == 0x50)
		{	
			writeRegister(REG_IRQ_FLAGS, IRQ_RX_DONE_MASK | IRQ_VALID_HEDER_MASK);
			
			packetLength = readRegister(REG_RX_NB_BYTES);
			adr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
			writeRegister(REG_FIFO_ADDR_PTR, adr);
			
			writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
			
			arr = malloc(packetLength);
			
			for(uint32_t i = 0; i <= packetLength-1; i++)
			{
				*(arr+i) = readRegister(REG_FIFO);
			}
			_p_lora_hendler(arr, packetLength, RX_DONE);
			free(arr);
		}
	if(event == IRQ_TX_DONE_MASK)
	{
		writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
		_p_lora_hendler(NULL, NULL, TX_DONE);
	}
}

void dio(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_DEBUG("REG_IRQ_FLAGS %x\r\n", readRegister(REG_IRQ_FLAGS));
	app_sched_event_put(NULL, NULL, serial_scheduled_ex);	
	irq_flag = true;
}

uint8_t lora_init(nrf_drv_spi_t spi, long frequency, p_lora_hendler_t p_lora_hendler)
{
	_spi = spi;
	_p_lora_hendler = p_lora_hendler;
			ret_code_t err_code;
	
	
	
	
		nrf_gpio_cfg_output(LORA_DEFAULT_RESET_PIN);
	
	//todo add check init in main file
		//err_code = nrf_drv_gpiote_init();
    //APP_ERROR_CHECK(err_code);
		
		 nrf_drv_gpiote_in_config_t config =   {       
        .is_watcher = false,                    
        .hi_accuracy = false,                 
        .pull = NRF_GPIO_PIN_PULLDOWN,            
        .sense = NRF_GPIOTE_POLARITY_LOTOHI,    
    };
		irq_flag = false;
		APP_ERROR_CHECK(nrf_drv_gpiote_in_init(LORA_DEFAULT_DIO0_PIN, &config, dio));
		if(err_code != NRF_SUCCESS)
		{
			NRF_LOG_ERROR("GPOIO INIT FAIL");
		}
		
		nrf_drv_gpiote_in_event_enable(LORA_DEFAULT_DIO0_PIN, true);
		
	NRF_LOG_INFO("LoRa initialization");
	NRF_LOG_INFO("Reset LoRa");
	
	nrf_gpio_pin_clear(LORA_DEFAULT_RESET_PIN);
  nrf_delay_ms(10);
  nrf_gpio_pin_set(LORA_DEFAULT_RESET_PIN);
  nrf_delay_ms(10);
	
	
	NRF_LOG_INFO("Check LoRa version");
	uint8_t version = 0;
	version = readRegister(REG_VERSION);
  if (version != 0x12) {
		NRF_LOG_ERROR("%x",	version);
		NRF_LOG_ERROR("LoRa version invalid");
    return 0;
  }
	
	NRF_LOG_INFO("LoRa version OK");
	
	NRF_LOG_INFO("Put to LoRa mode and sleep state");
	writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);

	NRF_LOG_INFO("Set frequency");
	setFrequency(frequency);

	NRF_LOG_DEBUG("Set FIFO base addresses");
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
	
	NRF_LOG_DEBUG("Set boost on, 150%% LNA current");
	writeRegister(REG_LNA, 0x20 | 0x03);
	
	NRF_LOG_DEBUG("Set automatic gain control");
	writeRegister(REG_MODEM_CONFIG_3, 0x04);

	#ifndef MAX_POWER
	NRF_LOG_INFO("Set MaxPower 15dbm OutputPower = 7.6dbm"); 
	writeRegister(REG_PA_CONFIG, 0x4F);
	#else
	NRF_LOG_INFO("Set MaxPower 17dbm OutputPower = 17dbm"); 
	writeRegister(REG_PA_CONFIG, 0xFF);
	#endif
	
	#ifdef SUPER_MAX_POWER
	NRF_LOG_INFO("Set MaxPower 20dbm OutputPower = 20dbm"); 
	writeRegister(REG_PA_CONFIG, 0xFF);
	writeRegister(0x4d, 0x07);
	#endif
	
	writeRegister(REG_MODEM_CONFIG_1, 0x72);
	NRF_LOG_INFO("Put to standby mode");
	writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
	
	//readRegister(0x1D);
	//readRegister(0x4d);
	return 1;
}

void beginPacket()
{
//	SEGGER_RTT_printf(0, "Pacet begin\r\nSet Signal bandwidth: 125 kHz\r\nError coding rate = 4/5\r\nExplicit header mode\r\n");
	//writeRegister(REG_MODEM_CONFIG_1, 0x72);
	
//	SEGGER_RTT_printf(0, "Pacet begin\r\nSet Signal bandwidth: 7.8 kHz\r\nError coding rate = 4/5\r\nExplicit header mode\r\n");
	writeRegister(REG_MODEM_CONFIG_1, 0x72);
	
	SEGGER_RTT_printf(0, "Reset FIFO address and paload length\r\n");
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_PAYLOAD_LENGTH, 0);
}

void lora_write(const uint8_t *buffer, uint16_t size)
{
	uint16_t currentLength = readRegister(REG_PAYLOAD_LENGTH);

	for (size_t i = 0; i < size; i++) {
    writeRegister(REG_FIFO, *(buffer+i));
		//SEGGER_RTT_printf(0, "%d\r\n", *(buffer+i));
  }
	writeRegister(REG_PAYLOAD_LENGTH, currentLength + size);
}


void endPacket()
{
  // put in TX mode
	writeRegister(REG_DIO_MAPPING_1, MAP_DIO0_LORA_TXDONE);
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
}
void lora_recive()
{
writeRegister(REG_MODEM_CONFIG_1, 0x72);
NRF_LOG_DEBUG("%x\r\n", readRegister(REG_MODEM_CONFIG_1));
writeRegister(REG_DIO_MAPPING_1, MAP_DIO0_LORA_RXDONE);
writeRegister(REG_FIFO_ADDR_PTR, 0);
writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);

}

int rssi()
{
	int rssi =0;
	rssi = readRegister(REG_PKT_RSSI_VALUE);
	rssi = rssi - (_frequency < 868E6 ? 164 : 157);
	//SEGGER_RTT_printf(0, " RSSI = %d\r\n", rssi);
	return rssi;
}
