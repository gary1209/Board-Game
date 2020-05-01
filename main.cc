#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <map>
#include <vector>

#define DEBUG 0
#define NONE "\033[m"
#define RED  "\033[1;31m"
#define BLUE "\033[0;34m"
#define WHITE "\033[1;30m"
#define BLACK "\033[1;37m"

using namespace std;


typedef std::pair<int, int> PII;

static int UserFirst; // 0 or 1
static int BoardSize; // 4 or 6

enum Player {
  FIRST = 0,
  SECOND = 1,
  NOBODY = 2
};

Player who;

// 下一步要下的位置跟值
struct Position{
  int x;
  int y;
  int val;
  Position(){}
  Position(int xx,int yy,int vv):x(xx),y(yy),val(vv) {}
}N;

vector<Position> killed;

void place(Position next, bool msg);
void show_winner();
Player get_winner();

int board[6][6]; //下了哪個棋，0是還沒下，-1是已經爆掉
Player board2[6][6]; //誰下的，爆的保持顏色
std::vector<int> avai[2];
const std::vector<int> AVAI4 = {2,3,5,8,13};
const std::vector<int> AVAI6 = {2,2,3,3,5,5,8,8,8,13,13};

static inline bool inside(int x, int y){
  return x >=0 && y>=0 && x< BoardSize && y < BoardSize;
}

static inline bool check_end(){
  // Terminal-test function
  return (avai[0].empty() && avai[1].empty());
}

bool check(int a, int b){
  // return true if should mark
  if(board[a][b] <= 0) return false;
  int sum = 0;
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if(inside(a+i, b+j) && board[a+i][b+j] > 0) sum+=board[a+i][b+j];
    }
  }
  if(DEBUG) printf("(%d,%d) sum: %d\n", a, b, sum);
  if(DEBUG) printf("board[a][b]: %d\n", board[a][b]);
  return sum > 15;
}

void print_board(){
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] == 0) {
        printf(WHITE" O " NONE);
      } else if(board[i][j] == -1) {
        printf(BLACK" X " NONE);
      } else {
        printf("%s%2d " NONE, board2[i][j] == FIRST ? RED : BLUE, board[i][j]);
      }
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



bool check_avai(Position next){
  if(next.x < 0 || next.y < 0 || next.x >= BoardSize || next.y >=BoardSize) return false;
  if(board[next.x][next.y]!=0) return false;
  auto &v = avai[who];
  return std::find(v.begin(), v.end(), next.val) != v.end();
}

void undo(){
  who = (who == FIRST ? SECOND : FIRST);
  while(true) {
    auto v = killed.back();
    killed.pop_back();
    if (v.val < 0) {
      avai[who].push_back(board[v.x][v.y]);
      board[v.x][v.y] = 0;
      board2[v.x][v.y] = NOBODY;
      sort(avai[who].begin(), avai[who].end());
      break;
    }
    board[v.x][v.y] = v.val;
  }
}

int f3(int a, int b){
  int sum = 0;
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if(inside(a+i, b+j) && board[a+i][b+j] > 0) sum+=board[a+i][b+j];
    }
  }
  if(sum < 16){
    return 16 - sum;
  } else {
    return 0;
  }
}

int eval(){
  //enext.valuate the score for [who]
  int s1 = 0; //棋面上有多少張牌
  int s2 = 0; //棋面上總分
  int s3 = 0; //每一張牌離爆掉還有多少分的總和
  int s4 = 0; //手牌總和
  int w1 = 3;
  int w2 = 5;
  int w3 = 1;
  int w4 = 10;
  Player enemy = FIRST ? SECOND : FIRST;
  for(int i = 0; i < BoardSize; i++){
    for(int j = 0; j < BoardSize; j++){
      if(board[i][j] > 0){
        if(board2[i][j]== who){
          // puts("KKK");
          s1+=1;
          s2+=board[i][j];
          s3+=f3(i, j);
        } else  {
          s1-=1;
          // puts("III");
          s2-=board[i][j];
          s3-=f3(i, j);
        }
      }
    }
  }
  
  auto &p = avai[who];
  for(auto v : p) s4+=v;
  auto &k = avai[who == FIRST ? SECOND : FIRST];
  for(auto t : k) s4-=t;

  // puts("//////////////");
  // print_board();
  // printf("%d EVAL:%d\n",who,((w1 * s1) + (w2 * s2) + (w3 * s3) + (w4 * s4)));
  // puts("//////////////");
  return (w1 * s1) + (w2 * s2) + (w3 * s3) + (w4 * s4);
}


