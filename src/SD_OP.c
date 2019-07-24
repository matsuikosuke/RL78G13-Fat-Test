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
//   -Name(en)   :   search_empty_SD_FAT                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカード空きクラスタ検索>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
void search_empty_SD_FAT(unsigned long all_cluster_num)
{

	
	
}


// *******************************************************************************/
//   -Name(en)   :   write_SD_FAT                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードFAT書き込み>                                      */
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

	//(1) 必要クラスタ数の計算
	//以下の処理はanalysis_SD_DIRかsearch_dir_SDにおいてall_DATA_bytesを求めていることを前提する。
	all_cluster_num = (all_DATA_sectors + BPB_SecPerClus - 1) % BPB_SecPerClus;
	
}


// *******************************************************************************/
//   -Name(en)   :   move_file_SD                                     　　　　　　 　*/
//   -Name(jp)   :   <SDカードファイル移動>                                      */
//   -OutLine    :                       */
//   -Input      :   None                                                        */
//   -Output     :   None                                                        */
//   -Return     :   None                                                        */
// *******************************************************************************/
int move_file_SD(unsigned char *file_name, unsigned char *org_dir_name, unsigned char *target_dir_name, unsigned char file_type, unsigned char target)
{
	int ret_search_file;
	
	//(1) ファイルのルートディレクトリ情報取得
	//targetを*CH_NAMEにしていればファイル名も更新される
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
			//(2) フォルダ作成（フォルダがない場合のみ実行）
			if(ret_search_file == SEARCH_SD_NO_DIR)
			{
				
				break;
			}
		}
	}
	
	//ファイルがなければ終了
	if(SEARCH_SD_NO_FILE == ret_search_file)
	{
		return 	ret_search_file;
	}
	
	//(3) 空きルートディレクトリ領域上書き	
	ret_search_file = SEARCH_SD_NO_DIR;
	while(ret_search_file == SEARCH_SD_NO_DIR)
	{
		ret_search_file = copy_empty_SD_SUB_DIR(&target_dir_name[0], 32);
		
		//(4) フォルダ作成（フォルダがない場合のみ実行）
		if(ret_search_file == SEARCH_SD_NO_DIR)
		{
			
			break;			
		}
	}
	
}


// *******************************************************************************/
//   -Name(en)   :   get_SSCAN_date                                     　　　　　　 　*/
//   -Name(jp)   :   <SSCAN.BIN日付データ抽出>                                      */
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
			
	//SDカードファイル名検索
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
		//データ read
		DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
		
		//最終クラスタでは、全セクターを読み込む必要はない場合もある。最終クラスタで送信すべきセクタ数を計算する。
		//データ続きが無い場合（＝現在のクラスタが最終クラスタ）
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
		//データ続きがある場合は、1クラスタあたりのセクタ数を送信すればよい。
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			cluster_max = BPB_SecPerClus;	
		}
			
		for(now_sector=0; now_sector<cluster_max; now_sector++)
		{
			
			//データ領域読み込み
			ret = read_SD_DATA_area(now_sector);
					
			//Read Error check
			if(ret == SD_READ_ERROR)
			{
				return ret;	
			}
				
			//SSCAN.BIN日付データ抽出
			get_information(now_sector);	
			if(bin_trans_state == 0xEE)
			{
				break;
			}
		}

		//転送クラスター数カウント
		transfered_cluster_count += 1;
			
		//データ続きが無い場合
		if(ret_search_file == SEARCH_SD_SUCCESS || bin_trans_state == 0xEE)
		{	
			transfered_cluster_count = 0;
			bin_trans_state = 0x00;
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
			return TRANS_AGAIN;
		}
	}
	
	return TRANS_SUCCESS;
}


