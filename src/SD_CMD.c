/***********************************************************************/
/*                                                                     */
/*  FILE        :SD_CMD.c                                        */
/*  DATE        :Thur, Oct 24, 2013                                     */
/*  DESCRIPTION :                               */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
#include "r_cg_macrodriver.h"
#include "prototype.h"
#include "extern.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"



// *******************************************************************************/
//   -Name(en)   :   SD_initialize                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h����������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int SD_initialize(void)
{	
	int i,j;
	
	//�R�}���h����
	switch (sd_start_status) {
		
		//(0) �ĊJ�����FSPI�ʐM���Ȃ�Έ�x��~������	
		case SD_OFF:
			//SD_wk = (unsigned char)CLOCK();
			if (SD_SIO_flag == 1U) 
			{
				R_CSI21_Stop();
				SD_SIO_flag = 0;
			}
			sd_start_status++;
			//break;
			/* continue */
			
		//(1) SPI�J�n
		case SD_POWER_ON:
			//if ((unsigned char)CLOCK() - SD_wk < SD_POWERUP_TIME)	break;
			//SPI�J�n  ���ӁF����������sck�N���b�N��400kHz�ȉ�
			R_CSI21_Start();
			SD_SIO_flag = 1;
			sd_start_status++;
			/* continue */
			
		//(2) CS�M���A�T�[�g���_�~�[�N���b�N���M	
		case SD_START_CS :
			//CS�M����High�ɕێ����āAsck��74�N���b�N�ȏ㑗��
			SD_CS = SD_CS_DISABLE;
			for (i = 0; i < 15; i++) 
			{
				//R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
			}

			//CS�M�����A�T�[�g
			SD_CS = SD_CS_ENABLE;	
			sd_start_status++;
			//return 0;
			/* continue */	
			
		//(3) CMD0���s	
		case SD_START_CMD0 :
			// ���Z�b�g�R�}���h �Z�b�g
			sd_trans[0] = SD_CMD0;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			//sd_trans[5] = SD_CRC_CMD0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			/*//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g*/
			
			// CMD0:���Z�b�g���� SPI���[�h�ֈڍs
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x01�����҂�
			for (j = 0; j < 1000; j++) {
				sd_trans[0] = SD_POLLING;
				R_CSI21_Send_Receive(&sd_trans[0],1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0]!= SD_POLLING ) break;
			}
			
			// ��������or�ُ퉞���Ȃ�(0)�����蒼�� 
			if (sd_check[0] != SD_INIT_OK) {					
				SD_CS = SD_CS_DISABLE;
				R_CSI21_Stop();
				sd_start_status = SD_OFF;
				return SD_START_ERROR;
			}			
			
			//SD��SDHC�ŏ������e�͈قȂ�̂�CMD8�Ŕ���
			sd_start_status++;
			KURUMI_LED_R = LED_OFF;
			KURUMI_LED_G = LED_ON;
			KURUMI_LED_B = LED_OFF;
			//return 0;
			/* continue */	
			
		//(4) CMD8���s		
		case SD_START_CMD8 :
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_CMD8;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0x01;
			sd_trans[4] = 0xAA;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// ����d���m�F�R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 20; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}			
			R_CSI21_Send_Receive(&sd_polling,4,&sd_check[1]);
			
			// �������Ȃ����MMC or SD ver1�Ƃ݂Ȃ���ACMD41(HCS=0)���s
			if (sd_check[0] != 0x01)
			{
				sd_start_status = SD_START_ACMD41_HCS0;
				return 0;
			}
//			else if(sd_check[0] == 0x01 && (0x0F & sd_check[4]) == 0x01 && sd_check[5] == 0xAA)
			else if(sd_check[0] == 0x01)
			{
				sd_start_status = SD_START_CMD58_RES01;
				return 0;				
			}
			else
			{
				return SD_START_ERROR;
			}
		
		//(5) CMD58���s		
		case SD_START_CMD58_RES01 :
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_CMD58;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// ����d���m�F�R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 20; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}			
			/*
			R_CSI21_Send_Receive(&sd_polling,4,&sd_check[1]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;*/
			
			// �ُ퉞���Ȃ��蒼��
			if (sd_check[0] != SD_INIT_OK)
			{
				return SD_START_ERROR;
			}
			else
			{
				R_CSI21_Send_Receive(&sd_polling,4,&sd_check[1]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				sd_start_status = SD_START_ACMD41_HCS1;
				return 0;
			}
			
		//(6)-a CMD55��CMD41���s(HCS=0)	
		case SD_START_ACMD41_HCS0 :
			// �g���R�}���h �Z�b�g
			sd_trans[0] = SD_CMD55;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// �g���R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 100; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}
			
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_ACMD41;
			sd_trans[1] = 0;	//HCS=0
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			/*SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g*/
			
			// �������R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 1000; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}
			
			// �������Ȃ����MMC�J�[�h�Ƃ݂Ȃ���CMD1���s
			if (sd_check[0] != SD_CMD_OK) 
			{
				sd_mode = MMC_MODE;
				sd_start_status = SD_START_CMD1;
				return 0;						
			}
			else
			{
				sd_mode = SD_MODE_V1;
				sd_start_status = SD_END;
				return 0;			
			}

				
		//(6)-b CMD1���s		
		case SD_START_CMD1 :
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_CMD1;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			/*//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g*/
			
			// �������R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 1000; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}
			
			// �ُ퉞���Ȃ珉�����R�}���h ���M��蒼��
			if (sd_check[0] != SD_CMD_OK)
			{
				return SD_START_ERROR;
			}
			else
			{
				sd_start_status = SD_END;
				return 0;			
			}
			
		//(6)-c CMD55��CMD41���s(HCS=1)	
		case SD_START_ACMD41_HCS1 :
			// �g���R�}���h �Z�b�g
			sd_trans[0] = SD_CMD55;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// �g���R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 1000; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}

			
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_ACMD41;
			sd_trans[1] = 0x40;	//HCS=1
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// �������R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x00(Ready)�����҂�
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			for (j = 0; j < 1000; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}
			
			// �ُ퉞���Ȃ�g���R�}���h�Ə������R�}���h ���M��蒼��
			if (sd_check[0] != SD_CMD_OK) return SD_START_ERROR;

			sd_start_status++;
			/* continue */	
		
		//(7) CMD58���s		
		case SD_START_CMD58_RES00 :
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_CMD58;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// ����d���m�F�R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 20; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}			
			
			// �ُ퉞���Ȃ��蒼��
			if (sd_check[0] != SD_CMD_OK) return SD_START_ERROR;			
			
			R_CSI21_Send_Receive(&sd_polling,4,&sd_check[1]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			//SD MODE���� CCS=1(SDHC) or CCS=0(SD Ver2)
			if( (sd_check[1] & 0x40) == 0x40 )
			{				
				sd_mode = SDHC_MODE;
			}
			else
			{
				sd_mode = SD_MODE_V2;				
			}

			sd_start_status++;
			/* continue */	
		
		//(8) CMD9���s		
		case SD_START_CMD9 :
			// �������R�}���h �Z�b�g
			sd_trans[0] = SD_CMD9;
			sd_trans[1] = 0;
			sd_trans[2] = 0;
			sd_trans[3] = 0;
			sd_trans[4] = 0;
			sd_trans[5] = calcSDCRC(&sd_trans[0]);
			
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			// ����d���m�F�R�}���h ���M
			R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// 0x00(Ready)�����҂�
			for (j = 0; j < 20; j++) {
				R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
				while(spi_progress_flag != 0x11){}
				spi_progress_flag = 0;
				if (sd_check[0] != SD_POLLING ) break;
			}			
			R_CSI21_Send_Receive(&sd_polling,17,&sd_csd[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			
			// �ُ퉞���Ȃ��蒼��
			if (sd_check[0] != SD_CMD_OK) return SD_START_ERROR;

			sd_start_status++;
			/* continue */	
			
		//(9) SD�J�n�I������		
		case SD_END :
			//1byte Dummy�M��
			SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
			
			//CS�M�����l�Q�[�g
			SD_CS = SD_CS_DISABLE;
			//SPI��~
			R_CSI21_Stop();
			//CLK�ύX SDR���16bit��SPI�ʐM���x�p�̃N���b�N�ݒ�
			//SDR11 = 0x0200;
			SDR11 = 0x0000;
			//SPI�J�n  
			R_CSI21_Start();
			
			sd_start_status++;
			KURUMI_LED_R = LED_OFF;
			KURUMI_LED_G = LED_OFF;
			KURUMI_LED_B = LED_ON;
			return SD_START_END;
		default :
			break;	
	}
}


// *******************************************************************************/
//   -Name(en)   :   SD_change_block_length                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�Z�N�^�[���ݒ菈��>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
void SD_change_sector_length(unsigned long sectorSize)
{		
	unsigned char a1,a2,a3,a4,dummy;
	unsigned int cmd16_res,i;
	
	
	SDsectorSize = sectorSize;
	
}



// *******************************************************************************/
//   -Name(en)   :   SD_single_read                                     �@�@�@�@ */
//   -Name(jp)   :   <SD�J�[�h�V���O���ǂݍ��ݏ���>                              */
//   -OutLine    :                       					 */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int SD_single_read(unsigned long dataAddress)

{		
	unsigned char a1,a2,a3,a4,dummy;
	unsigned int cmd17_res,i,j;
	
	//(1)�A�h���X���S�o�C�g�̈����ɕϊ�
	a1 = (unsigned char)((dataAddress&0xFF000000)>>24);
	a2 = (unsigned char)((dataAddress&0x00FF0000)>>16);
	a3 = (unsigned char)((dataAddress&0x0000FF00)>>8);
	a4 = (unsigned char)(dataAddress&0x000000FF);
	
	
	SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
	//time_count(10);
	
	//(7) 1byte Dummy�M��
	/*SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g*/
	
	
	for(i = 0; i < 20; i++)
	{
		//(2) CMD17���s
		sd_trans[0] = SD_CMD17;
		sd_trans[1] = a1;
		sd_trans[2] = a2;
		sd_trans[3] = a3;
		sd_trans[4] = a4;
		sd_trans[5] = calcSDCRC(&sd_trans[0]);
		
		R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
		
		//����Dummy�M��������Ɛ����Read�ł��Ȃ�SD�J�[�h�����������B
		//(3) 1byte Dummy�M��
		//SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
		//R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
		//while(spi_progress_flag != 0x11){}
		//spi_progress_flag = 0;	
		//SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
		
		//(4) 0x00(Ready)�����҂�
		for (j = 0; j < 500; j++) {
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			if (sd_check[0] == SD_CMD_OK ) break;
		}			
		
		
	///////////////////////////////////////////////////////	
	//(5) 0xFE�i�X�^�[�g�o�C�g�j�����҂�
		for (j = 0; j < 500; j++) {
		R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
		if (sd_check[0] == SD_READ_START ) break;
		}
		
		//�ُ퉞���Ȃ�(2)�����蒼��
		if (sd_check[0] == SD_READ_START)  break;
	}
	///////////////////////////////////////////////////////	
		
	/* ///////////////////////////////////////////////////////	
		//�ُ퉞���Ȃ�(2)�����蒼��
		if (sd_check[0] == SD_CMD_OK)  break;		
	}	
	
	//�ُ퉞���������ł�����Ȃ���΍ŏ������蒼��
	if (sd_check[0] != SD_CMD_OK) return SD_READ_ERROR;
	
	//(5) 0xFE�i�X�^�[�g�o�C�g�j�����҂�
	for (j = 0; j < 2000; j++) {
	//for (j = 0; j < 0xFFFF; j++) {
		R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
		if (sd_check[0] == SD_READ_START ) break;
	}			
	///////////////////////////////////////////////////////	*/
	
	//�ُ퉞���������ł�����Ȃ���΍ŏ������蒼��
	if (sd_check[0] != SD_READ_START) return SD_READ_ERROR;
	
	
	//(6) �f�[�^�ǂݍ���
	for(i=0; i<SDsectorSize; i++)
	{
		R_CSI21_Send_Receive(&sd_polling,1,&SDreadBuffer[i]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
	}
	
	//(7) CRC�f�[�^2byte�ǂݍ���
	for(i=0; i<2; i++)
	{
		R_CSI21_Send_Receive(&sd_polling,1,&sd_check[i]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
	}
	
	//(8) 1byte Dummy�M��
	SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	//SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
	
	//SD_CS = SD_CS_DISABLE; //CS�M�����l�Q�[�g
	return SD_READ_END;	
}


// *******************************************************************************/
//   -Name(en)   :   SD_single_write                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�V���O���������ݏ���>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int SD_single_write(unsigned long dataAddress)
{		
	unsigned char a1,a2,a3,a4,dummy;
	unsigned int cmd24_res,i,j;
	unsigned char 	SDwriteBuffer[1000];
	
	
	//(1)�A�h���X��4�o�C�g�̈����ɕϊ�
	a1 = (unsigned char)((dataAddress&0xFF000000)>>24);
	a2 = (unsigned char)((dataAddress&0x00FF0000)>>16);
	a3 = (unsigned char)((dataAddress&0x0000FF00)>>8);
	a4 = (unsigned char)(dataAddress&0x000000FF);
	
	//CMD24�g�[�N�� �X�^�[�g�o�C�g0xFE�Z�b�g
	SDwriteBuffer[0] = 0xFE;
	//��M�f�[�^��������
	for(i=0; i<SDsectorSize; i++)
	{
		SDwriteBuffer[i+1] = SDreadBuffer[i];
	}
	//�_�~�[CRC 2byte	
	SDwriteBuffer[SDsectorSize+2] = 0x65;
	SDwriteBuffer[SDsectorSize+3] = 0x43;	
	
	SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
	
	//(2) 1byte Dummy�M��
	//SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	//SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
	
	
	for(i = 0; i < 20; i++)
	{
		//(3) CMD24���s
		sd_trans[0] = SD_CMD24;
		sd_trans[1] = a1;
		sd_trans[2] = a2;
		sd_trans[3] = a3;
		sd_trans[4] = a4;
		sd_trans[5] = calcSDCRC(&sd_trans[0]);
		
		R_CSI21_Send_Receive(&sd_trans[0],6,&sd_receive[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
		
		//(4) 0x00(Ready)�����҂�
		for (j = 0; j < 2000; j++) {
			R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
			while(spi_progress_flag != 0x11){}
			spi_progress_flag = 0;
			if (sd_check[0] == SD_CMD_OK ) break;
		}			
		
		//�ُ퉞���Ȃ�(2)�����蒼��
		if (sd_check[0] == SD_CMD_OK)  break;		
	}	
		
	//(5) 2byte Dummy�M��
	//SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	//SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
		
	//(6) �f�[�^�p�P�b�g���M
	for(i=0; i<SDsectorSize+3; i++)
	{
		R_CSI21_Send_Receive(&SDwriteBuffer[i],1,&SDreadBuffer[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
	}

	//(7) 0x00(Busy)�I���҂�
	for (j = 0; j < 2000; j++) {
		R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
		while(spi_progress_flag != 0x11){}
		spi_progress_flag = 0;
		if (sd_check[0] != SD_CMD_BUSY ) break;
	}			
			
	//(8) 1byte Dummy�M��
	SD_CS = SD_CS_DISABLE;	//CS�M����High�ɕێ����āAsck��8�N���b�N�ȏ㑗��
	R_CSI21_Send_Receive(&sd_polling,1,&sd_check[0]);
	while(spi_progress_flag != 0x11){}
	spi_progress_flag = 0;	
	//SD_CS = SD_CS_ENABLE;	//CS�M�����A�T�[�g
	
	//SD_CS = SD_CS_DISABLE; //CS�M�����l�Q�[�g
	return SD_WRITE_END;	
	
}


//�f�[�^�̒f�Љ����N�������ꍇ�́A�}���`�ǂݍ��݂ł��Ȃ����Ƃɒ���
// *******************************************************************************/
//   -Name(en)   :   SD_multi_read                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�}���`�ǂݍ��ݏ���>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
void SD_multi_read(unsigned char dataSize, unsigned long dataAddress)
{		
	unsigned char a1,a2,a3,a4,dummy;
	unsigned int cmd18_res,i;
	//(1)�A�h���X���S�o�C�g�̈����ɕϊ�
	a1 = (unsigned char)((dataAddress&0xFF000000)>>24);
	a2 = (unsigned char)((dataAddress&0x00FF0000)>>16);
	a3 = (unsigned char)((dataAddress&0x0000FF00)>>8);
	a4 = (unsigned char)(dataAddress&0x000000FF);
	
}


// *******************************************************************************/
//   -Name(en)   :                                        �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�pCRC-7�v�Z�e�[�u��>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
static const unsigned char SDCRC_TABLE[] = {
    0x00, 0x12, 0x24, 0x36, 0x48, 0x5a, 0x6c, 0x7e, 0x90, 0x82, 0xb4, 0xa6, 0xd8, 0xca, 0xfc, 0xee,
    0x32, 0x20, 0x16, 0x04, 0x7a, 0x68, 0x5e, 0x4c, 0xa2, 0xb0, 0x86, 0x94, 0xea, 0xf8, 0xce, 0xdc,
    0x64, 0x76, 0x40, 0x52, 0x2c, 0x3e, 0x08, 0x1a, 0xf4, 0xe6, 0xd0, 0xc2, 0xbc, 0xae, 0x98, 0x8a,
    0x56, 0x44, 0x72, 0x60, 0x1e, 0x0c, 0x3a, 0x28, 0xc6, 0xd4, 0xe2, 0xf0, 0x8e, 0x9c, 0xaa, 0xb8,
    0xc8, 0xda, 0xec, 0xfe, 0x80, 0x92, 0xa4, 0xb6, 0x58, 0x4a, 0x7c, 0x6e, 0x10, 0x02, 0x34, 0x26,
    0xfa, 0xe8, 0xde, 0xcc, 0xb2, 0xa0, 0x96, 0x84, 0x6a, 0x78, 0x4e, 0x5c, 0x22, 0x30, 0x06, 0x14,
    0xac, 0xbe, 0x88, 0x9a, 0xe4, 0xf6, 0xc0, 0xd2, 0x3c, 0x2e, 0x18, 0x0a, 0x74, 0x66, 0x50, 0x42,
    0x9e, 0x8c, 0xba, 0xa8, 0xd6, 0xc4, 0xf2, 0xe0, 0x0e, 0x1c, 0x2a, 0x38, 0x46, 0x54, 0x62, 0x70,
    0x82, 0x90, 0xa6, 0xb4, 0xca, 0xd8, 0xee, 0xfc, 0x12, 0x00, 0x36, 0x24, 0x5a, 0x48, 0x7e, 0x6c,
    0xb0, 0xa2, 0x94, 0x86, 0xf8, 0xea, 0xdc, 0xce, 0x20, 0x32, 0x04, 0x16, 0x68, 0x7a, 0x4c, 0x5e,
    0xe6, 0xf4, 0xc2, 0xd0, 0xae, 0xbc, 0x8a, 0x98, 0x76, 0x64, 0x52, 0x40, 0x3e, 0x2c, 0x1a, 0x08,
    0xd4, 0xc6, 0xf0, 0xe2, 0x9c, 0x8e, 0xb8, 0xaa, 0x44, 0x56, 0x60, 0x72, 0x0c, 0x1e, 0x28, 0x3a,
    0x4a, 0x58, 0x6e, 0x7c, 0x02, 0x10, 0x26, 0x34, 0xda, 0xc8, 0xfe, 0xec, 0x92, 0x80, 0xb6, 0xa4,
    0x78, 0x6a, 0x5c, 0x4e, 0x30, 0x22, 0x14, 0x06, 0xe8, 0xfa, 0xcc, 0xde, 0xa0, 0xb2, 0x84, 0x96,
    0x2e, 0x3c, 0x0a, 0x18, 0x66, 0x74, 0x42, 0x50, 0xbe, 0xac, 0x9a, 0x88, 0xf6, 0xe4, 0xd2, 0xc0,
    0x1c, 0x0e, 0x38, 0x2a, 0x54, 0x46, 0x70, 0x62, 0x8c, 0x9e, 0xa8, 0xba, 0xc4, 0xd6, 0xe0, 0xf2,
};

// *******************************************************************************/
//   -Name(en)   :   calcSDCRC                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :   SD�̃R�}���h�ɕt������CRC��p�̊֐��B�X�g�b�v�r�b�g���t������                    */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
unsigned char calcSDCRC( const void* org)
{
    int i;
    unsigned char *data = (unsigned char*)org;
    unsigned char tmp = data[0];

    for( i=1; i<SD_CMD_LENGTH; i++)
    {
        // X'[i] =  X[i] ^ T( X'[i-1])�ɑ���
        tmp = data[i] ^ SDCRC_TABLE[tmp];
    }
    // SDCRC = [ CRC7 StopBit] = 0000 0001 xor X'[4]
    return( SD_STOPBIT ^ SDCRC_TABLE[tmp]);
}


