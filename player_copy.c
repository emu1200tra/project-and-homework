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
  //printf("in to player: %s %s %s\n", argv[1] , argv[2] , argv[3]);
  
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
  
  //write(out , "before wr\n" , sizeof("before wr\n"));
  FILE* wr = fopen(filename2 , "a+");
  //printf("filename2 : %s\n" , filename2);
  //fflush(stdout);
  if(wr == NULL){
    perror("with wr error:");
  }
  //write(out , "before rd\n" , strlen("before rd\n"));
  //printf("filename : %s\n" , filename);
  //fflush(stdout);
  FILE* rd = fopen(filename , "r");
  //fcntl(fileno(rd) , F_SETFL , O_NONBLOCK);
  //FILE* rd = fopen("judge1_A.FIFO" , "r");
  //printf("filename : %s\n" , filename);
  //fflush(stdout);
  //printf("filename : %s\n" , filename);
  if(rd == NULL){
    perror("with rd error:");
  }
  //write(out , "done\n" , sizeof("done\n"));
  int number_choose = getnumber();
  //dup2(rd , STDIN_FILENO);
  //dup2(wr , STDOUT_FILENO);
  fprintf(wr , "%s %s %d\n", index , key , number_choose);  
  fflush(wr);
  struct  timeval    tv;
  gettimeofday(&tv,NULL);
  //printf("back to judge first:%s %s %d, time:%d\n" , index , key , number_choose , tv.tv_usec);
  //fflush(stdout);
  //fd_set fd;
  //FD_ZERO(&fd);
  //int check;
  //write(out , "before 19 loop\n" , sizeof("before 19 loop\n"));
  for(int i = 0 ; i < 19 ; i++){
    //printf("i:%d\n" , i);
    //fflush(stdout);
    //write(out , "check fflush\n" , sizeof("check fflush\n"));
    //FD_SET(STDIN_FILENO , &fd);
    //write(out , "select before\n" , sizeof("select before\n"));
    //int size = getdtablesize();
    //check = select(size+1 , &fd , NULL , NULL , NULL);
    //write(out , "select done\n" , sizeof("select done\n"));
    /*if(check == -1){
      perror("select");
    }*/
    //if(FD_ISSET(STDIN_FILENO , &fd)){
    //write(out , "before scanf\n" , strlen("before scanf\n"));
    fscanf(rd , "%d %d %d %d" , &choos_list[0] , &choos_list[1] , &choos_list[2] , &choos_list[3]);
    /*choos_list[0] = 1;
    choos_list[1] = 2;
    choos_list[2] = 3;
    choos_list[3] = 4;*/
    //printf("choose list: %d %d %d %d\n" , choos_list[0] , choos_list[1] , choos_list[2] , choos_list[3]);
    //fflush(stdout);
    //number_choose = choose_number(choos_list);
    number_choose = getnumber();
    //printf("before printf: %s %s %d\n" , index , key , number_choose);
    //fflush(stdout);
    fprintf(wr , "%s %s %d\n", index , key , number_choose);
    fflush(wr);
    //}
    /*else{
      i=i-1;
    }*/
    //write(out , "done one loop\n" , sizeof("done one loop\n"));
  }
  if(rd != NULL){
    fclose(rd);  
  }
  if(wr != NULL){
    fclose(wr);  
  }
  //unlink(filename);
  //unlink(filename2);
  return 0;
}