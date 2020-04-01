#include "KAI.h"
#include "misc.h"
KAI::KAI(void){
	this->Color = 2;
	genHash();
}

KAI::~KAI(void){}

bool KAI::protocol_version(const char* data[], char* response){
	strcpy(response, "0.0.1");
  return 0;
}

bool KAI::name(const char* data[], char* response){
	strcpy(response, "Kuruwa_CDC_AI");
	return 0;
}

bool KAI::version(const char* data[], char* response){
	strcpy(response, "1.0.0");
	return 0;
}

bool KAI::known_command(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		if(!strcmp(data[0], commands_name[i])){
			strcpy(response, "true");
			return 0;
		}
	}
	strcpy(response, "false");
	return 0;
}

bool KAI::list_commands(const char* data[], char* response){
  for(int i = 0; i < COMMAND_NUM; i++){
		strcat(response, commands_name[i]);
		if(i < COMMAND_NUM - 1){
			strcat(response, "\n");
		}
	}
	return 0;
}

bool KAI::quit(const char* data[], char* response){
  fprintf(stderr, "Bye\n");
	return 0;
}

bool KAI::boardsize(const char* data[], char* response){
  fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]);
	return 0;
}

bool KAI::reset_board(const char* data[], char* response){
	this->Color = 2;
	this->Red_Time = -1; // known
	this->Black_Time = -1; // known
	this->initBoardState();
	return 0;
}

bool KAI::num_repetition(const char* data[], char* response){
  return 0;
}

bool KAI::num_moves_to_draw(const char* data[], char* response){
  return 0;
}

bool KAI::move(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool KAI::flip(const char* data[], char* response){
  char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}

bool KAI::genmove(const char* data[], char* response){
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = RED;
	}else if(!strcmp(data[0], "black")){
		this->Color = BLACK;
	}else{
		this->Color = 2;
	}
	// genmove
  char move[6];
	this->generateMove(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	return 0;
}

bool KAI::game_over(const char* data[], char* response){
  fprintf(stderr, "Game Result: %s\n", data[0]);
	return 0;
}

bool KAI::ready(const char* data[], char* response){
  return 0;
}

bool KAI::time_settings(const char* data[], char* response){
  return 0;
}

bool KAI::time_left(const char* data[], char* response){
  if(!strcmp(data[0], "red")){
		sscanf(data[1], "%d", &(this->Red_Time));
	}else{
		sscanf(data[1], "%d", &(this->Black_Time));
	}
	fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]);
	return 0;
}

bool KAI::showboard(const char* data[], char* response){
  Pirnf_Chessboard();
	return 0;
}

int GetFin(char c)
{
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	for(int f=0;f<16;f++)if(c==skind[f])return f;
	return -1;
}

void KAI::initBoardState ()
{
	/*

	iPieceCount[14] (index):  [00] [01] [02] [03] [04] [05] [06] [07] [08] [09] [10] [11] [12] [13]
	                           RK   RG   RM   RR   RN   RC   RP   BK   BG   BM   BR   BN   BC   BP
    	iCurrentPosition[32] (value):  [00] [01] [02] [03] [04] [05] [06] [07] [08] [09] [10] [11] [12] [13] [14] [15]
	                            	empty  RK   RR   RN   RN   RR   RC   RP  dark  BK   BG   BM   BR   BN   BC   BP
	*/
	// initial board
	char iCurrentPosition[32];
	for(int i = 0; i < 32; i++){ iCurrentPosition[i] = 'X'; }
	int iPieceCount[14] = {1, 2, 2, 2, 2, 2, 5, 1, 2, 2, 2, 2, 2, 5};

	fprintf(stderr, "\niPieceCount[14]:\n");
	for(int i=0;i<14;i++) {
		fprintf(stderr, "[%2d]",i);
	} fprintf(stderr, "\n");
	for(int i=0;i<14;i++) {
		fprintf(stderr, "%4d",iPieceCount[i]);
	} fprintf(stderr, "\n");
	fprintf(stderr, "iCurrentPosition[32]:\n");
	for(int i=28;i>=0;i-=4){
		for(int j=0;j<4;j++)
			fprintf(stderr, "%2c ",iCurrentPosition[i+j]);
		fprintf(stderr, " | ");
		for(int j=0;j<4;j++)
			fprintf(stderr, "%2d ",i+j);
		if(i%4==0)puts("");
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "\n\n\n");

	memcpy(this->CloseChess,iPieceCount,sizeof(int)*14);


	//convert to my format
	int Index = 0;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<4;j++)
		{
			this->Board[Index] = ConvertChessNo(GetFin(iCurrentPosition[Index]));
			Index++;
		}
	}
	this->Pirnf_Chessboard();
}

