#include <vector>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <stdlib.h>

#include <stdio.h> //test

static const double INF=2147483647;
static const double WIN=1000000000;
const char chess[14] = {'P', 'C', 'N', 'R', 'M', 'G', 'K', 'p', 'c', 'n', 'r', 'm', 'g', 'k'};
const int rate[7] = {2, 25, 5, 10, 20, 30, 50};
const int flip_neighbor[2][7] = {
	{-1000, -5000, -100, 200, 600, 800, -5000},
	{-1000, 5000, 100, -200, -600, -800, -5000}};
const int flip_step[2][7] = {
	{-100, 500, -100, -100, -1000, -1000, -1000},
	{-100, -5000, -200, -400, -600, -800, -1000}};
const int attack_value[7][7] = {
				{10, 0, 0, 0, 0, 0, 108},
				{ 0, 0, 0, 0, 0, 0,   0},
				{12,48,22, 0, 0, 0,   0},
				{12,48,24,34, 0, 0,   0},
				{12,48,24,36,58, 0,   0},
				{12,48,24,36,60,106,  0},
				{ 0,48,24,36,60,108,120}};
int ConvertBack(int input) {
	switch(input) {
		case -2: return 0; break; case -1: return 8; break;
		case 6: return   1; break; case 5: return   2; break;
		case 4: return   3; break; case 3: return   4; break;
		case 2: return   5; break; case 1: return   6; break;
		case 0: return   7; break; case 13: return  9; break;
		case 12: return 10; break; case 11: return 11; break;
		case 10: return 12; break; case 9: return 13; break;
		case 8: return 14; break; case 7: return 15; break;
	}
	return -1;
}
static unsigned long long hash_table[32][16];
void genHash() {
  srand(1);
  for ( int i = 0; i < 32; ++i) {
    for ( int j = 0; j < 16; ++j) {
      hash_table[i][j] = ((unsigned long long)rand() << 32) + rand();
    }
  }
}
struct Node
{
  int Board[32];
  int CloseChess[14];
  int Color;
  std::vector<Node*> parents;
  std::vector<Node*> childern;
  unsigned long long hash;

