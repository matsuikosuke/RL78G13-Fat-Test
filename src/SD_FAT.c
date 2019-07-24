/***********************************************************************/
/*                                                                     */
/*  FILE        :SD_FAT.c                                        */
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



// ******************************************************************************/
//   -Name(en)   :   analysis_SD_MBR                                     �@�@�@	*/
//   -Name(jp)   :   <SD�J�[�hMBR��͏���>                              	*/
//   -OutLine    :   SD�J�[�h�̃A�h���X0����n�܂�MBR�̈�̃f�[�^����͂���B   */
//   -Input      :   None                                                       */
//   -Output     :   PT_BootID, PT_System, 					*/
//  		     MRB_top_CHS�iCHS�`���ŋ��̊J�n�����Z�N�^�j		*/ 
//		     MRB_last_CHS�iCHS�`���ŋ��̏I�������Z�N�^�j		*/     
//  		     MRB_top_LBA�iLBA�`���ŋ��̊J�n�����Z�N�^�ԍ��j		*/
//		     MRB_all_sectors�iLBA�`���ŋ��̃T�C�Y�j			*/                         
//   -Return     :   None                                                       */
// *******************************************************************************/
void analysis_SD_MBR(void)
{		
	int ret,i;
	unsigned long x1,x2,x3;
	
	////////////////////////////////////////////
	//(1) read MBR (Master Boot Record)
	////////////////////////////////////////////
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			ret = SD_single_read(0x00000000);
		}
		else
		{
			ret = SD_single_read(0x00000000);
		}
		if(ret == SD_READ_END) break;
	}
	
	////////////////////////////////////////////
	//(2) analysis MBR (Master Boot Record)
	////////////////////////////////////////////
	//MBR_Partation1(OFFSET:446)�̏ꍇ
	
	//PT_BootID(OFFSET:0, Size:1)
	///�u�[�g�W���B
	//0x00:�u�[�g�s��
	//0x80:�u�[�g��
	MRB_flag = SDreadBuffer[446];	
	
	//PT_System(OFFSET:4, Size:1)
	//���̋��̎��(��\�I�Ȃ���)�B
	//0x00: ����(�󂫃G���g��)
	//0x01: FAT12 (CHS/LBA, 65536�Z�N�^����)
	//0x04: FAT16 (CHS/LBA, 65536�Z�N�^����)
	//0x05: �g����� (CHS/LBA)
	//0x06: FAT12/16 (CHS/LBA, 65536�Z�N�^�ȏ�)
	//0x07: NTFS (CHS/LBA)
	//0x0B: FAT32 (CHS/LBA)
	//0x0C: FAT32 (LBA)
	//0x0E: FAT12/16 (LBA)
	//0x0F: �g����� (LBA)
	MRB_type = SDreadBuffer[450];	
	
	//PT_StartHd(OFFSET:1, Size:1)
	//CHS�`���ŋ��̊J�n�����Z�N�^�������w�b�h�ԍ�(0�`254)�B
	//PT_StartCySc(OFFSET:2, Size:2)
	//CHS�`���ŋ��̊J�n�����Z�N�^�������V�����_�ԍ�(����10�r�b�g:0�`1023)
	//�g���b�N���Z�N�^�ԍ�(���6�r�b�g:1�`63)�B
	x2 = SDreadBuffer[449];
	x3 = SDreadBuffer[448];	
	MRB_top_CHS = (x2<<16) | (x3<<8) | SDreadBuffer[447];	//CHS�`���ŋ��̊J�n�����Z�N�^
	
	//PT_EndHd(OFFSET:5, Size:1)
	//CHS�`���ŋ��̏I�������Z�N�^�������w�b�h�ԍ�(0�`254)�B
	//PT_EndCySc(OFFSET:6, Size:2)
	//CHS�`���ŋ��̏I�������Z�N�^�������V�����_�ԍ�(����10�r�b�g:0�`1023)
	//�g���b�N���Z�N�^�ԍ�(���6�r�b�g:1�`63)�B
	x2 = SDreadBuffer[453];
	x3 = SDreadBuffer[452];	
	MRB_last_CHS = (x2<<16) | (x3<<8) | SDreadBuffer[451];	//CHS�`���ŋ��̏I�������Z�N�^
	
	//PT_LbaOfs(OFFSET:8, Size:4)
	//LBA�`���ŋ��̊J�n�����Z�N�^�ԍ�������(1�`0xFFFFFFFF)�B
	x1 = SDreadBuffer[457];
	x2 = SDreadBuffer[456];
	x3 = SDreadBuffer[455];	
	MRB_top_LBA = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[454];	//LBA�`���ŋ��̊J�n�����Z�N�^�ԍ�
	
	
	//PT_LbaSize(OFFSET:12, Size:4)
	//LBA�`���ŋ��̃T�C�Y������(1�`0xFFFFFFFF)�B
	x1 = SDreadBuffer[461];
	x2 = SDreadBuffer[460];
	x3 = SDreadBuffer[459];	
	MRB_all_sectors = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[458];	//LBA�`���ŋ��̃T�C�Y
	
	//�i�⑫�j���͈̔͂̕\���ɂ́ACHS�`����LBA�`����2�ʂ肪����B
}
	

