/*******************************************************************************
  * @file        bsp_spi_flash.c
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      trx
  * @date        2022-4-24
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-4-24  trx Created
*******************************************************************************/
#include "bsp_spi_flash.h"

/* Ð¾Æ¬I     	0x5217   21015 */

/* ´®ÐÐFlashµÄÆ¬Ñ¡GPIO¶Ë¿Ú£¬ PB14  */
#define SF_CS_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define SF_CS_GPIO                    GPIOB
#define SF_CS_PIN                    GPIO_PIN_14

#define SF_CS_0()                    SF_CS_GPIO->BSRR = ((uint32_t)SF_CS_PIN << 16U)
#define SF_CS_1()                    SF_CS_GPIO->BSRR = SF_CS_PIN
    
#define CMD_AAI       0xAD      /* AAI Á¬Ðø±à³ÌÖ¸Áî(FOR SST25VF016B) */
#define CMD_DISWR      0x04        /* ½ûÖ¹Ð´, ÍË³öAAI×´Ì¬ */
#define CMD_EWRSR      0x50        /* ÔÊÐíÐ´×´Ì¬¼Ä´æÆ÷µÄÃüÁî */
#define CMD_WRSR      0x01      /* Ð´×´Ì¬¼Ä´æÆ÷ÃüÁî */
#define CMD_WREN      0x06        /* Ð´Ê¹ÄÜÃüÁî */
#define CMD_READ      0x03      /* ¶ÁÊý¾ÝÇøÃüÁî */
#define CMD_RDSR      0x05        /* ¶Á×´Ì¬¼Ä´æÆ÷ÃüÁî */
#define CMD_RDID      0x9F        /* ¶ÁÆ÷¼þIDÃüÁî */
#define CMD_SE        0x20        /* ²Á³ýÉÈÇøÃüÁî */
#define CMD_BE        0xC7        /* ÅúÁ¿²Á³ýÃüÁî */
#define DUMMY_BYTE    0xA5        /* ÑÆÃüÁî£¬¿ÉÒÔÎªÈÎÒâÖµ£¬ÓÃÓÚ¶Á²Ù×÷ */

#define WIP_FLAG      0x01        /* ×´Ì¬¼Ä´æÆ÷ÖÐµÄÕýÔÚ±à³Ì±êÖ¾£¨WIP) */

SFLASH_T g_tSF;

void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

//static uint8_t g_spiTxBuf[4*1024];    /* ÓÃÓÚÐ´º¯Êý£¬ÏÈ¶Á³öÕû¸öÉÈÇø£¬ÐÞ¸Ä»º³åÇøºó£¬ÔÙÕû¸öÉÈÇø»ØÐ´ */

/*******************************************************************************
  * @FunctionName: sf_SetCS
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ29ÈÕ 13:13:02 
  * @Purpose:      ´®ÐÐflashÆ¬Ñ¡¿ØÖÆº¯Êý¡Ì¡Ì¡Ì
  * @param:        _Level£º0£ºÆ¬Ñ¡£»1£º½ûÖ¹Æ¬Ñ¡
  * @return:       none
*******************************************************************************/
void sf_SetCS(uint8_t _Level)
{
    if (_Level == 0)
    {
        bsp_Spi1BusEnter();    
        /* ×î¸ßËÙÐ´flash»á³ö´í£¬ËùÒÔÕâÀïÊ¹ÓÃ´Î¸ßËÙ */
        bsp_InitSPI1Param(SPI_BAUDRATEPRESCALER_21M, SPI_PHASE_2EDGE, SPI_POLARITY_HIGH);
        SF_CS_0();
    }
    else
    {        
        SF_CS_1();    
        bsp_Spi1BusExit();        
    }
}


