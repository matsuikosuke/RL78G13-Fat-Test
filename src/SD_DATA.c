/***********************************************************************/
/*                                                                     */
/*  FILE        :SD_DIR.c                                        */
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
//   -Name(en)   :   read_SD_DATA_area                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
int read_SD_DATA_area(unsigned int now_sector)
{		
	int ret;
	unsigned int i;
	//unsigned long DATA_N_start_sector;	
	unsigned long DATA_N_start_address;			
			
	
	DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
			
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			ret = SD_single_read(DATA_N_start_sector+now_sector);	
		}
		else
		{
			DATA_N_start_address = (DATA_N_start_sector+now_sector) * (unsigned long)BPB_BytsPerSec;
			ret = SD_single_read(DATA_N_start_address);	
		}
		
		if(ret == SD_READ_END) break;
	}	
	
	return ret;
}
	

// *******************************************************************************/
//   -Name(en)   :   write_SD_DATA_area                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
void write_SD_DATA_area(unsigned int now_sector)
{		
	int ret;
	unsigned int i;
	unsigned long	DATA_N_start_address;
	
	DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
	
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			ret = SD_single_write(DATA_N_start_sector+now_sector);	
		}
		else
		{
			DATA_N_start_address = (DATA_N_start_sector+now_sector) * (unsigned long)BPB_BytsPerSec;
			ret = SD_single_write(DATA_N_start_address);	
		}
		if(ret == SD_WRITE_END) break;
	}	
}


// *******************************************************************************/
//   -Name(en)   :   target_DATA_MOVE_SubDir                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
//�y�֐��zsearch_file_name�F�y�ϐ��zhit_num
//write_SD_DATA_area�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
void target_DATA_MOVE_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//�T�u�f�B���N�g��32byte�R�s�[���폜
	for(i=0; i<32; i++)
	{
		//SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1-start];	
		SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1];				
	}
	SDreadBuffer[hit_num-1-start] = 0xE5;	//�t�@�C�����1�����ڂ�0xE5�Ȃ�΍폜�t�@�C���ɂȂ�B
	
	//���[�g�f�B���N�g���㏑��
	write_SD_DATA_area(now_sector);
}

// *******************************************************************************/
//   -Name(en)   :   target_DATA_CH_NAME_SubDir                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
//�y�֐��zsearch_file_name�F�y�ϐ��zhit_num
//�y�֐��z�Ȃ��F�y�ϐ��znew_name[]
//write_SD_DATA_area�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
void target_DATA_CH_NAME_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//�ۑ聚�Fstart�͕s�v��������Ȃ��B
	if((file_type & 0x0F) == FILE_NAME)	//�t�@�C��������
	{
		start = 0;	//�t�@�C�������������ꍇhit_num-1���t�@�C�����擪�ɂȂ�B	
	}
	else if((file_type & 0x0F) == EXP_NAME)	//�g���q����
	{
		
		start = 8;	//�g���q���������ꍇhit_num-1���8�����O���t�@�C�����擪�ɂȂ�B
	}		
	
	/*//�ύX�t�@�C�����X�V
	for(i=0; i<5; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//�T�u�f�B���N�g�����̕ύX
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1-start] = new_name[i];						
	}
	
	//�T�u�f�B���N�g���㏑��
	write_SD_DATA_area(now_sector);
}


// *******************************************************************************/
//   -Name(en)   :   target_DELETE_FILE_SubDir                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
//�y�֐��zsearch_file_name�F�y�ϐ��zhit_num
//�y�֐��z�Ȃ��F�y�ϐ��znew_name[]
//write_SD_DATA_area�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
void target_DELETE_FILE_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	int ret_search_file;
	unsigned int i;
	
	//�N���X�^�ԍ��������� 1�� & ���N���X�^�ԍ��擾
	ret_search_file = search_next_cluster_SD(DELETE);
	
	//�ŏI�N���X�^�ԍ��ɓ��B����܂ŌJ��Ԃ�����
	while(SEARCH_SD_NEXT == ret_search_file)
	{
		DATA_cluster_num = next_DATA_cluster_num;
		ret_search_file = search_next_cluster_SD(DELETE);
	}	
}

