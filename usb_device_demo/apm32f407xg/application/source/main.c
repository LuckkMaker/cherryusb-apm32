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
#include "apm32f4xx_device_cfg.h"
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
    DAL_DeviceConfig();

    cdc_acm_hid_init(0, USB_OTG_FS_PERIPH_BASE);

    /* Infinite loop */
    while (1)
    {
        DAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);
        cdc_acm_data_send(0, "Hello World!\r\n", 14);
        DAL_Delay(500U);
    }
}

void usb_dc_low_level_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure USB OTG GPIO */
    __DAL_RCM_GPIOA_CLK_ENABLE();

    /* USB DM, DP pin configuration */
    GPIO_InitStruct.Pin         = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate   = GPIO_AF10_OTG_FS;
    DAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure USB OTG */
    __DAL_RCM_USB_OTG_FS_CLK_ENABLE();

    /* Configure interrupt */
    DAL_NVIC_SetPriority(OTG_FS_IRQn, 1U, 0U);
    DAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

void usb_dc_low_level_deinit(void)
{
    /* Disable peripheral clock */
    __DAL_RCM_USB_OTG_FS_CLK_DISABLE();

    /* USB DM, DP pin configuration */
    DAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

    /* Disable peripheral interrupt */
    DAL_NVIC_DisableIRQ(OTG_FS_IRQn);
}