/*
*********************************************************************************************************
*    º¯ Êý Ãû: sf_WriteEnable
*    ¹¦ÄÜËµÃ÷: ÏòÆ÷¼þ·¢ËÍÐ´Ê¹ÄÜÃüÁî¡Ì¡Ì¡Ì
*    ÐÎ    ²Î: ÎÞ 
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void sf_WriteEnable(void)
{
    sf_SetCS(0);                                    /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_WREN);            /* ·¢ËÍÃüÁî */
    bsp_spi1Transfer();
    sf_SetCS(1);                                    /* ½ûÄÜÆ¬Ñ¡ */
}
/*
*********************************************************************************************************
*    º¯ Êý Ãû: sf_WaitForWriteEnd
*    ¹¦ÄÜËµÃ÷: ²ÉÓÃÑ­»·²éÑ¯µÄ·½Ê½µÈ´ýÆ÷¼þÄÚ²¿Ð´²Ù×÷Íê³É
*    ÐÎ    ²Î:    ÎÞ	¡Ì¡Ì¡Ì
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
    sf_SetCS(0);                                    /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiTxBuf[0] = (CMD_RDSR);                        /* ·¢ËÍÃüÁî£¬ ¶Á×´Ì¬¼Ä´æÆ÷ */
    g_spiLen = 2;
    bsp_spi1Transfer();    
    sf_SetCS(1);                                    /* ½ûÄÜÆ¬Ñ¡ */
    
    while(1)
    {
        sf_SetCS(0);                                /* Ê¹ÄÜÆ¬Ñ¡ */
        g_spiTxBuf[0] = (CMD_RDSR);                    /* ·¢ËÍÃüÁî£¬ ¶Á×´Ì¬¼Ä´æÆ÷ */
        g_spiTxBuf[1] = 0;                            /* ÎÞ¹ØÊý¾Ý */
        g_spiLen = 2;
        bsp_spi1Transfer();    
        sf_SetCS(1);                                /* ½ûÄÜÆ¬Ñ¡ */
        /* ·¢ËÍ¶Á×´Ì¬Æ÷Ö¸Áîºó,ÅÐ¶Ï½ÓÊÕµ½×Ö½ÚµÄµÚ0Î»ÊÇ·ñÎª0 */
        /* 0´ú±í¸ÃÉè±¸ÒÑ×¼±¸ºÃ½øÒ»²½µÄÖ¸Ê¾(Í¨¹ý²éÊý¾ÝÊÖ²áµÃÖª) */
        if ((g_spiRxBuf[1] & WIP_FLAG) != SET)        /* ÅÐ¶Ï×´Ì¬¼Ä´æÆ÷µÄÃ¦±êÖ¾Î» */
        {
            break;
        }        
    }
    //printf("Æ÷¼þÄÚ²¿²Ù×÷Íê³É\r\n");
}


/*
*********************************************************************************************************
*    º¯ Êý Ãû: sf_ReadInfo
*    ¹¦ÄÜËµÃ÷: ¶ÁÈ¡Æ÷¼þID,²¢Ìî³äÆ÷¼þ²ÎÊý
*    ÐÎ    ²Î: ÎÞ
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/

void sf_ReadInfo(void)
{
    /* ×Ô¶¯Ê¶±ð´®ÐÐFlashÐÍºÅ */
    {
        g_tSF.ChipID = sf_ReadID();    /* Ð¾Æ¬ID */

        switch (g_tSF.ChipID)
        {
            case SST25VF016B_ID:
                strcpy(g_tSF.ChipName, "SST25VF016B");
                g_tSF.TotalSize = 2 * 1024 * 1024;    /* ×ÜÈÝÁ¿ = 2M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óÐ¡ = 4K */
                break;

            case MX25L1606E_ID:
                strcpy(g_tSF.ChipName, "MX25L1606E");
                g_tSF.TotalSize = 2 * 1024 * 1024;    /* ×ÜÈÝÁ¿ = 2M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óÐ¡ = 4K */
                break;

            case W25Q64BV_ID:
                strcpy(g_tSF.ChipName, "W25Q64BV");
                g_tSF.TotalSize = 8 * 1024 * 1024;    /* ×ÜÈÝÁ¿ = 8M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óÐ¡ = 4K */
                break;
            
            case N25Q128_ID:
                strcpy(g_tSF.ChipName, "N25Q128FV");
                g_tSF.TotalSize = 16 * 1024 * 1024;    /* ×ÜÈÝÁ¿ = 8M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óÐ¡ = 4K */
                break;            
            case W25Q128FV_ID:
                strcpy(g_tSF.ChipName,"W25Q128FV");
                g_tSF.TotalSize = 16 * 1024 * 1024;
                g_tSF.SectorSize = 4 * 1024;
                break;
            default:
                strcpy(g_tSF.ChipName, "Unknow Flash");
                g_tSF.TotalSize = 2 * 1024 * 1024;
                g_tSF.SectorSize = 4 * 1024;
                break;
        }
    }
}


