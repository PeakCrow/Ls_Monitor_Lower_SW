#include "bsp_lcd.h"

//NT35510 -- ÆÁÄ»ÏÔÊ¾Ð¾Æ¬
SRAM_HandleTypeDef TFTSRAM_Handler;//SRAM¾ä±ú(ÓÃÓÚ¿ØÖÆLCD)

//LCDµÄ»­±ÊÑÕÉ«ºÍ±³¾°É«
uint32_t POINT_COLOR = 0xff000000;//»­±ÊÑÕÉ«
uint32_t BACK_COLOR = 0xffffffff;//±³¾°É«


_lcd_dev lcddev;//¹ÜÀíLCDµÄÖØÒª²ÎÊý


//Ð´¼Ä´æÆ÷º¯Êý
//regval:¼Ä´æÆ÷Öµ
void LCD_WR_REG(vuint16_t regval)
{
    regval = regval;//Ê¹ÓÃ-O2ÓÅ»¯µÄÊ±ºò,±ØÐë²åÈëµÄÑÓÊ±
    LCD->LCD_REG = regval;//Ð´ÈëÒªÐ´µÄ    Ä´æÆ÷ÐòºÅ	
}
//Ð´LCDÊý¾Ý
//data:ÒªÐ´ÈëµÄÖµ
void LCD_WR_DATA(vuint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}
//¶ÁLCDÊý¾Ý
//·µ»ØÖµ:¶Áµ½µÄÖµ
uint16_t LCD_RD_DATA(void)
{
    vuint16_t ram;//·ÀÖ¹±»ÓÅ»¯
    ram = LCD->LCD_RAM;
    return ram;
}
//Ð´¼Ä´æÆ÷
//LCD_Reg:¼Ä´æÆ÷µØÖ·
//LCD_RegValue:ÒªÐ´ÈëµÄÊý¾Ý
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;//Ð´ÈëÒªÐ´µÄ¼Ä´æÆ÷ÐòºÅ
    LCD->LCD_RAM = LCD_RegValue;//Ð´ÈëÊý¾Ý
}

//¶Á¼Ä´æÆ÷
//LCD_Reg:¼Ä´æÆ÷µØÖ·
//·µ»ØÖµ:¶Áµ½µÄÊý¾Ý
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);//Ð´ÈëÒª¶ÁµÄ¼Ä´æÆ÷ÐòºÅ
    bsp_DelayUS(5);
    return LCD_RD_DATA();//·µ»Ø¶Áµ½µÄÖµ
}
//¿ªÊ¼Ð´GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}
//LCDÐ´GRAM
//RGB_Code:ÑÕÉ«Öµ
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;//Ð´Ê®ÁùÎ»    RAM	
}

