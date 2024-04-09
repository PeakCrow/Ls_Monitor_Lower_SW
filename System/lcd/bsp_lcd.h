#ifndef _BSP_LCD_H_
#define _BSP_LCD_H_
#include "sys.h"


typedef __IO uint16_t vuint16_t;

typedef struct
{
    uint16_t width;    //LCD¿í¶È
    uint16_t height;    //LCD¸ß¶È
    uint16_t id;        //LCD ID
    uint8_t dir;        //0,ÊúÆÁ£º1£¬ºáÆÁ
    uint16_t wramcmd;//¿ªÊ¼Ğ´gramÖ¸Áî
    uint16_t setxcmd;//ÉèÖÃx×ø±êÖ¸Áî
    uint16_t setycmd;//ÉèÖÃy×ø±êÖ¸Áî
}_lcd_dev;

extern _lcd_dev lcddev;        //¹ÜÀíLCDÖØÒª²ÎÊı
//LCDµÄ»­±ÊÑÕÉ«ºÍ±³¾°É«
extern uint32_t POINT_COLOR;        //Ä¬ÈÏºìÉ«
extern uint32_t BACK_COLOR;        //±³¾°ÑÕÉ«£¬Ä¬ÈÏÎª°×É«

#define LCD_LED PBout(15)     //LCD±³    â	PB15

typedef struct 
{
    vuint16_t LCD_REG;
    vuint16_t LCD_RAM;
}LCD_TypeDef;

//Ê¹ÓÃNOR/SRAMµÄ Bank1.sector4,µØÖ·Î»HADDR[27,26]=11 A6×÷ÎªÊı¾İÃüÁîÇø·ÖÏß 
//×¢ÒâÉèÖÃÊ±STM32ÄÚ²¿»áÓÒÒÆÒ»Î»¶ÔÆä!    111 1110=0X7E	
#define LCD_BASE    ((uint32_t)(0x6C000000 | 0x0000007E))
#define LCD            ((LCD_TypeDef *)LCD_BASE)


void bsp_Initlcd(void);
void LCD_WR_REG(vuint16_t);
void LCD_WR_DATA(vuint16_t);
uint16_t LCD_RD_DATA(void);
void LCD_WriteReg(uint16_t ,uint16_t);
uint16_t LCD_ReadReg(uint16_t);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t);
void LCD_Scan_Dir(uint8_t);
void LCD_Display_Dir(uint8_t);
void LCD_Clear(uint32_t);
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);
void LCD_DrawPoint(uint16_t x,uint16_t y);    
void LCD_Color_Fill(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t*);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);                           //Ìî³äµ¥É«
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
    
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint32_t color);    
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);                        //ÏÔÊ¾Ò»¸ö×Ö·û
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);        //ÏÔÊ¾Ò»¸ö×Ö·û´®,12/16×ÖÌå

//É¨Ãè·½Ïò¶¨Òå
#define L2R_U2D  0         //´Ó×óµ½ÓÒ,´ÓÉÏµ½ÏÂ
#define L2R_D2U  1         //´Ó×óµ½ÓÒ,´ÓÏÂµ½ÉÏ
#define R2L_U2D  2         //´ÓÓÒµ½×ó,´ÓÉÏµ½ÏÂ
#define R2L_D2U  3         //´ÓÓÒµ½×ó,´ÓÏÂµ½ÉÏ

#define U2D_L2R  4         //´ÓÉÏµ½ÏÂ,´Ó×óµ½ÓÒ
#define U2D_R2L  5         //´ÓÉÏµ½ÏÂ,´ÓÓÒµ½×ó
#define D2U_L2R  6         //´ÓÏÂµ½ÉÏ,´Ó×óµ½ÓÒ ¡Ì
#define D2U_R2L  7        //´ÓÏÂµ½ÉÏ,    ÓÓÒµ½×ó	 

#define DFT_SCAN_DIR  D2U_L2R  //Ä¬ÈÏµÄÉ¨Ãè·½Ïò

//»­±ÊÑÕÉ«
#define TFT_WHITE              0xFFFF
#define TFT_BLACK              0x0000      
#define TFT_BLUE              0x001F  
#define TFT_BRED             0XF81F
#define TFT_GRED              0XFFE0
#define TFT_GBLUE             0X07FF
#define TFT_RED                0xF800
#define TFT_MAGENTA            0xF81F
#define TFT_GREEN              0x07E0
#define TFT_CYAN               0x7FFF
#define TFT_YELLOW             0xFFE0
#define TFT_BROWN              0XBC40 //×ØÉ«
#define TFT_BRRED              0XFC07 //×ØºìÉ«
#define TFT_GRAY               0X8430 //»ÒÉ«




#endif