/*
*********************************************************************************************************
*    º¯ Êý Ãû: sf_ReadID
*    ¹¦ÄÜËµÃ÷: ¶ÁÈ¡Æ÷¼þÖÆÔìÉÌID
*    ÐÎ    ²Î:  ÎÞ
*    ·µ »Ø Öµ: 32bitµÄÆ÷¼þID (×î¸ß8bitÌî0£¬ÓÐÐ§IDÎ»ÊýÎª24bit£©
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
    uint32_t uiID;
    uint8_t id1, id2, id3;

    sf_SetCS(0);                            /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[0] = (CMD_RDID);                /* ·¢ËÍ¶ÁIDÃüÁî */
    g_spiLen = 4;
    bsp_spi1Transfer();
    
    id1 = g_spiRxBuf[1];                    /* ¶ÁIDµÄµÚ1¸ö×Ö½Ú */
    id2 = g_spiRxBuf[2];                    /* ¶ÁIDµÄµÚ2¸ö×Ö½Ú */
    id3 = g_spiRxBuf[3];                    /* ¶ÁIDµÄµÚ3¸ö×Ö½Ú */
    sf_SetCS(1);                            /* ½ûÄÜÆ¬Ñ¡ */

    uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;
    return uiID;
}


/*
*********************************************************************************************************
*    º¯ Êý Ãû: sf_EraseChip
*    ¹¦ÄÜËµÃ÷: ²Á³ýÕû¸öÐ¾Æ¬¡Ì¡Ì¡Ì
*    ÐÎ    ²Î:  ÎÞ
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void sf_EraseChip(void)
{    
    sf_WriteEnable();                                /* ·¢ËÍÐ´Ê¹ÄÜÃüÁî */

    /* ²Á³ýÉÈÇø²Ù×÷ */
    sf_SetCS(0);        /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_BE;                /* ·¢ËÍÕûÆ¬²Á³ýÃüÁî */
    bsp_spi1Transfer();
    sf_SetCS(1);                                    /* ½ûÄÜÆ¬Ñ¡ */

    sf_WaitForWriteEnd();                            /* µÈ´ý´®ÐÐFlashÄÚ²¿Ð´²Ù×÷Íê³É */
}

/*
*********************************************************************************************************
*    º¯ Êý Ãû: bsp_InitSFlash
*    ¹¦ÄÜËµÃ÷: ´®ÐÐfalshÓ²¼þ³õÊ¼»¯¡£ ÅäÖÃCS GPIOÆ¬Ñ¡ÐÅºÅ£¬ ¶ÁÈ¡ID¡£
*    ÐÎ    ²Î: ÎÞ
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{
    /* ÅäÖÃCS GPIO */
    {
        GPIO_InitTypeDef gpio_init;

        /* ´ò¿ªGPIOÊ±ÖÓ */
        SF_CS_CLK_ENABLE();
        
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;    /* ÉèÖÃÍÆÍìÊä³ö */
        gpio_init.Pull = GPIO_PULLUP;            /* ÉÏÏÂÀ­µç×è²»Ê¹ÄÜ */
        gpio_init.Speed = GPIO_SPEED_HIGH;      /* GPIOËÙ¶ÈµÈ¼     */	
        gpio_init.Pin = SF_CS_PIN;    
        HAL_GPIO_Init(SF_CS_GPIO, &gpio_init);    
    }
    
    /* ¶ÁÈ¡Ð¾Æ¬ID, ×Ô¶¯Ê¶±ðÐ¾Æ¬ÐÍºÅ */
    sf_ReadInfo();
}    
/*******************************************************************************
  * @FunctionName: sf_NeedErase
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 13:28:19 
  * @Purpose:      ÅÐ¶ÏÐ´PAGEÖ®Ç°ÊÇ·ñÐèÒª²Á³ý¡Ì¡Ì¡Ì
  * @param:        _ucpOldBuf£º¾ÉÊý¾Ý
  * @param:        _ucpNewBuf£ºÐÂÊý¾Ý
  * @param:        _usLen    £ºÊý¾Ý¸öÊý
  * @return:        0£»²»ÐèÒª²Á³ý£»1£ºÐèÒª²Á³ý
*******************************************************************************/
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t * _ucpNewBuf, uint16_t _usLen)
{
    uint16_t i;
    uint8_t ucOld;

    /*
    Ëã·¨µÚÒ»²½£ºoldÇó·´£¬new²»±ä
                1101    0101
            ~    
            =    0010    0101
    Ëã·¨µÚ¶þ²¿£ºoldÇó·´½á¹ûÎ»Óënew
                0010
            &    0101
            =    0000
    Ëã·¨µÚÈý²½£º½á¹ûÎª0£¬Ôò±íÊ¾ÎÞÐè²Á³ý£¬·ñÔò±íÊ¾ÐèÒª²Á³ý
    */

    for (i = 0; i < _usLen; ++i)
        {
            ucOld = *_ucpOldBuf++;
            ucOld = ~ucOld;

            if ((ucOld & (*_ucpNewBuf++)) != 0)
                {
                    return 1;
                }
        }
    return 0;
}

