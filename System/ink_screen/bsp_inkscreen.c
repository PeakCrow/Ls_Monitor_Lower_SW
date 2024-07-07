#include "bsp_inkscreen.h"


static void gpio_ink_init(void);
static void inkscreen_writebyte(uint8_t _value);
static void inkscreen_sendcommand(uint8_t _reg);
static void inkscreen_senddata(uint8_t _data);
static uint8_t inkscreen_readbusy(void);

/**
  * @FunctionName: GPIO_INK_Init
  * @Author:       trx
  * @DateTime:     2022Äê5ÔÂ10ÈÕ 18:59:21 
  * @Purpose:      ³ıÈ¥spiÍ¨Ñ¶ÒÔÍâµÄ¿ØÖÆÒı½ÅµÄ³õÊ¼»¯
  * @param:        void
  * @return:       none
*/
static void gpio_ink_init(void)
{
    GPIO_InitTypeDef gpio_initstruct = {0};

    /* Ê¹ÄÜÒı½ÅÊ±ÖÓ */
    INK_BUSY_CLK_ENABLE();
    INK_RST_CLK_ENABLE();
    INK_DC_CLK_ENABLE();
    INK_CS_CLK_ENABLE();

    /* ÅäÖÃÒı½ÅµÄÊä³öµÈ¼¶ */
    //HAL_GPIO_WritePin(INK_BUSY_GPIO_PORT,INK_BUSY_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_RST_GPIO_PORT,INK_RST_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_DC_GPIO_PORT,INK_DC_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_CS_GPIO_PORT,INK_CS_GPIO_PIN,GPIO_PIN_RESET);
    

    /* ÅäÖÃÒı½ÅÄ£Ê½ */
    /* RST PIN */
    gpio_initstruct.Pin         = INK_RST_GPIO_PIN;
    gpio_initstruct.Mode     = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull     = GPIO_NOPULL;
    gpio_initstruct.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(INK_RST_GPIO_PORT,&gpio_initstruct);

    /* DC PIN */
    gpio_initstruct.Pin         = INK_DC_GPIO_PIN;
    HAL_GPIO_Init(INK_DC_GPIO_PORT,&gpio_initstruct);

    /* CS PIN */
    gpio_initstruct.Pin         = INK_CS_GPIO_PIN;
    HAL_GPIO_Init(INK_CS_GPIO_PORT,&gpio_initstruct);

    /* BUSY PIN */
    gpio_initstruct.Pin         = INK_BUSY_GPIO_PIN;
    gpio_initstruct.Mode     = GPIO_MODE_INPUT;
    HAL_GPIO_Init(INK_BUSY_GPIO_PORT,&gpio_initstruct);
    
}
/**
  * @FunctionName: bsp_Inkscreen_write_byte
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ22ÈÕ22µã44·Ö 
  * @Purpose:      ÏòÄ«Ë®ÆÁspiĞ´Ò»¸ö×Ö½Ú
  * @param:        void
  * @return:       none
*/
static void inkscreen_writebyte(uint8_t _value)
{
    bsp_spi2Transfer(_value);
}
static void inkscreen_sendcommand(uint8_t _reg)
{
    PBout(12) = 0;
    PCout(0) = 0;
    inkscreen_writebyte(_reg);
    PCout(0) = 1;
}
static void inkscreen_senddata(uint8_t _data)
{
    PBout(12) = 1;
    PCout(0) = 0;
    inkscreen_writebyte(_data);
    PCout(0) = 1;
}
static uint8_t inkscreen_readbusy(void)
{
    App_Printf("ink screen is busying \r\n");
    uint8_t busy;
    do 
    {
        inkscreen_sendcommand(0x71);
        busy = PAin(1);
        busy = !(busy & 0x01);
    }
    while(busy);
    App_Printf("ink screen is release \r\n");
    inkscreen_delay_ms(200);
    return busy;
}
/**
  * @FunctionName: bsp_InksreenExit
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ22ÈÕ22µã44·Ö 
  * @Purpose:      Ä«Ë®ÆÁÍË³ö£¬Ö÷ÒªÊÇÀ­µÍ5vÒı½Å
  * @param:        void
  * @return:       none
*/
void bsp_InkscreenExit(void)
{
    PBout(12) = 0;
    PCout(0) = 0;
    PAout(7) = 0;        /* close 5v */
}