//´ÓILI93xx¶Á³öµÄÊý¾ÝÎªGBR¸ñÊ½£¬¶øÎÒÃÇÐ´ÈëµÄÊ±ºòÎªRGB¸ñÊ½¡£
//Í¨¹ý¸Ãº¯Êý×ª»»
//c:GBR¸ñÊ½µÄÑÕÉ«Öµ
//·µ»ØÖµ£ºRGB¸ñÊ½µÄÑÕÉ«Öµ
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r,g,b,rgb;
    b = (c>>0)&0x1f;//È¡³ö5Î»
    g = (c>>5)&0x3f;//È¡³ö6Î»
    r = (c>>11)&0x1f;//È¡³ö5Î»
    rgb = (b<<11)+(g<<5)+(r<<0);//½øÐÐÖØÐÂÅÅÁÐ£¬ÎªRGB¸ñÊ½
    return (rgb);
}
//µ±mdk -O1Ê±¼äÓÅ»¯Ê±ÐèÒªÉèÖÃ
//ÑÓÊ±i
void opt_delay(uint8_t i)
{
    while(i--);
}
//ÉèÖÃ¹â±êÎ»ÖÃ(¶ÔRGBÆÁÎÞÐ§)
//Xpos:ºá×ø±ê
//Ypos:×Ý×ø±ê
void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    if(lcddev.id == 0x5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos&0xff);
        
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos&0xff);
    }
}
//»­µã
//x,y:×ø±ê
//POINT_COLOR:´ËµãµÄÑÕÉ«
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);//ÉèÖÃ¹â±êÎ»ÖÃ
    LCD_WriteRAM_Prepare();//¿ªÊ¼Ð´ÈëGRAM
    LCD->LCD_RAM = POINT_COLOR;
}
//¶ÁÈ¡¸öÄ³µ    µÄÑÕÉ«Öµ	 
//x,y:×ø±ê
//·µ»ØÖµ:´ËµãµÄÑÕÉ«
uint32_t LCD_ReadPoint(uint32_t x,uint32_t y)
{
    uint16_t r = 0,g = 0,b = 0;
    if(x >= lcddev.width || y >= lcddev.height)
        return 0;//³¬¹ýÁË·¶Î§£¬Ö±½Ó·µ»Ø
    LCD_SetCursor(x,y);//ÉèÖÃ×ø±ê
    if(lcddev.id == 0x5310)
        LCD_WR_REG(0X2E);//·¢ËÍ¶ÁGRAMÖ¸Áî
    r = LCD_RD_DATA();//dummy read
    opt_delay(2);
    r = LCD_RD_DATA();//Êµ¼Ê×ø±êÑÕÉ«
    opt_delay(2);
    b = LCD_RD_DATA();
    g = r&0xff;
    g <<= 8;//¶Á³öµÄÑÕÉ«¸ñÊ½ÎªGBR¸ñÊ½
            //¶ÔÓÚ9341/5310/5510,µÚÒ»´Î¶ÁÈ¡µÄÊÇRGµÄÖµ,RÔÚÇ°,GÔÚºó,¸÷Õ¼8Î»
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

//SRAMµ×²ãÇý¶¯£¬Ê±ÖÓÊ¹ÄÜ£¬Òý½Å·ÖÅä
//´Ëº¯Êý»á±»HAL_SRAM_Init()µ÷ÓÃ
//hsram:SRAM¾ä±ú
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{    
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_FSMC_CLK_ENABLE();            //Ê¹ÄÜFSMCÊ±ÖÓ
    __HAL_RCC_GPIOD_CLK_ENABLE();            //Ê¹ÄÜGPIODÊ±ÖÓ
    __HAL_RCC_GPIOE_CLK_ENABLE();            //Ê¹ÄÜGPIOEÊ±ÖÓ
    __HAL_RCC_GPIOF_CLK_ENABLE();            //Ê¹ÄÜGPIOFÊ±ÖÓ
    __HAL_RCC_GPIOG_CLK_ENABLE();            //Ê¹ÄÜGPIOGÊ±ÖÓ
    
    //³õÊ¼»¯PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|\
                     GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;         //ÍÆÍì¸´ÓÃ
    GPIO_Initure.Pull=GPIO_PULLUP;            //ÉÏÀ­
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;        //¸ßËÙ
    GPIO_Initure.Alternate=GPIO_AF12_FSMC;    //¸´ÓÃÎªFSMC
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //³õÊ¼»¯
    
    //³õÊ¼»¯PE7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    //³õÊ¼»¯PF12
    GPIO_Initure.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    //³õÊ¼»¯PG12
    GPIO_Initure.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
}

void bsp_Initlcd(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();            //¿ªÆôGPIOBÊ±ÖÓ
    GPIO_Initure.Pin=GPIO_PIN_15;              //PB15,±³¹â¿ØÖÆ
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //ÍÆÍìÊä³ö
    GPIO_Initure.Pull=GPIO_PULLUP;          //ÉÏÀ­
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //¸ßËÙ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 

    TFTSRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;                
    TFTSRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    
    
    TFTSRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK4;                     //Ê¹ÓÃNE4
    TFTSRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;     //µØÖ·/Êý¾ÝÏß²»¸´ÓÃ
    TFTSRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;               //SRAM
    TFTSRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16; //16Î»Êý¾Ý¿í¶È
    TFTSRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE; //ÊÇ·ñÊ¹ÄÜÍ»·¢·ÃÎÊ,½ö¶ÔÍ¬²½Í»·¢´æ´¢Æ÷ÓÐÐ§,´Ë´¦Î´ÓÃµ½
    TFTSRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;//µÈ´ýÐÅºÅµÄ¼«ÐÔ,½öÔÚÍ»·¢Ä£Ê½·ÃÎÊÏÂÓÐÓÃ
    TFTSRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   //´æ´¢Æ÷ÊÇÔÚµÈ´ýÖÜÆÚÖ®Ç°µÄÒ»¸öÊ±ÖÓÖÜÆÚ»¹ÊÇµÈ´ýÖÜÆÚÆÚ¼äÊ¹ÄÜNWAIT
    TFTSRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;    //´æ´¢Æ÷Ð´Ê¹ÄÜ
    TFTSRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;           //µÈ´ýÊ¹ÄÜÎ»,´Ë´¦Î´ÓÃµ½
    TFTSRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_ENABLE;        //¶ÁÐ´Ê¹ÓÃ²»Í¬µÄÊ±Ðò
    TFTSRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;//ÊÇ·ñÊ¹ÄÜÍ¬²½´«ÊäÄ£Ê½ÏÂµÄµÈ´ýÐÅºÅ,´Ë´¦Î´ÓÃµ½
    TFTSRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;           //½ûÖ¹Í»·¢Ð´
    TFTSRAM_Handler.Init.ContinuousClock=FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;    


    //FMC¶ÁÊ±Ðò¿ØÖÆ¼Ä´æÆ÷
    FSMC_ReadWriteTim.AddressSetupTime=0x0F;           //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª16¸öHCLK 1/168M=6ns*16=96ns
    FSMC_ReadWriteTim.AddressHoldTime=0;
    FSMC_ReadWriteTim.DataSetupTime=60;                //Êý¾Ý±£´æÊ±¼äÎª    0¸öHCLK	=6*60=360ns
    FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//Ä£Ê½A
    //FMCÐ´Ê±Ðò¿ØÖÆ¼Ä´æÆ÷
    FSMC_WriteTim.BusTurnAroundDuration=0;            //×ÜÏßÖÜ×ª½×¶Î³ÖÐøÊ±¼äÎª0£¬´Ë±äÁ¿²»¸³ÖµµÄ»°»áÄªÃûÆäÃîµÄ×Ô¶¯ÐÞ¸ÄÎª4¡£µ¼ÖÂ³ÌÐòÔËÐÐÕý³£
    FSMC_WriteTim.AddressSetupTime=9;                  //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª9¸öHCLK =54ns 
    FSMC_WriteTim.AddressHoldTime=0;
    FSMC_WriteTim.DataSetupTime=8;                  //Êý¾Ý±£´æÊ±¼äÎª6ns*9¸öHCLK=54n
    FSMC_WriteTim.AccessMode=FSMC_ACCESS_MODE_A;    //Ä£Ê½A
    HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_WriteTim);    
    
    bsp_DelayMS(50);
    
    LCD_WR_REG(0XDA00);    
    lcddev.id=LCD_RD_DATA();        //¶Á»Ø0X    0	 
    LCD_WR_REG(0XDB00);    
    lcddev.id=LCD_RD_DATA();        //¶Á»Ø0X80
    lcddev.id<<=8;    
    //printf("%d",lcddev.id);
    LCD_WR_REG(0XDC00);    
    lcddev.id|=LCD_RD_DATA();        //¶Á»Ø0X00
    //printf("%d",lcddev.id);
    if(lcddev.id == 0x8000)
        lcddev.id = 0x5510;//NT35510¶Á»ØµÄIDÊÇ8000H,Îª·½±ãÇø·Ö,ÎÒÃÇÇ¿ÖÆÉèÖÃÎª5510
    printf("lcd id:%x\r\n",lcddev.id);//´òÓ¡LCD ID
    if(lcddev.id == 0x5510)
    {
        LCD_WriteReg(0xF000,0x55);
        LCD_WriteReg(0xF001,0xAA);
        LCD_WriteReg(0xF002,0x52);
        LCD_WriteReg(0xF003,0x08);
        LCD_WriteReg(0xF004,0x01);
        //AVDD Set AVDD 5.2V
        LCD_WriteReg(0xB000,0x0D);
        LCD_WriteReg(0xB001,0x0D);
        LCD_WriteReg(0xB002,0x0D);
        //AVDD ratio
        LCD_WriteReg(0xB600,0x34);
        LCD_WriteReg(0xB601,0x34);
        LCD_WriteReg(0xB602,0x34);
        //AVEE -5.2V
        LCD_WriteReg(0xB100,0x0D);
        LCD_WriteReg(0xB101,0x0D);
        LCD_WriteReg(0xB102,0x0D);
        //AVEE ratio
        LCD_WriteReg(0xB700,0x34);
        LCD_WriteReg(0xB701,0x34);
        LCD_WriteReg(0xB702,0x34);
        //VCL -2.5V
        LCD_WriteReg(0xB200,0x00);
        LCD_WriteReg(0xB201,0x00);
        LCD_WriteReg(0xB202,0x00);
        //VCL ratio
        LCD_WriteReg(0xB800,0x24);
        LCD_WriteReg(0xB801,0x24);
        LCD_WriteReg(0xB802,0x24);
        //VGH 15V (Free pump)
        LCD_WriteReg(0xBF00,0x01);
        LCD_WriteReg(0xB300,0x0F);
        LCD_WriteReg(0xB301,0x0F);
        LCD_WriteReg(0xB302,0x0F);
        //VGH ratio
        LCD_WriteReg(0xB900,0x34);
        LCD_WriteReg(0xB901,0x34);
        LCD_WriteReg(0xB902,0x34);
        //VGL_REG -10V
        LCD_WriteReg(0xB500,0x08);
        LCD_WriteReg(0xB501,0x08);
        LCD_WriteReg(0xB502,0x08);
        LCD_WriteReg(0xC200,0x03);
        //VGLX ratio
        LCD_WriteReg(0xBA00,0x24);
        LCD_WriteReg(0xBA01,0x24);
        LCD_WriteReg(0xBA02,0x24);
        //VGMP/VGSP 4.5V/0V
        LCD_WriteReg(0xBC00,0x00);
        LCD_WriteReg(0xBC01,0x78);
        LCD_WriteReg(0xBC02,0x00);
        //VGMN/VGSN -4.5V/0V
        LCD_WriteReg(0xBD00,0x00);
        LCD_WriteReg(0xBD01,0x78);
        LCD_WriteReg(0xBD02,0x00);
        //VCOM
        LCD_WriteReg(0xBE00,0x00);
        LCD_WriteReg(0xBE01,0x64);
        //Gamma Setting
        LCD_WriteReg(0xD100,0x00);
        LCD_WriteReg(0xD101,0x33);
        LCD_WriteReg(0xD102,0x00);
        LCD_WriteReg(0xD103,0x34);
        LCD_WriteReg(0xD104,0x00);
        LCD_WriteReg(0xD105,0x3A);
        LCD_WriteReg(0xD106,0x00);
        LCD_WriteReg(0xD107,0x4A);
        LCD_WriteReg(0xD108,0x00);
        LCD_WriteReg(0xD109,0x5C);
        LCD_WriteReg(0xD10A,0x00);
        LCD_WriteReg(0xD10B,0x81);
        LCD_WriteReg(0xD10C,0x00);
        LCD_WriteReg(0xD10D,0xA6);
        LCD_WriteReg(0xD10E,0x00);
        LCD_WriteReg(0xD10F,0xE5);
        LCD_WriteReg(0xD110,0x01);
        LCD_WriteReg(0xD111,0x13);
        LCD_WriteReg(0xD112,0x01);
        LCD_WriteReg(0xD113,0x54);
        LCD_WriteReg(0xD114,0x01);
        LCD_WriteReg(0xD115,0x82);
        LCD_WriteReg(0xD116,0x01);
        LCD_WriteReg(0xD117,0xCA);
        LCD_WriteReg(0xD118,0x02);
        LCD_WriteReg(0xD119,0x00);
        LCD_WriteReg(0xD11A,0x02);
        LCD_WriteReg(0xD11B,0x01);
        LCD_WriteReg(0xD11C,0x02);
        LCD_WriteReg(0xD11D,0x34);
        LCD_WriteReg(0xD11E,0x02);
        LCD_WriteReg(0xD11F,0x67);
        LCD_WriteReg(0xD120,0x02);
        LCD_WriteReg(0xD121,0x84);
        LCD_WriteReg(0xD122,0x02);
        LCD_WriteReg(0xD123,0xA4);
        LCD_WriteReg(0xD124,0x02);
        LCD_WriteReg(0xD125,0xB7);
        LCD_WriteReg(0xD126,0x02);
        LCD_WriteReg(0xD127,0xCF);
        LCD_WriteReg(0xD128,0x02);
        LCD_WriteReg(0xD129,0xDE);
        LCD_WriteReg(0xD12A,0x02);
        LCD_WriteReg(0xD12B,0xF2);
        LCD_WriteReg(0xD12C,0x02);
        LCD_WriteReg(0xD12D,0xFE);
        LCD_WriteReg(0xD12E,0x03);
        LCD_WriteReg(0xD12F,0x10);
        LCD_WriteReg(0xD130,0x03);
        LCD_WriteReg(0xD131,0x33);
        LCD_WriteReg(0xD132,0x03);
        LCD_WriteReg(0xD133,0x6D);
        LCD_WriteReg(0xD200,0x00);
        LCD_WriteReg(0xD201,0x33);
        LCD_WriteReg(0xD202,0x00);
        LCD_WriteReg(0xD203,0x34);
        LCD_WriteReg(0xD204,0x00);
        LCD_WriteReg(0xD205,0x3A);
        LCD_WriteReg(0xD206,0x00);
        LCD_WriteReg(0xD207,0x4A);
        LCD_WriteReg(0xD208,0x00);
        LCD_WriteReg(0xD209,0x5C);
        LCD_WriteReg(0xD20A,0x00);

        LCD_WriteReg(0xD20B,0x81);
        LCD_WriteReg(0xD20C,0x00);
        LCD_WriteReg(0xD20D,0xA6);
        LCD_WriteReg(0xD20E,0x00);
        LCD_WriteReg(0xD20F,0xE5);
        LCD_WriteReg(0xD210,0x01);
        LCD_WriteReg(0xD211,0x13);
        LCD_WriteReg(0xD212,0x01);
        LCD_WriteReg(0xD213,0x54);
        LCD_WriteReg(0xD214,0x01);
        LCD_WriteReg(0xD215,0x82);
        LCD_WriteReg(0xD216,0x01);
        LCD_WriteReg(0xD217,0xCA);
        LCD_WriteReg(0xD218,0x02);
        LCD_WriteReg(0xD219,0x00);
        LCD_WriteReg(0xD21A,0x02);
        LCD_WriteReg(0xD21B,0x01);
        LCD_WriteReg(0xD21C,0x02);
        LCD_WriteReg(0xD21D,0x34);
        LCD_WriteReg(0xD21E,0x02);
        LCD_WriteReg(0xD21F,0x67);
        LCD_WriteReg(0xD220,0x02);
        LCD_WriteReg(0xD221,0x84);
        LCD_WriteReg(0xD222,0x02);
        LCD_WriteReg(0xD223,0xA4);
        LCD_WriteReg(0xD224,0x02);
        LCD_WriteReg(0xD225,0xB7);
        LCD_WriteReg(0xD226,0x02);
        LCD_WriteReg(0xD227,0xCF);
        LCD_WriteReg(0xD228,0x02);
        LCD_WriteReg(0xD229,0xDE);
        LCD_WriteReg(0xD22A,0x02);
        LCD_WriteReg(0xD22B,0xF2);
        LCD_WriteReg(0xD22C,0x02);
        LCD_WriteReg(0xD22D,0xFE);
        LCD_WriteReg(0xD22E,0x03);
        LCD_WriteReg(0xD22F,0x10);
        LCD_WriteReg(0xD230,0x03);
        LCD_WriteReg(0xD231,0x33);
        LCD_WriteReg(0xD232,0x03);
        LCD_WriteReg(0xD233,0x6D);
        LCD_WriteReg(0xD300,0x00);
        LCD_WriteReg(0xD301,0x33);
        LCD_WriteReg(0xD302,0x00);
        LCD_WriteReg(0xD303,0x34);
        LCD_WriteReg(0xD304,0x00);
        LCD_WriteReg(0xD305,0x3A);
        LCD_WriteReg(0xD306,0x00);
        LCD_WriteReg(0xD307,0x4A);
        LCD_WriteReg(0xD308,0x00);
        LCD_WriteReg(0xD309,0x5C);
        LCD_WriteReg(0xD30A,0x00);

        LCD_WriteReg(0xD30B,0x81);
        LCD_WriteReg(0xD30C,0x00);
        LCD_WriteReg(0xD30D,0xA6);
        LCD_WriteReg(0xD30E,0x00);
        LCD_WriteReg(0xD30F,0xE5);
        LCD_WriteReg(0xD310,0x01);
        LCD_WriteReg(0xD311,0x13);
        LCD_WriteReg(0xD312,0x01);
        LCD_WriteReg(0xD313,0x54);
        LCD_WriteReg(0xD314,0x01);
        LCD_WriteReg(0xD315,0x82);
        LCD_WriteReg(0xD316,0x01);
        LCD_WriteReg(0xD317,0xCA);
        LCD_WriteReg(0xD318,0x02);
        LCD_WriteReg(0xD319,0x00);
        LCD_WriteReg(0xD31A,0x02);
        LCD_WriteReg(0xD31B,0x01);
        LCD_WriteReg(0xD31C,0x02);
        LCD_WriteReg(0xD31D,0x34);
        LCD_WriteReg(0xD31E,0x02);
        LCD_WriteReg(0xD31F,0x67);
        LCD_WriteReg(0xD320,0x02);
        LCD_WriteReg(0xD321,0x84);
        LCD_WriteReg(0xD322,0x02);
        LCD_WriteReg(0xD323,0xA4);
        LCD_WriteReg(0xD324,0x02);
        LCD_WriteReg(0xD325,0xB7);
        LCD_WriteReg(0xD326,0x02);
        LCD_WriteReg(0xD327,0xCF);
        LCD_WriteReg(0xD328,0x02);
        LCD_WriteReg(0xD329,0xDE);
        LCD_WriteReg(0xD32A,0x02);
        LCD_WriteReg(0xD32B,0xF2);
        LCD_WriteReg(0xD32C,0x02);
        LCD_WriteReg(0xD32D,0xFE);
        LCD_WriteReg(0xD32E,0x03);
        LCD_WriteReg(0xD32F,0x10);
        LCD_WriteReg(0xD330,0x03);
        LCD_WriteReg(0xD331,0x33);
        LCD_WriteReg(0xD332,0x03);
        LCD_WriteReg(0xD333,0x6D);
        LCD_WriteReg(0xD400,0x00);
        LCD_WriteReg(0xD401,0x33);
        LCD_WriteReg(0xD402,0x00);
        LCD_WriteReg(0xD403,0x34);
        LCD_WriteReg(0xD404,0x00);
        LCD_WriteReg(0xD405,0x3A);
        LCD_WriteReg(0xD406,0x00);
        LCD_WriteReg(0xD407,0x4A);
        LCD_WriteReg(0xD408,0x00);
        LCD_WriteReg(0xD409,0x5C);
        LCD_WriteReg(0xD40A,0x00);
        LCD_WriteReg(0xD40B,0x81);

        LCD_WriteReg(0xD40C,0x00);
        LCD_WriteReg(0xD40D,0xA6);
        LCD_WriteReg(0xD40E,0x00);
        LCD_WriteReg(0xD40F,0xE5);
        LCD_WriteReg(0xD410,0x01);
        LCD_WriteReg(0xD411,0x13);
        LCD_WriteReg(0xD412,0x01);
        LCD_WriteReg(0xD413,0x54);
        LCD_WriteReg(0xD414,0x01);
        LCD_WriteReg(0xD415,0x82);
        LCD_WriteReg(0xD416,0x01);
        LCD_WriteReg(0xD417,0xCA);
        LCD_WriteReg(0xD418,0x02);
        LCD_WriteReg(0xD419,0x00);
        LCD_WriteReg(0xD41A,0x02);
        LCD_WriteReg(0xD41B,0x01);
        LCD_WriteReg(0xD41C,0x02);
        LCD_WriteReg(0xD41D,0x34);
        LCD_WriteReg(0xD41E,0x02);
        LCD_WriteReg(0xD41F,0x67);
        LCD_WriteReg(0xD420,0x02);
        LCD_WriteReg(0xD421,0x84);
        LCD_WriteReg(0xD422,0x02);
        LCD_WriteReg(0xD423,0xA4);
        LCD_WriteReg(0xD424,0x02);
        LCD_WriteReg(0xD425,0xB7);
        LCD_WriteReg(0xD426,0x02);
        LCD_WriteReg(0xD427,0xCF);
        LCD_WriteReg(0xD428,0x02);
        LCD_WriteReg(0xD429,0xDE);
        LCD_WriteReg(0xD42A,0x02);
        LCD_WriteReg(0xD42B,0xF2);
        LCD_WriteReg(0xD42C,0x02);
        LCD_WriteReg(0xD42D,0xFE);
        LCD_WriteReg(0xD42E,0x03);
        LCD_WriteReg(0xD42F,0x10);
        LCD_WriteReg(0xD430,0x03);
        LCD_WriteReg(0xD431,0x33);
        LCD_WriteReg(0xD432,0x03);
        LCD_WriteReg(0xD433,0x6D);
        LCD_WriteReg(0xD500,0x00);
        LCD_WriteReg(0xD501,0x33);
        LCD_WriteReg(0xD502,0x00);
        LCD_WriteReg(0xD503,0x34);
        LCD_WriteReg(0xD504,0x00);
        LCD_WriteReg(0xD505,0x3A);
        LCD_WriteReg(0xD506,0x00);
        LCD_WriteReg(0xD507,0x4A);
        LCD_WriteReg(0xD508,0x00);
        LCD_WriteReg(0xD509,0x5C);
        LCD_WriteReg(0xD50A,0x00);
        LCD_WriteReg(0xD50B,0x81);

        LCD_WriteReg(0xD50C,0x00);
        LCD_WriteReg(0xD50D,0xA6);
        LCD_WriteReg(0xD50E,0x00);
        LCD_WriteReg(0xD50F,0xE5);
        LCD_WriteReg(0xD510,0x01);
        LCD_WriteReg(0xD511,0x13);
        LCD_WriteReg(0xD512,0x01);
        LCD_WriteReg(0xD513,0x54);
        LCD_WriteReg(0xD514,0x01);
        LCD_WriteReg(0xD515,0x82);
        LCD_WriteReg(0xD516,0x01);
        LCD_WriteReg(0xD517,0xCA);
        LCD_WriteReg(0xD518,0x02);
        LCD_WriteReg(0xD519,0x00);
        LCD_WriteReg(0xD51A,0x02);
        LCD_WriteReg(0xD51B,0x01);
        LCD_WriteReg(0xD51C,0x02);
        LCD_WriteReg(0xD51D,0x34);
        LCD_WriteReg(0xD51E,0x02);
        LCD_WriteReg(0xD51F,0x67);
        LCD_WriteReg(0xD520,0x02);
        LCD_WriteReg(0xD521,0x84);
        LCD_WriteReg(0xD522,0x02);
        LCD_WriteReg(0xD523,0xA4);
        LCD_WriteReg(0xD524,0x02);
        LCD_WriteReg(0xD525,0xB7);
        LCD_WriteReg(0xD526,0x02);
        LCD_WriteReg(0xD527,0xCF);
        LCD_WriteReg(0xD528,0x02);
        LCD_WriteReg(0xD529,0xDE);
        LCD_WriteReg(0xD52A,0x02);
        LCD_WriteReg(0xD52B,0xF2);
        LCD_WriteReg(0xD52C,0x02);
        LCD_WriteReg(0xD52D,0xFE);
        LCD_WriteReg(0xD52E,0x03);
        LCD_WriteReg(0xD52F,0x10);
        LCD_WriteReg(0xD530,0x03);
        LCD_WriteReg(0xD531,0x33);
        LCD_WriteReg(0xD532,0x03);
        LCD_WriteReg(0xD533,0x6D);
        LCD_WriteReg(0xD600,0x00);
        LCD_WriteReg(0xD601,0x33);
        LCD_WriteReg(0xD602,0x00);
        LCD_WriteReg(0xD603,0x34);
        LCD_WriteReg(0xD604,0x00);
        LCD_WriteReg(0xD605,0x3A);
        LCD_WriteReg(0xD606,0x00);
        LCD_WriteReg(0xD607,0x4A);
        LCD_WriteReg(0xD608,0x00);
        LCD_WriteReg(0xD609,0x5C);
        LCD_WriteReg(0xD60A,0x00);
        LCD_WriteReg(0xD60B,0x81);

        LCD_WriteReg(0xD60C,0x00);
        LCD_WriteReg(0xD60D,0xA6);
        LCD_WriteReg(0xD60E,0x00);
        LCD_WriteReg(0xD60F,0xE5);
        LCD_WriteReg(0xD610,0x01);
        LCD_WriteReg(0xD611,0x13);
        LCD_WriteReg(0xD612,0x01);
        LCD_WriteReg(0xD613,0x54);
        LCD_WriteReg(0xD614,0x01);
        LCD_WriteReg(0xD615,0x82);
        LCD_WriteReg(0xD616,0x01);
        LCD_WriteReg(0xD617,0xCA);
        LCD_WriteReg(0xD618,0x02);
        LCD_WriteReg(0xD619,0x00);
        LCD_WriteReg(0xD61A,0x02);
        LCD_WriteReg(0xD61B,0x01);
        LCD_WriteReg(0xD61C,0x02);
        LCD_WriteReg(0xD61D,0x34);
        LCD_WriteReg(0xD61E,0x02);
        LCD_WriteReg(0xD61F,0x67);
        LCD_WriteReg(0xD620,0x02);
        LCD_WriteReg(0xD621,0x84);
        LCD_WriteReg(0xD622,0x02);
        LCD_WriteReg(0xD623,0xA4);
        LCD_WriteReg(0xD624,0x02);
        LCD_WriteReg(0xD625,0xB7);
        LCD_WriteReg(0xD626,0x02);
        LCD_WriteReg(0xD627,0xCF);
        LCD_WriteReg(0xD628,0x02);
        LCD_WriteReg(0xD629,0xDE);
        LCD_WriteReg(0xD62A,0x02);
        LCD_WriteReg(0xD62B,0xF2);
        LCD_WriteReg(0xD62C,0x02);
        LCD_WriteReg(0xD62D,0xFE);
        LCD_WriteReg(0xD62E,0x03);
        LCD_WriteReg(0xD62F,0x10);
        LCD_WriteReg(0xD630,0x03);
        LCD_WriteReg(0xD631,0x33);
        LCD_WriteReg(0xD632,0x03);
        LCD_WriteReg(0xD633,0x6D);
        //LV2 Page 0 enable
        LCD_WriteReg(0xF000,0x55);
        LCD_WriteReg(0xF001,0xAA);
        LCD_WriteReg(0xF002,0x52);
        LCD_WriteReg(0xF003,0x08);
        LCD_WriteReg(0xF004,0x00);
        //Display control
        LCD_WriteReg(0xB100, 0xCC);
        LCD_WriteReg(0xB101, 0x00);
        //Source hold time
        LCD_WriteReg(0xB600,0x05);
        //Gate EQ control
        LCD_WriteReg(0xB700,0x70);
        LCD_WriteReg(0xB701,0x70);
        //Source EQ control (Mode 2)
        LCD_WriteReg(0xB800,0x01);
        LCD_WriteReg(0xB801,0x03);
        LCD_WriteReg(0xB802,0x03);
        LCD_WriteReg(0xB803,0x03);
        //Inversion mode (2-dot)
        LCD_WriteReg(0xBC00,0x02);
        LCD_WriteReg(0xBC01,0x00);
        LCD_WriteReg(0xBC02,0x00);
        //Timing control 4H w/ 4-delay
        LCD_WriteReg(0xC900,0xD0);
        LCD_WriteReg(0xC901,0x02);
        LCD_WriteReg(0xC902,0x50);
        LCD_WriteReg(0xC903,0x50);
        LCD_WriteReg(0xC904,0x50);
        LCD_WriteReg(0x3500,0x00);
        LCD_WriteReg(0x3A00,0x55);  //16-bit/pixel
        LCD_WR_REG(0x1100);
        bsp_DelayUS(120);
        LCD_WR_REG(0x2900);  
    }
    //³õÊ¼»¯Íê³Éºó£¬ÌáËÙ
    if(lcddev.id == 0x5510)//ÉèÖÃWRÊ±ÐòÎª×î¿ì
    {
        //ÖØÐÂÅäÖÃÐ´Ê±Ðò¿ØÖÆ¼Ä´æÆ÷µÄÊ±Ðò
        FSMC_Bank1E->BWTR[6] &=~(0xf<<0);//µØÖ·½¨Á¢Ê±¼ä(ADDSET)ÇåÁã
        FSMC_Bank1E->BWTR[6] &=~(0XF<<8);//Êý¾Ý±£´æÊ±¼äÇåÁã
        FSMC_Bank1E->BWTR[6] |= 3 << 0;//µØÖ·½¨Á¢Ê±¼äÎª3¸öHCLK = 18ns
        FSMC_Bank1E->BWTR[6] |= 2 << 8;//Êý¾Ý±£´æÊ±¼ä(DATAST)Îª6ns * 3¸öHCLK = 18ns
    }
    LCD_Display_Dir(1);//Ä¬ÈÏÎªºáÆÁ
    LCD_LED = 1;//µãÁÁ±³¹â
    LCD_Clear(TFT_WHITE);
}
//ÉèÖÃLCDÏÔÊ¾·½Ïò
//dir:0,ÊúÆÁ£»1,ºáÆÁ
void LCD_Display_Dir(uint8_t dir)
{
    lcddev.dir = dir;//ºáÆÁ/ÊúÆÁ
    if(dir == 0)
    {
        lcddev.width = 240;
        lcddev.height = 320;
        if(lcddev.id == 0x5510)
        {
            lcddev.wramcmd=0X2C00;
             lcddev.setxcmd=0X2A00;
            lcddev.setycmd=0X2B00; 
            lcddev.width=480;
            lcddev.height=800;    
        }
    }
    else //ºáÆÁ
    {
        lcddev.width = 320;
        lcddev.height = 240;
        if(lcddev.id == 0x5510)
        {
            lcddev.wramcmd=0X2C00;
             lcddev.setxcmd=0X2A00;
            lcddev.setycmd=0X2B00; 
            lcddev.width=800;
            lcddev.height=480;          
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR);//Ä¬ÈÏÉ¨Ãè·½Ïò
}
//ÉèÖÃLCDµÄ×Ô¶¯É¨Ãè·½Ïò(¶ÔRGBÆÁÎÞÐ§)
//×¢Òâ:ÆäËûº¯Êý¿ÉÄÜ»áÊÜµ½´Ëº¯ÊýÉèÖÃµÄÓ°Ïì(ÓÈÆäÊÇ9341),
//ËùÒÔ,Ò»°ãÉèÖÃÎªL2R_U2D¼´¿É,Èç¹ûÉèÖÃÎªÆäËûÉ¨Ãè·½Ê½,¿ÉÄÜµ¼ÖÂÏÔÊ¾²»Õý³£.
//dir:0~7,´ú±í8¸ö·½Ïò(¾ßÌå¶¨Òå¼ûlcd.h)
//9341/5310/5510/1963µÈICÒÑ¾­Ê    ¼Ê²âÊÔ	 

void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
//    uint16_t temp;
    if((lcddev.dir == 1) && (lcddev.id == 0x5510))//9341/5310/5510/1963,ÌØÊâ´¦Àí
    {
        switch(dir)
        {
            case L2R_U2D://´Ó×óµ½ÓÒ,´ÓÉÏµ½ÏÂ
                regval|=(0<<7)|(0<<6)|(0<<5); 
                break;
            case L2R_D2U://´Ó×óµ½ÓÒ,´ÓÏÂµ½ÉÏ
                regval|=(1<<7)|(0<<6)|(0<<5); 
                break;
            case R2L_U2D://´ÓÓÒµ½×ó,´ÓÉÏµ½ÏÂ
                regval|=(0<<7)|(1<<6)|(0<<5); 
                break;
            case R2L_D2U://´ÓÓÒµ½×ó,´ÓÏÂµ½ÉÏ
                regval|=(1<<7)|(1<<6)|(0<<5); 
                break;     
            case U2D_L2R://´ÓÉÏµ½ÏÂ,´Ó×óµ½ÓÒ
                regval|=(0<<7)|(0<<6)|(1<<5); 
                break;
            case U2D_R2L://´ÓÉÏµ½ÏÂ,´ÓÓÒµ½×ó
                regval|=(0<<7)|(1<<6)|(1<<5); 
                break;
            case D2U_L2R://´ÓÏÂµ½ÉÏ,´Ó×óµ½ÓÒ
                regval|=(1<<7)|(0<<6)|(1<<5); 
                break;
            case D2U_R2L://´ÓÏÂµ½ÉÏ,´ÓÓÒµ½×ó
                regval|=(1<<7)|(1<<6)|(1<<5); 
                break;     
    }
    if(lcddev.id == 0x5510)
        dirreg = 0x3600;
    //5510²»ÐèÒªBGR
    LCD_WriteReg(dirreg,regval);
    if(lcddev.id==0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
        LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
        LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
        LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
        LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
        LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
        LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
        LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
    }
}
    
}

