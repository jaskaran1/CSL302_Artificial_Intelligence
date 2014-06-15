/**
* This script searches the solution for an 8-puzzle problem through AI search techniques
* The blank tile is represented by the tile with the number 9
* The current search technique is a naive breadth first search
*/
//#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <list>//closed list of states for GBFS
#include <ctime>
#include <climits>
#define N 3         // NxN puzzle
#define MAXVALIDMOVES 4  // maximum number valid moves (4 for the center tile) UP,DOWN,LEFT,RIGHT
#define SOLDEPTH 100  // actual depth of the solution
//if there is a segmentation fault then increase the SOLDEPTH
//w=1 gives you uniform cost search.
//w=0 gives you greedy best first search
//w=0.5 gives you A*
float w=0.5;//Change value of w here
using namespace std;
clock_t begin,end;
double elapsed_time;
float minf;//variable used in IDA* to return min_f_val>flimit 
typedef struct      // coordinates of a single tile
{
    int i;
    int j;
} Location;

struct Node             // node of the search space.Each node is determined by the layout of the tiles
{
    int **layout;       // tile configuration.2D ARRAY
    int g_val;          // cost to reach this node
    int move;           // move from the parent to reach this node
    int h_val;		//heuristic value
    float f_val;		//f_val=g_val+h_val
    struct Node *parent;// pointer to the parent. We will use this for tracing back
};

// search queue elements
struct SearchQueueElement
{
    struct Node *nodeptr;               // pointer to the state in the search space
    struct SearchQueueElement *next;    // pointer to the next element in the search queue    
};

struct cmph//Functor for GBFS
{
 bool operator()(const SearchQueueElement* a,const SearchQueueElement* b)
 {
  return a->nodeptr->h_val<b->nodeptr->h_val;
 }
};
struct cmpf//Functor for Astar
{ 
 bool operator()(const SearchQueueElement* a,const SearchQueueElement* b)//Comparison function for A*
 {
  return a->nodeptr->f_val<b->nodeptr->f_val;
 }
};
multiset<struct SearchQueueElement*,cmph>GBFSQueue;
multiset<struct SearchQueueElement*,cmpf>ASTARQueue;
void SetGoal(int **a);       // Set the goal state of the puzzle
void PrintPuzzle(int **a);      // print the puzzle to the standard output
void MoveTile(int **a, Location x, int direction);     // move tile at location x along the direction
void Scramble(int **a);      // scramble the initial pattern moves number of times
void FindBlankTile(int **a, Location *blank);       // find the location of the blank tile
int IsValidMove(Location blank, int move);      // determine if a move is valid
int HeuristicMisplacedTiles(int **goal, int **a);   // compute the heuristic - number of misplaced tiles
int HeuristicManhattanDistance(int **goal, int **a);//computes the heuristic -manhattan distance
int GoalTest(int **goal, int **a);      // Test if the current state is a goal state
void PrintPath(int **a, int *path);     // print the path to the goal state
int * BFS(int **goal, int **a);      // breadth first search
int * DFS(int **goal, int **a);	     //depth first search.Max Depth is SOLDEPTH
int * GBFS(int **goal,int **a);//GBFS
int * ASTAR(int **goal,int **a,int choice);//ASTAR-choice-0 for misplacedtiles heuristic,1 for heuristicmanhattandistance
int * IDASTAR(int **goal,int **a);//IDASTAR just like DFS but limt is f_val
int counte,countc,maxdepth;//Performance metrics-count for number of nodes expanded/created,maxdepth in tree.
// search traversal functions
struct Node * CreateNode(int **goal,int **a);         // create a node with the required information
struct SearchQueueElement *CreateSearchQueueElement(struct Node *curnode);        // Create the search queue element
void AppendSearchQueueElementToEnd(struct SearchQueueElement* cursqelement);   //append a search queue element to the end of the queue.BFS
void AppendSearchQueueElementToFront(struct SearchQueueElement* cursqelement);//append a search queue element to the front of the queue.For DFS.
void FreeSearchMemory();
void UserInput(int **a);//for user input for the start state