void KAI::generateMove( char move[6] )
{
	Node root(*this);
	int BestMove;
	NegaScout(root, 0, 6, -INF, INF, BestMove);

	int startPoint = BestMove/100;
	int endPoint = BestMove%100;
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(endPoint%4),'1'+(7-endPoint/4));
}
/*void KAI::generateMove(char move[6])
{
	// generateMove Call by reference: change src,dst

	int Result[100];
	int count = this->Expand(this->Board,this->Color,Result);
	int startPoint = 0;
	int EndPount = 0;

	int CoverIndex[32];
	int CoverCount = 0;
	for(int i=0;i<32;i++)
		if(this->Board[i] == CHESS_COVER)
		{
			CoverIndex[CoverCount] = i;
			CoverCount++;
		}
	int Answer;
	if(CoverCount > 0)
	{
		Result[count] = CoverIndex[rand()%CoverCount];
		Result[count] = Result[count]*100+Result[count];
		count++;
	}


		//randon choose legal move
	if(count > 0)
	{
		Answer = Result[rand()%count];
		startPoint = Answer/100;
		EndPount   = Answer%100;
		sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(EndPount%4),'1'+(7-EndPount/4));
	}
		//no legal move -> flip chess
	else
	{
		sprintf(move, "NAN");
	}
	char chess_Start[4]="";
	char chess_End[4]="";
	Pirnf_Chess(Board[startPoint],chess_Start);
	Pirnf_Chess(Board[EndPount],chess_End);
	printf("My result: \n--------------------------\n");
	printf("(%d) -> (%d)\n",startPoint,EndPount);
	printf("<%s> -> <%s>\n",chess_Start,chess_End);
	printf("move:%s\n",move);
	printf("--------------------------\n");
	this->Pirnf_Chessboard();
}*/

void KAI::MakeMove(const char move[6])
{
	int src, dst;
	src = ('8'-move[1])*4+(move[0]-'a');
	if(move[2]=='('){
		fprintf(stderr, "# call flip(): flip(%d,%d) = %d\n",src, src, GetFin(move[3]));
		this->Board[src] = ConvertChessNo(GetFin(move[3]));
		--CloseChess[ConvertChessNo(GetFin(move[3]))];
		Pirnf_Chessboard();
	}else {
		dst = ('8'-move[4])*4+(move[3]-'a');
		fprintf(stderr, "# Search call move(): move : %d-%d \n",src,dst);
		this->Board[dst] = this->Board[src];
		this->Board[src] = CHESS_EMPTY;
		Pirnf_Chessboard();
	}
	/* init time */
}



//---------------------------------------------my search function---------------------------------------------
int KAI::ConvertChessNo(int input)
{
	switch(input)
	{
	case 0:
		return CHESS_EMPTY;
		break;
	case 8:
		return CHESS_COVER;
		break;
	case 1:
		return 6;
		break;
	case 2:
		return 5;
		break;
	case 3:
		return 4;
		break;
	case 4:
		return 3;
		break;
	case 5:
		return 2;
		break;
	case 6:
		return 1;
		break;
	case 7:
		return 0;
		break;
	case 9:
		return 13;
		break;
	case 10:
		return 12;
		break;
	case 11:
		return 11;
		break;
	case 12:
		return 10;
		break;
	case 13:
		return 9;
		break;
	case 14:
		return 8;
		break;
	case 15:
		return 7;
		break;
	}
	return -1;
}

int KAI::Expand(int* Board,int color,int *Result)
{
	int ResultCount = 0;
	for(int i=0;i<32;i++)
	{
		if(Board[i] >= 0 && Board[i]/7 == color)
		{
			//Gun
			if(Board[i] % 7 == 1)
			{
				int row = i/4;
				int col = i%4;
				for(int rowCount=row*4;rowCount<(row+1)*4;rowCount++)
				{
					if(Referee(Board,i,rowCount,color))
					{
						Result[ResultCount] = i*100+rowCount;
						ResultCount++;
					}
				}
				for(int colCount=col; colCount<32;colCount += 4)
				{

					if(Referee(Board,i,colCount,color))
					{
						Result[ResultCount] = i*100+colCount;
						ResultCount++;
					}
				}
			}
			else
			{
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++)
				{

					if(Move[k] >= 0 && Move[k] < 32 && Referee(Board,i,Move[k],color))
					{
						Result[ResultCount] = i*100+Move[k];
						ResultCount++;
					}
				}
			}

		};
	}
	return ResultCount;
}





