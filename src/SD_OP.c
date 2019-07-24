/***********************************************************************/
/*                                                                     */
/*  FILE        :SD_OP.c                                        */
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
//   -Name(en)   :   search_empty_SD_FAT                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�󂫃N���X�^����>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
void search_empty_SD_FAT(unsigned long all_cluster_num)
{

	
	
}


// *******************************************************************************/
//   -Name(en)   :   write_SD_FAT                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�hFAT��������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
void write_SD_FAT(unsigned char file_type)
{
	unsigned long	all_cluster_num;
	//unsigned char	FAT_sector_num[0xFF];
	//unsigned char	FAT_offset_num[0xFF];

	//(1) �K�v�N���X�^���̌v�Z
	//�ȉ��̏�����analysis_SD_DIR��search_dir_SD�ɂ�����all_DATA_bytes�����߂Ă��邱�Ƃ�O�񂷂�B
	all_cluster_num = (all_DATA_sectors + BPB_SecPerClus - 1) % BPB_SecPerClus;
	
}


// *******************************************************************************/
//   -Name(en)   :   move_file_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�t�@�C���ړ�>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int move_file_SD(unsigned char *file_name, unsigned char *org_dir_name, unsigned char *target_dir_name, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	
	//(1) �t�@�C���̃��[�g�f�B���N�g�����擾
	//target��*CH_NAME�ɂ��Ă���΃t�@�C�������X�V�����
	ret_search_file = SEARCH_SD_NO_DIR;
	if((file_type & 0xF0) == ROOT_FILE)
	{
		ret_search_file = search_file_SD(&file_name[0], file_type, target);
	}
	else
	{
		while(ret_search_file == SEARCH_SD_NO_DIR)
		{
			ret_search_file = search_dir_SD(&file_name[0], &org_dir_name[0], file_type, target);
			//(2) �t�H���_�쐬�i�t�H���_���Ȃ��ꍇ�̂ݎ��s�j
			if(ret_search_file == SEARCH_SD_NO_DIR)
			{
				
				break;
			}
		}
	}
	
	//�t�@�C�����Ȃ���ΏI��
	if(SEARCH_SD_NO_FILE == ret_search_file)
	{
		return 	ret_search_file;
	}
	
	//(3) �󂫃��[�g�f�B���N�g���̈�㏑��	
	ret_search_file = SEARCH_SD_NO_DIR;
	while(ret_search_file == SEARCH_SD_NO_DIR)
	{
		ret_search_file = copy_empty_SD_SUB_DIR(&target_dir_name[0], 32);
		
		//(4) �t�H���_�쐬�i�t�H���_���Ȃ��ꍇ�̂ݎ��s�j
		if(ret_search_file == SEARCH_SD_NO_DIR)
		{
			
			break;			
		}
	}
	
}


// *******************************************************************************/
//   -Name(en)   :   get_SSCAN_date                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SSCAN.BIN���t�f�[�^���o>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int get_SSCAN_date(void)
{
	int ret = 0;
	int ret_search_file;
	unsigned int i,now_sector;
	unsigned int cluster_max;
	unsigned long m;
	unsigned long k;
	unsigned long	DATA_N_start_address;
	unsigned long target;
	unsigned char sscan_file_name[11];
						
	sscan_file_name[0] 	= 'S';
	sscan_file_name[1] 	= 'S';
	sscan_file_name[2] 	= 'C';
	sscan_file_name[3] 	= 'A';
	sscan_file_name[4] 	= 'N';
	sscan_file_name[5] 	= 0x20;
	sscan_file_name[6] 	= 0x20;
	sscan_file_name[7] 	= 0x20;
	sscan_file_name[8] 	= 'B';
	sscan_file_name[9] 	= 'I';
	sscan_file_name[10] 	= 'N';
			
	//SD�J�[�h�t�@�C��������
	ret_search_file = search_file_SD(&sscan_file_name[0], (FILE_NAME | ROOT_FILE), DATA_READ);
	
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		return ret_search_file;
	}
	
	
	ret_search_file = search_next_cluster_SD(NORMAL);
	transfered_cluster_count = 0;	
	bin_trans_state = 0;
	
	while(1U)
	{	
		//�f�[�^ read
		DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
		
		//�ŏI�N���X�^�ł́A�S�Z�N�^�[��ǂݍ��ޕK�v�͂Ȃ��ꍇ������B�ŏI�N���X�^�ő��M���ׂ��Z�N�^�����v�Z����B
		//�f�[�^�����������ꍇ�i�����݂̃N���X�^���ŏI�N���X�^�j
		if(ret_search_file == SEARCH_SD_SUCCESS)
		{
			m = all_DATA_sectors % BPB_SecPerClus;
			if(m == 0)
			{
				cluster_max = BPB_SecPerClus;					
			}
			else
			{
				cluster_max = (unsigned int)m;	
			}
		}
		//�f�[�^����������ꍇ�́A1�N���X�^������̃Z�N�^���𑗐M����΂悢�B
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			cluster_max = BPB_SecPerClus;	
		}
			
		for(now_sector=0; now_sector<cluster_max; now_sector++)
		{
			
			//�f�[�^�̈�ǂݍ���
			ret = read_SD_DATA_area(now_sector);
					
			//Read Error check
			if(ret == SD_READ_ERROR)
			{
				return ret;	
			}
				
			//SSCAN.BIN���t�f�[�^���o
			get_information(now_sector);	
			if(bin_trans_state == 0xEE)
			{
				break;
			}
		}

		//�]���N���X�^�[���J�E���g
		transfered_cluster_count += 1;
			
		//�f�[�^�����������ꍇ
		if(ret_search_file == SEARCH_SD_SUCCESS || bin_trans_state == 0xEE)
		{	
			transfered_cluster_count = 0;
			bin_trans_state = 0x00;
			break;			
		}
		//�f�[�^����������ꍇ
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			DATA_cluster_num = next_DATA_cluster_num;
			ret_search_file = search_next_cluster_SD(NORMAL);
		}
		//�G���[�̏ꍇ
		else
		{
			return TRANS_AGAIN;
		}
	}
	
	return TRANS_SUCCESS;
}


