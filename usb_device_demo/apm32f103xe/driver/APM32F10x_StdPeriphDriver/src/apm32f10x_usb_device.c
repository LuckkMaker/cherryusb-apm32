/*!
 * @file        apm32f10x_usb_device.h
 *
 * @brief       USB device function handle
 *
 * @version     V1.0.1
 *
 * @date        2023-03-27
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
#include "apm32f10x_usb_device.h"
#include "bsp_delay.h"

/** @addtogroup APM32F10x_StdPeriphDriver
  @{
*/

/** @addtogroup USB_Device_Driver USB Device Driver
  @{
*/

/** @defgroup USB_Device_Functions Functions
  @{
*/

#if defined (USB_DEVICE)

/*!
 * @brief     USB device start
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Start(USBD_HANDLE_T* usbdh)
{
    usbdh->usbGlobal->INTSTS = 0;
    
    USBD_EnableInterrupt(usbdh->usbGlobal, USBD_INT_CTR | \
                                           USBD_INT_WKUP | \
                                           USBD_INT_SUS | \
                                           USBD_INT_ERR | \
                                           USBD_INT_RST | \
                                           USBD_INT_SOF | \
                                           USBD_INT_ESOF);
}

/*!
 * @brief     USB device stop
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Stop(USBD_HANDLE_T* usbdh)
{
    /* Disable USB all global interrupt */
    USBD_DisableInterrupt(usbdh->usbGlobal, USBD_INT_CTR | \
                                            USBD_INT_WKUP | \
                                            USBD_INT_SUS | \
                                            USBD_INT_ERR | \
                                            USBD_INT_RST | \
                                            USBD_INT_SOF | \
                                            USBD_INT_ESOF);
}

/*!
 * @brief     USB device stop
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_StopDevice(USBD_HANDLE_T* usbdh)
{
    USBD_SetForceReset(usbdh->usbGlobal);
    
    usbdh->usbGlobal->INTSTS = 0;
    
    usbdh->usbGlobal->CTRL_B.FORRST = BIT_SET;
    usbdh->usbGlobal->CTRL_B.PWRDOWN = BIT_SET;
}

/*!
 * @brief     USB device open EP
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @param     epType: endpoint type
 *
 * @param     epMps: endpoint maxinum of packet size
 *
 * @retval    None
 */
void USBD_EP_Open(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                  uint8_t epType, uint16_t epMps)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    if ((epAddr & 0x80) == 0x80)
    {
        usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;

        usbdh->epIN[epAddrTemp].epNum   = epAddrTemp;
        usbdh->epIN[epAddrTemp].epType  = epType;
        usbdh->epIN[epAddrTemp].mps     = epMps;

        usbdh->epIN[epAddrTemp].txFifoNum = usbdh->epIN[epAddrTemp].epNum;
    }
    else
    {
        usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;

        usbdh->epOUT[epAddrTemp].epNum   = epAddrTemp;
        usbdh->epOUT[epAddrTemp].epType  = epType;
        usbdh->epOUT[epAddrTemp].mps     = epMps;
    }

    /* Init data PID */
    if (epType == EP_TYPE_BULK)
    {
        usbdh->epIN[epAddrTemp].dataPID = 0;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        USBD_ConfigEP(usbdh->usbGlobal, &usbdh->epIN[epAddrTemp]);
    }
    else
    {
        USBD_ConfigEP(usbdh->usbGlobal, &usbdh->epOUT[epAddrTemp]);
    }
}

/*!
 * @brief     USB device close EP
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_Close(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    if ((epAddr & 0x80) == 0x80)
    {
        usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;
    }
    else
    {
        usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        USBD_ResetEP(usbdh->usbGlobal, &usbdh->epIN[epAddrTemp]);
    }
    else
    {
        USBD_ResetEP(usbdh->usbGlobal, &usbdh->epOUT[epAddrTemp]);
    }
}

/*!
 * @brief     USB device set EP on stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_ClearStall(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    USBD_ENDPOINT_INFO_T *ep;
    
    if ((epAddr & 0x0F) > usbdh->usbCfg.devEndpointNum)
    {
        return;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        ep = &usbdh->epIN[epAddr & 0x0F];
        ep->epDir = EP_DIR_IN;
    }
    else
    {
        ep = &usbdh->epOUT[epAddr & 0x0F];
        ep->epDir = EP_DIR_OUT;
    }

    ep->stallStatus = DISABLE;
    ep->epNum = epAddr & 0x0F;

    if(ep->bufferStatus == USBD_EP_BUFFER_SINGLE)
    {
        if (ep->epDir == EP_DIR_IN)
        {
            USBD_EP_ResetTxToggle(usbdh->usbGlobal, ep->epNum);
            
            if(ep->epType != EP_TYPE_ISO)
            {
                USBD_EP_SetTxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_NAK);
            }
        }
        else
        {
            USBD_EP_ResetRxToggle(usbdh->usbGlobal, ep->epNum);
            
            USBD_EP_SetRxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_VALID);
        }
    }
}

/*!
 * @brief     USB device set EP on stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_Stall(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    USBD_ENDPOINT_INFO_T *ep;
    
    if ((epAddr & 0x0F) > usbdh->usbCfg.devEndpointNum)
    {
        return;
    }
    
    if ((epAddr & 0x80) == 0x80)
    {
        ep = &usbdh->epIN[epAddr & 0x0F];
        ep->epDir = EP_DIR_IN;
    }
    else
    {
        ep = &usbdh->epOUT[epAddr & 0x0F];
        ep->epDir = EP_DIR_OUT;
    }

    ep->stallStatus = ENABLE;
    ep->epNum = epAddr & 0x0F;
    
    if (ep->epDir == EP_DIR_IN)
    {
        USBD_EP_SetTxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_STALL);
    }
    else
    {
        USBD_EP_SetRxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_STALL);
    }
}

/*!
 * @brief     USB device get EP stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    Stall status
 */
uint8_t USBD_EP_ReadStallStatus(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    if ((epAddr & 0x80) == 0x80)
    {
        return (usbdh->epIN[epAddr & 0x7F].stallStatus);
    }
    else
    {
        return (usbdh->epOUT[epAddr & 0x7F].stallStatus);
    }
}

/*!
 * @brief     USB device EP start transfer
 *
 * @param     usbdh: USB device handler.
 *
 * @param     ep : endpoint handler
 *
 * @retval    None
 */
void USBD_EP_XferStart(USBD_HANDLE_T* usbdh, USBD_ENDPOINT_INFO_T* ep)
{
    uint32_t length;
    uint16_t epStatus;
    
    if(ep->epDir == EP_DIR_IN)
    {
        if(ep->bufLen > ep->mps)
        {
            length = ep->mps;
        }
        else
        {
            length = ep->bufLen;
        }
        
        if(ep->bufferStatus == USBD_EP_BUFFER_SINGLE)
        {
            USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr, ep->buffer, length);
            USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
        }
        else
        {
            /* double buffer */
            if(ep->epType == EP_TYPE_BULK)
            {
                if(ep->dbBufferLen > ep->mps)
                {
                    USBD_EP_SetKind(usbdh->usbGlobal, ep->epNum);
                    
                    ep->dbBufferLen -= length;
                    
                    epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, ep->epNum);
                    
                    if(epStatus & USBD_EP_BIT_TXDTOG)
                    {
                        /* Buffer1 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr1, ep->buffer, length);
                        ep->buffer += length;
                        
                        if(ep->dbBufferLen > ep->mps)
                        {
                            ep->dbBufferLen -= length;
                        }
                        else
                        {
                            length = ep->dbBufferLen;
                            ep->dbBufferLen = 0;
                        }
                        
                        /* Buffer0 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
                    }
                    else
                    {
                        /* Buffer0 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
                        ep->buffer += length;
                        
                        if(ep->dbBufferLen > ep->mps)
                        {
                            ep->dbBufferLen -= length;
                        }
                        else
                        {
                            length = ep->dbBufferLen;
                            ep->dbBufferLen = 0;
                        }
                        
                        /* Buffer1 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr1, ep->buffer, length);
                    }
                }
                else
                {
                    USBD_EP_ResetKind(usbdh->usbGlobal, ep->epNum);
                    
                    length = ep->dbBufferLen;
                    
                    USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                    
                    USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
                }
            }
            else
            {
                USBD_EP_SetKind(usbdh->usbGlobal, ep->epNum);
                
                ep->dbBufferLen -= length;
                
                epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, ep->epNum);
                
                if(epStatus & USBD_EP_BIT_TXDTOG)
                {
                    /* Buffer1 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    else
                    {
                        USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    
                    USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr1, ep->buffer, length);
                    ep->buffer += length;
                    
                    if(ep->dbBufferLen > ep->mps)
                    {
                        ep->dbBufferLen -= length;
                    }
                    else
                    {
                        length = ep->dbBufferLen;
                        ep->dbBufferLen = 0;
                    }
                    
                    if(length)
                    {
                        /* Buffer0 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
                    }
                }
                else
                {
                    /* Buffer0 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    else
                    {
                        USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    
                    USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
                    ep->buffer += length;
                    
                    if(ep->dbBufferLen > ep->mps)
                    {
                        ep->dbBufferLen -= length;
                    }
                    else
                    {
                        length = ep->dbBufferLen;
                        ep->dbBufferLen = 0;
                    }
                    
                    if(length)
                    {
                        /* Buffer1 */
                        if(ep->epDir == 0)
                        {
                            USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        else
                        {
                            USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                        }
                        
                        USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr1, ep->buffer, length);
                    }
                }
            }
        }
        
        USBD_EP_SetTxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_VALID);
    }
    else
    {
        if(ep->bufferStatus == USBD_EP_BUFFER_SINGLE)
        {
            if(ep->bufLen > ep->mps)
            {
                length = ep->mps;
                ep->bufLen -= length;
            }
            else
            {
                length = ep->bufLen;
                ep->bufLen = 0;
            }
            
            USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
        }
        else
        {
            switch(ep->epType)
            {
                case EP_TYPE_BULK:
                    /* Buffer0 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, ep->mps);
                    }
                    else
                    {
                        USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, ep->mps);
                    }
                    
                    /* Buffer1 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, ep->mps);
                    }
                    else
                    {
                        USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, ep->mps);
                    }
                    
                    if(ep->bufCount)
                    {
                        epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, ep->epNum);
                        
                        if(((epStatus & USBD_EP_BIT_RXDTOG) && ((epStatus & USBD_EP_BIT_TXDTOG))) || \
                           (((epStatus & USBD_EP_BIT_RXDTOG) == 0) && ((epStatus & USBD_EP_BIT_TXDTOG) == 0)))
                        {
                            USBD_EP_ToggleTx(usbdh->usbGlobal,ep->epNum);
                        }
                    }
                    break;
                
                case EP_TYPE_ISO:
                    if(ep->bufLen > ep->mps)
                    {
                        length = ep->mps;
                        ep->bufLen -= length;
                    }
                    else
                    {
                        length = ep->bufLen;
                        ep->bufLen = 0;
                    }
                    
                    /* Buffer0 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    else
                    {
                        USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    
                    /* Buffer1 */
                    if(ep->epDir == 0)
                    {
                        USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    else
                    {
                        USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                    }
                    break;
                
                default:
                    return;
            }
        }
        
        USBD_EP_SetRxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_VALID);
    }
}

