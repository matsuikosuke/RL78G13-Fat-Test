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
//   -Name(en)   :   read_SD_DIR                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void read_SD_DIR(unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{		
	int ret;
	unsigned int i;
	unsigned long ROOT_N_address;
	
	
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			if(FAT_type == FAT32)
			{
				ret = SD_single_read(DATA_start_sector+(now_cluster_num-2)*(unsigned long)BPB_SecPerClus+ROOT_sector_num);
			}
			else
			{
				ret = SD_single_read(ROOT_start_sector+ROOT_sector_num);				
			}
		}
		else
		{
			if(FAT_type == FAT32)
			{
				ROOT_N_address = (DATA_start_sector+(now_cluster_num-2)*(unsigned long)BPB_SecPerClus+ROOT_sector_num) * (unsigned long)BPB_BytsPerSec;
				ret = SD_single_read(ROOT_N_address);
			}
			else
			{
				ROOT_N_address = (ROOT_start_sector+ROOT_sector_num) * (unsigned long)BPB_BytsPerSec;
				ret = SD_single_read(ROOT_N_address);				
			}
		}
		if(ret == SD_READ_END) break;
	}	
}
	


// *******************************************************************************/
//   -Name(en)   :   write_SD_DIR_Root                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void write_SD_DIR_Root(unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{		
	int ret;
	unsigned int i;	
	unsigned long ROOT_N_address;
	
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			if(FAT_type == FAT32)
			{
				ret = SD_single_write(DATA_start_sector+(now_cluster_num-2)*(unsigned long)BPB_SecPerClus+ROOT_sector_num);
			}
			else
			{
				ret = SD_single_write(ROOT_start_sector+ROOT_sector_num);				
			}
		}
		else
		{
			if(FAT_type == FAT32)
			{
				ROOT_N_address = (DATA_start_sector+(now_cluster_num-2)*(unsigned long)BPB_SecPerClus+ROOT_sector_num) * (unsigned long)BPB_BytsPerSec;
				ret = SD_single_write(ROOT_N_address);
			}
			else
			{
				ROOT_N_address = (ROOT_start_sector+ROOT_sector_num) * (unsigned long)BPB_BytsPerSec;
				ret = SD_single_write(ROOT_N_address);				
			}
		}
		if(ret == SD_WRITE_END) break;
	}	
}





// *******************************************************************************/
//   -Name(en)   :   target_DATA_MOVE_Root                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
//�y�֐��zsearch_file_name�F�y�ϐ��zhit_num
//write_SD_DIR_Root�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_MOVE_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{
	int ret;
	unsigned int i;
	
	//���[�g�f�B���N�g��32byte�R�s�[���폜
	for(i=0; i<32; i++)
	{
		SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1];
		//SDreadBuffer[hit_num+i-1-start] = 0x00;						
	}
	SDreadBuffer[hit_num-1] = 0xE5;
	
	//���[�g�f�B���N�g���㏑��
	write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);	
	
}


// *******************************************************************************/
//   -Name(en)   :   target_DATA_CH_NAME_Root                                     �@�@�@�@�@�@ �@*/
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
//write_SD_DIR_Root�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_CH_NAME_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{
	int ret;
	unsigned int i;
	
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
	
	//���[�g�f�B���N�g�����̕ύX
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1] = new_name[i];						
	}
		
	//���[�g�f�B���N�g���㏑��
	write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);		
}