// search variables
struct SearchQueueElement *head = NULL;//head of the queue in BFS is global
struct SearchQueueElement *tail=NULL;//for O(1) append
int main(int argc, char *argv[])
{
    int i, j;
    int **puzzle[6],**goal;       // puzzle variable.To store the current state of the puzzle for the 6 searches.We will be performing operations on this.Thus we need 6 2-d arrays.
    int *path=NULL;//stores the path from start to goal.Used for printing.

    // allocate memory to the variable that stores the puzzle.
    for(i=0;i<6;i++)
    	{puzzle[i]= (int **)malloc(sizeof(int *)*N);
    	 for(j=0;j<N;j++)
    	 	puzzle[i][j]=(int*)calloc(N,sizeof(int));
    	}
    goal = (int **)malloc(sizeof(int *)*N);
    
    for (i=0; i<N; i++)
         goal[i] = (int *)calloc(N, sizeof(int));
    // set the goal state the puzzle
    SetGoal(puzzle[0]);

    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            goal[i][j] = puzzle[0][i][j];

    printf("Goal state tile configuration for BFS:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    //Scramble(puzzle[0]);//Comment this when entering your own input configuration
    UserInput(puzzle[0]);//Uncomment this when entering your own input configuration.
    //Do remember that the input configuration shoud be an achievable configuration from the goal state
    for(i=0;i<N;i++)//copying the scrambled puzzle contents to all other puzzles
    	for(j=0;j<N;j++)
    	    for(int k=1;k<6;k++)
    	   	puzzle[k][i][j]=puzzle[0][i][j];
    printf("Start state tile configuration for BFS:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[0]);

    // perform breadth first search
    printf("Performing BFS\n");
    begin=clock();
    path = BFS(goal, puzzle[0]);//returns the list of moves connecting puzzle to goal.
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    // print path
    PrintPath(puzzle[0], path);//since puzzle is passed by reference all the changes are made to the puzzle to reach the goal node.
    // free memory
    FreeSearchMemory();
    for (i=0; i<N; i++)
         free(puzzle[0][i]);
    free(puzzle[0]);
    free(path);
//-------------------------------------------------------    
    printf("Goal state tile configuration for DFS:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    printf("Start state tile configuration for DFS:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[1]);
    printf("Performing DFS\n");
    begin=clock();
    path=DFS(goal, puzzle[1]);
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time Taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    PrintPath(puzzle[1], path);
    FreeSearchMemory();
    for (i=0; i<N; i++)
       free(puzzle[1][i]);
    free(puzzle[1]);
    free(path);
//-------------------------------------------------------Important point to note:
//GBFS can get stuck in loops since it is only expanding based on h_val.a small h_val can give larger h_val children.We need to create a discovered list of nodes   
    printf("Goal state tile configuration for GBFS:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    printf("Start state tile configuration for GBFS:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[2]);
    printf("Performing GBFS\n");
    begin=clock();
    path=GBFS(goal,puzzle[2]);
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time Taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    PrintPath(puzzle[2], path);
    for (i=0; i<N; i++)
       free(puzzle[2][i]);
    free(puzzle[2]);
    free(path);
//-------------------------------------------------------
    printf("Goal state tile configuration for ASTAR using misplaced tiles heuristic:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    printf("Start state tile configuration for ASTAR:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[3]);
    printf("Performing ASTAR using misplaced tiles heuristic\n");
    begin=clock();
    path=ASTAR(goal,puzzle[3],0);
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time Taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    PrintPath(puzzle[3], path);
    for (i=0; i<N; i++)
       free(puzzle[3][i]);
    free(puzzle[3]);
    free(path);
//---------------------------------------------------------
    printf("Goal state tile configuration for ASTAR using manhattan distance heuristic:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    printf("Start state tile configuration for ASTAR:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[4]);
    printf("Performing ASTAR using manhattan distance heuristic\n");
    begin=clock();
    path=ASTAR(goal,puzzle[4],1);
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time Taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    PrintPath(puzzle[4], path);
    for (i=0; i<N; i++)
       free(puzzle[4][i]);
    free(puzzle[4]);
    free(path);
//--------------------------------------------------------
    printf("Goal state tile configuration for IDASTAR:\n");
    // print the goal tile configuration
    PrintPuzzle(goal);
    printf("Start state tile configuration for IDASTAR:\n");
    // print the start state tile configuration
    PrintPuzzle(puzzle[5]);
    printf("Performing IDASTAR\n");
    begin=clock();
    path=IDASTAR(goal,puzzle[5]);
    end=clock();
    elapsed_time=double(end-begin)/CLOCKS_PER_SEC;
    printf("Number of nodes generated=%d,Number of nodes expanded=%d,Maximum Depth reached=%d,Time Taken=%lf\n",countc,counte,maxdepth,elapsed_time);
    if(path==NULL)
    	printf("Didn't find goal node for given f_limit\n");
    else	
    	PrintPath(puzzle[5], path);
    for (i=0; i<N; i++)
       free(puzzle[5][i]);
    free(puzzle[5]);
    free(path);        
    return(1);
}

void UserInput(int **a)
{printf("Set the start state for the game.It should be a valid configuration , achievable from the goal configuration or program will go in a loop:\n");
    int i, j;
    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            scanf("%d",&a[i][j]);
    return;

}

// This function sets the goal configuration of the tiles in the puzzle. Feel free to replace
// the code within the function to any create an goal layout of your liking.
// This will the goal state of the puzzle
void SetGoal(int **a)
{printf("Set the goal state for the game:\n");
    int i, j;
    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            scanf("%d",&a[i][j]);
    return;
}

// This function prints the 8 puzzle problem to the standard output. The blank tile
// is represented by the tile with the number 9
void PrintPuzzle(int **a)
{
    int i, j;

    for (i=0; i<N; i++)
    {
        for (j=0; j<N; j++)
            if (a[i][j] != N*N)
                printf("%d ", a[i][j]);
            else
                printf("  ");

        printf("\n");
    }
    return;
}


// This function moves the tile at location x, along the 4 possible directions
// 0 - left
// 1 - right
// 2 - top
// 3 - bottom
void MoveTile(int **a, Location x, int direction)
{
    int temp;

    // value of the tile at the source location
    temp = a[x.i][x.j];//this will be swapped with the neighbouring tile

    switch (direction)
    {
        // move tile left
    case 0:
        a[x.i][x.j] = a[x.i][x.j-1];
        a[x.i][x.j-1] = temp;
        break;
        // move tile right
    case 1:
        a[x.i][x.j] = a[x.i][x.j+1];
        a[x.i][x.j+1] = temp;
        break;
        // move tile top
    case 2:
        a[x.i][x.j] = a[x.i-1][x.j];
        a[x.i-1][x.j] = temp;
        break;
        // move tile bottom
    case 3:
        a[x.i][x.j] = a[x.i+1][x.j];
        a[x.i+1][x.j] = temp;
        break;
    }
    return;
}

// This function scrambles the initial tile configuration using valid moves.
// It keeps track of the location of the blank tile
// It moves the blank tile along any of the valid moves a fixed number of times
// specified by moves.
void Scramble(int **a)
{
    int i, j;
    Location blank;
    int nvalidmoves=0, *validmoves;
    int move=0;     // stores the move in the previous iteration.initialized to any value


    for (i=0; i<SOLDEPTH; i++)//this means that we're making SOLDEPTH number of moves.Hence,the solution is SOLDEPTH moves away from the goal state
    {
        // determine the initial location of the blank tile
        FindBlankTile(a, &blank);

        // list all valid moves
        // ignore the moves that cancel the previous move
        validmoves = (int *)calloc(MAXVALIDMOVES, sizeof(int));
        for (j=0; j<MAXVALIDMOVES; j++)
        {
            // ignore the moves that cancel previous move
            if ((j == 0 && move != 1) || (j == 1 && move != 0) ||
                    (j == 2 && move != 3) || (j == 3 && move != 2))
            {
                // consider only valid moves
                if (IsValidMove(blank, j)==1)
                {
                    validmoves[nvalidmoves] = j;//store the valid moves in a list
                    nvalidmoves++;
                }
            }
        }//this loop is used to create a list of valid moves from the current state

        // select a move at random
        //srand((unsigned)time(NULL));
        move = validmoves[rand() % nvalidmoves];//move is updated
        // perform the move
        MoveTile(a, blank, move);//perform move

        nvalidmoves = 0;//reinitialize for the next move
        free(validmoves);
    }
}

// This function determines the location of the blank tile in the puzzle
void FindBlankTile(int **a, Location *blank)
{
    int i, j;

    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            if (a[i][j] == N*N)
            {
                blank->i = i;
                blank->j = j;
                return;
            }
}

// This function determines if a move is valid
// returns 1 if move is valid else 0
int IsValidMove(Location blank, int move)//check boundary conditions
{
    switch (move)
    {
        // left
    case 0:
        if (blank.j-1 >= 0)
            return(1);
        break;
        // right
    case 1:
        if (blank.j+1 <= N-1)
            return(1);
        break;
        // top
    case 2:
        if (blank.i-1 >= 0)
            return(1);
        break;
        // bottom
    case 3:
        if (blank.i+1 <= N-1)
            return(1);
        break;
    }
    return(0);
}

// This function computes the number of misplaced tiles
int HeuristicMisplacedTiles(int **goal, int **a)
{
    int i, j;
    int h=0;

    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            if (goal[i][j] != a[i][j])
                h++;
    return(h);
}

// This function checks if the current state is the goal state
int GoalTest(int **goal, int **a)
{
    int i, j;

    for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            if (goal[i][j] != a[i][j])
                return(0);
    return(1);
}

// This function computes sum of Manhattan distance heuristic
int HeuristicManhattanDistance(int **goal, int **a)
{int h=0;
 int posgx,posgy,ax,ay;
 for(int k=1;k<=N*N;k++)//for every number k in bot grids find k's position
 	{
 	 for(int i=0;i<N;i++)
 		{for(int j=0;j<N;j++)
 			{if(a[i][j]==k)
 				{ax=i;ay=j;}
 			 if(goal[i][j]==k)
 				{posgx=i;posgy=j;}
 			}		
 		}
 	h+=abs(posgx-ax)+abs(posgy-ay);		
 		
 	}	
    // Fill in the code
    return(h);
}

void PrintPath(int **a, int *path)
{
    int i;
    Location blank;

    printf("path to the goal node: \n");
    PrintPuzzle(a);
    if(path==NULL)
    printf("Path is NULL\n");
    //printf("Halllo %d\n",path[0]);
    for (i=path[0]; i>0; i--)//path[0] is the length of path.We traverse path moves in opposite direction.Since the bfs gives path from goal to start.Thus,we have to read the path array from the end ie from root to goal. 
    {
        FindBlankTile(a, &blank);
        MoveTile(a, blank, path[i]);
        printf("move blank tile: ");
        switch (path[i])
        {
            case 0: printf("left");
                    break;
            case 1: printf("right");
                    break;
            case 2: printf("top");
                    break;
            case 3: printf("bottom");
                break;
        }
        printf("\n");
        PrintPuzzle(a);
    }
}

// This function performs breadth first search
int *BFS(int **goal, int **start)
{   counte=0;
    countc=0;
    maxdepth=0;
    head=NULL;
    int i;
    struct Node *curnode;
    struct SearchQueueElement *cursqelement, *temphead;
    Location blank;
    int *path = NULL;

    // create the root node of the search tree
    curnode = CreateNode(goal,start);

    // create the first element of the search queue
    if (head == NULL)
    {
        cursqelement = CreateSearchQueueElement(curnode);
        countc++;
        head = tail = cursqelement;
    }

    temphead = head;

    while(temphead != NULL)
    {   maxdepth=max(maxdepth,temphead->nodeptr->g_val);
        // check for goal
        if (GoalTest(goal, temphead->nodeptr->layout) == 1)
        {   
            // we have found a goal state!
            printf("goal state found at depth: %d\n", temphead->nodeptr->g_val);
            // path[0] - length of the path
            // path[1:path[0]] - the moves in the path
            path = (int *)malloc(sizeof(int));
            path[0] = 0;
            // traverse the path in the reverse order - from goal to root
            // while traversing, store the blank tile moves for each step
            curnode = temphead->nodeptr;
            while (curnode->parent != NULL)
            {
                path = (int *)realloc(path, sizeof(int)*(path[0]+2));
                path[path[0]+1] = curnode->move;
                path[0]++;
                curnode = curnode->parent;
            }
            return(path);
        }
        FindBlankTile(temphead->nodeptr->layout, &blank);
        // compute the children of the current node
        counte++;
        for (i=0; i<MAXVALIDMOVES; i++)
        {int mv=temphead->nodeptr->move;
         if (IsValidMove(blank, i) == 1 && (mv==-1 ||mv==0 && i!=1 || mv==1 && i!=0 || mv==2 && i!=3 || mv==3 && i!=2))//Part-1
            {
                curnode = CreateNode(goal,temphead->nodeptr->layout);//create child having copy of layout of parent node
                countc++;
                MoveTile(curnode->layout, blank, i);//move the blank tile in this child node
                curnode->move = i;//store the move which created the child from the parent
                curnode->g_val = temphead->nodeptr->g_val+1;
                curnode->parent = temphead->nodeptr;
                cursqelement = CreateSearchQueueElement(curnode);
                AppendSearchQueueElementToEnd(cursqelement);
            }
        }

        temphead = temphead->next;
    }
    return path;
}
int* DFS(int **goal, int **start)
{   counte=0;
    countc=0;
    maxdepth=0;
    head=NULL;
    int i;
    struct Node *curnode;
    struct SearchQueueElement *cursqelement, *temphead,*prevtemphead,*temp;//prevtemphead points to node just before temphead
    Location blank;
    int *path = NULL;

    // create the root node of the search tree
    curnode = CreateNode(goal,start);
    countc++;
    // create the first element of the search queue
    if (head == NULL)
    {
        cursqelement = CreateSearchQueueElement(curnode);
        head = tail = cursqelement;
    }

    temphead = head;
    while(temphead != NULL)
    {maxdepth=max(maxdepth,temphead->nodeptr->g_val);
     //PrintPuzzle(temphead->nodeptr->layout);
     //printf("\ndepth=%d,move from parent=%d",temphead->nodeptr->g_val,temphead->nodeptr->move);
     //printf("\n");
        // check for goal
        if (GoalTest(goal, temphead->nodeptr->layout) == 1)
        {
            // we have found a goal state!
            printf("goal state found at depth: %d\n", temphead->nodeptr->g_val);
            // path[0] - length of the path
            // path[1:path[0]] - the moves in the path
            path = (int *)malloc(sizeof(int));
            path[0] = 0;
            // traverse the path in the reverse order - from goal to root
            // while traversing, store the blank tile moves for each step
            curnode = temphead->nodeptr;
            while (curnode->parent != NULL)
            {
                path = (int *)realloc(path, sizeof(int)*(path[0]+2));
                path[path[0]+1] = curnode->move;
                path[0]++;
                curnode = curnode->parent;
            }
            return(path);
        }
        
        if(temphead->nodeptr->g_val==SOLDEPTH)//depth for recursion
        	{temphead=temphead->next;
        	 continue;}
        counte++;	
        FindBlankTile(temphead->nodeptr->layout, &blank);
        // compute the children of the current node
        int flag=0;
        for (i=0; i<MAXVALIDMOVES; i++)
        {int mv=temphead->nodeptr->move;
         if (IsValidMove(blank, i) == 1 && (mv==-1 ||mv==0 && i!=1 || mv==1 && i!=0 || mv==2 && i!=3 || mv==3 && i!=2))//Part-1
            {
                curnode = CreateNode(goal,temphead->nodeptr->layout);//create child having copy of layout of parent node
                countc++;
                MoveTile(curnode->layout, blank, i);//move the blank tile in this child node
                curnode->move = i;//store the move which created the child from the parent
                curnode->g_val = temphead->nodeptr->g_val+1;//g_val tells the depth of the node.use it to restrict depth.
                curnode->parent = temphead->nodeptr;
                cursqelement = CreateSearchQueueElement(curnode);
                AppendSearchQueueElementToFront(cursqelement);
                if(flag==0)//append operation for the first child
                	{prevtemphead=head;
                	 flag=1;
                	}
            }
        }
        //delete temphead since it has been expanded
        prevtemphead->next=temphead->next;
        temp=temphead;
        temphead = head;
        free(temp);}
  return path;
}
int* DFS_fval(int **goal, int **start,int depthlimit)//DFS is restricted by f_val rather than depth
{   int localcounte=0;
    int localcountc=0;
    int localmaxdepth=0;
    minf=INT_MAX;
    head=NULL;
    int i;
    struct Node *curnode,*parentcopy;
    //head is the top element of stack
    struct SearchQueueElement *cursqelement,*prevhead,*temp,*temphead,*copy;//prevtemphead points to node just before temphead
    Location blank;
    int *path = NULL;

    // create the root node of the search tree
    curnode = CreateNode(goal,start);
    curnode->h_val=HeuristicMisplacedTiles(goal,curnode->layout);
    curnode->f_val=curnode->g_val+curnode->h_val;
    localcountc++;
    // create the first element of the search queue
    if (head == NULL)
    {
        cursqelement = CreateSearchQueueElement(curnode);
        head = tail = cursqelement;
    }
    temphead=head;
    while(temphead != NULL)
    {localmaxdepth=max(localmaxdepth,temphead->nodeptr->g_val);
     //PrintPuzzle(temphead->nodeptr->layout);
     //printf("\ndepth=%d,move from parent=%d\n",temphead->nodeptr->g_val,temphead->nodeptr->move);
        // check for goal
        if (GoalTest(goal, temphead->nodeptr->layout) == 1)
        {   countc+=localcountc;
            counte+=localcounte;
            maxdepth=max(maxdepth,localmaxdepth);
            // we have found a goal state!
            printf("goal state found at depth: %d\n", temphead->nodeptr->g_val);
            // path[0] - length of the path
            // path[1:path[0]] - the moves in the path
            path = (int *)malloc(sizeof(int));
            path[0] = 0;
            // traverse the path in the reverse order - from goal to root
            // while traversing, store the blank tile moves for each step
            curnode = temphead->nodeptr;
            while (curnode->parent != NULL)
            {
                path = (int *)realloc(path, sizeof(int)*(path[0]+2));
                path[path[0]+1] = curnode->move;
                path[0]++;
                curnode = curnode->parent;
            }
            return(path);
        }
        FindBlankTile(temphead->nodeptr->layout, &blank);
        // compute the children of the current node
       
        //create a copy of parent node
        parentcopy=CreateNode(goal,temphead->nodeptr->layout);
        parentcopy->g_val=temphead->nodeptr->g_val;
        parentcopy->f_val=temphead->nodeptr->f_val;
        parentcopy->h_val=temphead->nodeptr->h_val;
        parentcopy->move=temphead->nodeptr->move;
        parentcopy->parent=temphead->nodeptr->parent;
        //delete the parent node from queue
        temp=head;
        head=head->next;
        free(temp);
        localcounte++;
        for (i=0; i<MAXVALIDMOVES; i++)
        {int mv=parentcopy->move;
         if (IsValidMove(blank, i) == 1 && (mv==-1 ||mv==0 && i!=1 || mv==1 && i!=0 || mv==2 && i!=3 || mv==3 && i!=2))//Part-1
            {
                curnode = CreateNode(goal,parentcopy->layout);//create child having copy of layout of parent node
                MoveTile(curnode->layout, blank, i);//move the blank tile in this child node
                curnode->parent = parentcopy;
                curnode->move = i;//store the move which created the child from the parent
                curnode->g_val = parentcopy->g_val+1;//g_val tells the depth of the node.use it to restrict depth.
                curnode->h_val=HeuristicMisplacedTiles(goal,curnode->layout);
                curnode->f_val=curnode->g_val+curnode->h_val;
    		if(curnode->f_val<=depthlimit)//condition for entry in the queue
                       {
                        localcountc++;	
                        cursqelement = CreateSearchQueueElement(curnode);
                	AppendSearchQueueElementToFront(cursqelement);
                	}
                else//curnode->f_val>depthlimit
                	{minf=min(minf,curnode->f_val);
                	 for (int j=0; j<N; j++)
    				free(curnode->layout[j]);
                         free(curnode->layout);
                	 free(curnode);
                	}       	
            }
        }
        temphead=head;
       }
  countc+=localcountc;
  counte+=localcounte;
  maxdepth=max(maxdepth,localmaxdepth);
  return path;
                         
}

int* GBFS(int** goal,int** start)//need a closed list.using a list of states.I'll have to check all the states in the list.
{   countc=0;counte=0;
    maxdepth=0;
    int i;
    struct Node *curnode;
    struct SearchQueueElement *cursqelement,*temphead;
    Location blank;
    int *path = NULL;
    list<int**>closed;//list of layouts seen
    // create the root node of the search tree
    list<int**>::iterator itlist;
    curnode = CreateNode(goal,start);
    curnode->h_val=HeuristicMisplacedTiles(goal,curnode->layout);//Replace it with Manhattan
    countc++;
    // create the first element of the search queue
    GBFSQueue.clear();//just to be sure.We clear all the elements
    cursqelement = CreateSearchQueueElement(curnode);
    GBFSQueue.insert(cursqelement);
    multiset<struct SearchQueueElement*,cmph>::iterator it;
    while(!GBFSQueue.empty())
    {   it=GBFSQueue.begin();
        temphead=*it;
        maxdepth=max(maxdepth,temphead->nodeptr->g_val);
        GBFSQueue.erase(it);
        //PrintPuzzle(temphead->nodeptr->layout);
        //printf("\nh_val=%d,move from parent=%d\n",temphead->nodeptr->h_val,temphead->nodeptr->move);
        //Push the node in list to mark it as seen
        if (GoalTest(goal, temphead->nodeptr->layout) == 1)
        {
            printf("goal state found at depth: %d\n", temphead->nodeptr->g_val);
            path = (int *)malloc(sizeof(int));
            path[0] = 0;
            curnode = temphead->nodeptr;
            while (curnode->parent != NULL)
            {
                path = (int *)realloc(path, sizeof(int)*(path[0]+2));
                path[path[0]+1] = curnode->move;
                path[0]++;
                curnode = curnode->parent;
            }
            return(path);
        }
        closed.push_back(temphead->nodeptr->layout);//pointer to the layout stored in the list.thus marked explored
        //printf("size=%lu\n",closed.size());
        FindBlankTile(temphead->nodeptr->layout, &blank);
        // compute the children of the current node
        counte++;
        for (i=0; i<MAXVALIDMOVES; i++)
        {int mv=temphead->nodeptr->move;
         if (IsValidMove(blank, i) == 1 && (mv==-1 ||mv==0 && i!=1 || mv==1 && i!=0 || mv==2 && i!=3 || mv==3 && i!=2))//Part-1
            {
                curnode = CreateNode(goal,temphead->nodeptr->layout);//create child having copy of layout of parent node
                countc++;
                MoveTile(curnode->layout, blank, i);//move the blank tile in this child node
                curnode->move = i;//store the move which created the child from the parent
                curnode->g_val = temphead->nodeptr->g_val+1;
                curnode->h_val=HeuristicMisplacedTiles(goal,curnode->layout);
                curnode->parent = temphead->nodeptr;
                cursqelement = CreateSearchQueueElement(curnode);//We are creating these elements and are storing pointers pointing to them in the queue
                bool repeat=0;
                //printf("repeatbef=%d\n",repeat);	
                for(itlist=closed.begin();itlist!=closed.end();itlist++)
                	{//PrintPuzzle(*itlist);
                	 if(HeuristicMisplacedTiles(*itlist,curnode->layout)==0)//This done to check if repeat or not.
                		{repeat=1;
                		 break;}
                	}
                //printf("repeat=%d\n",repeat);	
                if(!repeat)		 
                	GBFSQueue.insert(cursqelement);
                else//delete the created node
                	{for (int j=0; j<N; j++)
    				free(curnode->layout[j]);
                         free(curnode->layout);
                	 free(curnode);
                	 free(cursqelement);
                	
                	}	
             }
          // printf("in loop\n");
           
        }
    }
    return(path);
}

int* ASTAR(int** goal,int** start,int choice)
{   counte=0;
    countc=0;
    maxdepth=0;
    int i;
    struct Node *curnode;
    struct SearchQueueElement *cursqelement,*temphead;
    Location blank;
    int *path = NULL;

    // create the root node of the search tree
    curnode = CreateNode(goal,start);
    int(*foo)(int**,int**);//function pointer
    if(choice==0)
    	foo=HeuristicMisplacedTiles;
    else
    	foo=HeuristicManhattanDistance;	
    curnode->h_val=foo(goal,curnode->layout);
    curnode->f_val=w*curnode->g_val+(1.-w)*curnode->h_val;
    countc++;
    // create the first element of the search queue
    ASTARQueue.clear();//just to be sure.We clear all the elements
    cursqelement = CreateSearchQueueElement(curnode);
    ASTARQueue.insert(cursqelement);
    multiset<struct SearchQueueElement*,cmpf>::iterator it;
    while(!ASTARQueue.empty())
    {   it=ASTARQueue.begin();
        temphead=*it;
        ASTARQueue.erase(it);
        maxdepth=max(maxdepth,temphead->nodeptr->g_val);
        // check for goal
        if (GoalTest(goal, temphead->nodeptr->layout) == 1)
        {   // we have found a goal state!
            printf("goal state found at depth: %d having f_val=%f and h_val=%d\n", temphead->nodeptr->g_val,temphead->nodeptr->f_val,temphead->nodeptr->h_val);
            // path[0] - length of the path
            // path[1:path[0]] - the moves in the path
            path = (int *)malloc(sizeof(int));
            path[0] = 0;
            // traverse the path in the reverse order - from goal to root
            // while traversing, store the blank tile moves for each step
            curnode = temphead->nodeptr;
            while (curnode->parent != NULL)
            {
                path = (int *)realloc(path, sizeof(int)*(path[0]+2));
                path[path[0]+1] = curnode->move;
                path[0]++;
                curnode = curnode->parent;
            }
            return(path);
        }
        FindBlankTile(temphead->nodeptr->layout, &blank);
        // compute the children of the current node
        counte++;
        for (i=0; i<MAXVALIDMOVES; i++)
        {int mv=temphead->nodeptr->move;
         if (IsValidMove(blank, i) == 1 && (mv==-1 ||mv==0 && i!=1 || mv==1 && i!=0 || mv==2 && i!=3 || mv==3 && i!=2))//Part-1
            {
                curnode = CreateNode(goal,temphead->nodeptr->layout);//create child having copy of layout of parent node
                countc++;
                MoveTile(curnode->layout, blank, i);//move the blank tile in this child node
                curnode->move = i;//store the move which created the child from the parent
                curnode->g_val = temphead->nodeptr->g_val+1;
                curnode->h_val=foo(goal,curnode->layout);
                curnode->f_val=w*curnode->g_val+(1.0-w)*curnode->h_val;
                //curnode->f_val=max(curnode->g_val+curnode->h_val,temphead->nodeptr->f_val);//pathmax correction
                curnode->parent = temphead->nodeptr;
                cursqelement = CreateSearchQueueElement(curnode);
                ASTARQueue.insert(cursqelement);
            }
        }
    }
}
int* IDASTAR(int **goal,int** start)//default value for f_val
{  //:An important point is that the current node when expanded can give children whose f_val exceeds limit but the goal node has an f_val which is less than that limit.That is it isn't strictly increasing.
   counte=0;
   countc=0;
   maxdepth=0;
   struct Node *curnode;
   curnode=CreateNode(goal,start);
   curnode->h_val=HeuristicMisplacedTiles(goal,curnode->layout);
   curnode->f_val=curnode->g_val+curnode->h_val;             
   int i=curnode->f_val;
   	while(1)
   	{//printf("For f_val=%d\n",i);
   	 int* path=NULL;
   	 path=DFS_fval(goal,start,i);
   	 FreeSearchMemory();
   	 //if(i>=SOLDEPTH)
   	 //	return path;
   	 if(path!=NULL)
   	 	return path;
   	 else
   	 	i=minf;//the f_value of node just exceeding threshhold	
   	}
   //printf("GAMEOVER\n");	
   return NULL;	
}
// This function creates a node variable. Copies the contents of the layout of the node,
// computes the heuristic values
struct Node *CreateNode(int**goal,int **a)
{
    int i, j;
    struct Node *curnode;

    curnode = (struct Node *)malloc(sizeof(struct Node));
    curnode->layout = (int **)malloc(sizeof(int*)*N);
    for (i=0; i<N; i++)
    {
        curnode->layout[i] = (int *)malloc(sizeof(int)*N);
        for (j=0; j<N; j++)
            curnode->layout[i][j] = a[i][j];
    }
    curnode->parent = NULL;
    curnode->g_val = 0;
    curnode->move = -1;
    return(curnode);
}

// This function creates a search queue element
struct SearchQueueElement *CreateSearchQueueElement(struct Node *curnode)
{
    struct SearchQueueElement *cursqelement;

    cursqelement = (struct SearchQueueElement*)malloc(sizeof(struct SearchQueueElement));
    cursqelement->nodeptr = curnode;
    cursqelement->next = NULL;

    return(cursqelement);

}

// This function appends a search queue element to the end of the queue - for breadth first search
void AppendSearchQueueElementToEnd(struct SearchQueueElement* cursqelement)//O(1)
{
    struct SearchQueueElement *tempsqelement;
    if (head != NULL)
    {
        tail->next = cursqelement;
        tail=cursqelement;
    }
    else
        head = tail = cursqelement;
}

void AppendSearchQueueElementToFront(struct SearchQueueElement* cursqelement)//O(1)
{
  if (head != NULL)
    {cursqelement->next=head;
     head=cursqelement;}
  else
     head = tail = cursqelement;
 
}

void FreeSearchMemory()
{
    int i;
    struct SearchQueueElement *cursqelement, *tempsqelement;
    struct Node *curnode;

    cursqelement = head;
    while (cursqelement != NULL)
    {
        tempsqelement = cursqelement;
        cursqelement = cursqelement->next;
        curnode = tempsqelement->nodeptr;
        free(tempsqelement);
        for (i=0; i<N; i++)
            free(curnode->layout[i]);
        free(curnode->layout);
        free(curnode);
    }
    head = NULL;
}
