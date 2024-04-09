#ifndef _BSP_TOUCH_H_
#define _BSP_TOUCH_H_

#include "sys.h"

#define TP_PRES_DOWN 0X80    //ÆÁÄ»±»°´ÏÂ£¬È¡×î¸ßÎ»
#define TP_CATH_PRES 0X40   //ÓÐ°´¼ü°´ÏÂ£¬È¡´Î¸ßÎ»
#define CT_MAX_TOUCH 5        //µçÈÝÆÁÖ§³ÖµÄµãÊý£¬¹Ì¶¨Îª5µã
//´¥ÃþÆÁ¿ØÖÆÆ÷
typedef struct
{
    uint8_t (*init)(void);            //³õÊ¼»¯´¥ÃþÆÁ¿ØÖÆÆ÷
    uint8_t (*scan)(uint8_t);                //É¨Ãè´¥ÃþÆÁ.0,ÆÁÄ»É¨Ã    ;1,ÎïÀí×ø±ê;	 
    void (*adjust)(void);        //´¥ÃþÆÁÐ£×¼ 
    uint16_t x[CT_MAX_TOUCH];         //µ±Ç°×ø±ê
    uint16_t y[CT_MAX_TOUCH];        //µçÈÝÆÁÓÐ×î¶à5×é×ø±ê,µç×èÆÁÔòÓÃx[0],y[0]´ú±í:´Ë´ÎÉ¨ÃèÊ±,´¥ÆÁµÄ×ø±ê,ÓÃ
                                //x[4],y[4]´æ´¢µÚÒ»´Î°´ÏÂÊ±µÄ×ø±ê. 
    uint8_t  sta;                    //±ÊµÄ×´Ì¬ 
                                //b7:°´ÏÂ1/ËÉ¿ª0; 
                                //b6:0,Ã»ÓÐ°´¼ü°´ÏÂ;1,ÓÐ°´¼ü°´ÏÂ. 
                                //b5:±£Áô
                                //b4~b0:µçÈÝ´¥ÃþÆÁ°´ÏÂµÄµãÊý(0,±íÊ¾Î´°´ÏÂ,1±íÊ¾°´ÏÂ)
/////////////////////´¥ÃþÆÁÐ£×¼²ÎÊý(µçÈÝÆÁ²»ÐèÒªÐ£×¼)///////                                ///////								
    float xfac;                    
    float yfac;
    short xoff;
    short yoff;       
//ÐÂÔöµÄ²ÎÊý,µ±´¥ÃþÆÁµÄ×óÓÒÉÏÏÂÍêÈ«µßµ¹Ê±ÐèÒªÓÃµ½.
//b0:0,ÊúÆÁ(ÊÊºÏ×óÓÒÎªX×ø±ê,ÉÏÏÂÎªY×ø±êµÄTP)
//   1,ºáÆÁ(ÊÊºÏ×óÓÒÎªY×ø±ê,ÉÏÏÂÎªX×ø±êµÄTP) 
//b1~6:±£Áô.
//b7:0,µç×èÆÁ
//   1,µçÈÝÆÁ 
    uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;
//µç×èÆÁÐ¾Æ    Á¬½ÓÒý½Å	   
#define PEN          PBin(1)      //T_PEN
#define DOUT         PBin(2)       //T_MISO
#define TDIN         PFout(11)      //T_MOSI
#define TCLK         PBout(0)      //T_SCK
#define TCS          PCout(13)      //T_CS  Æ¬Ñ¡Òý½Å
///* ÖÐ¶ÏÒý½Å */
//#define TP_INT_EXTI_IRQ                 EXTI1_IRQn
///* ÖÐ¶Ï·þÎñº¯Êý */
//#define TP_IRQHandler                   EXTI1_IRQHandler
//µç×èÆÁ/µçÈÝÆÁ ¹²ÓÃº¯Êý
uint8_t TP_Scan(uint8_t tp);                                //É¨Ãè
uint8_t TP_Init(void);                                //³õÊ¼»¯
void bsp_InitLcdTouch(void);


/************************************************IICÍ¨Ñ¶****************************************/


//IO·½Ïò    èÖÃ	½«¼Ä´æÆ÷ÖÐµÄµÚ(2        1)Î»ºÍÏÂÒ»Î»ÖÃ0		¼Ä´æÆ÷µÄµÚ(2*11)Î»ÖÃ0
#define CT_SDA_IN() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}//PF11ÊäÈëÄ£Ê½
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;}//PF11Êä³öÄ£Ê½
//IO²Ù×÷º¯Êý
#define CT_IIC_SCL    PBout(0)    //SCL-PB0Òý½ÅÊä³ö¸ßµÍµçÆ½
#define CT_IIC_SDA    PFout(11)    //SDA-PF11Òý½ÅÊä³ö¸ßµÍµçÆ½
#define CT_READ_SDA    PFin(11)    //ÊäÈëSDA-PF11ÊäÈë¸ßµÍµçÆ½

