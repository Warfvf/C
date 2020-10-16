//Bahdanovich Viktoryia
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LENGTH 101

int checking_arguments(int argc, char * argv[]); 	//function argument validation
int all_list(); 									//function for output the whole list of contacts
int search(char * argv[]); 							//function for search matches in contacts

int checking_arguments(int argc, char * argv[]){
	//the program can`t have more than two arguments
	if(argc>2){  
		fprintf(stderr,"error: invalid arguments\n");
		return 1;
	}
	else{
		int length = strlen(argv[1]);
		//argument length can`t be greather then 100
		if(length>100){ 
			fprintf(stderr,"error: invalid arguments\n");
			return 1;
		}
		else{
			for(int i=0; i<length; i++){  
				char chr = argv[1][i]; 
				//search criteria must contain sequence of digits 0 to 9
				if(chr<'0' || chr>'9'){ 
					fprintf(stderr,"error: invalid argument\n");
					return 1;
				}
			}
		} 
		return 0;
	}
}


int all_list(){
	char str_name[LENGTH]; //string for reading the name input from stdin
	char str_tel[LENGTH];  //string for reading the number input from stdin
	char name[LENGTH];     //string for parsed name input
	char tel[LENGTH];		//string for parsed number input

	while(!feof(stdin)){
		if(fgets(str_name,LENGTH,stdin)==NULL)			//condition to avoid re-reading the last string
			break;
		sscanf(str_name,"%[^\n]", name);			//read a string until a `\n` sign for easiest output
		if(strlen(name)==strlen(str_name)){     
			char c = '*';							//in case that a string from stdin is longer than 100 we read first 100 chars and made the rest in this string invalid
			while(c!='\n'){
				c = getchar();
			}
		}  				
		fgets(str_tel,LENGTH,stdin);
		sscanf(str_tel,"%[^\n]", tel);				//read a string until a `\n` sign for easiest output
		if(strlen(tel)==strlen(str_tel)){       	
			char c = '*';							//in case that a string from stdin is longer than 100 we read first 100 chars and made the rest in this string invalid
			while(c!='\n'){
				c = getchar();
			}
		}
		//condition to avoid something except numbers 0-9 and `+` in a number
		for(int i = 0; i<(strlen(tel)-1); i++){		
			if(( tel[i]<'0' || tel[i]>'9') && tel[i]!= '+' ){ 
				fprintf(stderr,"error: invalid arguments\n");
				return 1;
			}
		}
		printf("%s, %s\n", name, tel);
	}
	return 0;
}

int search(char * argv[]){
    char str_name[LENGTH]; 							//string for reading the name input from stdin
	char str_tel[LENGTH];  							//string for reading the number input from stdin
	char name[LENGTH];     							//string for parsed name input
	char tel[LENGTH];									//string for parsed number input
	int not_found = 0;								//variable to check wheather matches were found
	while(!feof(stdin)){
		if(fgets(str_name,LENGTH,stdin)==NULL)			//condition to avoid re-reading the last string
			break;
		sscanf(str_name,"%[^\n]", name);			//read a string until a `\n` sign for easiest output
		if(strlen(name)==strlen(str_name)){         
			char c = '*';							//in case that a string from stdin is longer than 100 we read first 100 chars and made the rest in this string invalid
			while(c!='\n'){
				c = getchar();
			}
		}  				
		fgets(str_tel,LENGTH,stdin);
		sscanf(str_tel,"%[^\n]", tel);				//read a string until a `\n` sign for easiest output
		if(strlen(tel)==strlen(str_tel)){          	
			char c = '*';							//in case that a string from stdin is longer than 100 we read first 100 chars and made the rest in this string invalid
			while(c!='\n'){
				c = getchar();
			}
		}
		for(int i = 0; i<(strlen(tel)-1); i++){
			if(( tel[i]<'0' || tel[i]>'9') && tel[i]!= '+' ){ 
				fprintf(stderr,"error: invalid arguments\n");
				return 1;
			}
		}
		//rewriting to lowercase
		int name_len = (strlen(name)+1);
		int tel_len = (strlen(tel)+1);
		for(int i = 0; i<name_len; i++){
			if(i==strlen(name)){
				name[i]='\0';
				break;
			}
			else if(name[i]<'[' && name[i]>'@')
				name[i]+=32;
		}
		for(int i = 0; i<tel_len; i++){
			if(i==strlen(tel)){
				tel[i]='\0';
				break;
			}
		}
		//re-write `list_to_num` into numbers
		char num_name[LENGTH];	    //string for names re-writed into numbers
		char num_tel[LENGTH];	    //string for TELEPHONE NUMBERS WHICH INCLUDE `+` re-writed into numbers

		strcpy (num_name, name);
		strcpy (num_tel, tel);

		//re-write inputs into numbers
		for(int i = 0; num_tel[i]!='\0'; i++){
			if(tel[i]=='+')
				num_tel[i]='0';
		}
		
		
		for(int i = 0; num_name[i]!='\0'; i++){
			switch(num_name[i]){           
				case 'a': case 'b': case 'c': num_name[i]='2'; break;
				case 'd': case 'e': case 'f': num_name[i]='3'; break;
				case 'g': case 'h': case 'i': num_name[i]='4'; break;
				case 'j': case 'k': case 'l': num_name[i]='5'; break;
				case 'm': case 'n': case 'o': num_name[i]='6'; break;
				case 'p': case 'q': case 'r': case 's': num_name[i]='7'; break;
				case 't': case 'u': case 'v': num_name[i]='8'; break;
				case 'w': case 'x': case 'y': case 'z': num_name[i]='9'; break;
				case '+': num_name[i]='0'; break;
				default: break;
			}
		}
		//search
		int found_n = 0;	//variable to check wheather matches were found
		int found_t = 0;	//variable to check wheather matches were found
		for(int i = 0; num_name[i]!='\0'; i++){
			if(num_name[i]==argv[1][found_n]){
				if(name[i]<'{' && name[i]>'`')
					name[i]-=32;
				found_n++;
			}		
		}
		for(int i = 0; num_tel[i]!='\0'; i++){
			if(num_tel[i]==argv[1][found_t]){
				found_t++;
			}		
		}
		if (strstr(num_name, argv[1]) != NULL || strstr(num_tel, argv[1]) != NULL){
			not_found++;
			if(strstr(num_name, argv[1]) != NULL){
				int index = (strstr(num_name, argv[1]) - num_name);
				int len_of_arg = (index+strlen(argv[1]));
				for(; index < len_of_arg; index++){ 
					if(name[index]<'{' && name[index]>'`')
						name[index]-=32;
				}
			}
			else{
				for(int i = 0; name[i]!='\0'; i++){
					if(name[i]<'[' && name[i]>'@')
						name[i]+=32;
				}
			}
			printf("%s, %s\n", name, tel);
		}
		else if(found_n==strlen(argv[1]) || found_t==strlen(argv[1])){
			not_found++;
			if(found_n!=strlen(argv[1])){
				for(int i = 0; name[i]!='\0'; i++){
					if(name[i]<'[' && name[i]>'@')
						name[i]+=32;
				}
			}
			printf("%s, %s\n", name, tel);
		}
		

	}
	if (not_found==0)
		printf("Not found\n");
	return 0;
}


int main(int argc, char * argv[]){
	if(argc==1)
			all_list();
	else {
		if(checking_arguments(argc, argv) == 0)
			search(argv);
	}
	return 0;
}
