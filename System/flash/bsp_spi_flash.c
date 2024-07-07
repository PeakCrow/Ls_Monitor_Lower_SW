/**
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
*/
#include "bsp_spi_flash.h"

/* Ğ¾Æ¬I     	0x5217   21015 */

/* ´®ĞĞFlashµÄÆ¬Ñ¡GPIO¶Ë¿Ú£¬ PB14  */
#define SF_CS_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define SF_CS_GPIO                    GPIOB
#define SF_CS_PIN                    GPIO_PIN_14

#define SF_CS_0()                    SF_CS_GPIO->BSRR = ((uint32_t)SF_CS_PIN << 16U)
#define SF_CS_1()                    SF_CS_GPIO->BSRR = SF_CS_PIN
    
#define CMD_AAI       0xAD      /* AAI Á¬Ğø±à³ÌÖ¸Áî(FOR SST25VF016B) */
#define CMD_DISWR      0x04        /* ½ûÖ¹Ğ´, ÍË³öAAI×´Ì¬ */
#define CMD_EWRSR      0x50        /* ÔÊĞíĞ´×´Ì¬¼Ä´æÆ÷µÄÃüÁî */
#define CMD_WRSR      0x01      /* Ğ´×´Ì¬¼Ä´æÆ÷ÃüÁî */
#define CMD_WREN      0x06        /* Ğ´Ê¹ÄÜÃüÁî */
#define CMD_READ      0x03      /* ¶ÁÊı¾İÇøÃüÁî */
#define CMD_RDSR      0x05        /* ¶Á×´Ì¬¼Ä´æÆ÷ÃüÁî */
#define CMD_RDID      0x9F        /* ¶ÁÆ÷¼şIDÃüÁî */
#define CMD_SE        0x20        /* ²Á³ıÉÈÇøÃüÁî */
#define CMD_BE        0xC7        /* ÅúÁ¿²Á³ıÃüÁî */
#define DUMMY_BYTE    0xA5        /* ÑÆÃüÁî£¬¿ÉÒÔÎªÈÎÒâÖµ£¬ÓÃÓÚ¶Á²Ù×÷ */

#define WIP_FLAG      0x01        /* ×´Ì¬¼Ä´æÆ÷ÖĞµÄÕıÔÚ±à³Ì±êÖ¾£¨WIP) */

SFLASH_T g_tSF;

void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

//static uint8_t g_spiTxBuf[4*1024];    /* ÓÃÓÚĞ´º¯Êı£¬ÏÈ¶Á³öÕû¸öÉÈÇø£¬ĞŞ¸Ä»º³åÇøºó£¬ÔÙÕû¸öÉÈÇø»ØĞ´ */

