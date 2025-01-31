#include "bsp_lcd.h"

//NT35510 -- 屏幕显示芯片
SRAM_HandleTypeDef TFTSRAM_Handler;//SRAM句柄(用于控制LCD)

//LCD的画笔颜色和背景色
uint32_t POINT_COLOR = 0xff000000;//画笔颜色
uint32_t BACK_COLOR = 0xffffffff;//背景色


_lcd_dev lcddev;//管理LCD的重要参数


//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(vuint16_t regval)
{
    regval = regval;//使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG = regval;//写入要写的    拇嫫餍蚝�	
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(vuint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}
//读LCD数据
//返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
    vuint16_t ram;//防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;//写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;//写入数据
}

//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);//写入要读的寄存器序号
    bsp_DelayUS(5);
    return LCD_RD_DATA();//返回读到的值
}
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;//写十六位    RAM	
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r,g,b,rgb;
    b = (c>>0)&0x1f;//取出5位
    g = (c>>5)&0x3f;//取出6位
    r = (c>>11)&0x1f;//取出5位
    rgb = (b<<11)+(g<<5)+(r<<0);//进行重新排列，为RGB格式
    return (rgb);
}
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(uint8_t i)
{
    while(i--);
}
//设置光标位置(对RGB屏无效)
//Xpos:横坐标
//Ypos:纵坐标
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
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);//设置光标位置
    LCD_WriteRAM_Prepare();//开始写入GRAM
    LCD->LCD_RAM = POINT_COLOR;
}
//读取个某�    的颜色值	 
//x,y:坐标
//返回值:此点的颜色
uint32_t LCD_ReadPoint(uint32_t x,uint32_t y)
{
    uint16_t r = 0,g = 0,b = 0;
    if(x >= lcddev.width || y >= lcddev.height)
        return 0;//超过了范围，直接返回
    LCD_SetCursor(x,y);//设置坐标
    if(lcddev.id == 0x5310)
        LCD_WR_REG(0X2E);//发送读GRAM指令
    r = LCD_RD_DATA();//dummy read
    opt_delay(2);
    r = LCD_RD_DATA();//实际坐标颜色
    opt_delay(2);
    b = LCD_RD_DATA();
    g = r&0xff;
    g <<= 8;//读出的颜色格式为GBR格式
            //对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

//SRAM底层驱动，时钟使能，引脚分配
//此函数会被HAL_SRAM_Init()调用
//hsram:SRAM句柄
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_FSMC_CLK_ENABLE();        //使能FSMC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();       //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();       //使能GPIOE时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();       //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();       //使能GPIOG时钟
    
    //初始化PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|\
                    GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;         //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;        //高速
    GPIO_Initure.Alternate=GPIO_AF12_FSMC;    //复用为FSMC
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //初始化
    
    //初始化PE7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                    GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    //初始化PF12
    GPIO_Initure.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    //初始化PG12
    GPIO_Initure.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
}

void bsp_Initlcd(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
    GPIO_Initure.Pin=GPIO_PIN_15;           //PB15,背光控制
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    __HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOG时钟
    GPIO_Initure.Pin=GPIO_PIN_14;           //PG14,复位控制 拉低复位
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure); 
    
    TFTSRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;                
    TFTSRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    
    
    TFTSRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK4;                     //使用NE4
    TFTSRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;  //地址/数据线不复用
    TFTSRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;              //SRAM
    TFTSRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16; //16位数据宽度
    TFTSRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE; //是否使能突发访问,仅对同步突发存储器有效,此处未用到
    TFTSRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;//等待信号的极性,仅在突发模式访问下有用
    TFTSRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    TFTSRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;    //存储器写使能
    TFTSRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;           //等待使能位,此处未用到
    TFTSRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_ENABLE;        //读写使用不同的时序
    TFTSRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;//是否使能同步传输模式下的等待信号,此处未用到
    TFTSRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;           //禁止突发写
    TFTSRAM_Handler.Init.ContinuousClock=FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
    
    
    //FMC读时序控制寄存器
    FSMC_ReadWriteTim.AddressSetupTime=0x0F;        //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
    FSMC_ReadWriteTim.AddressHoldTime=0;
    FSMC_ReadWriteTim.DataSetupTime=60;             //数据保存时间为    0个HCLK	=6*60=360ns
    FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//模式A
    //FMC写时序控制寄存器
    FSMC_WriteTim.BusTurnAroundDuration=0;          //总线周转阶段持续时间为0，此变量不赋值的话会莫名其妙的自动修改为4。导致程序运行正常
    FSMC_WriteTim.AddressSetupTime=9;               //地址建立时间（ADDSET）为9个HCLK =54ns 
    FSMC_WriteTim.AddressHoldTime=0;
    FSMC_WriteTim.DataSetupTime=8;                  //数据保存时间为6ns*9个HCLK=54n
    FSMC_WriteTim.AccessMode=FSMC_ACCESS_MODE_A;    //模式A
    HAL_SRAM_Init(&TFTSRAM_Handler,&FSMC_ReadWriteTim,&FSMC_WriteTim);
    
    bsp_DelayMS(50);
    
    LCD_WR_REG(0XDA00);    
    lcddev.id=LCD_RD_DATA();        //读回0X00
    LCD_WR_REG(0XDB00);    
    lcddev.id=LCD_RD_DATA();        //读回0X80
    lcddev.id<<=8;    
    //printf("%d",lcddev.id);
    LCD_WR_REG(0XDC00);    
    lcddev.id|=LCD_RD_DATA();       //读回0X00
    //printf("%d",lcddev.id);
    if(lcddev.id == 0x8000)
        lcddev.id = 0x5510;//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
    printf("lcd id:%x\r\n",lcddev.id);//打印LCD ID
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
    //初始化完成后，提速
    if(lcddev.id == 0x5510)//设置WR时序为最快
    {
        //重新配置写时序控制寄存器的时序
        FSMC_Bank1E->BWTR[6] &=~(0xf<<0);   //地址建立时间(ADDSET)清零
        FSMC_Bank1E->BWTR[6] &=~(0XF<<8);   //数据保存时间清零
        FSMC_Bank1E->BWTR[6] |= 3 << 0;     //地址建立时间为3个HCLK = 18ns
        FSMC_Bank1E->BWTR[6] |= 2 << 8;     //数据保存时间(DATAST)为6ns * 3个HCLK = 18ns
    }
    LCD_Display_Dir(1);//默认为横屏
    LCD_LED = 1;//点亮背光
    LCD_Clear(TFT_WHITE);
    PGout(14) = 1;
}
//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(uint8_t dir)
{
    lcddev.dir = dir;//横屏/竖屏
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
    else //横屏
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
    LCD_Scan_Dir(DFT_SCAN_DIR);//默认扫描方向
}
//设置LCD的自动扫描方向(对RGB屏无效)
//注意:其他函数可能会受到此函数设置的影响(尤其是9341),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9341/5310/5510/1963等IC已经实际测试