// *******************************************************************************/
//   -Name(en)   :   check_SSCANBIN                                     　　　　　　 　*/
//   -Name(jp)   :   <SSCAN.BIN日付データ有無確認>                                      */
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
	
	//SDカードファイル名検索
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
		//データ read
		DATA_N_start_sector = DATA_start_sector + (DATA_cluster_num - 2) * BPB_SecPerClus;
		
		//最終クラスタでは、全セクターを読み込む必要はない場合もある。最終クラスタで送信すべきセクタ数を計算する。
		//データ続きが無い場合（＝現在のクラスタが最終クラスタ）
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
		//データ続きがある場合は、1クラスタあたりのセクタ数を送信すればよい。
		else if(ret_search_file == SEARCH_SD_NEXT)
		{
			cluster_max = BPB_SecPerClus;	
		}
			
		for(now_sector=0; now_sector<cluster_max; now_sector++)
		{
			
			//データ領域読み込み
			ret = read_SD_DATA_area(now_sector);
					
			//Read Error check
			if(ret == SD_READ_ERROR)
			{
				return ret;	
			}
				
			//SSCAN.BIN日付データ抽出
			get_information(now_sector);	
			if(bin_trans_state == 0xEE)
			{
				break;
			}
		}

		//転送クラスター数カウント
		transfered_cluster_count += 1;
			
		//データ続きが無い場合
		if(ret_search_file == SEARCH_SD_SUCCESS || bin_trans_state == 0xEE)
		{	
			transfered_cluster_count = 0;
			bin_trans_state = 0x00;
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
			return TRANS_AGAIN;
		}
	}
	
	return TRANS_SUCCESS;
}
*/