/**
  * @FunctionName: sf_SetCS
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ29ÈÕ 13:13:02 
  * @Purpose:      ´®ĞĞflashÆ¬Ñ¡¿ØÖÆº¯Êı¡Ì¡Ì¡Ì
  * @param:        _Level£º0£ºÆ¬Ñ¡£»1£º½ûÖ¹Æ¬Ñ¡
  * @return:       none
*/
void sf_SetCS(uint8_t _Level)
{
    if (_Level == 0)
    {
        bsp_Spi1BusEnter();    
        /* ×î¸ßËÙĞ´flash»á³ö´í£¬ËùÒÔÕâÀïÊ¹ÓÃ´Î¸ßËÙ */
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
* 
*    º¯ Êı Ãû: sf_WriteEnable
*    ¹¦ÄÜËµÃ÷: ÏòÆ÷¼ş·¢ËÍĞ´Ê¹ÄÜÃüÁî¡Ì¡Ì¡Ì
*    ĞÎ    ²Î: ÎŞ 
*    ·µ »Ø Öµ: ÎŞ
* 
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
* 
*    º¯ Êı Ãû: sf_WaitForWriteEnd
*    ¹¦ÄÜËµÃ÷: ²ÉÓÃÑ­»·²éÑ¯µÄ·½Ê½µÈ´ıÆ÷¼şÄÚ²¿Ğ´²Ù×÷Íê³É
*    ĞÎ    ²Î:    ÎŞ	¡Ì¡Ì¡Ì
*    ·µ »Ø Öµ: ÎŞ
* 
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
        g_spiTxBuf[1] = 0;                            /* ÎŞ¹ØÊı¾İ */
        g_spiLen = 2;
        bsp_spi1Transfer();    
        sf_SetCS(1);                                /* ½ûÄÜÆ¬Ñ¡ */
        /* ·¢ËÍ¶Á×´Ì¬Æ÷Ö¸Áîºó,ÅĞ¶Ï½ÓÊÕµ½×Ö½ÚµÄµÚ0Î»ÊÇ·ñÎª0 */
        /* 0´ú±í¸ÃÉè±¸ÒÑ×¼±¸ºÃ½øÒ»²½µÄÖ¸Ê¾(Í¨¹ı²éÊı¾İÊÖ²áµÃÖª) */
        if ((g_spiRxBuf[1] & WIP_FLAG) != SET)        /* ÅĞ¶Ï×´Ì¬¼Ä´æÆ÷µÄÃ¦±êÖ¾Î» */
        {
            break;
        }        
    }
    //printf("Æ÷¼şÄÚ²¿²Ù×÷Íê³É\r\n");
}


/*
* 
*    º¯ Êı Ãû: sf_ReadInfo
*    ¹¦ÄÜËµÃ÷: ¶ÁÈ¡Æ÷¼şID,²¢Ìî³äÆ÷¼ş²ÎÊı
*    ĞÎ    ²Î: ÎŞ
*    ·µ »Ø Öµ: ÎŞ
* 
*/

