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
//   -Name(en)   :   read_SD_DIR                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
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
//   -Name(en)   :   write_SD_DIR_Root                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
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
//   -Name(en)   :   target_DATA_MOVE_Root                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_single_read：【変数】SDreadBuffer[]
//【関数】search_file_name：【変数】hit_num
//write_SD_DIR_Rootで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_MOVE_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{
	int ret;
	unsigned int i;
	
	//ルートディレクトリ32byteコピー＆削除
	for(i=0; i<32; i++)
	{
		SDrootDirBuffer[i] = SDreadBuffer[hit_num+i-1];
		//SDreadBuffer[hit_num+i-1-start] = 0x00;						
	}
	SDreadBuffer[hit_num-1] = 0xE5;
	
	//ルートディレクトリ上書き
	write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);	
	
}


// *******************************************************************************/
//   -Name(en)   :   target_DATA_CH_NAME_Root                                     　　　　　　 　*/
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
//write_SD_DIR_Rootで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_CH_NAME_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{
	int ret;
	unsigned int i;
	
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
	
	//ルートディレクトリ名称変更
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1] = new_name[i];						
	}
		
	//ルートディレクトリ上書き
	write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);		
}



// *******************************************************************************/
//   -Name(en)   :   target_DELETE_FILE_Root                                     　　　　　　 　*/
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
//write_SD_DIR_Rootで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DELETE_FILE_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
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
//   -Name(en)   :   target_DATA_MOVE_CH_NAME_Root                                     　　　　　　 　*/
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
//target_DATA_COPY_Rootで必要になる変数
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, FAT_type
void target_DATA_MOVE_CH_NAME_Root(unsigned char file_type, unsigned long ROOT_sector_num, unsigned long now_cluster_num)
{			
	unsigned int i;
	
	/*//変更ファイル名更新
	for(i=0; i<6; i++)
	{
		new_name[i] = file_date[i];				
	}
	new_name[6] 	= 'S';
	new_name[7] 	= '~';
	new_name[8] 	= 'B';
	new_name[9] 	= 'I';
	new_name[10] 	= 'N';*/
	
	//ルートディレクトリ名称変更
	for(i=0; i<11; i++)
	{
		SDreadBuffer[hit_num+i-1] = new_name[i];						
	}
					
	//ルートディレクトリ32byteコピー＆削除＆ルートディレクトリ上書き			
	target_DATA_MOVE_Root(file_type, ROOT_sector_num, now_cluster_num);
}


