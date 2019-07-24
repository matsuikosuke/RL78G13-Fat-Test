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
//   -Name(en)   :   analysis_SD_MBR                                     　　　	*/
//   -Name(jp)   :   <SDカードMBR解析処理>                              	*/
//   -OutLine    :   SDカードのアドレス0から始まるMBR領域のデータを解析する。   */
//   -Input      :   None                                                       */
//   -Output     :   PT_BootID, PT_System, 					*/
//  		     MRB_top_CHS（CHS形式で区画の開始物理セクタ）		*/ 
//		     MRB_last_CHS（CHS形式で区画の終了物理セクタ）		*/     
//  		     MRB_top_LBA（LBA形式で区画の開始物理セクタ番号）		*/
//		     MRB_all_sectors（LBA形式で区画のサイズ）			*/                         
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
	//MBR_Partation1(OFFSET:446)の場合
	
	//PT_BootID(OFFSET:0, Size:1)
	///ブート標識。
	//0x00:ブート不可
	//0x80:ブート可
	MRB_flag = SDreadBuffer[446];	
	
	//PT_System(OFFSET:4, Size:1)
	//この区画の種類(代表的なもの)。
	//0x00: 無し(空きエントリ)
	//0x01: FAT12 (CHS/LBA, 65536セクタ未満)
	//0x04: FAT16 (CHS/LBA, 65536セクタ未満)
	//0x05: 拡張区画 (CHS/LBA)
	//0x06: FAT12/16 (CHS/LBA, 65536セクタ以上)
	//0x07: NTFS (CHS/LBA)
	//0x0B: FAT32 (CHS/LBA)
	//0x0C: FAT32 (LBA)
	//0x0E: FAT12/16 (LBA)
	//0x0F: 拡張区画 (LBA)
	MRB_type = SDreadBuffer[450];	
	
	//PT_StartHd(OFFSET:1, Size:1)
	//CHS形式で区画の開始物理セクタを示すヘッド番号(0～254)。
	//PT_StartCySc(OFFSET:2, Size:2)
	//CHS形式で区画の開始物理セクタを示すシリンダ番号(下位10ビット:0～1023)
	//トラック内セクタ番号(上位6ビット:1～63)。
	x2 = SDreadBuffer[449];
	x3 = SDreadBuffer[448];	
	MRB_top_CHS = (x2<<16) | (x3<<8) | SDreadBuffer[447];	//CHS形式で区画の開始物理セクタ
	
	//PT_EndHd(OFFSET:5, Size:1)
	//CHS形式で区画の終了物理セクタを示すヘッド番号(0～254)。
	//PT_EndCySc(OFFSET:6, Size:2)
	//CHS形式で区画の終了物理セクタを示すシリンダ番号(下位10ビット:0～1023)
	//トラック内セクタ番号(上位6ビット:1～63)。
	x2 = SDreadBuffer[453];
	x3 = SDreadBuffer[452];	
	MRB_last_CHS = (x2<<16) | (x3<<8) | SDreadBuffer[451];	//CHS形式で区画の終了物理セクタ
	
	//PT_LbaOfs(OFFSET:8, Size:4)
	//LBA形式で区画の開始物理セクタ番号を示す(1～0xFFFFFFFF)。
	x1 = SDreadBuffer[457];
	x2 = SDreadBuffer[456];
	x3 = SDreadBuffer[455];	
	MRB_top_LBA = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[454];	//LBA形式で区画の開始物理セクタ番号
	
	
	//PT_LbaSize(OFFSET:12, Size:4)
	//LBA形式で区画のサイズを示す(1～0xFFFFFFFF)。
	x1 = SDreadBuffer[461];
	x2 = SDreadBuffer[460];
	x3 = SDreadBuffer[459];	
	MRB_all_sectors = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[458];	//LBA形式で区画のサイズ
	
	//（補足）区画の範囲の表現には、CHS形式とLBA形式の2通りがある。
}
	

