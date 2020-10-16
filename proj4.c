//Bahdanovich Viktoryia
#include "lib.h"

int checking_arguments(int argc, char * argv[]){
    //checking of invalid access
    if (argc!=6){		
		fprintf(stderr,"error: invalid quantity of arguments\n");
		return 1;
	}

    //checking wheather arguments aren`t integer numbers, hence `strtol` determine wheather inputs contain space, punctuation mark and other non-numbers character
    char *endstr = NULL;
    for(int i = 1; i<6; i++){
		long int arg = strtol(argv[i], &endstr, 10);
        //condition determines whether conversion failed
		if (*endstr!='\0'){
            fprintf(stderr,"error: invalid arguments (neither input can contain space, punctuation mark and other non-numbers character)\n");
			return 1;
		}

        //checking wheather arguments are correct
        else if(i == 1 && arg<1){
            fprintf(stderr,"error: the first argument is invalid\n");
			return 1;
        }
        else if(i == 2 && (arg<0 || arg>=2001)){
            fprintf(stderr,"error: the second argument is invalid\n");
			return 1;
        }
        else if(i == 3 && (arg<0 || arg>=2001)){
            fprintf(stderr,"error: the third argument is invalid\n");
			return 1;
        }
        else if(i == 4 && (arg<0 || arg>=2001)){
            fprintf(stderr,"error: the fourth argument is invalid\n");
			return 1;
        }
        else if(i == 5 && (arg<0 || arg>=2001)){
            fprintf(stderr,"error: the fifth argument is invalid\n");
			return 1;
        }
    }
	return 0;
}

void judge_proc(int JG, int JT, int PI){
    do { 
    usleep(JG);
    fprintf(file,"%-3d         : JUDGE         : wants to enter\n", ++variables->A);
    fprintf(file,"%-3d         : JUDGE         : enters              : %d   : %d   : %d\n", ++variables->A, variables->NE, variables->NC, variables->NB);
    sem_wait(JUDGE_IS_HERE);
    if(variables->NE != variables->NC){
        fprintf(file,"%-3d         : JUDGE         : waits for imm       : %d   : %d   : %d\n", ++variables->A, variables->NE, variables->NC, variables->NB);
        variables->judge_waits = 1;
        sem_wait(IMM_REG);
    }
    fprintf(file,"%-3d         : JUDGE         : starts confirmation : %d   : %d   : %d\n", ++variables->A, variables->NE, variables->NC, variables->NB);
    
    usleep(JT);
    variables->RI += variables->NC;
    variables->got_certificate = variables->NC;
    variables->NE = 0;
    variables->NC = 0;
    
    fprintf(file,"%-3d         : JUDGE         : ends confirmation   : %d   : %d   : %d\n", ++variables->A, variables->NE, variables->NC, variables->NB);
    if(variables->got_certificate > 0)
        sem_post(ENDS_CONFIRM);
    usleep(JT);
    
    fprintf(file,"%-3d         : JUDGE         : leaves              : %d   : %d   : %d\n", ++variables->A, variables->NE, variables->NC, variables->NB);
    sem_post(JUDGE_IS_HERE);
    } while(variables->RI != PI);

    fprintf(file,"%-3d         : JUDGE         : finishes\n", ++variables->A);
    sem_post(EXIT_MAIN);
    exit(0);
}

void generator_proc(int IG, int IT){ 
    usleep(IG);
    variables->I++;
    
    pid_t pid_i_flag;
    pid_i_flag = fork();
    if(pid_i_flag == 0)
        imm_proc(IT, variables->I);
    
    return;
}

void dctor(int *addr){
    munmap(addr, 8*sizeof(int));
}

void imm_proc(int IT, int Imm_num){
    fprintf(file,"%-3d         : IMM %d         : starts\n", ++variables->A, Imm_num);

    sem_wait(JUDGE_IS_HERE);
    sem_post(JUDGE_IS_HERE);
    
    fprintf(file,"%-3d         : IMM %d         : enters              : %d   : %d   : %d\n", ++variables->A, Imm_num, ++variables->NE, variables->NC, ++variables->NB);
    fprintf(file,"%-3d         : IMM %d         : checks              : %d   : %d   : %d\n", ++variables->A, Imm_num, variables->NE, ++variables->NC, variables->NB);
    if(variables->NE == variables->NC && variables->judge_waits == 1){
        variables->judge_waits = 0;
        sem_post(IMM_REG);
    }
    sem_wait(ENDS_CONFIRM);
    sem_post(ENDS_CONFIRM);
    variables->got_certificate--;
    if(variables->got_certificate == 0)
        sem_wait(ENDS_CONFIRM);

    fprintf(file,"%-3d         : IMM %d         : wants certificate   : %d   : %d   : %d\n", ++variables->A, Imm_num, variables->NE, variables->NC, variables->NB);
    usleep(IT);
    fprintf(file,"%-3d         : IMM %d         : got certificate     : %d   : %d   : %d\n", ++variables->A, Imm_num, variables->NE, variables->NC, variables->NB);
    sem_wait(JUDGE_IS_HERE);
    fprintf(file,"%-3d         : IMM %d         : leaves              : %d   : %d   : %d\n", ++variables->A, Imm_num, variables->NE, variables->NC, --variables->NB);
    sem_post(JUDGE_IS_HERE);
    sem_post(EXIT_MAIN);
    exit(0);
}


void main(int argc, char * argv[]){
    if(checking_arguments(argc, argv))
        exit(1);
    //pointer to shared memory region
    void *addr = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(addr == MAP_FAILED){
        fprintf(stderr, "MAP failed..\n" );
        exit(1);
    }
    variables = addr;

    variables->I = 0;
    variables->A = 0;
    variables->NB = 0;
    variables->NC = 0;
    variables->NB = 0;
    variables->RI = 0;

    int PI = atoi(argv[1]);
    //transfer to microseconds for usleep()
    int IG = atoi(argv[2])*1000;
    int JG = atoi(argv[3])*1000;
    int IT = atoi(argv[4])*1000;
    int JT = atoi(argv[5])*1000;

    EXIT_MAIN = sem_open("bahda", O_CREAT, 0666, 0);     
    JUDGE_IS_HERE = sem_open("bahda_j_i_h", O_CREAT, 0666, 1);     
    ENDS_CONFIRM = sem_open("bahda_e_c", O_CREAT, 0666, 0);     
    IMM_REG = sem_open("bahda_i_r", O_CREAT, 0666, 0);     
    
    file = fopen("proj4.out", "w");
    setbuf(file, NULL);

    pid_t pid_j_flag, pid_g_flag;
    pid_j_flag = fork();
    if(pid_j_flag == 0)
        judge_proc(JG, JT, PI);

    pid_g_flag = fork();
    if(pid_g_flag == 0){
        for(int i = 0; i<PI; i++)
            generator_proc(IG, IT);
        exit(0);
    }

    for(int i = 0; i<PI+1; i++){
        sem_wait(EXIT_MAIN);
    }
    //the end of the program    
    sem_unlink("bahda");
    sem_close(EXIT_MAIN);
    sem_unlink("bahda_j_i_h");
    sem_close(JUDGE_IS_HERE);
    sem_unlink("bahda_e_c");
    sem_close(ENDS_CONFIRM);
    sem_unlink("bahda_i_r");
    sem_close(IMM_REG);
    dctor(addr);
    fclose(file);
    exit(0);
}