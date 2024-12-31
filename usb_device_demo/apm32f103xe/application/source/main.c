/**
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2023-12-01
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes ***************************************************************/
#include "main.h"

/* Private includes *******************************************************/
#include "apm32f1xx_device_cfg.h"
#include "bsp_delay.h"
#include "cdc_acm_hid.h"

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/**
 * @brief   Main program
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    /* Device configuration */
    SPD_DeviceConfig();

    cdc_acm_hid_init(0, USBD_BASE);

    /* Infinite loop */
    while (1)
    {
        cdc_acm_data_send(0, "Hello World!\r\n", 14);
        APM_DelayMs(500U);
    }
}

void usb_dc_low_level_init(void)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USB);

#ifdef APM32F10X_HD
#if USB_SELECT == USB1
    NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 1, 0);
#else
    NVIC_EnableIRQRequest(USBD2_LP_CAN2_RX0_IRQn, 1, 0);
#endif
#else
#if USB_SELECT == USB1
    NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 1, 0);
#else
    NVIC_EnableIRQRequest(USBD2_LP_IRQn, 1, 0);
#endif
#endif

#if USB_SELECT == USB1
    USBD2_Disable(USBD);
#else
    USBD2_Enable(USBD);
#endif /* USB_SELECT */
}

void usb_dc_low_level_deinit(void)
{
    RCM_DisableAPB1PeriphClock(RCM_APB1_PERIPH_USB);

#ifdef APM32F10X_HD
#if USB_SELECT == USB1
    NVIC_DisableIRQRequest(USBD1_LP_CAN1_RX0_IRQn);
#else
    NVIC_DisableIRQRequest(USBD2_LP_CAN2_RX0_IRQn);
#endif
#else
#if USB_SELECT == USB1
    NVIC_DisableIRQRequest(USBD1_LP_CAN1_RX0_IRQn);
#else
    NVIC_DisableIRQRequest(USBD2_LP_IRQn);
#endif
#endif

#if USB_SELECT == USB1
    USBD2_Enable(USBD);
#else
    USBD2_Disable(USBD);
#endif /* USB_SELECT */
}