/*!
 * @brief     USB device EP receive handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    None
 */
void USBD_EP_Receive(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                     uint8_t* buffer, uint32_t length)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    usbdh->epOUT[epAddrTemp].epNum = epAddr & 0x0F;
    usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;

    usbdh->epOUT[epAddrTemp].buffer = buffer;
    usbdh->epOUT[epAddrTemp].bufCount = 0;
    usbdh->epOUT[epAddrTemp].bufLen = length;

    if (epAddr & 0x0F)
    {
        USBD_EP_XferStart(usbdh, &usbdh->epOUT[epAddrTemp]);
    }
    else
    {
        /* EP0 */
        USBD_EP_XferStart(usbdh, &usbdh->epOUT[epAddrTemp]);
    }
}

/*!
 * @brief     USB device EP transfer handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    None
 */
void USBD_EP_Transfer(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                      uint8_t* buffer, uint32_t length)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    usbdh->epIN[epAddrTemp].epNum = epAddr & 0x0F;
    usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;

    usbdh->epIN[epAddrTemp].buffer = buffer;
    usbdh->epIN[epAddrTemp].bufCount = 0;
    usbdh->epIN[epAddrTemp].bufLen = length;
    
#if defined (USB_DEVICE)
    usbdh->epIN[epAddrTemp].dbBufferFill = ENABLE;
    usbdh->epIN[epAddrTemp].dbBufferLen = length;
#endif

    if (epAddr & 0x0F)
    {
        USBD_EP_XferStart(usbdh, &usbdh->epIN[epAddrTemp]);
    }
    else
    {
        /* EP0 */
        USBD_EP_XferStart(usbdh, &usbdh->epIN[epAddrTemp]);
    }
}

/*!
 * @brief     USB device read EP last receive data size
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr: endpoint address
 *
 * @retval    size of last receive data
 */
uint32_t USBD_EP_ReadRxDataLen(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    return usbdh->epOUT[epAddr & 0x0F].bufCount;
}

/*!
 * @brief     USB device flush EP handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @retval    usb device status
 */
void USBD_EP_Flush(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    if (epAddr & 0x80)
    {

    }
    else
    {

    }
}

/*!
 * @brief     Handle USB device double buffer BULK IN transfer
 *
 * @param     usbdh: USB device handler
 *
 * @param     ep: ep handler
 *
 * @param     epStatus: ep register value
 *
 * @retval    None
 */
static void USBD_EP_DB_Transmit(USBD_HANDLE_T* usbdh, USBD_ENDPOINT_INFO_T* ep, uint16_t epStatus)
{
    uint32_t length;
    uint16_t cnt;
    
    /* Buffer0 */
    if(epStatus & USBD_EP_BIT_TXDTOG)
    {
        cnt = USBD_EP_ReadTxCnt(usbdh->usbGlobal, ep->epNum);
        
        if(ep->bufLen > cnt)
        {
            ep->bufLen -= cnt;
        }
        else
        {
            ep->bufLen = 0;
        }
        
        if(ep->bufLen == 0)
        {
            /* Buffer0 */
            if(ep->epDir == 0)
            {
                USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            else
            {
                USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            
            /* Buffer1 */
            if(ep->epDir == 0)
            {
                USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            else
            {
                USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            
            USBD_DataInStageCallback(usbdh, ep->epNum);
            
            if(epStatus & USBD_EP_BIT_RXDTOG)
            {
                USBD_EP_ToggleRx(usbdh->usbGlobal, ep->epNum);
            }
        }
        else
        {
            if(epStatus & USBD_EP_BIT_RXDTOG)
            {
                USBD_EP_ToggleRx(usbdh->usbGlobal, ep->epNum);
            }
            
            if(ep->dbBufferFill == ENABLE)
            {
                ep->buffer += cnt;
                ep->bufCount += cnt;
                
                if(ep->dbBufferLen >= ep->mps)
                {
                    length = ep->mps;
                    ep->dbBufferLen -= length;
                }
                else if(ep->dbBufferLen == 0)
                {
                    length = cnt;
                    ep->dbBufferFill = DISABLE;
                }
                else
                {
                    length = ep->dbBufferLen;
                    ep->dbBufferLen = 0;
                    ep->dbBufferFill = DISABLE;
                }
                
                /* Buffer0 */
                if(ep->epDir == 0)
                {
                    USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, length);
                }
                else
                {
                    USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, length);
                }
                
                /* Write buffer to PMA */
                USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr0, ep->buffer, length);
            }
        }
    }
    /* Buffer1 */
    else
    {
        /* Buffer1 */
        cnt = USBD_EP_ReadRxCnt(usbdh->usbGlobal, ep->epNum);
        
        if(ep->bufLen >= cnt)
        {
            ep->bufLen -= cnt;
        }
        else
        {
            ep->bufLen = 0;
        }
        
        if(ep->bufLen == 0)
        {
            /* Buffer0 */
            if(ep->epDir == 0)
            {
                USBD_EP_SetBuffer0RxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            else
            {
                USBD_EP_SetTxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            
            /* Buffer1 */
            if(ep->epDir == 0)
            {
                USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            else
            {
                USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, 0);
            }
            
            USBD_DataInStageCallback(usbdh, ep->epNum);
            
            if((epStatus & USBD_EP_BIT_RXDTOG) == 0)
            {
                USBD_EP_ToggleRx(usbdh->usbGlobal, ep->epNum);
            }
        }
        else
        {
            if((epStatus & USBD_EP_BIT_RXDTOG) == 0)
            {
                USBD_EP_ToggleRx(usbdh->usbGlobal, ep->epNum);
            }
            
            if(ep->dbBufferFill == ENABLE)
            {
                ep->buffer += cnt;
                ep->bufCount += cnt;
                
                if(ep->dbBufferLen >= ep->mps)
                {
                    length = ep->mps;
                    ep->dbBufferLen -= length;
                }
                else if(ep->dbBufferLen == 0)
                {
                    length = cnt;
                    ep->dbBufferFill = DISABLE;
                }
                else
                {
                    length = ep->dbBufferLen;
                    ep->dbBufferLen = 0;
                    ep->dbBufferFill = DISABLE;
                }
                
                /* Buffer1 */
                if(ep->epDir == 0)
                {
                    USBD_EP_SetRxCnt(usbdh->usbGlobal, ep->epNum, length);
                }
                else
                {
                    USBD_EP_SetBuffer0TxCnt(usbdh->usbGlobal, ep->epNum, length);
                }
                
                /* Write buffer to PMA */
                USBD_EP_WritePacketData(usbdh->usbGlobal, ep->pmaAddr1, ep->buffer, length);
            }
        }
    }
    
    /* Enable IN EP */
    USBD_EP_SetTxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_VALID);
}

/*!
 * @brief     Handle USB device double buffer BULK OUT transfer
 *
 * @param     usbdh: USB device handler
 *
 * @param     ep: ep handler
 *
 * @param     epStatus: ep register value
 *
 * @retval    None
 */
static uint16_t USBD_EP_DB_Receive(USBD_HANDLE_T* usbdh, USBD_ENDPOINT_INFO_T* ep, uint16_t epStatus)
{
    uint16_t cnt;
    
    /* Buffer0 */
    if(epStatus & USBD_EP_BIT_RXDTOG)
    {
        /* Buffer0 */
        cnt = USBD_EP_ReadTxCnt(usbdh->usbGlobal, ep->epNum);
        
        if(ep->bufLen >= cnt)
        {
            ep->bufLen -= cnt;
        }
        else
        {
            ep->bufLen = 0;
        }
        
        if(ep->bufLen == 0)
        {
            USBD_EP_SetRxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_NAK);
        }
        
        if(epStatus & USBD_EP_BIT_TXDTOG)
        {
            USBD_EP_ToggleTx(usbdh->usbGlobal, ep->epNum);
        }
        
        if(cnt)
        {
            USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                   ep->pmaAddr0, \
                                   ep->buffer, \
                                   cnt);
        }
    }
    /* Buffer1 */
    else
    {
        /* Buffer1 */
        cnt = USBD_EP_ReadRxCnt(usbdh->usbGlobal, ep->epNum);
        
        if(ep->bufLen >= cnt)
        {
            ep->bufLen -= cnt;
        }
        else
        {
            ep->bufLen = 0;
        }
        
        if(ep->bufLen == 0)
        {
            USBD_EP_SetRxStatus(usbdh->usbGlobal, ep->epNum, USBD_EP_STATUS_NAK);
        }
        
        if((epStatus & USBD_EP_BIT_TXDTOG) == 0)
        {
            USBD_EP_ToggleTx(usbdh->usbGlobal, ep->epNum);
        }
        
        if(cnt)
        {
            USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                   ep->pmaAddr1, \
                                   ep->buffer, \
                                   cnt);
        }
    }
    
    return cnt;
}

