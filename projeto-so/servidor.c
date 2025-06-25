#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int writefd,readfd;

void servidor(int writefd,int readfd){
  char mensagem[1000], resposta[1000],operacao[10];
  int saldo = 100000; //praticando boas práticas de DB que aprendi na internet, isso aqui são 100000 centavos, que será dividido por 100 para representar o valor em real
  int valor;
  
  read(readfd, mensagem, sizeof(mensagem));
  sscanf(mensagem,"%[^:]:%d",operacao,&valor);
  
  if (strcmp(operacao,"SAQUE") ==0){
    if(saldo >= valor*100){
      saldo = saldo - (valor*100);
      sprintf(resposta,"Saque aprovado, Saldo restante = %d\n",(saldo/100));
    }else
      sprintf(resposta,"Saque negado, voce nao tem saldo suficiente, saldo atual = %d\n",saldo/100);
  }else if(strcmp(operacao,"DEPOSITO")==0){
    saldo = saldo + (valor*100);
    sprintf(resposta,"Deposito Concluido, seu saldo agora e = %d\n",(saldo/100));
    }
  
  write(writefd,resposta,strlen(resposta)+1);
  
}
