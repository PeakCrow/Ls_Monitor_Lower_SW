#include "bsp_inkscreen.h"


static void gpio_ink_init(void);
static void inkscreen_writebyte(uint8_t _value);
static void inkscreen_sendcommand(uint8_t _reg);
static void inkscreen_senddata(uint8_t _data);
static uint8_t inkscreen_readbusy(void);

/*******************************************************************************
  * @FunctionName: GPIO_INK_Init
  * @Author:       trx
  * @DateTime:     2022年5月10日 18:59:21 
  * @Purpose:      除去spi通讯以外的控制引脚的初始化
  * @param:        void
  * @return:       none
*******************************************************************************/
static void gpio_ink_init(void)
{
    GPIO_InitTypeDef gpio_initstruct = {0};

    /* 使能引脚时钟 */
    INK_BUSY_CLK_ENABLE();
    INK_RST_CLK_ENABLE();
    INK_DC_CLK_ENABLE();
    INK_CS_CLK_ENABLE();

    /* 配置引脚的输出等级 */
    //HAL_GPIO_WritePin(INK_BUSY_GPIO_PORT,INK_BUSY_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_RST_GPIO_PORT,INK_RST_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_DC_GPIO_PORT,INK_DC_GPIO_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INK_CS_GPIO_PORT,INK_CS_GPIO_PIN,GPIO_PIN_RESET);
    

    /* 配置引脚模式 */
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
/*******************************************************************************
  * @FunctionName: bsp_Inkscreen_write_byte
  * @Author:       trx
  * @DateTime:     2023年4月22日22点44分 
  * @Purpose:      向墨水屏spi写一个字节
  * @param:        void
  * @return:       none
*******************************************************************************/
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
/*******************************************************************************
  * @FunctionName: bsp_InksreenExit
  * @Author:       trx
  * @DateTime:     2023年4月22日22点44分 
  * @Purpose:      墨水屏退出，主要是拉低5v引脚
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenExit(void)
{
    PBout(12) = 0;
    PCout(0) = 0;
    PAout(7) = 0;        /* close 5v */
}

/*******************************************************************************
  * @FunctionName: bsp_InksreenInit
  * @Author:       trx
  * @DateTime:     2023年4月22日22点44分 
  * @Purpose:      墨水屏初始化-->应该将写图案函数也放过来
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenInit(void)
{
    gpio_ink_init();
    PBout(12) = 0;
    PCout(0) = 0;
    PAout(7) = 1;

    bsp_InkscreenReset();

    /* 打开电�     */	
    inkscreen_sendcommand(0x04);  
    inkscreen_readbusy();
    /* panel setting 仪表设置 */
    inkscreen_sendcommand(0x00);
    /* 0x0f 0x89 全都是0x00寄存器的默认配置 */
    inkscreen_senddata(0x0f);
    inkscreen_senddata(0x89);
    /* resolution setting 分辨率设置 */
    inkscreen_sendcommand(0x61);
    /* 水平方向128 -   1 = 127 */
    inkscreen_senddata (0x80);
    /* 竖直方向296 -     1 = 295，需要1个字节加1位来表示 */
    inkscreen_senddata (0x01);
    inkscreen_senddata (0x28);
    /* vcom and data interval setting vcom和数据间隔设置 */
    inkscreen_sendcommand(0X50);
    /* 配置屏幕颜色数据包含有红色、黑色和白色 */
    inkscreen_senddata(0x77);

    bsp_InkscreeenClear();
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreenReset
  * @Author:       trx
  * @DateTime:     2023年4月22日23点01分
  * @Purpose:      墨水屏复位
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenReset(void)
{
    PAout(7) = 1;
    inkscreen_delay_ms(200);
    PAout(7) = 0;
    inkscreen_delay_ms(5);
    PAout(1) = 1;
    inkscreen_delay_ms(200);
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreeenClear
  * @Author:       trx
  * @DateTime:     2023年4月23日22点39分
  * @Purpose:      墨水屏清屏
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreeenClear(void)
{
    uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
    uint16_t height = INKSCREEN_HEIGHT;

    /* 发送黑/白色数据 */
    inkscreen_sendcommand(INKSCREEN_BW);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;j < width;j++)
        {
            inkscreen_senddata(0xff);
        }
    }
    /* 发送红色数据 */
    inkscreen_sendcommand(INKSCREEN_RED);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;j < width;j++)
        {
            inkscreen_senddata(0xff);
        }
    }
    /* 显示刷新指令 */
    inkscreen_sendcommand(INKSCREEN_DRF);
    inkscreen_readbusy();
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreeenSleep
  * @Author:       trx
  * @DateTime:     2023年4月23日22点47分
  * @Purpose:      墨水屏睡眠
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreeenSleep(void)
{
    /* 掉电 */
    inkscreen_sendcommand(INKSCREEN_POF);
    inkscreen_readbusy();
    /* 深度睡眠 */
    inkscreen_sendcommand(INKSCREEN_DSLP);
    inkscreen_senddata(INKSCREEN_CHC);
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreenDisplay
  * @Author:       trx
  * @DateTime:     2023年4月23日22点47分
  * @Purpose:      墨水屏显示
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenDisplay(const uint8_t *_blackimage,const uint8_t * _ryimage)
{
    uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
    uint16_t height = INKSCREEN_HEIGHT;

    /* 发送黑色数据 */
    inkscreen_sendcommand(INKSCREEN_BW);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;i < width;j++)
        {
            inkscreen_senddata(_blackimage[i + j * width]);
        }
    }
    /* partial out 排除影响？ */
    inkscreen_sendcommand(INKSCREEN_PTOUT);

    /* 发送红色数据 */
    inkscreen_sendcommand(INKSCREEN_RED);
    for(uint8_t i = 0;i < height;i++)
    {
        for(uint8_t j = 0;i < width;j++)
        {
            inkscreen_senddata(_ryimage[i + j * width]);
        }
    }
    /* partial out 排除影响？ */
    inkscreen_sendcommand(INKSCREEN_PTOUT);

    /* 显示刷新 */
    inkscreen_sendcommand(INKSCREEN_DRF);
    inkscreen_readbusy();
}

