/*!
 * @brief     Handle USB device correct transfer interrupt
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
static void USBD_EP_CTRHandler(USBD_HANDLE_T* usbdh)
{
    USBD_ENDPOINT_INFO_T* ep;
    uint8_t epNum;
    uint8_t epDir;
    uint16_t epStatus;
    
    uint16_t bufCnt;
    uint16_t txBufCnt;
    
    while(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_CTR) == SET)
    {
        epNum = USBD_EP_ReadID(usbdh->usbGlobal);
        epDir = USBD_EP_ReadDir(usbdh->usbGlobal);
        
        /* EP0 */
        if(epNum == USBD_EP_0)
        {
            /* EP IN */
            if(epDir == 0)
            {
                USBD_EP_ResetTxFlag(usbdh->usbGlobal, USBD_EP_0);
                ep = &usbdh->epIN[USBD_EP_0];
                
                ep->bufCount = USBD_EP_ReadTxCnt(usbdh->usbGlobal, epNum);
                ep->buffer += ep->bufCount;
                
                /* IN stage */
                USBD_DataInStageCallback(usbdh, USBD_EP_0);
                
                if((ep->bufLen == 0) && (usbdh->address > 0))
                {
                    USBD_SetDeviceAddr(usbdh->usbGlobal, usbdh->address);
                    USBD_Enable(usbdh->usbGlobal);
                    usbdh->address = 0;
                }
            }
            else
            {
                ep = &usbdh->epOUT[USBD_EP_0];
                epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, USBD_EP_0);
                
                /* SETUP */
                if(epStatus & USBD_EP_BIT_SETUP)
                {
                    ep->bufCount = USBD_EP_ReadRxCnt(usbdh->usbGlobal, ep->epNum);
                    USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                           ep->pmaAddr, \
                                           (uint8_t *)usbdh->setup, \
                                           ep->bufCount);
                    
                    USBD_EP_ResetRxFlag(usbdh->usbGlobal, USBD_EP_0);
                    
                    USBD_SetupStageCallback(usbdh);
                }
                /* OUT or SETUP */
                else if(epStatus & USBD_EP_BIT_CTFR)
                {
                    USBD_EP_ResetRxFlag(usbdh->usbGlobal, USBD_EP_0);
                    
                    ep->bufCount = USBD_EP_ReadRxCnt(usbdh->usbGlobal, ep->epNum);
                    
                    if((ep->bufCount !=0) && (ep->buffer != 0))
                    {
                        USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                               ep->pmaAddr, \
                                               ep->buffer, \
                                               ep->bufCount);
                        
                        ep->buffer += ep->bufCount;
                        
                        USBD_DataOutStageCallback(usbdh, USBD_EP_0);
                    }
                    
                    epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, USBD_EP_0);
                    
                    if((epStatus & USBD_EP_BIT_SETUP) == 0)
                    {
                        USBD_EP_SetRxCnt(usbdh->usbGlobal, USBD_EP_0, ep->mps);
                        USBD_EP_SetRxStatus(usbdh->usbGlobal, USBD_EP_0, USBD_EP_STATUS_VALID);
                    }
                }
            }
        }
        else
        {
            epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, epNum);
            
            if(epStatus & USBD_EP_BIT_CTFR)
            {
                USBD_EP_ResetRxFlag(usbdh->usbGlobal, epNum);
                
                ep = &usbdh->epOUT[epNum];
                
                /* Single Buffer */
                if(ep->bufferStatus == USBD_EP_BUFFER_SINGLE)
                {
                    bufCnt = (uint16_t)USBD_EP_ReadRxCnt(usbdh->usbGlobal, epNum);
                    
                    if(bufCnt)
                    {
                        USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                               ep->pmaAddr, \
                                               ep->buffer, \
                                               bufCnt);
                    }
                }
                else
                {
                    /* DB bulk OUT */
                    if(ep->epType == EP_TYPE_BULK)
                    {
                        bufCnt = USBD_EP_DB_Receive(usbdh, ep, epStatus);
                    }
                    /* DB iso OUT */
                    else
                    {
                        USBD_EP_ToggleTx(usbdh->usbGlobal, ep->epNum);
                        
                        epStatus = USBD_EP_ReadStatus(usbdh->usbGlobal, epNum);
                        
                        if(epStatus & USBD_EP_BIT_RXDTOG)
                        {
                            /* Buffer0 */
                            bufCnt = (uint16_t)USBD_EP_ReadTxCnt(usbdh->usbGlobal, ep->epNum);
                            
                            if(bufCnt)
                            {
                                USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                                       ep->pmaAddr0, \
                                                       ep->buffer, \
                                                       bufCnt);
                            }
                        }
                        else
                        {
                            /* Buffer1 */
                            bufCnt = (uint16_t)USBD_EP_ReadRxCnt(usbdh->usbGlobal, ep->epNum);
                            
                            if(bufCnt)
                            {
                                USBD_EP_ReadPacketData(usbdh->usbGlobal, \
                                                       ep->pmaAddr1, \
                                                       ep->buffer, \
                                                       bufCnt);
                            }
                        }
                    }
                }
                
                /* Multi packets */
                ep->bufCount += bufCnt;
                ep->buffer += bufCnt;
                
                if((ep->bufLen == 0) || (bufCnt < ep->mps))
                {
                    USBD_DataOutStageCallback(usbdh, epNum);
                }
                else
                {
                    USBD_EP_XferStart(usbdh, ep);
                }
            }
            else if(epStatus & USBD_EP_BIT_CTFT)
            {
                USBD_EP_ResetTxFlag(usbdh->usbGlobal, epNum);
                
                ep = &usbdh->epIN[epNum];
                
                if((ep->epType == EP_TYPE_BULK) || \
                   (ep->epType == EP_TYPE_CONTROL) || \
                   ((ep->epType == EP_TYPE_INTERRUPT) && (epStatus & USBD_EP_BIT_KIND) == 0))
                {
                    txBufCnt = (uint16_t)USBD_EP_ReadTxCnt(usbdh->usbGlobal, ep->epNum);
                    
                    if(ep->bufLen > txBufCnt)
                    {
                        ep->bufLen -= txBufCnt;
                    }
                    else
                    {
                        ep->bufLen = 0;
                    }
                    
                    if(ep->bufLen == 0)
                    {
                        USBD_DataInStageCallback(usbdh, ep->epNum);
                    }
                    else
                    {
                        ep->buffer += txBufCnt;
                        ep->bufCount += txBufCnt;
                        USBD_EP_XferStart(usbdh, ep);
                    }
                }
                else
                {
                    USBD_EP_DB_Transmit(usbdh, ep, epStatus);
                }
            }
        }
    }
}

/*!
 * @brief     Handle USB device suspend status
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
static void USBD_SuspendHandler(USBD_HANDLE_T* usbdh)
{
    uint8_t i;
    uint16_t backupEP[8];
    
    for(i = 0; i < 8; i++)
    {
        backupEP[i] = (uint16_t)usbdh->usbGlobal->EP[i].EP;
    }
    
    USBD_SetForceReset(usbdh->usbGlobal);
    USBD_ResetForceReset(usbdh->usbGlobal);
    
    while (USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_RST) == RESET);
    
    USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_RST);
    
    for(i = 0; i < 8; i++)
    {
        usbdh->usbGlobal->EP[i].EP = backupEP[i];
    }
    
    USBD_SetForceSuspend(usbdh->usbGlobal);
    
    USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_SUS);
    
    /* Enter low power mode */
    USBD_SetLowerPowerMode(usbdh->usbGlobal);
}

/*!
 * @brief     USB device set device address
 *
 * @param     usbdh: USB device handler
 *
 * @param     address: address
 *
 * @retval    None
 */
void USBD_SetDevAddress(USBD_HANDLE_T* usbdh, uint8_t address)
{
    usbdh->address = address;

    if(address == 0)
    {
        USBD_SetDeviceAddr(usbdh->usbGlobal, address);
        USBD_Enable(usbdh->usbGlobal);
    }
}

/*!
 * @brief     USB device configure PMA
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @param     bufferStatus: endpoint kind
 *
 * @param     pmaAddr: PMA address
 *
 * @retval    None
 */
void USBD_ConfigPMA(USBD_HANDLE_T* usbdh, uint16_t epAddr, uint16_t bufferStatus, uint32_t pmaAddr)
{
    USBD_ENDPOINT_INFO_T *ep;
    
    if((epAddr & 0x80) == 0x80)
    {
        ep = &usbdh->epIN[epAddr & 0x07];
    }
    else
    {
        ep = &usbdh->epOUT[epAddr];
    }
    
    if(bufferStatus == USBD_EP_BUFFER_SINGLE)
    {
        ep->bufferStatus = USBD_EP_BUFFER_SINGLE;
        ep->pmaAddr = (uint16_t)pmaAddr;
    }
    else
    {
        ep->bufferStatus = USBD_EP_BUFFER_DOUBLE;
        ep->pmaAddr0 = (uint16_t)(pmaAddr & 0xFFFF);
        ep->pmaAddr1 = (uint16_t)((pmaAddr & 0xFFFF0000) >> 16);
    }
}