// *******************************************************************************/
//   -Name(en)   :   analysis_SD_BPB                                     　　　　 */
//   -Name(jp)   :   <SDカードBPB解析処理>                              */
//   -OutLine    :                       					 */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_MBR：【変数】MRB_top_LBA, MRB_all_sectors 
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
	
	//1セクタあたりのバイト数
	y1 = SDreadBuffer[12];
	BPB_BytsPerSec = (y1<<8) | SDreadBuffer[11];
	
	//1クラスタあたりのセクタ数
	BPB_SecPerClus = SDreadBuffer[13];
	
	//予約セクタ数 (FAT領域の予約セクタ数)
	y1 = SDreadBuffer[15];
	BPB_RsvdSecCnt = (y1<<8) | SDreadBuffer[14];

	//FATの数
	BPB_NumFATs = SDreadBuffer[16];

	//ルートディレクトリエントリ数 (FAT32 では 0)
	y1 = SDreadBuffer[18];
	BPB_RootEntCnt = (y1<<8) | SDreadBuffer[17];	
	
	
	//FAT領域の開始位置とサイズの計算
	FAT_start_sector = MRB_top_LBA + (unsigned long)BPB_RsvdSecCnt;
	FAT_start_address = FAT_start_sector * (unsigned long)BPB_BytsPerSec; 	
		
	//FAT のセクタ数 (FAT32 では 0。代わりにBPB_FATSz32が使われる。)
	y1 = SDreadBuffer[23];
	BPB_FATSz16 = (y1<<8) | SDreadBuffer[22];
	
	//FAT12/16かFAT32かでFATサイズの計算に使用する変数が異なる。
	if(BPB_FATSz16 == 0x00)	//BPB_FATSz16が無効ならばFAT32
	{	
		//1個のFATが持つセクタ数 
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
	
	//全FATが持つセクタ数 		
	FAT_sectors = (unsigned long)BPB_FATSz * (unsigned long)BPB_NumFATs;
	
	//ルートディレクトリの開始位置とサイズの計算
	//1個のルートディレクトリのサイズを32byteとして計算
	ROOT_start_sector = FAT_start_sector + FAT_sectors;
	ROOT_start_address = FAT_start_address + FAT_sectors * (unsigned long)BPB_BytsPerSec;
	ROOT_sectors = (32 *(unsigned long)BPB_RootEntCnt + (unsigned long)BPB_BytsPerSec -1) / (unsigned long)BPB_BytsPerSec;
	
	//データ領域の開始位置とサイズの計算
	DATA_start_sector = ROOT_start_sector + ROOT_sectors;
	DATA_start_address = DATA_start_sector * (unsigned long)BPB_BytsPerSec;
	DATA_sectors = MRB_all_sectors - DATA_start_sector;
	
	//FAT typeの計算
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
	
	//FAT32におけるオフセット36以降のフィールド情報取得
	if(FAT_type == FAT32)
	{
		//ルートディレクトリの先頭クラスタ番号 
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
		
		//最後に割り当てられたクラスタ番号
		x1 = SDreadBuffer[495];
		x2 = SDreadBuffer[494];
		x3 = SDreadBuffer[493];	
		FSI_Nxt_Free = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[492];
		
	}
}


// *******************************************************************************/
//   -Name(en)   :   search_cluster_address_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードFAT解析処理（次クラスタ情報取得）>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】FAT_start_sector, BPB_BytsPerSec, FAT_type
void search_cluster_address_SD(unsigned long cluster_num, unsigned char target)
{		
	unsigned long FAT_N_address;
	//unsigned long FAT_N_offset_long;
	int ret,i;
	unsigned long x1,x2,x3,x4;
	
	//(1)クラスタ#Nのエントリ位置計算
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
	
	//(3)クラスタ#Nのエントリ解析
	if(FAT_type == FAT32)
	{
		x1 = SDreadBuffer[FAT_N_offset+3];
		x2 = SDreadBuffer[FAT_N_offset+2];
		x3 = SDreadBuffer[FAT_N_offset+1];	
		next_DATA_cluster_num = ((x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[FAT_N_offset]) & 0x0FFFFFFF;
		
		
		//消去モードの場合クラスタ番号を0x00で上書き
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
		
		//消去モードの場合クラスタ番号を0x00で上書き
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
		//クラスタ番号奇数の場合
		if(cluster_num & 1)
		{
			x3 = SDreadBuffer[FAT_N_offset+1]<<4;
			x4 = SDreadBuffer[FAT_N_offset]>>4;
		}
		//クラスタ番号偶数の場合
		else
		{
			x3 = (SDreadBuffer[FAT_N_offset+1] & 0x0F) << 8;
			x4 = SDreadBuffer[FAT_N_offset];
		}		
		
		next_DATA_cluster_num = (x3<<8) | x4;
		
		//消去モードの場合クラスタ番号を0x00で上書き
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
//   -Name(en)   :   search_next_cluster_SD                                     　　*/
//   -Name(jp)   :   <SDカードデータ続き情報取得>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】FAT_start_sector, BPB_BytsPerSec, FAT_type
//事前の計算処理：【変数】DATA_cluster_num（現在のデータのクラスタ番号）
int search_next_cluster_SD(unsigned char target)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//FAT解析：データ続きの格納されたデータクラスタ番号導出
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
