/**
  * @file  iic_dup.c
  * @brief IIC
  * @par   date        version    author    remarks
  *        2016-03-21  v1.0       zbt       
  *
  */

/** ͷ�ļ������� ------------------------------------------------ */
#include "myiic.h"

/** ˽�к꣨���Ͷ��壩 -------------------------------------------- */ 
#define IIC1_SCL(pin_status)        HAL_GPIO_WritePin(SHT20_SCL_GPIO_Port, SHT20_SCL_Pin, pin_status);
#define IIC1_SDA(pin_status)        HAL_GPIO_WritePin(SHT20_SDA_GPIO_Port, SHT20_SDA_Pin, pin_status);
#define IIC1_SCL_IS_HIGH()          (HAL_GPIO_ReadPin(SHT20_SCL_GPIO_Port, SHT20_SCL_Pin) != GPIO_PIN_RESET)
#define IIC1_SDA_IS_HIGH()          (HAL_GPIO_ReadPin(SHT20_SDA_GPIO_Port, SHT20_SDA_Pin) != GPIO_PIN_RESET)

/** ˽�б��� --------------------------------------------------- */


/** �ⲿ���� --------------------------------------------------- */

/** ˽�к���ԭ�� ----------------------------------------------- */
static void iic_delay(void);

/** ���к��� --------------------------------------------------- */
/**
  * @brief  IIC����
  * @param  None
  * @retval None
  * @note   ��SCL���ڸߵ�ƽʱ��SDA����һ���½��أ�������IIC�����ź�
  */
void iic_start(void)
{
    IIC1_SCL(GPIO_PIN_SET);
    /** SDA����һ���½��� */
    IIC1_SDA(GPIO_PIN_SET);
    iic_delay(); 
    
    IIC1_SDA(GPIO_PIN_RESET);
    iic_delay(); 
    IIC1_SCL(GPIO_PIN_RESET);   /**< ����׼���������� */
    iic_delay();   
}

/**
  * @brief  IICֹͣ
  * @param  None
  * @retval None
  * @note   ��SCL���ڸߵ�ƽʱ��SDAһ�������أ�������IICֹͣ�ź�
            
  */
void iic_stop(void)
{
    IIC1_SCL(GPIO_PIN_RESET);
    iic_delay();
    /* SDA����һ�������� */
    IIC1_SDA(GPIO_PIN_RESET);
    iic_delay();
    
    IIC1_SCL(GPIO_PIN_SET);
    iic_delay();
    IIC1_SDA(GPIO_PIN_SET);
    iic_delay();
}

/**
  * @brief  IIC����һ�ֽ�����
  * @param  None
  * @retval None
  * @note   
  */
void iic_sendbyte(uint8_t byte)
{
    uint8_t i;
    
    /* ����һ�ֽڸ�7λ*/
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            IIC1_SDA(GPIO_PIN_SET);
        }
        else
        {
            IIC1_SDA(GPIO_PIN_RESET);
        }
        
        iic_delay();
        IIC1_SCL(GPIO_PIN_SET);
        iic_delay();
        IIC1_SCL(GPIO_PIN_RESET);
        
        if (i == 7)
        {
            IIC1_SDA(GPIO_PIN_SET);
        }
        
        byte <<= 1;
        iic_delay();
    }      
}

/**
  * @brief  IIC��ȡһ�ֽ�����
  * @param  None
  * @retval None
  * @note             
  */
uint8_t iic_readbyte(void)
{
    uint8_t i;
    uint8_t recv_value = 0;
    
    IIC1_SDA(GPIO_PIN_SET);
    iic_delay();
    
    for (i = 0; i < 8; i++)
    {
        IIC1_SCL(GPIO_PIN_SET);
        iic_delay();
        recv_value <<= 1;
        if (IIC1_SDA_IS_HIGH())
        {
            recv_value |= 0x01;
        }
        else
        {
            recv_value &= ~0x01;
        }
        
        iic_delay();
        IIC1_SCL(GPIO_PIN_RESET);
    }
    
    return recv_value;
}

/**
  * @brief  IIC�ȴ�Ӧ���ź�
  * @param  None
  * @retval ack_status: Ӧ��״̬��0��ʾӦ��1��ʾ�豸����Ӧ
  */
uint8_t iic_wait_ack(void)
{
    uint8_t ack_status = 0;
    
    /* �ڵȴ�Ӧ���ź�֮ǰ��Ҫ�ͷ����ߣ�����SDA��λ */
    IIC1_SDA(GPIO_PIN_SET);
    iic_delay();
    IIC1_SCL(GPIO_PIN_SET);
    iic_delay();
    
    if (IIC1_SDA_IS_HIGH())
    {    
        ack_status = 1;
        iic_stop();
    }
    else
    {
        ack_status = 0;
    }
    
    IIC1_SCL(GPIO_PIN_RESET);
    iic_delay();
    
    return ack_status;
}

/**
  * @brief  ��������������������Ӧ���ź�
  * @param  None
  * @retval None
  */
void iic_ack(void)
{
    IIC1_SDA(GPIO_PIN_RESET);
    iic_delay();
    
    IIC1_SCL(GPIO_PIN_SET);
    iic_delay();
    IIC1_SCL(GPIO_PIN_RESET);
    iic_delay();
    
    IIC1_SDA(GPIO_PIN_SET);
}

/**
  * @brief  ����������������������Ӧ���ź�
  * @param  None
  * @retval None
  */
void iic_nack(void)
{
    IIC1_SDA(GPIO_PIN_SET);
    iic_delay();
    
    IIC1_SCL(GPIO_PIN_SET);
    iic_delay();
    IIC1_SCL(GPIO_PIN_RESET);
    iic_delay();
}

/**
  * @brief  ���IIC�������豸״̬
  * @param  device_addr: �ӻ��豸��ַ 
  * @retval ack_status: 0 ��������or 1���쳣��
  * @note   ���������豸��ַ�ȴ��ӻ��豸Ӧ������ӻ���ȷӦ���ź�
            �����IIC�����Ϲҽ��˴ӻ��豸�������ʾ���������豸
  */
uint8_t iic_check_device_status(uint8_t device_addr)
{
    uint8_t ack_status;
    
    if (IIC1_SCL_IS_HIGH() && IIC1_SDA_IS_HIGH())
    {
        iic_start();
        
        iic_sendbyte(device_addr | IIC_WRITE);
        ack_status = iic_wait_ack();

        iic_stop();
        
        return ack_status;
    }
    
    return 1;
}

/** ˽�к��� --------------------------------------------------- */
/**
  * @brief  ����ģ��IICʱ�ļ���ʱ
  * @param  None
  * @retval None
  */
static void iic_delay(void)
{
    uint8_t i = 0;
    uint8_t delay = 5;
    
    while (delay--)
    {
        i = 10;
        while (i--);
    }
}

