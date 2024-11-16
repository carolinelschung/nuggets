// Source code of map modlue for the Nuggets project
// CS50, 24F
// Team 10, Anna Filyurina, Nov. 2024

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<math.h>


/*
Helper function that determines the visibility of one point and returns true or false
*/
static bool isVisible(int x, int y, int ptX, int ptY, char* masterMap, const int NC);


/*
Function that takes in seed and based on it returns a player's starting coordinates on the masterMap.
*/
void map_player_init(char* masterMap, int* x, int* y, int* seed, const int NC, const int NR){
  int location = 0;
  
  if(seed == NULL){
    srand(getpid());
  }else{
    srand(*seed);
  }

  while(masterMap[location] != '.'){
    *x = rand() % NC;
    *y = rand() % NR;
    location = (*y)*NC + (*x);
  }
  // while (x, y) location is not a valid '.' on masterMap
  //   if seed is non null 
  //       x = srand(seed)
  //       y = srand(seed)
  //   else 
  //       x = srand(getpid())
  //       y = srand(getpid())
  // return
}

/*
Function that takes in a player's coordinates and the master map and outputs the visibleMap (only what the payer sees immediately)  

*/
void map_get_visible(int x, int y, char* masterMap, char* visibleMap, const int NC, const int NR){
  int length = strlen(masterMap);
  int ptX, ptY; // coordinaets of the point we want to determine the visibility of 
  for(int i = 0; i < length; i++){
    ptY = i/NC;
    ptX = i - ptY*NC;
    //printf("got to is visible function call, %d, %d, %d\n", i, NC, NR);
    //fflush(stdout);
    if(isVisible(x, y, ptX, ptY, masterMap, NC)){
      visibleMap[i] = masterMap[i];
    }else{
      visibleMap[i] = ' ';
    }
    //printf("visible %c and master %c\n", visibleMap[i], masterMap[i]);
    //fflush(stdout);
  }
  // for each character on the masterMap (0 to strlen(masterMap))
  //   calculate coordinates of the point in (ptX, ptY) form
  //   if isVisible(x, y, ptX, ptY, masterMap)
  //       visibleMap(i) = masterMap(i);
  //   else 
  //       visibleMap(i) = ' ';
}

static bool isVisible(int x, int y, int ptX, int ptY, char* masterMap, const int NC){
  float k; // k is the slope coefficient
  int yNew, xNew;
  int location;
  //printf("got to k  x %d y %d ptX %d ptY %d\n", x, y, ptX, ptY);
  //fflush(stdout);
  if(ptY == y){

    //printf("got to first for\n");
    //fflush(stdout);
    if(x - ptX < 0){
      for(int i = 1; i < abs(x - ptX); i++){
        yNew = y; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z'))){
          return false;
        }
      }
    }else{
      for(int i = -1; i > (-1)*(x - ptX); i--){
        yNew = y; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z'))){
          return false;
        }
      }
    }
    

    //printf("out of second for\n");
    //fflush(stdout);
    return true;

  }else if(ptX == x){
    //k = 1;

    //printf("got to second for\n");
    //fflush(stdout);


    if(y - ptY > 0){
      for(int i = -1; i > (-1)*(y - ptY); i--){
        yNew = y + i;
        xNew = x;
        location = (yNew)*NC + xNew;
        if(masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')){
          return false;
        }
      }
    }else{
      for(int i = 1; i < abs(y - ptY); i++){
        yNew = y + i; // the fractional part is discarded in this case
        xNew = x;
        location = (yNew)*NC + xNew;
        if(masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')){
          return false;
        }
      }
    }
    
    //printf("out of second for\n");
    //fflush(stdout);
    return true;

  }else{
    float deltaY, deltaX; // avoiding integer devision
    float intermediate;
    deltaY = ptY - y;
    deltaX = ptX - x;
    k = deltaY/deltaX;
    //printf("%d %d %d %d %f\n", ptX, ptY, x, y, k);


    if((x-ptX) > 0){
      //kSign = xSign*ySign;
      //printf("got to first for\n");
      //fflush(stdout);
      for(int i = -1; i > (-1)*(x - ptX); i--){
        intermediate = i*k;
        yNew = y + intermediate; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + NC] != '.' && masterMap[location + NC] != '*' && (masterMap[location+NC] < 'A' || masterMap[location+NC] > 'Z'))){
          return false;
        }
      }
    }else{
      for(int i = 1; i < abs(x - ptX); i++){
        intermediate = i*k;
        yNew = y + intermediate; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + NC] != '.' && masterMap[location + NC] != '*' && (masterMap[location+NC] < 'A' || masterMap[location+NC] > 'Z'))){
          return false;
        }
      }
    }

    if((y - ptY) > 0){
      //printf("got to second for\n");
      //fflush(stdout);
      for(int i = -1; i > (-1)*(y - ptY); i--){
        intermediate = i*(1/k);
        xNew = x + intermediate; // (round this value down)
        yNew = y + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if ((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + 1] != '.' && masterMap[location + 1] != '*'  && (masterMap[location+1] < 'A' || masterMap[location+1] > 'Z'))){
          return false;
        }
      }
      //printf("out of second for\n");
      //fflush(stdout);
    }else{
      //printf("got to second for\n");
      //fflush(stdout);
      for(int i = 1; i < abs(y - ptY); i++){
        intermediate = i*(1/k);
        xNew = x + intermediate; // (round this value down)
        yNew = y + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if ((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + 1] != '.' && masterMap[location + 1] != '*' && (masterMap[location+1] < 'A' || masterMap[location+1] > 'Z'))){
          return false;
        }
      }
      //printf("out of second for\n");
      //fflush(stdout);
    }
    
    return true;
  }
// calculate slope coefficient between the two point = k (i.e. rise/run)
// for(int i = 1; i < |x - ptX|; i++)
//     yNew = x + i*k (round this value down)
//     xNew = x + i
//     if both masterMap(xNew, yNew) and masterMap(xNew, yNew+1) are non '.'/letter/gold
//         return false
// for(int i = 1; i < |y - ptY|; i++)
//     xNew = Y + i*(1/k) (round this value down)
//     yNew = y + i
//     if both masterMap(xNew, yNew) and masterMap(xNew+1, yNew) are non '.'/letter/gold
//         return false

// return true
}

/*
Function that takes in the visible map and the payer's presious map and merges them omitting the gold from previous map. 

*/
void map_merge(char* playerMap, char* visibleMap){
  int length = strlen(playerMap);
  for(int i = 0; i < length; i++){
    if(playerMap[i] == '*' || (playerMap[i] >= 'A' && playerMap[i] <= 'Z')){
      playerMap[i] = '.';
    }
    if(visibleMap[i] != ' '){
      playerMap[i] = visibleMap[i];
    }
  }
  // for each character on the masterMap (0 to strlen(playerMap))
  //   erase gold or other player:
  //   if playerMap(i) == golr or other letter 
  //       playerMap(i) = '.'
  //   insert whatever is coming from the new visible map:
  //   if visibleMap != ' '
  //       playerMap(i) = visibleMap(i)
}
