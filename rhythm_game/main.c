/*
	リズムゲームメイン
						*/
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include"rhythm_game_editor.h"
#include"rhythm_game_define_file.h"
#include"error_code_define.h"
typedef struct {
	int bestcombo;
	int seiseki;
	struct {
		int perfect, great, miss;
	}seido;
}SCORE;
SCORE score;
typedef struct {
	int fps_valie, note_speed;
}SYSTEM;
char note[ARRAY_GYO][7];	//表示エリア配列
int combo;			//現在コンボ
FILE* fp, * fp_result;
LARGE_INTEGER cpu_freq;
void miss_note(int, int, int);
void gamen(SYSTEM* system_valie);
int note_hantei(int, char, char);
int solo_note(int, char, int, int);
void result(char play_name[80]);
void main()
{
	FILE* fp_work;
	SYSTEM system_data;
	int i, j;
	char k;
	char name[80] = { 0 }, work_name[80] = { 0 };
	SCORE work_score;
	QueryPerformanceFrequency(&cpu_freq);
	if ((fp = fopen("system_data.txt", "r")) == NULL)
	{
		printf("システムエラー"ERROR_SYSTEMFILE);exit(1);
	}
	if (fscanf(fp, "%d %d", &system_data.fps_valie, &system_data.note_speed) != 2)
	{
		printf("読み込みエラーが発生しました。"ERROR_SYSTEMFILE_LOAD);
		fclose(fp);exit(1);
	}
	fclose(fp);
	timeBeginPeriod(1);
	while (1)
	{
		k = '0';
		printf(GAMEN_CLEAR"\n\n1.play\n2.設定\n3.創作\n4.終了\n");
		while (k < '1' || k > '4')
			k = _getch();
		if (k == END_KYE) break;	//終了
		for (i = 0;i < ARRAY_GYO;i++)
			for (j = 0;j < 6;j++)
				note[i][j] = NOT_NOTE;
		if (k != SYSTEM_KYE)
		{
			printf(GAMEN_CLEAR);
			if ((fp = fopen("gakkyoku.txt", "r")) == NULL || (fp_result = fopen("rhythm_game_result.txt", "r")) == NULL)
			{
				printf("楽曲名ファイルエラー"ERROR_GAKKYOKUFILE_1); exit(1);
			}
			printf("楽曲選択\n\n");
			for (i = 1;fscanf(fp, "%s", name) != EOF;i++)
			{
				printf("%d.%s\n", i, name);
				if (k == PLARY_KYE)
				{
					while (fscanf(fp_result, "%s %d %d", work_name, &work_score.seiseki, &work_score.bestcombo) != EOF)
					{
						if (strcmp(name, work_name) == 0)
						{
							printf("\n      -{BEST}-     -BESTCOMBO-\n");
							switch (work_score.seiseki)
							{
							case 1:printf(COLLAR_COMBO_FC"     FULL COMBO!        %d\n\n"COLLAR_RESET, work_score.bestcombo); break;
							case 2:printf(COLLAR_COMBO_AP"    ALL PERFECT!!        %d\n\n"COLLAR_RESET, work_score.bestcombo); break;
							default:printf("         ──              %d\n\n", work_score.bestcombo);
							}
							break;
						}
					}
				}
			}
			if (k == EDITOR_KYE) { printf("\n%d.新規作成\n", i);i++; }
			for (i--, j = 0, printf("\n");i<j || 1>j;)
			{
				printf("番号入力_");
				if (scanf("%d", &j) != 1)
					while (getchar() != '\n');
				printf("\x1b[1A\x1b[K");
			}
			fclose(fp);
			fclose(fp_result);
			if (k == EDITOR_KYE && i != j || k == PLARY_KYE)
			{
				if ((fp = fopen("gakkyoku.txt", "r")) == NULL)
				{
					printf("楽曲名ファイルエラー"ERROR_GAKKYOKUFILE_2); exit(1);
				}
				for (i = j, j = 1;j <= i;j++)
					if (fscanf(fp, "%s", name) != 1)
					{
						printf("読み込みエラーが発生しました。"ERROR_GAKKYOKUFILE_2_LOAD); exit(1);
					}
				strcat(name, ".txt");
			}
		}
		if (k == PLARY_KYE) //メインゲーム
		{
			if ((fp = fopen(name, "r")) == NULL)
			{
				printf("エラーが発生しました"ERROR_RHYTHM_NAME);exit(1);
			}
			system("cls");
			Sleep(700);
			printf("\x1b[?25l");
			gamen(&system_data);//game system main
			fclose(fp);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			result(name);	//リザルト
			printf("\n\n\n                     次へ.(a)");
			while (_getch() != 'a');
		}
		else if (k == SYSTEM_KYE)	//設定
		{
			while (1)
			{
				k = 0;
				printf("\x1b[H\x1b[2J設定一覧\n 1.FPS\n 2.ノーツスピード\n 3.終了\n");
				while (k < FPS_EDITOR || k>SYSTEM_END)
					k = _getch();
				printf(GAMEN_CLEAR);
				if (k == FPS_EDITOR)
				{
					printf("FPS設定\n  1.60fps\n  2.120fps\n");
					do
					{
						printf("選択:");
						if (scanf("%c", &k) != 1)
							while (getchar() != '\n');
						printf("\x1b[4H\x1b[K");

					} while (k != '1' && k != '2');
					system_data.fps_valie = 60 * (k - MAGICNUMBER_0);
				}
				else if (k == NOTE_SPEED_EDITOR)
				{
					printf("ノーツスピード設定\n");
					while (1)
					{
						printf("0.お試し\n\n1   最遅\n|\n5   中間\n|\n9   最速\n");
						do
						{
							printf("選択(現在:%d):", system_data.note_speed);
							if (scanf("%c", &k) != 1)
								while (getchar() != '\n');
							printf("\x1b[9H\x1b[k");
						} while (k < NOTE_SPEED_TEST || k>'9');
						while (getchar() != '\n');
						if (k == NOTE_SPEED_TEST)
						{
							printf(GAMEN_CLEAR);
							if ((fp = fopen("note_speed_test.txt", "r")) == NULL)
							{
								printf("譜面呼び出しエラー"ERROR_NOTE_SPEED_TESTFILE); Sleep(1000);
								break;
							}
							else
							{
								printf("\x1b[?25l");
								gamen(&system_data);
								printf("\x1b[?25h");
							}
							FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
							printf(GAMEN_CLEAR);
						}
						else
						{
							system_data.note_speed = k - MAGICNUMBER_0;
							printf("ノーツ速度を変更しました。\n\x1b[H\x1b[2J");
							break;
						}
					}
				}
				else if (k == SYSTEM_END) break;
			}
			if ((fp = fopen("system_data.txt", "w")) != NULL)
			{
				fprintf(fp, "%d %d", system_data.fps_valie, system_data.note_speed);
				fclose(fp);
			}
		}
		else if (k == EDITOR_KYE)	//創作モード
		{
			if (i == j)
			{
				while (1)
				{
					if ((fp_work = fopen("gakkyoku.txt", "a")) == NULL)
					{
						printf("楽曲名ファイルエラー"ERROR_GAKKYOKU_EDITOR); exit(1);
					}
					printf("楽曲ファイル名入力(.txt入力不可):"); 
					do
					{
						while (scanf("%s", name) != 1)
							while (getchar() != '\n');
						printf("\x1b[11;34H\x1b[K");
						strcat(name, ".txt");
					} while ((fp = fopen(name, "w")) == NULL);
					name[strlen(name) - 4] = '\0';
					fprintf(fp_work, "\n%s", name);
					fclose(fp_work);
					break;
				}
				k = NEW_MODE;
				if ((fp_work = fopen("rhythm_game_result.txt", "a")) == NULL)
				{
					printf("読み込みエラー"ERROR_RESULT_EDITOR); exit(1);
				}
				fprintf(fp_work, "\n%s 0 0", name);
				fclose(fp_work);
				strcat(name, ".txt");
			}
			else
			{
				printf(GAMEN_CLEAR"編集方法\n 1.上書き\n 2.続きから\n");
				do
				{
					printf("選択:");
					if (scanf("%c", &k) != 1)
						while (getchar() != '\n');
					printf("\x1b[4H\x1b[K");
				} while (k != OVERWRITE_MODE && k != CONTINUATION_MODE);
				if (k == OVERWRITE_MODE || k == NEW_MODE)
				{
					if ((fp = fopen(name, "w")) == NULL)
					{
						printf("ファイルオープンエラー"ERROR_EDITOR_NAME);exit(1);
					}
				}
				else
				{
					if ((fp = fopen(name, "r")) == NULL)
					{
						printf("ファイルオープンエラー"ERROR_EDITOR_NAME_CONTINUATION);exit(1);
					}
				}
			}
			system("cls");
			printf("\x1b[?25l");
			editor(note, fp, k, name);
			fclose(fp);
			if (k == CONTINUATION_MODE)
			{
				remove(name);
				if (rename("rhythm_sub.txt", name) != 0)
				{
					printf("読み込みエラーが発生しました。"ERROR_CONTINUATION_RENAME);exit(1);
				}
				
			}
		}
	}
	timeEndPeriod(1);
}
void gamen(SYSTEM* system_valie)
{
	int i, j, game_end = 18;
	char gamen[4000], combo_henkan[10];	//画面表示用
	char kye[7] = { "ASDJKL" }, kye_work[30];
	double time_now, time_oneloop, time_work, oneframe;		//fps計算
	oneframe = (double)1000 / system_valie->fps_valie;
	LARGE_INTEGER time_end, time_start;
	int tempo;				//ノーツ速度
	double tempo_sum;
	int hantei = 3, hantei_cnt;			//判定
	char kye_nowtouch = 0x0, kye_oldtouch;
	int fps_count, note_speed;
	note_speed = system_valie->note_speed;
	score.bestcombo = 0;
	score.seido.perfect = 0;
	score.seido.great = 0;
	score.seido.miss = 0;
	QueryPerformanceCounter(&time_start);
	//game system main
	for (fps_count = 0, combo = 0, time_now = 0.0, tempo_sum = 0.0, hantei_cnt = 0;game_end != 0;)
	{
		//FPS制御
		Sleep(5);
		do
		{
			QueryPerformanceCounter(&time_end);
			time_work = (double)(time_end.QuadPart - time_start.QuadPart) / (double)cpu_freq.QuadPart;
			time_work *= 1000.0;
			YieldProcessor();
		} while (time_work < oneframe);
		QueryPerformanceCounter(&time_end);
		time_oneloop = (double)(time_end.QuadPart - time_start.QuadPart) / (double)cpu_freq.QuadPart;
		time_oneloop *= 1000.0;
		time_now += time_oneloop;
		QueryPerformanceCounter(&time_start);
		kye_oldtouch = kye_nowtouch;
		//ノーツ降下処理
		while (time_now >= tempo_sum)
		{
			if (fps_count == note_speed - 1)
			{
				if (fscanf(fp, "%d %s", &tempo, note[0]) == EOF) game_end--;
				fps_count = 0;
			}
			else
			{
				tempo = 0;
				for (i = 0;i < 6;i++)
				{
					switch (note[1][i])
					{
					case LONG_NOTE_FAKE:
					case LONG_NOTE:
					case LONG_START_NOTE:note[0][i] = LONG_NOTE_FAKE; break;
					case LONG_NOTE_LINK_FAKE:
					case LONG_NOTE_LINK:
					case LONG_START_NOTE_LINK:note[0][i] = LONG_NOTE_LINK_FAKE; break;
					default:note[0][i] = NOT_NOTE;
					}
				}
				fps_count++;
			}
			if (tempo != 0) tempo_sum = ((double)60 / tempo * (double)1000) / (double)note_speed;
			for (i = MISS_LATE - 1;i >= 0;i--)
				for (j = 0;j < 6;j++)
				{
					note[i + 1][j] = note[i][j];
					note[i][j] = NOT_NOTE;
				}
			time_now -= tempo_sum;
			if (hantei_cnt == 35) { hantei = 3; hantei_cnt = 0; }
			else hantei_cnt++;
			kye_nowtouch = 0x0;
		}
		//play画面
		gamen[0] = '\0';
		strcat(gamen, "\n\n\n\n\n");
		for (i = 0;i < ARRAY_GYO;i++)
		{
			if (hantei == 2)strcat(gamen, COLLAR_MISS_EFFECT"\n               │"COLLAR_RESET);
			else strcat(gamen, "\n               │");
			if (i == PERFECT)	strcat(gamen, "□");
			else		strcat(gamen, " ");
			for (j = 0;j < 6;j++)
			{
				switch (note[i][j])
				{
				case NORMAL_NOTE:			strcat(gamen, COLLAR_NORMAL_NOTE"■■"COLLAR_RESET);break;
				case LONG_NOTE_FAKE:
				case LONG_NOTE:				strcat(gamen, COLLAR_LONG_NOTE"○○"COLLAR_RESET); break;
				case LONG_START_NOTE:		strcat(gamen, COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET); break;
				case LONG_END_NOTE:			strcat(gamen, COLLAR_LONG_SE_NOTE"■■"COLLAR_RESET); break;
				case TRACE_NOTE:			strcat(gamen, COLLAR_TRACE_NOTE"◇◇"COLLAR_RESET); break;
				case NORMAL_NOTE_LINK:		strcat(gamen, COLLAR_NORMAL_NOTE"■■■"COLLAR_RESET); break;
				case LONG_NOTE_LINK_FAKE:
				case LONG_NOTE_LINK:		strcat(gamen, COLLAR_LONG_NOTE"○○○"COLLAR_RESET); break;
				case LONG_START_NOTE_LINK:	strcat(gamen, COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET); break;
				case LONG_END_NOTE_LINK:	strcat(gamen, COLLAR_LONG_SE_NOTE"■■■"COLLAR_RESET); break;
				case TRACE_NOTE_LINK:		strcat(gamen, COLLAR_TRACE_NOTE"◇◇◇"COLLAR_RESET); break;
				default:
					if (i == PERFECT)		strcat(gamen, "□□");
					else					strcat(gamen, "  ");
				}
				if (note[i][j] != NORMAL_NOTE_LINK && note[i][j] != LONG_NOTE_LINK && note[i][j] != LONG_NOTE_LINK_FAKE && note[i][j] != TRACE_NOTE_LINK && note[i][j] != LONG_START_NOTE_LINK && note[i][j] != LONG_END_NOTE_LINK)
				{
					if (i == PERFECT)	strcat(gamen, "□");
					else			strcat(gamen, " ");
				}
			}
			if (hantei == 2)strcat(gamen, COLLAR_MISS_EFFECT"│"COLLAR_RESET);
			else strcat(gamen, "│");
			if (i == 2)
			{
				if (score.seido.great == 0 && score.seido.miss == 0)	strcat(gamen, COLLAR_COMBO_AP"  COMBO"COLLAR_RESET);
				else if (score.seido.miss == 0)					strcat(gamen, COLLAR_COMBO_FC"  COMBO"COLLAR_RESET);
				else											strcat(gamen, "  COMBO");
			}
			if (i == 3)
			{
				sprintf(combo_henkan, "   %3d", combo);
				strcat(gamen, combo_henkan);
			}
		}
		strcat(gamen, "\n                  ");
		for (i = 0;i < 6;i++)
		{
			if ((kye_nowtouch >> i & 0x1) == 1)	sprintf(kye_work, COLLAR_KYE"%c  "COLLAR_RESET, kye[i]);
			else				sprintf(kye_work, "%c  ", kye[i]);
			strcat(gamen, kye_work);
		}
		strcat(gamen, "\n\n");
		//判定表示
		if (time_now <= tempo_sum)
		{
			switch (hantei)
			{
			case 0: strcat(gamen, COLLAR_PERFECT"                     perfect!\n"COLLAR_RESET);	break;
			case 1:	strcat(gamen, COLLAR_GREAT"                      great  \n"COLLAR_RESET);	break;
			case 2:	strcat(gamen, "                       miss  \n");
			default: strcat(gamen, "                             \n");
			}
		}
		printf("%s", gamen);

		//入力判定
		if (GetAsyncKeyState(kye[0]) & 0x8000)	kye_nowtouch |= 0x1;
		if (GetAsyncKeyState(kye[1]) & 0x8000)	kye_nowtouch |= 0x2;
		if (GetAsyncKeyState(kye[2]) & 0x8000)  kye_nowtouch |= 0x4;
		if (GetAsyncKeyState(kye[3]) & 0x8000)	kye_nowtouch |= 0x8;
		if (GetAsyncKeyState(kye[4]) & 0x8000)	kye_nowtouch |= 0x10;
		if (GetAsyncKeyState(kye[5]) & 0x8000)	kye_nowtouch |= 0x20;
		hantei = note_hantei(hantei, kye_nowtouch, kye_oldtouch);
		printf("\x1b[H");
	}
	if (score.bestcombo < combo) score.bestcombo = combo;
}
int note_hantei(int hantei, char kye_nowtouch, char kye_oldtouch)
{
	int i, j;
	char now_kye_solo, now_kye_longend;
	//ロングノーツ終わり
	now_kye_longend = ~kye_nowtouch & kye_oldtouch;
	if (now_kye_longend != 0) hantei = solo_note(hantei, now_kye_longend, LONG_END_NOTE, LONG_END_NOTE_LINK);
	//長押し可ノーツ判定
	//ロングノーツ
	for (i = 0;i < 6;i++)
		if ((kye_nowtouch >> i & 0x1) == 1 && note[PERFECT][i] == LONG_NOTE || (kye_nowtouch >> i & 0x1) == 1 && note[PERFECT][i] == LONG_NOTE_LINK)
		{
			for (j = i;j < 6;j++)
			{
				if (note[PERFECT][j] == LONG_NOTE)
				{
					note[PERFECT][j] = NOT_NOTE;
					break;
				}
				else if (note[PERFECT][j] == LONG_NOTE_LINK)
				{
					note[PERFECT][j] = NOT_NOTE;
				}
				else if (note[PERFECT][j] != NORMAL_NOTE_LINK && note[PERFECT][j] != TRACE_NOTE_LINK) break;
			}
			for (j = i - 1;j >= 0;j--)
			{
				if (note[PERFECT][j] == LONG_NOTE_LINK)
				{
					note[PERFECT][j] = NOT_NOTE;
				}
				else if (note[PERFECT][j] != NORMAL_NOTE_LINK && note[PERFECT][j] != TRACE_NOTE_LINK) break;
			}
			combo++;
			hantei = 0;
			score.seido.perfect++;
		}
	//ノーツ速度揃えノーツ
	for (i = 0;i < 6;i++)
		if (note[PERFECT][i] == LONG_NOTE_FAKE || note[PERFECT][i] == LONG_NOTE_LINK_FAKE)
			note[PERFECT][i] = NOT_NOTE;
	//トレースノーツ
	for (i = 0;i < 6;i++)
		if ((kye_nowtouch >> i & 0x1) == 1 && note[PERFECT][i] == TRACE_NOTE || (kye_nowtouch >> i & 0x1) == 1 && note[PERFECT][i] == TRACE_NOTE_LINK || (kye_nowtouch >> i & 0x1) == 1 && note[GREAT_LATE][i] == TRACE_NOTE || (kye_nowtouch >> i & 0x1) == 1 && note[GREAT_LATE][i] == TRACE_NOTE_LINK)
		{
			for (j = i;j < 6;j++)
			{
				if (note[GREAT_LATE][j] == TRACE_NOTE || note[PERFECT][j] == TRACE_NOTE)
				{
					if (note[GREAT_LATE][j] == TRACE_NOTE && note[PERFECT][j] == TRACE_NOTE)
					{
						note[GREAT_LATE][j] = NOT_NOTE;
						note[PERFECT][j] = NOT_NOTE;
					}
					else if (note[GREAT_LATE][j] == TRACE_NOTE) note[GREAT_LATE][j] = NOT_NOTE;
					else					  note[PERFECT][j] = NOT_NOTE;
					break;
				}
				else if (note[GREAT_LATE][j] == TRACE_NOTE_LINK || note[PERFECT][j] == TRACE_NOTE_LINK)
				{
					if (note[GREAT_LATE][j] == TRACE_NOTE_LINK && note[PERFECT][j] == TRACE_NOTE_LINK)
					{
						note[GREAT_LATE][j] = NOT_NOTE;
						note[PERFECT][j] = NOT_NOTE;
					}
					else if (note[GREAT_LATE][j] == TRACE_NOTE_LINK) note[GREAT_LATE][j] = NOT_NOTE;
					else					  note[PERFECT][j] = NOT_NOTE;
				}
				else break;
			}
			for (j = i - 1;j >= 0;j--)
			{
				if (note[GREAT_LATE][j] == TRACE_NOTE_LINK || note[PERFECT][j] == TRACE_NOTE_LINK)
				{
					if (note[GREAT_LATE][j] == TRACE_NOTE_LINK && note[PERFECT][j] == TRACE_NOTE_LINK)
					{
						note[GREAT_LATE][j] = NOT_NOTE;
						note[PERFECT][j] = NOT_NOTE;
					}
					else if (note[GREAT_LATE][j] == TRACE_NOTE_LINK) note[GREAT_LATE][j] = NOT_NOTE;
					else					  note[PERFECT][j] = NOT_NOTE;
				}
				else break;
			}
			combo++;
			hantei = 0;
			score.seido.perfect++;
		}
	//単押しのみ可ノーツ判定
	now_kye_solo = kye_nowtouch & ~kye_oldtouch;	//単押しノーツ多重反応対策
	if (now_kye_solo != 0)
	{
		hantei = solo_note(hantei, now_kye_solo, NORMAL_NOTE, NORMAL_NOTE_LINK);
		hantei = solo_note(hantei, now_kye_solo, LONG_START_NOTE, LONG_START_NOTE_LINK);
	}
	//スルーノーツmiss判定処理
	for (i = 0;i < 6;i++)
		if (note[MISS_LATE][i] != NOT_NOTE)
		{
			switch (note[MISS_LATE][i])
			{
			case NORMAL_NOTE:
			case NORMAL_NOTE_LINK:		miss_note(i, NORMAL_NOTE, NORMAL_NOTE_LINK); break;
			case LONG_NOTE:
			case LONG_NOTE_LINK:		miss_note(i, LONG_NOTE, LONG_NOTE_LINK); break;
			case LONG_START_NOTE:
			case LONG_START_NOTE_LINK:	miss_note(i, LONG_START_NOTE, LONG_START_NOTE_LINK); break;
			case LONG_END_NOTE:
			case LONG_END_NOTE_LINK:	miss_note(i, LONG_END_NOTE, LONG_END_NOTE_LINK); break;
			case TRACE_NOTE:
			case TRACE_NOTE_LINK:		miss_note(i, TRACE_NOTE, TRACE_NOTE_LINK); break;
			}
			combo = 0;
			hantei = 2;
			score.seido.miss++;
		}
	return hantei;
}
void miss_note(int i, int notenumber, int notenumber_link)
{
	int j;
	for (j = i;j < 6;j++)
	{
		if (note[MISS_LATE][j] == notenumber)
		{
			note[MISS_LATE][j] = NOT_NOTE;
			break;
		}
		else if (note[MISS_LATE][j] == notenumber_link) note[MISS_LATE][j] = NOT_NOTE;
		else break;
	}
	for (j = i - 1;j >= 0;j--)
	{
		if (note[MISS_LATE][j] == notenumber_link)
		{
			note[MISS_LATE][j] = NOT_NOTE;
		}
		else break;
	}
}
int solo_note(int hantei, char hit, int notenumber, int notenumber_link)
{
	int i, j, m;
	for (i = 0;i < 6;i++)
	{
		if ((hit >> i & 0x1) == 1)
		{
			for (j = GREAT_LATE;j >= MISS_FAST;j--)
			{
				if (note[j][i] == notenumber || note[j][i] == notenumber_link)
				{
					for (m = i;m < 6;m++)
					{
						if (note[j][m] == notenumber || note[j][m] == notenumber_link)
						{
							if (note[j][m] == notenumber)
							{
								note[j][m] = NOT_NOTE;
								break;
							}
							note[j][m] = NOT_NOTE;
						}
						else break;
					}
					for (m = i - 1;m >= 0;m--)
					{
						if (note[j][m] == notenumber_link)
						{
							note[j][m] = NOT_NOTE;
						}
						else break;
					}
					if (notenumber == LONG_END_NOTE) m = 1;
					else m = 0;
					if (j == PERFECT || m == 1 && j == PERFECT + m)
					{
						combo++;
						hantei = 0;
						score.seido.perfect++;
					}
					else if (j == MISS_FAST)
					{
						if (score.bestcombo < combo) score.bestcombo = combo;
						combo = 0;
						hantei = 2;
						score.seido.miss++;
					}
					else
					{
						combo++;
						hantei = 1;
						score.seido.great++;
					}
					break;
				}
			}
		}
	}
	return hantei;
}
void result(char play_name[80])
{
	char work_name[80];
	SCORE work_score;
	//リザルト表示
	printf("\x1b[14;19H");
	if (score.seido.perfect == score.bestcombo && score.seido.miss == 0) { printf(COLLAR_COMBO_AP" ALL PERFECT!!"COLLAR_RESET); score.seiseki = 2; }
	else if (score.seido.miss == 0) { printf(COLLAR_COMBO_FC"  FULL COMBO!"COLLAR_RESET); score.seiseki = 1; }
	else { printf("     CLEAR"); score.seiseki = 0; }
	Sleep(5000);
	printf(GAMEN_CLEAR);
	printf("   BESTCOMBO\n");
	if (score.seido.perfect == score.bestcombo && score.seido.miss == 0)	printf(COLLAR_COMBO_AP" ALL PERFECT!!"COLLAR_RESET);
	else if (score.seido.miss == 0)									printf(COLLAR_COMBO_FC"  FULL COMBO!"COLLAR_RESET);
	printf("\n      %d\n\n", score.bestcombo);
	printf(COLLAR_PERFECT"perfect  %04d\n"COLLAR_RESET, score.seido.perfect);
	printf(COLLAR_GREAT"great    %04d\n"COLLAR_RESET, score.seido.great);
	printf("miss     %04d\n", score.seido.miss);
	printf("\x1b[?25h");
	//リザルト保存
	if ((fp_result = fopen("rhythm_game_result.txt", "r")) == NULL || (fp = fopen("sub.txt", "w")) == NULL)
	{
		printf("ファイルオープンエラーが発生しました。"ERROR_RESULT); exit(1);
	}
	while (fscanf(fp_result, "%s %d %d", work_name, &work_score.seiseki, &work_score.bestcombo) != EOF)
	{
		strcat(work_name, ".txt");
		if (strcmp(work_name,
			play_name) == 0 && (work_score.seiseki < score.seiseki || work_score.bestcombo < score.bestcombo))
			work_score = score;
		work_name[strlen(work_name) - 4] = '\0';
		fprintf(fp, "%s %d %d\n", work_name, work_score.seiseki, work_score.bestcombo);
	}
	fclose(fp_result);
	fclose(fp);
	remove("rhythm_game_result.txt");
	if (rename("sub.txt", "rhythm_game_result.txt") != 0)
	{
		printf("読み込みエラーが発生しました。"ERROR_RESULT_RENAME); exit(1);
	}
}
