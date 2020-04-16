#include <map>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>

#define DEBUG 0
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

int board[6][6];//下了哪個棋，0是還沒下，1是已經爆掉
turn board2[6][6];//誰下的
std::vector<int> u4 = {2,3,5,8,13};
std::vector<int> a4 = {2,3,5,8,13};
std::vector<int> u6 = {2,2,3,3,5,5,8,8,8,13,13};
std::vector<int> a6 = {2,2,3,3,5,5,8,8,8,13,13};


void print_board(){
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] == 0)printf(" O ");
      else if(board[i][j] == -1)printf(" X ");
      else if(board2[i][j] == human) printf(BLUE"%2d " NONE,board[i][j]);
      else printf(RED"%2d " NONE, board[i][j]);

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

void ai_search(){
  if(BoardSize == 4) val = a4.front();
  if(BoardSize == 6) val = a6.front();
  while(true){
    if(BoardSize == 4){
      int min = 0;
      int max = 3;
      pos_x = rand() % 4;
      pos_y = rand() % 4;
      if(check_avai()) break;
    }
    if(BoardSize == 6){
      int min = 0;
      int max = 6;
      pos_x = rand() % 6;
      pos_y = rand() % 6;
      if(check_avai()) break;
    }
  }
}

void place(){
  board[pos_x][pos_y] = val;
  board2[pos_x][pos_y] = who;
  //刪除要下的棋
  if(isHuman() && BoardSize == 4){
    for(int i = 0; i < u4.size(); i++){
      if(u4[i] == val) u4.erase(u4.begin()+i);
    }
  }
  if(isHuman() && BoardSize == 6){
    for(int i = 0; i < u6.size(); i++){
      if(u6[i] == val) u6.erase(u6.begin()+i);
    }
  }
  if(!isHuman() && BoardSize == 4){
    for(int i = 0; i < a4.size(); i++){
      if(a4[i] == val) a4.erase(a4.begin()+i);
    }
  }
  if(!isHuman() && BoardSize == 6){
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

bool inside(int x, int y){
  if(x >=0 && y>=0 && x< BoardSize && y < BoardSize) return true;
  return false;
}

bool check(int a, int b){
//return true if should mark
  int sum = 0;
  if(inside(a+1,b+1) && board[a+1][b+1] > 0) sum+=board[a+1][b+1];
  if(inside(a+1,b)   && board[a+1][b]   > 0) sum+=board[a+1][b];
  if(inside(a+1,b-1) && board[a+1][b-1] > 0) sum+=board[a+1][b-1];
  if(inside(a,b+1)   && board[a][b+1]   > 0) sum+=board[a][b+1];
  if(inside(a,b-1)   && board[a][b-1]   > 0) sum+=board[a][b-1];
  if(inside(a,b)     && board[a][b]     > 0) sum+=board[a][b];
  if(inside(a-1,b+1) && board[a-1][b+1] > 0) sum+=board[a-1][b+1];
  if(inside(a-1,b-1) && board[a-1][b-1] > 0) sum+=board[a-1][b-1];
  if(inside(a-1,b)   && board[a-1][b]   > 0) sum+=board[a-1][b];
  if(DEBUG) printf("(%d,%d) sum: %d\n", a, b, sum);
  if(DEBUG) printf("board[a][b]: %d\n", board[a][b]);
  if(sum <= 15) return false;
  if(board[a][b] == 0) return false;
  if(DEBUG) printf("!!!(%d,%d) sum: %d\n", a, b, sum);

  return true;
}

struct pos{
  int x;
  int y;
}ker;

void check_and_mark(){
  vector<pos> marked;
  
  if(check(pos_x - 1, pos_y - 1)) {
    ker.x = pos_x - 1;
    ker.y = pos_y - 1;
    marked.push_back(ker);
  }
  if(check(pos_x + 1, pos_y - 1)) {
    ker.x = pos_x + 1;
    ker.y = pos_y - 1;
    marked.push_back(ker);
  }
  if(check(pos_x    , pos_y - 1)) {
    ker.x = pos_x;
    ker.y = pos_y - 1;
    marked.push_back(ker);
  }
  if(check(pos_x - 1, pos_y + 1)) {
    ker.x = pos_x - 1;
    ker.y = pos_y + 1;
    marked.push_back(ker);
  }
  if(check(pos_x    , pos_y + 1)) {
    ker.x = pos_x;
    ker.y = pos_y + 1;
    marked.push_back(ker);
  }
  if(check(pos_x + 1, pos_y + 1)) {
    ker.x = pos_x + 1;
    ker.y = pos_y + 1;
    marked.push_back(ker);
  }
  if(check(pos_x - 1, pos_y    )) {
    ker.x = pos_x - 1;
    ker.y = pos_y;
    marked.push_back(ker);
  }
  if(check(pos_x    , pos_y    )) {
    ker.x = pos_x;
    ker.y = pos_y;
    marked.push_back(ker);
  }
  if(check(pos_x + 1, pos_y    )) {
    ker.x = pos_x + 1;
    ker.y = pos_y;
    marked.push_back(ker);
  }
  
  //map<int, int>::iterator iter;
  // for(iter = marked.begin(); iter != marked.end(); iter++){
  //   printf("board[%d][%d] => X\n", iter->first, iter->second);
  //   board[iter->first][iter->second] = -1;
  //   marked.erase(iter);
  // }
  // assert(marked.empty());
  vector <pos>::iterator iter;
  for(iter=marked.begin(); iter!=marked.end(); iter++){
    board[iter->x][iter->y] = -1;
  }
  marked.clear();
  return;
}

void show_winner(){
  int ai_max = 0;
  int ai_score = 0;
  int user_max = 0;
  int user_score = 0;
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] != -1 && board2[i][j] == human){
        user_score+=board[i][j];
        if(board[i][j] > user_max) user_max = board[i][j];
      }
      if(board[i][j] != -1 && board2[i][j] == ai){
        ai_score+=board[i][j];
        if(board[i][j] > ai_max) ai_max = board[i][j];
      }
    }
  }
  print_board();
  if(1) printf("========END========\nAI's score: %d\nUser's score: %d\n", ai_score, user_score);
  if(ai_score > user_score) printf("AI Win!\n");
  if(ai_score < user_score) printf("User Win!\n");
  if(ai_score == user_score){
    if(ai_max > user_max) printf("AI Win!\n");
    if(ai_max < user_max) printf("User Win!\n");
    if(ai_max == user_max) printf("Draw!\n");
  }
  return;
}

void play(){
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
    if(!check_avai()){
      puts("Not valid position!\n");
      continue;
    }
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
  srand(time(NULL));
  init();
  play();
  show_winner();
}