// *******************************************************************************/
//   -Name(en)   :   analysis_SD_DIR                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードルートディレクトリ解析処理（ファイル名検索）>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】SD_initialize：【変数】sd_mode
//【関数】analysis_SD_BPB：【変数】DATA_start_sector, BPB_SecPerClus, BPB_BytsPerSec, ROOT_start_sector, ROOT_sector_num, FAT_type
//【関数】read_SD_DIR to SD_single_read：【変数】SDreadBuffer[]
int analysis_SD_DIR(unsigned char *name, unsigned char file_type, unsigned char target)
{		
	int ret_search_file = 0;
	int hit;
	unsigned long ROOT_sector_num;
	unsigned long now_cluster_num;
	unsigned long search_sector_num;
	
	//read_SD_DIR関数を使う場合は、以下の処理を事前に必ず行うこと
	//FAT32とFAT12/16ではルートディレクトリの上限数が異なる。
	if(FAT_type == FAT32)
	{
		//FTA32はルートディレクトリ領域が連続しているとは限らないので1クラスタ分のセクタを読み込んでから、次クラスタ番号を検索する。
		search_sector_num = BPB_SecPerClus;		
		//FAT32の場合、ルートディレクトリはデータ領域にあり、その開始クラスタ番号はBPB_RootClusに格納される。
		now_cluster_num = BPB_RootClus;
	}
	else
	{
		//ルートディレクトリのセクター数
		search_sector_num = ROOT_sectors;
		//FAT16の場合、ルートディレクトリ領域は1クラスタ以下なので番号は不要
		now_cluster_num = 0;
	}
	
	hit = 0x00;
	while(1U)
	{
		for(ROOT_sector_num=0; ROOT_sector_num<search_sector_num ; ROOT_sector_num++)
		{
			//(1) ルートディレクトリ読み込み	
			read_SD_DIR(ROOT_sector_num, now_cluster_num);			
			
			//(2) ファイル名検索	
			hit = search_file_name(&name[0], file_type);	//hit_num更新
			
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
					target_DATA_MOVE_Root(file_type, ROOT_sector_num, now_cluster_num);
					break;				
				}	
				//ファイル名を変更したい場合
				else if(target == DATA_CH_NAME)
				{	
					//get_file_date_SD();
					analysis_file_info_SD();					
					target_DATA_CH_NAME_Root(file_type, ROOT_sector_num, now_cluster_num);
					break;				
				}
				//ファイル名を変更して、別の場所へ移動したい場合
				else if(target == DATA_MOVE_CH_NAME)
				{	
					//get_file_date_SD();
					analysis_file_info_SD();					
					target_DATA_MOVE_CH_NAME_Root(file_type, ROOT_sector_num, now_cluster_num);	
					break;	
				}
				//ファイルを削除したい場合
				else if(target == DELETE_FILE)
				{	
					
					//ルートディレクトリ名称先頭1文字変更
					SDreadBuffer[hit_num+0-1] = 0xE5;	
					
					//ルートディレクトリ名称拡張子変更
					SDreadBuffer[hit_num+8-1] = 'T';
					SDreadBuffer[hit_num+9-1] = 'X';
					SDreadBuffer[hit_num+10-1] = 'T';
							
					//ルートディレクトリ上書き
					write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);	
					
					analysis_file_info_SD();					
					target_DELETE_FILE_Root(file_type, ROOT_sector_num, now_cluster_num);
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
		if(FAT_type == FAT32)
		{
			//FAT解析：検索データ続きの格納されたデータクラスタ番号導出
			search_cluster_address_SD(BPB_RootClus, NORMAL);
			//データ続きが無い場合	
			if(next_DATA_cluster_num = 0x0FFFFFFF)
			{
				//ルートディレクトリ領域を全て検索しているので、一致なしでwhile loopを抜けて検索終了
				ret_search_file = SEARCH_SD_NO_FILE;
				break;
			}
			//データ続きがある場合
			else
			{
				//FATのクラスタ番号が0x0FFFFFFFになるまでWhile loopで検索を継続する。
				now_cluster_num = next_DATA_cluster_num;
			}
		}
		else
		{
			//FAT32以外では、この時点でルートディレクトリ領域を全て検索しているので、一致なしでwhile loopを抜けて検索終了
			ret_search_file = SEARCH_SD_NO_FILE;
			break;				
		}				
	}
	
	return	ret_search_file;
}




// *******************************************************************************/
//   -Name(en)   :   search_file_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードファイル名検索>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int search_file_SD(unsigned char *name, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR解析：BPBの先頭セクタ導出
	analysis_SD_MBR();
	
	//BPB解析：FAT、ルートディレクトリ、データ領域の先頭セクタ、FATタイプの導出
	analysis_SD_BPB();
	
	//ルートディレクトリ解析：検索ファイルのデータクラスタ番号導出
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
		//SDカードデータ続き情報取得
		ret_search_file = search_next_cluster_SD(NORMAL);
	}
	
	return ret_search_file;
}




// *******************************************************************************/
//   -Name(en)   :   delete_file_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードファイル削除>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int delete_file_SD(unsigned char *name, unsigned char file_type)
{
	int ret_search_file;
	
	ret_search_file = SEARCH_SD_FAIL;
	
	//MPR解析：BPBの先頭セクタ導出
	analysis_SD_MBR();
	
	//BPB解析：FAT、ルートディレクトリ、データ領域の先頭セクタ、FATタイプの導出
	analysis_SD_BPB();
	
	//ルートディレクトリ解析：検索ファイルのデータクラスタ番号導出
	ret_search_file = analysis_SD_DIR(&name[0], file_type, DELETE_FILE);
	
	
	return ret_search_file;
}