// *******************************************************************************/
//   -Name(en)   :   target_DELETE_FILE_Root                                     �@�@�@�@�@�@ �@*/
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
//write_SD_DIR_Root�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DELETE_FILE_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
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
//   -Name(en)   :   target_DATA_MOVE_CH_NAME_Root                                     �@�@�@�@�@�@ �@*/
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
//target_DATA_COPY_Root�ŕK�v�ɂȂ�ϐ�
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_MOVE_CH_NAME_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{			
	unsigned int i;
	
	/*//�ύX�t�@�C�����X�V
	for(i=0; i<6; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//���[�g�f�B���N�g�����̕ύX
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1] = new_name[i];						
	}
					
	//���[�g�f�B���N�g��32byte�R�s�[���폜�����[�g�f�B���N�g���㏑��			
	target_DATA_MOVE_Root(file_type, ROOT_sector_num, now_cluster_num);
}


// *******************************************************************************/
//   -Name(en)   :   analysis_SD_DIR                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h���[�g�f�B���N�g����͏����i�t�@�C���������j>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zDATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, ROOT_sector_num, FAT_type
//�y�֐��zread_SD_DIR to SD_single_read�F�y�ϐ��zSDreadBuffer[]
int analysis_SD_DIR(unsigned char *name, unsigned char file_type, unsigned char target)
{		
	int ret_search_file = 0;
	int hit;
	unsigned long ROOT_sector_num;
	unsigned long now_cluster_num;
	unsigned long search_sector_num;
	
	//read_SD_DIR�֐����g���ꍇ�́A�ȉ��̏��������O�ɕK���s������
	//FAT32��FAT12/16�ł̓��[�g�f�B���N�g���̏�������قȂ�B
	if(FAT_type == FAT32)
	{
		//FTA32�̓��[�g�f�B���N�g���̈悪�A�����Ă���Ƃ͌���Ȃ��̂�1�N���X�^���̃Z�N�^��ǂݍ���ł���A���N���X�^�ԍ�����������B
		search_sector_num = BPB_SecPerClus;		
		//FAT32�̏ꍇ�A���[�g�f�B���N�g���̓f�[�^�̈�ɂ���A���̊J�n�N���X�^�ԍ���BPB_RootClus�Ɋi�[�����B
		now_cluster_num = BPB_RootClus;
	}
	else
	{
		//���[�g�f�B���N�g���̃Z�N�^�[��
		search_sector_num = ROOT_sectors;
		//FAT16�̏ꍇ�A���[�g�f�B���N�g���̈��1�N���X�^�ȉ��Ȃ̂Ŕԍ��͕s�v
		now_cluster_num = 0;
	}
	
	hit = 0x00;
	while(1U)
	{
		for(ROOT_sector_num=0; ROOT_sector_num<search_sector_num ; ROOT_sector_num++)
		{
			//(1) ���[�g�f�B���N�g���ǂݍ���	
			read_SD_DIR(ROOT_sector_num, now_cluster_num);			
			
			//(2) �t�@�C��������	
			hit = search_file_name(&name[0], file_type);	//hit_num�X�V
			
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
					target_DATA_MOVE_Root(file_type, ROOT_sector_num, now_cluster_num);
					break;				
				}	
				//�t�@�C������ύX�������ꍇ
				else if(target == DATA_CH_NAME)
				{	
					//get_file_date_SD();
					analysis_file_info_SD();					
					target_DATA_CH_NAME_Root(file_type, ROOT_sector_num, now_cluster_num);
					break;				
				}
				//�t�@�C������ύX���āA�ʂ̏ꏊ�ֈړ��������ꍇ
				else if(target == DATA_MOVE_CH_NAME)
				{	
					//get_file_date_SD();
					analysis_file_info_SD();					
					target_DATA_MOVE_CH_NAME_Root(file_type, ROOT_sector_num, now_cluster_num);	
					break;	
				}
				//�t�@�C�����폜�������ꍇ
				else if(target == DELETE_FILE)
				{	
					
					//���[�g�f�B���N�g�����̐擪1�����ύX
					SDreadBuffer[hit_num+0-1] = 0xE5;	
					
					//���[�g�f�B���N�g�����̊g���q�ύX
					SDreadBuffer[hit_num+8-1] = 'T';
					SDreadBuffer[hit_num+9-1] = 'X';
					SDreadBuffer[hit_num+10-1] = 'T';
							
					//���[�g�f�B���N�g���㏑��
					write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);	
					
					analysis_file_info_SD();					
					target_DELETE_FILE_Root(file_type, ROOT_sector_num, now_cluster_num);
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
		if(FAT_type == FAT32)
		{
			//FAT��́F�����f�[�^�����̊i�[���ꂽ�f�[�^�N���X�^�ԍ����o
			search_cluster_address_SD(BPB_RootClus, NORMAL);
			//�f�[�^�����������ꍇ	
			if(next_DATA_cluster_num = 0x0FFFFFFF)
			{
				//���[�g�f�B���N�g���̈��S�Č������Ă���̂ŁA��v�Ȃ���while loop�𔲂��Č����I��
				ret_search_file = SEARCH_SD_NO_FILE;
				break;
			}
			//�f�[�^����������ꍇ
			else
			{
				//FAT�̃N���X�^�ԍ���0x0FFFFFFF�ɂȂ�܂�While loop�Ō������p������B
				now_cluster_num = next_DATA_cluster_num;
			}
		}
		else
		{
			//FAT32�ȊO�ł́A���̎��_�Ń��[�g�f�B���N�g���̈��S�Č������Ă���̂ŁA��v�Ȃ���while loop�𔲂��Č����I��
			ret_search_file = SEARCH_SD_NO_FILE;
			break;				
		}				
	}
	
	return	ret_search_file;
}




// *******************************************************************************/
//   -Name(en)   :   search_file_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�t�@�C��������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int search_file_SD(unsigned char *name, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR��́FBPB�̐擪�Z�N�^���o
	analysis_SD_MBR();
	
	//BPB��́FFAT�A���[�g�f�B���N�g���A�f�[�^�̈�̐擪�Z�N�^�AFAT�^�C�v�̓��o
	analysis_SD_BPB();
	
	//���[�g�f�B���N�g����́F�����t�@�C���̃f�[�^�N���X�^�ԍ����o
	ret_search_file = analysis_SD_DIR(&name[0], file_type, target);
	/*if((file_type & 0x0F) == 0x01)
	{
		ret_search_file = analysis_SD_DIR(&name[0], file_type, target);
	}
	else if((file_type & 0x0F) == 0x02)
	{
		ret_search_file = analysis_SD_DIR(&name[8], file_type, target);
	}*/
	
	if(ret_search_file != SEARCH_SD_NO_FILE)
	{
		//SD�J�[�h�f�[�^�������擾
		ret_search_file = search_next_cluster_SD(NORMAL);
	}
	
	return ret_search_file;
}




