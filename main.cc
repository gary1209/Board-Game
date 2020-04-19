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

typedef std::pair<int, int> PII;

static int UserFirst;
static int BoardSize;

enum Player {
  FIRST = 0,
  SECOND = 1,
  NOBODY = 2
};

Player who;

// 下一步要下的位置跟值
int pos_x;
int pos_y;
int val;

int board[6][6];//下了哪個棋，0是還沒下，1是已經爆掉
Player board2[6][6];//誰下的
std::vector<int> avai[2];
const std::vector<int> AVAI4 = {2,3,5,8,13};
const std::vector<int> AVAI6 = {2,2,3,3,5,5,8,8,8,13,13};

static inline bool inside(int x, int y){
  return x >=0 && y>=0 && x< BoardSize && y < BoardSize;
}

bool check(int a, int b){
//return true if should mark
  int sum = 0;
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if(inside(a+i, b+j) && board[a+i][b+j] > 0) sum+=board[a+i][b+j];
    }
  }
  if(DEBUG) printf("(%d,%d) sum: %d\n", a, b, sum);
  if(DEBUG) printf("board[a][b]: %d\n", board[a][b]);
  if(sum <= 15) return false;
  if(board[a][b] == 0) return false;
  return true;
}

void print_board(){
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] == 0)printf(" O ");
      else if(board[i][j] == -1)printf(" X ");
      else if(board2[i][j] == FIRST) printf(BLUE"%2d " NONE,board[i][j]);
      else printf(RED"%2d " NONE, board[i][j]);

    }
    printf("\n");
  };
  printf("[User chess pieces]: [ ");
  for(auto p : avai[UserFirst ? FIRST : SECOND]){
    printf("%d ", p);
  }
  puts("]");
  
  printf("[  AI chess pieces]: [ ");
  for(auto p : avai[!UserFirst ? FIRST : SECOND]){
    printf("%d ", p);
  }
  puts("]");
}

bool isHuman(){
  return (who == FIRST) == UserFirst;
}



bool check_avai(){
  if(pos_x < 0 || pos_y < 0 || pos_x >= BoardSize || pos_y >=BoardSize) return false;
  if(board[pos_x][pos_y]!=0) return false;
  auto &v = avai[who];
  return std::find(v.begin(), v.end(), val) != v.end();
}

void ai_search(){
  val = avai[who].front();
  while(true){
      pos_x = rand() % BoardSize;
      pos_y = rand() % BoardSize;
      if(check_avai()) break;
  }
}

void place(){
  board[pos_x][pos_y] = val;
  board2[pos_x][pos_y] = who;
  auto &v = avai[who];
  v.erase(std::find(v.begin(), v.end(), val));
  if(isHuman()){
    printf("User : (%d,%d,%d)\n", pos_x, pos_y, val);
  }else{
    printf("AI : (%d,%d,%d)\n", pos_x, pos_y, val);
  }
  vector<PII> marked;
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if(check(pos_x + i, pos_y + j)){
        marked.emplace_back(pos_x + i, pos_y + j);
      }
    }
  }
  for(auto p : marked) {
    board[p.first][p.second] = -1;
    board2[p.first][p.second] = NOBODY;
  }
  marked.clear();
}

bool check_end(){
  return (avai[0].empty() && avai[1].empty());
}



void show_winner(){
  int score[2], smax[2] = {};
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] <= 0) continue;
      score[board2[i][j]]+=board[i][j];
      if(board[i][j] > smax[board2[i][j]]) smax[board2[i][j]] = board[i][j];
    }
  }
  
  printf("========END========\nFIRST score: %d\nSECOND score: %d\n", score[FIRST], score[SECOND]);
  Player winner;
  if (score[FIRST] > score[SECOND]) {
    winner = FIRST;
  } else if (score[FIRST] < score[SECOND]) {
    winner = SECOND;
  } else if (smax[FIRST] > smax[SECOND]) {
    winner = FIRST;
  } else if (smax[FIRST] < smax[SECOND]) {
    winner = SECOND;
  } else {
    winner = NOBODY;
  }
  if (winner == NOBODY) {
    printf("Draw!\n");
  } else if ((winner == FIRST) == UserFirst) {
    printf("User Win!\n");
  } else {
    printf("AI Win!\n");
  }
}

void play(){
  if(DEBUG){
    if(isHuman()) {
      printf(RED"It's human's turn!\n" NONE);
    }
    else{
      printf(RED"It's AI's turn\n" NONE);
    }
  }
  print_board();
  while(true){
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
    print_board();
    if(check_end()) break;
    who = (who == FIRST ? SECOND : FIRST);
  }
}

void init(){
  for(int i = 0; i < 6; i++){
    for(int j = 0; j < 6; j++){
      board[i][j] = 0;
      board2[i][j] = NOBODY;
    }
  }
  printf("User first? (0/1):");
  scanf("%d", &UserFirst);
  printf("Board Size? (4/6):");
  scanf("%d", &BoardSize);
  assert(UserFirst == 0 || UserFirst ==1);
  assert(BoardSize == 4 || BoardSize ==6);
  if(BoardSize == 4) {
    avai[0] = AVAI4;
    avai[1] = AVAI4;
  } else {
    avai[0] = AVAI6;
    avai[1] = AVAI6;
  }
  who = FIRST;
  if(DEBUG) printf(RED"%d %d\n" NONE, UserFirst, BoardSize);
}

int main(){
  srand(time(NULL));
  init();
  play();
  show_winner();
}


