#include "no_warnings.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "ov7725.h"
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"

extern void delay(unsigned msec);

static uint32_t my_ret = 0;

uint8_t ov7725_WriteReg(uint8_t Addr, uint8_t Data)
{
  uint32_t timeout = TIMEOUT_MAX;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C2, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2C2, 0x42, I2C_Direction_Transmitter);

  /* Test on I2C2 EV6 and clear it */
  timeout = TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /* Send I2C2 location address LSB */
  I2C_SendData(I2C2, (uint8_t)(Addr));

  /* Test on I2C2 EV8 and clear it */
  timeout = TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /* Send Data */
  I2C_SendData(I2C2, Data);

  /* Test on I2C2 EV8 and clear it */
  timeout = TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /* Send I2C2 STOP Condition */
  I2C_GenerateSTOP(I2C2, ENABLE);

  /* If operation is OK, return 0 */
  return 0;
}


uint8_t ov7725_ReadReg(uint8_t Addr)
{
    uint32_t timeout = TIMEOUT_MAX;
    uint8_t Data = 0;

    /* Generate the Start Condition */
    I2C_GenerateSTART(I2C2, ENABLE);

    /* Test on I2C2 EV5 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            return 0xFF;
        }
    }

    /* Send DCMI selcted device slave Address for write */
    I2C_Send7bitAddress(I2C2, 0x43, I2C_Direction_Transmitter);

    /* Test on I2C2 EV6 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            return 0xFF;
        }
    }
     /* Clear EV6 by setting again the PE bit */
     I2C_Cmd(I2C2, ENABLE);
    /* Send I2C2 location address LSB */
    I2C_SendData(I2C2, (uint8_t)(Addr));

    /* Test on I2C2 EV8 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            return 0xFF;
        }
    }

    /* Clear AF flag if arised */
    I2C2->SR1 |= (uint16_t)0x0400;
    I2C_GenerateSTOP(I2C2, ENABLE);
    /* Generate the Start Condition */
    I2C_GenerateSTART(I2C2, ENABLE);

    /* Test on I2C2 EV6 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            return 0xFF;
        }
    }

    /* Send DCMI selcted device slave Address for write */
    I2C_Send7bitAddress(I2C2, 0x43, I2C_Direction_Receiver);

    /* Test on I2C2 EV6 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            //break;
            return 0xFF;
        }
    }
    Data = 0xff;
    /* Prepare an NACK for the next data received */
    I2C_AcknowledgeConfig(I2C2, DISABLE);

    /* Test on I2C2 EV7 and clear it */
    timeout = TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) {
            return 0xFF;
        }
    }

    /* Prepare Stop after receiving data */
    I2C_GenerateSTOP(I2C2, ENABLE);

    /* Receive the Data */
    Data = I2C_ReceiveData(I2C2);
    /* return the read data */
    return Data;
}

void ov7725_set_context()
{
    /*TO DO*/
    uint8_t tmp = 0;
    tmp = ov7725_init();
    my_ret = 0xff00 | tmp;
}

uint8_t ov7725_init() {
    ov7725_WriteReg(0x12, 0x80);
    delay(400);
    my_ret =  ov7725_ReadReg(0x0b);
    ov7725_WriteReg(0x3d, 0x03);
    ov7725_WriteReg(0x17, 0x22);
    ov7725_WriteReg(0x18, 0xa4);
    ov7725_WriteReg(0x19, 0x07);
    ov7725_WriteReg(0x1a, 0xf0);
    ov7725_WriteReg(0x32, 0x00);

    if (FLOAT_AS_BOOL(global_data.param[PARAM_VIDEO_ONLY])) {
         ov7725_WriteReg(0x29, 0x24);//width:  10->64 18->96 28->160
         ov7725_WriteReg(0x2c, 0x36);//height: 20->64 30->96 3c->120
	}
	else {
         ov7725_WriteReg(0x29, 0x10);//width:  10->64 18->96 28->160
         ov7725_WriteReg(0x2c, 0x20);//height: 20->64 30->96 3c->120
	}
	ov7725_WriteReg(0x2a, 0x00);
    ov7725_WriteReg(0x11, 0x00);
    ov7725_WriteReg(0x42, 0x7f);
    ov7725_WriteReg(0x4d, 0x09);
    ov7725_WriteReg(0x63, 0xe0);
    ov7725_WriteReg(0x64, 0xff);
    ov7725_WriteReg(0x65, 0x2f);
    ov7725_WriteReg(0x66, 0x00);
    ov7725_WriteReg(0x67, 0x48);
    ov7725_WriteReg(0x13, 0xf0);
    ov7725_WriteReg(0x0d, 0x41);
    ov7725_WriteReg(0x0f, 0xc5);
    ov7725_WriteReg(0x14, 0x11);
    ov7725_WriteReg(0x22, 0xff); 
    ov7725_WriteReg(0x23, 0x01);
    ov7725_WriteReg(0x24, 0x40);
    ov7725_WriteReg(0x25, 0x30);
    ov7725_WriteReg(0x26, 0xa1);
    ov7725_WriteReg(0x2b, 0x9e);
    ov7725_WriteReg(0x6b, 0xaa);
    ov7725_WriteReg(0x13, 0xff);
    ov7725_WriteReg(0x90, 0x05);
    ov7725_WriteReg(0x91, 0x01);
    ov7725_WriteReg(0x92, 0x03);
    ov7725_WriteReg(0x93, 0x00);
    ov7725_WriteReg(0x94, 0xb0);
    ov7725_WriteReg(0x95, 0x9d);
    ov7725_WriteReg(0x96, 0x13);
    ov7725_WriteReg(0x97, 0x16);
    ov7725_WriteReg(0x98, 0x7b);
    ov7725_WriteReg(0x99, 0x91);
    ov7725_WriteReg(0x9a, 0x1e);
    ov7725_WriteReg(0x9b, 0x08);
    ov7725_WriteReg(0x9c, 0x20);
    ov7725_WriteReg(0x9e, 0x81);
    ov7725_WriteReg(0xa6, 0x04);
    ov7725_WriteReg(0x7e, 0x0c);
    ov7725_WriteReg(0x7f, 0x16);
    ov7725_WriteReg(0x80, 0x2a);
    ov7725_WriteReg(0x81, 0x4e);
    ov7725_WriteReg(0x82, 0x61);
    ov7725_WriteReg(0x83, 0x6f);
    ov7725_WriteReg(0x84, 0x7b);
    ov7725_WriteReg(0x85, 0x86);
    ov7725_WriteReg(0x86, 0x8e);
    ov7725_WriteReg(0x87, 0x97);
    ov7725_WriteReg(0x88, 0xa4);
    ov7725_WriteReg(0x89, 0xaf);
    ov7725_WriteReg(0x8a, 0xc5);
    ov7725_WriteReg(0x8b, 0xd7);
    ov7725_WriteReg(0x8c, 0xe8);
    ov7725_WriteReg(0x8d, 0x20);

    ov7725_WriteReg(0x13, 0x20);//
    ov7725_WriteReg(0x0e, 0x41);//
    ov7725_WriteReg(0x00, 0x00);//AGC – Gain control gain setting
    ov7725_WriteReg(0x08, 0x01);//exposure - high
    ov7725_WriteReg(0x10, 0xe0);//exposure - low

    return (my_ret&0xff);
}
