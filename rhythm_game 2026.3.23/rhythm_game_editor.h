#pragma once
/*
	譜面制作モード
					*/
#include<stdio.h>
#include<string.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include"rhythm_game_define_file.h"
#include"error_code_define.h"
#define PROCESS_CHOICE 1
#define NOTE_CHOICE 2
#define KYE_CHOICE 3
typedef struct {
	int tempo[ARRAY_GYO];			//BPM保存
	int tempo_old;						//画面外BPM表示用
	int measure_beat, standard_beat;	//拍子
	char (*note)[7];
}MUSIC_DATA;
void descent(int, int, MUSIC_DATA* data, FILE* fp);
void data_hyouji(int, int, int, int, MUSIC_DATA* data, char);
int gyo_warp(int,char,char name[80],MUSIC_DATA* editor_data);
void gyo_save(int, MUSIC_DATA* data, FILE* fp);
void editor(char note_gamen[ARRAY_GYO][7], FILE* fp, char editor_mode, char name[80])
{
	FILE* fp_work;
	int i, j;
	int editor_gyo, gamen_gyo_max=0, editor_tempo_gyo_max = 0, editor_note_gyo_max = 0;
	char kye, kye_number;
	int note_number;
	char gyo_flg = 0x0;
	MUSIC_DATA editor;
	editor.note = note_gamen;
	for (i = 0;i < ARRAY_GYO;i++)
	{
		for (j = 0;j < 6;j++)
			editor.note[i][j] = '0';
		editor.note[i][j] = '\0';
		editor.tempo[i] = 0;
	}
	editor.tempo_old = 0;
	if (editor_mode == OVERWRITE_MODE || editor_mode == NEW_MODE)
	{
		do
		{
			printf("拍子入力:");
			if (scanf("%d/%d", &editor.measure_beat, &editor.standard_beat) != 1)
				while (getchar() != '\n');
			printf("\x1b[H\x1b[K");
			
		} while (editor.measure_beat <= 0 && 16% editor.standard_beat!=0);
		
		do
		{
			printf("楽曲開始初期のBPM:");
			if (scanf("%d", &editor.tempo[1]) != 1)
				while (getchar() != '\n');
			printf("\x1b[H\x1b[K");
		} while (editor.tempo[1] <= 0);
		editor.tempo[0] = editor.tempo[1];
	}
	else
	{
		if ((fp_work = fopen("editor_sub.txt", "w")) == NULL)
		{
			printf("ファイルオープンエラーが発生しました。");exit(1);
		}
		while (fscanf(fp, "%d %d/%d %s", &editor.tempo[0], &editor.measure_beat, &editor.standard_beat, editor.note[0]) != EOF)
		{
			gamen_gyo_max++;
			descent(gamen_gyo_max, editor_tempo_gyo_max, &editor, fp_work);
			if (editor_tempo_gyo_max != ARRAY_GYO - 1) editor_tempo_gyo_max++;	//BPM参照開始カウント
			editor_note_gyo_max++;	//ノーツデータ編集行の上限増加
		}
		fclose(fp);
		fp = fp_work;
	}
	while (1)
	{
		printf("\x1b[H");
		editor_gyo = 0;
		data_hyouji(editor_note_gyo_max, editor_gyo, gamen_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
		do	//選択メニュー1
		{
			printf("キー指定:");
			if (scanf(" %c", &kye) != 1)
				while (getchar() != '\n');
			printf("\x1b[29H\x1b[K");
			kye_number = 0;
			switch (kye)
			{
			case 'l':kye_number++;
			case 'k':kye_number++;
			case 'j':kye_number++;
			case 'd':kye_number++;
			case 's':kye_number++;
			case 'a':break;
			case 'g':
				do	//行変更
				{
					printf("行指定:");
					if (scanf("%d", &editor_gyo) != 1)
						while (getchar() != '\n');
					printf("\x1b[29H\x1b[K");
					editor_gyo = gamen_gyo_max - (editor_gyo - 1);	//+:過去方向 0:画面の最大行 -:最新方向
				} while (editor_gyo < gamen_gyo_max - editor_note_gyo_max || editor_gyo > editor_note_gyo_max);
				if ((gyo_flg == 0x6 || gyo_flg == 0x4) && editor_gyo > ARRAY_GYO - 1)
				{
					gyo_flg = 0x5;
					if ((fp_work = fopen(name, "r")) == NULL)
					{
						printf("エラー\n"); exit(1);
					}
					for (i = 0;fscanf(fp_work, "%*d %*d/%*d %*s") != EOF && i < gamen_gyo_max;i++);	//コピー始める行までスキップ
					for (j = i;fscanf(fp_work, "%d %d/%d %s", &editor.tempo[0], &editor.measure_beat, &editor.standard_beat, editor.note[0]) != EOF;j++)	//EOF(データの最後まで)コピー
					{
						gamen_gyo_max++;
						descent(gamen_gyo_max, editor_tempo_gyo_max, &editor, fp);
						editor_tempo_gyo_max++;
					}
					gyo_save(gamen_gyo_max, &editor, fp);
					fclose(fp);
					fclose(fp_work);
					gamen_gyo_max = i - editor_gyo;
					remove(name);
					rename("editor_sub.txt", name);
					editor_gyo = 0;
					editor_tempo_gyo_max = gyo_warp(gamen_gyo_max, gyo_flg, name, &editor);	//二次元配列等を整備
					if ((fp = fopen("editor_sub.txt", "a")) == NULL)	//一時保存データ用ファイル
					{
						printf("エラー\n"); exit(1);
					}
				}
				else if ((gyo_flg >> 1) == 0x0 && editor_gyo > ARRAY_GYO - 1)	//画面表示が最高列の時かつ、画面外への移動時 - 過去への移動 -
				{
					gyo_flg = 0x4;
					gamen_gyo_max = editor_note_gyo_max - editor_gyo;
					editor_gyo = 0;
					gyo_save(editor_note_gyo_max, &editor, fp);	//あまりのデータ一括保存
					fclose(fp);
					editor_tempo_gyo_max = gyo_warp(gamen_gyo_max, gyo_flg, name, &editor);	//二次元配列等を整備
					if ((fp = fopen("editor_sub.txt", "a")) == NULL)	//一時保存データ用ファイル
					{
						printf("エラー\n"); exit(1);
					}
				}
				else if ((gyo_flg & 0x4) == 0x4 && editor_gyo < 0)	//画面外への移動時 - 最新への移動 -
				{
					if ((fp_work = fopen(name, "r")) == NULL)
					{
						printf("エラー\n"); exit(1);
					}
					for (i = 0;fscanf(fp_work, "%*d %*d/%*d %*s") != EOF && i < gamen_gyo_max;i++);	//コピー始める行までスキップ
					if (gyo_flg == 0x5)
						fscanf(fp_work, "%*d %*d/%*d %*s");
					if (editor_note_gyo_max - (gamen_gyo_max - editor_gyo) == 0)		//最高列への移動(ifの条件:全体のデータ行数-(画面の最大行-移動する行数))
					{
						gyo_flg = 0x1;
					}
					else	//最後列以外への移動
					{
						gyo_flg = 0x6;
					}
					for (j=i;fscanf(fp_work, "%d %d/%d %s", &editor.tempo[0], &editor.measure_beat, &editor.standard_beat, editor.note[0]) != EOF;j++)	//EOF(データの最後まで)コピー
					{
						gamen_gyo_max++;
						descent(gamen_gyo_max, editor_tempo_gyo_max, &editor, fp);
						editor_tempo_gyo_max++;
					}
					gyo_save(gamen_gyo_max, &editor, fp);
					fclose(fp);
					fclose(fp_work);

					if (gyo_flg == 0x1)
						gamen_gyo_max = editor_note_gyo_max;
					else
						gamen_gyo_max = i - editor_gyo;
					editor_gyo = 0;
					remove(name);
					rename("editor_sub.txt", name);
					editor_tempo_gyo_max = gyo_warp(gamen_gyo_max, gyo_flg, name,&editor);	//表示用データ準備
					if (gyo_flg == 0x1)
					{
						remove(name);
						rename("editor_sub.txt", name);
						if ((fp = fopen(name, "a")) == NULL)	//これから使用するファイルオープン
						{
							printf("エラー\n"); exit(1);
						}
					}
					else
					{
						if ((fp = fopen("editor_sub.txt", "a")) == NULL)
						{
							printf("エラー\n"); exit(1);
						}
					}
				}
				data_hyouji(editor_note_gyo_max, editor_gyo, gamen_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
				break;
			case 't':
				if (editor_gyo<ARRAY_GYO-3)	//テンポ(ノーツ速度)変更
				{
					do
					{
						printf("ノーツ速度(BPM):");
						if (scanf("%d", &editor.tempo[editor_gyo]) != 1)
							while (getchar() != '\n');
						printf("\x1b[29H\x1b[K");
					} while (editor.tempo[editor_gyo] < 0);
					data_hyouji(editor_note_gyo_max, editor_gyo, gamen_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
				}
				break;
			case 'q':
				printf("\x1b[2J終了します。\n");
				Sleep(2000);
			}
		} while (kye != 'q' && kye != 'a' && kye != 's' && kye != 'd' && kye != 'j' && kye != 'k' && kye != 'l');
		if (kye == 'q') break;
		while (1)	//選択メニュー2
		{
			data_hyouji(editor_note_gyo_max, editor_gyo, gamen_gyo_max, editor_tempo_gyo_max, &editor, NOTE_CHOICE);
			do
			{
				printf("ノーツ選択:");
				if (scanf("%d", &note_number) != 1)
					while (getchar() != '\n');
				printf("\x1b[29H\x1b[K");
				switch (note_number)
				{
				case 0:editor.note[editor_gyo][kye_number] = NOT_NOTE; break;
				case 1:editor.note[editor_gyo][kye_number] = NORMAL_NOTE; break;
				case 2:editor.note[editor_gyo][kye_number] = NORMAL_NOTE_LINK; break;
				case 3:editor.note[editor_gyo][kye_number] = LONG_NOTE; break;
				case 4:editor.note[editor_gyo][kye_number] = LONG_NOTE_LINK; break;
				case 5:editor.note[editor_gyo][kye_number] = LONG_START_NOTE; break;
				case 6:editor.note[editor_gyo][kye_number] = LONG_START_NOTE_LINK; break;
				case 7:editor.note[editor_gyo][kye_number] = LONG_END_NOTE; break;
				case 8:editor.note[editor_gyo][kye_number] = LONG_END_NOTE_LINK; break;
				case 9:editor.note[editor_gyo][kye_number] = TRACE_NOTE; break;
				case 10:editor.note[editor_gyo][kye_number] = TRACE_NOTE_LINK; break;
				}
			} while ((note_number < 0 || note_number > 10) && note_number != 99);
			if (note_number == 99) break;
			data_hyouji(editor_note_gyo_max, editor_gyo, gamen_gyo_max, editor_tempo_gyo_max, &editor, KYE_CHOICE);
			do
			{
				printf("キー指定:");
				if (scanf(" %c", &kye) != 1)
					while (getchar() != '\n');
				printf("\x1b[29H\x1b[K");
				kye_number = 0;
				switch (kye)
				{
				case 'l':kye_number++;
				case 'k':kye_number++;
				case 'j':kye_number++;
				case 'd':kye_number++;
				case 's':kye_number++;
				case 'a':break;
				}
			} while (kye != 'q' && kye != 'a' && kye != 's' && kye != 'd' && kye != 'j' && kye != 'k' && kye != 'l');
			if (kye == 'q') break;
		}
		if (editor_gyo == 0)	//行降下処理兼データ移動
		{	
			gamen_gyo_max++;
			descent(gamen_gyo_max, editor_tempo_gyo_max, &editor, fp);
			if (editor_tempo_gyo_max!=ARRAY_GYO-1) editor_tempo_gyo_max++;	//BPM参照開始カウント
			if((gyo_flg>>1)==0x0) editor_note_gyo_max++;	//ノーツデータ編集行の上限増加
			if ((gyo_flg & 0x4) == 0x4&&editor_note_gyo_max==gamen_gyo_max)	//過去に戻っていた状態から最新の行まで来た時
			{
				gyo_flg = 0x0;	//データ移動初期(なし)状態
				fclose(fp);
				remove(name);
				rename("editor_sub.txt", name);
				if ((fp = fopen(name, "a")) == NULL)	//これから使用するファイルオープン
				{
					printf("エラー\n"); exit(1);
				}
			}
		}
	}
	//↓編集終了時
	if (editor_note_gyo_max!=gamen_gyo_max)
	{
		if ((fp_work = fopen(name, "r")) == NULL)
		{
			printf("エラー\n"); exit(1);
		}
		for (i = 0;fscanf(fp_work, "%*d %*d/%*d %*s") != EOF && i < gamen_gyo_max;i++);
		if (gyo_flg == 0x5)
			fscanf(fp_work, "%*d %*d/%*d %*s");
		while (fscanf(fp_work, "%d %d/%d %s", &editor.tempo[0], &editor.measure_beat, &editor.standard_beat, &editor.note[0]) != EOF)
		{
			gamen_gyo_max++;
			descent(gamen_gyo_max, editor_tempo_gyo_max, &editor, fp);
			if (editor_tempo_gyo_max != ARRAY_GYO-1) editor_tempo_gyo_max++;	//BPM参照開始カウント
		}
		gyo_save(editor_note_gyo_max, &editor, fp);
		fclose(fp);
		fclose(fp_work);
		remove(name);
		rename("editor_sub.txt", name);
	}
	else
	{
		gyo_save(editor_note_gyo_max, &editor, fp);	//あまりのデータ一括保存
		fclose(fp);
	}
}
void data_hyouji(int data_max, int gyo, int gyo_max, int tempo_gyo_max, MUSIC_DATA* data, char id)	//画面表示
{
	int i, j;
	int bpm_valie;
	int measure;
	measure = data->measure_beat;
	printf("\x1b[H");
	for (i = 0;i < ARRAY_GYO;i++)
	{
		if (data->tempo[i] != 0)
		{
			bpm_valie = data->tempo[i];
			break;
		}
	}
	if (i == ARRAY_GYO)
		bpm_valie = data->tempo_old;
	printf("\n          (BPM)  [現在のBPM :%3d]", bpm_valie);
	for (i = 0;i < ARRAY_GYO;i++)
	{
		if (i == ARRAY_GYO - 1 && data->tempo_old != 0) printf("\n      ↓%3d ", data->tempo_old);	//見えないつながってるBPM表示
		else printf("\n           ");
		if (data->tempo[i] != 0) printf("%3d │", data->tempo[i]);	//BPM変更時
		else printf(" │  │");	//BPM変更ないとき
		if (i == gyo) printf(COLLAR_KYE"□"COLLAR_RESET);	//指定行表示
		else		printf(" ");
		for (j = 0;j < 6;j++)
		{
			if (data->note[i][j] == NORMAL_NOTE)				printf(COLLAR_NORMAL_NOTE"■■"COLLAR_RESET);
			else if (data->note[i][j] == LONG_NOTE)			printf(COLLAR_LONG_NOTE"○○"COLLAR_RESET);
			else if (data->note[i][j] == LONG_START_NOTE)		printf(COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET);
			else if (data->note[i][j] == LONG_END_NOTE)		printf(COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET);
			else if (data->note[i][j] == TRACE_NOTE)			printf(COLLAR_TRACE_NOTE"◇◇"COLLAR_RESET);
			else if (data->note[i][j] == NORMAL_NOTE_LINK)	printf(COLLAR_NORMAL_NOTE"■■■"COLLAR_RESET);
			else if (data->note[i][j] == LONG_NOTE_LINK)		printf(COLLAR_LONG_NOTE"○○○"COLLAR_RESET);
			else if (data->note[i][j] == LONG_START_NOTE_LINK)printf(COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET);
			else if (data->note[i][j] == LONG_END_NOTE_LINK)	printf(COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET);
			else if (data->note[i][j] == TRACE_NOTE_LINK)		printf(COLLAR_TRACE_NOTE"◇◇◇"COLLAR_RESET);
			else if (i == gyo)		printf(COLLAR_KYE"□□"COLLAR_RESET);
			else					printf("  ");
			if (data->note[i][j] != NORMAL_NOTE_LINK && data->note[i][j] != LONG_NOTE_LINK && data->note[i][j] != TRACE_NOTE_LINK && data->note[i][j] != LONG_START_NOTE_LINK && data->note[i][j] != LONG_END_NOTE_LINK)
			{
				if (i == gyo)	printf(COLLAR_KYE"□"COLLAR_RESET);
				else			printf(" ");
			}
		}
		if (i == gyo) printf(COLLAR_RESET);
		printf("│");
		if ((gyo_max - i) % ((16/data->standard_beat)*data->measure_beat) == 0) printf(COLLAR_MEASURE);
		else if (gyo_max - i + 1 == 1 || (gyo_max - i) % (16 / data->standard_beat) == 0) printf(COLLAR_BEAT);
		else printf(COLLAR_GYO);
		if (gyo_max - i + 1 > 0) printf(" %3d", gyo_max - i + 1);	//行数表示
		else printf("    ");
		printf(COLLAR_RESET);
		if (i == 0) printf("  ┌ 総行数 ┐");
		else if (i == 1) printf("  │  %4d  │",data_max+1);
		else if (i == 2) printf("  └────────┘");
	}
	printf("\n                  A  S  D  J  K  L");
	switch (id)
	{
	case PROCESS_CHOICE:	//行程1
		printf("\x1b[21H\x1b[K");
		printf("             ┏━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("             ┃ キー選択              ┃\n             ┃  a s d j k l          ┃\n");
		printf("             ┃ その他                ┃\n             ┃  g:行変更             ┃\n");
		if (gyo< ARRAY_GYO - 3) printf("             ┃  t:ノーツ速度変更     ┃\n");
		else printf("             ┃                       ┃\n");
		printf("             ┃  q:譜面制作終了       ┃\n");
		printf("             ┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");break;
	case NOTE_CHOICE:	//行程2(ノーツデータ番号)
		printf("\x1b[21H\x1b[K");
		printf("             ┏━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("             ┃ 0:ノーツ消去  99:終了 ┃\n             ┃ 1:");
		printf(COLLAR_NORMAL_NOTE"■■"COLLAR_RESET); printf("   2:");
		printf(COLLAR_NORMAL_NOTE"■■■"COLLAR_RESET); printf("          ┃\n             ┃ 3:");
		printf(COLLAR_LONG_NOTE"○○"COLLAR_RESET); printf("   4:");
		printf(COLLAR_LONG_NOTE"○○○"COLLAR_RESET); printf("          ┃\n             ┃ 5:");
		printf(COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET); printf("   6:");
		printf(COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET); printf(" (start)  ┃\n             ┃ 7:");
		printf(COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET); printf("   8:");
		printf(COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET); printf(" (end)    ┃\n             ┃ 9:");
		printf(COLLAR_TRACE_NOTE"◇◇"COLLAR_RESET); printf("  10:");
		printf(COLLAR_TRACE_NOTE"◇◇◇"COLLAR_RESET); printf("          ┃\n");
		printf("             ┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");break;
	case KYE_CHOICE:	//行程2(キー指定)
		printf("\x1b[21H\x1b[K");
		printf("             ┏━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("             ┃ キー選択              ┃\n             ┃  a s d j k l          ┃\n");
		printf("             ┃                       ┃\n             ┃ その他                ┃\n");
		printf("             ┃  q:現在の行の編集終了 ┃\n             ┃                       ┃\n");
		printf("             ┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");break;
	}
}
void descent(int gyo_max, int tempo_gyo_max, MUSIC_DATA* data, FILE* fp)	//ノーツデータの移動
{
	int i, j;
	for (i = gyo_max;i >= 0;i--)
	{
		if (i < ARRAY_GYO - 1)	//ノーツデータの降下
		{
			for (j = 0;j < 6;j++)
			{
				data->note[i + 1][j] = data->note[i][j];
				data->note[i][j] = NOT_NOTE;
			}
		}
	}
	if (tempo_gyo_max != ARRAY_GYO-1) tempo_gyo_max++;
	for (i = tempo_gyo_max;i >= 0;i--)	//BPMデータの降下
	{
		if (i < ARRAY_GYO - 1)
		{
			data->tempo[i + 1] = data->tempo[i];
			data->tempo[i] = 0;
		}
	}
	if (data->tempo[ARRAY_GYO-1] != 0) data->tempo_old = data->tempo[ARRAY_GYO-1];
	if (gyo_max >= ARRAY_GYO-2)	//データ保存
	{
		fprintf(fp, "%d %d/%d %s\n", data->tempo[ARRAY_GYO - 1], data->measure_beat, data->standard_beat, data->note[ARRAY_GYO - 1]);
	}
	printf("\x1b[2J");
	for (j = 0;j < ARRAY_GYO;j++)
		printf("%d %d/%d %s\n", data->tempo[j], data->measure_beat, data->standard_beat, data->note[j]);
	Sleep(300);
}
int gyo_warp(int gyo,char flg,char name[80],MUSIC_DATA* editor_data)	//画面外行への行移動
{ 
	FILE* fp, * fp_work;
	int i,j;
	int max=0,tempo_max=0;
	if (flg == 0x1 || flg == 0x6 || flg == 0x5)
		max--;
	printf("%d\n", gyo);
	Sleep(2000);
	for (i = 0;i < ARRAY_GYO;i++)	//初期化
	{
		for (j = 0;j < 6;j++)
			editor_data->note[i][j] = '0';
		editor_data->note[i][j] = '\0';
		editor_data->tempo[i] = 0;
	}
	editor_data->tempo_old = 0;
	if ((fp_work = fopen(name, "r")) == NULL || (fp = fopen("editor_sub.txt", "w")) == NULL)
	{
		printf("エラー\n");exit(1);
	}
	if (flg == 0x5)
		fscanf(fp_work, "%*d %*d/%*d %*s");
	for (i = 0;fscanf(fp_work, "%d %d/%d %s",&editor_data->tempo[0], & editor_data->measure_beat, &editor_data->standard_beat, editor_data->note[0]) != EOF && i < gyo+1;i++)	//BPM以外のデータをコピー
	{
		max++;
		descent(max, tempo_max, editor_data, fp);
		if (tempo_max != ARRAY_GYO-1) tempo_max++;
	}
	printf("\x1b[2J");
	fclose(fp);
	fclose(fp_work);
	return tempo_max;
}
void gyo_save(int gyo_max,MUSIC_DATA* data,FILE* fp)
{
	int i;
	if (gyo_max >= ARRAY_GYO) gyo_max = ARRAY_GYO;
	while (1)	//データがなくなるまで保存
	{
		for (i = 0;i < ARRAY_GYO;i++)
		{
			if (strcmp(data->note[i], "000000") != 0 && data->tempo[i] == 0) break;
		}
		if (i == ARRAY_GYO) break;
		gyo_max++;
		descent(gyo_max, ARRAY_GYO-1,data,fp);
	}
}