int negamax2(int level, int alpha, int beta){
  if(check_end()) {
    auto p = get_winner();
    if(p == who) return +1000000;
    else if(p == NOBODY) return 0;
    else return -1000000;
  }
  struct Position ker;
  if(level >= 3) return eval();
  // if(BoardSize == 4 && level >= 5) return eval();
  // if(BoardSize == 6 && level >= 9) return eval();

  int score = -2000000;
  auto copy = avai[who];

  for(int i = 0; i < BoardSize; i++){
    ker.x = i;
    for(int j = 0; j < BoardSize; j++){
      ker.y = j;
      for(auto p : copy){
        ker.val = p;
        if(check_avai(ker)) {
          place(ker, 0);
          // printf("level:%d alpha:%d beta:%d\n", level , alpha , beta);
          int tmp = -negamax2(level + 1, -beta, -alpha);
          // printf("tmp %d !!!\n", tmp);
          undo();
          if(tmp > score) {
            score = tmp;
            if(level == 0) N = ker;
            if (tmp > alpha) {
              if (tmp >= beta) break;
              alpha = tmp;
            }
          }
        }
      }
    }
  }
  // printf("HERE! level:%d alpha:%d beta:%d score:%d\n", level , alpha , beta, score);
  return score;
}

int step = 0;
bool look_up(){
  N.x = 0;
  N.y = 0;
  N.val = 8;
  if(check_avai(N)) return true;
  N.x = 0;
  N.y = BoardSize - 1;
  N.val = 8;
  if(check_avai(N)) return true;
  N.x = BoardSize - 1;
  N.y = 0;
  N.val = 8;
  if(check_avai(N)) return true;
  N.x = BoardSize - 1;
  N.y = BoardSize - 1;
  N.val = 8;
  if(check_avai(N)) return true;
  N.x = 0;
  N.y = 0;
  N.val = 5;
  if(check_avai(N)) return true;
  N.x = 0;
  N.y = BoardSize - 1;
  N.val = 5;
  if(check_avai(N)) return true;
  N.x = BoardSize - 1;
  N.y = 0;
  N.val = 5;
  if(check_avai(N)) return true;
  N.x = BoardSize - 1;
  N.y = BoardSize - 1;
  N.val = 5;
  if(check_avai(N)) return true;
  return false;
}

Position ai_search2(){
  if(step < 4) {
    step++;
    if(look_up()) return N;
  }
  // puts("kkk");
  negamax2(0, -1000000, 1000000);
  return N;
}

Position ai_search(){
  //random pick a place
  struct Position n;

  n.val = avai[who].front();
  while(true){
      n.x = rand() % BoardSize;
      n.y = rand() % BoardSize;
      if(check_avai(n)) break;
  }
  return n;
}


void place(Position next, bool msg){
  board[next.x][next.y] = next.val;
  board2[next.x][next.y] = who;
  auto &v = avai[who];
  v.erase(std::find(v.begin(), v.end(), next.val));
  if (msg) {
    if(isHuman()){
      printf("User : (%d,%d,%d)\n", next.x, next.y, next.val);
    }else{
      printf("AI : (%d,%d,%d)\n", next.x, next.y, next.val);
    }
  }
  killed.emplace_back(next.x, next.y, -1);
  auto s = killed.size();
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if(inside(next.x+i,next.y+j)&&check(next.x + i, next.y + j)){
        killed.emplace_back(next.x + i, next.y + j, board[next.x+i][next.y+j]);
      }
    }
  }
  for (; s < killed.size(); s++) {
    board[killed[s].x][killed[s].y] = -1;
  }
  who = (who == FIRST ? SECOND : FIRST);
}

Player get_winner(){
  int score[2] = {}, smax[2] = {};
    for(int i = 0; i < BoardSize; i++){
      for(int j = 0; j < BoardSize; j++){
        if(board[i][j] <= 0) continue;
        score[board2[i][j]]+=board[i][j];
        if(board[i][j] > smax[board2[i][j]]) smax[board2[i][j]] = board[i][j];
      }
    }
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
  return winner;  
}

void show_winner(){
  // Utility function
  
  printf("========END========\n");
  Player winner = get_winner();

  if (winner == NOBODY) {
    printf("Draw!\n");
  } else if ((winner == FIRST) == UserFirst) {
    printf("User Win!\n");
  } else {
    printf("AI Win!\n");
  }
  return;
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
    struct Position next;

    if(isHuman()){
      printf("Input (row, col, weight): ");
      scanf("%d%d%d",&next.x, &next.y, &next.val);
    }else{
      next = ai_search2();
      // printf("@@@ x:%d,y:%d,val:%d\n", next.x, next.y, next.val);
    }
    if(!check_avai(next)){
      puts("Not valid position!\n");
      // exit(0);
      // assert(isHuman());
      continue;
    }
    place(next, 1);
    print_board();
    if(check_end()) break;
    
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
