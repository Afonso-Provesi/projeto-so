//main
#include<stdio.h>
#include "cliente.c"
#include "servidor.c"
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include<time.h>

void criaPipes();
void contaTempo();

int finalizado = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
        pthread_t thread1,thread2;
        
        pthread_create(&thread2,NULL,contaTempo,NULL);
	pthread_create(&thread1,NULL,criaPipes,NULL);
	  
        pthread_join(thread1, NULL);
        
        pthread_mutex_lock(&mutex);
        finalizado = 1;
        pthread_mutex_unlock(&mutex);
        
        pthread_join(thread2,NULL);
        
        return 0;
}


void criaPipes(){
  int descritor, pipe1[2], pipe2[2];
	  
	  if(pipe(pipe1)<0 || pipe(pipe2)<0){
		  printf("erro na chamada Pipe");
		  exit(0);
	  }

	  if((descritor = fork())<0){
		  printf("erro na chamada Fork");
		  exit(0);
	  }

	  else if(descritor>0){
		  close(pipe1[0]);//fecha leitura no pipe1
		  close(pipe2[1]);//fecha escrita no pipe2
		  
		  cliente(pipe2[0],pipe1[1]);

		  close(pipe1[1]);
		  close(pipe2[0]);
		  pthread_exit(NULL);
	  }
	  
	  else{
		  close(pipe1[1]);//fecha escrita no pipe 1
		  close(pipe2[0]);//fecha leitura no pipe 2
		    
		  servidor(pipe2[1],pipe1[0]);

		  close(pipe1[0]);
		  close(pipe2[1]);
		  exit(0);
	  }
}

void contaTempo(){
    clock_t start_t, end_t;
    double total_t;

    start_t = clock(); 

    while(1){
      pthread_mutex_lock(&mutex);
      if(finalizado){
        pthread_mutex_unlock(&mutex);
        break;
      }
      pthread_mutex_unlock(&mutex);
      usleep(10000);
    }

    end_t = clock(); 

    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    printf("Tempo de execução: %f segundos\n", total_t);
    pthread_exit(NULL);
}
