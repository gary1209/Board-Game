#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>

#define DEBUG 1
#define NONE "\033[m"
#define RED  "\033[1;31m"
#define BLUE "\033[0;34m"

using namespace std;

//??static bool UserFirst;
static int UserFirst;
static int BoardSize;

enum turn {
  human = 2,
  ai    = 1,
  empty = 0,
};

turn who;

// 下一步要下的位置跟值
int pos_x;
int pos_y;
int val;

int board[6][6];//下了哪個棋，0是還沒下，1是打叉
turn board2[6][6];//誰下的
std::vector<int> u4 = {2,3,5,8,13};
std::vector<int> a4 = {2,3,5,8,13};
std::vector<int> u6 = {2,2,3,3,5,5,8,8,8,13,13};
std::vector<int> a6 = {2,2,3,3,5,5,8,8,8,13,13};


void print_board(){
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] == 0)printf("O ");
      else if(board[i][j] == -1)printf("X ");
      else if(board2[i][j] == human) printf(BLUE"%d " NONE,board[i][j]);
      else printf(RED"%d " NONE, board[i][j]);

    }
    printf("\n");
  };
  printf("[User chess pieces]: [ ");
  if(BoardSize == 4){
//    printf("size of vector: %lu\n",u4.size());
    for(auto t = u4.begin(); t != u4.end(); t++){
      printf("%d ", *t);
    }
  }
  if(BoardSize == 6){
    for(auto t = u6.begin(); t != u6.end(); t++){
      printf("%d ", *t);
    }
  }
  puts("]");
  
  printf("[  AI chess pieces]: [ ");
  if(BoardSize == 4){
    for(auto t = a4.begin(); t != a4.end(); t++){
      printf("%d ", *t);
    }
  }
  if(BoardSize == 6){
    for(auto t = a6.begin(); t != a6.end(); t++){
      printf("%d ", *t);
    }
  }
  puts("]");
}

bool isHuman(){
  if(who == human) return true;
  return false;
}

void ai_search(){
  pos_x = 0;
  pos_y = 0;
  val = 8;
}

bool check_avai(){
  if(pos_x >= BoardSize || pos_y >=BoardSize) return false;
  if(board[pos_x][pos_y]!=0) return false;
  if(BoardSize == 4){
    if(isHuman()){
      if(std::find(u4.begin(), u4.end(), val) != u4.end()){
        return true;
      }else{
        return false;
      }
    }else{
    //is AI
      if(std::find(a4.begin(), a4.end(), val) != a4.end()){
        return true;
      }else{
        return false;
      }
    }
  }else{
  //BoardSize == 6
    if(isHuman()){
      if(std::find(u6.begin(), u6.end(), val) != u6.end()){
        return true;
      }else{
        return false;
      }
    }else{
    //is AI
      if(std::find(a6.begin(), a6.end(), val) != a6.end()){
        return true;
      }else{
        return false;
      }
    }
  }
  return true;
}

void place(){
  board[pos_x][pos_y] = val;
  board2[pos_x][pos_y] = who;
  //刪除要下的棋
  if(isHuman() && BoardSize == 4){
    //puts("human && 4");
    for(int i = 0; i < u4.size(); i++){
      if(u4[i] == val) u4.erase(u4.begin()+i);
    }
  }
  if(isHuman() && BoardSize == 6){
    //puts("human && 6");
    for(int i = 0; i < u6.size(); i++){
      if(u6[i] == val) u6.erase(u6.begin()+i);
    }
  }
  if(!isHuman() && BoardSize == 4){
    //puts("AI && 4");
    for(int i = 0; i < a4.size(); i++){
      if(a4[i] == val) a4.erase(a4.begin()+i);
    }
  }
  if(!isHuman() && BoardSize == 6){
    //puts("AI && 6");
    for(int i = 0; i < a6.size(); i++){
      if(a6[i] == val) a6.erase(a6.begin()+i);
    }
  }
  if(isHuman())printf("User : (%d,%d,%d)\n", pos_x, pos_y, val);
  else printf("AI : (%d,%d,%d)\n", pos_x, pos_y, val);
}

bool check_end(){
  if(BoardSize == 4 && u4.size() == 0 && a4.size() == 0) return true;
  if(BoardSize == 6 && u6.size() == 0 && a6.size() == 0) return true;
  return false;
}

void check_and_mark(){
  return;
}

void show_winner(){
  return;
}

void play(){
  print_board();
  if(DEBUG){
    if(who == human) printf(RED"It's human's turn!\n" NONE);
    if(who == ai) printf(RED"It's AI's turn\n" NONE);
  }
  while(true){
    print_board();
    if(isHuman()){
      printf("Input (row, col, weight): ");
      scanf("%d%d%d",&pos_x, &pos_y, &val);
    }else{
      ai_search();
    }
    assert(check_avai());
    place();
    check_and_mark();
    if(check_end()) break;
    //change player
    if(isHuman()) who = ai;
    else who = human;
  }
}

void init(){
  printf("User first? (0/1):");
  scanf("%d", &UserFirst);
  printf("Board Size? (4/6):");
  scanf("%d", &BoardSize);
  assert(UserFirst == 0 || UserFirst ==1);
  assert(BoardSize == 4 || BoardSize ==6);
  if(UserFirst) who = human;
  else who = ai;
  if(DEBUG) printf(RED"%d %d\n" NONE, UserFirst, BoardSize);
}

int main(){
  init();
  play();
  show_winner();
}