void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
//  uint16_t temp;
    if((lcddev.dir == 1) && (lcddev.id == 0x5510))//9341/5310/5510/1963,特殊处理
    {
        switch(dir)
        {
            case L2R_U2D://从左到右,从上到下
                regval|=(0<<7)|(0<<6)|(0<<5); 
                break;
            case L2R_D2U://从左到右,从下到上
                regval|=(1<<7)|(0<<6)|(0<<5); 
                break;
            case R2L_U2D://从右到左,从上到下
                regval|=(0<<7)|(1<<6)|(0<<5); 
                break;
            case R2L_D2U://从右到左,从下到上
                regval|=(1<<7)|(1<<6)|(0<<5); 
                break;     
            case U2D_L2R://从上到下,从左到右
                regval|=(0<<7)|(0<<6)|(1<<5); 
                break;
            case U2D_R2L://从上到下,从右到左
                regval|=(0<<7)|(1<<6)|(1<<5); 
                break;
            case D2U_L2R://从下到上,从左到右
                regval|=(1<<7)|(0<<6)|(1<<5); 
                break;
            case D2U_R2L://从下到上,从右到左
                regval|=(1<<7)|(1<<6)|(1<<5); 
                break;     
    }
    if(lcddev.id == 0x5510)
        dirreg = 0x3600;
    //5510不需要BGR
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
    totalpoint *= lcddev.height;//得到总点数
    LCD_SetCursor(0x00,0x0000);//设置光标位置
    LCD_WriteRAM_Prepare();//开始写入GRAM
    for(index = 0;index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}


//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
    int a,b;
    int di;
    a=0;b=r;
    di=3-(r<<1);             //判断下个点位置的标志
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
        //使用Bresenham算法画圆     
        if(di<0)
            di +=4*a+6;      
        else
        {
            di+=10+4*(a-b);   
            b--;
        }
    }
}
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t height,width;
    uint16_t i,j;
    width = ex - sx + 1;//得到填充的宽度
    height = ey - sy + 1;//得到填充的高度
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx,sy+i);//设置光标位置
        LCD_WriteRAM_Prepare();//开始写入GRAM
        for(j = 0; j < width; j++)
            LCD->LCD_RAM = color[i*width+j];//写入数据
    }
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
#if 1
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);     //得到字体一个字符对�    点阵集所占的字节数	
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206_lcd[num][t];         //调用1206字体
        else if(size==16)temp=asc2_1608_lcd[num][t];    //调用1608字体
        else if(size==24)temp=asc2_2412_lcd[num][t];    //调用2412字体
        else if(size==32)temp=asc2_3216_lcd[num][t];    //调用3216字体
        else return;                                    //没有的字库
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
            else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=lcddev.height)return;        //超区域了
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=lcddev.width)return;    //超区域了
                break;
            }
        }       
    }
}   
#endif

//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串�        嫉刂�		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
    uint8_t x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//快速画点
//x,y:坐标
//color:颜色
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
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{          
    uint16_t i,j;
    uint16_t xlen=0; 
    xlen=ex-sx+1;     
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);                    //设置光标位置 
        LCD_WriteRAM_Prepare();                 //开始写入GRAM
        for(j=0;j<xlen;j++)LCD->LCD_RAM=color;    //显示颜色 
    }  
}  
/**
 * @brief       设置窗口(对RGB屏无效), 并自动设置画点坐标到窗口左上角(sx,sy).
 * @param       sx,sy:窗口起始坐标(左上角)
 * @param       width,height:窗口宽度和高度,必须大于0!!
 *   @note      窗体大小:width*height.
 *
 * @retval      无
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


