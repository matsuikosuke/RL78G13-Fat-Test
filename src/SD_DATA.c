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
//   -Name(en)   :   read_SD_DATA_area                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
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
//   -Name(en)   :   write_SD_DATA_area                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
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
//   -Name(en)   :   target_DATA_MOVE_SubDir                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_single_read：【変数】SDreadBuffer[]
//【関数】search_file_name：【変数】hit_num
//write_SD_DATA_areaで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
void target_DATA_MOVE_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//サブディレクトリ32byteコピー＆削除
	for(i=0; i<32; i++)
	{
		//SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1-start];	
		SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1];				
	}
	SDreadBuffer[hit_num-1-start] = 0xE5;	//ファイル情報1文字目が0xE5ならば削除ファイルになる。
	
	//ルートディレクトリ上書き
	write_SD_DATA_area(now_sector);
}

// *******************************************************************************/
//   -Name(en)   :   target_DATA_CH_NAME_SubDir                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_single_read：【変数】SDreadBuffer[]
//【関数】search_file_name：【変数】hit_num
//【関数】なし：【変数】new_name[]
//write_SD_DATA_areaで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
void target_DATA_CH_NAME_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//課題★：startは不要かもしれない。
	if((file_type & 0x0F) == FILE_NAME)	//ファイル名検索
	{
		start = 0;	//ファイル名検索した場合hit_num-1がファイル情報先頭になる。	
	}
	else if((file_type & 0x0F) == EXP_NAME)	//拡張子検索
	{
		
		start = 8;	//拡張子検索した場合hit_num-1より8文字前がファイル情報先頭になる。
	}		
	
	/*//変更ファイル名更新
	for(i=0; i<5; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//サブディレクトリ名称変更
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1-start] = new_name[i];						
	}
	
	//サブディレクトリ上書き
	write_SD_DATA_area(now_sector);
}


// *******************************************************************************/
//   -Name(en)   :   target_DELETE_FILE_SubDir                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_single_read：【変数】SDreadBuffer[]
//【関数】search_file_name：【変数】hit_num
//【関数】なし：【変数】new_name[]
//write_SD_DATA_areaで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
void target_DELETE_FILE_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	int ret_search_file;
	unsigned int i;
	
	//クラスタ番号消去処理 1個目 & 次クラスタ番号取得
	ret_search_file = search_next_cluster_SD(DELETE);
	
	//最終クラスタ番号に到達するまで繰り返し処理
	while(SEARCH_SD_NEXT == ret_search_file)
	{
		DATA_cluster_num = next_DATA_cluster_num;
		ret_search_file = search_next_cluster_SD(DELETE);
	}	
}