/*******************************************************************************
  * @FunctionName: sf_CmpData
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 14:07:40 
  * @Purpose:      ±È½ÏflashµÄÊý¾Ý¡Ì¡Ì¡Ì
  * @param:        _uiSrcAddr£ºÊý¾Ý»º³åÇø
  * @param:        _ucpTar     falshµØÖ·
  * @param:        _uiSize     Êý¾Ý¸öÊý£¬²»ÄÜ³¬³öÐ¾Æ¬×ÜÈÝÁ¿
  * @return:       0£ºÏàµÈ£»1£»²»ÏàµÈ£»
*******************************************************************************/
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t * _ucpTar, uint32_t _uiSize)
{
    uint16_t i,j;
    uint16_t rem;
    
    /* Èç¹û¶ÁÈ¡µÄÊý¾Ý³¤¶ÈÎª0»òÕß³¬³ö´®ÐÐflashµØÖ·¿Õ¼ä£¬ÔòÖ±½Ó·µ»Ø */

    if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
        {
            return     1;
        }

    if (_uiSize == 0)
        {
            return 0;
        }
    sf_SetCS(0);                /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_READ);
    g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff0000) >> 16);
    g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff00) >> 8);
    g_spiTxBuf[g_spiLen++] = (_uiSrcAddr & 0xff);
    bsp_spi1Transfer();

    /* ¿ªÊ¼¶ÁÊý¾Ý£¬Ó¦Îªµ×²ãDMA»º³åÇøÓÐÏÞ£¬±ØÐë·Ö°ü¶Á */
    for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; ++i)
        {
            g_spiLen = SPI_BUFFER_SIZE;
            bsp_spi1Transfer();

            for (j = 0; j < SPI_BUFFER_SIZE; ++j)
                {
                    if(g_spiRxBuf[j] != *_ucpTar++)
                        {
                            goto NOTEQ;            /* ²»ÏàµÈ */
                        }
                }
        }
    rem = _uiSize % SPI_BUFFER_SIZE;            /* Ê£Óà×Ö½Ú */
    if (rem > 0)
        {
            g_spiLen = rem;
            bsp_spi1Transfer();

            for (j = 0; j < rem; ++j)
                {
                    if (g_spiRxBuf[j] != *_ucpTar++)
                        {
                            goto NOTEQ;        /* ²»ÏàµÈ */
                        }
                }
        }
    sf_SetCS(1);
    return 0;            /* ÏàµÈ */

NOTEQ:
    sf_SetCS(1);
    return 1;            /* ²»ÏàµÈ */
}


