#include "BC95_CluB.h"
#include "string.h"


char Recv_Data_Buf[BUF_MAX_LENTH] = {0};


/*----------------------------------------------|AT COM|--------------|RET|---------|MAX_LEN|-----|TIME|-----------|POINT FUNCTION|-------*/
const AT_Com_Type_Query AT_COM_Test 	= DEF_COM_QUERY("AT\r", 	  "OK", 		20,				1000,			BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_BAND   = DEF_COM_QUERY("AT+BAND=5\r", 	"OK", 20, 200, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NRB    = DEF_COM_QUERY("AT+NRB\r", 	"REBOOTING", 0, 200, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NSOCL  = DEF_COM_QUERY("AT+NSOCL=0\r", "OK", 20, 200, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NSOST  = DEF_COM_QUERY("AT+NSOST=0,120.55.96.180,5000,2,AABB\r", 	"OK", 20, 1900, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NSOCR  = DEF_COM_QUERY("AT+NSOCR=DGRAM,17,8888,1\r", "OK", 20, 200, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NCDP   = DEF_COM_QUERY("AT+NCDP=117.78.47.187\r", "OK", 20, 200, BC95C_Query_Point_Func);
const AT_Com_Type_Query AT_COM_NMGS   = DEF_COM_QUERY("AT+NMGS=1,05\r", "OK", 20, 200, BC95C_Query_Point_Func);

/*---------------------------------------------- Custom AT -----------------------------------------*/

const AT_Com_Type_Custom AT_COM_Custom_CSQ   = DEF_COM_Custom("AT+CSQ\r",  "+CSQ:",bc95_net_data.net_csq, 20, 200, BC95C_Custom_CSQ_Func);
const AT_Com_Type_Custom AT_COM_Custom_IP    = DEF_COM_Custom("AT+CGPADDR\r", "+CGPADDR:0,", bc95_net_data.net_ip, 20, 200, BC95C_Custom_IP_Func);
const AT_Com_Type_Custom AT_COM_Custom_IMEI  = DEF_COM_Custom("AT+CGSN=1\r", 	"+CGSN:", bc95_net_data.net_csq, 20, 200, BC95C_Custom_IMEI_Func);
const AT_Com_Type_Custom AT_COM_Custom_TIME  = DEF_COM_Custom("AT+CCLK?\r", 	"+CCLK:", bc95_net_data.net_time, 30, 200, BC95C_Custom_CCLK_Func);
const AT_Com_Type_Custom AT_COM_Custom_CSCON = DEF_COM_Custom("AT+CSCON?\r", 	"+CSCON:",bc95_net_data.net_cscon, 20, 200, BC95C_Custom_CSCON_Func);
const AT_Com_Type_Custom AT_COM_Custom_CEREG = DEF_COM_Custom("AT+CEREG?\r", 	"+CEREG:",bc95_net_data.net_cereg, 20, 200, BC95C_Custom_CEREG_Func);
const AT_Com_Type_Custom AT_COM_Custom_CGATT = DEF_COM_Custom("AT+CGATT?\r", 	"+CGATT:",bc95_net_data.net_cgatt, 20, 200, BC95C_Custom_CGATT_Func);
const AT_Com_Type_Custom AT_COM_Custom_CFUN  = DEF_COM_Custom("AT+CFUN?\r", 	"+CFUN:",bc95_net_data.net_cfun, 20, 200, BC95C_Custom_CFUN_Func);
const AT_Com_Type_Custom AT_COM_Custom_NSONMI= DEF_COM_Custom(NULL, 0,bc95_net_data.net_nsonmi, 20, 2000, BC95C_Custom_NSONMI_Func);
const AT_Com_Type_Custom AT_COM_Custom_NSORF = DEF_COM_Custom("AT+NSORF=0,15\r", 	(uint8_t*)"0,120.55.96.180,5000,15,",bc95_net_data.net_nsorf, 50, 1000, BC95C_Custom_NSORF_Func);

//const AT_Com_Type_Custom AT_COM_Custom_TIME = {};
//const AT_Com_Type_Custom AT_COM_Custom_IMEI = {};



static uint8_t BC95_Send_Data(const char *SendData, uint8_t lenth, 
									char *RecvData, uint8_t Recv_buf_Len,
									uint32_t Wait_Time)
{
		//Send Data
		HAL_StatusTypeDef ret;
		HAL_UART_Abort(BC95_UART);
		memset(RecvData, 0, Recv_buf_Len);
		ret = HAL_UART_Transmit(BC95_UART, (uint8_t*)SendData, lenth, lenth*10);
		if(HAL_OK != ret)
		{
			return 0;
		}
		//Recv Data
		HAL_UART_Receive(BC95_UART, (uint8_t*)RecvData, Recv_buf_Len, Wait_Time);	
		for(int i=Recv_buf_Len; i>0; i--){
			if(Recv_Data_Buf[i] != 0x00){
				//��������ݣ��򷵻����ݳ���
				return i+1;
			}
		}
		return 0;	
}


//�˺���Ϊ��ѯ��ʹ�õ�ͨ�ú���������AT����ѯ�Ƿ񷵻�OK��������ص�ֵ�У��򷵻�1.
static uint8_t BC95C_Query_Point_Func(const uint8_t *SendData,			//Ҫ���͵�����
									uint8_t SendLenth, 						//Ҫ���͵ĳ���
									const uint8_t* AT_Expect, 				//�ڴ����ص�ֵ
									uint8_t  AT_Expect_len,					//�ڴ�ֵ�ĳ���
									uint8_t Retu_Max_lenth,					//���ķ��س���
									uint32_t wait_time)						//��ĵȴ�ʱ��
{
	int ret = 0;	
	ret = BC95_Send_Data((const char*)SendData, SendLenth, Recv_Data_Buf, Retu_Max_lenth, wait_time);
	if(ret >0 && ret > AT_Expect_len)
	{
		char *p = strstr(Recv_Data_Buf, (char *)AT_Expect);
		if(p != NULL){
			return 1;
		}
	}
	return 0;
}




//����һ�����ݣ���ȡ�õ�ǰ����������
/*���´����У�
SendData��Ҫ���͵�����
SendLen��Ҫ�������ݵĳ���
MaxRetLen���������Ե����ݳ���
Param��	Ҫ�ڽ����Ѱ�ҵ�ֵ
Param_lenth��ҪѰ��ֵ�ĳ���
Result_Data��ȡ��Ѱ��ֵ���ڵ���һ�У���ָ��ָ��Result_Data
waittime��Ҫ�ȴ���ʱ��
*/
static uint8_t BC95_Query_Param_OK(const uint8_t* SendData, uint8_t SendLen, uint8_t MaxRetLen,
		 							const uint8_t *Param, uint8_t Param_lenth, 
		 								uint8_t *Result_Data, uint32_t waittime)
{
	int ret = BC95_Send_Data((char*)SendData, SendLen, Recv_Data_Buf, MaxRetLen, waittime);
	if(ret > 0){
		char *p1 = strstr(Recv_Data_Buf, (char*)Param);
		if(p1 != NULL){
			char *p2 = strstr(p1, "\r");
			if(p2 != NULL){
				strncpy((char*)Result_Data, p1, p2-p1);
				return p2-p1;
			}
		}
	}
	
	return 0;
}


/*���´����У�
SendData: Ҫ���͵�����ָ�루Ĭ����������д�ã�
SendLenth��Ҫ���͵����ݳ��ȣ�Ĭ����������д�ã�
Param�������Ҫ�Ӳ�������������ӣ���������ATָ�����Ҫ��������Ҫ�����
Param_len:  Param�����ĳ��ȣ��ں������ڣ�ͨ���жϳ��Ⱦ����Ƿ�׷�Ӳ���
Result�� ��������ȡ�������ݣ����շ������ָ����
Retu_max_len����Ԥ֪�������
*/


static uint8_t BC95C_Custom_CSQ_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
		if(ret > 0){
			char* p = strstr(Recv_Data_Buf, ",");
			if(p != NULL)
			{
				strncpy(Result, Recv_Data_Buf+Param_len, p-Recv_Data_Buf-Param_len);
			}
		}
		return 0;
}

static uint8_t BC95C_Custom_IP_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
		int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, (const uint8_t*)"+CSQ:", sizeof("+CSQ:"), (uint8_t*)Recv_Data_Buf, 500);
	return 0;
}
static uint8_t BC95C_Custom_IMEI_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
		
	return 0;
}
static uint8_t BC95C_Custom_CCLK_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 30, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
	if(ret > 0){
		strncpy(Result, Recv_Data_Buf+Param_len, ret-Param_len);
		return 1;
	}
	
	return 0;
}
static uint8_t BC95C_Custom_CSCON_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
		if(ret > 0){
			char* p = strstr(Recv_Data_Buf, ",");
			if(p != NULL)
			{
				strncpy(Result, Recv_Data_Buf+7, p-Recv_Data_Buf-7);
			}
		}
return 0;
}
static uint8_t BC95C_Custom_CEREG_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
		if(ret > 0){
			char* p = strstr(Recv_Data_Buf, "\r");
			if(p != NULL)
			{
				strncpy(Result, Recv_Data_Buf+9, p-Recv_Data_Buf-9);
			}
		}
