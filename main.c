

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_dfu_svci.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "bsp_btn_ble.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "nrf_ble_gatt.h"
#include "fds.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble_nus.h"

#include "hx711.h"
#include "correct.h"
#include "remote.h"
#include "remote_work.h"
#include "remote_setup.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "rgb_led.h"
#include "ble_comm.h"
#include "ble_correct.h"
#include "ble_set.h"
#include "archive.h"
#include "weight.h"

#include "device_name.h"
#include "LoRa.h"
#include "m_interface.h"



#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define MANUFACTURER_NAME               "Scale-driver"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout in units of seconds. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_MAX_TIMERS 3

//*************************************   IMPORTANT DEFINES!!!!!!!!!!!!!!    ************************************

#define MINUTES_FOR_SAVE         				15                  // only for production mode
#define TIMER_DEBUG_MS					 				1000
#define TIMER_PROD_MS						 				1000
#define CLOCK_COUNTER_IN_LIFE_COUNTER   60                 // in production must be 60 (1min = 60s)
#define ADC_TIMER												100
//***************************************************************************************************************

//#define MINUTES_FOR_FDS_CLEAR    60

BLE_NUS_DEF(m_nus);                                                                 /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

#define SCHED_MAX_EVENT_DATA_SIZE 8
#define SCHED_QUEUE_SIZE 64


#define LORA_SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi_lora = NRF_DRV_SPI_INSTANCE(LORA_SPI_INSTANCE);  /**< SPI instance. */

//static uint8_t* p_arr;
interface_t * _interface;


static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */
static void advertising_start(bool erase_bonds);                                    /**< Forward declaration of advertising start function */

uint8_t ble_string_get[20] = "";
//uint8_t ble_string_put1[20] = "";
uint8_t ble_string_adc[20] = "ad";
APP_TIMER_DEF(m_clock_id);
APP_TIMER_DEF(m_adc_timer_id);



int scale_coef = (802-309)/10;

void ble_comm_send_handler(uint8_t* buf){
			
			uint16_t length = strlen((char*)buf);
			if(ble_active) ble_nus_string_send(&m_nus, buf, &length);
}




void clock_value_save(void)
{
		life_counter++;
		//SEGGER_RTT_printf(0, "l/c_changed = %d\n\r", life_counter);
		ret_code_t error_code; 
		if(admin || root)
		{
			if((life_counter - current_life_counter) >= ADMIN_TIME_MINUTES) 
			{
				current_life_counter = 0;
				admin = 0;
				root = 0;
				//SEGGER_RTT_printf(0, "admin = %d\n\r", admin);
				
			}
		}
		
		if(activate_status == DEMO)
		{
			error_code = fds_update_value(&life_counter, file_id_c, fds_rk_clock);
			APP_ERROR_CHECK(error_code);
		//	SEGGER_RTT_printf(0, "save l/c= %d, c/c = %d, error = %d\n\r", life_counter, fds_clear_counter, error_code);
		}
		
		else if (life_counter % MINUTES_FOR_SAVE == 0)
		{
			//segtext("I'm here\n");
			error_code = fds_update_value(&life_counter, file_id_c, fds_rk_clock);
			
			//if(life_counter % 120 == 0)
			SEGGER_RTT_printf(0, "save l/c= %d, c/c = %d\n\r", life_counter, fds_clear_counter);
		}
	
		clock_counter = 0;
	//	SEGGER_RTT_printf(0, "l/c = %d\r\n", life_counter);
}


static void m_adc_timer_handler (void *p_context){
			Weighing();
			//adc_value = adc_value_r >> ble_settings.adcBitForCut;
			//segtext("work\n");
			//show_weight();	
			//weight_test();
			adc_cut();
			weight_define();
//	if(ble_settings.showADC >= 2){
//			//segnum1(adc_value);
//			
//			
//		}
	  
	
	
	if(ble_settings.showADC == 1 || ble_settings.showADC == 3)
	{
		//SEGGER_RTT_printf(0, "%d\r\n", adc_value >> ble_settings.adcBitForCut);
		uint8_t adc_pref[] = "ad";
		sprintf((char*)ble_string_adc, "%d", adc_value);
		uint16_t length = strlen((char*)ble_string_adc);
		memcpy(adc_pref+2, ble_string_adc, length);
		uint16_t length2 = strlen((char*)adc_pref);		
		//SEGGER_RTT_printf(0, "%d\r\n", adc_value);
		ble_nus_string_send(&m_nus, adc_pref, &length2);
	}
}