/*******************************************************************************
  * @FunctionName: sf_ReadBuffer
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 14:47:32 
  * @Purpose:      Á¬Ðø¶ÁÈ¡Èô¸É×Ö½Ú£¬×Ö½Ú¸öÊý²»ÄÜ³¬³öÐ¾Æ¬ÊýÁ¿¡Ì¡Ì¡Ì
  * @param:        _pBuf       Êý¾ÝÔ´»º³åÇø
  * @param:        _uiReadAddr Ê×µØÖ·
  * @param:        _uiSize     Êý¾Ý¸öÊý£¬²»ÄÜ³¬³öÐ¾Æ¬×ÜÈÝÁ¿
*******************************************************************************/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
    uint16_t rem;
    uint16_t i;

    /* Èç¹û¶ÁÈ¡µÄÊý¾Ý³¤¶ÈÎª0»òÕß³¬³ö´®ÐÐflashµØÖ·¿Õ¼ä£¬ÔòÖ±½Ó·µ»Ø */
    if ((_uiSize == 0) || (_uiReadAddr + _uiSize) > g_tSF.TotalSize)
    {
        return ;
    }

    /* ²Á³ýÉÈÇø²Ù×÷ */
    sf_SetCS(0);                                /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_READ);                        /* ·¢ËÍ¶ÁÖ¸Áî */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff0000) >> 16);    /* ·¢ËÍÉÈÇøµØÖ·µÄ¸ß8bit */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff00) >> 8);        /* ·¢ËÍÉÈÇøµØÖ·µÄÖÐ8bit */
    g_spiTxBuf[g_spiLen++] = (_uiReadAddr & 0xff);                /* ·¢ËÍÉÈÇøµØÖ·µÍ8bit */
    bsp_spi1Transfer();            /* µÚÒ»´Î·¢ËÍ0x03 */

    /* ¿ªÊ¼¶ÁÊý¾Ý£¬ÒòÎªµ×²ãDMA»º³åÇøÓÐÏÞ£¬±ØÐë·Ö°ü¶Á */
    for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; i++)
    {
        g_spiLen = SPI_BUFFER_SIZE;                            /* Ã¿´Î¶ÁÈ¡4k´óÐ¡µÄÉÈÇø */
        bsp_spi1Transfer();    /* µÚ¶þ´Î·¢ËÍ0x03 */
        /* ´Ó´æ´¢Çøg_spiRxBuf¸´ÖÆSPI_BUFFER_SIZE¸ö×Ö½Úµ½_pBuf */
        /* ·µ»ØÒ»¸öÖ¸Ïò_pBuf´æ´¢ÇøµÄÖ¸Õë */
        memcpy(_pBuf, g_spiRxBuf,SPI_BUFFER_SIZE);
        _pBuf += SPI_BUFFER_SIZE;                            /* µØÖ·ÔËËã£¬½«_pBufÖ¸ÕëÖ¸ÏòÏÂÒ»¸öÉÈÇøµÄÊ×µØÖ· */
    }
    rem = _uiSize % SPI_BUFFER_SIZE;            /* Ê£Óà×Ö½Ú£¬¼ÆËãÒª¶ÁÈ¡µÄÊý¾ÝÊÇ·ñÊÇÕûÉÈÇø */

    if (rem > 0)
    {
        g_spiLen = rem;
        bsp_spi1Transfer();
        memcpy(_pBuf,g_spiRxBuf,rem);        /* ½«¶àÓàµÄ×Ö½ÚÔÙ´Î½øÐÐ¸´ÖÆ */
    }
    sf_SetCS(1);                                /* ½ûÖ¹Æ¬Ñ¡ */
}
/*******************************************************************************
  * @FunctionName: sf_AutoWriteSector
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 15:47:44 
  * @Purpose:      Ð´Ò»¸öÉÈÇø²¢Ð£Ñé£¬Èç¹û²»ÕýÈ·ÔòÔÙÖØÐ´Á½´Î£¬±¾º¯Êý×Ô¶¯Íê³É¡Ì¡Ì¡Ì
  * @param:        _ucpSrc     Êý¾ÝÔ´»º³åÇø
  * @param:        _uiWrAddr   Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usWrLen    Êý¾Ý¸öÊý£¬²»ÄÜ³¬¹ýÉÈÇø´óÐ¡
  * @return:       0£º´íÎó£»1£º³É¹¦£»
*******************************************************************************/
uint8_t sf_AutoWriteSector(uint8_t * _ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
    uint16_t i;
    uint16_t j;                /* ÓÃÓÚÑÓÊ± */
    uint32_t uiFirstAddr;    /* ÉÈÇøÊ×µØÖ· */
    uint8_t ucNeedErase;    /* 1±íÊ¾ÐèÒª²Á³ý */
    uint8_t cRet;
    
    /* ³¤¶ÈÎª0Ê±²»¼ÌÐø²Ù×÷£¬Ö±½ÓÈÏÎª³É¹¦ */
    if (_usWrLen == 0)
        {
            return 1;
        }

    /* Èç¹ûÆ«ÒÆµØÖ·³¬¹ýÐ¾Æ¬ÈÝÁ¿ÔòÍË³ö */
    if (_uiWrAddr >= g_tSF.TotalSize)
        {
            return 0;
        }

    /* Èç¹ûÊý¾Ý³¤¶È´óÓÚÉÈÇøÈÝÁ¿£¬ÔòÍË³ö */
    if (_usWrLen > g_tSF.SectorSize)
        {
            return 0;
        }

    /* Èç¹ûflashÖÐµÄÊý¾ÝÃ»ÓÐ±ä»¯£¬Ôò²»Ð´flash */
    sf_ReadBuffer(g_spiTxBuf,_uiWrAddr,_usWrLen);
    if (memcmp(g_spiTxBuf,_ucpSrc,_usWrLen) == 0)
        {
            return 1;
        }

    /* ÅÐ¶ÏÊÇ·ñÐèÒªÏÈ²Á³ýÉÈÇø */
    /* Èç¹û¾ÉÊý¾ÝÐÞ¸ÄÎªÐÂÊý¾Ý£¬ËùÓÐÎ»¾ùÊÇ 1->0 »òÕß 0->0,ÔòÎÞÐè²Á³ý£¬Ìá¸ßflashÊÙÃü */
    ucNeedErase = 0;            /* 0²»ÐèÒª²Á³ý */
    if (sf_NeedErase(g_spiTxBuf,_ucpSrc,_usWrLen))
        {
            ucNeedErase = 1;    /* 1ÐèÒª²Á³ý */
        }

    uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

    if (_usWrLen == g_tSF.SectorSize)        /* Õû¸öÉÈÇø¶¼¸ÄÐ´ */
        {
            for (i = 0; i < g_tSF.SectorSize; ++i)
                {
                    g_spiTxBuf[i] = _ucpSrc[i];
                }
        }
    else                                    /* ¸ÄÐ´²¿·ÖÊý¾Ý */
    {
        /* ÏÈ½«Õû¸öÉÈÇøµÄÊý¾Ý¶Á³ö */
        sf_ReadBuffer(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);

        /* ÔÙÓÃÐÂÊý¾Ý¸²¸Ç */
        i = _uiWrAddr & (g_tSF.SectorSize - 1);
        memcpy(&g_spiTxBuf[i],_ucpSrc,_usWrLen);
    }

    /* Ð´ÍêÖ®ºó½øÐÐÐ£Ñé£¬Èç¹û²»ÕýÈ·ÔòÖØÐ´£¬×î¶à3´Î */
    cRet = 0;
    for (i = 0; i < 3; ++i)
        {
            /* Èç¹û¾ÉÊý¾ÝÐÞ¸ÄÎªÐÂÊý¾Ý£¬ËùÓÐÎ»¾ùÊÇ1->0,»òÕß0->0£¬ÔòÎÞÐè²Á³ý£¬Ìá¸ßflashÊÙÃü */
            if (ucNeedErase == 1)
                {
                    sf_EraseSector(uiFirstAddr);    /* ²Á³ý1¸öÉÈÇø */
                }

            /* ±à³ÌÒ»¸öÉÈÇø */
            sf_PageWrite(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);
            
            if (sf_CmpData(_uiWrAddr,_ucpSrc,_usWrLen) == 0)
                {
                    cRet = 1;
                    break;
                }
            else
            {
                if (sf_CmpData(_uiWrAddr,_ucpSrc,_usWrLen) == 0)
                    {
                        cRet = 1;
                        break;
                    }
                /* Ê§°ÜºóÑÓ³ÙÒ»¶ÎÊ±¼äÔÙÖØÊÔ */
                for(j = 0;j < 10000;j++);
            }
        }
    return cRet;
}