// *******************************************************************************/
//   -Name(en)   :   target_DATA_MOVE_CH_NAME_SubDir                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
//�y�֐��zsearch_file_name�F�y�ϐ��zhit_num
//�y�֐��z�Ȃ��F�y�ϐ��znew_name[]
//target_DATA_MOVE_SubDir�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//�y�֐��zanalysis_file_info_SD�F�y�ϐ��zDATA_cluster_num
void target_DATA_MOVE_CH_NAME_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//�ۑ聚�Fstart�͕s�v��������Ȃ��B
	if((file_type & 0x0F) == FILE_NAME)	//�t�@�C��������
	{
		start = 0;	//�t�@�C�������������ꍇhit_num-1���t�@�C�����擪�ɂȂ�B	
	}
	else if((file_type & 0x0F) == EXP_NAME)	//�g���q����
	{
		
		start = 8;	//�g���q���������ꍇhit_num-1���8�����O���t�@�C�����擪�ɂȂ�B
	}		
	
	//�ύX�t�@�C�����X�V
	/*//�t�@�C�����t�f�[�^�𖼏̂ɂ���B
	for(i=0; i<5; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';*/
	/*//���O�Ɏ擾�����t�@�C�����Ɠ���ɂ���B
	for(i=0; i<8; i++)
	{
		new_name[i] = file_name[i];				
	}
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//���[�g�f�B���N�g�����̕ύX
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1-start] = new_name[i];						
	}
	
	//���[�g�f�B���N�g��32byte�R�s�[���폜�����[�g�f�B���N�g���㏑��
	target_DATA_MOVE_SubDir(file_type, now_sector);
}




// *******************************************************************************/
//   -Name(en)   :   search_dir_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�t�H���_���t�@�C��������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, ROOT_sector_num, FAT_type
//�y�֐��zread_SD_DATA_area to SD_single_read�F�y�ϐ��zSDreadBuffer[]
int search_dir_SD(unsigned char *file_name_now, unsigned char *org_dir_name_now, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	unsigned int now_sector;	
	int hit;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR��́FBPB�̐擪�Z�N�^���o
	analysis_SD_MBR();
	
	//BPB��́FFAT�A���[�g�f�B���N�g���A�f�[�^�̈�̐擪�Z�N�^�AFAT�^�C�v�̓��o
	analysis_SD_BPB();
	
	//���[�g�f�B���N�g����́F�����t�H���_�̃f�[�^�N���X�^�ԍ����o�B���̏�����DATA_cluster_num���X�V�����B
	//�t�H���_���̌����Ȃ̂Ŋg���q�ł͂Ȃ��t�@�C���������ɂ��Ȃ��Ɗ֐�analysis_SD_DIR���̕ϐ�start�̒l���s���ɂȂ�B
	ret_search_file = analysis_SD_DIR(&org_dir_name_now[0], (FILE_NAME | SUB_FILE), DATA_READ);
	
	//�t�@�C��������v�Ȃ��̏ꍇ
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		ret_search_file = SEARCH_SD_NO_DIR;
	}
	//�t�@�C��������v�L��̏ꍇ
	else
	{
		//SD�J�[�h�f�[�^�������擾
		ret_search_file = search_next_cluster_SD(NORMAL);
	
		while(1U)
		{	
			//(1) �f�[�^�̈�ǂݍ���	
			DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
			
			for(now_sector=0; now_sector<BPB_SecPerClus; now_sector++)
			{
				read_SD_DATA_area(now_sector);
				
				//(2) �t�@�C��������			
				hit = search_file_name(&file_name_now[0], file_type);
				
				//(3)-a �S������v�����ꍇ
				if(hit == 0x01)
				{
					//�t�@�C�������擾�������ꍇ
					if(target == DATA_READ)
					{
						//�S������v������now_sector��for loop�𔲂��Č����I���B�Y���t�@�C���̃N���X�^�ԍ����L�^����B
						analysis_file_info_SD();
						break;	
					}
					//�t�@�C����ʂ̏ꏊ�ֈړ��������ꍇ
					else if(target == DATA_MOVE)
					{
						//analysis_file_info_SD();					
						target_DATA_MOVE_SubDir(file_type, now_sector);
						break;			
					}	
					//�t�@�C������ύX�������ꍇ
					else if(target == DATA_CH_NAME)
					{
						//get_file_date_SD();
						analysis_file_info_SD();
						target_DATA_CH_NAME_SubDir(file_type, now_sector);
						break;					
					}	
					//�t�@�C������ύX���āA�ʂ̏ꏊ�ֈړ��������ꍇ
					else if(target == DATA_MOVE_CH_NAME)
					{
						//get_file_date_SD();
						analysis_file_info_SD();	
						target_DATA_MOVE_CH_NAME_SubDir(file_type, now_sector);
						break;					
					}	
					//�t�@�C�����폜�������ꍇ
					else if(target == DELETE_FILE)
					{
						//�T�u�f�B���N�g�����̐擪1�����ύX
						SDreadBuffer[hit_num+0-1-0] = 0xE5;		
						
						//�T�u�f�B���N�g�����̊g���q�ύX
						SDreadBuffer[hit_num+8-1-0] = 'T';
						SDreadBuffer[hit_num+9-1-0] = 'X';
						SDreadBuffer[hit_num+10-1-0] = 'T';
						
						//�T�u�f�B���N�g���㏑��
						write_SD_DATA_area(now_sector);	
						
						analysis_file_info_SD();
						target_DELETE_FILE_SubDir(file_type, now_sector);
						break;					
					}					
				}	
			}
			
			
			//�S������v������while loop�𔲂��Č����I��
			if(hit == 0x01)
			{
				break;					
			}	
			
			//(3)-b �S������v���Ă��Ȃ��ꍇ
			//�f�[�^�����������ꍇ
			if(ret_search_file == SEARCH_SD_SUCCESS)
			{
				ret_search_file = SEARCH_SD_NO_FILE;
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
				break;	
			}
		
		}
	}	
	
	return ret_search_file;	
}
	


