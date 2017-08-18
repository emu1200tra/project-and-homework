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
  struct  timeval    tv;
  gettimeofday(&tv,NULL);
  int random_key = tv.tv_usec%65535 + 1;
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
  int record_if_double[3] = {0};
  int record_die[4] = {0};
  int score[4] = {0};
  char r_fifo[128];
  int record_in = dup(0);
  int record_out = dup(1);
  sprintf(r_fifo , "judge%s.FIFO" , argv[1]);
  char w_fifoA[128] , w_fifoB[128] , w_fifoC[128] , w_fifoD[128];
  sprintf(w_fifoA , "judge%s_A.FIFO" , argv[1]);
  sprintf(w_fifoB , "judge%s_B.FIFO" , argv[1]);
  sprintf(w_fifoC , "judge%s_C.FIFO" , argv[1]);
  sprintf(w_fifoD , "judge%s_D.FIFO" , argv[1]);
  FILE* r_fd;
  FILE* w_fdA;
  FILE* w_fdB;
  FILE* w_fdC;
  FILE* w_fdD;
  int getinput[4];
  char id[128];
  strcpy(id , argv[1]);
  while(1){
    for(int i = 0 ; i < 4 ; i++){
      record_data[i].num = 0;
      if(i != 4){
        record_if_double[i] = 0;
      }
      record_die[i] = 0;
      score[i] = 0;
    }

    mkfifo(r_fifo , O_CREAT | 0700);
    mkfifo(w_fifoA , O_CREAT | 0700);
    mkfifo(w_fifoB , O_CREAT | 0700);
    mkfifo(w_fifoC , O_CREAT | 0700);
    mkfifo(w_fifoD , O_CREAT | 0700);
    char buffer[256];
    read(0 , buffer , sizeof(buffer));
    sscanf(buffer , "%d %d %d %d\n" , &getinput[0] , &getinput[1] , &getinput[2] , &getinput[3]);
    if(getinput[0] == -1 && getinput[1] == -1 && getinput[2] == -1 && getinput[3] == -1){
      break;
    }
    pid_t pid;
    
    for(char i = 0 ; i < 4 ; i++){
      sprintf(record_data[i].key , "%d" , ran_key());
      pid=fork();
      if(pid==0){
        char tmp_char[128];
        sprintf(tmp_char , "%c" , 'A'+i);
        int err = execl("./player" , "./player" , id , tmp_char , record_data[i].key , NULL);
        exit(0);
      }
      else if(pid<0){
        perror("fork eror");
        exit(0);
      }
    }
    r_fd = fopen(r_fifo , "r");
    w_fdA = fopen(w_fifoA , "w");
    w_fdB = fopen(w_fifoB , "w");
    w_fdC = fopen(w_fifoC , "w");
    w_fdD = fopen(w_fifoD , "w");
    fd_set masterfds , fds;
    FD_ZERO(&masterfds);
    FD_ZERO(&fds);
    FD_SET(fileno(r_fd) , &masterfds);
    struct timeval tvbegin , tvend;
    for(int i = 0 ; i < 4 ; i++){
      memcpy(&fds , &masterfds , sizeof(masterfds));
      struct timeval tv;
      tv.tv_sec = 3;
      tv.tv_usec = 0;
      struct timeval tv1;
      tv1.tv_sec = 0;
      tv1.tv_usec = 1000;
      if(i != 3){
        select(1024 , &fds , NULL , NULL , &tv1); 
      }
      else{
        select(1024 , &fds , NULL , NULL , &tv);
      }
      if(FD_ISSET(fileno(r_fd) , &fds)){
        int tmp_num;
        char tmp_input1 , tmp_input2[128];
        char buffer1[512] , buffer2[512];
        memset(buffer1 , '\0' , 512);
        memset(buffer2 , '\0' , 512);
        read(fileno(r_fd) , buffer1 , sizeof(buffer1));
        int counter = 0 , counter2 = 0;
        while(buffer1[counter] != '\0'){
          buffer2[counter2] = buffer1[counter];
          counter2++;
          counter++;
          if(buffer1[counter] == '\n'){
            buffer2[counter2] = '\0';
            sscanf(buffer2 , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
            //dprintf(2 , "get from player:%c %s %d\n", tmp_input1 , tmp_input2 , tmp_num);
            //fflush(stderr);
            if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) == 0){
              record_data[tmp_input1-'A'].num = tmp_num;
              counter2 = 0;
              counter++;
            }
          }
        }
      }  
    }
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
        score[i] += 1;
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
        if(k != 3){
          record_if_double[k] = 0;
        }
      }
      sprintf(send , "%d %d %d %d\n" , record_data[0].num , record_data[1].num , record_data[2].num , record_data[3].num);
      fprintf(w_fdA , "%s", send);
      fflush(w_fdA);
      fprintf(w_fdB , "%s", send);
      fflush(w_fdB);
      fprintf(w_fdC , "%s", send);
      fflush(w_fdC);
      fprintf(w_fdD , "%s", send);
      fflush(w_fdD);
      for(int k = 0 ; k < 4 ; k++){
        record_data[k].num = 0;  
      }
      FD_ZERO(&masterfds);
      FD_ZERO(&fds);
      FD_SET(fileno(r_fd) , &masterfds);
      for(int i = 0 ; i < 4 ; i++){
        memcpy(&fds , &masterfds , sizeof(masterfds));
        struct timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        struct timeval tv1;
        tv1.tv_sec = 0;
        tv1.tv_usec = 1000;
        if(i != 3){
          select(1024 , &fds , NULL , NULL , &tv1);
        }
        else{
          select(1024 , &fds , NULL , NULL , &tv);
        }
        if(FD_ISSET(fileno(r_fd) , &fds)){
          int tmp_num;
          char tmp_input1 , tmp_input2[128];
          char buffer1[512] , buffer2[512];
          memset(buffer1 , '\0' , 512);
          memset(buffer2 , '\0' , 512);
          read(fileno(r_fd) , buffer1 , sizeof(buffer1));
          int counter = 0 , counter2 = 0;
          while(buffer1[counter] != '\0'){
            buffer2[counter2] = buffer1[counter];
            counter2++;
            counter++;
            if(buffer1[counter] == '\n'){
              buffer2[counter2] = '\0';
              sscanf(buffer2 , "%c %s %d" , &tmp_input1 , tmp_input2 , &tmp_num);
              //dprintf(2 , "get from player:%c %s %d\n", tmp_input1 , tmp_input2 , tmp_num);
              //fflush(stderr);
              if(strcmp(tmp_input2 , record_data[tmp_input1-'A'].key) == 0){
                if(record_die[tmp_input1-'A'] == 0){
                  record_data[tmp_input1-'A'].num = tmp_num;  
                  
                } 
                else{
                  i--;
                  if(i < 0){
                    i = 0;
                  }
                }         
                counter2 = 0;
                counter++;
              }
            }
          }
        }
      }
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
        else{
          score[i] += 0;
        }
      }
    }
    /*for(int i = 0 ; i < 4 ; i ++){
      printf("score%d:%d\n", i , score[i]);
      fflush(stdout);
    }*/
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
    write(1 , send_to_big_judge , strlen(send_to_big_judge));
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
    if(remove(r_fifo)<0){
    //perror("unlink fail");
    }
    if(remove(w_fifoA)<0){
      //perror("unlink2 fail");
    }
    if(remove(w_fifoB)<0){
      //perror("unlink3 fail");
    }
    if(remove(w_fifoC)<0){
      //perror("unlink4 fail");
    }
    if(remove(w_fifoD)){
      //perror("unlink5 fail");
    }
  }
  close(record_in);
  close(record_out);
  close(0);
  close(1);
  if(unlink(r_fifo)<0){
    //perror("unlink fail");
  }
  if(unlink(w_fifoA)<0){
    //perror("unlink2 fail");
  }
  if(unlink(w_fifoB)<0){
    //perror("unlink3 fail");
  }
  if(unlink(w_fifoC)<0){
    //perror("unlink4 fail");
  }
  if(unlink(w_fifoD)){
    //perror("unlink5 fail");
  }
  //perror("going to die\n");
  return 0;
}