// *******************************************************************************/
//   -Name(en)   :   delete_file_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�t�@�C���폜>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int delete_file_SD(unsigned char *name, unsigned char file_type)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR��́FBPB�̐擪�Z�N�^���o
	analysis_SD_MBR();
	
	//BPB��́FFAT�A���[�g�f�B���N�g���A�f�[�^�̈�̐擪�Z�N�^�AFAT�^�C�v�̓��o
	analysis_SD_BPB();
	
	//���[�g�f�B���N�g����́F�����t�@�C���̃f�[�^�N���X�^�ԍ����o
	ret_search_file = analysis_SD_DIR(&name[0], file_type, DELETE_FILE);
	
	
	return ret_search_file;
}



// *******************************************************************************/
//   -Name(en)   :   copy_empty_SD_DIR                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�󂫃��[�g�f�B���N�g����������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_DIR�F�y�ϐ��zSDrootDirBuffer�i�R�s�[�������t�@�C���̃t�@�C�����j
int copy_empty_SD_DIR(unsigned int write_bytes)
{
	int ret_search_area = 0;
	int hit;
	unsigned int i,j;
	unsigned long ROOT_sector_num;
	unsigned long now_cluster_num;
	unsigned long search_sector_num;
	
	
	//FAT32��FAT12/16�ł̓��[�g�f�B���N�g���̏�������قȂ�B
	if(FAT_type == FAT32)
	{
		//FTA32�̓��[�g�f�B���N�g���̈悪�A�����Ă���Ƃ͌���Ȃ��̂�1�N���X�^���̃Z�N�^��ǂݍ���ł���A���N���X�^�ԍ�����������B
		search_sector_num = BPB_SecPerClus;		
		//FAT32�̏ꍇ�A���[�g�f�B���N�g���̓f�[�^�̈�ɂ���A���̊J�n�N���X�^�ԍ���BPB_RootClus�Ɋi�[�����B
		now_cluster_num = BPB_RootClus;
	}
	else
	{
		//���[�g�f�B���N�g���̃Z�N�^�[��
		search_sector_num = ROOT_sectors;
		//FAT16�̏ꍇ�A���[�g�f�B���N�g���̈��1�N���X�^�ȉ��Ȃ̂Ŕԍ��͕s�v
		now_cluster_num = 0;				
	}
			
	hit = 0x00;
	while(1U)
	{
		for(ROOT_sector_num=0; ROOT_sector_num<search_sector_num ; ROOT_sector_num++)
		{
			//(1) read ROOT	
			read_SD_DIR(ROOT_sector_num, now_cluster_num);
						
			//(2) �󂫗̈挟��				
			hit = search_empty(write_bytes);	
			
			//(3)-a �S������v�����ꍇ�B���[�g�f�B���N�g����������
			if(hit == 0x01)
			{				
				//���[�g�f�B���N�g���X�V
				for(i=0; i<write_bytes; i++)
				{
					SDreadBuffer[j+i] = SDrootDirBuffer[i];						
				}
				
				write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);
										
				break;	
			}				
		}
		
		//�S������v������while loop�𔲂��Č����I��
		if(hit == 0x01)
		{
			break;					
		}	
		
		//(3)-b �S������v���Ă��Ȃ��ꍇ
		if(FAT_type == FAT32)
		{
			//FAT��́F�����f�[�^�����̊i�[���ꂽ�f�[�^�N���X�^�ԍ����o
			search_cluster_address_SD(BPB_RootClus, NORMAL);	
			if(next_DATA_cluster_num = 0x0FFFFFFF)
			{
				//���[�g�f�B���N�g���̈��S�Č������Ă���̂ŁA��v�Ȃ���while loop�𔲂��Č����I��
				ret_search_area = SEARCH_SD_NO_AREA;
				break;
			}
			else
			{
				//FAT�̃N���X�^�ԍ���0x0FFFFFFF�ɂȂ�܂�While loop�Ō������p������B
				now_cluster_num = next_DATA_cluster_num;
			}
		}
		else
		{
			//FAT32�ȊO�ł́A���̎��_�Ń��[�g�f�B���N�g���̈��S�Č������Ă���̂ŁA��v�Ȃ���while loop�𔲂��Č����I��
			ret_search_area = SEARCH_SD_NO_AREA;
			break;				
		}				
	}
	
	return ret_search_area;
}