static void m_clock_timer_handler (void *p_context)
{
	clock_counter++;
	if(time_to_sleep > 0)
	{
		time_to_sleep--; // time for turn hx711 off decrease in seconds
	}
	for (uint8_t i = 0; i<=20; i++)
							{
								ble_string_adc[i] = 0;
							}
	if(adc_value)
	{
		
	}
	//SEGGER_RTT_printf(0, "clock = %d\r\n", clock_counter);
	if(clock_counter == CLOCK_COUNTER_IN_LIFE_COUNTER)  
	{
		//SEGGER_RTT_printf(0, "clock go to save = %d\r\n", clock_counter);
			clock_value_save();
		
	}
	//
}
void fds_get_init_data()
{
	fds_get_data(&life_counter, file_id_c, fds_rk_clock);
	fds_get_data(&activate_status, file_id_c, fds_rk_activate_status);
	fds_get_data(&activate_attempts, file_id_c, fds_rk_activate_attempts);
	fds_get_data(&power_down_count, file_id_c, fds_rk_power_down);
	fds_get_data(&corr_counter, file_id_c, fds_rk_corr_counter);
	fds_get_data(&demo1, file_id_c, fds_rk_demo1);
	fds_get_data(&demo2, file_id_c, fds_rk_demo1+1);
	fds_get_data(&demo3, file_id_c, fds_rk_demo1+2);
	fds_get_data(&demo4, file_id_c, fds_rk_demo1+3);
	fds_get_data(&demo5, file_id_c, fds_rk_demo1+4);
	fds_get_data(&cur_cor_max, file_id_c, fds_rk_cur_cor_max);
	fds_get_data(&cur_hr_max, file_id_c, fds_rk_cur_cor_max+1);
	fds_get_data(&cur_res_max, file_id_c, fds_rk_cur_cor_max+2);
	
	fds_get_data(&num_cor_buts, file_id, fds_rk_num_cor_but);
	fds_get_data(&cor_feedback, file_id, fds_rk_cor_feedback);
	fds_get_data(&correct_mode, file_id, fds_rk_corr_mode);
	
	fds_get_data(&time_feedback, file_id, fds_rk_time_feedback);
	fds_get_data(&feedback, file_id, fds_rk_feedback);
	fds_get_data(&fds_mac_init, file_id_c, fds_rk_mac_init);
	fds_get_data(&uart_work, file_id, fds_rk_uart_work);
	fds_get_data(&weight_float, file_id, fds_rk_weight_float);
	fds_get_data(&startWeightIndex, file_id, fds_rk_uart_weight_st);
	fds_get_data(&endWeightIndex, file_id, fds_rk_uart_weight_end);
	fds_get_data(&uart_ble_mode, file_id, fds_rk_uart_ble_mode);
	
	
	fds_get_data(&fds_remote_type, file_id_c, fds_rk_remote_type);
	fds_get_data(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
	fds_get_data(&fds_soft_version, file_id_c, fds_rk_soft_version);
	
	fds_get_data(&fds_volume_counter, file_id_c, fds_rk_volume_counter);
	fds_get_data(&fds_archive_counter, file_id_c, fds_rk_archive_counter);
	fds_get_data(&fds_option_status, file_id_c, fds_rk_option_status);
	fds_get_data(&fds_pcb_config, file_id_c, fds_rk_pcb_config);
	fds_get_data(&fds_uart_automode, file_id_c, fds_rk_uart_automode);
	
	fds_get_data(&fds_clear_counter, file_id_c, fds_rk_clear_counter);
	
	power_down_count++;
	fds_update_value(&power_down_count, file_id_c, fds_rk_power_down);
  init_corr_values();
	init_cal_values();
	offset_fds_kg_get_data();
	autocor_adc_values_get();
	SEGGER_RTT_printf(0, "STATUS = %d\r\n", activate_status);
		//APP_ERROR_CHECK(err_code);
}

static void m_clock_timer_init(void)
{
	
	app_timer_init();
	app_timer_create(&m_clock_id, APP_TIMER_MODE_REPEATED, m_clock_timer_handler); 
	app_timer_create(&m_adc_timer_id, APP_TIMER_MODE_REPEATED, m_adc_timer_handler); 
	app_timer_start(m_adc_timer_id, APP_TIMER_TICKS(ADC_TIMER), NULL);
	#ifdef DEBUG_MODE
	app_timer_start(m_clock_id, APP_TIMER_TICKS(TIMER_DEBUG_MS), NULL);
	#else
	app_timer_start(m_clock_id, APP_TIMER_TICKS(TIMER_PROD_MS), NULL);
	#endif
	
}

void gpio_init()
{
	nrf_gpio_cfg_output(RED_PIN);
	nrf_gpio_cfg_output(GREEN_PIN);
	nrf_gpio_cfg_output(BLUE_PIN);
	nrf_gpio_cfg_output(17);
	nrf_gpio_cfg_output(15);
	nrf_gpio_pin_clear(15);
	//nrf_gpio_cfg_output(31);
	
}

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}};

// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
#ifndef DEBUG_MODE

static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");
            // YOUR_JOB: Disconnect all bonded devices that currently are connected.
            //           This is required to receive a service changed indication
            //           on bootup after a successful (or aborted) Device Firmware Update.
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}

#endif

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_INFO("Connected to a previously bonded device.");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            advertising_start(false);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{

    // Initialize timer module.
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)device_name,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
																					
																					

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

