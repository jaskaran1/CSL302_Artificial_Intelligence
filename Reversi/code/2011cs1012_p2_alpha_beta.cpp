#include<iostream>
#include<cstdio>
#include<ctime>
#include<cstdlib>
#include<vector>
#include<utility>
#include<cctype>
#include<algorithm>
#include<climits>
#include<cfloat>
#define PLYLIMIT 11//at ply limit 17 1min for every move
using namespace std;
float Alpha_Beta_Max_Value(char grid[8][8],int ply,bool validmin,float& alpha,float& beta);
float Alpha_Beta_Min_Value(char grid[8][8],int ply,bool validmax,float& alpha,float& beta);
clock_t begin,end;//for timekeeping
double elapsed_time=0.0;
float w1=1;//weight for weighted_disc_count
float w2=1;//weight for disc_count
float w3=1;//weight for mobility heuristic
//Always an odd number
//Works fast till ply level 7
char grid[8][8];//global
int weight[8][8]={{8,1,6,6,6,6,1,8},
		  {1,0,3,3,3,3,0,1},
		  {6,3,5,4,4,5,3,6},
		  {6,3,4,5,5,4,3,6},
		  {6,3,4,5,5,4,3,6},
		  {6,3,5,4,4,5,3,6},
		  {1,0,3,3,3,3,0,1},
		  {8,1,6,6,6,6,1,8}
		 };
int numberofmoves=0;//stores the total number of moves made by the CPU
int generated=0;//number of nodes generated
int explored=0;//number of nodes explored
int pruned=0;//number of nodes pruned
void printboard()
{printf("   ");
 for(int i=0;i<8;i++)
	printf("%c  ",'a'+i);
 printf("\n\n");	
 for(int i=0;i<8;i++)
	{printf("%d  ",i);
	 for(int j=0;j<8;j++)
		printf("%c  ",grid[i][j]);
	 printf("%d\n\n",i);
	 //printf("\n");
	 }
 printf("   ");
 for(int i=0;i<8;i++)
	printf("%c  ",'a'+i);
 printf("\n\n");	 	
}
//---------------------------
void ShowScore(int& h,int& c)
{int human=0;
 int cpu=0;
 for(int i=0;i<8;i++)
	for(int j=0;j<8;j++)
		if(grid[i][j]=='B')
			human++;
		else if(grid[i][j]=='W')
			cpu++;
 h=human;
 c=cpu;
 printf("------SCORE------\nHUMAN=%d\nCPU=%d\n",human,cpu);
}
//----------------------------- 
bool ValidateAndUpdate(char grid[8][8],int r,int c,int player,bool type)//type=0->only checking no update,type=1->check and update
//lastmove is a vector stores the update made to the game board
{char oppvalue;//if player=1(B) oppvalue is W
 char value;
 if(player==1)//Human
 	{oppvalue='W';
 	 value='B';}
 else//Computer
 	{value='W';
 	 oppvalue='B';}
 if(grid[r][c]==' ')
	{//checking left
	 int j,k,pos,pos2;
	 bool move=false;
	 for(j=c-1;j>=0 && grid[r][j]==oppvalue;j--);
	 if(j>=0&&grid[r][j]==value && j<c-1)//atleast one oppvalue is there
	 	{move=true;
	 	 pos=j;
	 	 if(type)
	 	 	{for(j=c;j>pos;j--)
	 	 		grid[r][j]=value;
	 	 	}
	 	}
	 //checking right
	 for(j=c+1;j<8 && grid[r][j]==oppvalue;j++);
	 if(j<8&&grid[r][j]==value &&j>c+1)//atleast one oppvalue is there
	 	{move=true;
	 	 pos=j;
	 	 if(type)
	 	 	{for(j=c;j<pos;j++)
	 	 		grid[r][j]=value;
	 	 	}
	 	 }
	 //checking up
	 for(j=r-1;j>=0 && grid[j][c]==oppvalue;j--);
	 if(j>=0&&grid[j][c]==value && j<r-1)//atleast one oppvalue is there
	 	{move=true;
	 	 pos=j;
	 	 if(type)
	 	 	{for(j=r;j>pos;j--)
	 	 		grid[j][c]=value;
	 	 		}
	 	}
	 //checking down	
	 for(j=r+1;j<8 && grid[j][c]==oppvalue;j++);
	 if(j<8&&grid[j][c]==value&&j>r+1)//atleast one oppvalue is there
	 	{move=true;
	 	 pos=j;
	 	 if(type)
	 	 	{for(j=r;j<pos;j++)
	 	 		 grid[j][c]=value;
	 	 	}
	 	}
	 //check top left diagonal
	 for(j=r-1,k=c-1;j>=0 && k>=0 && grid[j][k]==oppvalue;j--,k--);
	 if(j>=0&&k>=0&&grid[j][k]==value &&j<r-1)
	 	{move=true;
	 	 pos=j;
	 	 pos2=k;
	 	 if(type)
	 	 	{for(j=r,k=c;j>pos && k>pos2;j--,k--)
	 	 		grid[j][k]=value;
	 	 		}
	 	 }	
	 //check bottom right diagonal		
	 for(j=r+1,k=c+1;j<8 && k<8 && grid[j][k]==oppvalue;j++,k++);
	 if(j<8&&k<8&&grid[j][k]==value &&j>r+1)
	 	{move=true;
	 	 pos=j;
	 	 pos2=k;
	 	 if(type)
	 	 	{for(j=r,k=c;j<pos && k<pos2;j++,k++)
	 	 		grid[j][k]=value;
	 	 		}
	 	}
	 //check top right diagonal
	 for(j=r-1,k=c+1;j>=0 && k<8 && grid[j][k]==oppvalue;j--,k++);
	 if(j>=0&&k<8&&grid[j][k]==value &&j<r-1)
	 	{move=true;
	 	 pos=j;
	 	 pos2=k;
	 	 if(type)
	 	 	{for(j=r,k=c;j>pos && k<pos2;j--,k++)
	 	 		 grid[j][k]=value;
	 	 	}
	 	}	
	 //check bottom left diagonal
	 for(j=r+1,k=c-1;j<8 && k>=0 && grid[j][k]==oppvalue;j++,k--);
	 if(j<8&&k>=0&&grid[j][k]==value&&j>r+1)
	 	{move=true;
	 	 pos=j;
	 	 pos2=k;
	 	 if(type)
	 	 	{for(j=r,k=c;j<pos && k>pos2;j++,k--)
	 			 grid[j][k]=value;
	 			}
	 	}	
	 return move;
	 }
 return false;	
}
//--------------------------
//void GAMEOVER()
//{printf()
//}