/*!
 * @brief     Config the USB device peripheral according to the specified parameters
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Config(USBD_HANDLE_T* usbdh)
{
    uint8_t i;
    
    /* Init control endpoint structure */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* OUT control endpoint */
        usbdh->epOUT[i].epNum       = i;
        usbdh->epOUT[i].epDir       = EP_DIR_OUT;

        usbdh->epOUT[i].epType      = EP_TYPE_CONTROL;
        usbdh->epOUT[i].mps         = 0;
        usbdh->epOUT[i].buffer      = 0;
        usbdh->epOUT[i].bufLen      = 0;

        /* IN control endpoint */
        usbdh->epIN[i].epNum        = i;
        usbdh->epIN[i].epDir        = EP_DIR_IN;
        usbdh->epIN[i].txFifoNum    = i;

        usbdh->epIN[i].epType       = EP_TYPE_CONTROL;
        usbdh->epIN[i].mps          = 0;
        usbdh->epIN[i].buffer       = 0;
        usbdh->epIN[i].bufLen       = 0;
    }

    /* Init address */
    usbdh->address = 0;
    
    USBD_SetForceSuspend(usbdh->usbGlobal);
    
    usbdh->usbGlobal->CTRL = 0;
    
    usbdh->usbGlobal->INTSTS = 0;
    
    usbdh->usbGlobal->BUFFTB = USBD_BUFFTB_ADDR;
}

/*!
 * @brief     Handle USB device wakeup interrupt
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_WakeupIsrHandler(USBD_HANDLE_T* usbdh)
{
    EINT->IPEND = USBD_WAKEUP_EINT_LINE;
}

/*!
 * @brief     Handle USB device global interrupt
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_IsrHandler(USBD_HANDLE_T* usbdh)
{
    /* Handle Correct Transfer */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_CTR))
    {
        USBD_EP_CTRHandler(usbdh);
    }
    
    /* Handle USB Reset interrupt */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_RST))
    {
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_RST);
        
        USBD_EnumDoneCallback(usbdh);
        
        USBD_SetDevAddress(usbdh, 0x00);
    }
    
    /* Handle Packet Memory Overflow */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_PMAOU))
    {
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_PMAOU);
    }
    
    /* Handle Failure Of Transfer */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_ERR))
    {
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_ERR);
    }
    
    /* Handle Wakeup Request */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_WKUP))
    {
        
        USBD_ResetLowerPowerMode(usbdh->usbGlobal);
        USBD_ResetForceSuspend(usbdh->usbGlobal);
        
        USBD_ResumeCallback(usbdh);
        
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_WKUP);
    }
    
    /* Handle Suspend Mode Request */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_SUS))
    {
        USBD_SuspendHandler(usbdh);
        
        USBD_SuspendCallback(usbdh);
    }
    
    /* Handle Start Of Frame */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_SOF))
    {
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_SOF);
        
        USBD_SOFCallback(usbdh);
    }
    
    /* Handle Expected Start of Frame */
    if(USBD_ReadIntFlag(usbdh->usbGlobal, USBD_INT_ESOF))
    {
        USBD_ClearIntFlag(usbdh->usbGlobal, USBD_INT_ESOF);
    }
}

#endif /* defined (USB_DEVICE) */

#if defined (USB_OTG)

/*!
 * @brief       Handle EP0 OUT SETUP transfer
 *
 * @param       usbdh: USB device handler.
 *
 * @param       dmaStatus: DMA status
 *
 * @param       setup: setup packet
 *
 * @retval      None
 */
void USBD_EP0_OutHandler(USBD_HANDLE_T* usbdh, uint8_t dmaStatus, uint8_t* setup)
{
//    uint32_t usbCoreID = *(__IO uint32_t*)(&usbdh->usbGlobal->GCID + 0x01);

    /* Configure EP0 */
    usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPTRS = 0;
    usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPTRS_B.EPPCNT = BIT_SET;
    usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPTRS_B.PID_SPCNT = 0x03;
    usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPTRS_B.EPTRS = 24;

    if (dmaStatus == ENABLE)
    {
        usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPDMA = (uint32_t)setup;

        /* Enable endpoint */
        usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPCTRL_B.USBAEP = BIT_SET;
        usbdh->usbDevice->EP_OUT[USBD_EP_0].DOEPCTRL_B.EPEN = BIT_SET;
    }
}

/*!
 * @brief       Handle EP OUT transfer complete interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @param       epNum: endpoint number
 *
 * @retval      None
 */
static void USBD_EP_OutXferHandler(USBD_HANDLE_T* usbdh, uint32_t epNum)
{
//    uint32_t usbCoreID = *(__IO uint32_t*)(&usbdh->usbGlobal->GCID + 0x01);
    uint8_t setupStatus = usbdh->usbDevice->EP_OUT[epNum].DOEPINT_B.SETPCMP;
    uint8_t rxOutDisStatus = usbdh->usbDevice->EP_OUT[epNum].DOEPINT_B.RXOTDIS;
    uint8_t xferSize = usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS;

    if (usbdh->usbCfg.dmaStatus == DISABLE)
    {
        /* Make sure is EP0 */
        if ((epNum == 0) && (usbdh->epOUT[epNum].bufLen == 0))
        {
            USBD_EP0_OutHandler(usbdh, DISABLE, (uint8_t*)usbdh->setup);
        }

        /* OUT data stage */
        USBD_DataOutStageCallback(usbdh, epNum);
    }
    else
    {
        /* SETUP stage is Done */
        if (setupStatus)
        {

        }
        else if (rxOutDisStatus)
        {
            usbdh->usbDevice->EP_OUT[epNum].DOEPINT_B.RXOTDIS = BIT_SET;
        }
        else if ((setupStatus | rxOutDisStatus) == 0)
        {
            /* receive OUT data packet on EP0 */
            usbdh->epOUT[epNum].bufCount = usbdh->epOUT[epNum].mps - xferSize;

            usbdh->epOUT[epNum].buffer += usbdh->epOUT[epNum].mps;

            /* Make sure is EP0 */
            if ((epNum == 0) && (usbdh->epOUT[epNum].bufLen == 0))
            {
                USBD_EP0_OutHandler(usbdh, ENABLE, (uint8_t*)usbdh->setup);
            }

            /* OUT data stage */
            USBD_DataOutStageCallback(usbdh, epNum);
        }
        else
        {

        }
    }
}

/*!
 * @brief       Handle EP OUT SETUP transfer interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @param       epNum: endpoint number
 *
 * @retval      None
 */
static void USBD_EP_OutSetupHandler(USBD_HANDLE_T* usbdh, uint32_t epNum)
{
//    uint32_t usbCoreID = *(__IO uint32_t*)(&usbdh->usbGlobal->GCID + 0x01);

    USBD_SetupStageCallback(usbdh);
}

/*!
 * @brief       Prepare empty TX FIFO
 *
 * @param       usbdh: USB device handler.
 *
 * @param       epNum: endpoint number
 *
 * @retval      None
 */
void USBD_EP_PushDataEmptyTxFifo(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    USB_OTG_ENDPOINT_INFO_T* ep;
    uint32_t lengthTemp;
    uint32_t length;

    ep = &usbdh->epIN[epNum];

    if (ep->bufCount > ep->bufLen)
    {
        return;
    }

    length = ep->bufLen - ep->bufCount;

    if (length > ep->mps)
    {
        length = ep->mps;
    }

    lengthTemp = (length + 3) / 4;

    while ((usbdh->usbDevice->EP_IN[epNum].DITXFSTS_B.INEPTXFSA >= lengthTemp) && \
            (ep->bufCount < ep->bufLen) && \
            (ep->bufLen != 0))
    {
        length = ep->bufLen - ep->bufCount;

        if (length > ep->mps)
        {
            length = ep->mps;
        }

        lengthTemp = (length + 3) / 4;

        USB_OTG_FIFO_WriteFifoPacket(usbdh->usbFifo, (uint8_t)epNum, ep->buffer, \
                                     (uint16_t)length, \
                                     usbdh->usbCfg.dmaStatus);

        ep->buffer      += length;
        ep->bufCount    += length;
    }

    if (ep->bufLen <= ep->bufCount)
    {
        USB_OTG_D_DisableInEpEmptyInterrupt(usbdh->usbDevice, epNum);
    }
}

/*!
 * @brief     USB device read EP last receive data size
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr: endpoint address
 *
 * @retval    size of last receive data
 */
uint32_t USBD_EP_ReadRxDataLen(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    return usbdh->epOUT[epAddr & 0x0F].bufCount;
}

/*!
 * @brief     USB device EP start transfer
 *
 * @param     usbdh: USB device handler.
 *
 * @param     endpoint : endpoint handler
 *
 * @param     dmaStatus : DMA status
 *
 * @retval    None
 */