/*******************************************************************************
  * @FunctionName: sf_WriteBuffer
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:59:58 
  * @Purpose:      Ð´Ò»¸öÉÈÇø²¢Ð£Ñé£¬Èç¹û²»ÕýÈ·ÔòÔÙÖØÐ´Á½´Î£¬º¯Êý×Ô¶¯Íê³É,Ò»¸öÉÈÇøµÄ´óÐ¡Îª4K¡Ì¡Ì¡Ì
  * @param:        _pBuf        Êý¾ÝÔ´»º³åÇø
  * @param:        _uiWriteAddr Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usWriteSize  Êý¾Ý¸öÊý£¬²»ÔÊÐí³¬¹ýÐ¾Æ¬ÈÝÁ¿
  * @return:       1£º³É¹¦£»0£ºÊ§°Ü£»
*******************************************************************************/
uint8_t sf_WriteBuffer(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
    uint32_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,temp = 0;

    Addr = _uiWriteAddr % g_tSF.SectorSize;            /* ÉÈÇøÄÚµÄµØÖ· */
    count = g_tSF.SectorSize - Addr;                /* ´ÓÆðÊ¼µØÖ·¿ªÊ¼ÒªÐ´¼¸¸öÉÈÇø */
    NumOfPage = _usWriteSize / g_tSF.SectorSize;    /* Ò³Êý */
    NumOfSingle = _usWriteSize % g_tSF.SectorSize;    /* Ê£ÓàµÄ×Ö½ÚÊý */

    if (Addr == 0)                /* ÆðÊ¼µØÖ·ÊÇÉÈÇøÊ×µØÖ· */
        {
            if (NumOfPage == 0)    /* Êý¾Ý³¤¶ÈÐ¡ÓÚÉÈÇø´óÐ¡ */
                {
                    if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize))
                    {
                        return 0;
                    }
                }
            else                /* Êý¾Ý³¤¶È´óÓÚÉÈÇø´óÐ¡ */
                {
                    while (NumOfPage--)
                        {
                            if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,g_tSF.SectorSize) == 0)
                            {
                                return 0;
                            }
                            _uiWriteAddr += g_tSF.SectorSize;
                            _pBuf += g_tSF.SectorSize;
                        }
                    if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,NumOfSingle) == 0)
                        {
                            return 0;
                        }
                }
        }
    else                            /* ÆðÊ¼µØÖ·²»ÊÇÉÈÇøÊ×µØÖ· */
        {
            if(NumOfPage == 0)        /* Êý¾Ý³¤¶ÈÐ¡ÓÚÉÈÇø´óÐ¡ */
            {
                if (NumOfSingle > count)
                    {
                        temp = NumOfSingle - count;

                        if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,count) == 0)
                            {
                                return 0;
                            }

                        _uiWriteAddr += count;
                        _pBuf += count;

                        if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,temp) == 0)
                            {
                                return 0;
                            }
                    }
                else
                    {
                        if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize) == 0)
                        {
                            return 0;
                        }
                    }
            }
            else                /* Êý¾Ý³¤¶È´óÓÚÉÈÇø´óÐ¡ */
                {
                    _usWriteSize -= count;
                    NumOfPage = _usWriteSize / g_tSF.SectorSize;
                    NumOfSingle = _usWriteSize % g_tSF.SectorSize;
                    if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,count) == 0)
                        {
                            return 0;
                        }
                    _uiWriteAddr += count;
                    _pBuf += count;

                    while (NumOfPage--)
                        {
                            if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,g_tSF.SectorSize) == 0)
                                {
                                    return 0;
                                }
                            _uiWriteAddr += g_tSF.SectorSize;
                            _pBuf += g_tSF.SectorSize;
                        }
                    if (NumOfSingle != 0)
                        {
                            if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,NumOfSingle) == 0)
                                {
                                    return 0;
                                }
                        }
                }
        }
    return 1;            /* ³É¹¦ */
}