// *******************************************************************************/
//   -Name(en)   :   copy_empty_SD_SUB_DIR                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�󂫃��[�g�f�B���N�g����������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int copy_empty_SD_SUB_DIR(unsigned char *target_dir_name, unsigned int write_bytes)
{
	int ret_search_file;
	unsigned int i;
	unsigned int now_sector;
	int hit;
	
	ret_search_file = SEARCH_SD_FAIL;
		
	//���[�g�f�B���N�g����́F�����t�H���_�̃f�[�^�N���X�^�ԍ����o�B���̏�����DATA_cluster_num���X�V�����B
	ret_search_file = analysis_SD_DIR(&target_dir_name[0], ( FILE_NAME | SUB_FILE ), DATA_READ);
	
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		
		ret_search_file = SEARCH_SD_NO_DIR;
	}
	else
	{				
		hit = 0x00;
		//SD�J�[�h�f�[�^�������擾
		ret_search_file = search_next_cluster_SD(NORMAL);
		
		while(1U)
		{	
			//(1) �f�[�^ read
			DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
			
			
			for(now_sector=0; now_sector<BPB_SecPerClus; now_sector++)
			{
				read_SD_DATA_area(now_sector);
				
				//(2) �󂫗̈挟��					
				hit = search_empty(write_bytes);				
			
				//�S������v�����ꍇ�B���[�g�f�B���N�g����������
				if(hit == 0x01)
				{				
					//���[�g�f�B���N�g���X�V
					for(i=0; i<write_bytes; i++)
					{
						SDreadBuffer[hit_num+i] = SDrootDirBuffer[i];						
					}
					
					//���[�g�f�B���N�g���㏑��
					write_SD_DATA_area(now_sector);
										
					break;	
				}					
			}
			
			
			//�S������v������while loop�𔲂��Č����I��
			if(hit == 0x01)
			{
				break;					
			}	
			
			//������v���Ȃ������ꍇ�B
			//�f�[�^�����������ꍇ
			if(ret_search_file == SEARCH_SD_SUCCESS)
			{
				ret_search_file = SEARCH_SD_NO_FILE;
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
				break;	
			}		
		}
	}
	
	return ret_search_file;
}


	