void LCD_Clear(uint32_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;//µÃµ½×ÜµãÊý
    LCD_SetCursor(0x00,0x0000);//ÉèÖÃ¹â±êÎ»ÖÃ
    LCD_WriteRAM_Prepare();//¿ªÊ¼Ð´ÈëGRAM
    for(index = 0;index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}


//ÔÚÖ¸¶¨Î»ÖÃ»­Ò»¸öÖ¸¶¨´óÐ¡µÄÔ²
//(x,y):ÖÐÐÄµã
//r    :°ë¾¶
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
    int a,b;
    int di;
    a=0;b=r;      
    di=3-(r<<1);             //ÅÐ¶ÏÏÂ¸öµãÎ»ÖÃµÄ±êÖ¾
    while(a<=b)
    {
        LCD_DrawPoint(x0+a,y0-b);             //5
         LCD_DrawPoint(x0+b,y0-a);             //0           
        LCD_DrawPoint(x0+b,y0+a);             //4               
        LCD_DrawPoint(x0+a,y0+b);             //6 
        LCD_DrawPoint(x0-a,y0+b);             //1       
         LCD_DrawPoint(x0-b,y0+a);             
        LCD_DrawPoint(x0-a,y0-b);             //2             
          LCD_DrawPoint(x0-b,y0-a);             //7                  
        a++;
        //Ê¹ÓÃBresenhamËã·¨»­Ô²     
        if(di<0)
            di +=4*a+6;      
        else
        {
            di+=10+4*(a-b);   
            b--;
        }                             
    }
}
//ÔÚÖ¸¶¨ÇøÓòÄÚÌ            Ö¸¶¨ÑÕÉ«¿é			 
//(sx,sy),(ex,ey):Ìî³ä¾ØÐÎ¶Ô½Ç×ø±ê,ÇøÓò´óÐ¡Îª:(ex-sx+1)*(ey-sy+1)   
//color:ÒªÌî³äµÄÑÕÉ«
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t height,width;
    uint16_t i,j;
    width = ex - sx + 1;//µÃµ½Ìî³äµÄ¿í¶È
    height = ey - sy + 1;//µÃµ½Ìî³äµÄ¸ß¶È
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx,sy+i);//ÉèÖÃ¹â±êÎ»ÖÃ
        LCD_WriteRAM_Prepare();//¿ªÊ¼Ð´ÈëGRAM
        for(j = 0; j < width; j++)
            LCD->LCD_RAM = color[i*width+j];//Ð´ÈëÊý¾Ý
    }
}
    