// *******************************************************************************/
//   -Name(en)   :   check_SSCANBIN                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SSCAN.BIN���t�f�[�^�L���m�F>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
/*
int check_SSCANBIN(void)
{
	int ret;
	int ret_search_file;
	unsigned int i,now_sector;
	unsigned int cluster_max;
	unsigned long m;
	unsigned long k;
	unsigned long	DATA_N_start_address;
	unsigned long target;
			
	ret = MORE_DATA;
	
	//SD�J�[�h�t�@�C��������
	ret_search_file = search_file_SD(&file_name[0], (FILE_NAME | ROOT_FILE), DATA_READ);
	
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		return ret_search_file;
	}
	
	
	ret_search_file = search_next_cluster_SD(NORMAL);
	transfered_cluster_count = 0;	
	bin_trans_state = 0;
	
	while(1U)
	{	
		//�f�[�^ read
		DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
		
		//�ŏI�N���X�^�ł́A�S�Z�N�^�[��ǂݍ��ޕK�v�͂Ȃ��ꍇ������B�ŏI�N���X�^�ő��M���ׂ��Z�N�^�����v�Z����B
		//�f�[�^�����������ꍇ�i�����݂̃N���X�^���ŏI�N���X�^�j
		if(ret_search_file == SEARCH_SD_SUCCESS)
		{
			m = all_DATA_sectors % BPB_SecPerClus;
			if(m == 0)
			{
				cluster_max = BPB_SecPerClus;					
			}
			else
			{
				cluster_max = (unsigned int)m;	
			}
		}
		//�f�[�^����������ꍇ�́A1�N���X�^������̃Z�N�^���𑗐M����΂悢�B
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			cluster_max = BPB_SecPerClus;	
		}
			
		for(now_sector=0; now_sector<cluster_max; now_sector++)
		{
			
			//�f�[�^�̈�ǂݍ���
			ret = read_SD_DATA_area(now_sector);
					
			//Read Error check
			if(ret == SD_READ_ERROR)
			{
				return ret;	
			}
				
			//SSCAN.BIN���t�f�[�^���o
			get_information(now_sector);	
			if(bin_trans_state == 0xEE)
			{
				break;
			}
		}

		//�]���N���X�^�[���J�E���g
		transfered_cluster_count += 1;
			
		//�f�[�^�����������ꍇ
		if(ret_search_file == SEARCH_SD_SUCCESS || bin_trans_state == 0xEE)
		{	
			transfered_cluster_count = 0;
			bin_trans_state = 0x00;
			break;			
		}
		//�f�[�^����������ꍇ
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			DATA_cluster_num = next_DATA_cluster_num;
			ret_search_file = search_next_cluster_SD(NORMAL);
		}
		//�G���[�̏ꍇ
		else
		{
			return TRANS_AGAIN;
		}
	}
	
	return TRANS_SUCCESS;
}
*/

