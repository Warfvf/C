#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>          //close() and etc. 
#include <fcntl.h>           //O_XXX flags
#include <sys/stat.h>        //S_IXXX flags
#include <sys/types.h>       //mode_t
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

typedef struct{
    int A;                  //serial number of the performed action
    int I;                  //internal process identifier within the relevant category
    int NE;                 //the current number of immigrants who entered the building and have not yet been decided
    int NC;                 //the current number of immigrants who have registered and have not yet been decided
    int NB;                 //the current number of immigrants who are in the building
    int RI;                 //the total number of immigrants who have certificate
    int judge_waits;        //judge waits flag
    int got_certificate;    //the current number of immigrants who got certificate
} items;
items *variables;

sem_t *EXIT_MAIN;       //waiting for all subsidiary processes to finish
sem_t *JUDGE_IS_HERE;   //closed for immigrants when judge is in the courthouse and vice versa
sem_t *ENDS_CONFIRM;    //closed for immigrants until judge ends confirmation
//sem_t *IMM_LEAVES;      //closed for judge until immigrants who have certificate leave
sem_t *IMM_REG;         //closed for judge until immigrants who entered the building are registered

int checking_arguments(int argc, char * argv[]); //function argument validation
void judge_proc(int JG, int JT, int PI);    //function for judge process
void generator_proc(int IG, int IT);        //function for additional process generating immigrants
void dctor(int *addr);                      //de-allocation
void imm_proc(int IT, int Imm_num);         //function for immigrant process
FILE *file;