//---------------------------- Display --------------------------------
//Display chess board
void KAI::Pirnf_Chessboard()
{
	char Mes[1024]="";
	char temp[1024];
	char myColor[10]="";
	if(Color == -99)
		strcpy(myColor,"Unknown");
	else if(this->Color == RED)
		strcpy(myColor,"Red");
	else
		strcpy(myColor,"Black");
	sprintf(temp,"------------%s-------------\n",myColor);
	strcat(Mes,temp);
	strcat(Mes,"<8> ");
	for(int i=0;i<32;i++)
	{
		if(i != 0 && i % 4 == 0)
		{
			sprintf(temp,"\n<%d> ",8-(i/4));
			strcat(Mes,temp);
		}
		char chess_name[4]="";
		Pirnf_Chess(this->Board[i],chess_name);
		sprintf(temp,"%5s", chess_name);
		strcat(Mes,temp);


	}
	strcat(Mes,"\n\n     ");
	for(int i=0;i<4;i++)
	{
		sprintf(temp," <%c> ",'a'+i);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n");
	fprintf(stderr, "%s",Mes);
}


//Print chess
void  KAI::Pirnf_Chess(int chess_no,char *Result)
{
		// XX -> Empty
		if(chess_no == CHESS_EMPTY)
		{
			strcat(Result, " - ");
			return;
		}
		//OO -> DarkChess
		else if(chess_no == CHESS_COVER)
		{
			strcat(Result, " X ");
			return;
		}
		/*if(Color == RED )
				strcat(Result, "R");
		else
				strcat(Result, "B");*/
		switch(chess_no)
		{
		case 0:
				strcat(Result, " P ");
				break;
		case 1:
				strcat(Result, " C ");
				break;
		case 2:
				strcat(Result, " N ");
				break;
		case 3:
				strcat(Result, " R ");
				break;
		case 4:
				strcat(Result, " M ");
				break;
		case 5:
				strcat(Result, " G ");
				break;
		case 6:
				strcat(Result, " K ");
				break;
		case 7:
				strcat(Result, " p ");
				break;
		case 8:
				strcat(Result, " c ");
				break;
		case 9:
				strcat(Result, " n ");
				break;
		case 10:
				strcat(Result, " r ");
				break;
		case 11:
				strcat(Result, " m ");
				break;
		case 12:
				strcat(Result, " g ");
				break;
		case 13:
				strcat(Result, " k ");
				break;
	}
}

//legal move generation
int KAI::FlipList ( int *flip_moves ) {
	int res = 0;
	for(int i=0;i<32;i++) {
		if(this->Board[i] == CHESS_COVER) {
			flip_moves[res++] = i*100+i;
		}
	}
	return (res);
}

// Referee
bool KAI::Referee(int* chess,int from_location_no,int to_location_no,int UserId)
{
	bool IsCurrent = true;
	int from_chess_no = chess[from_location_no];
	int to_chess_no = chess[to_location_no];
	int from_row = from_location_no / 4;
	int to_row = to_location_no / 4;
	int from_col = from_location_no % 4;
	int to_col = to_location_no % 4;

	if(from_chess_no < 0 || ( to_chess_no < 0 && to_chess_no != CHESS_EMPTY) )
	{
		IsCurrent = false;
	}
	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId)
	{
		IsCurrent = false;
	}
	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0)
	{
		IsCurrent = false;
	}
	//check attack
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1)//legal move
	{
		IsCurrent = true;
	}
	else if(from_chess_no % 7 == 1 ) //judge gun
	{
		int row_gap = from_row-to_row;
		int col_gap = from_col-to_col;
		int between_Count = 0;
		//slant
		if(from_row-to_row == 0 || from_col-to_col == 0)
		{
			//row
			if(row_gap == 0)
			{
				for(int i=1;i<abs(col_gap);i++)
				{
					int between_chess;
					if(col_gap>0)
						between_chess = chess[from_location_no-i] ;
					else
						between_chess = chess[from_location_no+i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}
			}
			//column
			else
			{
				for(int i=1;i<abs(row_gap);i++)
				{
					int between_chess;
					if(row_gap > 0)
						between_chess = chess[from_location_no-4*i] ;
					else
						between_chess = chess[from_location_no+4*i] ;
					if(between_chess != CHESS_EMPTY)
						between_Count++;
				}

			}

			if(between_Count != 1 )
			{
				IsCurrent = false;
			}
			else if(to_chess_no == CHESS_EMPTY)
			{
				IsCurrent = false;
			}
		}
		//slide
		else
		{
			IsCurrent = false;
		}
	}
	else // non gun
	{
		//judge pawn or king

		//distance
		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1)
		{
			IsCurrent = false;
		}
		//judge pawn
		else if(from_chess_no % 7 == 0)
		{
			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6)
			{
				IsCurrent = false;
			}
		}
		//judge king
		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0)
		{
			IsCurrent = false;
		}
		else if(from_chess_no % 7 < to_chess_no% 7)
		{
			IsCurrent = false;
		}
	}
	return IsCurrent;
}