//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û
//x,y:ÆðÊ¼×ø±ê
//num:ÒªÏÔÊ¾µÄ×Ö·û:" "--->"~"
//size:×ÖÌå´óÐ¡ 12/16/24/32
//mode:µþ¼Ó·½Ê½(1)»¹ÊÇ·Çµþ¼Ó·½Ê½(0)
#if 1
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{                                
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);        //µÃµ½×ÖÌåÒ»¸ö×Ö·û¶ÔÓ    µãÕó¼¯ËùÕ¼µÄ×Ö½ÚÊý	
     num=num-' ';//µÃµ½Æ«ÒÆºóµÄÖµ£¨ASCII×Ö¿âÊÇ´Ó¿Õ¸ñ¿ªÊ¼È¡Ä££¬ËùÒÔ-' '¾ÍÊÇ¶ÔÓ¦×Ö·ûµÄ×Ö¿â£©
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206_lcd[num][t];          //µ÷ÓÃ1206×ÖÌå
        else if(size==16)temp=asc2_1608_lcd[num][t];    //µ÷ÓÃ1608×ÖÌå
        else if(size==24)temp=asc2_2412_lcd[num][t];    //µ÷ÓÃ2412×ÖÌå
        else if(size==32)temp=asc2_3216_lcd[num][t];    //µ÷ÓÃ3216×ÖÌå
        else return;                                //Ã»ÓÐµÄ×Ö¿â
        for(t1=0;t1<8;t1++)
        {                
            if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
            else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=lcddev.height)return;        //³¬ÇøÓòÁË
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=lcddev.width)return;    //³¬ÇøÓòÁË
                break;
            }
        }       
    }                            
}   
#endif

