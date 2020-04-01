#ifndef KAI_INCLUDED
#define KAI_INCLUDED
#include <stdio.h>
#include <string.h>
#include <math.h>

#define RED 0
#define BLACK 1
#define CHESS_COVER -1
#define CHESS_EMPTY -2
#define COMMAND_NUM 18
class KAI
{
	enum {
		EMPTY=0,
		RK, RG, RM, RR, RN, RC, RP,
		CLOSE,
		BK, BG, BM, BR, BN, BC, BP,
		NUM_KIND
	};
	const char* commands_name[COMMAND_NUM] = {
		"protocol_version",
		"name",
		"version",
		"known_command",
		"list_commands",
		"quit",
		"boardsize",
		"reset_board",
		"num_repetition",
		"num_moves_to_draw",
		"move",
		"flip",
		"genmove",
		"game_over",
		"ready",
		"time_settings",
		"time_left",
  	"showboard"
	};
public:
	void generateMove(char move[6]);
	void MakeMove(const char move[6]);
	KAI(void);
	~KAI(void);
	void initBoardState();
	void Pirnf_Chessboard();
	// commands
	bool protocol_version(const char* data[], char* response);// 0
	bool name(const char* data[], char* response);// 1
	bool version(const char* data[], char* response);// 2
	bool known_command(const char* data[], char* response);// 3
	bool list_commands(const char* data[], char* response);// 4
	bool quit(const char* data[], char* response);// 5
	bool boardsize(const char* data[], char* response);// 6
	bool reset_board(const char* data[], char* response);// 7
	bool num_repetition(const char* data[], char* response);// 8
	bool num_moves_to_draw(const char* data[], char* response);// 9
	bool move(const char* data[], char* response);// 10
	bool flip(const char* data[], char* response);// 11
	bool genmove(const char* data[], char* response);// 12
	bool game_over(const char* data[], char* response);// 13
	bool ready(const char* data[], char* response);// 14
	bool time_settings(const char* data[], char* response);// 15
	bool time_left(const char* data[], char* response);// 16
	bool showboard(const char* data[], char* response);// 17
	//utility
	int FlipList ( int *flip_moves );
	int ConvertChessNo(int input);
	void Pirnf_Chess(int chess_no,char *Result);
	bool Referee(int* Board,int Startoint,int EndPoint,int color);
	int Expand(int* Board,int color,int *Result);
	int getBaord (int i) { return Board[i]; }
	int getColor() { return Color; }
	int getClose(int i) { return CloseChess[i]; }
private:
	int Color;
	int Red_Time, Black_Time;
	int Board[32];
	int CloseChess[14];
};

#endif
