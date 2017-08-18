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

int record[65536] = {0};
int ran_key(){
  int i = 0;
  srandom(time(NULL));
  int random_key = rand()%65535 + 1;
  return random_key;
}

typedef struct{
  char index[128];
  char key[128];
  int num;
}record_d;

record_d record_data[4];


int main(int argc , char *argv[]){
  char array[2];
  for(int i = 0 ; i < 4 ; i++){
    array[0] = 'A' + i;
    array[1] = '\0';
    strcpy(record_data[i].index , array);
    strcpy(record_data[i].key , "-1");
    record_data[i].num = 0;
  }
  //write(1 , "check1" , sizeof("check1"));
  int record_if_double[3] = {0};
  int record_die[4] = {0};
  int score[4] = {0};
  char r_fifo[128];
  int record_in = dup(0);
  int record_out = dup(1);
  sprintf(r_fifo , "judge%s.FIFO" , argv[1]);
  //write(1 , "check2" , sizeof("check2"));
  char w_fifoA[128] , w_fifoB[128] , w_fifoC[128] , w_fifoD[128];
  sprintf(w_fifoA , "judge%s_A.FIFO" , argv[1]);
  sprintf(w_fifoB , "judge%s_B.FIFO" , argv[1]);
  sprintf(w_fifoC , "judge%s_C.FIFO" , argv[1]);
  sprintf(w_fifoD , "judge%s_D.FIFO" , argv[1]);
  //write(1 , "check3" , sizeof("check3"));
  mkfifo(r_fifo , O_CREAT | 0700);
  mkfifo(w_fifoA , O_CREAT | 0700);
  mkfifo(w_fifoB , O_CREAT | 0700);
  mkfifo(w_fifoC , O_CREAT | 0700);
  mkfifo(w_fifoD , O_CREAT | 0700);
  //dprintf(1 , "filename in judge: %s\n%s\n%s\n%s\n%s\n" , r_fifo , w_fifoA , w_fifoB , w_fifoC , w_fifoD);
  //fflush(0);
  
  FILE* r_fd;
  //write(1 , "check4" , sizeof("check4"));
  FILE* w_fdA;
  FILE* w_fdB;
  FILE* w_fdC;
  FILE* w_fdD;
  //write(1 , "check5" , sizeof("check5"));
  int getinput[4];
  char id[128];
  strcpy(id , argv[1]);
  //write(1 , "check6\n" , sizeof("check6\n"));
  struct timeval time_s;
  time_s.tv_sec = 3;
  time_s.tv_usec = 0;
  while(1){
    for(int i = 0 ; i < 4 ; i++){
      record_data[i].num = 0;
      if(i != 4){
        record_if_double[i] = 0;
      }
      record_die[i] = 0;
      score[i] = 0;
    }
    //dup2(record_in , STDIN_FILENO);
    char buffer[256];
    read(0 , buffer , sizeof(buffer));
    //write(2 , buffer , strlen(buffer));
    //perror("before scanf\n");
    sscanf(buffer , "%d %d %d %d\n" , &getinput[0] , &getinput[1] , &getinput[2] , &getinput[3]);
    //dprintf(2 , "%d %d %d %d\n" , getinput[0] , getinput[1] , getinput[2] , getinput[3]);
    //fflush(0);
    if(getinput[0] == -1 && getinput[1] == -1 && getinput[2] == -1 && getinput[3] == -1){
      //write(2 , "break\n" , strlen("break\n"));
      //system("rm -f *.FIFO");
      break;
    }
    pid_t pid;
    
    for(char i = 0 ; i < 4 ; i++){
      //printf("check:key\n");
      //fflush(stdout);
      sprintf(record_data[i].key , "%d" , ran_key());
      pid=fork();
      if(pid==0){
        //printf("fork complete\n");
        //fflush(stdout);
        char tmp_char[128];
        sprintf(tmp_char , "%c" , 'A'+i);
        //printf("%s %s %s\n" , id , tmp_char , record_data[i].key);
        //fflush(stdout);
        int err = execl("./player" , "./player" , id , tmp_char , record_data[i].key , NULL);
        //printf("err:%dn" , err);
        fflush(stdout);
        exit(0);
      }
      else if(pid<0){
        perror("fork eror");
        exit(0);
      }
      //printf("this is parent\n");
      //fflush(stdout);
    }
    r_fd = fopen(r_fifo , "r");
    //fcntl(fileno(r_fd) , F_SETFL , O_NONBLOCK);
    w_fdA = fopen(w_fifoA , "w");
    w_fdB = fopen(w_fifoB , "w");
    w_fdC = fopen(w_fifoC , "w");
    w_fdD = fopen(w_fifoD , "w");
    //dup2(r_fd , STDIN_FILENO);
    //write(record_out , "check dup2\n" , sizeof("check dup2\n"));
    fd_set masterfds , fds;
    FD_ZERO(&masterfds);
    FD_ZERO(&fds);
    FD_SET(fileno(r_fd) , &masterfds);
    time_s.tv_sec = 3;
    time_s.tv_usec = 0;
    memcpy(&fds , &masterfds , sizeof(masterfds));
    if(select(1024 , &fds , NULL , NULL , NULL) == -1){
      exit(4);
    }
    if(FD_ISSET(fileno(r_fd) , &fds)){
      int tmp_num;
      char tmp_input1 , tmp_input2[128];
      char buffer[128];
      fgets(buffer , 128 , r_fd);
      sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
      if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
        perror("key error");
      }
      record_data[tmp_input1-'A'].num = tmp_num;
      fgets(buffer , 128 , r_fd);
      sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
      //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
      //fflush(stdout);
      //char buffer[1024];
      //read(fileno(r_fd) , buffer , sizeof(buffer));
      //printf("check fifo:%s\n", buffer);
      //fflush(stdout);
      if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
        perror("key error");
        //goto jump;
      }
      record_data[tmp_input1-'A'].num = tmp_num;
      fgets(buffer , 128 , r_fd);
      sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
      //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
      //fflush(stdout);
      //char buffer[1024];
      //read(fileno(r_fd) , buffer , sizeof(buffer));
      //printf("check fifo:%s\n", buffer);
      //fflush(stdout);
      if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
        perror("key error");
        //goto jump;
      }
      record_data[tmp_input1-'A'].num = tmp_num;
      fgets(buffer , 128 , r_fd);
      sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
      //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
      //fflush(stdout);
      //char buffer[1024];
      //read(fileno(r_fd) , buffer , sizeof(buffer));
      //printf("check fifo:%s\n", buffer);
      //fflush(stdout);
      if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
        perror("key error");
        //goto jump;
      }
      record_data[tmp_input1-'A'].num = tmp_num;
    }
      //write(record_out , "before jump\n" , sizeof("before jump\n"));
      //jump:
      //continue;
    //}
    //write(record_out , "pause\n" , sizeof("pause\n"));
    for(int i = 0 ; i < 4 ; i++){
      if(record_data[i].num == 1){
        record_if_double[0]++;
      }
      else if(record_data[i].num == 3){
        record_if_double[1]++;
      }
      else if(record_data[i].num == 5){
        record_if_double[2]++;
      }
      else{
        record_die[i] = 1;
      }
    }
    for(int i = 0 ; i < 4 ; i++){
      if(record_data[i].num == 1 && record_if_double[0] == 1){
        score[i]++;
      }
      else if(record_data[i].num == 3 && record_if_double[1] == 1){
        score[i] += 3;
      }
      else if(record_data[i].num == 5 && record_if_double[2] == 1){
        score[i] += 5;
      }
      else{
        score[i] += 0;
      }
    }
    for(int j = 1 ; j < 20 ; j++){
      char send[128];
      for(int k = 0 ; k < 4 ; k++){
        if(record_die[k] == 1){
          record_data[k].num = 0;
        }
      }
      sprintf(send , "%d %d %d %d\n" , record_data[0].num , record_data[1].num , record_data[2].num , record_data[3].num);
      //fflush(0);
      fprintf(w_fdA , "%s", send);
      fflush(w_fdA);
      fprintf(w_fdB , "%s", send);
      fflush(w_fdB);
      fprintf(w_fdC , "%s", send);
      fflush(w_fdC);
      fprintf(w_fdD , "%s", send);
      fflush(w_fdD);
      /*for(int k = 0 ; k < 4 ; k++){
          record_data[k].num = 0;
      }*/
      FD_ZERO(&masterfds);
      FD_ZERO(&fds);
      FD_SET(fileno(r_fd) , &masterfds);
    
    //write(record_out , "before loop\n" , sizeof("before loop\n"));
    //for(int counter = 0 ; counter < 4 ; counter++){
      time_s.tv_sec = 3;
      time_s.tv_usec = 0;
      memcpy(&fds , &masterfds , sizeof(masterfds));
      if(select(1024 , &fds , NULL , NULL , /*&time_s*/NULL) == -1){
        exit(4);
      }
      //put time into
      //write(record_out , "select complete\n" , sizeof("select complete\n"));
      struct timeval tv;
      gettimeofday(&tv,NULL);
      //printf("time:%d\n" , tv.tv_usec);
      //fflush(stdout);
      if(FD_ISSET(fileno(r_fd) , &fds)){
        //printf("isset complete\n");
        //f//flush(stdout);
        int tmp_num;
        char tmp_input1 , tmp_input2[128];
        char buffer[128];
        fgets(buffer , 128 , r_fd);
        sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
        //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
        //fflush(stdout);
        //char buffer[1024];
        //read(fileno(r_fd) , buffer , sizeof(buffer));
        //printf("check fifo:%s\n", buffer);
        //fflush(stdout);
        if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
          perror("key error");
          //goto jump;
        }
        record_data[tmp_input1-'A'].num = tmp_num;
        fgets(buffer , 128 , r_fd);
        sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
        //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
        //fflush(stdout);
        //char buffer[1024];
        //read(fileno(r_fd) , buffer , sizeof(buffer));
        //printf("check fifo:%s\n", buffer);
        //fflush(stdout);
        if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
          perror("key error");
          //goto jump;
        }
        record_data[tmp_input1-'A'].num = tmp_num;
        fgets(buffer , 128 , r_fd);
        sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
        //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
        //fflush(stdout);
        //char buffer[1024];
        //read(fileno(r_fd) , buffer , sizeof(buffer));
        //printf("check fifo:%s\n", buffer);
        //fflush(stdout);
        if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
          perror("key error");
          //goto jump;
        }
        record_data[tmp_input1-'A'].num = tmp_num;
        fgets(buffer , 128 , r_fd);
        sscanf(buffer , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
        //printf("%c %s %d\n" , tmp_input1 , tmp_input2 , tmp_num);
        //fflush(stdout);
        //char buffer[1024];
        //read(fileno(r_fd) , buffer , sizeof(buffer));
        //printf("check fifo:%s\n", buffer);
        //fflush(stdout);
        if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) != 0){
          perror("key error");
          //goto jump;
        }
        record_data[tmp_input1-'A'].num = tmp_num;
      }
      //write(record_out , "before jump\n" , sizeof("before jump\n"));
      //jump:
      //continue;
    //}
      for(int i = 0 ; i < 4 ; i++){
        if(record_die[i] == 0){
          if(record_data[i].num == 1){
            record_if_double[0]++;
          }
          else if(record_data[i].num == 3){
            record_if_double[1]++;
          }
          else if(record_data[i].num == 5){
            record_if_double[2]++;
          }
          else {
            record_die[i] = 1;
          }
        }
      }
      for(int i = 0 ; i < 4 ; i++){
        if(record_die[i] == 0){
          if(record_data[i].num == 1 && record_if_double[0] == 1){
            score[i]++;
          }
          else if(record_data[i].num == 3 && record_if_double[1] == 1){
            score[i] += 3;
          }
          else if(record_data[i].num == 5 && record_if_double[2] == 1){
            score[i] += 5;
          }
          else{
            score[i] += 0;
          }
        }
      }
    }
    for(int i = 0 ; i <= 2 ; i++){
      for(int j = 0 ; j <= 2-i ; j++){
        if(score[j] >= score[j+1]){
          int tmp = score[j];
          int tmp2 = getinput[j];
          score[j] = score[j+1];
          getinput[j] = getinput[j+1];
          score[j+1] = tmp;
          getinput[j+1] = tmp2;
        }
      }
    }
    int tmp = 0;
    for(int i = 0 ; i < 4 ; i++){
      if(i == 0){
        tmp = score[i];
        score[i] = 4-i; 
      }
      else{
        if(score[i] == tmp){
          tmp = score[i];
          score[i] = score[i-1];
        }
        else{
          tmp = score[i];
          score[i] = 4-i;
        }
      }
    }
    for(int i = 0 ; i <= 2 ; i++){
      for(int j = 0 ; j <= 2-i ; j++){
        if(getinput[j] >= getinput[j+1]){
          int tmp = score[j];
          int tmp2 = getinput[j];
          score[j] = score[j+1];
          getinput[j] = getinput[j+1];
          score[j+1] = tmp;
          getinput[j+1] = tmp2;
        }
      }
    }
    char send_to_big_judge[512];
    sprintf(send_to_big_judge , "%d %d\n%d %d\n%d %d\n%d %d\n" , getinput[0] , score[0] , getinput[1] , score[1] , getinput[2] , score[2] , getinput[3] , score[3]);
    write(record_out , send_to_big_judge , strlen(send_to_big_judge));
    for(int i = 0 ; i < 4 ; i++){
      int status;
      wait(&status);
    }
    record[atoi(record_data[0].key)] = 0;
    record[atoi(record_data[1].key)] = 0;
    record[atoi(record_data[2].key)] = 0;
    record[atoi(record_data[3].key)] = 0;
    fclose(r_fd);
    fclose(w_fdA);
    fclose(w_fdB);
    fclose(w_fdC);
    fclose(w_fdD);
  }
  close(record_in);
  close(record_out);
  close(0);
  close(1);
  unlink(r_fifo);
  unlink(w_fifoA);
  unlink(w_fifoB);
  unlink(w_fifoC);
  unlink(w_fifoD);
  //perror("going to die\n");
  return 0;
}