bool CPUMoveRandom()
{//Create a list of valid moves
 vector< pair<int,int> >validCPUmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,2,0))
 			validCPUmoves.push_back(make_pair(i,j));}
 	}
 int size=validCPUmoves.size();
 if(size==0)
 	{printf("CPU doesn't have any valid moves left\n");
 	 return 0;}
 //Random move	 		
 pair<int,int>move=validCPUmoves[rand()%size];
 ValidateAndUpdate(grid,move.first,move.second,2,1);
 printf("CPU played:%d%c\n",move.first,move.second+'a');
 return 1;	 
}
//--------------------List of Heuristic Functions------------------------------------------
//----cpu is the max player and human is the min player----------------------------
int disc_count(char grid[8][8])//or coin parity
{int human=0;
 int cpu=0;
 for(int i=0;i<8;i++)
	{for(int j=0;j<8;j++)
		{if(grid[i][j]=='B')
			human++;
		 else if(grid[i][j]=='W')
			cpu++;
		}
	}
 return 100*(cpu-human)/(cpu+human);}

int weighted_disc_count(char grid[8][8])//Static weights heurstic function
{int human=0;
 int cpu=0;
 for(int i=0;i<8;i++)
	{for(int j=0;j<8;j++)
		{
		if(grid[i][j]=='B')
			 human+=weight[i][j];
		else if(grid[i][j]=='W')
			 cpu+=weight[i][j];
			
			
		}
	}	
 return 100*(cpu-human)/(cpu+human);
 }