// *******************************************************************************/
//   -Name(en)   :   copy_empty_SD_DIR                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカード空きルートディレクトリ書き込み>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_DIR：【変数】SDrootDirBuffer（コピーしたいファイルのファイル情報）
int copy_empty_SD_DIR(unsigned int write_bytes)
{
	int ret_search_area = 0;
	int hit;
	unsigned int i,j;
	unsigned long ROOT_sector_num;
	unsigned long now_cluster_num;
	unsigned long search_sector_num;
	
	
	//FAT32とFAT12/16ではルートディレクトリの上限数が異なる。
	if(FAT_type == FAT32)
	{
		//FTA32はルートディレクトリ領域が連続しているとは限らないので1クラスタ分のセクタを読み込んでから、次クラスタ番号を検索する。
		search_sector_num = BPB_SecPerClus;		
		//FAT32の場合、ルートディレクトリはデータ領域にあり、その開始クラスタ番号はBPB_RootClusに格納される。
		now_cluster_num = BPB_RootClus;
	}
	else
	{
		//ルートディレクトリのセクター数
		search_sector_num = ROOT_sectors;
		//FAT16の場合、ルートディレクトリ領域は1クラスタ以下なので番号は不要
		now_cluster_num = 0;				
	}
			
	hit = 0x00;
	while(1U)
	{
		for(ROOT_sector_num=0; ROOT_sector_num<search_sector_num ; ROOT_sector_num++)
		{
			//(1) read ROOT	
			read_SD_DIR(ROOT_sector_num, now_cluster_num);
						
			//(2) 空き領域検索				
			hit = search_empty(write_bytes);	
			
			//(3)-a 全文字一致した場合。ルートディレクトリ書き換え
			if(hit == 0x01)
			{				
				//ルートディレクトリ更新
				for(i=0; i<write_bytes; i++)
				{
					SDreadBuffer[j+i] = SDrootDirBuffer[i];						
				}
				
				write_SD_DIR_Root(ROOT_sector_num, now_cluster_num);
										
				break;	
			}				
		}
		
		//全文字一致したらwhile loopを抜けて検索終了
		if(hit == 0x01)
		{
			break;					
		}	
		
		//(3)-b 全文字一致していない場合
		if(FAT_type == FAT32)
		{
			//FAT解析：検索データ続きの格納されたデータクラスタ番号導出
			search_cluster_address_SD(BPB_RootClus, NORMAL);	
			if(next_DATA_cluster_num = 0x0FFFFFFF)
			{
				//ルートディレクトリ領域を全て検索しているので、一致なしでwhile loopを抜けて検索終了
				ret_search_area = SEARCH_SD_NO_AREA;
				break;
			}
			else
			{
				//FATのクラスタ番号が0x0FFFFFFFになるまでWhile loopで検索を継続する。
				now_cluster_num = next_DATA_cluster_num;
			}
		}
		else
		{
			//FAT32以外では、この時点でルートディレクトリ領域を全て検索しているので、一致なしでwhile loopを抜けて検索終了
			ret_search_area = SEARCH_SD_NO_AREA;
			break;				
		}				
	}
	
	return ret_search_area;
}



