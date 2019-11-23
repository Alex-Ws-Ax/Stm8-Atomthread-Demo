#include "RC522.h"
#include "stm8l15x_gpio.h"
#include "Spi.h"
#define MAXRLEN 18
extern void delay(unsigned int ms);
void CardReaderInit(void)
{
  RC522_Init();
  PcdReset();
  PcdAntennaOff(); 
  delay(20);
  PcdAntennaOn();
  delay(10);
}
void RC522_Init(void)
{
  GPIO_Init(GPIOB,GPIO_Pin_3,GPIO_Mode_Out_PP_Low_Slow);//CS
  SpiInit();
}
void Rc522_RST(u8 value)
{
  if(value)
  {
    GPIO_SetBits(GPIOB,GPIO_Pin_3);
  }
  else 
  {
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
  }
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
int PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
  int status;  
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ClearBitMask(Status2Reg,0x08);
  WriteRawRC(BitFramingReg,0x07);
  SetBitMask(TxControlReg,0x03);
  
  ucComMF522Buf[0] = req_code;
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
  
  if ((status == MI_OK) && (unLen == 0x10))
  {    
    *pTagType     = ucComMF522Buf[0];
    *(pTagType+1) = ucComMF522Buf[1];
  }
  else
  {   status = MI_ERR;  
  }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
int PcdAnticoll(unsigned char *pSnr)
{
  int status;
  unsigned char i,snr_check=0;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  
  ClearBitMask(Status2Reg,0x08);
  WriteRawRC(BitFramingReg,0x00);
  ClearBitMask(CollReg,0x80);
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x20;
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
  
  if (status == MI_OK)
  {
    for (i=0; i<4; i++)
    {   
      *(pSnr+i)  = ucComMF522Buf[i];
      snr_check ^= ucComMF522Buf[i];
      
    }
    if (snr_check != ucComMF522Buf[i])
    {   status = MI_ERR;    }
  }
  
  SetBitMask(CollReg,0x80);
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
int PcdSelect(unsigned char *pSnr)
{
  int status;
  unsigned char i;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x70;
  ucComMF522Buf[6] = 0;
  for (i=0; i<4; i++)
  {
    ucComMF522Buf[i+2] = *(pSnr+i);
    ucComMF522Buf[6]  ^= *(pSnr+i);
  }
  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
  ClearBitMask(Status2Reg,0x08);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
  
  if ((status == MI_OK) && (unLen == 0x18))
  {   status = MI_OK;  }
  else
  {   status = MI_ERR;    }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
int PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
  int status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = auth_mode;
  ucComMF522Buf[1] = addr;
  for (i=0; i<6; i++)
  {    ucComMF522Buf[i+2] = *(pKey+i);   }
  for (i=0; i<6; i++)
  {    ucComMF522Buf[i+8] = *(pSnr+i);   }
  //   memcpy(&ucComMF522Buf[2], pKey, 6); 
  //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
  
  status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
  if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
int PcdRead(unsigned char addr,unsigned char *pData)
{
  int status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_READ;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x90))
    //   {   memcpy(pData, ucComMF522Buf, 16);   }
  {
    for (i=0; i<16; i++)
    {    *(pData+i) = ucComMF522Buf[i];   }
  }
  else
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
int PcdWrite(unsigned char addr,unsigned char *pData)
{
  int status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_WRITE;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    //memcpy(ucComMF522Buf, pData, 16);
    for (i=0; i<16; i++)
    {    ucComMF522Buf[i] = *(pData+i);   }
    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
int PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
  int status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = dd_mode;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    // memcpy(ucComMF522Buf, pValue, 4);
    for (i=0; i<16; i++)
    {    ucComMF522Buf[i] = *(pValue+i);   }
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    unLen = 0;
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {    status = MI_OK;    }
  }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
int PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
  int status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_RESTORE;
  ucComMF522Buf[1] = sourceaddr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = 0;
    ucComMF522Buf[1] = 0;
    ucComMF522Buf[2] = 0;
    ucComMF522Buf[3] = 0;
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {    status = MI_OK;    }
  }
  
  if (status != MI_OK)
  {    return MI_ERR;   }
  
  ucComMF522Buf[0] = PICC_TRANSFER;
  ucComMF522Buf[1] = goaladdr;
  
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
int PcdHalt(void)
{
  int status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_HALT;
  ucComMF522Buf[1] = 0;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
  unsigned char i,n;
  ClearBitMask(DivIrqReg,0x04);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80);
  for (i=0; i<len; i++)
  {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
  WriteRawRC(CommandReg, PCD_CALCCRC);
  i = 0xFF;
  do 
  {
    n = ReadRawRC(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));
  pOutData[0] = ReadRawRC(CRCResultRegL);
  pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
int PcdReset(void)
{
  //unsigned char i;
  Rc522_RST(1);
  delay(1);                
  Rc522_RST(0);
  delay(1);    
  Rc522_RST(1);
  delay(1);                   
  
  WriteRawRC(CommandReg,PCD_RESETPHASE);
  
  
  delay(1);                
  WriteRawRC(ModeReg,0x3C);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
  unsigned char Reg=0;
  Reg=ReadRawRC(ModeReg);
  if(Reg==0x3C)
  {
    Reg=0;
  }
  WriteRawRC(TReloadRegL,30);           
  WriteRawRC(TReloadRegH,0);
  WriteRawRC(TModeReg,0x8D);
  WriteRawRC(TPrescalerReg,0x3E);
  WriteRawRC(TxAutoReg,0x40);
  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
  unsigned char ucAddr;
  unsigned char ucResult=0;
  ucAddr = ((Address<<1)&0x7E)|0x80;
  SpiCSLow();
  SpiSendRecv(ucAddr);
  ucResult=SpiSendRecv(0);
  SpiCSHigh();
  return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
  unsigned char  ucAddr;
  ucAddr = ((Address<<1)&0x7E);
  SpiCSLow();
  SpiSendRecv(ucAddr);
  SpiSendRecv(value);
  SpiCSHigh();
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
int PcdComMF522(unsigned char Command, 
                unsigned char *pInData, 
                unsigned char InLenByte,
                unsigned char *pOutData, 
                unsigned int  *pOutLenBit)
{
  int status = MI_ERR;
  unsigned char irqEn   = 0x00;
  unsigned char waitFor = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;
  switch (Command)
  {
  case PCD_AUTHENT:
    irqEn   = 0x12;
    waitFor = 0x10;
    break;
  case PCD_TRANSCEIVE:
    irqEn   = 0x77;
    waitFor = 0x30;
    break;
  default:
    break;
  }
  
  WriteRawRC(ComIEnReg,irqEn|0x80);
  ClearBitMask(ComIrqReg,0x80);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80);
  
  for (i=0; i<InLenByte; i++)
  {   WriteRawRC(FIFODataReg, pInData[i]);    }
  WriteRawRC(CommandReg, Command);
  
  
  if (Command == PCD_TRANSCEIVE)
  {    SetBitMask(BitFramingReg,0x80);  }
  
  i = 100;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
  do 
  {
    n = ReadRawRC(ComIrqReg);
    i--;
    delay(1);
  }
  while ((i!=0) && !(n&0x01) && !(n&waitFor));
  ClearBitMask(BitFramingReg,0x80);
  
  if (i!=0)
  {    
    status=ReadRawRC(ErrorReg);
    if(!(status&0x1B))
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
      {   status = MI_NOTAGERR;   }
      if (Command == PCD_TRANSCEIVE)
      {
        n = ReadRawRC(FIFOLevelReg);
        lastBits = ReadRawRC(ControlReg) & 0x07;
        if (lastBits)
        {   *pOutLenBit = (n-1)*8 + lastBits;   }
        else
        {   *pOutLenBit = n*8;   }
        if (n == 0)
        {   n = 1;    }
        if (n > MAXRLEN)
        {   n = MAXRLEN;   }
        for (i=0; i<n; i++)
        {   pOutData[i] = ReadRawRC(FIFODataReg);    }
      }
    }
    else
    {   status = MI_ERR;   }
    
  }
  
  
  SetBitMask(ControlReg,0x80);           // stop timer now
  WriteRawRC(CommandReg,PCD_IDLE); 
  return status;
}


/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
  unsigned char i;
  i = ReadRawRC(TxControlReg);
  if (!(i & 0x03))
  {
    SetBitMask(TxControlReg, 0x03);
  }
}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
}

int GetCardID(unsigned char * ID)
{
  int status=0;
  //	u8 temp=0;
  status = PcdRequest(PICC_REQALL, ID);//Ѱ��
  if (status != MI_OK)
  {    
    PcdReset();
    PcdAntennaOff(); 
    PcdAntennaOn(); 
    return MI_ERR;
  }	
  
#if 0
  printf("��������:");
  for(i=0;i<2;i++)
  {
    temp=ID[i];
    printf("%02X",temp);  
  }	
#endif
  status = PcdAnticoll(ID);//����ײ
  if(status != MI_OK)
  {    
    return MI_ERR; 
  }
  
  ////////����Ϊ�����ն˴�ӡ��������////////////////////////
#if 0
  printf("�����кţ�");	//�����ն���ʾ,
  for(i=0;i<4;i++)
  {
    temp=ID[i];
    printf("%X",temp);
    
  }
#endif
  return MI_OK;
}