// *******************************************************************************/
//   -Name(en)   :   target_DATA_MOVE_CH_NAME_SubDir                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_single_read：【変数】SDreadBuffer[]
//【関数】search_file_name：【変数】hit_num
//【関数】なし：【変数】new_name[]
//target_DATA_MOVE_SubDirで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec
//【関数】analysis_file_info_SD：【変数】DATA_cluster_num
void target_DATA_MOVE_CH_NAME_SubDir(unsigned char file_type, unsigned int now_sector)
{
	int ret;
	unsigned int i;
	int start=0;	
	
	//課題★：startは不要かもしれない。
	if((file_type & 0x0F) == FILE_NAME)	//ファイル名検索
	{
		start = 0;	//ファイル名検索した場合hit_num-1がファイル情報先頭になる。	
	}
	else if((file_type & 0x0F) == EXP_NAME)	//拡張子検索
	{
		
		start = 8;	//拡張子検索した場合hit_num-1より8文字前がファイル情報先頭になる。
	}		
	
	//変更ファイル名更新
	/*//ファイル日付データを名称にする。
	for(i=0; i<5; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';*/
	/*//事前に取得したファイル名と同一にする。
	for(i=0; i<8; i++)
	{
		new_name[i] = file_name[i];				
	}
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//ルートディレクトリ名称変更
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1-start] = new_name[i];						
	}
	
	//ルートディレクトリ32byteコピー＆削除＆ルートディレクトリ上書き
	target_DATA_MOVE_SubDir(file_type, now_sector);
}




// *******************************************************************************/
//   -Name(en)   :   search_dir_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードフォルダ内ファイル名検索>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, ROOT_sector_num, FAT_type
//【関数】read_SD_DATA_area to SD_single_read：【変数】SDreadBuffer[]
int search_dir_SD(unsigned char *file_name_now, unsigned char *org_dir_name_now, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	unsigned int now_sector;	
	int hit;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR解析：BPBの先頭セクタ導出
	analysis_SD_MBR();
	
	//BPB解析：FAT、ルートディレクトリ、データ領域の先頭セクタ、FATタイプの導出
	analysis_SD_BPB();
	
	//ルートディレクトリ解析：検索フォルダのデータクラスタ番号導出。この処理でDATA_cluster_numが更新される。
	//フォルダ名の検索なので拡張子ではなくファイル名検索にしないと関数analysis_SD_DIR内の変数startの値が不正になる。
	ret_search_file = analysis_SD_DIR(&org_dir_name_now[0], (FILE_NAME | SUB_FILE), DATA_READ);
	
	//ファイル検索一致なしの場合
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		ret_search_file = SEARCH_SD_NO_DIR;
	}
	//ファイル検索一致有りの場合
	else
	{
		//SDカードデータ続き情報取得
		ret_search_file = search_next_cluster_SD(NORMAL);
	
		while(1U)
		{	
			//(1) データ領域読み込み	
			DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
			
			for(now_sector=0; now_sector<BPB_SecPerClus; now_sector++)
			{
				read_SD_DATA_area(now_sector);
				
				//(2) ファイル名検索			
				hit = search_file_name(&file_name_now[0], file_type);
				
				//(3)-a 全文字一致した場合
				if(hit == 0x01)
				{
					//ファイル情報を取得したい場合
					if(target == DATA_READ)
					{
						//全文字一致したらnow_sectorのfor loopを抜けて検索終了。該当ファイルのクラスタ番号を記録する。
						analysis_file_info_SD();
						break;	
					}
					//ファイルを別の場所へ移動したい場合
					else if(target == DATA_MOVE)
					{
						//analysis_file_info_SD();					
						target_DATA_MOVE_SubDir(file_type, now_sector);
						break;			
					}	
					//ファイル名を変更したい場合
					else if(target == DATA_CH_NAME)
					{
						//get_file_date_SD();
						analysis_file_info_SD();
						target_DATA_CH_NAME_SubDir(file_type, now_sector);
						break;					
					}	
					//ファイル名を変更して、別の場所へ移動したい場合
					else if(target == DATA_MOVE_CH_NAME)
					{
						//get_file_date_SD();
						analysis_file_info_SD();	
						target_DATA_MOVE_CH_NAME_SubDir(file_type, now_sector);
						break;					
					}	
					//ファイルを削除したい場合
					else if(target == DELETE_FILE)
					{
						//サブディレクトリ名称先頭1文字変更
						SDreadBuffer[hit_num+0-1-0] = 0xE5;		
						
						//サブディレクトリ名称拡張子変更
						SDreadBuffer[hit_num+8-1-0] = 'T';
						SDreadBuffer[hit_num+9-1-0] = 'X';
						SDreadBuffer[hit_num+10-1-0] = 'T';
						
						//サブディレクトリ上書き
						write_SD_DATA_area(now_sector);	
						
						analysis_file_info_SD();
						target_DELETE_FILE_SubDir(file_type, now_sector);
						break;					
					}					
				}	
			}
			
			
			//全文字一致したらwhile loopを抜けて検索終了
			if(hit == 0x01)
			{
				break;					
			}	
			
			//(3)-b 全文字一致していない場合
			//データ続きが無い場合
			if(ret_search_file == SEARCH_SD_SUCCESS)
			{
				ret_search_file = SEARCH_SD_NO_FILE;
				break;			
			}
			//データ続きがある場合
			else if(ret_search_file == SEARCH_SD_NEXT)
			{
				DATA_cluster_num = next_DATA_cluster_num;
				ret_search_file = search_next_cluster_SD(NORMAL);
			}
			//エラーの場合
			else
			{
				break;	
			}
		
		}
	}	
	
	return ret_search_file;	
}
	


// *******************************************************************************/
//   -Name(en)   :   copy_empty_SD_SUB_DIR                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカード空きルートディレクトリ書き込み>                                      */
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
		
	//ルートディレクトリ解析：検索フォルダのデータクラスタ番号導出。この処理でDATA_cluster_numが更新される。
	ret_search_file = analysis_SD_DIR(&target_dir_name[0], ( FILE_NAME | SUB_FILE ), DATA_READ);
	
	if(ret_search_file == SEARCH_SD_NO_FILE)
	{
		
		ret_search_file = SEARCH_SD_NO_DIR;
	}
	else
	{				
		hit = 0x00;
		//SDカードデータ続き情報取得
		ret_search_file = search_next_cluster_SD(NORMAL);
		
		while(1U)
		{	
			//(1) データ read
			DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
			
			
			for(now_sector=0; now_sector<BPB_SecPerClus; now_sector++)
			{
				read_SD_DATA_area(now_sector);
				
				//(2) 空き領域検索					
				hit = search_empty(write_bytes);				
			
				//全文字一致した場合。ルートディレクトリ書き換え
				if(hit == 0x01)
				{				
					//ルートディレクトリ更新
					for(i=0; i<write_bytes; i++)
					{
						SDreadBuffer[hit_num+i] = SDrootDirBuffer[i];						
					}
					
					//ルートディレクトリ上書き
					write_SD_DATA_area(now_sector);
										
					break;	
				}					
			}
			
			
			//全文字一致したらwhile loopを抜けて検索終了
			if(hit == 0x01)
			{
				break;					
			}	
			
			//文字一致しなかった場合。
			//データ続きが無い場合
			if(ret_search_file == SEARCH_SD_SUCCESS)
			{
				ret_search_file = SEARCH_SD_NO_FILE;
				break;			
			}
			//データ続きがある場合
			else if(ret_search_file == SEARCH_SD_NEXT)
			{
				DATA_cluster_num = next_DATA_cluster_num;
				ret_search_file = search_next_cluster_SD(NORMAL);
			}
			//エラーの場合
			else
			{
				break;	
			}		
		}
	}
	
	return ret_search_file;
}


	