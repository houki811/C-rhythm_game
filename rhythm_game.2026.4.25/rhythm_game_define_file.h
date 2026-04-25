#pragma once
//■コード
#define GAMEN_CLEAR "\x1b[H\x1b[2J"
#define MAGICNUMBER_0 0x30
//■配列
#define ARRAY_GYO 16
//■ゲームモード選択
#define PLARY_KYE '1'
#define SYSTEM_KYE '2'
#define EDITOR_KYE '3'
#define END_KYE '4'
	//playおよび創作楽曲選択
#define SENTAKU_END 0
	//設定選択
#define FPS_EDITOR '1'
#define NOTE_SPEED_EDITOR '2'
#define NOTE_SPEED_TEST '0'
#define SYSTEM_END '3'
//■編集モード選択
#define OVERWRITE_MODE '1'
#define CONTINUATION_MODE '2'
#define NEW_MODE '3'
//■編集モード(画面外移動関数)
#define SELECTION_RESTRICTIONS 0x8
#define PAST_STRAT 0x4
#define MORE_PAST 0x5
#define PAST_RETURN 0x6
#define PAST_END 0x1
#define IF_EQUAL(a,b) (((a) & (b)) == b)
#define IF_DIFFERENT(a,b)(((a) & (b)) != b)
//■判定行
#define PERFECT 13
#define GREAT_FAST 12
#define GREAT_LATE 14
#define MISS_FAST 11
#define MISS_LATE 15
//■ノーツ番号
	//単体(右端)
#define NOT_NOTE '0'
#define NORMAL_NOTE '1'
#define LONG_NOTE '2'
#define TRACE_NOTE '3'
#define LONG_START_NOTE '4'
#define LONG_END_NOTE '5'
	//複数
#define NORMAL_NOTE_LINK 'a'
#define LONG_NOTE_LINK 'b'
#define TRACE_NOTE_LINK 'c'
#define LONG_START_NOTE_LINK 'd'
#define LONG_END_NOTE_LINK 'e'
	//表示用ノーツ
#define LONG_NOTE_FAKE 'x'
#define LONG_NOTE_LINK_FAKE 'z'
//■カラー
	//カラーリセット
#define COLLAR_RESET "\x1b[0m"
	//AP・FCカラー
#define COLLAR_COMBO_AP "\x1b[38;2;255;182;193m"
#define COLLAR_COMBO_FC "\x1b[38;2;173;216;230m"
	//ノーツカラー
#define COLLAR_NORMAL_NOTE "\x1b[38;2;222;240;242m"
#define COLLAR_LONG_NOTE "\x1b[38;2;164;242;226m"
#define COLLAR_LONG_SE_NOTE "\x1b[38;2;140;242;221m"
#define COLLAR_TRACE_NOTE "\x1b[38;2;249;250;121m"
	//入力キー表示カラー
#define COLLAR_KYE "\x1b[38;2;103;215;238m"
	//missエフェクトカラー
#define COLLAR_MISS_EFFECT "\x1b[38;2;196;43;30m"
	//精度表示カラー
#define COLLAR_PERFECT "\x1b[38;2;242;192;231m"
#define COLLAR_GREAT "\x1b[38;2;150;166;242m"
	//創作モード(1拍)
#define COLLAR_BEAT "\x1b[38;2;80;168;186m"
	//創作モード(1小節)
#define COLLAR_MEASURE "\x1b[38;2;51;170;238m"
	//創作モード(その他の行表示)
#define COLLAR_GYO "\x1b[38;2;128;128;128m"