return 0;
}
static uint8_t BC95C_Custom_CGATT_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
		if(ret > 0){
			char* p = strstr(Recv_Data_Buf, ",");
			if(p != NULL)
			{
				strncpy(Result, Recv_Data_Buf+7, p-Recv_Data_Buf-7);
			}
		}
return 0;
}
static uint8_t BC95C_Custom_CFUN_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	int ret = BC95_Query_Param_OK(SendData, SendLenth, 20, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
		if(ret > 0){
			char* p = strstr(Recv_Data_Buf, "\r");
			if(p != NULL)
			{
				strncpy(Result, Recv_Data_Buf+8, p-Recv_Data_Buf-8);
			}
		}
return 0;
}
static uint8_t BC95C_Custom_NSONMI_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
	HAL_UART_Receive(BC95_UART, (uint8_t*)Recv_Data_Buf, Retu_max_len, wait_time);	
//		for(int i=Retu_max_len; i>0; i--){
//			if(Recv_Data_Buf[i] != 0x00){
//				//��������ݣ��򷵻����ݳ���
//				return i+1;
//			}
//		}
		char *p1 = strstr(Recv_Data_Buf, (char*)Param);
		if(p1 != NULL){
			char *p2 = strstr(p1, "\r");
			if(p2 != NULL){
				strncpy((char*)Result, p1, p2-p1);
		return 1;
	}
}
	return 0;
}
static uint8_t BC95C_Custom_NSORF_Func(const uint8_t *SendData, uint8_t SendLenth, uint8_t* Param, uint8_t Param_len, char* Result, uint8_t Retu_max_len, uint32_t wait_time)
{
int ret = BC95_Query_Param_OK(SendData, SendLenth, 70, Param, Param_len, (uint8_t*)Recv_Data_Buf, 500);
	if(ret > 0){
		strncpy(Result, Recv_Data_Buf+Param_len, ret-Param_len-2);
		return 1;
	}
	
	return 0;
}

