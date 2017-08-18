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

int main(int argc , char*argv[]){
  pid_t pid;
  //int record_output;
  //record_output = dup(1);
  int judge_num = atoi(argv[1]);
  //printf("judge num: %d\n", judge_num);
  int player_num = atoi(argv[2]);
  //printf("player_num: %d\n" , player_num);
  int judge_list[13] = {0};
  int fd_list_ptoc[13][2];
  int fd_list_ctop[13][2];
  int score[21] = {0};
  int score_place_record[21];
  for(int i = 0 ; i < 21 ; i++){
    score_place_record[i] = i;
  }
  for(int i = 1 ; i <= judge_num ; i++){
    pipe(fd_list_ctop[i]);
    pipe(fd_list_ptoc[i]);
    if(pid=fork()==0){
      close(fd_list_ptoc[i][1]);
      close(fd_list_ctop[i][0]);
      dup2(fd_list_ctop[i][1] , 1);
      //close(fd_list_ctop[i][1]);
      dup2(fd_list_ptoc[i][0] , 0);
      //close(fd_list_ptoc[i][0]);
      char send[128];
      sprintf(send , "%d" , i);
      execlp("./judge" , "./judge" , send , NULL);
      exit(0);
    }
    else if(pid<0){
      write(1 , "fork error\n" , sizeof("fork error\n"));
    }
    else{
      close(fd_list_ctop[i][1]);
      close(fd_list_ptoc[i][0]);
      //fcntl(fd_list_ctop[0] , F_SETFL , O_NONBLOCK);
    }
  }
  //write(1 , "fork complete\n" , sizeof("fork complete\n"));
  for(int cp1 = 1 ; cp1 <= player_num-3 ; cp1++){
    for(int cp2 = cp1+1 ; cp2 <= player_num-2 ; cp2++){
      for(int cp3 = cp2+1 ; cp3 <= player_num-1 ; cp3++){
        for(int cp4 = cp3+1 ; cp4 <= player_num ; cp4++){
          int sign = 0;
          while(1){
            for(int j = 1 ; j <= judge_num ; j++){
              if(judge_list[j] == 0){
                char buffer[256];
                sprintf(buffer , "%d %d %d %d\n" , cp1 , cp2 , cp3 , cp4);
                //write(1 , buffer , strlen(buffer));
                write(fd_list_ptoc[j][1] , buffer , strlen(buffer));
                //write(1 , "write down complete\n" , sizeof("write down complete\n"));
                sign = 1;
                judge_list[j] = -1;
              }
            }
            for(int j = 1 ; j <= judge_num ; j++){
              if(judge_list[j]==-1){
                //dup2(fd_list_ctop[j][0] , STDIN_FILENO);
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(fd_list_ctop[j][0] , &fds);
                select(1024 , &fds , NULL , NULL , 0);
                if(FD_ISSET(fd_list_ctop[j][0] , &fds)){
                  int input1[4] , input2[4];
                  //write(1 , "into select\n" , sizeof("into select\n"));
                  char buffer[256];
                  read(fd_list_ctop[j][0] , buffer , sizeof(buffer));
                  sscanf(buffer , "%d %d\n%d %d\n%d %d\n%d %d\n" , &input1[0] , &input2[0]  , &input1[1] , &input2[1] , &input1[2] , &input2[2] , &input1[3] , &input2[3]);
                  for(int counter = 0 ; counter < 4 ; counter++){
                    //printf("%d %d\n" , input1[counter] , input2[counter]);
                    //fflush(stdout);
                    if(input2[counter] == 1){
                      score[input1[counter]] += 3;
                    }
                    else if(input2[counter] == 2){
                      score[input1[counter]] += 2;
                    }
                    else if(input2[counter] == 3){
                      score[input1[counter]] += 1;
                    }
                    else{
                      score[input1[counter]] += 0;
                    }
                  }
                  judge_list[j] = 0;
                }
              }
            }
            if(sign){
              sign = 0;
              break;
            }
          }
          //write(2 , "one round\n" , sizeof("one round\n"));
        }
      }
    }
  }
  for(int j = 1 ; j <= judge_num ; j++){
    if(judge_list[j]==-1){
      //dup2(fd_list_ctop[j][0] , STDIN_FILENO);
      int input1[4] , input2[4];
      for(int counter = 0 ; counter < 4 ; counter++){
        char buffer[256];
        read(fd_list_ctop[j][0] , buffer , sizeof(buffer));
        sscanf(buffer , "%d %d\n" , &input1[counter] , &input2[counter]);
        if(input2[counter] == 1){
          score[input1[counter]] += 3;
        }
        else if(input2[counter] == 2){
          score[input1[counter]] += 2;
        }
        else if(input2[counter] == 3){
          score[input1[counter]] += 1;
        }
        else{
          score[input1[counter]] += 0;
        }
      }
      judge_list[j] = 0;
    }
  }
  //dup2(record_output , 1);
  //printf("judge complete\n");
  fflush(stdout);
  for(int i = 1 ; i <= judge_num ; i++){
    //dup2(record_output , 1);
    //printf("yee222:%d\n" , i);
    fflush(stdout);
    //dup2(fd_list_ptoc[i][1] , STDOUT_FILENO);
    //char buffer[256];
    //sprintf(buffer , "%d %d %d %d\n" , -1 , -1 , -1 , -1);
    write(fd_list_ptoc[i][1] , "-1 -1 -1 -1\n" , sizeof("-1 -1 -1 -1\n"));
    //write(1 , "write out buffer.\n" , sizeof("write out buffer.\n"));
    //close(fd_list_ptoc[i][1]);
    //close(fd_list_ctop[i][0]);
  }
  //dup2(record_output , 1);
  //printf("close complete\n");
  //fflush(stdout);
  for(int i = 0 ; i < player_num-1 ; i++){
    for(int j = 1 ; j <= player_num - i - 1 ; j++){
      if(score[j] >= score[j+1]){
        int tmp = score[j];
        int tmp2 = score_place_record[j];
        score[j] = score[j+1];
        score_place_record[j] = score_place_record[j+1];
        score[j+1] = tmp;
        score_place_record[j+1] = tmp2;
      }
    }
  }
  //printf("sort complete\n");
  //fflush(stdout);
  //dup2(record_output , 1);
  for(int i = player_num ; i > 0 ; i--){
    printf("%d %d\n", score_place_record[i] , score[i]);
    fflush(stdout);
  }
  //int status;
  for(int i = 1 ; i <= judge_num ; i++){
    //printf("wait:\n");
    wait(NULL);
  }
  
  //printf("yee\n");
  //close(record_output);
  for(int i = 1 ; i <= judge_num ; i++){
    close(fd_list_ptoc[i][1]);
    close(fd_list_ctop[i][0]);
  }
  
  return 0;
}