//IICËùÓÐ²Ù×÷º¯Êý
void CT_IIC_Init(void);//³õÊ¼»¯IICµÄIO¿Ú
void CT_IIC_Start(void);//·¢ËÍIIC¿ªÊ¼ÐÅºÅ
void CT_IIC_Stop(void);//·¢ËÍIICÍ£Ö¹ÐÅºÅ
void CT_IIC_Send_Byte(uint8_t txd);//IIC·¢ËÍÒ»¸ö×Ö½Ú
uint8_t CT_IIC_Read_Byte(uint8_t ack);//IIC¶ÁÈ¡Ò»¸ö×Ö½Ú
uint8_t CT_IIC_Wait_Ack(void);//IICµÈ´ýACKÐÅºÅ
void CT_IIC_Ack(void);//IIC·¢ËÍACKÐÅºÅ
void CT_IIC_NAck(void);//IIC²»·¢ËÍACKÐÅºÅ


/************************************************GT9147Çý¶¯****************************************/

//IO²Ù×÷º¯Êý
#define GT_RST    PCout(13)    //GT9147¸´Î»Òý½Å
#define GT_INT    PBin(1)        //GT9147ÖÐ¶ÏÒý½Å

//I2C¶ÁÐ´ÃüÁî
#define    GT_CMD_WR    0X28    //Ð´ÃüÁî
#define GT_CMD_RD    0X29    //¶ÁÃüÁî

//GT9147²¿·Ö¼Ä´æÆ÷¶¨Òå
#define GT_CTRL_REG        0X8040    //GT9147¿ØÖÆ¼Ä´æÆ÷
#define GT_CFGS_REG        0X8047    //GT9147ÅäÖÃÆðÊ¼µØÖ·¼Ä´æÆ÷
#define GT_CHECK_REG    0X80FF    //GT9147Ð£ÑéºÍ¼Ä´æÆ÷
#define GT_PID_REG        0X8140    //GT9147²úÆ·ID¼Ä´æÆ÷

#define GT_GSTID_REG    0X814E    //GT9147µ±Ç°¼ì²âµ½µÄ´¥ÃþÇé¿ö
#define GT_TP1_REG        0X8150    //µÚÒ»¸ö´¥ÃþµãÊý¾ÝµØÖ·
#define GT_TP2_REG        0X8158    //µÚ¶þ¸ö´¥ÃþµãÊý¾ÝµØÖ·
#define GT_TP3_REG        0X8160    //µÚÈý¸ö´¥ÃþµãÊý¾ÝµØÖ·
#define GT_TP4_REG         0X8168    //µÚËÄ¸ö´¥ÃþµãÊý¾ÝµØÖ·
#define GT_TP5_REG        0X8170    //µÚÎå¸ö´¥ÃþµãÊý¾ÝµØÖ·
#define GT_TOUCH_AREA_L   0X8154
#define GT_TOUCH_AREA_H   0X8155//µÚÒ»¸ö´¥ÃþµãµÄ´¥ÃþÃæ»ý

uint8_t GT9147_Send_Cfg(uint8_t mode);
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
void GT947_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
uint8_t GT9147_Init(void);
uint8_t GT9147_Scan(uint8_t mode);


/*******************************µçÈÝÆÁ»­°å¹¦ÄÜ²âÊÔ*************************************************/
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color);
void gui_fill_circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color);
void Load_Drow_Dialog(void);
void ctp_test(void);

#endif