/*******************************************************************************
  * @FunctionName: sf_PageWrite
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:03:59 
  * @Purpose:      Ò³±à³Ì¡Ì¡Ì¡Ì
  * @param:        _pBuf        Êý¾ÝÔ´»º³åÇø
  * @param:        _uiWriteAddr Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usSize       Êý¾Ý¸öÊý£¬Ò³´óÐ¡µÄÕûÊý±¶(256×Ö½ÚµÄÕûÊý±¶)
*******************************************************************************/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
    uint32_t i,j;
    if (g_tSF.ChipID == SST25VF016B_ID)
        {
            /* AAIÖ¸ÁîÒªÇó´«ÈëµÄÊý¾Ý¸öÊýÊÇÅ¼Êý */
            if ((_usSize < 2) && (_usSize % 2))
                {
                    return ;
                }
            sf_WriteEnable();        /* ·¢ËÍÐ´Ê¹ÄÜÃüÁî */

            sf_SetCS(0);            /* Ê¹ÄÜÆ¬Ñ¡ */
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = CMD_AAI;        /* ·¢ËÍAAIÃüÁî(µØÖ·×Ô¶¯Ôö¼Ó±à³Ì) */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);    /* ·¢ËÍÉÈÇøµØÖ·µÄ¸ß8bit */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);    /* ·¢ËÍÉÈÇøµØÖ·ÖÐ8bit */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff));            /*·¢ËÍÉÈÇøµØÖ·µÍ8bit */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++);                        /* ·¢ËÍµÚÒ»¸öÊý¾Ý */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++);                        /* ·¢ËÍµÚ2¸öÊý¾Ý */
            bsp_spi1Transfer();
            sf_SetCS(0);            /* ½ûÖ¹Æ¬Ñ¡ */

            sf_WaitForWriteEnd();    /* µÈ´ý´®ÐÐflashÄÚ²¿Ð´²Ù×÷Íê³É */

            _usSize -= 2;            /* ¼ÆËãÊ£Óà×Ö½ÚÊýÄ¿ */

            for (i = 0; i < _usSize / 2; ++i)
                {
                    sf_SetCS(0);    /* Ê¹ÄÜÆ¬Ñ¡ */
                    g_spiLen = 0;
                    g_spiTxBuf[g_spiLen++] = (CMD_AAI);
                    g_spiTxBuf[g_spiLen++] = (*_pBuf++);
                    g_spiTxBuf[g_spiLen++] = (*_pBuf++);
                    bsp_spi1Transfer();
                    sf_SetCS(1);    /* ½ûÖ¹Æ¬Ñ¡ */
                    sf_WaitForWriteEnd();/* µÈ´ý´®ÐÐflashÄÚ²¿²Ù×÷Íê³É */
                }

            /* ½øÈëÐ´±£»¤×´Ì¬ */
            sf_SetCS(0);
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
            bsp_spi1Transfer();
            sf_SetCS(1);

            sf_WaitForWriteEnd();
        }
    else    /* for MX25L1606E,W25164BV */
        {
            for (j = 0; j < _usSize / 256; ++j)
                {
                    sf_WriteEnable();    /* ·¢ËÍÐ´Ê¹ÄÜÃüÁî */

                    sf_SetCS(0);        /* Ê¹ÄÜÆ¬Ñ¡ */
                    g_spiLen = 0;
                    g_spiTxBuf[g_spiLen++] = (0x02);                                /* ·¢ËÍAAIÃüÁî(µØÖ·×Ô¶¯Ôö¼Ó±à³Ì) */
                    g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);
                    g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);
                    g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xff);
                    for (i = 0; i < 256; ++i)
                        {
                            g_spiTxBuf[g_spiLen++] = (*_pBuf++);            /* ·¢ËÍÊý¾Ý */
                        }
                    bsp_spi1Transfer();
                    sf_SetCS(1);        /* ½ûÖ¹Æ¬Ñ¡ */

                    sf_WaitForWriteEnd();/* µÈ´ý´®ÐÐflashÄÚ²¿²Ù×÷Íê³É */
                    _uiWriteAddr += 256;
                }

            /* ½øÈëÐ´±£»¤×´Ì¬ */
            sf_SetCS(0);
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
            bsp_spi1Transfer();
            sf_SetCS(1);

            sf_WaitForWriteEnd();        /* µÈ´ý´®ÐÐflashÄÚ²¿²Ù×÷Íê³É */
        }
}


/*******************************************************************************
  * @FunctionName: sf_EraseSector
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:21:35 
  * @Purpose:      ²Á³ýÖ¸¶¨µÄÉÈÇø¡Ì¡Ì¡Ì
  * @param:        _uiSectorAddr      ÉÈÇøµØÖ·
  * @return:       none
*******************************************************************************/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
    sf_WriteEnable();        /* ·¢ËÍÐ´Ê¹ÄÜÃüÁî */

    /* ²Á³ýÉÈÇø²Ù×÷ */
    sf_SetCS(0);            /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_SE;
    /* ÔÚ·¢ËÍ²Á³ýÖ¸Áîºó,½ô½ÓÕâ·¢ËÍ24Î»µÄµØÖ· */
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xf0000) >> 16);
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xff00) >> 8);
    g_spiTxBuf[g_spiLen++] = (_uiSectorAddr & 0xff);
    bsp_spi1Transfer();
    sf_SetCS(1);            /* ½ûÖ¹Æ¬Ñ¡ */

    sf_WaitForWriteEnd();    /* µÈ´ý´®ÐÐflashÄÚ²¿²Ù×÷Íê³É */
}