// *******************************************************************************/
//   -Name(en)   :   analysis_SD_BPB                                     �@�@�@�@ */
//   -Name(jp)   :   <SD�J�[�hBPB��͏���>                              */
//   -OutLine    :                       					 */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zanalysis_SD_MBR�F�y�ϐ��zMRB_top_LBA, MRB_all_sectors 
void analysis_SD_BPB(void)
{		
	int ret,i;
	unsigned long x1,x2,x3;
	unsigned int y1;
	unsigned long CountofClusters;
	unsigned long BPB_start_address;
	unsigned long FAT_start_address;	
	unsigned long ROOT_start_address;

	////////////////////////////////////////////
	//(1) read BPB (BIOS Parameter Blobk) 
	////////////////////////////////////////////
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			ret = SD_single_read(MRB_top_LBA);
		}
		else
		{
			BPB_start_address = MRB_top_LBA * (unsigned long)SDsectorSize;	
			ret = SD_single_read(BPB_start_address);
		}
		if(ret == SD_READ_END) break;
	}
	
	////////////////////////////////////////////
	//(2) analysis BPB (BIOS Parameter Blobk) 
	////////////////////////////////////////////	
	
	//1�Z�N�^������̃o�C�g��
	y1 = SDreadBuffer[12];
	BPB_BytsPerSec = (y1<<8) | SDreadBuffer[11];
	
	//1�N���X�^������̃Z�N�^��
	BPB_SecPerClus = SDreadBuffer[13];
	
	//�\��Z�N�^�� (FAT�̈�̗\��Z�N�^��)
	y1 = SDreadBuffer[15];
	BPB_RsvdSecCnt = (y1<<8) | SDreadBuffer[14];

	//FAT�̐�
	BPB_NumFATs = SDreadBuffer[16];

	//���[�g�f�B���N�g���G���g���� (FAT32 �ł� 0)
	y1 = SDreadBuffer[18];
	BPB_RootEntCnt = (y1<<8) | SDreadBuffer[17];	
	
	
	//FAT�̈�̊J�n�ʒu�ƃT�C�Y�̌v�Z
	FAT_start_sector = MRB_top_LBA + (unsigned long)BPB_RsvdSecCnt;
	FAT_start_address = FAT_start_sector * (unsigned long)BPB_BytsPerSec; 	
		
	//FAT �̃Z�N�^�� (FAT32 �ł� 0�B�����BPB_FATSz32���g����B)
	y1 = SDreadBuffer[23];
	BPB_FATSz16 = (y1<<8) | SDreadBuffer[22];
	
	//FAT12/16��FAT32����FAT�T�C�Y�̌v�Z�Ɏg�p����ϐ����قȂ�B
	if(BPB_FATSz16 == 0x00)	//BPB_FATSz16�������Ȃ��FAT32
	{	
		//1��FAT�����Z�N�^�� 
		x1 = SDreadBuffer[39];
		x2 = SDreadBuffer[38];
		x3 = SDreadBuffer[37];	
		BPB_FATSz32 = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[36];		
		
		BPB_FATSz = BPB_FATSz32;
		
	}
	else
	{
		BPB_FATSz = BPB_FATSz16;
	}
	
	//�SFAT�����Z�N�^�� 		
	FAT_sectors = (unsigned long)BPB_FATSz * (unsigned long)BPB_NumFATs;
	
	//���[�g�f�B���N�g���̊J�n�ʒu�ƃT�C�Y�̌v�Z
	//1�̃��[�g�f�B���N�g���̃T�C�Y��32byte�Ƃ��Čv�Z
	ROOT_start_sector = FAT_start_sector + FAT_sectors;
	ROOT_start_address = FAT_start_address + FAT_sectors * (unsigned long)BPB_BytsPerSec;
	ROOT_sectors = (32 *(unsigned long)BPB_RootEntCnt + (unsigned long)BPB_BytsPerSec -1) / (unsigned long)BPB_BytsPerSec;
	
	//�f�[�^�̈�̊J�n�ʒu�ƃT�C�Y�̌v�Z
	DATA_start_sector = ROOT_start_sector + ROOT_sectors;
	DATA_start_address = DATA_start_sector * (unsigned long)BPB_BytsPerSec;
	DATA_sectors = MRB_all_sectors - DATA_start_sector;
	
	//FAT type�̌v�Z
	CountofClusters = DATA_sectors / (unsigned long)BPB_SecPerClus;
	if(CountofClusters <= 4085)
	{
		FAT_type = FAT12;
	}
	else if(CountofClusters >= 65526)
	{
		FAT_type = FAT32;		
	}
	else
	{
		FAT_type = FAT16;		
	}	
	
	//FAT32�ɂ�����I�t�Z�b�g36�ȍ~�̃t�B�[���h���擾
	if(FAT_type == FAT32)
	{
		//���[�g�f�B���N�g���̐擪�N���X�^�ԍ� 
		x1 = SDreadBuffer[47];
		x2 = SDreadBuffer[46];
		x3 = SDreadBuffer[45];	
		BPB_RootClus = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[44];	
		
		
		//read FSINFO
		for(i=0; i<20; i++)
		{
			ret = 0;
			if(sd_mode == SDHC_MODE)
			{
				ret = SD_single_read(MRB_top_LBA+1);
			}
			else
			{
				BPB_start_address = (MRB_top_LBA+1) * (unsigned long)SDsectorSize;	
				ret = SD_single_read(BPB_start_address);
			}
			if(ret == SD_READ_END) break;
		}
		
		//�Ō�Ɋ��蓖�Ă�ꂽ�N���X�^�ԍ�
		x1 = SDreadBuffer[495];
		x2 = SDreadBuffer[494];
		x3 = SDreadBuffer[493];	
		FSI_Nxt_Free = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[492];
		
	}
}