// *******************************************************************************/
//   -Name(en)   :   analysis_file_info_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zBPB_BytsPerSec
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
void analysis_file_info_SD(void)
{
	unsigned long x1,x2,x3;
	unsigned int date1, date2, date3;
	unsigned char one, ten;
	
	//�t�@�C���S�N���X�^��
	x1 = SDreadBuffer[hit_num+21-1];
	x2 = SDreadBuffer[hit_num+20-1];
	x3 = SDreadBuffer[hit_num+27-1];	
	DATA_cluster_num = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[hit_num+26-1];
	
	//�t�@�C���S�o�C�g��
	x1 = SDreadBuffer[hit_num+31-1];
	x2 = SDreadBuffer[hit_num+30-1];
	x3 = SDreadBuffer[hit_num+29-1];	
	all_DATA_bytes = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[hit_num+28-1];
	
	//�t�@�C���S�Z�N�^��
	all_DATA_sectors = (all_DATA_bytes + BPB_BytsPerSec - 1) / BPB_BytsPerSec;
	
	//�t�@�C���쐬����
	DIR_CrtTime[0] =  SDreadBuffer[hit_num+14-1];
	DIR_CrtTime[1] =  SDreadBuffer[hit_num+15-1];
	
	//�t�@�C���쐬��
	DIR_CrtDate[0] =  SDreadBuffer[hit_num+16-1];
	DIR_CrtDate[1] =  SDreadBuffer[hit_num+17-1];
		
	//�t�@�C���I�[�v����
	DIR_LstAccDate[0] =  SDreadBuffer[hit_num+18-1];
	DIR_LstAccDate[1] =  SDreadBuffer[hit_num+19-1];	
	
	//�t�@�C���������ݎ���
	DIR_WrtTime[0] =  SDreadBuffer[hit_num+22-1];
	DIR_WrtTime[1] =  SDreadBuffer[hit_num+23-1];
	
	date1 = DIR_WrtTime[1];
	date2 = (date1<<8) | DIR_WrtTime[0];
	
	if('R' == file_name[8])
	{
		//��	
		date3 = ((0xF800 & date2) >> 11);	
		R_file_date[3] = (unsigned char)date3;
		
		//��
		date3 = ((0x07E0 & date2) >> 5);	
		R_file_date[4] = (unsigned char)date3;	
	}
	
	//�t�@�C���������ݓ�
	DIR_WrtDate[0] =  SDreadBuffer[hit_num+24-1];
	DIR_WrtDate[1] =  SDreadBuffer[hit_num+25-1];
	
	date1 = DIR_WrtDate[1];
	date2 = (date1<<8) | DIR_WrtDate[0];
	
	//�N
	date3 = ((0xFE00 & date2) >> 9) + 1980;	
	date3 %= 100;	
	file_date[0] = (unsigned char)(date3/10) + '0';
	file_date[1] = (unsigned char)(date3%10) + '0';
	if('R' == file_name[8])
	{
		R_file_date[0] = (unsigned char)date3;
	}
	
	//��
	date3 = ((0x1E0 & date2) >> 5);	
	file_date[2] = (unsigned char)(date3/10) + '0';
	file_date[3] = (unsigned char)(date3%10) + '0';
	if('R' == file_name[8])
	{
		R_file_date[1] = (unsigned char)date3;
	}
	
	//��
	date3 = (0x1F & date2);	
	file_date[4] = (unsigned char)(date3/10) + '0';
	file_date[5] = (unsigned char)(date3%10) + '0';	
	if('R' == file_name[8])
	{
		R_file_date[2] = (unsigned char)date3;
	}
	//get_file_date_SD();
}


