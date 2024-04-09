#include "bsp_key.h"
/*
    °´¼ü½Ó¿Ú·ÖÅä??
    K0¼ü    º	P    4	µÍµçÆ½±íÊ¾°´??
    Kup¼ü    º	P    0	¸ßµçÆ½±íÊ¾°´??
*/

#define HARD_KEY_NUM    2    /* ÊµÌå°´¼ü¸öÊı */
#define KEY_COUNT        3    /* Á½¸ö¶ÀÁ¢°´¼ü + 1¸ö×éºÏ¼ü */

/* Ê¹ÄÜGPIOÊ±ÖÓ */
#define ALL_KEY_GPIO_CLK_ENABLE()    { \
    __HAL_RCC_GPIOE_CLK_ENABLE();    \
    __HAL_RCC_GPIOE_CLK_ENABLE();    \
};

/* ÒÀ´Î¶¨ÒåIOÒı½Å½á¹¹ÌåÖ¸Õë£¬IOÒı½ÅºÅ£¬¼¤»îµç??*/
typedef struct
{
    GPIO_TypeDef* gpio;
    uint16_t pin;
    uint8_t ActiveLevel;
}X_GPIO_T;

/* ¶¨Òå½á¹¹Ìå±ä??*/
static const X_GPIO_T s_gpio_list[HARD_KEY_NUM] = {
    {GPIOE, GPIO_PIN_2, 1},
    {GPIOE,    GPIO_PIN_3, 1},
    
};