//ÏÔÊ¾×Ö·û´®
//x,y:Æðµã×ø±ê
//width,height:ÇøÓò´óÐ¡  
//size:×ÖÌå´óÐ¡
//*p:×Ö·û´®Æ        ¼µØÖ·		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
    uint8_t x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' '))//ÅÐ¶ÏÊÇ²»ÊÇ·Ç·¨×Ö·û!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//ÍË³ö
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//¿ìËÙ»­µã
//x,y:×ø±ê
//color:ÑÕÉ«
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{       
    if(lcddev.id==0X9341||lcddev.id==0X5310)
    {
        LCD_WR_REG(lcddev.setxcmd); 
        LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);               
        LCD_WR_REG(lcddev.setycmd); 
        LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);               
    }else if(lcddev.id==0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
        LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);      
        LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
        LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF); 
    }else if(lcddev.id==0X1963)
    {
        if(lcddev.dir==0)x=lcddev.width-1-x;
        LCD_WR_REG(lcddev.setxcmd); 
        LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);         
        LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);         
        LCD_WR_REG(lcddev.setycmd); 
        LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);         
        LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);         
    }         
    LCD->LCD_REG=lcddev.wramcmd; 
    LCD->LCD_RAM=color; 
}
//ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äµ¥¸öÑÕÉ«
//(sx,sy),(ex,ey):Ìî³ä¾ØÐÎ¶Ô½Ç×ø±ê,ÇøÓò´óÐ¡Îª:(ex-sx+1)*(ey-sy+1)   
//color:ÒªÌî³äµÄÑÕÉ«
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{          
    uint16_t i,j;
    uint16_t xlen=0; 
    xlen=ex-sx+1;     
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);                      //ÉèÖÃ¹â±êÎ»ÖÃ 
        LCD_WriteRAM_Prepare();                 //¿ªÊ¼Ð´Èë    RAM	  
        for(j=0;j<xlen;j++)LCD->LCD_RAM=color;    //ÏÔÊ¾Ñ    É« 	    
    }  
}  
/**
 * @brief       ÉèÖÃ´°¿Ú(¶ÔRGBÆÁÎÞÐ§), ²¢×Ô¶¯ÉèÖÃ»­µã×ø±êµ½´°¿Ú×óÉÏ½Ç(sx,sy).
 * @param       sx,sy:´°¿ÚÆðÊ¼×ø±ê(×óÉÏ½Ç)
 * @param       width,height:´°¿Ú¿í¶ÈºÍ¸ß¶È,±ØÐë´óÓÚ0!!
 *   @note      ´°Ìå´óÐ¡:width*height.
 *
 * @retval      ÎÞ
 */
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0xFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0xFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0xFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0xFF);
    }

}