// *******************************************************************************/
//   -Name(en)   :   get_file_date_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zBPB_BytsPerSec
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
/*void get_file_date_SD(void)
{
	unsigned long x1,x2,x3;
	unsigned int date1, date2, date3;
	unsigned char one, ten;
	
	//�t�@�C���������ݓ�
	DIR_WrtDate[0] =  SDreadBuffer[hit_num+24-1];
	DIR_WrtDate[1] =  SDreadBuffer[hit_num+25-1];
	
	date1 = DIR_WrtDate[1];
	date2 = (date1<<8) | DIR_WrtDate[0];
	
	//�N
	date3 = ((0xFE00 & date2) >> 9) + 1980;	
	date3 %= 100;	
	file_date[0] = (unsigned char)(date3/10) + '0';
	file_date[1] = (unsigned char)(date3%10) + '0';
	
	//��
	date3 = ((0x1E0 & date2) >> 5);	
	file_date[2] = (unsigned char)(date3/10) + '0';
	file_date[3] = (unsigned char)(date3%10) + '0';
	
	//��
	date3 = (0x1F & date2);	
	file_date[4] = (unsigned char)(date3/10) + '0';
	file_date[5] = (unsigned char)(date3%10) + '0';	
}*/

// *******************************************************************************/
//   -Name(en)   :   search_file_name                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�h�t�@�C��������>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zBPB_BytsPerSec
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
int search_file_name(unsigned char *name, unsigned char file_type)
{
	unsigned int l;
	unsigned int search_byte_num, target_byte_num;
	int hit;
	unsigned char search_name;
	unsigned char start=0;
	
	hit_num = 0;		//�����ς�byte�N���A
	if((file_type & 0x0F) == FILE_NAME)	//�t�@�C��������
	{
		start = 0;	//�t�@�C�������������ꍇ�́A11�����̈�v���m�F����B
	}
	else if((file_type & 0x0F) == EXP_NAME)	//�g���q����
	{
		
		start = 8;	//�g���q���������ꍇ�A�ŏ���8�����̓t�@�C�����Ȃ̂Ŗ�������B
	}
	
	//1�����ڌ���
	search_name = *(name+start);	//search_name=&name => name=name+1
	for(target_byte_num=hit_num; target_byte_num<BPB_BytsPerSec; target_byte_num++)
	{
		if(SDreadBuffer[target_byte_num] == search_name)
		{
			hit_num = target_byte_num+1;	//���񌟍�����1�����ڈ�vbyte�̎�byte����J�n������B
			
			
			//2�����ڈȍ~��v�m�F
			for(search_byte_num=1; search_byte_num<11-start; search_byte_num++)	//File name��8byte�A�g���q��3byte
			{	
				//�g���q�݂̂Ō������Ă���ꍇ�A�폜�ς݃t�@�C���Ȃ�Ζ�������
				if((file_type & 0x0F) == EXP_NAME)
				{
					if(SDreadBuffer[target_byte_num-start] == 0xE5)
					{
						break;
					}
				}
				
				search_name = *(++name+start);	//search_name=&name => name=name+1
				if(SDreadBuffer[target_byte_num+search_byte_num] == search_name)
				{
					hit = 0x01;	//������v�t���O�A�T�[�g
				}
				else
				{							
					hit = 0x00;	//������v�t���O�l�Q�[�g
					//for(l=search_byte_num; l > start; l--)
					for(l=search_byte_num; l > 0; l--)
					{
						search_name = *(--name+start);
					}
					break;	//������v���Ȃ����i��for loop�𔲂��āA�ŏ��̕������猟���ĊJ�B
				}
			}
			
			if(hit == 0x01)
			{
				//�g���q�݂̂Ō������Ă���ꍇ�A�t�@�C�������擾����
				if((file_type & 0x0F) == EXP_NAME)
				{
					//hit_num��FILE_NAME�Ƒ�����
					hit_num = hit_num-start;
					
					for(search_byte_num=0; search_byte_num<8; search_byte_num++)
					{
						file_name[search_byte_num] = SDreadBuffer[hit_num-1+search_byte_num];
					}
				}
				
				//�S������v������target_byte_num��for loop�𔲂��Č����I���B
				break;
			}			
		}			
	}		
	
	return hit;
}