int mobility(char grid[8][8])//actual mobility heuristic function
{//valid moves for human
 int validhuman=0;
 int validcpu=0;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,1,0))
 			validhuman++;
 		 if(ValidateAndUpdate(grid,i,j,2,0))
 		 	validcpu++;	
 		}
 	}
 if(validhuman+validcpu!=0)
 	return 100*(validcpu-validhuman)/(validcpu+validhuman);
 else
 	return 0;		
 
}

//int stability(char grid[8][8])
//{}
 
float Max_Value(char grid[8][8],int ply,bool validmin);//predeclaration

float Min_Value(char grid[8][8],int ply,bool validmax)//Human plays optimally by playing the min-move.
{if(ply>=PLYLIMIT)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid); 
 vector< pair<int,int> >validHUMANmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,1,0))
 			validHUMANmoves.push_back(make_pair(i,j));
 		}
 	}
 int size=validHUMANmoves.size();
 float v=FLT_MAX;
 char work[8][8];
 for(int i=0;i<size;i++)
 	{pair<int,int>move=validHUMANmoves[i];
 	 for(int j=0;j<8;j++)
 	 	for(int k=0;k<8;k++)
 	 		work[j][k]=grid[j][k];
 	 ValidateAndUpdate(work,move.first,move.second,1,1);//the move is applied
 	 v=min(v,Max_Value(work,ply+1,1));
 	 }
 if(size==0 && validmax==0)//recursion can't continue further.reached the terminal state.
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid);	 	 	
 if(size==0)//call Max_Value
 	return Max_Value(grid,ply+1,0);
 return v;}
 
 
float Max_Value(char grid[8][8],int ply,bool validmin)//CPU plays optimally by playing the max-move.
{if(ply>=PLYLIMIT)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid); 
 vector< pair<int,int> >validCPUmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,2,0))
 			validCPUmoves.push_back(make_pair(i,j));
 		}
 	}
 int size=validCPUmoves.size();
 float v=-FLT_MAX;
 char work[8][8];
 for(int i=0;i<size;i++)
 	{pair<int,int>move=validCPUmoves[i];
 	 for(int j=0;j<8;j++)
 	 	for(int k=0;k<8;k++)
 	 		work[j][k]=grid[j][k];
 	 ValidateAndUpdate(work,move.first,move.second,2,1);//the move is applied
 	 v=max(v,Min_Value(work,ply+1,1));}
 if(size==0 && validmin==0)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid);
 if(size==0)
 	return Min_Value(grid,ply+1,0);		 	 	
 return v;}

bool CPUMoveMiniMax()
{vector< pair<int,int> >validCPUmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,2,0))
 			validCPUmoves.push_back(make_pair(i,j));}
 	}
 int size=validCPUmoves.size();
 //for(int i=0;i<size;i++)
 //	printf("(%d,%d) ",validCPUmoves[i].first,validCPUmoves[i].second);
 //printf("\n");
 if(size==0)
 	{printf("CPU doesn't have any valid moves left\n");
 	 return 0;}
 char work[8][8];//Our work grid.
 float ans=-FLT_MAX;
 pair<int,int>maxadvantagemove;
 int ply=0;
 for(int i=0;i<size;i++)//the base loop for minimax
 	{pair<int,int>move=validCPUmoves[i];//the move to be applied
 	 for(int j=0;j<8;j++)//creates the child node caused by the current move
 	 	for(int k=0;k<8;k++)
 			work[j][k]=grid[j][k];
 
 	 ValidateAndUpdate(work,move.first,move.second,2,1);//the move is applied
 	 float x=Min_Value(work,ply+1,1);//minimax  value of the child
 	 //printf("value of child=%f\n",x);
 	 if(x>ans)//finding the max value since cpu is the max player
 	 	{ans=x;
 	 	 maxadvantagemove=move;
 	 	}
 	 }
 if(ans==-FLT_MAX)
 printf("something is wrong\n");	 
 ValidateAndUpdate(grid,maxadvantagemove.first,maxadvantagemove.second,2,1);
 printf("CPU played:%d%c\n",maxadvantagemove.first,maxadvantagemove.second+'a');
 return 1;	 
	  
}
//-------------------------------------Alpha-Beta pruning-------------------------------------
float Alpha_Beta_Max_Value(char grid[8][8],int ply,bool validmin,float& alpha,float& beta);