/**
  * @FunctionName: bsp_InksreenInit
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ22ÈÕ22µã44·Ö 
  * @Purpose:      Ä«Ë®ÆÁ³õÊ¼»¯-->Ó¦¸Ã½«Ğ´Í¼°¸º¯ÊıÒ²·Å¹ıÀ´
  * @param:        void
  * @return:       none
*/
void bsp_InkscreenInit(void)
{
    gpio_ink_init();
    PBout(12) = 0;
    PCout(0) = 0;
    PAout(7) = 1;

    bsp_InkscreenReset();

    /* ´ò¿ªµçÔ     */	
    inkscreen_sendcommand(0x04);  
    inkscreen_readbusy();
    /* panel setting ÒÇ±íÉèÖÃ */
    inkscreen_sendcommand(0x00);
    /* 0x0f 0x89 È«¶¼ÊÇ0x00¼Ä´æÆ÷µÄÄ¬ÈÏÅäÖÃ */
    inkscreen_senddata(0x0f);
    inkscreen_senddata(0x89);
    /* resolution setting ·Ö±æÂÊÉèÖÃ */
    inkscreen_sendcommand(0x61);
    /* Ë®Æ½·½Ïò128 -   1 = 127 */
    inkscreen_senddata (0x80);
    /* ÊúÖ±·½Ïò296 -     1 = 295£¬ĞèÒª1¸ö×Ö½Ú¼Ó1Î»À´±íÊ¾ */
    inkscreen_senddata (0x01);
    inkscreen_senddata (0x28);
    /* vcom and data interval setting vcomºÍÊı¾İ¼ä¸ôÉèÖÃ */
    inkscreen_sendcommand(0X50);
    /* ÅäÖÃÆÁÄ»ÑÕÉ«Êı¾İ°üº¬ÓĞºìÉ«¡¢ºÚÉ«ºÍ°×É« */
    inkscreen_senddata(0x77);

    bsp_InkscreeenClear();
}
/**
  * @FunctionName: bsp_InkscreenReset
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ22ÈÕ23µã01·Ö
  * @Purpose:      Ä«Ë®ÆÁ¸´Î»
  * @param:        void
  * @return:       none
*/
void bsp_InkscreenReset(void)
{
    PAout(7) = 1;
    inkscreen_delay_ms(200);
    PAout(7) = 0;
    inkscreen_delay_ms(5);
    PAout(1) = 1;
    inkscreen_delay_ms(200);
}
/**
  * @FunctionName: bsp_InkscreeenClear
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ23ÈÕ22µã39·Ö
  * @Purpose:      Ä«Ë®ÆÁÇåÆÁ
  * @param:        void
  * @return:       none
*/
void bsp_InkscreeenClear(void)
{
    uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
    uint16_t height = INKSCREEN_HEIGHT;

    /* ·¢ËÍºÚ/°×É«Êı¾İ */
    inkscreen_sendcommand(INKSCREEN_BW);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;j < width;j++)
        {
            inkscreen_senddata(0xff);
        }
    }
    /* ·¢ËÍºìÉ«Êı¾İ */
    inkscreen_sendcommand(INKSCREEN_RED);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;j < width;j++)
        {
            inkscreen_senddata(0xff);
        }
    }
    /* ÏÔÊ¾Ë¢ĞÂÖ¸Áî */
    inkscreen_sendcommand(INKSCREEN_DRF);
    inkscreen_readbusy();
}
/**
  * @FunctionName: bsp_InkscreeenSleep
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ23ÈÕ22µã47·Ö
  * @Purpose:      Ä«Ë®ÆÁË¯Ãß
  * @param:        void
  * @return:       none
*/
void bsp_InkscreeenSleep(void)
{
    /* µôµç */
    inkscreen_sendcommand(INKSCREEN_POF);
    inkscreen_readbusy();
    /* Éî¶ÈË¯Ãß */
    inkscreen_sendcommand(INKSCREEN_DSLP);
    inkscreen_senddata(INKSCREEN_CHC);
}
/**
  * @FunctionName: bsp_InkscreenDisplay
  * @Author:       trx
  * @DateTime:     2023Äê4ÔÂ23ÈÕ22µã47·Ö
  * @Purpose:      Ä«Ë®ÆÁÏÔÊ¾
  * @param:        void
  * @return:       none
*/
void bsp_InkscreenDisplay(const uint8_t *_blackimage,const uint8_t * _ryimage)
{
    uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
    uint16_t height = INKSCREEN_HEIGHT;

    /* ·¢ËÍºÚÉ«Êı¾İ */
    inkscreen_sendcommand(INKSCREEN_BW);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;i < width;j++)
        {
            inkscreen_senddata(_blackimage[i + j * width]);
        }
    }
    /* partial out ÅÅ³ıÓ°Ïì£¿ */
    inkscreen_sendcommand(INKSCREEN_PTOUT);

    /* ·¢ËÍºìÉ«Êı¾İ */
    inkscreen_sendcommand(INKSCREEN_RED);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;i < width;j++)
        {
            inkscreen_senddata(_ryimage[i + j * width]);
        }
    }
    /* partial out ÅÅ³ıÓ°Ïì£¿ */
    inkscreen_sendcommand(INKSCREEN_PTOUT);

    /* ÏÔÊ¾Ë¢ĞÂ */
    inkscreen_sendcommand(INKSCREEN_DRF);
    inkscreen_readbusy();
}

