void USBD_EP_XferStart(USBD_HANDLE_T* usbdh, USB_OTG_ENDPOINT_INFO_T* endpoint, uint8_t dmaStatus)
{
    uint8_t epNum = endpoint->epNum;
    uint8_t epDir = endpoint->epDir;
    uint16_t epPacketCnt;

    if (epDir == EP_DIR_OUT)
    {
        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPPCNT = BIT_RESET;
        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS = BIT_RESET;

        if (endpoint->bufLen)
        {
            epPacketCnt = (uint16_t)((endpoint->bufLen + endpoint->mps - 1) / (endpoint->mps));
            usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPPCNT = epPacketCnt;
            usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS = endpoint->mps * epPacketCnt;
        }
        else
        {
            usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPPCNT = 0x01;
            usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS = endpoint->mps;
        }

        if (dmaStatus == ENABLE)
        {
            if ((uint32_t)endpoint->buffer != 0)
            {
                usbdh->usbDevice->EP_OUT[epNum].DOEPDMA = (uint32_t)endpoint->buffer;
            }
        }

        if (endpoint->epType == EP_TYPE_ISO)
        {
            if (usbdh->usbDevice->DSTS_B.SOFNUM & 0x01)
            {
                usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.OFSET = BIT_SET;
            }
            else
            {
                usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.DPIDSET = BIT_SET;
            }
        }

        /* Enable endpoint */
        usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.NAKCLR = BIT_SET;
        usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.EPEN = BIT_SET;
    }
    else
    {
        /* Handle 0 length packet */
        if (endpoint->bufLen == 0)
        {
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = BIT_RESET;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = BIT_RESET;

            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = 0x01;
        }
        else
        {
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = BIT_RESET;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = BIT_RESET;

            epPacketCnt = (uint16_t)((endpoint->bufLen + endpoint->mps - 1) / (endpoint->mps));
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = epPacketCnt;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = endpoint->bufLen;

            if (endpoint->epType == EP_TYPE_ISO)
            {
                usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.TXDCNT = BIT_RESET;
                usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.TXDCNT = 0x01;
            }
        }

        if (dmaStatus == ENABLE)
        {
            if ((uint32_t)endpoint->dmaAddr != 0)
            {
                usbdh->usbDevice->EP_IN[epNum].DIEPDMA = (uint32_t)endpoint->dmaAddr;
            }

            if (endpoint->epType == EP_TYPE_ISO)
            {
                if (usbdh->usbDevice->DSTS_B.SOFNUM & 0x01)
                {
                    usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.OFSET = BIT_SET;
                }
                else
                {
                    usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.DPIDSET = BIT_SET;
                }
            }

            /* Enable endpoint */
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.NAKCLR = BIT_SET;
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.EPEN = BIT_SET;
        }
        else
        {
            /* Enable endpoint */
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.NAKCLR = BIT_SET;
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.EPEN = BIT_SET;

            if (endpoint->epType == EP_TYPE_ISO)
            {
                if (usbdh->usbDevice->DSTS_B.SOFNUM & 0x01)
                {
                    usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.OFSET = BIT_SET;
                }
                else
                {
                    usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.DPIDSET = BIT_SET;
                }

                USB_OTG_FIFO_WriteFifoPacket(usbdh->usbFifo, endpoint->epNum, \
                                             endpoint->buffer, endpoint->bufLen, \
                                             dmaStatus);
            }
            else
            {
                if (endpoint->bufLen)
                {
                    USB_OTG_D_EnableInEpEmptyInterrupt(usbdh->usbDevice, epNum);
                }
            }
        }
    }
}

/*!
 * @brief     USB device EP0 start transfer
 *
 * @param     usbdh: USB device handler.
 *
 * @param     endpoint : endpoint handler
 *
 * @param     dmaStatus : DMA status
 *
 * @retval    None
 */
void USBD_EP0_XferStart(USBD_HANDLE_T* usbdh, USB_OTG_ENDPOINT_INFO_T* endpoint, uint8_t dmaStatus)
{
    uint8_t epNum = endpoint->epNum;

    uint8_t epDir = endpoint->epDir;

    if (epDir == EP_DIR_OUT)
    {
        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPPCNT = BIT_RESET;
        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS = BIT_RESET;

        if (endpoint->bufLen)
        {
            endpoint->bufLen = endpoint->mps;
        }

        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPPCNT = BIT_SET;
        usbdh->usbDevice->EP_OUT[epNum].DOEPTRS_B.EPTRS = endpoint->mps;

        if (dmaStatus == ENABLE)
        {
            if ((uint32_t)endpoint->buffer != 0)
            {
                usbdh->usbDevice->EP_OUT[epNum].DOEPDMA = (uint32_t)endpoint->buffer;
            }
        }

        /* Enable endpoint */
        usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.NAKCLR = BIT_SET;
        usbdh->usbDevice->EP_OUT[epNum].DOEPCTRL_B.EPEN = BIT_SET;
    }
    else
    {
        /* Handle 0 length packet */
        if (endpoint->bufLen == 0)
        {
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = BIT_RESET;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = BIT_RESET;

            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = 0x01;
        }
        else
        {
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = BIT_RESET;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = BIT_RESET;

            if (endpoint->bufLen > endpoint->mps)
            {
                endpoint->bufLen = endpoint->mps;
            }

            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPPCNT = 0x01;
            usbdh->usbDevice->EP_IN[epNum].DIEPTRS_B.EPTRS = endpoint->bufLen;
        }

        if (dmaStatus == ENABLE)
        {
            if (endpoint->dmaAddr != 0)
            {
                usbdh->usbDevice->EP_IN[epNum].DIEPDMA = (uint32_t)(endpoint->dmaAddr);
            }

            /* Enable endpoint */
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.NAKCLR = BIT_SET;
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.EPEN = BIT_SET;
        }
        else
        {
            /* Enable endpoint */
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.NAKCLR = BIT_SET;
            usbdh->usbDevice->EP_IN[epNum].DIEPCTRL_B.EPEN = BIT_SET;

            if (endpoint->bufLen)
            {
                USB_OTG_D_EnableInEpEmptyInterrupt(usbdh->usbDevice, epNum);
            }
        }
    }
}

/*!
 * @brief     USB device EP receive handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    None
 */
void USBD_EP_Receive(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                     uint8_t* buffer, uint32_t length)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    usbdh->epOUT[epAddrTemp].epNum = epAddr & 0x0F;
    usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;

    usbdh->epOUT[epAddrTemp].buffer = buffer;
    usbdh->epOUT[epAddrTemp].bufCount = 0;
    usbdh->epOUT[epAddrTemp].bufLen = length;

    if (epAddr & 0x0F)
    {
        USBD_EP_XferStart(usbdh, &usbdh->epOUT[epAddrTemp], usbdh->usbCfg.dmaStatus);
    }
    else
    {
        USBD_EP0_XferStart(usbdh, &usbdh->epOUT[epAddrTemp], usbdh->usbCfg.dmaStatus);
    }
}

/*!
 * @brief     USB device EP transfer handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    None
 */
void USBD_EP_Transfer(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                      uint8_t* buffer, uint32_t length)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    usbdh->epIN[epAddrTemp].epNum = epAddr & 0x0F;
    usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;

    usbdh->epIN[epAddrTemp].buffer = buffer;
    usbdh->epIN[epAddrTemp].bufCount = 0;
    usbdh->epIN[epAddrTemp].bufLen = length;

    if (epAddr & 0x0F)
    {
        USBD_EP_XferStart(usbdh, &usbdh->epIN[epAddrTemp], usbdh->usbCfg.dmaStatus);
    }
    else
    {
        USBD_EP0_XferStart(usbdh, &usbdh->epIN[epAddrTemp], usbdh->usbCfg.dmaStatus);
    }
}

/*!
 * @brief     USB device flush EP handler
 *
 * @param     usbdh: USB device handler.
 *
 * @param     epAddr : endpoint address
 *
 * @retval    usb device status
 */
void USBD_EP_Flush(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    if (epAddr & 0x80)
    {
        USB_OTG_FlushTxFIFO(usbdh->usbGlobal, (uint8_t)(epAddr & 0x0F));
    }
    else
    {
        USB_OTG_FlushRxFIFO(usbdh->usbGlobal);
    }
}

/*!
 * @brief       Handle RxFIFO no empty interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_RxFifoNoEmptyIsrHandler(USBD_HANDLE_T* usbdh)
{
    USBD_FIFO_STA_T fifoStatus;
    uint8_t epNum;
    uint8_t packetStatus;
    uint16_t packetCnt;

    /* Disable RxFIFO no empty Interrup */
    USB_OTG_DisableGlobalInterrupt(usbdh->usbGlobal, USB_INT_G_RXFNONE);

    /* Read and pop the RxFIFO status data */
    fifoStatus.FIFO_STATUS = USB_OTG_PopRxFifoStatus(usbdh->usbGlobal);
    epNum = fifoStatus.FIFO_STATUS_B.epNum;
    packetStatus = fifoStatus.FIFO_STATUS_B.packetStatus;
    packetCnt = fifoStatus.FIFO_STATUS_B.byteCount;

    switch (packetStatus)
    {
        case USBD_PKTSTS_G_OUT_NAK:
            break;

        case USBD_PKTSTS_REV_DATA_OUT_PKT:
            /* Receive OUT data packet */
            if ((packetCnt != 0) && (usbdh->epOUT[epNum].buffer != NULL))
            {
                USB_OTG_FIFO_ReadRxFifoPacket(usbdh->usbFifo, \
                                              usbdh->epOUT[epNum].buffer, \
                                              packetCnt);

                usbdh->epOUT[epNum].buffer += packetCnt;
                usbdh->epOUT[epNum].bufCount += packetCnt;
            }
            break;

        case USBD_PKTSTS_OUT_DONE:
            break;

        case USBD_PKTSTS_SETUP_DONE:
            break;

        case USBD_PKTSTS_REV_SETUP_PKT:
            USB_OTG_FIFO_ReadRxFifoPacket(usbdh->usbFifo, \
                                          (uint8_t*)usbdh->setup, \
                                          8);

            usbdh->epOUT[epNum].bufCount += packetCnt;
            break;

        default:
            break;
    }

    /* Enable RxFIFO no empty Interrupt */
    USB_OTG_EnableGlobalInterrupt(usbdh->usbGlobal, USB_INT_G_RXFNONE);
}