float Alpha_Beta_Min_Value(char grid[8][8],int ply,bool validmax,float& alpha,float& beta)//Human plays optimally by playing the min-move.
{if(ply>=PLYLIMIT)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid)+w3*mobility(grid);//human is the min player 
 vector< pair<int,int> >validHUMANmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,1,0))
 			validHUMANmoves.push_back(make_pair(i,j));
 		}
 	}
 int size=validHUMANmoves.size();
 float v=FLT_MAX;
 char work[8][8];
 explored++;
 for(int i=0;i<size;i++)
 	{pair<int,int>move=validHUMANmoves[i];
 	 for(int j=0;j<8;j++)
 	 	for(int k=0;k<8;k++)
 	 		work[j][k]=grid[j][k];
 	 ValidateAndUpdate(work,move.first,move.second,1,1);//the move is applied
 	 generated++;
 	 v=min(v,Alpha_Beta_Max_Value(work,ply+1,1,alpha,beta));
 	 if(v<=alpha)
 	 	{pruned++;
 	 	 return v;}
 	 beta=min(beta,v);	
 	 }
 if(size==0 && validmax==0)//recursion can't continue further.reached the terminal state.
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid)+w3*mobility(grid);	 	 	
 if(size==0)//call Max_Value
 	return Alpha_Beta_Max_Value(grid,ply+1,0,alpha,beta);
 return v;
}
float Alpha_Beta_Max_Value(char grid[8][8],int ply,bool validmin,float& alpha,float& beta)
{if(ply>=PLYLIMIT)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid)+w3*mobility(grid); 
 
 vector< pair<int,int> >validCPUmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,2,0))
 			validCPUmoves.push_back(make_pair(i,j));
 		}
 	}
 int size=validCPUmoves.size();
 float v=-FLT_MAX;
 char work[8][8];
 explored++;
 for(int i=0;i<size;i++)
 	{pair<int,int>move=validCPUmoves[i];
 	 for(int j=0;j<8;j++)
 	 	for(int k=0;k<8;k++)
 	 		work[j][k]=grid[j][k];
 	 ValidateAndUpdate(work,move.first,move.second,2,1);//the move is applied
 	 generated++;
 	 v=max(v,Alpha_Beta_Min_Value(work,ply+1,1,alpha,beta));
 	 if(v>=beta)
 	 	{pruned++;
 	 	 return v;}
 	 alpha=max(alpha,v);}
 if(size==0 && validmin==0)
 	return w1*weighted_disc_count(grid)+w2*disc_count(grid)+w3*mobility(grid);
 if(size==0)
 	return Alpha_Beta_Min_Value(grid,ply+1,0,alpha,beta);		 	 	
 return v;
 }
 
 
bool CPUMoveAlphaBeta()
{vector< pair<int,int> >validCPUmoves;
 explored++;
 float alpha=-FLT_MAX;//keeps the max value
 float beta=FLT_MAX;//keeps the min value
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,2,0))
 			validCPUmoves.push_back(make_pair(i,j));}
 	}
 int size=validCPUmoves.size();
 if(size==0)
 	{printf("CPU doesn't have any valid moves left\nFinal grid configuration\n");
 	 return 0;}
 char work[8][8];//Our work grid.
 float ans=-FLT_MAX;
 pair<int,int>maxadvantagemove;
 int ply=0;
 for(int i=0;i<size;i++)//the base loop for minimax
 	{pair<int,int>move=validCPUmoves[i];//the move to be applied
 	 for(int j=0;j<8;j++)//creates the child node caused by the current move
 	 	for(int k=0;k<8;k++)
 			work[j][k]=grid[j][k];
 
 	 ValidateAndUpdate(work,move.first,move.second,2,1);//the move is applied
 	 generated++;
 	 float x=Alpha_Beta_Min_Value(work,ply+1,1,alpha,beta);//minimax  value of the child
 	 if(x>ans)//we are trying to find the maximum value
 	 	{ans=x;
 	 	 maxadvantagemove=move;
 	 	}
 	 }
 ValidateAndUpdate(grid,maxadvantagemove.first,maxadvantagemove.second,2,1);
 printf("CPU played:%d%c\n",maxadvantagemove.first,maxadvantagemove.second+'a');
 return 1;
 }
