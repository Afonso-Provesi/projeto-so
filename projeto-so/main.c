//main
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include<time.h>

void criaPipes();
void contaTempo();
void servidor(int writefd,int readfd);
int cliente(int readfd,int writefd);

int writefd,readfd;

int finalizado = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutecao = PTHREAD_MUTEX_INITIALIZER;

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
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Espera até a outra thread sinalizar que terminou
    while (1) {
        pthread_mutex_lock(&mutex);
        if (finalizado) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        usleep(10000); // dorme 10 ms para não consumir CPU
    }

    clock_gettime(CLOCK_MONOTONIC, &end);


    double tempo_total = (end.tv_sec - start.tv_sec) +
                         (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Tempo total de execução: %.2f segundos\n", tempo_total);

    return NULL;
}

int writefd,readfd;

void servidor(int writefd,int readfd){
  char mensagem[1000], resposta[1000],operacao[10];
  int saldo = 100000; //praticando boas práticas de DB que aprendi na internet, isso aqui são 100000 centavos, que será dividido por 100 para representar o valor em real
  int valor;
  
  while(1){
    pthread_mutex_lock(&mutecao);
    read(readfd, mensagem, sizeof(mensagem));
    sscanf(mensagem,"%[^:]:%d",operacao,&valor);
    pthread_mutex_unlock(&mutecao);
    
    if (strcmp(operacao,"SAQUE") ==0){
      if(saldo >= valor*100){
        saldo = saldo - (valor*100);
        
        pthread_mutex_lock(&mutecao);
        sprintf(resposta,"Saque aprovado, Saldo restante = %d\n",(saldo/100));
        pthread_mutex_unlock(&mutecao);
      }else{
        pthread_mutex_lock(&mutecao);
        sprintf(resposta,"Saque negado, voce nao tem saldo suficiente, saldo atual = %d\n",saldo/100);
        pthread_mutex_unlock(&mutecao);
      }
    }else if(strcmp(operacao,"DEPOSITO")==0){
      saldo = saldo + (valor*100);
      pthread_mutex_lock(&mutecao);
      sprintf(resposta,"Deposito Concluido, seu saldo agora e = %d\n",(saldo/100));
      pthread_mutex_unlock(&mutecao);
      }
      
  pthread_mutex_lock(&mutecao);
  write(writefd,resposta,strlen(resposta)+1);
  pthread_mutex_unlock(&mutecao);
  }
}

int cliente(int readfd,int writefd){
  char clientRequest[30], mensagem[100],resposta[100];
  int valor;
  
  while(1){
  printf("Digite aqui qual operaçao voce deseja fazer: ");
  scanf("%s",clientRequest);
  
    if(strcmp(clientRequest,"saque")==0){
      printf("Quanto voce deseja sacar ?: ");
      pthread_mutex_lock(&mutecao);
      scanf("%d",&valor);
      pthread_mutex_unlock(&mutecao);
      
      pthread_mutex_lock(&mutecao);
      sprintf(mensagem,"SAQUE:%d",valor);
      pthread_mutex_unlock(&mutecao);
      
      pthread_mutex_lock(&mutecao);
      write(writefd,mensagem,strlen(mensagem)+1);
      pthread_mutex_unlock(&mutecao);
      printf("\nAguardando a solicitacao de saque ser aceita...\n");
      sleep(1);
      
      pthread_mutex_lock(&mutecao);
      read(readfd,resposta,sizeof(resposta));
      pthread_mutex_unlock(&mutecao);
      printf("%s\n",resposta);
    }else if(strcmp(clientRequest,"deposito") == 0){
      printf("Quanto voce deseja depositar ?: ");
      pthread_mutex_lock(&mutecao);
      scanf("%d",&valor);
      pthread_mutex_unlock(&mutecao);
      
      pthread_mutex_lock(&mutecao);
      sprintf(mensagem,"DEPOSITO:%d",valor);
      pthread_mutex_unlock(&mutecao);
      
      pthread_mutex_lock(&mutecao);
      write(writefd,mensagem,strlen(mensagem)+1);
      pthread_mutex_unlock(&mutecao);
      printf("\nAguardando a solicitacao de deposito ser aceita...\n");
      
      pthread_mutex_lock(&mutecao);
      read(readfd,resposta,sizeof(resposta));
      pthread_mutex_unlock(&mutecao);
      printf("%s\n",resposta);
    }else if(strcmp(clientRequest,"sair")==0){
      pthread_exit(NULL);
    }
  }
  
  return 1;
}
