
#include <stdio.h>
#include <sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <signal.h>

struct buforr
{
long mtype;
int mvalue;
};
void signalSemafor(int semID, int number);
int waitSemafor(int semID, int number, int flags);
int alokujSemafor(key_t klucz, int number, int flagi);
int msRecieve(int msgID,int number)
{
        struct buforr komunikat;
        if(msgrcv(msgID,&komunikat,sizeof(int),number,0)==-1)
        exit(-1);
}
int msSend(int msgID,int number)
{
        struct buforr komunikat;
        komunikat.mtype=number;
        if(msgsnd(msgID,&komunikat,sizeof(int),0)==-1)
        exit(-1);
}
int alokujSemafor(key_t klucz, int number, int flagi)
{
   int semID;
   if ( (semID = semget(klucz, number, flagi)) == -1)
   {
      perror("Blad semget (alokujSemafor): ");
      exit(1);
   }
   return semID;
}
int waitSemafor(int semID, int number, int flags)
{
   struct sembuf operacje;
   operacje.sem_num = number;
   operacje.sem_op = -1;         // -1 -> wait
   operacje.sem_flg = 0 | flags; // SEM_UNDO;

   if (semop(semID, &operacje, 1) == -1) // wykonuje to co w sembuf struct
   {
      // perror("Blad semop (waitSemafor)");
      return -1;
   }

   return 1;
}

void signalSemafor(int semID, int number)
{
   struct sembuf operacje;
   operacje.sem_num = number;
   operacje.sem_op = 1;
 operacje.sem_flg = SEM_UNDO;
   if (semop(semID, &operacje, 1) == -1)
      perror("Blad semop (postSemafor): ");

   return;
}

int main(int argc, char *argv[])
{
struct buforr komunikat;
   key_t klucz, kluczm, kluczs;
   int msgID;
   int shmID, semID;
   int i,j;
   int numer,numermod; //numer  filozofa
   int *widelec;
   char bufor[3];
 if ((klucz = ftok(".", 'A')) == -1)
   {
      printf("Blad ftok (A)\n");
      exit(1);
   };

   msgID = msgget(klucz, IPC_CREAT | 0666);
   if (msgID == -1)
   {
      printf("blad klejki komunikatow\n");
      exit(1);
   }
   kluczm = ftok(".", 'B');
   kluczs = ftok(".", 'C');
   shmID = shmget(kluczm, 5 * sizeof(int), IPC_CREAT | 0666);
   if (shmID==-1)
   {
   printf("blad shm\n");
   exit(1);
   }
   semID = alokujSemafor(kluczs, 1, IPC_CREAT | 0666);
fflush(stdout);
   widelec = (int *)shmat(shmID, 0, 0);//dolaczenie tej pamieci do przestarzni adresowej
   numer= atoi(argv[1]);//pobranie numeru filozofa
for(i=0;i<5;i++)
{
 printf("Filozof %d mysli \n",numer);
msRecieve(msgID,numer);//czekanie na lewy
numermod=(numer+1)%5;
msRecieve(msgID,numermod);//czekanie na prawy
waitSemafor(semID, 0, SEM_UNDO);
widelec[(numer+1)%5]=numer;//podeniesienie widelcow i ustawienie numeru filozofa
widelec[numer]=numer;
signalSemafor(semID, 0);
 printf("Filozof %d je\n",numer);
for(j=1;j<6;j++)
{
printf(" %d  ",widelec[j]);
}
printf("\n");
waitSemafor(semID, 0, SEM_UNDO);
 widelec[(numer+1)%5]=0;//zwolnienie widelcow
widelec[numer]=0;
signalSemafor(semID, 0);
msSend(msgID,numer);
msSend(msgID,numermod);
printf("Filozof %d skonczyl jesc\n",numer);
}
}
