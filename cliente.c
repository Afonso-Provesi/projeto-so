#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>

int readfd, writefd;

int cliente(int readfd,int writefd){
  char clientRequest[30], mensagem[100],resposta[100];
  int valor;
  printf("Digite aqui qual operaçao voce deseja fazer: ");
  scanf("%s",clientRequest);
  
  if(strcmp(clientRequest,"saque")==0){
    printf("Quanto voce deseja sacar ?: ");
    scanf("%d",&valor);
    
    sprintf(mensagem,"SAQUE:%d",valor);
    
    write(writefd,mensagem,strlen(mensagem)+1);
    printf("\nAguardando a solicitacao de saque ser aceita...\n");
    sleep(1);
    
    read(readfd,resposta,sizeof(resposta));
    printf("%s\n",resposta);
  }else if(strcmp(clientRequest,"deposito") == 0){
    printf("Quanto voce deseja depositar ?: ");
    scanf("%d",&valor);
    
    sprintf(mensagem,"DEPOSITO:%d",valor);
    
    write(writefd,mensagem,strlen(mensagem)+1);
    printf("\nAguardando a solicitacao de deposito ser aceita...\n");
    
    read(readfd,resposta,sizeof(resposta));
    printf("%s\n",resposta);
  }
  
  return 1;
}
