/*!
 * @file        apm32f10x_int.c
 *
 * @brief       Main Interrupt Service Routines
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
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "main.h"
#include "apm32f10x_int.h"
#include "bsp_delay.h"

extern void USBD_IRQHandler(uint8_t busid);

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
    APM_DelayTickDec();
}


#if defined (USB_DEVICE)

/*!
 * @brief   This function handles USB low priority or CAN RX0 Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
#ifdef APM32F10X_HD
#if USB_SELECT == USB1
void USBD1_LP_CAN1_RX0_IRQHandler(void)
#else
void USBD2_LP_CAN2_RX0_IRQHandler(void)
#endif /* USB_SELECT */
#else
#if USB_SELECT == USB1
void USBD1_LP_CAN1_RX0_IRQHandler(void)
#else
void USBD2_LP_IRQHandler(void)
#endif /* USB_SELECT */
#endif
{
    USBD_IRQHandler(0);
}


/*!
 * @brief   This function handles USB high priority or CAN TX Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
#ifdef APM32F10X_HD
#if USB_SELECT == USB1
void USBD1_HP_CAN1_TX_IRQHandler(void)
#else
void USBD2_HP_CAN2_TX_IRQHandler(void)
#endif /* USB_SELECT */
#else
#if USB_SELECT == USB1
void USBD1_HP_CAN1_TX_IRQHandler(void)
#else
void USBD2_HP_IRQHandler(void)
#endif /* USB_SELECT */
#endif
{
    
}

#endif /* defined (USB_DEVICE) */