  Node ( KAI &ai ) {
    hash = 0;
    for ( int i=0; i<32; i++ ) {
      Board[i] = ai.getBaord(i);
      hash ^= hash_table[i][ConvertBack(ai.getBaord(i))];
    }
    for ( int i=0; i<14; i++ ) {
      CloseChess[i] = ai.getClose(i);
    }
    Color = ai.getColor();
  }
  Node ( const Node& n ): Color(n.Color), hash(n.hash) {
    for ( int i=0; i<32; i++ ) {
      Board[i] = n.Board[i];
    }
    for ( int i=0; i<14; i++ ) {
      CloseChess[i] = n.CloseChess[i];
    }
  }
  int NonFlipList(int* Board,int color,int *non_flip_moves) {
  	int res = 0;
  	for(int i=0;i<32;i++) {
  		if ( Board[i] >= 0 && Board[i]/7 == color ) {
  			if ( Board[i] % 7 == 1 ) { // Cannon
  				int row = i/4;
  				int col = i%4;
  				for ( int rowCount=row*4; rowCount<(row+1)*4; rowCount++ ) {
  					if ( IsLegal(Board,i,rowCount,color)) {
  						non_flip_moves[res++] = i*100+rowCount;
  					}
  				}
  				for(int colCount=col; colCount<32;colCount += 4) {
  					if(IsLegal(Board,i,colCount,color)) {
  						non_flip_moves[res++] = i*100+colCount;
  					}
  				}
  			}
  			else {
  				int Move[4] = {i-4,i+1,i+4,i-1};
  				for(int k=0; k<4;k++) {
  					if(Move[k] >= 0 && Move[k] < 32 && IsLegal(Board,i,Move[k],color)) {
  						non_flip_moves[res++] = i*100+Move[k];
  					}
  				}
  			}
  		};
  	}
  	return (res);
  }
	int BestFlip() {
		int res = -1;
		int score = -INF;
  	for(int i=0; i<32; i++) {
			int now = 0;
  		if(Board[i] == CHESS_COVER) {
  			if ( i%4 != 0 && Board[i-1] >= 0) { now += flip_neighbor[(Board[i-1]/7)^Color][Board[i-1]%7]; }
				if ( i%4 != 3 && Board[i+1] >= 0) { now += flip_neighbor[(Board[i+1]/7)^Color][Board[i+1]%7]; }
				if ( i-4 >= 0 && Board[i-4] >= 0) { now += flip_neighbor[(Board[i-4]/7)^Color][Board[i-4]%7]; }
				if ( i+4 < 32 && Board[i+4] >= 0) { now += flip_neighbor[(Board[i+4]/7)^Color][Board[i+4]%7]; }
				int ii = i-1, count = 0;
				while ( ii >= i/4*4 && count < 2 ) {
					if (Board[ii] >= -1) ++count;
					--ii;
				}
				if ( count == 2 && Board[ii+1] >= 0) { now += flip_step[(Board[ii+1]/7)^Color][Board[ii+1]%7]; }
				ii = i+1; count = 0;
				while ( ii < i/4*4+4 && count < 2 ) {
					if (Board[ii] >= -1) ++count;
					++ii;
				}
				if ( count == 2 && Board[ii-1] >= 0) { now += flip_step[(Board[ii-1]/7)^Color][Board[ii-1]%7]; }
				ii = i-4; count = 0;
				while ( ii >= 0 && count < 2 ) {
					if (Board[ii] >= -1) ++count;
					ii -= 4;
				}
				if ( count == 2 && Board[ii+4] >= 0) { now += flip_step[(Board[ii+4]/7)^Color][Board[ii+4]%7]; }
				ii = i+4; count = 0;
				while ( ii < 32 && count < 2 ) {
					if (Board[ii] >= -1) ++count;
					ii += 4;
				}
				if ( count == 2 && Board[ii-4] >= 0) { now += flip_step[(Board[ii-4]/7)^Color][Board[ii-4]%7]; }
				if (now > score) {
					res = i*100+i;
					score = now;
				}
				else if (now == score) {
					if ( rand() % 2 ) { res = i*100+i; }
				}
  		}
  	}
		//fprintf(stderr, "score: %i\n", score);
		//fprintf(stderr, "best flip: %i\n", res);
  	return (res);
	}
  bool IsLegal ( int* chess,int from_location_no,int to_location_no,int UserId ) {
  	bool IsCurrent = true;
  	int from_chess_no = chess[from_location_no], to_chess_no = chess[to_location_no];
  	int from_row = from_location_no / 4, to_row = to_location_no / 4;
  	int from_col = from_location_no % 4, to_col = to_location_no % 4;
  	if(from_chess_no < 0 || ( to_chess_no < 0 && to_chess_no != CHESS_EMPTY) ) {
  		IsCurrent = false;
  	}
  	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId) {
  		IsCurrent = false;
  	}
  	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0) {
  		IsCurrent = false;
  	}
  	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1){ //legal move
  		IsCurrent = true;
  	}
  	else if(from_chess_no % 7 == 1 ) { //judge cannon
  		int row_gap = from_row-to_row;
  		int col_gap = from_col-to_col;
  		int between_Count = 0;
  		if(from_row-to_row == 0 || from_col-to_col == 0) {
  			if(row_gap == 0)  {
  				for(int i=1;i<abs(col_gap);i++) {
  					int between_chess;
  					if(col_gap>0) { between_chess = chess[from_location_no-i] ; }
  					else { between_chess = chess[from_location_no+i] ; }
  					if(between_chess != CHESS_EMPTY) { between_Count++; }
  				}
  			}
  			else {
  				for(int i=1;i<abs(row_gap);i++) {
  					int between_chess;
  					if(row_gap > 0) { between_chess = chess[from_location_no-4*i] ; }
  					else { between_chess = chess[from_location_no+4*i] ; }
  					if(between_chess != CHESS_EMPTY) { between_Count++; }
  				}
  			}
  			if(between_Count != 1 ) {
  				IsCurrent = false;
  			}
  			else if(to_chess_no == CHESS_EMPTY) {
  				IsCurrent = false;
  			}
  		}
  		else {
  			IsCurrent = false;
  		}
  	}
  	else { // non cannon
  		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1) {
  			IsCurrent = false;
  		}
  		else if(from_chess_no % 7 == 0) {
  			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6) {
  				IsCurrent = false;
  			}
  		}
  		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0) {
  			IsCurrent = false;
  		}
  		else if(from_chess_no % 7 < to_chess_no% 7) {
  			IsCurrent = false;
  		}
  	}
  	return IsCurrent;
  }
  void MakeMove(const char move[6]) {
  	int src, dst;
  	src = ('8'-move[1])*4+(move[0]-'a');
  	if(move[2]=='('){
  		Board[src] = ConvertChessNo(GetFin(move[3]));
      --CloseChess[ConvertChessNo(GetFin(move[3]))];
  	} else {
  		dst = ('8'-move[4])*4+(move[3]-'a');
  		Board[dst] = Board[src];
  		Board[src] = CHESS_EMPTY;
  	}
    Color = Color^1;
  }
  int ConvertChessNo(int input) {
  	switch(input) {
  		case 0: return CHESS_EMPTY; break; case 8: return CHESS_COVER; break;
  		case 1: return   6; break; case 2: return   5; break;
  		case 3: return   4; break; case 4: return   3; break;
  		case 5: return   2; break; case 6: return   1; break;
  		case 7: return   0; break; case 9: return  13; break;
  		case 10: return 12; break; case 11: return 11; break;
  		case 12: return 10; break; case 13: return 9; break;
  		case 14: return 8; break; case 15: return 7; break;
  	}
  	return -1;
  }
  int GetFin(char c) {
  	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
  	for(int f=0;f<16;f++){
  		if(c==skind[f]) {
  			return f;
  		}
  	}
  	return -1;
  }
};
double Eval(const Node &B) {
  	double t=0;
		int ind=0;
		int myChess[16];
  	for(int p=0; p<32; p++){
  		int c = (B.Board[p]>=0)? B.Board[p] / 7 : -1;
  		if(c == B.Color) {
        t+=rate[B.Board[p]%7]*10000;
				myChess[ind]=p;
				++ind;
      }
      else if ( c != -1 ) {
        t-=rate[B.Board[p]%7]*10000;
				for (int j=0; j<ind; ++j) {
					int dis = abs(myChess[j]/4 - p/4) + abs(myChess[j]%4 - p%4);
					double mul = (dis == 1)? 0.5:pow(2.0, 2.0-dis);
					t += attack_value[B.Board[myChess[j]]%7][B.Board[p]%7] * mul * 100;
				}
      }
  	}
  	for(int i=B.Color*7; i<B.Color*7+7; ++i) { t += rate[i%7]*10000*B.CloseChess[i]; }

    for(int i=(B.Color^1)*7; i<(B.Color^1)*7+7; ++i) { t -= rate[i%7]*10000*B.CloseChess[i]; }
  	return t+rand()%10;
}
double NegaScout( Node &B, int dep, int cut, double alpha, double beta, int& BestMove ) {
	if (cut==0) { return Eval(B); }
  int result[100];
	int best_flip = B.BestFlip();
	int non_flips = B.NonFlipList(B.Board, B.Color, result);
	if ( best_flip == -1 && non_flips == 0 ) { return -WIN+dep; }
	if ( dep == 0 && best_flip != -1 && non_flips == 0) {
		BestMove = best_flip;
		fprintf(stderr, "%i flip_only\n", best_flip);
		return 0;
	}
	double ret = -INF;
	double upb = beta;
	for (int i=0; i<non_flips; i++) {
		Node N(B);
    char move[6];
    sprintf(move, "%c%c-%c%c",'a'+(result[i]/100%4),'1'+(7-result[i]/400),'a'+(result[i]%4),'1'+(7-result[i]%100/4));
		N.MakeMove(move);
    int _;
		double tmp= -NegaScout(N, dep+1, cut-1, -upb, -std::max(alpha, ret), _);
		if (tmp > ret) {
			if ( upb == beta || cut < 3 || tmp >= beta ) {
				ret = tmp;
				BestMove = result[i];
				if (dep == 0) { fprintf(stderr, "%s score= %.2lf\n", move, tmp); } //debug
			}
			else {
				ret = -NegaScout(N, dep+1, cut-1, -beta, -tmp, _);
				BestMove = result[i];
				if (dep == 0) { fprintf(stderr, "%s score= %.2lf\n", move, tmp); } //debugs
			}
		}
		if (ret >= beta)	return ret;
		upb = std::max(alpha, ret) + 1;
	}
	if ( best_flip != -1 ) {
		int total = 0;
		for (int i = 0; i < 14; ++i) { total += B.CloseChess[i]; }
		double flip_score = 0;
		for (int i = 0; i < 14; ++i) {
			if (B.CloseChess[i] > 0) {
				Node N(B);
				char move[6];
				sprintf(move, "%c%c(%c)", 'a'+(best_flip%4), '1'+(7-best_flip/400), chess[i]);
				N.MakeMove(move);
				int _;
				double tmp = -NegaScout(N, dep+1, cut-1, -INF, INF, _);
				//fprintf(stderr, "%c%c(%c) flip_score = %.2lf\n", 'a'+(best_flip%4), '1'+(7-best_flip/400), chess[i], tmp);
				flip_score += tmp * B.CloseChess[i] / total;
			}
		}
		if (dep == 0) { fprintf(stderr, "%c%c flip_score = %.2lf\n", 'a'+(best_flip%4), '1'+(7-best_flip/400), flip_score); } //debug
		if (flip_score > ret) {
			ret = flip_score;
			if(dep == 0) {
				BestMove = best_flip;
			}
		}
		if(ret >= beta)	return ret;
	}
	return ret;
}