// *******************************************************************************/
//   -Name(en)   :   analysis_file_info_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_BPB：【変数】BPB_BytsPerSec
//【関数】SD_single_read：【変数】SDreadBuffer[]
void analysis_file_info_SD(void)
{
	unsigned long x1,x2,x3;
	unsigned int date1, date2, date3;
	unsigned char one, ten;
	
	//ファイル全クラスタ数
	x1 = SDreadBuffer[hit_num+21-1];
	x2 = SDreadBuffer[hit_num+20-1];
	x3 = SDreadBuffer[hit_num+27-1];	
	DATA_cluster_num = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[hit_num+26-1];
	
	//ファイル全バイト数
	x1 = SDreadBuffer[hit_num+31-1];
	x2 = SDreadBuffer[hit_num+30-1];
	x3 = SDreadBuffer[hit_num+29-1];	
	all_DATA_bytes = (x1<<24) | (x2<<16) | (x3<<8) | SDreadBuffer[hit_num+28-1];
	
	//ファイル全セクタ数
	all_DATA_sectors = (all_DATA_bytes + BPB_BytsPerSec - 1) / BPB_BytsPerSec;
	
	//ファイル作成時刻
	DIR_CrtTime[0] =  SDreadBuffer[hit_num+14-1];
	DIR_CrtTime[1] =  SDreadBuffer[hit_num+15-1];
	
	//ファイル作成日
	DIR_CrtDate[0] =  SDreadBuffer[hit_num+16-1];
	DIR_CrtDate[1] =  SDreadBuffer[hit_num+17-1];
		
	//ファイルオープン日
	DIR_LstAccDate[0] =  SDreadBuffer[hit_num+18-1];
	DIR_LstAccDate[1] =  SDreadBuffer[hit_num+19-1];	
	
	//ファイル書き込み時刻
	DIR_WrtTime[0] =  SDreadBuffer[hit_num+22-1];
	DIR_WrtTime[1] =  SDreadBuffer[hit_num+23-1];
	
	date1 = DIR_WrtTime[1];
	date2 = (date1<<8) | DIR_WrtTime[0];
	
	if('R' == file_name[8])
	{
		//時	
		date3 = ((0xF800 & date2) >> 11);	
		R_file_date[3] = (unsigned char)date3;
		
		//分
		date3 = ((0x07E0 & date2) >> 5);	
		R_file_date[4] = (unsigned char)date3;	
	}
	
	//ファイル書き込み日
	DIR_WrtDate[0] =  SDreadBuffer[hit_num+24-1];
	DIR_WrtDate[1] =  SDreadBuffer[hit_num+25-1];
	
	date1 = DIR_WrtDate[1];
	date2 = (date1<<8) | DIR_WrtDate[0];
	
	//年
	date3 = ((0xFE00 & date2) >> 9) + 1980;	
	date3 %= 100;	
	file_date[0] = (unsigned char)(date3/10) + '0';
	file_date[1] = (unsigned char)(date3%10) + '0';
	if('R' == file_name[8])
	{
		R_file_date[0] = (unsigned char)date3;
	}
	
	//月
	date3 = ((0x1E0 & date2) >> 5);	
	file_date[2] = (unsigned char)(date3/10) + '0';
	file_date[3] = (unsigned char)(date3%10) + '0';
	if('R' == file_name[8])
	{
		R_file_date[1] = (unsigned char)date3;
	}
	
	//日
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
//   -Name(en)   :   get_file_date_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_BPB：【変数】BPB_BytsPerSec
//【関数】SD_single_read：【変数】SDreadBuffer[]
/*void get_file_date_SD(void)
{
	unsigned long x1,x2,x3;
	unsigned int date1, date2, date3;
	unsigned char one, ten;
	
	//ファイル書き込み日
	DIR_WrtDate[0] =  SDreadBuffer[hit_num+24-1];
	DIR_WrtDate[1] =  SDreadBuffer[hit_num+25-1];
	
	date1 = DIR_WrtDate[1];
	date2 = (date1<<8) | DIR_WrtDate[0];
	
	//年
	date3 = ((0xFE00 & date2) >> 9) + 1980;	
	date3 %= 100;	
	file_date[0] = (unsigned char)(date3/10) + '0';
	file_date[1] = (unsigned char)(date3%10) + '0';
	
	//月
	date3 = ((0x1E0 & date2) >> 5);	
	file_date[2] = (unsigned char)(date3/10) + '0';
	file_date[3] = (unsigned char)(date3%10) + '0';
	
	//日
	date3 = (0x1F & date2);	
	file_date[4] = (unsigned char)(date3/10) + '0';
	file_date[5] = (unsigned char)(date3%10) + '0';	
}*/

// *******************************************************************************/
//   -Name(en)   :   search_file_name                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードファイル名検索>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_BPB：【変数】BPB_BytsPerSec
//【関数】SD_single_read：【変数】SDreadBuffer[]
int search_file_name(unsigned char *name, unsigned char file_type)
{
	unsigned int l;
	unsigned int search_byte_num, target_byte_num;
	int hit;
	unsigned char search_name;
	unsigned char start=0;
	
	hit_num = 0;		//検索済みbyteクリア
	if((file_type & 0x0F) == FILE_NAME)	//ファイル名検索
	{
		start = 0;	//ファイル名検索した場合は、11文字の一致を確認する。
	}
	else if((file_type & 0x0F) == EXP_NAME)	//拡張子検索
	{
		
		start = 8;	//拡張子検索した場合、最初の8文字はファイル名なので無視する。
	}
	
	//1文字目検索
	search_name = *(name+start);	//search_name=&name => name=name+1
	for(target_byte_num=hit_num; target_byte_num<BPB_BytsPerSec; target_byte_num++)
	{
		if(SDreadBuffer[target_byte_num] == search_name)
		{
			hit_num = target_byte_num+1;	//次回検索時は1文字目一致byteの次byteから開始させる。
			
			
			//2文字目以降一致確認
			for(search_byte_num=1; search_byte_num<11-start; search_byte_num++)	//File nameは8byte、拡張子は3byte
			{	
				//拡張子のみで検索している場合、削除済みファイルならば無視する
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
					hit = 0x01;	//文字一致フラグアサート
				}
				else
				{							
					hit = 0x00;	//文字一致フラグネゲート
					//for(l=search_byte_num; l > start; l--)
					for(l=search_byte_num; l > 0; l--)
					{
						search_name = *(--name+start);
					}
					break;	//文字一致しなければiのfor loopを抜けて、最初の文字から検索再開。
				}
			}
			
			if(hit == 0x01)
			{
				//拡張子のみで検索している場合、ファイル名を取得する
				if((file_type & 0x0F) == EXP_NAME)
				{
					//hit_numをFILE_NAMEと揃える
					hit_num = hit_num-start;
					
					for(search_byte_num=0; search_byte_num<8; search_byte_num++)
					{
						file_name[search_byte_num] = SDreadBuffer[hit_num-1+search_byte_num];
					}
				}
				
				//全文字一致したらtarget_byte_numのfor loopを抜けて検索終了。
				break;
			}			
		}			
	}		
	
	return hit;
}