static void nus_data_handler(ble_nus_evt_t * p_evt)
{
	  //uint32_t err_code = 0;

	  if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
			
        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
				
						{
							ble_string_get[i] = p_evt->params.rx_data.p_data[i];
							
						}
						ble_comm(ble_string_get);
						//ble_debug(ble_string_get);
					//flush ble_string_get buffer
				for(uint8_t i = 0; i<=20; i++)

						{
							ble_string_get[i] = 0;
							
						}
		}
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
			uint32_t err_code;
	
#ifndef DEBUG_MODE
	
    ble_dfu_buttonless_init_t dfus_init =
    {
        .evt_handler = ble_dfu_evt_handler
    };

    // Initialize the async SVCI interface to bootloader.
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);


    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
#endif
	
		ble_nus_init_t nus_init;
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
			
    }
		
		
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
static void application_timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
       uint32_t err_code;
       err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
       APP_ERROR_CHECK(err_code); */
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
//    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);

//    APP_ERROR_CHECK(err_code);

//    // Prepare wakeup buttons.
//    err_code = bsp_btn_ble_sleep_mode_prepare();
//    APP_ERROR_CHECK(err_code);

//    // Go to system-off mode (this function will not return; wakeup will cause a reset).
//    err_code = sd_power_system_off();
//    APP_ERROR_CHECK(err_code);
}

//add test comment
/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
			correct(0,0,0);
				ble_active = 0;
		// LED indication will be changed when advertising starts.
            break;

        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
						
            break;

#ifndef S140
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;
#endif

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init()
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/** @brief Clear bonding information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}







/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = NULL;

    init.evt_handler = on_adv_evt;
    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}



/**@brief Function for the Power manager.
 */
static void log_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief   Function for initializing the GATT module.
 * @details The GATT module handles ATT_MTU and Data Length update procedures automatically.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);

        NRF_LOG_DEBUG("advertising is started");
    }
}

static void power_management_init(void)
{
    uint32_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}





static void correct_handle(uint8_t type, uint16_t value){
		uint16_t add_number = 0;
		switch(type){
			case PLUS: add_number = 1000;
				break;
			case PERCENT: add_number = 2000;
				break;
		}
		current_correct = add_number + value;
		buttons_handle();
}

void lora_handler(uint8_t * _p_arr, uint8_t size, lora_event_t event)
{
	switch (event)
		{
				case RX_DONE:
					{
						correction_t *p_correction  = (correction_t*) (_p_arr+1);
						switch(*_p_arr)
							{
									case REMOTE_CORRECTION_SELECT: correct_handle(p_correction->v_type, p_correction->value);
											break;
									case REMOTE_CORRECTION_CANCEL: correct(0,0,0);
										break;
									case REMOTE_CORRECTION_EDIT: correct_handle(p_correction->v_type, p_correction->value);
											break;
									case REMOTE_MODE_CHANGE: change_correct_mode();
											break;
									case REMOTE_INIT:
										if(correct_mode == COR_MANUAL) lora_write_flag_1byte(REMOTE_MODE_CHANGE, 1);
										else if (correct_mode == COR_AUTO) lora_write_flag_1byte(REMOTE_MODE_CHANGE, 0);
							}
								lora_recive();
				break;
					}
				case TX_DONE: lora_recive();
		}
}


static void lora_initialize(){
#ifdef LORA_USE
		lora_init(spi_lora, 433E6, &lora_handler);
		lora_recive();
#endif
}

/**@brief Function for application main entry.
 */
int main(void)
{
		interface_t interface;
	
		_interface = &interface;
	
		interface.p_arr = nrf_calloc(7, sizeof(correction_t));
	
//		interface.interface_evnt_handler = interface_evnt_handler;
		interface.use_grams = false;
	
    bool erase_bonds;
		uint32_t err_code;
    // Initialize.
    log_init();
		ble_stack_init();
    timers_init();
    power_management_init();
    gpio_init();
		ble_set_init();
		nrf_define_test_pin();
  	define_pins();
		nrf_gpiote();
		pwm_init_corr();
		pwm_init_rgb();
		HX711_init();
		m_clock_timer_init();
		peer_manager_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    conn_params_init();
		//test one more comment
    application_timers_start();
		
		APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
		
		lora_initialize();

    advertising_start(erase_bonds);
		sd_ble_gap_addr_get(&mac_address);
			
		err_code = fds_test_init();
		APP_ERROR_CHECK(err_code);
		while(init_flag == 0);
		segtext("app started\n");
		fds_init_values();
	  fds_get_init_data();
		
		test_expired();
	  start_led();
		if(uart_work || uart_ble_mode) uart_init();
		sd_ble_gap_tx_power_set(4);
		check_for_old_board();
		//SEGGER_RTT_printf(0, "start_weight_index = %d\n", startWeightIndex);
		SEGGER_RTT_printf(0, "fds_uart_automode = %d, cal_turn_on = %d\n", fds_uart_automode, cal_turn_on);
		segtext("fds_option_status = ");
		segnum1(fds_option_status);
							
    for (;;)
    {
			
			app_sched_execute();
					if(correct_mode == COR_AUTO)
	
						{
							cor_auto_handle();
						}
					power_manage();
			}

}