/*!
 * @brief       Handle OUT endpiont interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_OutEndpointIsrHandler(USBD_HANDLE_T* usbdh)
{
    uint16_t epIntBits;
    uint8_t epNum;

    /* Read all device endpoint interrupt bit */
    epIntBits = USB_OTG_D_ReadAllOutEpIntNumber(usbdh->usbDevice);

    for (epNum = 0; epNum < 16; epNum++)
    {
        if (!(epIntBits >> epNum))
        {
            break;
        }
        /* Find out all the interrupt OUT endpoint */
        else if ((epIntBits >> epNum) & 0x01)
        {
            /* Transfer completed */
            if (USB_OTG_D_ReadOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_TSFCMP))
            {
                USB_OTG_D_ClearOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_TSFCMP);

                /* Handle OUT transfer EP */
                USBD_EP_OutXferHandler(usbdh, epNum);
            }

            /* Setup completed */
            if (USB_OTG_D_ReadOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_SETPCMP))
            {
                USB_OTG_D_ClearOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_SETPCMP);

                /* Handle SETUP transfer */
                USBD_EP_OutSetupHandler(usbdh, epNum);
            }

            /* OUT token received when endpoint disabled */
            if (USB_OTG_D_ReadOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_RXOTDIS))
            {
                USB_OTG_D_ClearOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_RXOTDIS);
            }

            /* Endpoint disable */
            if (USB_OTG_D_ReadOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_EPDIS))
            {
                USB_OTG_D_ClearOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_EPDIS);
            }

            /* Received Back-to-back SETUP packets over 3 */
            if (USB_OTG_D_ReadOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_RXBSP))
            {
                USB_OTG_D_ClearOutEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_OUT_RXBSP);
            }
        }
    }
}

/*!
 * @brief       Handle IN endpiont interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_InEndpointIsrHandler(USBD_HANDLE_T* usbdh)
{
    uint16_t epIntBits;
    uint8_t epNum;

    /* Read all device endpoint interrupt bit */
    epIntBits = USB_OTG_D_ReadAllInEpIntNumber(usbdh->usbDevice);

    for (epNum = 0; epNum < 16; epNum++)
    {
        if (!(epIntBits >> epNum))
        {
            break;
        }
        /* Find out all the interrupt IN endpoint */
        else if ((epIntBits >> epNum) & 0x01)
        {
            /* Transfer completed */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_TSFCMP))
            {
                USB_OTG_D_DisableInEpEmptyInterrupt(usbdh->usbDevice, epNum & 0x0F);

                USB_OTG_D_ClearInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_TSFCMP);

                if (usbdh->usbCfg.dmaStatus == ENABLE)
                {
                    usbdh->epIN[epNum].buffer += usbdh->epIN[epNum].mps;

                    /* Prepare EP0 for next setup */
                    if ((usbdh->epIN[epNum].bufLen == 0) && \
                            (epNum == USBD_EP_0))
                    {
                        /* prepare to receive next setup packets */
                        USBD_EP0_OutHandler(usbdh, usbdh->usbCfg.dmaStatus, (uint8_t*)usbdh->setup);
                    }
                }

                /* Callback DATA IN stage */
                USBD_DataInStageCallback(usbdh, epNum);
            }

            /* Timeout */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_TO))
            {
                USB_OTG_D_ClearInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_TO);
            }

            /* IN token received when TxFIFO is empty */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_ITXEMP))
            {
                USB_OTG_D_ClearInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_ITXEMP);
            }

            /* IN endpoint NAK effective */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_IEPNAKE))
            {
                USB_OTG_D_ClearInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_IEPNAKE);
            }

            /* Endpoint disabled */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_EPDIS))
            {
                USB_OTG_D_ClearInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_EPDIS);
            }

            /* TxFIFO empty */
            if (USB_OTG_D_ReadInEpIntStatus(usbdh->usbDevice, epNum, USBD_INT_EP_IN_TXFE))
            {
                USBD_EP_PushDataEmptyTxFifo(usbdh, epNum);
            }
        }
    }
}

/*!
 * @brief       Handle USB Resume interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_ResumeIsrHandler(USBD_HANDLE_T* usbdh)
{
    USB_OTG_D_DisableRemoteWakeupSignal(usbdh->usbDevice);

    /* Add LP mode handle */

    /* Resume Callback */
    USBD_ResumeCallback(usbdh);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_RWAKE);
}

/*!
 * @brief       Handle USB Suspend interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_SuspendIsrHandler(USBD_HANDLE_T* usbdh)
{
    if (usbdh->usbDevice->DSTS_B.SUSSTS)
    {
        /* Suspend Callback */
        USBD_SuspendCallback(usbdh);
    }

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_USBSUS);
}

/*!
 * @brief       Handle USB Reset interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_ResetIsrHandler(USBD_HANDLE_T* usbdh)
{
    uint8_t i;

    USB_OTG_D_DisableRemoteWakeupSignal(usbdh->usbDevice);

    /* Flush all Tx FIFOs */
    USB_OTG_FlushTxFIFO(usbdh->usbGlobal, 0x10);

    /* Init endpoint */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* Set OUT endpoint */
        usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.STALLH = BIT_RESET;
        usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.NAKSET = BIT_SET;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_OUT[i].DOEPINT = 0xFB7F;

        /* Set IN endpoint */
        usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.STALLH = BIT_RESET;
        usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.NAKSET = BIT_SET;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_IN[i].DIEPINT = 0xFB7F;
    }

    /* Enable enpoint 0 global interrupt */
    USB_OTG_D_EnableOutEpInterruptMask(usbdh->usbDevice, USBD_EP_0);
    USB_OTG_D_EnableInEpInterruptMask(usbdh->usbDevice, USBD_EP_0);

    /* Only use in HS */
    if (usbdh->usbCfg.ep1Status == ENABLE)
    {
        /* IN EP1 */
        USB_OTG_D_EnableInEp1Interrupt(usbdh->usbDevice, \
                                       USBD_INT_EP_IN_TSFCMP    |
                                       USBD_INT_EP_IN_EPDIS     |
                                       USBD_INT_EP_IN_TO);



        /* OUT EP1 */
        USB_OTG_D_EnableOutEp1Interrupt(usbdh->usbDevice, \
                                        USBD_INT_EP_OUT_TSFCMP  |
                                        USBD_INT_EP_OUT_EPDIS   |
                                        USBD_INT_EP_OUT_SETPCMP);
    }
    else
    {
        /* IN EP */
        USB_OTG_D_EnableInEpInterrupt(usbdh->usbDevice, \
                                      USBD_INT_EP_IN_TSFCMP |
                                      USBD_INT_EP_IN_EPDIS  |
                                      USBD_INT_EP_IN_TO);

        /* OUT EP */
        USB_OTG_D_EnableOutEpInterrupt(usbdh->usbDevice, \
                                       USBD_INT_EP_OUT_TSFCMP   |
                                       USBD_INT_EP_OUT_EPDIS    |
                                       USBD_INT_EP_OUT_SETPCMP  |
                                       USBD_INT_EP_OUT_RXOTPR   |
                                       USBD_INT_EP_OUT_NAK);
    }

    /* Set device address to 0 */
    USB_OTG_D_ConfigDeviceAddress(usbdh->usbDevice, (0x00 & 0x7F));

    /* setup EP0 to receive SETUP packet */
    USBD_EP0_OutHandler(usbdh, usbdh->usbCfg.dmaStatus, (uint8_t*)usbdh->setup);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_USBRST);
}

/*!
 * @brief       Handle USB Enum Done interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_EnumDoneIsrHandler(USBD_HANDLE_T* usbdh)
{
    uint32_t enumSpeed;

    /* Active EP0 for SETUP */
    usbdh->usbDevice->EP_IN[0].DIEPCTRL_B.MAXPS = USDB_EP0_MAXPS_64_BYTES;

    /* Clear all NAK */
    usbdh->usbDevice->DCTRL_B.GINAKCLR = BIT_SET;

    /* Store USB device speed */
    enumSpeed = USB_OTG_D_ReadEnumSpeed(usbdh->usbDevice);

    switch (enumSpeed)
    {
        case USBD_ENUM_SPEED_HS:
            usbdh->usbCfg.speed = USB_SPEED_HSFSLS;
            break;

        case USBD_ENUM_SPEED_HS_IN_FS:
        case USBD_ENUM_SPEED_FS:
            usbdh->usbCfg.speed = USB_SPEED_FSLS;
            break;

        default:
            /* Speed error status */
            usbdh->usbCfg.speed = 0xFF;
            break;
    }

    /* Set turnaround time */
    USB_OTG_SetTurnaroundTime(usbdh->usbGlobal, RCM_ReadHCLKFreq(), usbdh->usbCfg.speed);

    USBD_EnumDoneCallback(usbdh);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_ENUMD);
}