// *******************************************************************************/
//   -Name(en)   :   search_empty                                     　　　　　　 　*/
//   -Name(jp)   :   <>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
//本関数を使用する前には、以下の関数で変数を取得しないといけない。
//【関数】analysis_SD_BPB：【変数】BPB_BytsPerSec
//【関数】SD_single_read：【変数】SDreadBuffer[]
int search_empty(unsigned int write_bytes)
{
	unsigned int search_byte_num, target_byte_num;
	int hit;
	
	hit_num = 0;
	//1文字目検索
	for(target_byte_num=0; target_byte_num<BPB_BytsPerSec; target_byte_num=target_byte_num+write_bytes)//検索はwrite_bytesのbyte周期で行う。
	{
		if(SDreadBuffer[target_byte_num] == 0x00)
		{
			//2文字目以降一致確認
			for(search_byte_num=1; search_byte_num<write_bytes; search_byte_num++)
			{
				if(SDreadBuffer[target_byte_num+search_byte_num] == 0x00)
				{
					hit = 0x01;	//文字一致フラグアサート
				}
				else
				{							
					hit = 0x00;	//文字一致フラグネゲート
					break;	//文字一致しなければiのfor loopを抜けて、最初の文字から検索再開。
				}
			}
			
			if(hit == 0x01)
			{
				//全文字一致したらjのfor loopを抜けて検索終了。
				break;
			}			
		}	
	}		
	
	hit_num = target_byte_num;
	
	return hit;
}