/*
¶¨ÒåÒ»¸öºêº¯Êı¼ò»¯ºóĞø´ú??
ÅĞ¶ÏGPIOÒı½ÅÊÇ·ñÓĞĞ§°´ÏÂ
*/
static KEY_T s_tBtn[KEY_COUNT] = {0};
static KEY_FIFO_T s_tKey;    /* ????FIFO?????????? */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);
static void bsp_SetKeyParam(uint8_t _ucKeyID,uint16_t _LongTime,uint8_t _RepeatSpeed);
/*
*    ?????? KeyPinActive
*    ¹¦ÄÜËµÃ÷: ÅĞ¶Ï°´¼üÊÇ·ñ°´ÏÂ(Ó²¼ş??
*    ??   ?? _id£º°´¼üºÅ
*    ?????? 1£º±íÊ¾°´ÏÂ£»0±íÊ¾ÊÍ·Å
*    Ê±¼ä??022????9??5??8??
*/
static uint8_t KeyPinActive(uint8_t _id)
{
    uint8_t level;
    /* IDR¼Ä´æÆ÷ÓÃÀ´¶ÁÈ¡IOÒı½ÅµÄÊäÈë£¬16Î»¿É¶Á£¬A...I */
    //¸ßµçÆ½°´ÏÂ
    //Ö»ÓĞÊ¹ÓÃµÍµçÆ½×÷Îª¼¤»îĞÅºÅµÄÊ±ºò²»±Ø½øĞĞÌõ¼ş»òÔËËã£¬ÒòÎªÎ»Óë³öÀ´¶¼Îª0 ÓĞĞ§
    //¸ßµçÆ½µÄÇé¿öÏÂÖ»ÓĞµÚÒ»¸öÒı½ÅÊÇ¿ÉÓÃµÄ£¬ÀıÈç´ËÊ±µÄPA0
    //ÂéÁË
    if((s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == 4
        || (s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == 8)
    {
        level = 1;
    }
    else 
    {
        level = 0;
    }
    //printf("%d %d\r\n",(s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin),level);
    if(level == s_gpio_list[_id].ActiveLevel)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
*   º¯ Êı Ãû: IsKeyDownFunc
*   ¹¦ÄÜËµÃ÷:ÅĞ¶Ï°´¼üÊÇ·ñ°´ÏÂ£¬µ¥¼üºÍ×éºÏ¼üÇø??Èí¼ş??
*   ĞÎ    ²Î: _id:°´¼ü??
*   ·µ »Ø Öµ: 1£º±íÊ¾°´¼ü°´ÏÂ£»0£º±íÊ¾°´¼üÊÍ·Å
*   Ê±    ¼ä:2024Äê3ÔÂ31ÈÕ 15:05:32
*/
static uint8_t IsKeyDownFunc(uint8_t _id)
{
    /* ÊµÌåµ¥¼ü */
    if(_id < HARD_KEY_NUM)
    {
        uint8_t i;
        uint8_t count = 0;
        uint8_t save = 255;
        /* ÅĞ¶ÏÓĞ¼¸¸ö¼ü°´ÏÂ */
        for(i = 0;i < HARD_KEY_NUM;i++)
        {
            if(KeyPinActive(i))
            {
                count++;
                save = i;
            }
        }
        //printf("%d %d %d\r\n",count,save,_id);
        if(count == 1 && save == _id)
        {
            return 1;/* µ¥¸ö°´¼ü°´ÏÂÓĞĞ§ */
        }
        return 0;
    }
    
    /* ????K0Kup */
    if(_id == HARD_KEY_NUM + 0) // 0 - 1 - 2(????)
    {
        if(KeyPinActive(KID_K0) && KeyPinActive(KID_Kup))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}
/*
*    ?????? bsp_InitKey
*    ¹¦ÄÜËµÃ÷: °´¼üÇı¶¯³õÊ¼??
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????9??2??7??
*/
void bsp_InitKey(void)
{
    bsp_InitKeyVar();    /* ³õÊ¼»¯°´¼ü±ä??*/
    bsp_InitKeyHard();    /* ³õÊ¼»¯°´¼üÓ²??*/
}
/*
*    ?????? bsp_InitKeyHard
*    ¹¦ÄÜËµÃ÷: ³õÊ¼»¯°´¼üÓ²??
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????9??2??7??
*/
static void bsp_InitKeyHard(void)
{
    GPIO_InitTypeDef gpio_init;
    uint8_t i;
    /* ´ò¿ªGPIOÊ±ÖÓ */
    ALL_KEY_GPIO_CLK_ENABLE();
    
    /* ÅäÖÃËùÓĞµÄ°´¼üGPIOÎª¸¡¶¯ÊäÈëÄ£??Êµ¼ÊMCU¸´Î»ºó¾ÍÊÇÊäÈë×´?? */
    gpio_init.Mode = GPIO_MODE_INPUT;            /* ÉèÖÃÊäÈëÄ£Ê½ */
    gpio_init.Pull = GPIO_PULLDOWN;                /* ÏÂÀ­µç×èÊ¹ÄÜ */
    gpio_init.Speed= GPIO_SPEED_FREQ_VERY_HIGH;    /* GPIOËÙ¶ÈµÈ¼¶ */
    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        gpio_init.Pin = s_gpio_list[i].pin;
        HAL_GPIO_Init(s_gpio_list[i].gpio, &gpio_init);    
    }    
}

static void bsp_InitKeyVar(void)
{
    uint8_t i;
    
    /* ¶Ô°´¼üFIFO¶ÁĞ´Ö¸Õë?? */
    s_tKey.Read = 0;
    s_tKey.Write = 0;
    s_tKey.Read2 = 0;
    
    /* Ã¿¸ö°´¼ü½á¹¹Ìå³ÉÔ±±äÁ¿¸³Ò»×éÈ±Ê¡??*/
    for(i = 0;i < KEY_COUNT;i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;        /* ???????0???????????? */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;    /* ????????????????????? */
        s_tBtn[i].State = 0;                    /* ¦Ä???? */
        s_tBtn[i].RepeatSpeed = 5;                /* ??????????????0????????????,5????50ms????????? */
        s_tBtn[i].RepeatCount = 0;                /* ?????????? */
    }
    //????????????????????????????????????????????
    bsp_SetKeyParam(KID_Kup,100,6);//???ÁLkey up???????60ms???????
}

/*
*    ??????bsp_SetKeyParam 
*    ¹¦ÄÜËµÃ÷: ĞŞ¸Ä°´¼üµÄ²Î??
*    ??   ?? _ucKeyID£º°´¼üID£»_LongTime£º³¤°´Ê±¼ä£»_RepeatSpeed£º³¤°´ËÙ¶È
*    ?????? none
*    Ê±¼ä??022????0??9??6??
*/
void bsp_SetKeyParam(uint8_t _ucKeyID,uint16_t _LongTime,uint8_t _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;        /* ???????0??????????????? */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;/* ??????????????0???????????? */
    s_tBtn[_ucKeyID].RepeatCount = 0;            /* ?????????? */
}

/*
*    ?????? bsp_PutKey
*    ¹¦ÄÜËµÃ÷: ??¸ö¼üÖµÑ¹ÈëFIFO»º³åÇøÖĞ£¬¿ÉÓÃÓÚÄ£ÄâÒ»¸ö°´??
*    ??   ?? _KeyCode£º°´¼ü´ú??
*    ?????? none
*    Ê±¼ä??022????0??9??0??
*/
void bsp_PutKey(uint8_t _KeyCode)
{
    s_tKey.Buf[s_tKey.Write] = _KeyCode;
    if(++s_tKey.Write >= KEY_FIFO_SIZE)
        s_tKey.Write = 0;        /* FIFO¿Õ¼äĞ´Âú£¬Write»á±»ÖØĞÂ¸³ÖµÎª0 */
}
/*
*    ?????? bsp_GetKey
*    ¹¦ÄÜËµÃ÷: ´Ó°´¼üFIFO»º³åÇøÖĞ¶ÁÈ¡Ò»¸ö¼ü??
*    ??   ?? none
*    ?????? °´¼ü´úÂë
*    Ê±¼ä??022????0??9??6??
*/
uint8_t bsp_GetKey(void)
{
    uint8_t ret;
    if(s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;    /* writeºÍreadÖµÏàµÈ£¬´ú±íÃ»ÓĞ°´¼ü°´ÏÂ
                                »òÕßÒÑ¾­´ÓFIFOÖĞÈ¡×ßÁËÈ«²¿°´¼ü*/
    }else
    {
        ret = s_tKey.Buf[s_tKey.Read];        /* ¶ÁÈ¡µ±Ç°ReadÖĞµÄ??*/
        if(++s_tKey.Read >= KEY_FIFO_SIZE)    /* Èç¹ûReadµÄÖµ´óÓÚ°´¼üµÄÊıÁ¿ */
        {
            s_tKey.Read = 0;                /* ½«ReadÖµÖØĞÂ¸³ÖµÎª0 */
        }
        return ret;
    }
}
/*
*    ?????? bsp_DetectKey
*    ¹¦ÄÜËµÃ÷: ¼ì²âÒ»¸ö°´¼ü£¬·Ç×èÈû×´Ì¬£¬±ØĞë±»ÖÜÆÚĞÔµÄµ÷ÓÃ
*    ??   ?? i£º°´¼üÒı½ÅµÄÊıÄ¿
*    ?????? none
*    Ê±¼ä??022????0??1??9??
*/
static void bsp_DetectKey(uint8_t i)
{
    KEY_T *pBtn;
    
    pBtn = &s_tBtn[i];
    if(IsKeyDownFunc(i))//Ö´ĞĞ°´¼ü°´ÏÂµÄ´¦??
    {
        if(pBtn->Count < KEY_FILTER_TIME)    //count×÷ÎªÂË²¨¼ÆÊıÆ÷³õÊ¼ÖµÎª25ms
        {
            pBtn->Count = KEY_FILTER_TIME;    //??????????¦Ì????§³??50ms??????P??50ms?????????
        }else if(pBtn->Count < 2 * KEY_FILTER_TIME)//???????§³??100ms
        {
            pBtn->Count++;                    //ÂË²¨¼ÆÊıÆ÷×Ô??
        }else
        {
            if(pBtn->State == 0)            //0±íÊ¾°´¼üµ¯Æğ£¬ÔÚ½øÈë394ĞĞºó£¬°´ÏÂ×´Ì¬ÏûÊ§£¬±íÃ÷°´¼ü°´ÏÂÊ±¼äĞ¡ÓÚ25ms
            {
                pBtn->State = 1;            //´ËÊ±¸³??±íÊ¾°´¼ü°´ÏÂ
                
                /* ·¢ËÍ°´Å¥°´ÏÂµÄÏûÏ¢ */
                bsp_PutKey((uint8_t)(3 * i + 1));//µÚÒ»¸öKEY_ID½á¹¹Ìå°´¼ü°´ÏÂ£¬¶ÔÓ¦µÄ×´??
                                                 //??*0 + 1 = 1£¬´ú±íµÄÊÇKEY_ENUMÃ¶¾ÙÖĞµÄ1--KEY_0_DOWN
            }
            
            if(pBtn->LongTime > 0)
            {
                if(pBtn->LongCount < pBtn->LongTime)//Èç¹û°´¼ü°´ÏÂµÄÊ±¼äĞ¡ÓÚ°´¼üÅäÖÃµÄ³¤°´Ê±¼ä
                {
                    /* ·¢ËÍ°´Å¥³ÖĞø°´ÏÂµÄÏûÏ¢ */
                    if(++pBtn->LongCount == pBtn->LongTime)//µ±°´¼ü°´ÏÂµÄÊ±¼äµÈÓÚ°´¼üÅäÖÃµÄ³¤°´Ê±??
                    {
                        /* ¼üÖµ·ÅÈë°´¼üFIFO */
                        bsp_PutKey((uint8_t)(3 * i + 3));    //´ú±íµÄÊÇKEY_ENUMÃ¶¾ÙÖĞµÄ3--KEY_0_LONG
                    }
                }else
                {
                    if(pBtn->RepeatSpeed > 0)
                    {
                        if(++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                        {
                            pBtn->RepeatCount = 0;
                            /* ³¤°´¼üºó£¬Ã¿??0ms·¢ËÍÒ»¸ö°´??*/
                            bsp_PutKey((uint8_t)(3 * i + 1));
                        }
                    }
                }
            }
        }
    }
    else//Ö´ĞĞ°´¼üËÉÊÖµÄ´¦Àí£¬»òÕßÖ´ĞĞ°´¼üÃ»ÓĞ°´ÏÂµÄ´¦Àí
    {
        if(pBtn->Count > KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if(pBtn->Count != 0)
        {
            pBtn->Count--;
        }
        else
        {
            if(pBtn->State == 1)
            {
                pBtn->State = 0;
                
                /* ·¢ËÍ°´¼üµ¯ÆğµÄÏûÏ¢ */
                bsp_PutKey((uint8_t)(3 * i + 2));
            }
        }
        
        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}
/*
*    ?????? bsp_KeyScan10ms
*    ¹¦ÄÜËµÃ÷: É¨ÃèËùÓĞ°´¼ü£¬·Ç×èÈû£¬±»sustickÖĞ¶ÏÖÜÆÚĞÔµÄµ÷ÓÃ??0msÒ»??
*    ??   ?? none
*    ?????? none
*    Ê±¼ä??022????0??1??3??
*/
void bsp_Key_Scan10ms(void)
{
    uint8_t i;
    
    for(i = 0;i < KEY_COUNT;i++)
    {
        bsp_DetectKey(i);
    }
}