/***********************************************************************************************************************
* Function Name: get_information
* Description  : HttpBodyファイル情報を送信する
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
					
					
					//計測開始日時取得
					a = ((unsigned int)temp_buf[0] << 8) | (unsigned int)temp_buf[1];
					b = ((0xFFC0 & a)>>6) | ((0x003F & a)<<10);					
					//年
					c = (0xFF00 & b) >> 8;
					compare_start_date[0] = (unsigned char)c;
					//月
					c = 0x00FF & b;
					compare_start_date[1] = (unsigned char)c;
					
					
					a = ((unsigned int)temp_buf[2] << 8) | (unsigned int)temp_buf[3];
					b = ((0xFFE0 & a)>>5) | ((0x001F & a)<<11);				
					//日
					c = (0xFF00 & b) >> 8;
					compare_start_date[2] = (unsigned char)c;				
					//時
					c = 0x00FF & b;
					compare_start_date[3] = (unsigned char)c;
					
					
					a = ((unsigned int)temp_buf[4] << 8) | (unsigned int)temp_buf[5];
					b = ((0xFFF0 & a)>>4) | ((0x000F & a)<<12);				
					//分
					c = (0xFF00 & b) >> 8;
					compare_start_date[4] = (unsigned char)c;				
					//秒
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
	
	//compare_start_dateデータが0x00の場合
	if(0x00 == compare_start_date[0] || 0x00 == compare_start_date[1] || 0x00 == compare_start_date[2])
	{	
		return COMPARE_END;		//SSCAN.BINの日付データはもう存在しないので検索終了	
	}
	
	//年の比較
	if(R_file_date[0] > compare_start_date[0])
	{
		return COMPARE_NEXT;		//RファイルがSSCAN.BINの日付データより未来なので次の領域を検索
	}
	else if(R_file_date[0] < compare_start_date[0])
	{
		return COMPARE_END;		//RファイルがSSCAN.BINの日付データより過去なので検索終了		
	}
	
	//月の比較
	if(R_file_date[1] > compare_start_date[1])
	{
		return COMPARE_NEXT;		//RファイルがSSCAN.BINの日付データより未来なので次の領域を検索
	}
	else if(R_file_date[1] < compare_start_date[1])
	{
		return COMPARE_END;		//RファイルがSSCAN.BINの日付データより過去なので検索終了		
	}
	
	//日の比較
	if(R_file_date[2] > compare_start_date[2])
	{
		return COMPARE_NEXT;		//RファイルがSSCAN.BINの日付データより未来なので次の領域を検索
	}
	else if(R_file_date[2] < compare_start_date[2])
	{
		return COMPARE_END;		//RファイルがSSCAN.BINの日付データより過去なので検索終了		
	}
	
	//時の比較
	if(R_file_date[3] > compare_start_date[3])
	{
		return COMPARE_NEXT;		//RファイルがSSCAN.BINの日付データより未来なので次の領域を検索
	}
	else if(R_file_date[3] < compare_start_date[3])
	{
		return COMPARE_END;		//RファイルがSSCAN.BINの日付データより過去なので検索終了		
	}
	
	//分の比較
	if(R_file_date[4] > compare_start_date[4])
	{
		return COMPARE_NEXT;		//RファイルがSSCAN.BINの日付データより未来なので次の領域を検索
	}
	else if(R_file_date[4] < compare_start_date[4])
	{
		return COMPARE_END;		//RファイルがSSCAN.BINの日付データより過去なので検索終了		
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
	
		
	//年の比較
	if(old_compare_start_date[0] > compare_start_date[0])
	{
		return COMPARE_LATEST_NOW;		//最新の日付データ変更の必要なし
	}
	else if(old_compare_start_date[0] < compare_start_date[0])
	{
		return COMPARE_LATEST_NEXT;		//最新の日付データを更新		
	}
	
	//月の比較
	if(old_compare_start_date[1] > compare_start_date[1])
	{
		return COMPARE_LATEST_NOW;		//最新の日付データ変更の必要なし
	}
	else if(old_compare_start_date[1] < compare_start_date[1])
	{
		return COMPARE_LATEST_NEXT;		//最新の日付データを更新		
	}
	
	//日の比較
	if(old_compare_start_date[2] > compare_start_date[2])
	{
		return COMPARE_LATEST_NOW;		//最新の日付データ変更の必要なし
	}
	else if(old_compare_start_date[2] < compare_start_date[2])
	{
		return COMPARE_LATEST_NEXT;		//最新の日付データを更新		
	}
	
	//時の比較
	if(old_compare_start_date[3] > compare_start_date[3])
	{
		return COMPARE_LATEST_NOW;		//最新の日付データ変更の必要なし
	}
	else if(old_compare_start_date[3] < compare_start_date[3])
	{
		return COMPARE_LATEST_NEXT;		//最新の日付データを更新		
	}
	
	//分の比較
	if(old_compare_start_date[4] > compare_start_date[4])
	{
		return COMPARE_LATEST_NOW;		//最新の日付データ変更の必要なし
	}
	else if(old_compare_start_date[4] < compare_start_date[4])
	{
		return COMPARE_LATEST_NEXT;		//最新の日付データを更新		
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
	
		
	//文字列に変換
	//年
	string_compare_start_date[0] = trans_hex((unsigned char)compare_start_date[0]/10);
	string_compare_start_date[1] = trans_hex((unsigned char)compare_start_date[0]%10);
	
	//月
	string_compare_start_date[2] = trans_hex((unsigned char)compare_start_date[1]/10);
	string_compare_start_date[3] = trans_hex((unsigned char)compare_start_date[1]%10);
	
	//日
	string_compare_start_date[4] = trans_hex((unsigned char)compare_start_date[2]/10);
	string_compare_start_date[5] = trans_hex((unsigned char)compare_start_date[2]%10);
	
	//時
	string_compare_start_date[6] = trans_hex((unsigned char)compare_start_date[3]/10);
	string_compare_start_date[7] = trans_hex((unsigned char)compare_start_date[3]%10);
	
	//分
	string_compare_start_date[8] = trans_hex((unsigned char)compare_start_date[4]/10);
	string_compare_start_date[9] = trans_hex((unsigned char)compare_start_date[4]%10);
	
	
	
	//月の比較
	if(R_file_name[0] != string_compare_start_date[2])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	if(R_file_name[1] != string_compare_start_date[3])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	//日の比較
	if(R_file_name[2] != string_compare_start_date[4])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	if(R_file_name[3] != string_compare_start_date[5])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	//時の比較
	if(R_file_name[4] != string_compare_start_date[6])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	if(R_file_name[5] != string_compare_start_date[7])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	//分の比較
	if(R_file_name[6] != string_compare_start_date[8])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	
	if(R_file_name[7] != string_compare_start_date[9])
	{
		return COMPARE_NEXT;		//一致しなければ次の領域を検索			
	}
	else
	{
		return COMPARE_END;		//全て一致すれば検索終了	
	}
}

