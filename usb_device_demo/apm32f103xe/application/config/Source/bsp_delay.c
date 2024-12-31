/*!
 * @file        bsp_delay.c
 *
 * @brief       Delay board support package program body
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "bsp_delay.h"

__IO uint32_t delayTick;

/*!
 * @brief       Configures Delay.
 *
 * @param       None
 *
 * @retval      None
 */
void APM_DelayInit(void)
{
    if (SysTick_Config(SystemCoreClock / 1000 / SYSTICK_FRQ))
    {
        while (1);
    }

    NVIC_EnableIRQRequest(SysTick_IRQn, 15, 0);
}

/*!
 * @brief       Decrement tick
 *
 * @param       None
 *
 * @retval      None
 */
void APM_DelayTickDec(void)
{
    if (delayTick != 0x00)
    {
        delayTick--;
    }
}

/*!
 * @brief       Configures Delay us.
 *
 * @param       nus: Specifies the delay to be configured.
 *              This parameter can be one of following parameters:
 *              @arg nus
 *
 * @retval      None
 */
void APM_DelayUs(__IO uint32_t nus)
{
    delayTick = nus;

    while (delayTick != 0);
}

/*!
 * @brief       Configures Delay ms.
 *
 * @param       nms: Specifies the delay to be configured.
 *              This parameter can be one of following parameters:
 *              @arg nms
 *
 * @retval      None
 */
void APM_DelayMs(__IO uint32_t nms)
{
    delayTick = nms * 1000;

    while (delayTick != 0);
}
