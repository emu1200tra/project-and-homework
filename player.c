#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

int getnumber(){
  struct  timeval    tv;
  gettimeofday(&tv,NULL);
  int r = tv.tv_usec%3;
  if(r == 0){
    return 1;
  }
  else if(r==1){
    return 3;
  }
  else{
    return 5;
  }
}

int choose_number(int array[4]){
  int record[3] = {0};
  for(int i = 0 ; i < 4 ; i++){
    if(array[i] == 1){
      record[0]++;
    }
    else if(array[i] == 3){
      record[1]++;
    }
    else if(array[i] == 5){
      record[2]++;
    }
  }
  int max = 0;
  for(int i = 0 ; i < 3 ; i++){
    if(record[i]>=max){
      max = record[i];
    }
  }
  if(max == 0){
    return 1;
  }
  else if(max == 1){
    return 3;
  }
  else if(max == 2){
    return 5;
  }
  else{
    return 5;
  }
}

int main(int argc , char* argv[]){
  fflush(stdout);
  char index[128];
  strcpy(index , argv[2]);
  int in = dup(0);
  int out = dup(1);
  char judge_id[128];
  strcpy(judge_id , argv[1]);
  char key[128];
  strcpy(key , argv[3]);
  int choos_list[4] = {0};
  char filename[512];
  char filename2[512];
  sprintf(filename , "judge%s_%s.FIFO" , judge_id , index);
  sprintf(filename2 , "judge%s.FIFO" , judge_id);
  
  FILE* wr = fopen(filename2 , "a+");
  if(wr == NULL){
    perror("with wr error:");
  }
  FILE* rd = fopen(filename , "r");
  if(rd == NULL){
    perror("with rd error:");
  }
  int number_choose = getnumber();
  fprintf(wr , "%s %s %d\n", index , key , number_choose);  
  fflush(wr);
  //printf("player:%s %s %d\n", index , key , number_choose);  
  //fflush(stdout);
  struct  timeval    tv;
  gettimeofday(&tv,NULL);
  for(int i = 0 ; i < 19 ; i++){
    fscanf(rd , "%d %d %d %d" , &choos_list[0] , &choos_list[1] , &choos_list[2] , &choos_list[3]);
    number_choose = getnumber();
    fprintf(wr , "%s %s %d\n", index , key , number_choose);
    fflush(wr);
  }
  if(rd != NULL){
    fclose(rd); 
    unlink(filename); 
  }
  if(wr != NULL){
    fclose(wr);
    unlink(filename2);  
  }
  return 0;
}