void sf_ReadInfo(void)
{
    /* ×Ô¶¯Ê¶±ğ´®ĞĞFlashĞÍºÅ */
    {
        g_tSF.ChipID = sf_ReadID();    /* Ğ¾Æ¬ID */

        switch (g_tSF.ChipID)
        {
            case SST25VF016B_ID:
                strcpy(g_tSF.ChipName, "SST25VF016B");
                g_tSF.TotalSize = 2 * 1024 * 1024;    /* ×ÜÈİÁ¿ = 2M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óĞ¡ = 4K */
                break;

            case MX25L1606E_ID:
                strcpy(g_tSF.ChipName, "MX25L1606E");
                g_tSF.TotalSize = 2 * 1024 * 1024;    /* ×ÜÈİÁ¿ = 2M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óĞ¡ = 4K */
                break;

            case W25Q64BV_ID:
                strcpy(g_tSF.ChipName, "W25Q64BV");
                g_tSF.TotalSize = 8 * 1024 * 1024;    /* ×ÜÈİÁ¿ = 8M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óĞ¡ = 4K */
                break;
            
            case N25Q128_ID:
                strcpy(g_tSF.ChipName, "N25Q128FV");
                g_tSF.TotalSize = 16 * 1024 * 1024;    /* ×ÜÈİÁ¿ = 8M */
                g_tSF.SectorSize = 4 * 1024;        /* ÉÈÇø´óĞ¡ = 4K */
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
* 
*    º¯ Êı Ãû: sf_ReadID
*    ¹¦ÄÜËµÃ÷: ¶ÁÈ¡Æ÷¼şÖÆÔìÉÌID
*    ĞÎ    ²Î:  ÎŞ
*    ·µ »Ø Öµ: 32bitµÄÆ÷¼şID (×î¸ß8bitÌî0£¬ÓĞĞ§IDÎ»ÊıÎª24bit£©
* 
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
* 
*    º¯ Êı Ãû: sf_EraseChip
*    ¹¦ÄÜËµÃ÷: ²Á³ıÕû¸öĞ¾Æ¬¡Ì¡Ì¡Ì
*    ĞÎ    ²Î:  ÎŞ
*    ·µ »Ø Öµ: ÎŞ
* 
*/
void sf_EraseChip(void)
{    
    sf_WriteEnable();                                /* ·¢ËÍĞ´Ê¹ÄÜÃüÁî */

    /* ²Á³ıÉÈÇø²Ù×÷ */
    sf_SetCS(0);        /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_BE;                /* ·¢ËÍÕûÆ¬²Á³ıÃüÁî */
    bsp_spi1Transfer();
    sf_SetCS(1);                                    /* ½ûÄÜÆ¬Ñ¡ */

    sf_WaitForWriteEnd();                            /* µÈ´ı´®ĞĞFlashÄÚ²¿Ğ´²Ù×÷Íê³É */
}

/*
* 
*    º¯ Êı Ãû: bsp_InitSFlash
*    ¹¦ÄÜËµÃ÷: ´®ĞĞfalshÓ²¼ş³õÊ¼»¯¡£ ÅäÖÃCS GPIOÆ¬Ñ¡ĞÅºÅ£¬ ¶ÁÈ¡ID¡£
*    ĞÎ    ²Î: ÎŞ
*    ·µ »Ø Öµ: ÎŞ
* 
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
    
    /* ¶ÁÈ¡Ğ¾Æ¬ID, ×Ô¶¯Ê¶±ğĞ¾Æ¬ĞÍºÅ */
    sf_ReadInfo();
}    
/**
  * @FunctionName: sf_NeedErase
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 13:28:19 
  * @Purpose:      ÅĞ¶ÏĞ´PAGEÖ®Ç°ÊÇ·ñĞèÒª²Á³ı¡Ì¡Ì¡Ì
  * @param:        _ucpOldBuf£º¾ÉÊı¾İ
  * @param:        _ucpNewBuf£ºĞÂÊı¾İ
  * @param:        _usLen    £ºÊı¾İ¸öÊı
  * @return:        0£»²»ĞèÒª²Á³ı£»1£ºĞèÒª²Á³ı
*/
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t * _ucpNewBuf, uint16_t _usLen)
{
    uint16_t i;
    uint8_t ucOld;

    /*
    Ëã·¨µÚÒ»²½£ºoldÇó·´£¬new²»±ä
                1101    0101
            ~    
            =    0010    0101
    Ëã·¨µÚ¶ş²¿£ºoldÇó·´½á¹ûÎ»Óënew
                0010
            &    0101
            =    0000
    Ëã·¨µÚÈı²½£º½á¹ûÎª0£¬Ôò±íÊ¾ÎŞĞè²Á³ı£¬·ñÔò±íÊ¾ĞèÒª²Á³ı
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

/**
  * @FunctionName: sf_CmpData
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 14:07:40 
  * @Purpose:      ±È½ÏflashµÄÊı¾İ¡Ì¡Ì¡Ì
  * @param:        _uiSrcAddr£ºÊı¾İ»º³åÇø
  * @param:        _ucpTar     falshµØÖ·
  * @param:        _uiSize     Êı¾İ¸öÊı£¬²»ÄÜ³¬³öĞ¾Æ¬×ÜÈİÁ¿
  * @return:       0£ºÏàµÈ£»1£»²»ÏàµÈ£»
*/
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t * _ucpTar, uint32_t _uiSize)
{
    uint16_t i,j;
    uint16_t rem;
    
    /* Èç¹û¶ÁÈ¡µÄÊı¾İ³¤¶ÈÎª0»òÕß³¬³ö´®ĞĞflashµØÖ·¿Õ¼ä£¬ÔòÖ±½Ó·µ»Ø */

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

    /* ¿ªÊ¼¶ÁÊı¾İ£¬Ó¦Îªµ×²ãDMA»º³åÇøÓĞÏŞ£¬±ØĞë·Ö°ü¶Á */
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


/**
  * @FunctionName: sf_ReadBuffer
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 14:47:32 
  * @Purpose:      Á¬Ğø¶ÁÈ¡Èô¸É×Ö½Ú£¬×Ö½Ú¸öÊı²»ÄÜ³¬³öĞ¾Æ¬ÊıÁ¿¡Ì¡Ì¡Ì
  * @param:        _pBuf       Êı¾İÔ´»º³åÇø
  * @param:        _uiReadAddr Ê×µØÖ·
  * @param:        _uiSize     Êı¾İ¸öÊı£¬²»ÄÜ³¬³öĞ¾Æ¬×ÜÈİÁ¿
*/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
    uint16_t rem;
    uint16_t i;

    /* Èç¹û¶ÁÈ¡µÄÊı¾İ³¤¶ÈÎª0»òÕß³¬³ö´®ĞĞflashµØÖ·¿Õ¼ä£¬ÔòÖ±½Ó·µ»Ø */
    if ((_uiSize == 0) || (_uiReadAddr + _uiSize) > g_tSF.TotalSize)
    {
        return ;
    }

    /* ²Á³ıÉÈÇø²Ù×÷ */
    sf_SetCS(0);                                /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_READ);                        /* ·¢ËÍ¶ÁÖ¸Áî */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff0000) >> 16);    /* ·¢ËÍÉÈÇøµØÖ·µÄ¸ß8bit */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff00) >> 8);        /* ·¢ËÍÉÈÇøµØÖ·µÄÖĞ8bit */
    g_spiTxBuf[g_spiLen++] = (_uiReadAddr & 0xff);                /* ·¢ËÍÉÈÇøµØÖ·µÍ8bit */
    bsp_spi1Transfer();            /* µÚÒ»´Î·¢ËÍ0x03 */

    /* ¿ªÊ¼¶ÁÊı¾İ£¬ÒòÎªµ×²ãDMA»º³åÇøÓĞÏŞ£¬±ØĞë·Ö°ü¶Á */
    for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; i++)
    {
        g_spiLen = SPI_BUFFER_SIZE;                            /* Ã¿´Î¶ÁÈ¡4k´óĞ¡µÄÉÈÇø */
        bsp_spi1Transfer();    /* µÚ¶ş´Î·¢ËÍ0x03 */
        /* ´Ó´æ´¢Çøg_spiRxBuf¸´ÖÆSPI_BUFFER_SIZE¸ö×Ö½Úµ½_pBuf */
        /* ·µ»ØÒ»¸öÖ¸Ïò_pBuf´æ´¢ÇøµÄÖ¸Õë */
        memcpy(_pBuf, g_spiRxBuf,SPI_BUFFER_SIZE);
        _pBuf += SPI_BUFFER_SIZE;                            /* µØÖ·ÔËËã£¬½«_pBufÖ¸ÕëÖ¸ÏòÏÂÒ»¸öÉÈÇøµÄÊ×µØÖ· */
    }
    rem = _uiSize % SPI_BUFFER_SIZE;            /* Ê£Óà×Ö½Ú£¬¼ÆËãÒª¶ÁÈ¡µÄÊı¾İÊÇ·ñÊÇÕûÉÈÇø */

    if (rem > 0)
    {
        g_spiLen = rem;
        bsp_spi1Transfer();
        memcpy(_pBuf,g_spiRxBuf,rem);        /* ½«¶àÓàµÄ×Ö½ÚÔÙ´Î½øĞĞ¸´ÖÆ */
    }
    sf_SetCS(1);                                /* ½ûÖ¹Æ¬Ñ¡ */
}
/**
  * @FunctionName: sf_AutoWriteSector
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 15:47:44 
  * @Purpose:      Ğ´Ò»¸öÉÈÇø²¢Ğ£Ñé£¬Èç¹û²»ÕıÈ·ÔòÔÙÖØĞ´Á½´Î£¬±¾º¯Êı×Ô¶¯Íê³É¡Ì¡Ì¡Ì
  * @param:        _ucpSrc     Êı¾İÔ´»º³åÇø
  * @param:        _uiWrAddr   Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usWrLen    Êı¾İ¸öÊı£¬²»ÄÜ³¬¹ıÉÈÇø´óĞ¡
  * @return:       0£º´íÎó£»1£º³É¹¦£»
*/
uint8_t sf_AutoWriteSector(uint8_t * _ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
    uint16_t i;
    uint16_t j;                /* ÓÃÓÚÑÓÊ± */
    uint32_t uiFirstAddr;    /* ÉÈÇøÊ×µØÖ· */
    uint8_t ucNeedErase;    /* 1±íÊ¾ĞèÒª²Á³ı */
    uint8_t cRet;
    
    /* ³¤¶ÈÎª0Ê±²»¼ÌĞø²Ù×÷£¬Ö±½ÓÈÏÎª³É¹¦ */
    if (_usWrLen == 0)
        {
            return 1;
        }

    /* Èç¹ûÆ«ÒÆµØÖ·³¬¹ıĞ¾Æ¬ÈİÁ¿ÔòÍË³ö */
    if (_uiWrAddr >= g_tSF.TotalSize)
        {
            return 0;
        }

    /* Èç¹ûÊı¾İ³¤¶È´óÓÚÉÈÇøÈİÁ¿£¬ÔòÍË³ö */
    if (_usWrLen > g_tSF.SectorSize)
        {
            return 0;
        }

    /* Èç¹ûflashÖĞµÄÊı¾İÃ»ÓĞ±ä»¯£¬Ôò²»Ğ´flash */
    sf_ReadBuffer(g_spiTxBuf,_uiWrAddr,_usWrLen);
    if (memcmp(g_spiTxBuf,_ucpSrc,_usWrLen) == 0)
        {
            return 1;
        }

    /* ÅĞ¶ÏÊÇ·ñĞèÒªÏÈ²Á³ıÉÈÇø */
    /* Èç¹û¾ÉÊı¾İĞŞ¸ÄÎªĞÂÊı¾İ£¬ËùÓĞÎ»¾ùÊÇ 1->0 »òÕß 0->0,ÔòÎŞĞè²Á³ı£¬Ìá¸ßflashÊÙÃü */
    ucNeedErase = 0;            /* 0²»ĞèÒª²Á³ı */
    if (sf_NeedErase(g_spiTxBuf,_ucpSrc,_usWrLen))
        {
            ucNeedErase = 1;    /* 1ĞèÒª²Á³ı */
        }

    uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

    if (_usWrLen == g_tSF.SectorSize)        /* Õû¸öÉÈÇø¶¼¸ÄĞ´ */
        {
            for (i = 0; i < g_tSF.SectorSize; ++i)
                {
                    g_spiTxBuf[i] = _ucpSrc[i];
                }
        }
    else                                    /* ¸ÄĞ´²¿·ÖÊı¾İ */
    {
        /* ÏÈ½«Õû¸öÉÈÇøµÄÊı¾İ¶Á³ö */
        sf_ReadBuffer(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);

        /* ÔÙÓÃĞÂÊı¾İ¸²¸Ç */
        i = _uiWrAddr & (g_tSF.SectorSize - 1);
        memcpy(&g_spiTxBuf[i],_ucpSrc,_usWrLen);
    }

    /* Ğ´ÍêÖ®ºó½øĞĞĞ£Ñé£¬Èç¹û²»ÕıÈ·ÔòÖØĞ´£¬×î¶à3´Î */
    cRet = 0;
    for (i = 0; i < 3; ++i)
        {
            /* Èç¹û¾ÉÊı¾İĞŞ¸ÄÎªĞÂÊı¾İ£¬ËùÓĞÎ»¾ùÊÇ1->0,»òÕß0->0£¬ÔòÎŞĞè²Á³ı£¬Ìá¸ßflashÊÙÃü */
            if (ucNeedErase == 1)
                {
                    sf_EraseSector(uiFirstAddr);    /* ²Á³ı1¸öÉÈÇø */
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

/**
  * @FunctionName: sf_WriteBuffer
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:59:58 
  * @Purpose:      Ğ´Ò»¸öÉÈÇø²¢Ğ£Ñé£¬Èç¹û²»ÕıÈ·ÔòÔÙÖØĞ´Á½´Î£¬º¯Êı×Ô¶¯Íê³É,Ò»¸öÉÈÇøµÄ´óĞ¡Îª4K¡Ì¡Ì¡Ì
  * @param:        _pBuf        Êı¾İÔ´»º³åÇø
  * @param:        _uiWriteAddr Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usWriteSize  Êı¾İ¸öÊı£¬²»ÔÊĞí³¬¹ıĞ¾Æ¬ÈİÁ¿
  * @return:       1£º³É¹¦£»0£ºÊ§°Ü£»
*/
uint8_t sf_WriteBuffer(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
    uint32_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,temp = 0;

    Addr = _uiWriteAddr % g_tSF.SectorSize;            /* ÉÈÇøÄÚµÄµØÖ· */
    count = g_tSF.SectorSize - Addr;                /* ´ÓÆğÊ¼µØÖ·¿ªÊ¼ÒªĞ´¼¸¸öÉÈÇø */
    NumOfPage = _usWriteSize / g_tSF.SectorSize;    /* Ò³Êı */
    NumOfSingle = _usWriteSize % g_tSF.SectorSize;    /* Ê£ÓàµÄ×Ö½ÚÊı */

    if (Addr == 0)                /* ÆğÊ¼µØÖ·ÊÇÉÈÇøÊ×µØÖ· */
        {
            if (NumOfPage == 0)    /* Êı¾İ³¤¶ÈĞ¡ÓÚÉÈÇø´óĞ¡ */
                {
                    if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize))
                    {
                        return 0;
                    }
                }
            else                /* Êı¾İ³¤¶È´óÓÚÉÈÇø´óĞ¡ */
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
    else                            /* ÆğÊ¼µØÖ·²»ÊÇÉÈÇøÊ×µØÖ· */
        {
            if(NumOfPage == 0)        /* Êı¾İ³¤¶ÈĞ¡ÓÚÉÈÇø´óĞ¡ */
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
            else                /* Êı¾İ³¤¶È´óÓÚÉÈÇø´óĞ¡ */
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

/**
  * @FunctionName: sf_PageWrite
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:03:59 
  * @Purpose:      Ò³±à³Ì¡Ì¡Ì¡Ì
  * @param:        _pBuf        Êı¾İÔ´»º³åÇø
  * @param:        _uiWriteAddr Ä¿±êÇøÓòÊ×µØÖ·
  * @param:        _usSize       Êı¾İ¸öÊı£¬Ò³´óĞ¡µÄÕûÊı±¶(256×Ö½ÚµÄÕûÊı±¶)
*/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
    uint32_t i,j;
    if (g_tSF.ChipID == SST25VF016B_ID)
        {
            /* AAIÖ¸ÁîÒªÇó´«ÈëµÄÊı¾İ¸öÊıÊÇÅ¼Êı */
            if ((_usSize < 2) && (_usSize % 2))
                {
                    return ;
                }
            sf_WriteEnable();        /* ·¢ËÍĞ´Ê¹ÄÜÃüÁî */

            sf_SetCS(0);            /* Ê¹ÄÜÆ¬Ñ¡ */
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = CMD_AAI;        /* ·¢ËÍAAIÃüÁî(µØÖ·×Ô¶¯Ôö¼Ó±à³Ì) */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);    /* ·¢ËÍÉÈÇøµØÖ·µÄ¸ß8bit */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);    /* ·¢ËÍÉÈÇøµØÖ·ÖĞ8bit */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff));            /*·¢ËÍÉÈÇøµØÖ·µÍ8bit */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++);                        /* ·¢ËÍµÚÒ»¸öÊı¾İ */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++);                        /* ·¢ËÍµÚ2¸öÊı¾İ */
            bsp_spi1Transfer();
            sf_SetCS(0);            /* ½ûÖ¹Æ¬Ñ¡ */

            sf_WaitForWriteEnd();    /* µÈ´ı´®ĞĞflashÄÚ²¿Ğ´²Ù×÷Íê³É */

            _usSize -= 2;            /* ¼ÆËãÊ£Óà×Ö½ÚÊıÄ¿ */

            for (i = 0; i < _usSize / 2; ++i)
                {
                    sf_SetCS(0);    /* Ê¹ÄÜÆ¬Ñ¡ */
                    g_spiLen = 0;
                    g_spiTxBuf[g_spiLen++] = (CMD_AAI);
                    g_spiTxBuf[g_spiLen++] = (*_pBuf++);
                    g_spiTxBuf[g_spiLen++] = (*_pBuf++);
                    bsp_spi1Transfer();
                    sf_SetCS(1);    /* ½ûÖ¹Æ¬Ñ¡ */
                    sf_WaitForWriteEnd();/* µÈ´ı´®ĞĞflashÄÚ²¿²Ù×÷Íê³É */
                }

            /* ½øÈëĞ´±£»¤×´Ì¬ */
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
                    sf_WriteEnable();    /* ·¢ËÍĞ´Ê¹ÄÜÃüÁî */

                    sf_SetCS(0);        /* Ê¹ÄÜÆ¬Ñ¡ */
                    g_spiLen = 0;
                    g_spiTxBuf[g_spiLen++] = (0x02);                                /* ·¢ËÍAAIÃüÁî(µØÖ·×Ô¶¯Ôö¼Ó±à³Ì) */
                    g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);
                    g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);
                    g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xff);
                    for (i = 0; i < 256; ++i)
                        {
                            g_spiTxBuf[g_spiLen++] = (*_pBuf++);            /* ·¢ËÍÊı¾İ */
                        }
                    bsp_spi1Transfer();
                    sf_SetCS(1);        /* ½ûÖ¹Æ¬Ñ¡ */

                    sf_WaitForWriteEnd();/* µÈ´ı´®ĞĞflashÄÚ²¿²Ù×÷Íê³É */
                    _uiWriteAddr += 256;
                }

            /* ½øÈëĞ´±£»¤×´Ì¬ */
            sf_SetCS(0);
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
            bsp_spi1Transfer();
            sf_SetCS(1);

            sf_WaitForWriteEnd();        /* µÈ´ı´®ĞĞflashÄÚ²¿²Ù×÷Íê³É */
        }
}


/**
  * @FunctionName: sf_EraseSector
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ 17:21:35 
  * @Purpose:      ²Á³ıÖ¸¶¨µÄÉÈÇø¡Ì¡Ì¡Ì
  * @param:        _uiSectorAddr      ÉÈÇøµØÖ·
  * @return:       none
*/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
    sf_WriteEnable();        /* ·¢ËÍĞ´Ê¹ÄÜÃüÁî */

    /* ²Á³ıÉÈÇø²Ù×÷ */
    sf_SetCS(0);            /* Ê¹ÄÜÆ¬Ñ¡ */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_SE;
    /* ÔÚ·¢ËÍ²Á³ıÖ¸Áîºó,½ô½ÓÕâ·¢ËÍ24Î»µÄµØÖ· */
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xf0000) >> 16);
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xff00) >> 8);
    g_spiTxBuf[g_spiLen++] = (_uiSectorAddr & 0xff);
    bsp_spi1Transfer();
    sf_SetCS(1);            /* ½ûÖ¹Æ¬Ñ¡ */

    sf_WaitForWriteEnd();    /* µÈ´ı´®ĞĞflashÄÚ²¿²Ù×÷Íê³É */
}