// *******************************************************************************/
//   -Name(en)   :   search_empty                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zBPB_BytsPerSec
//�y�֐��zSD_single_read�F�y�ϐ��zSDreadBuffer[]
int search_empty(unsigned int write_bytes)
{
	unsigned int search_byte_num, target_byte_num;
	int hit;
	
	hit_num = 0;
	//1�����ڌ���
	for(target_byte_num=0; target_byte_num<BPB_BytsPerSec; target_byte_num=target_byte_num+write_bytes)//������write_bytes��byte�����ōs���B
	{
		if(SDreadBuffer[target_byte_num] == 0x00)
		{
			//2�����ڈȍ~��v�m�F
			for(search_byte_num=1; search_byte_num<write_bytes; search_byte_num++)
			{
				if(SDreadBuffer[target_byte_num+search_byte_num] == 0x00)
				{
					hit = 0x01;	//������v�t���O�A�T�[�g
				}
				else
				{							
					hit = 0x00;	//������v�t���O�l�Q�[�g
					break;	//������v���Ȃ����i��for loop�𔲂��āA�ŏ��̕������猟���ĊJ�B
				}
			}
			
			if(hit == 0x01)
			{
				//�S������v������j��for loop�𔲂��Č����I���B
				break;
			}			
		}	
	}		
	
	hit_num = target_byte_num;
	
	return hit;
}



