//Bahdanovich Viktoryia
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define I0 1e-12
#define UT 0.0258563

int checking_arguments(int argc, char * argv[]); 	//function argument validation
double diode(double u0, double r, double eps); 		//diode voltage calculation function
double diode_I(double Up); 							//diode amperage calculation function

int checking_arguments(int argc, char * argv[]){
	//checking of invalid access
	if (argc!=4){		
		fprintf(stderr,"error: invalid arguments \n");
		return 1;
	}
	//condition determines whether `eps` is infinity value
	else if((atof(argv[3])) == INFINITY){ 			
			fprintf(stderr,"error: invalid argument (`eps` can't be infinity value)\n");
			return 1;
    }
	char *endstr = NULL;
        for(int i = 1; i<4; i++){
		double check = strtod (argv[i], &endstr);
		//condition determines whether conversion failed
		if (*endstr!='\0'){ 
			fprintf(stderr,"error: invalid arguments (neither input can contain space, punctuation mark except dot, letter except `E` and `e`)\n");
			return 1;
		}
		//condition determines whether `eps` or `r` are zero values
		else if(i != 1 && check == 0){ 
			fprintf(stderr,"error: invalid arguments (`eps` and `R` can't be zero values)\n");
			return 1;
    		}
		//condition determines that neither input can be negative
		else if (check < 0){ 
			fprintf(stderr,"error: invalid arguments (neither input can be negative)\n");
			return 1;
	 	}
	}
	return 0;
}


double diode(double u0, double r, double eps){
	double start = 0; 					//left border
	double end = u0; 					//right border
	double previous_middle = 0; 		//previous value of the middle
	double Up, Ip, Ir, Ur;
	double middle = 0;
	while(fabs(end-start) > eps){
		middle = (end+start)/2.0;
		if(previous_middle == middle) 	//infinity cycle check
			break;
		previous_middle = middle;
		Ur = u0 - middle;
		Ir = Ur/r;
		Ip = I0*((exp(middle/UT)) - 1);
		if(Ir > Ip)
			start = middle;
		else if(Ir < Ip)
			end = middle;
		Up = middle;
		if(Ir == Ip)
			break;
	}
	return Up;
}


double diode_I(double Up){
	double Ip = I0*((exp(Up/UT)) - 1);
	return Ip;
}


int main( int argc, char * argv[] ){
	int input_check = checking_arguments(argc, argv);
	if(input_check!=1){
		if(atof(argv[1]) == INFINITY && atof(argv[2]) == INFINITY)
			printf("Up=inf V\nIp=inf A\n");
		else if(atof(argv[1]) != INFINITY && atof(argv[2]) == INFINITY)
			printf("Up=0 V\nIp=0 A\n");
		else{
			double u0 = atof(argv[1]);
			double r = atof(argv[2]);
			double eps = atof(argv[3]);
			double Up = diode (u0,r,eps);
			double Ip = diode_I(Up);
			printf("Up=%g V\nIp=%g A\n", Up, Ip );
		}
	}
	return 0;
}