/*!
 * @brief       Handle USB Start of Frame interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_SofIsrHanlder(USBD_HANDLE_T* usbdh)
{
    USBD_SOFCallback(usbdh);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_SOF);
}

/*!
 * @brief       Handle USB incomplete ISO IN interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_IsoInInCompleteIsrHanlder(USBD_HANDLE_T* usbdh)
{
    uint8_t epNum = 0;

    USBD_IsoInInCompleteCallback(usbdh, epNum);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_IIINTX);
}

/*!
 * @brief       Handle USB incomplete ISO OUT interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_IsoOutInCompleteIsrHanlder(USBD_HANDLE_T* usbdh)
{
    uint8_t epNum = 0;

    USBD_IsoOutInCompleteCallback(usbdh, epNum);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_IP_OUTTX);
}

/*!
 * @brief       Handle USB New Session detected interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_NewSessionIsrHandler(USBD_HANDLE_T* usbdh)
{
    /* Connect callback */
    USBD_ConnectCallback(usbdh);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_SREQ);
}

/*!
 * @brief       Handle USB OTG interrupt
 *
 * @param       usbdh: USB device handler.
 *
 * @retval      None
 */
static void USBD_OTGIsrHandler(USBD_HANDLE_T* usbdh)
{
    /* Session end */
    if (usbdh->usbGlobal->GINT_B.SEFLG == BIT_SET)
    {
        /* Disconnect callback */
        USBD_DisconnectCallback(usbdh);

        usbdh->usbGlobal->GINT_B.SEFLG = BIT_SET;
    }
}

/*!
 * @brief     Handle USB device global interrupt
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_OTG_IsrHandler(USBD_HANDLE_T* usbdh)
{
    if (USB_OTG_ReadMode(usbdh->usbGlobal) == USB_OTG_MODE_DEVICE)
    {
        /* Avoid spurious interrupt */
        if (USB_OTG_ReadInterrupts(usbdh->usbGlobal) == 0U)
        {
            return;
        }

        /* Handle Mode Mismatch Interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_MMIS))
        {
            /* Clear interrupt */
            USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_MMIS);
        }

        /* Handle RxFIFO no empty interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_RXFNONE))
        {
            USBD_RxFifoNoEmptyIsrHandler(usbdh);
        }

        /* Handle OUT Endpoint interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_ONEP))
        {
            USBD_OutEndpointIsrHandler(usbdh);
        }

        /* Handle IN Endpoint interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_INEP))
        {
            USBD_InEndpointIsrHandler(usbdh);
        }

        /* Handle USB Reset interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_USBRST))
        {
            USBD_ResetIsrHandler(usbdh);
        }

        /* Handle Resume/remote wakeup detected interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_RWAKE))
        {
            USBD_ResumeIsrHandler(usbdh);
        }

        /* Handle USB suspend interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_USBSUS))
        {
            USBD_SuspendIsrHandler(usbdh);
        }

        /* Handle Enumeration done interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_ENUMD))
        {
            USBD_EnumDoneIsrHandler(usbdh);
        }

        /* Handle SOF interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_SOF))
        {
            USBD_SofIsrHanlder(usbdh);
        }

        /* Handle Incomplete ISO IN interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_IIINTX))
        {
            USBD_IsoInInCompleteIsrHanlder(usbdh);
        }

        /* Handle Incomplete ISO OUT interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_IP_OUTTX))
        {
            USBD_IsoOutInCompleteIsrHanlder(usbdh);
        }

        /* Handle Session request/new session detected interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_SREQ))
        {
            USBD_NewSessionIsrHandler(usbdh);
        }

        /* Handle OTG interrupt */
        if (USB_OTG_ReadGlobalIntFlag(usbdh->usbGlobal, USB_INT_G_OTG))
        {
            USBD_OTGIsrHandler(usbdh);
        }
    }
}

/*!
 * @brief     Config Depth and start address of TxFifo
 *
 * @param     usbdh: USB device handler
 *
 * @param     epInNum : IN EP number
 *
 * @param     depth : depth of TxFIFO
 *
 * @retval    None
 */
void USBD_OTG_ConfigDeviceTxFifo(USBD_HANDLE_T* usbdh, uint8_t epInNum, uint16_t depth)
{
    uint32_t txFifoConfig = (((uint32_t)depth) << 16);
    uint32_t txOffset;
    uint8_t i;

    txOffset = usbdh->usbGlobal->GRXFIFO;

    if (epInNum)
    {
        txOffset += (usbdh->usbGlobal->GTXFCFG) >> 16;

        for (i = 0; i < epInNum - 1; i++)
        {
            txOffset += (usbdh->usbGlobal->DTXFIFO[i].word >> 16);
        }

        usbdh->usbGlobal->DTXFIFO[epInNum - 1].word = txFifoConfig | txOffset;
    }
    else
    {
        usbdh->usbGlobal->GTXFCFG = txFifoConfig | txOffset;
    }
}

/*!
 * @brief     Config the USB device peripheral according to the specified parameters
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Config(USBD_HANDLE_T* usbdh)
{
    uint8_t i;

    /* FS PHY */
    USB_OTG_ConfigPHY(usbdh->usbGlobal, USB_OTG_PHY_SP_FS);

    /* Reset core */
    USB_OTG_CoreReset(usbdh->usbGlobal);
    APM_DelayMs(50);

    /* Activate the power down*/
    USB_OTG_DisablePowerDown(usbdh->usbGlobal);

    /* Device mode or host mode */
    USB_OTG_ConfigMode(usbdh->usbGlobal, usbdh->usbCfg.mode);

    /* Init control endpoint structure */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* OUT control endpoint */
        usbdh->epOUT[i].epNum       = i;
        usbdh->epOUT[i].epDir       = EP_DIR_OUT;

        usbdh->epOUT[i].epType      = EP_TYPE_CONTROL;
        usbdh->epOUT[i].mps         = 0;
        usbdh->epOUT[i].buffer      = 0;
        usbdh->epOUT[i].bufLen      = 0;

        /* IN control endpoint */
        usbdh->epIN[i].epNum        = i;
        usbdh->epIN[i].epDir        = EP_DIR_IN;
        usbdh->epIN[i].txFifoNum    = i;

        usbdh->epIN[i].epType       = EP_TYPE_CONTROL;
        usbdh->epIN[i].mps          = 0;
        usbdh->epIN[i].buffer       = 0;
        usbdh->epIN[i].bufLen       = 0;
    }

    /* Init address */
    usbdh->address = 0;

    /* Init device control register */

    /* Clear IN endpoint FIFO */
    for (i = 0; i < 0x0F; i++)
    {
        usbdh->usbGlobal->DTXFIFO[i].word = 0;
    }

    /* Configure VBUS sense */
    usbdh->usbGlobal->GGCCFG_B.BDVBSEN = BIT_SET;

    /* PHY Clock restart */
    usbdh->usbPower->PCGCTRL = 0;

    /* Configure device parameters */
    USB_OTG_D_ConfigPeriodicFrameInterval(usbdh->usbDevice, USBD_FRAME_INTERVAL_80);

    /* FS PHY */
    USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_FS);

    /* Flush all Tx and Rx FIFOs */
    USB_OTG_FlushTxFIFO(usbdh->usbGlobal, 0x10);
    USB_OTG_FlushRxFIFO(usbdh->usbGlobal);

    USB_OTG_D_DisableAllInEpInterrupt(usbdh->usbDevice);
    USB_OTG_D_DisableAllOutEpInterrupt(usbdh->usbDevice);
    USB_OTG_D_DisableAllEpInterrupt(usbdh->usbDevice);

    /* Init endpoint */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* Reset OUT endpoint */
        if (usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.EPEN)
        {
            if (i != 0)
            {
                usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.EPDIS = BIT_SET;
            }

            usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            usbdh->usbDevice->EP_OUT[i].DOEPCTRL = 0;
        }

        /* Reset transfer size */
        usbdh->usbDevice->EP_OUT[i].DOEPTRS = 0;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_OUT[i].DOEPINT = 0xFB7F;

        /* Reset IN endpoint */
        if (usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.EPEN)
        {
            if (i != 0)
            {
                usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.EPDIS = BIT_SET;
            }

            usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            usbdh->usbDevice->EP_IN[i].DIEPCTRL = 0;
        }

        /* Reset transfer size */
        usbdh->usbDevice->EP_IN[i].DIEPTRS = 0;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_IN[i].DIEPINT = 0xFB7F;
    }

    USB_OTG_D_DisableInEpInterrupt(usbdh->usbDevice, USBD_INT_EP_IN_TXFUDR);

    USB_OTG_DisableAllGlobalInterrupt(usbdh->usbGlobal);

    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, 0xBFFFFFFF);

    usbdh->usbGlobal->GINTMASK |= USB_INT_G_RXFNONE;

    if (usbdh->usbCfg.sofStatus == ENABLE)
    {
        usbdh->usbGlobal->GINTMASK |= USB_INT_G_SOF;
    }

    if (usbdh->usbCfg.vbusSense == ENABLE)
    {
        usbdh->usbGlobal->GINTMASK |= USB_INT_G_OTG | USB_INT_G_SREQ;
    }

    /* Enable the common interrupts */
    usbdh->usbGlobal->GINTMASK |= (USB_INT_G_USBSUS     |
                                   USB_INT_G_USBRST     |
                                   USB_INT_G_ENUMD      |
                                   USB_INT_G_INEP       |
                                   USB_INT_G_ONEP       |
                                   USB_INT_G_IIINTX     |
                                   USB_INT_G_IP_OUTTX   |
                                   USB_INT_G_RWAKE);

    /* Reset PHY, gate and PHY CLK */
    usbdh->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
    usbdh->usbPower->PCGCTRL_B.GCLK = BIT_RESET;

    /* Enable soft disconnect */
    USB_OTG_D_ConfigSoftDisconnect(usbdh->usbDevice, USBD_SOFT_DISC_ENABLE);
}