//--------------------------------------------------------------------------------


bool HumanMove()
{vector< pair<int,int> >validHUMANmoves;
 for(int i=0;i<8;i++)
 	{for(int j=0;j<8;j++)
 		{if(ValidateAndUpdate(grid,i,j,1,0))
 			validHUMANmoves.push_back(make_pair(i,j));}
 	}
 int size=validHUMANmoves.size();
 if(size==0)
 	{printf("HUMAN doesn't have any valid moves left\n");
 	 return 0;}
 //Show human all the valid moves
 printf("Set of Valid Moves\n");
 for(int i=0;i<size;i++)
 	printf("(%d,%c) ",validHUMANmoves[i].first,'a'+validHUMANmoves[i].second);
 printf("\n");	
 return 1;}


int main()
{for(int i=0;i<8;i++)
	for(int j=0;j<8;j++)
		grid[i][j]=' ';
 grid[3][3]=grid[4][4]='W';//Human player(1).
 grid[3][4]=grid[4][3]='B';//CPU(2).
 printboard();
 int human,cpu;
 ShowScore(human,cpu);
 bool last=0;
 while(1)//Game loop
 {bool hum=HumanMove();//check if move for human exists or not
  if(hum==0 && last==0)//the CPU didn't have any move from the previous iteration
  	{printboard();
  	 break;}
  if(hum==1)//human has a move left to play	 
  {printf("Enter move (x,y) where x is the row number and y is the column letter\n");
   int x,y;
   char c;
   scanf("%d%c",&x,&c);
   if(isalpha(c))
  	c=tolower(c);
   else
  	{printf("The row number and column alphabet shouldn't have any whitespace character between them\n");
  	 return -1;
  	}	
   y=c-'a';
   if(!ValidateAndUpdate(grid,x,y,1,1))
  	{printf("Invalid move\n");
  	 continue;}
   ShowScore(human,cpu);//Updates as well as shows score	 
   }
  printf("CPU making move...\n");	 
  bool CPU;
  begin=clock();
  //CPU=CPUMoveMiniMax();//CPU makes moves using minimax algorithm
  CPU=CPUMoveAlphaBeta();
  end=clock();
  elapsed_time+=double(end-begin)/CLOCKS_PER_SEC;
  printf("Time taken by CPU move=%lf\n",double(end-begin)/CLOCKS_PER_SEC);
  if(CPU==0&&hum==0)
  	{printboard();
  	 break;}
  if(CPU==1)//if CPU has made a move	 
  {numberofmoves++;	 
   printboard();	 
   ShowScore(human,cpu);}
  last=CPU;//last move of CPU stored to check for no possible move or not.	
  
 }
 printf("--------------------Game Over---------------------\n");
 printf("Total number of moves played by the CPU:%d\n",numberofmoves); 
 printf("Average time taken/move by the CPU:%lf\n",elapsed_time/numberofmoves);
 printf("Total Nodes generated %d\n",generated);
 printf("Total Nodes explored %d\n",explored);
 printf("Total Nodes pruned %d\n",pruned);
 printf("Weight value w1=%f,w2=%f,w3=%f\n",w1,w2,w3);
 if(human>cpu)
 	printf("You win\n");
 else if(human<cpu)
 	printf("Computer wins\n");
 else
 	printf("Draw\n");
 printf("Human->%d Computer->%d\n",human,cpu);			
 return 0;		
}
