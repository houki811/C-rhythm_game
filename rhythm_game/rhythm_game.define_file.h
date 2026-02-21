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