/*!
 * @brief     USB device open EP
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @param     epType: endpoint type
 *
 * @param     epMps: endpoint maxinum of packet size
 *
 * @retval    None
 */
void USBD_EP_Open(USBD_HANDLE_T* usbdh, uint8_t epAddr, \
                  uint8_t epType, uint16_t epMps)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    if ((epAddr & 0x80) == 0x80)
    {
        usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;

        usbdh->epIN[epAddrTemp].epNum   = epAddrTemp;
        usbdh->epIN[epAddrTemp].epType  = epType;
        usbdh->epIN[epAddrTemp].mps     = epMps;

        usbdh->epIN[epAddrTemp].txFifoNum = usbdh->epIN[epAddrTemp].epNum;
    }
    else
    {
        usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;

        usbdh->epOUT[epAddrTemp].epNum   = epAddrTemp;
        usbdh->epOUT[epAddrTemp].epType  = epType;
        usbdh->epOUT[epAddrTemp].mps     = epMps;
    }

    /* Init data PID */
    if (epType == EP_TYPE_BULK)
    {
        usbdh->epIN[epAddrTemp].dataPID = 0;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        USB_OTG_D_ConfigEP(usbdh->usbDevice, &usbdh->epIN[epAddrTemp]);
    }
    else
    {
        USB_OTG_D_ConfigEP(usbdh->usbDevice, &usbdh->epOUT[epAddrTemp]);
    }
}

/*!
 * @brief     USB device close EP
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_Close(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    uint8_t epAddrTemp = epAddr & 0x0F;

    if ((epAddr & 0x80) == 0x80)
    {
        usbdh->epIN[epAddrTemp].epDir = EP_DIR_IN;
    }
    else
    {
        usbdh->epOUT[epAddrTemp].epDir = EP_DIR_OUT;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        USB_OTG_D_ResetEP(usbdh->usbDevice, &usbdh->epIN[epAddrTemp]);
    }
    else
    {
        USB_OTG_D_ResetEP(usbdh->usbDevice, &usbdh->epOUT[epAddrTemp]);
    }
}

/*!
 * @brief     USB device get EP stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    Stall status
 */
uint8_t USBD_EP_ReadStallStatus(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    if ((epAddr & 0x80) == 0x80)
    {
        return (usbdh->epIN[epAddr & 0x7F].stallStatus);
    }
    else
    {
        return (usbdh->epOUT[epAddr & 0x7F].stallStatus);
    }
}

/*!
 * @brief     USB device set EP on stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_ClearStall(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    USB_OTG_ENDPOINT_INFO_T* ep;

    if ((epAddr & 0x0F) > usbdh->usbCfg.devEndpointNum)
    {
        return;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        ep = &usbdh->epIN[epAddr & 0x0F];
        ep->epDir = EP_DIR_IN;
    }
    else
    {
        ep = &usbdh->epOUT[epAddr & 0x0F];
        ep->epDir = EP_DIR_OUT;
    }

    ep->stallStatus = DISABLE;
    ep->epNum = epAddr & 0x0F;

    if (ep->epDir == EP_DIR_IN)
    {
        usbdh->usbDevice->EP_IN[ep->epNum].DIEPCTRL_B.STALLH = BIT_RESET;

        if ((ep->epType == EP_TYPE_BULK) || (ep->epType == EP_TYPE_INTERRUPT))
        {
            usbdh->usbDevice->EP_IN[ep->epNum].DIEPCTRL_B.DPIDSET = BIT_SET;
        }
    }
    else
    {
        usbdh->usbDevice->EP_OUT[ep->epNum].DOEPCTRL_B.STALLH = BIT_RESET;

        if ((ep->epType == EP_TYPE_BULK) || (ep->epType == EP_TYPE_INTERRUPT))
        {
            usbdh->usbDevice->EP_OUT[ep->epNum].DOEPCTRL_B.DPIDSET = BIT_SET;
        }
    }
}

/*!
 * @brief     USB device set device address
 *
 * @param     usbdh: USB device handler
 *
 * @param     address: address
 *
 * @retval    None
 */
void USBD_SetDevAddress(USBD_HANDLE_T* usbdh, uint8_t address)
{
    usbdh->address = address;

    USB_OTG_D_ConfigDeviceAddress(usbdh->usbDevice, address);
}

/*!
 * @brief     USB device set EP on stall status
 *
 * @param     usbdh: USB device handler
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_Stall(USBD_HANDLE_T* usbdh, uint8_t epAddr)
{
    USB_OTG_ENDPOINT_INFO_T* ep;

    if ((epAddr & 0x0F) > usbdh->usbCfg.devEndpointNum)
    {
        return;
    }

    if ((epAddr & 0x80) == 0x80)
    {
        ep = &usbdh->epIN[epAddr & 0x0F];
        ep->epDir = EP_DIR_IN;
    }
    else
    {
        ep = &usbdh->epOUT[epAddr & 0x0F];
        ep->epDir = EP_DIR_OUT;
    }

    ep->stallStatus = ENABLE;
    ep->epNum = epAddr & 0x0F;

    if (ep->epDir == EP_DIR_IN)
    {
        if ((ep->epNum != 0) && \
                (usbdh->usbDevice->EP_IN[ep->epNum].DIEPCTRL_B.EPEN == 0))
        {
            usbdh->usbDevice->EP_IN[ep->epNum].DIEPCTRL_B.EPDIS = BIT_RESET;
        }

        usbdh->usbDevice->EP_IN[ep->epNum].DIEPCTRL_B.STALLH = BIT_SET;
    }
    else
    {
        if ((ep->epNum != 0) && \
                (usbdh->usbDevice->EP_OUT[ep->epNum].DOEPCTRL_B.EPEN == 0))
        {
            usbdh->usbDevice->EP_OUT[ep->epNum].DOEPCTRL_B.EPDIS = BIT_RESET;
        }

        usbdh->usbDevice->EP_OUT[ep->epNum].DOEPCTRL_B.STALLH = BIT_SET;
    }
    
    if((epAddr & 0x0F) == USBD_EP_0)
    {
        USBD_EP0_OutHandler(usbdh, usbdh->usbCfg.dmaStatus, (uint8_t *)usbdh->setup);
    }
}

/*!
 * @brief     USB device start
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Start(USBD_HANDLE_T* usbdh)
{
    if ((usbdh->usbCfg.batteryStatus == ENABLE) && \
            (usbdh->usbCfg.phyType != USB_OTG_PHY_EX))
    {
        USB_OTG_DisablePowerDown(usbdh->usbGlobal);
    }

    /* Enable USB OTG all global interrupt */
    USB_OTG_EnableAllGlobalInterrupt(usbdh->usbGlobal);

    /* Reset PHY, gate and PHY CLK */
    usbdh->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
    usbdh->usbPower->PCGCTRL_B.GCLK = BIT_RESET;

    /* Disable soft disconnect */
    USB_OTG_D_ConfigSoftDisconnect(usbdh->usbDevice, USBD_SOFT_DISC_NORMAL);
}

/*!
 * @brief     USB device stop
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_Stop(USBD_HANDLE_T* usbdh)
{
    /* Enable USB OTG all global interrupt */
    USB_OTG_DisableAllGlobalInterrupt(usbdh->usbGlobal);
    
    /* Reset PHY, gate and PHY CLK */
    usbdh->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
    usbdh->usbPower->PCGCTRL_B.GCLK = BIT_RESET;
    
    /* Enable soft disconnect */
    USB_OTG_D_ConfigSoftDisconnect(usbdh->usbDevice, USBD_SOFT_DISC_ENABLE);
    
    USB_OTG_FlushTxFIFO(usbdh->usbGlobal, 0x10);
    
    if((usbdh->usbCfg.batteryStatus == ENABLE) && \
       (usbdh->usbCfg.phyType != USB_OTG_PHY_EX))
    {
        USB_OTG_EnablePowerDown(usbdh->usbGlobal);
    }
}

/*!
 * @brief     USB device stop
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_StopDevice(USBD_HANDLE_T* usbdh)
{
    uint8_t i;
    
    for(i = 0; i < 0x08; i++)
    {
        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_OUT[i].DOEPINT &= 0xFB7F;
        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_IN[i].DIEPINT &= 0xFB7F;
    }
    
    usbdh->usbDevice->DINIMASK = 0;
    usbdh->usbDevice->DOUTIMASK = 0;
    usbdh->usbDevice->DAEPIMASK = 0;
    
    USB_OTG_FlushRxFIFO(usbdh->usbGlobal);
    
    USB_OTG_FlushTxFIFO(usbdh->usbGlobal, 0x10);
}

#endif /* defined (USB_OTG) */

/*!
 * @brief     USB device resume callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_ResumeCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device suspend callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_SuspendCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device enum done callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_EnumDoneCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device SETUP stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_SetupStageCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device data IN stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
__weak void USBD_DataInStageCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    /* callback interface */
}

/*!
 * @brief     USB device data OUT stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
__weak void USBD_DataOutStageCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    /* callback interface */
}

/*!
 * @brief       USB device SOF event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
__weak void USBD_SOFCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device ISO IN in complete callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
__weak void USBD_IsoInInCompleteCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    /* callback interface */
}

/*!
 * @brief     USB device ISO OUT in complete callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
__weak void USBD_IsoOutInCompleteCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    /* callback interface */
}

/*!
 * @brief     USB device connect callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_ConnectCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/*!
 * @brief     USB device disconnect callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
__weak void USBD_DisconnectCallback(USBD_HANDLE_T* usbdh)
{
    /* callback interface */
}

/**@} end of group USB_Device_Functions*/
/**@} end of group USB_Device_Driver*/
/**@} end of group APM32F10x_StdPeriphDriver*/
