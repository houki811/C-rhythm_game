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
	int tempo[ARRAY_GYO];
	int tempo_now, tempo_old;
	int tempo_history[ARRAY_GYO - 2], tempo_history_old;
	int measure_beat, standard_beat;
	char (*note)[7];
}MUSIC_DATA;
void descent(int, int, MUSIC_DATA* data, FILE* fp);
void data_hyouji(int, int, int, MUSIC_DATA* data, char);
void editor(char note_gamen[ARRAY_GYO][7], FILE* fp, char editor_mode, char name[80])
{
	FILE* fp_work;
	int i, j;
	int editor_gyo, editor_tempo_gyo_max = ARRAY_GYO - 3, editor_note_gyo_max = 0;
	char kye, kye_number;
	int note_number;
	MUSIC_DATA editor;
	editor.note = note_gamen;
	for (i = 0;i < ARRAY_GYO;i++)
	{
		for (j = 0;j < 6;j++)
			editor.note[i][j] = '0';
		editor.tempo[i] = 0;
		editor.note[i][j] = '\0';
	}
	editor.tempo_old = 0;
	if (editor_mode == OVERWRITE_MODE || editor_mode == NEW_MODE)
	{
		printf("拍子入力:");
		do
		{
			if (scanf("%d/%d", &editor.measure_beat, &editor.standard_beat) != 1)
				while (getchar() != '\n');
			printf("\x1b[H\x1b[K");
		} while (editor.measure_beat <= 0 && 16% editor.standard_beat!=0);
		printf("楽曲開始初期のBPM:");
		do
		{
			if (scanf("%d", &editor.tempo[ARRAY_GYO - 3]) != 1)
				while (getchar() != '\n');
			printf("\x1b[H\x1b[K");
		} while (editor.tempo[ARRAY_GYO - 3] <= 0);
		editor.tempo_now = editor.tempo[ARRAY_GYO - 3];
		fprintf(fp, "%d %d/%d %s\n", editor.tempo[ARRAY_GYO - 3], editor.measure_beat, editor.standard_beat, editor.note[ARRAY_GYO - 3]);
	}
	else
	{
		if ((fp_work = fopen("rhythm_sub.txt", "w")) == NULL)
		{
			printf("ファイルオープンエラーが発生しました。");exit(1);
		}
		while (fscanf(fp, "%d %d/%d %s", &editor.tempo[0], &editor.measure_beat, &editor.standard_beat, editor.note[0]) != EOF)
		{
			if (editor.tempo[0] != 0) editor.tempo_now = editor.tempo[0];
			descent(ARRAY_GYO, ARRAY_GYO - 1, &editor, fp_work);
			editor_note_gyo_max++;
			if (editor_tempo_gyo_max != ARRAY_GYO - 1) editor_tempo_gyo_max++;
		}
		fclose(fp);
		fp = fp_work;
	}
	while (1)
	{
		printf("\x1b[H");
		editor_gyo = 0;
		data_hyouji(editor_gyo, editor_note_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
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
					editor_gyo = editor_note_gyo_max - (editor_gyo - 1);
				} while (editor_gyo > editor_note_gyo_max || editor_gyo < 0);
				data_hyouji(editor_gyo, editor_note_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
				break;
			case 't':
				if (editor_tempo_gyo_max >= ARRAY_GYO - 2 && editor_gyo <= 2)	//テンポ(ノーツ速度)変更
				{
					do
					{
						printf("ノーツ速度(BPM):");
						if (scanf("%d", &editor.tempo[editor_gyo + 13]) != 1)
							while (getchar() != '\n');
						printf("\x1b[29H\x1b[K");
					} while (editor.tempo[editor_gyo + 13] < 0);
					if (editor_gyo <= 2 && editor.tempo[editor_gyo + 13] != 0)
						editor.tempo_now = editor.tempo[editor_gyo + 13];
					data_hyouji(editor_gyo, editor_note_gyo_max, editor_tempo_gyo_max, &editor, PROCESS_CHOICE);
				}
				break;
			case 'q':
				printf("終了します。\n");
			}
		} while (kye != 'q' && kye != 'a' && kye != 's' && kye != 'd' && kye != 'j' && kye != 'k' && kye != 'l');
		if (kye == 'q') break;
		while (1)	//選択メニュー2
		{
			data_hyouji(editor_gyo, editor_note_gyo_max, editor_tempo_gyo_max, &editor, NOTE_CHOICE);
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
			data_hyouji(editor_gyo, editor_note_gyo_max, editor_tempo_gyo_max, &editor, KYE_CHOICE);
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
			descent(editor_tempo_gyo_max, editor_tempo_gyo_max, &editor, fp);
			editor_note_gyo_max++;	//ノーツデータ編集行の上限増加
			if (editor_tempo_gyo_max != ARRAY_GYO - 1) editor_tempo_gyo_max++;	//BPMデータ編集行の上限増加
		}
	}
	for (i = editor_tempo_gyo_max - 1;i > 0;i--)	//ループでファイル移動できなかったデータ移動
		fprintf(fp, "%d %d/%d %s\n", editor.tempo[i], editor.measure_beat, editor.standard_beat, editor.note[i]);
	fclose(fp);
}
void data_hyouji(int gyo, int gyo_max, int tempo_gyo_max, MUSIC_DATA* data, char id)
{
	int i, j;
	int bpm_valie;
	int measure;
	measure = data->measure_beat;
	printf("\x1b[H");
	if (gyo == 0) bpm_valie = data->tempo_now;
	else
	{
		bpm_valie = 0;
		if (gyo < 3)	//BPM履歴データを参照する必要がないとき
		{
			for (i = ARRAY_GYO - 1;i >= gyo + 13;i--)
				if (data->tempo[i] != 0) bpm_valie = data->tempo[i];
		}
		if (gyo >= 3 || bpm_valie == 0)	//BPMデータを参照する必要がある時、または必要がない行でもBPMが特定できなかったとき
		{
			if (gyo == ARRAY_GYO - 1)	//指定行が配列の最大行のとき
				bpm_valie = data->tempo_history[ARRAY_GYO - 3];
			for (i = ARRAY_GYO - 3;i > gyo - 3;i--)
				if (data->tempo_history[i] != 0) bpm_valie = data->tempo_history[i];
			if (bpm_valie == 0) bpm_valie = data->tempo_history_old;	//ここまででBPMが特定できなかったとき
		}
	}
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
		if (gyo_max - i + 1 > 0) printf(" %2d", gyo_max - i + 1);	//行数表示
		printf(COLLAR_RESET);
	}
	printf("\n                  A  S  D  J  K  L");
	switch (id)
	{
	case PROCESS_CHOICE:	//行程1
		printf("\x1b[21H\x1b[K");
		printf("             ┏━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("             ┃ キー選択              ┃\n             ┃  a s d j k l          ┃\n");
		printf("             ┃ その他                ┃\n             ┃  g:行変更             ┃\n");
		if (tempo_gyo_max >= ARRAY_GYO - 2 && gyo <= 2) printf("             ┃  t:ノーツ速度変更     ┃\n");
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
void descent(int gyo_max, int tempo_gyo_max, MUSIC_DATA* data, FILE* fp)
{
	int i, j;
	if (data->tempo[ARRAY_GYO - 1] != 0) data->tempo_old = data->tempo[ARRAY_GYO - 1];	//左下見えないところのBPMデータを保存
	if (data->tempo_history[ARRAY_GYO - 4] != 0)data->tempo_history_old = data->tempo_history[ARRAY_GYO - 4];	//一番過去のBPMデータ保存
	for (i = gyo_max;i >= 0;i--)
	{
		if (i < ARRAY_GYO - 1)	//ノーツデータ、BPMデータの降下
		{
			for (j = 0;j < 6;j++)
			{
				data->note[i + 1][j] = data->note[i][j];
				data->note[i][j] = NOT_NOTE;
			}
			data->tempo[i + 1] = data->tempo[i];
			data->tempo[i] = 0;
		}
		if (i < ARRAY_GYO - 3)	//過去のBPMデータの降下
		{
			data->tempo_history[i + 1] = data->tempo_history[i];
			data->tempo_history[i] = 0;
		}
	}
	if (tempo_gyo_max == ARRAY_GYO - 1) fprintf(fp, "%d %d/%d %s\n", data->tempo[ARRAY_GYO - 1], data->measure_beat, data->standard_beat, data->note[ARRAY_GYO - 1]);
	data->tempo_history[0] = data->tempo[ARRAY_GYO - 1];	//BPMデータの最大行を履歴配列に追加
}