// *******************************************************************************/
//   -Name(en)   :   search_cluster_address_SD                                     �@�@�@�@�@�@ �@*/
//   -Name(jp)   :   <SD�J�[�hFAT��͏����i���N���X�^���擾�j>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zFAT_start_sector, BPB_BytsPerSec, FAT_type
void search_cluster_address_SD(unsigned long cluster_num, unsigned char target)
{		
	unsigned long FAT_N_address;
	//unsigned long FAT_N_offset_long;
	int ret,i;
	unsigned long x1,x2,x3,x4;
	
	//(1)�N���X�^#N�̃G���g���ʒu�v�Z
	if(FAT_type == FAT32)
	{
		FAT_N_sector = FAT_start_sector + (cluster_num * 4) / (unsigned long)BPB_BytsPerSec;
		FAT_N_offset = (cluster_num * 4) % (unsigned long)BPB_BytsPerSec;
	}
	else if(FAT_type == FAT16)
	{
		FAT_N_sector = FAT_start_sector + (cluster_num * 2)/ (unsigned long)BPB_BytsPerSec;
		FAT_N_offset = (cluster_num * 2) % (unsigned long)BPB_BytsPerSec;
		//FAT_N_offset_long = (cluster_num * 2) % (unsigned long)BPB_BytsPerSec;
		//FAT_N_offset = (unsigned int)FAT_N_offset_long;
	}
	else if(FAT_type == FAT12)
	{
		FAT_N_sector = FAT_start_sector + (cluster_num + (cluster_num/2))/ (unsigned long)BPB_BytsPerSec;
		FAT_N_offset = (cluster_num + (cluster_num/2)) % (unsigned long)BPB_BytsPerSec;
		//FAT_N_offset_long = (cluster_num + (cluster_num/2)) % (unsigned long)BPB_BytsPerSec;
		//FAT_N_offset = (unsigned int)FAT_N_offset_long;
	}
	
	//(2)read FAT	
	for(i=0; i<20; i++)
	{
		ret = 0;
		if(sd_mode == SDHC_MODE)
		{
			ret = SD_single_read(FAT_N_sector);
		}
		else
		{
			FAT_N_address = FAT_N_sector*(unsigned long)BPB_BytsPerSec;
			ret = SD_single_read(FAT_N_address);
		}
		if(ret == SD_READ_END) break;
	}
	
	//(3)�N���X�^#N�̃G���g�����
	if(FAT_type == FAT32)
	{
		x1 = SDreadBuffer[FAT_N_offset+3];
		x2 = SDreadBuffer[FAT_N_offset+2];
		x3 = SDreadBuffer[FAT_N_offset+1];	
		next_DATA_cluster_num = ((x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[FAT_N_offset]) & 0x0FFFFFFF;
		
		
		//�������[�h�̏ꍇ�N���X�^�ԍ���0x00�ŏ㏑��
		if(DELETE == target)
		{
			SDreadBuffer[FAT_N_offset] = 0x00;
			SDreadBuffer[FAT_N_offset+1] = 0x00;
			SDreadBuffer[FAT_N_offset+2] = 0x00;
			SDreadBuffer[FAT_N_offset+3] = 0x00;
			if(sd_mode == SDHC_MODE)
			{
				ret = SD_single_write(FAT_N_sector);
			}
			else
			{
				ret = SD_single_write(FAT_N_address);
			}
		}
	}
	else if(FAT_type == FAT16)
	{
		x1 = 0x00;
		x2 = 0x00;
		x3 = SDreadBuffer[FAT_N_offset+1];	
		next_DATA_cluster_num = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[FAT_N_offset];
		
		//�������[�h�̏ꍇ�N���X�^�ԍ���0x00�ŏ㏑��
		if(DELETE == target)
		{
			SDreadBuffer[FAT_N_offset] = 0x00;
			SDreadBuffer[FAT_N_offset+1] = 0x00;
			if(sd_mode == SDHC_MODE)
			{
				ret = SD_single_write(FAT_N_sector);
			}
			else
			{
				ret = SD_single_write(FAT_N_address);
			}
		}
	}
	else if(FAT_type == FAT12)
	{
		//�N���X�^�ԍ���̏ꍇ
		if(cluster_num & 1)
		{
			x3 = SDreadBuffer[FAT_N_offset+1]<<4;
			x4 = SDreadBuffer[FAT_N_offset]>>4;
		}
		//�N���X�^�ԍ������̏ꍇ
		else
		{
			x3 = (SDreadBuffer[FAT_N_offset+1] & 0x0F) << 8;
			x4 = SDreadBuffer[FAT_N_offset];
		}		
		
		next_DATA_cluster_num = (x3<<8) | x4;
		
		//�������[�h�̏ꍇ�N���X�^�ԍ���0x00�ŏ㏑��
		if(DELETE == target)
		{
			SDreadBuffer[FAT_N_offset] = 0x00;
			SDreadBuffer[FAT_N_offset+1] = 0x00;
			if(sd_mode == SDHC_MODE)
			{
				ret = SD_single_write(FAT_N_sector);
			}
			else
			{
				ret = SD_single_write(FAT_N_address);
			}
		}
	}	
}




// *******************************************************************************/
//   -Name(en)   :   search_next_cluster_SD                                     �@�@*/
//   -Name(jp)   :   <SD�J�[�h�f�[�^�������擾>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//�{�֐����g�p����O�ɂ́A�ȉ��̊֐��ŕϐ����擾���Ȃ��Ƃ����Ȃ��B
//�y�֐��zSD_initialize�F�y�ϐ��zsd_mode
//�y�֐��zanalysis_SD_BPB�F�y�ϐ��zFAT_start_sector, BPB_BytsPerSec, FAT_type
//���O�̌v�Z�����F�y�ϐ��zDATA_cluster_num�i���݂̃f�[�^�̃N���X�^�ԍ��j
int search_next_cluster_SD(unsigned char target)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//FAT��́F�f�[�^�����̊i�[���ꂽ�f�[�^�N���X�^�ԍ����o
	search_cluster_address_SD(DATA_cluster_num, target);
	
	if(FAT_type == FAT32)
	{
		if( (next_DATA_cluster_num & 0x0FFFFFF) == 0x0FFFFFF )
		{			
			ret_search_file = SEARCH_SD_SUCCESS;
		}
		else
		{
			ret_search_file = SEARCH_SD_NEXT;				
		}
	}
	else if(FAT_type == FAT16)
	{
		if( (next_DATA_cluster_num & 0xFFF8) == 0xFFF8 )
		{			
			ret_search_file = SEARCH_SD_SUCCESS;
		}
		else
		{
			ret_search_file = SEARCH_SD_NEXT;				
		}
		
	}
	else if(FAT_type == FAT12)
	{
		if( (next_DATA_cluster_num & 0xFF8) == 0xFF8 )
		{			
			ret_search_file = SEARCH_SD_SUCCESS;
		}
		else
		{
			ret_search_file = SEARCH_SD_NEXT;				
		}
		
	}				
	
	return ret_search_file;
}
