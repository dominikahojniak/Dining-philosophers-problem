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


struct buforr{          // struktura komunikatu
long mtype;             // typ komunikatu
int mvalue;             //tresc komunikatu
};

int shmID, semID, msgID; // ID semafora, kolejki kom. pami�ci dzielonej
void inicjalizujSem(int semID, int number, int val)
{
 if ( semctl(semID, number, SETVAL, val) == -1 )
 {perror("Blad semctl (inicjalizujSemafor): "); exit(1);}
}

int alokujSem(key_t klucz, int number, int flagi)
{
 int semID;
 if ( (semID = semget(klucz, number, flagi)) == -1)
 { perror("Blad semget (alokujSemafor): ");exit(1); }
 return semID;
}
// funkcja koniec -- do obs�ugi przerwania
void koniec(int sig)
{
   msgctl(msgID, IPC_RMID, NULL);
   shmctl(shmID, IPC_RMID, NULL);
   semctl(semID, 0, IPC_RMID, NULL);
   printf("MAIN - funkcja koniec sygnal %d: Koniec.\n", sig);
   exit(1);
}
int main(int argc, char *argv[])
{
key_t klucz, kluczm,kluczs; // klucze do kolejki komunikatow i pamieci dzielonej
int msgID; // id kolejki komunikatow
int shmID; // id pamieci dzielonej
int semID; // id semaforow
int i,numer,numermod;
int N=5;
struct buforr komunikat;
char bufor[3]; //
int *widelec;
 struct sigaction act;
   act.sa_handler = koniec;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   sigaction(SIGINT, &act, 0);//zeby byl wykonywana funkcja koniec SIGINT

        if ( ( klucz = ftok(".", 'A')) == -1 )
        {
                printf("Blad ftok (main) \n");
                exit(1);
        }

msgID = msgget(klucz, IPC_CREAT | IPC_EXCL | 0666); // tworzenie kolejki komunikatow
//pewnosc zeby nasze komunikaty byly unikalne
komunikat.mtype=5;//inicjalizacja widelcow
for(i=0;i<=5;i++)
        msgsnd(msgID, &komunikat, sizeof(int), 0);
        if (msgID==-1)
        {
                printf("blad kolejki komunikatow\n");
                exit(1);
        }
        kluczm = ftok(".", 'B'); // klucz dla pamieci dzielonej

shmID = shmget(kluczm, 5*sizeof(int), IPC_CREAT | IPC_EXCL | 0666); // tworzenie pamieci dzielonej
 if (shmID == -1)
   {
      printf("blad pamięci dzielonej\n");
      exit(1);
   }
kluczs = ftok(".", 'C');
   semID = alokujSem(kluczs, 1, IPC_CREAT | IPC_EXCL | 0666);
        inicjalizujSem(semID,0,1);//podniesiony semafor
if (semID == -1)
   {
      printf("blad semaforów \n");
      exit(1);
   }
fflush(stdout);
widelec = (int *)shmat(shmID,NULL,0);//pamiec dzielona
for(i=0;i<5;i++){
widelec[i]=0;
}
        for (i = 1; i < 6; i++)

                switch (fork())         // uruchamianie procesow filozofow
                {
                case -1:
                        printf ("Blad fork (mainprog)\n");
                        exit(2);
                case 0:
                        sprintf(bufor,"%d",i);//przekazanie numeru
        komunikat.mtype=i;
        msgsnd(msgID, &komunikat, sizeof(int), 0);
                        execl("./filozof","filozof",bufor,NULL);
                }

        for (i = 0; i<5; i++)
        {
                wait(NULL);
        }

                                        //zwalnianie zasobow
        msgctl(msgID,IPC_RMID,NULL);
        shmctl(shmID,IPC_RMID,NULL);
        semctl(semID, 0, IPC_RMID, NULL);
        printf("MAIN: Konec.\n");
}