/***********************************************************************************************************************
* Function Name: get_information
* Description  : HttpBody�t�@�C�����𑗐M����
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void get_information(unsigned int now_sector) 
{
	unsigned int now_index, j;
	unsigned long now_sector_address;
	unsigned long now_byte_address;
	unsigned char hex_num;
	unsigned int a, b, c;
	unsigned int temp_index;
	unsigned char token_message[BLOCK_LEN*8];

	sent_size = 0;
	now_sector_address = (unsigned long)transfered_cluster_count * (unsigned long)BPB_SecPerClus * (unsigned long)BPB_BytsPerSec + (unsigned long)now_sector * (unsigned long)BPB_BytsPerSec;
	now_byte_address = now_sector_address;
	
	for(now_index=0; now_index<BPB_BytsPerSec; now_index++)
	{
		//
		if(target_measurement_address == now_byte_address || bin_trans_state == MEASUREMENT_AREA)
		{		
			if(bin_trans_state == 0x00)
			{
				bin_trans_state = MEASUREMENT_AREA;
			}
			
				
			if(0 < (6 - sent_size))
			{				
				if(sent_size < 5)
				{
					temp_buf[sent_size] = SDreadBuffer[now_index];
					
				}
				else if(5 == sent_size)
				{					
					temp_buf[sent_size] = SDreadBuffer[now_index];
					
					
					//�v���J�n�����擾
					a = ((unsigned int)temp_buf[0] << 8) | (unsigned int)temp_buf[1];
					b = ((0xFFC0 & a)>>6) | ((0x003F & a)<<10);					
					//�N
					c = (0xFF00 & b) >> 8;
					compare_start_date[0] = (unsigned char)c;
					//��
					c = 0x00FF & b;
					compare_start_date[1] = (unsigned char)c;
					
					
					a = ((unsigned int)temp_buf[2] << 8) | (unsigned int)temp_buf[3];
					b = ((0xFFE0 & a)>>5) | ((0x001F & a)<<11);				
					//��
					c = (0xFF00 & b) >> 8;
					compare_start_date[2] = (unsigned char)c;				
					//��
					c = 0x00FF & b;
					compare_start_date[3] = (unsigned char)c;
					
					
					a = ((unsigned int)temp_buf[4] << 8) | (unsigned int)temp_buf[5];
					b = ((0xFFF0 & a)>>4) | ((0x000F & a)<<12);				
					//��
					c = (0xFF00 & b) >> 8;
					compare_start_date[4] = (unsigned char)c;				
					//�b
					c = 0x00FF & b;
					compare_start_date[5] = (unsigned char)c;	
				}
				
				sent_size += 1;
			}
			
			if(sent_size == 6)
			{
				bin_trans_state = 0xEE;
				sent_size = 0;
				break;
			}
		}
		
		
		now_byte_address += 1;
	}
}


/***********************************************************************************************************************
* Function Name: date_compare
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
int date_compare(void) 
{
	int ret;
	
	//compare_start_date�f�[�^��0x00�̏ꍇ
	if(0x00 == compare_start_date[0] || 0x00 == compare_start_date[1] || 0x00 == compare_start_date[2])
	{	
		return COMPARE_END;		//SSCAN.BIN�̓��t�f�[�^�͂������݂��Ȃ��̂Ō����I��	
	}
	
	//�N�̔�r
	if(R_file_date[0] > compare_start_date[0])
	{
		return COMPARE_NEXT;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^��薢���Ȃ̂Ŏ��̗̈������
	}
	else if(R_file_date[0] < compare_start_date[0])
	{
		return COMPARE_END;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^���ߋ��Ȃ̂Ō����I��		
	}
	
	//���̔�r
	if(R_file_date[1] > compare_start_date[1])
	{
		return COMPARE_NEXT;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^��薢���Ȃ̂Ŏ��̗̈������
	}
	else if(R_file_date[1] < compare_start_date[1])
	{
		return COMPARE_END;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^���ߋ��Ȃ̂Ō����I��		
	}
	
	//���̔�r
	if(R_file_date[2] > compare_start_date[2])
	{
		return COMPARE_NEXT;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^��薢���Ȃ̂Ŏ��̗̈������
	}
	else if(R_file_date[2] < compare_start_date[2])
	{
		return COMPARE_END;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^���ߋ��Ȃ̂Ō����I��		
	}
	
	//���̔�r
	if(R_file_date[3] > compare_start_date[3])
	{
		return COMPARE_NEXT;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^��薢���Ȃ̂Ŏ��̗̈������
	}
	else if(R_file_date[3] < compare_start_date[3])
	{
		return COMPARE_END;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^���ߋ��Ȃ̂Ō����I��		
	}
	
	//���̔�r
	if(R_file_date[4] > compare_start_date[4])
	{
		return COMPARE_NEXT;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^��薢���Ȃ̂Ŏ��̗̈������
	}
	else if(R_file_date[4] < compare_start_date[4])
	{
		return COMPARE_END;		//R�t�@�C����SSCAN.BIN�̓��t�f�[�^���ߋ��Ȃ̂Ō����I��		
	}	
}



/***********************************************************************************************************************
* Function Name: date_latest
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
int date_latest(void) 
{
	int ret;
	
		
	//�N�̔�r
	if(old_compare_start_date[0] > compare_start_date[0])
	{
		return COMPARE_LATEST_NOW;		//�ŐV�̓��t�f�[�^�ύX�̕K�v�Ȃ�
	}
	else if(old_compare_start_date[0] < compare_start_date[0])
	{
		return COMPARE_LATEST_NEXT;		//�ŐV�̓��t�f�[�^���X�V		
	}
	
	//���̔�r
	if(old_compare_start_date[1] > compare_start_date[1])
	{
		return COMPARE_LATEST_NOW;		//�ŐV�̓��t�f�[�^�ύX�̕K�v�Ȃ�
	}
	else if(old_compare_start_date[1] < compare_start_date[1])
	{
		return COMPARE_LATEST_NEXT;		//�ŐV�̓��t�f�[�^���X�V		
	}
	
	//���̔�r
	if(old_compare_start_date[2] > compare_start_date[2])
	{
		return COMPARE_LATEST_NOW;		//�ŐV�̓��t�f�[�^�ύX�̕K�v�Ȃ�
	}
	else if(old_compare_start_date[2] < compare_start_date[2])
	{
		return COMPARE_LATEST_NEXT;		//�ŐV�̓��t�f�[�^���X�V		
	}
	
	//���̔�r
	if(old_compare_start_date[3] > compare_start_date[3])
	{
		return COMPARE_LATEST_NOW;		//�ŐV�̓��t�f�[�^�ύX�̕K�v�Ȃ�
	}
	else if(old_compare_start_date[3] < compare_start_date[3])
	{
		return COMPARE_LATEST_NEXT;		//�ŐV�̓��t�f�[�^���X�V		
	}
	
	//���̔�r
	if(old_compare_start_date[4] > compare_start_date[4])
	{
		return COMPARE_LATEST_NOW;		//�ŐV�̓��t�f�[�^�ύX�̕K�v�Ȃ�
	}
	else if(old_compare_start_date[4] < compare_start_date[4])
	{
		return COMPARE_LATEST_NEXT;		//�ŐV�̓��t�f�[�^���X�V		
	}	
}


/***********************************************************************************************************************
* Function Name: date_name
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
int date_name(void) 
{
	int ret;
	unsigned char string_compare_start_date[10];
	
	//
	
		
	//������ɕϊ�
	//�N
	string_compare_start_date[0] = trans_hex((unsigned char)compare_start_date[0]/10);
	string_compare_start_date[1] = trans_hex((unsigned char)compare_start_date[0]%10);
	
	//��
	string_compare_start_date[2] = trans_hex((unsigned char)compare_start_date[1]/10);
	string_compare_start_date[3] = trans_hex((unsigned char)compare_start_date[1]%10);
	
	//��
	string_compare_start_date[4] = trans_hex((unsigned char)compare_start_date[2]/10);
	string_compare_start_date[5] = trans_hex((unsigned char)compare_start_date[2]%10);
	
	//��
	string_compare_start_date[6] = trans_hex((unsigned char)compare_start_date[3]/10);
	string_compare_start_date[7] = trans_hex((unsigned char)compare_start_date[3]%10);
	
	//��
	string_compare_start_date[8] = trans_hex((unsigned char)compare_start_date[4]/10);
	string_compare_start_date[9] = trans_hex((unsigned char)compare_start_date[4]%10);
	
	
	
	//���̔�r
	if(R_file_name[0] != string_compare_start_date[2])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	if(R_file_name[1] != string_compare_start_date[3])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	//���̔�r
	if(R_file_name[2] != string_compare_start_date[4])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	if(R_file_name[3] != string_compare_start_date[5])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	//���̔�r
	if(R_file_name[4] != string_compare_start_date[6])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	if(R_file_name[5] != string_compare_start_date[7])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	//���̔�r
	if(R_file_name[6] != string_compare_start_date[8])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	
	if(R_file_name[7] != string_compare_start_date[9])
	{
		return COMPARE_NEXT;		//��v���Ȃ���Ύ��̗̈������			
	}
	else
	{
		return COMPARE_END;		//�S�Ĉ�v����Ό����I��	
	}
}

