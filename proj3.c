//Bahdanovich Viktoryia
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define buff 129
FILE *file;

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

struct prirustek {
    int dr;
    int ds;
};
 
enum smery { VYCHOD, SEVER, ZAPAD, JIH, POCET_SMERU };
 
struct prirustek d[POCET_SMERU] = {
    {0, +1}, // VYCHOD  EAST  (0)
    {-1, 0}, // SEVER   NORTH (1)
    {0, -1}, // ZAPAD   WEST  (2)
    {+1, 0}, // JIH     SOUTH (3)
};

int ctor(int argc, char * argv[], Map *map, FILE *file);          //inicialization of map
void dctor(Map *map);                                             //re-inicialization of map
int help(int argc, char * argv[]);                                //function for help output
bool isborder(Map *map, int r, int c, int border);                //border detection function
int test(Map *map);                                               //functions for map validation output
int test_in_a_row(Map *map);
int test_in_a_col(Map *map);
int data_test(int argc, char * argv[], Map *map, FILE *file);     //data validation function
int file_test(int argc, char * argv[], Map *map, FILE *file);     //map validation function and declaration of map
int arg_check(int argc, char * argv[], Map *map);                 //argument validation function in case of `--rpath/--lpath`
int R_C_check(int argc, char * argv[], Map *map);                 //start coordinates validation function in case of `--rpath/--lpath`
int start_border(Map *map, int r, int c, int leftright);          //determines first border to check
void quest(Map *map, int r, int c, int leftright, int direction); //search of an exit

int ctor(int argc, char * argv[], Map *map, FILE *file){
    //variable to determine data validity in file
    int validity = data_test(argc, argv, map, file);

    if(  (map->cells = malloc(map->rows*map->cols*sizeof(unsigned char)) ) == NULL ){
        fprintf(stderr,"Allocation error.\n");
		return 1;
    }
    else if(file_test(argc, argv, map, file) == 0 && test(map) == 0 && validity == 0)
        return 0;
}
 
void dctor(Map *map){
    free(map->cells);
}

int help(int argc, char * argv[]){
    if(strcmp(argv[1],"--help")==0){
        printf("\n    *Nápověda používání programu*\n\n`--test soubor.txt` - pouze zkontroluje, že soubor daný druhým argumentem programu obsahuje řádnou definici mapy bludiště.\n\n`--rpath R C soubor.txt` - hledá průchod bludištěm na vstupu na řádku R a sloupci C. Průchod hledá pomocí pravidla pravé ruky (pravá ruka vždy na zdi).\n\n`--lpath R C soubor.txt` - hledá průchod bludištěm na vstupu na řádku R a sloupci C, ale za pomoci pravidla levé ruky.\n\n");
        //`--shortest R C soubor.txt` - (prémiové řešení) hledá nejkratší cestu z bludiště při vstupu na řádku R a sloupci C."
        return 0;
    }
    else{
        fprintf(stderr,"error: invalid arguments (for reference enter `--help`)\n");
		return 1;
    }
}

bool isborder(Map *map, int r, int c, int border){
    int up_and_down_border = 4;
    int right_border = 2;
    int left_border = 1;
    //`▲` cell
    if((r+c)&1 && border == SEVER)
        return true;
    //`▼` cell     
    else if((r+c)%2 == 0 && border == JIH)
        return true;
    if (border == VYCHOD ){
        //2 is responsible for the right border
        if(((map->cells[map->cols*r+c]) & right_border) == right_border)
            return true;
        else
            return false;
    }
    else if (border == SEVER || border == JIH){
        //4 is responsible for an up or a down border
        if(((map->cells[map->cols*r+c]) & up_and_down_border) == up_and_down_border)
            return true;
        else
            return false;
    }
    else if (border == ZAPAD ){
        //1 is responsible for the left border
        if((map->cells[map->cols*r+c]) & left_border)
            return true;
        else
            return false;
    }
}

int test(Map *map){
    if(test_in_a_row(map) == 0 && test_in_a_col(map) == 0 )
        return 0;
}


int test_in_a_row(Map *map){
    for(int r = 0; r < map->rows; r++){
        for(int c = 0; c < (map->cols - 1); c++){
            if(isborder(map, r, c, VYCHOD) != isborder(map, r, c+1, ZAPAD)){
                fprintf(stderr,"Invalid parameters: incompatible types of cells in a row\n");
				return 1;
            }
        }
    }
    return 0;
}


int test_in_a_col(Map *map){
    for(int r = 0; r < (map->rows - 1); r++){
        for(int c = 0; c < map->cols; c++){
            //condition to check only cells with odd sum of coordinates indices
            if((r+c)&1){
                if(isborder(map, r, c, JIH) != isborder(map, r+1, c, SEVER)){
                    fprintf(stderr,"Invalid parameters: incompatible types of cells in a col\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}

int data_test(int argc, char * argv[], Map *map, FILE *file){
    int r, s;
    //a string for reading map data from file
    char r_and_s[buff];
          
    fgets(r_and_s,buff,file);
    
    for(int i = 0; i<(strlen(r_and_s)-1); i++){	
            //condition to avoid something except numbers 0-9	
			if((r_and_s[i]<'0' || r_and_s[i]>'9') && r_and_s[i]!=' ' && r_and_s[i]!='\n' && r_and_s[i]!='\0'){ 
                fprintf(stderr,"Invalid data in the first string\n");
				return 1;
			}
    }
    //condition to avoid missing number of columns
    if(sscanf(r_and_s,"%d %d", &r, &s)!=2){
        fprintf(stderr,"Invalid data: not enough parameters in the first string\n");
		return 1;
    }
    //condition to avoid too many parameters in the first string
    else{
        char str_for_s[buff];
        //re-write integer `s` to string
        sprintf(str_for_s, "%d", s);
        //the last cell of argument `s` in `r_and_s`
        int last_cell_of_s = (strstr(r_and_s,str_for_s)-r_and_s)+strlen(str_for_s);
        for(int i = last_cell_of_s; i<(strlen(r_and_s)-1); i++){	
            if(r_and_s[i]!=' ' && r_and_s[i]!='\n' && r_and_s[i]!='\0'){ 
                fprintf(stderr,"Invalid data: too many parameters in the first string\n");
				return 1;
			}
        }
    }
    map->rows = r;
    map->cols = s;
    return 0; 
}

int file_test(int argc, char * argv[], Map *map, FILE *file){
    //variable to count cells
    int quantity_of_cells = 0;
    //strings for reading map data from file
    char str[buff];
    //variable to count iterations of next while
    int iter = 0;
    //variable to count a col index in the next while
    int index = 0;

    while(!feof(file)){
        //condition to avoid re-reading the last string
		if(fgets(str,buff,file)==NULL)		
			break;
        for(int i = 0; i<(strlen(str)-1); i++){	
            //condition to avoid something except numbers 0-7	
			if((str[i]<'0' || str[i]>'7') && str[i]!=' ' && str[i]!='\n' && str[i]!='\0'){ 
                fprintf(stderr,"Invalid parameters: incorrect data for creating a map\n");
				return 1;
			}
            //condition to avoid numbers greather then 9 (double figures and so on)
            else if(str[i]!=' ' && str[i+1]!=' ' && str[i+1]!='\n' && str[i+1]!='\0'){
                fprintf(stderr,"Invalid data: parameters greather then 9 in cells definition\n");
				return 1;
            }
            //condition for counting the quantity of cells
            if(str[i]>='0' && str[i]<='7'){
                quantity_of_cells++;
                map->cells[index+(iter*map->cols)]=str[i];
                index++;
            }
		}
        index = 0;
        //condition to detect if a map was difined properly
        if((quantity_of_cells-(map->cols*iter))!=map->cols){
            fprintf(stderr,"Invalid definition: not enough parameters for creating a map\n");
            return 1;
        }
        iter++;
    }
    //condition to detect if it were missed data 
    if(quantity_of_cells!=(map->rows*map->cols)){
        fprintf(stderr,"Invalid definition: not enough parameters for creating a map\n");
		return 1;
    }
    return 0;
}

int arg_check(int argc, char * argv[], Map *map){
    int len_r = strlen(argv[2]);
		
	for(int i=0; i<len_r; i++){  
		char chr = argv[2][i];
        //search criteria must contain sequence of digits 0 to 9 
	    if(chr<'0' || chr>'9'){ 
			fprintf(stderr,"error: invalid `R` argument (for reference enter `--help`)\n");
			return 1;
		}
    }
    int len_c = strlen(argv[3]);
	
    for(int i=0; i<len_c; i++){  
		char chr = argv[3][i];
        //search criteria must contain sequence of digits 0 to 9 
	    if(chr<'0' || chr>'9'){ 
			fprintf(stderr,"error: invalid `C` argument (for reference enter `--help`)\n");
			return 1;
		}
    }

    if(R_C_check(argc, argv, map) == 0)
        return 0;
}

int R_C_check(int argc, char * argv[], Map *map){
    int r = atoi(argv[2])-1;
    int c = atoi(argv[3])-1;

    //condition to detect if `r` and `c` are within map size
    if( r < 0 || c < 0 || r > map->rows || c > map->cols ){
        fprintf(stderr,"error: entered argument is outside the map\n");
		return 1;
    }

    //conditions to detect if `r` and `c` are around the edges of map and that they were entered properly
    if(r != 0 && r != (map->rows-1)){
        if(c != 0 && c != (map->cols-1)){
            fprintf(stderr,"error: entered coordinates aren`t around the edges of map\n");
		    return 1;
        }
        else if(c == 0){
            if(isborder(map, r, c, ZAPAD) == true){
                fprintf(stderr,"error: we can`t enter => there is a border\n");
                return 1;
            }
        }
        else if(c == (map->cols-1)){
            if(isborder(map, r, c, VYCHOD) == true){
                fprintf(stderr,"error: we can`t enter => there is a border\n");
                return 1;
            }
        }
    }

    if(r == 0 && c != 0 && c != (map->cols-1)){
        if((r+c)&1){
            fprintf(stderr,"error: we can`t enter from above `odd` cell\n");
		    return 1;
        }
        else if(isborder(map, r, c, SEVER) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }

    if(r == (map->rows-1) && c != 0 && c != (map->cols-1)){
        if(((r+c)%2)==0){
            fprintf(stderr,"error: we can`t enter from below `even` cell\n");
		    return 1;
        }
        else if(isborder(map, r, c, JIH) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }

    if(r == 0 && c == 0){
        if(isborder(map, r, c, ZAPAD) == true && isborder(map, r, c, SEVER) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }

    if(r == (map->rows-1) && c == 0){
        if(isborder(map, r, c, ZAPAD) == true && isborder(map, r, c, JIH) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }

    if(r == 0 && c == (map->cols-1)){
        if(isborder(map, r, c, VYCHOD) == true && isborder(map, r, c, SEVER) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }

    if(r == (map->rows-1) && c == (map->cols-1)){
        if(isborder(map, r, c, VYCHOD) == true && isborder(map, r, c, JIH) == true){
            fprintf(stderr,"error: we can`t enter => there is a border\n");
		    return 1;
        }
    }
    return 0;
}

int start_border(Map *map, int r, int c, int leftright){
    int direction;
     //--rpath
    if(leftright == 0){
        if( c == 0 && r%2 == 0 && isborder(map, r, c, ZAPAD) == false)
            direction = VYCHOD;
        else if( c == 0 && (r&1) && isborder(map, r, c, ZAPAD) == false)
            direction = JIH;
        else if( c == (map->cols-1) && (r+c)%2 == 0 && isborder(map, r, c, VYCHOD) == false)
            direction = SEVER;
        else if( c == (map->cols-1) && ((r+c)&1) && isborder(map, r, c, VYCHOD) == false)
            direction = ZAPAD;
        else if( r == 0 && (r+c)%2 == 0 && isborder(map, r, c, SEVER) == false )
            direction = ZAPAD;
        else if( r == (map->rows-1) && ((r+c)&1) && isborder(map, r, c, JIH) == false )
            direction = VYCHOD;
    }
    //--lpath
    else if(leftright == 1){
        if( c == 0 && r%2 == 0 && isborder(map, r, c, ZAPAD) == false)
            direction = ZAPAD;
        else if( c == 0 && (r&1) && isborder(map, r, c, ZAPAD) == false)
            direction = SEVER;
        else if( c == (map->cols-1) && (r+c)%2 == 0 && isborder(map, r, c, VYCHOD) == false)
            direction = JIH;
        else if( c == (map->cols-1) && ((r+c)&1) && isborder(map, r, c, VYCHOD) == false)
            direction = VYCHOD;
        else if( r == 0 && (r+c)%2 == 0 && isborder(map, r, c, SEVER) == false )
            direction = VYCHOD;
        else if( r == (map->rows-1) && ((r+c)&1) && isborder(map, r, c, JIH) == false )
            direction = ZAPAD;
    }

    return direction;
}

void quest(Map *map, int r, int c, int leftright, int direction){
    int l_turn = 3;     //variable for clock-wise turn for --lpath
    int r_turn = 1;     //variable for counterclock-wise turn for --rpath
    //--rpath
    if(leftright == 0){
        while( r >= 0 && r <= (map->rows-1) && c >= 0 && c <= (map->cols-1) ){
            if(isborder(map, r, c, direction) == true){
                direction = (direction+r_turn)%POCET_SMERU;
                continue;
            }
            else if(isborder(map, r, c, direction) == false){
                printf("%d,%d\n", r+1, c+1);
                r = r + d[direction].dr;
                c = c + d[direction].ds;
                direction = (direction+l_turn)%POCET_SMERU;
            }
        }
        return;
    }
    //--lpath
    else if(leftright == 1){
        while( r >= 0 && r <= (map->rows-1) && c >= 0 && c <= (map->cols-1) ){
            if(isborder(map, r, c, direction) == true){
                direction = (direction+l_turn)%POCET_SMERU;
                continue;
            }
            else if(isborder(map, r, c, direction) == false){
                printf("%d,%d\n", r+1, c+1);
                r = r + d[direction].dr;
                c = c + d[direction].ds;
                direction = (direction+r_turn)%POCET_SMERU;
            }
                
        }
        return;
    }
}

int main(int argc, char * argv[]){
    //the program can`t be run with only 1 argument
    if(argc==1){
		fprintf(stderr,"error: few arguments (for reference enter `--help`)\n");
		return 1;
    }
    //`--help`
    else if(argc==2){
        if(help(argc, argv)== 0)
            return 0;
        else
            return 1;
    }

    //if there is a text file specified in command line than it will be our file describing a map
    for(int i = 0; i<argc; i++){
        if(strcmp(argv[i],".txt")>0)
            file = fopen(argv[i],"r");
    }

    //inicialization of map
    Map map;
    //variable to determine data validity in file
    int validity = ctor(argc, argv, &map, file);
    //`--test`
    if(argc==3){
        if(strcmp(argv[1],"--test")==0 && strcmp(argv[2],".txt")>0){
            if(validity == 0){
                printf("Valid\n");
                dctor(&map);
                fclose(file);
                return 0;
            }
        }
        else
            //is that ok with(out) free & re-allok???
            fprintf(stderr,"error: invalid arguments (for reference enter `--help`)\n");
    }
	//`--rpath` or `--lpath`
	else if(argc==5){
        if(validity == 0){
            if((strcmp(argv[1],"--rpath")==0 || strcmp(argv[1],"--lpath")==0) && strcmp(argv[4],".txt")>0 && arg_check(argc, argv, &map) == 0 ){
                //leftright 0 - right
                //leftright 1 - left

                int leftright;
                int r = atoi(argv[2])-1;
                int c = atoi(argv[3])-1;

                if (strcmp(argv[1], "--rpath") == 0)
                    leftright = 0;
                else if(strcmp(argv[1], "--lpath") == 0) 
                    leftright = 1;


                int direction = start_border(&map, r, c, leftright);
                quest(&map, r, c, leftright, direction);
                
                dctor(&map);
                fclose(file);
                return 0;
            }
        }
    }
    else
        fprintf(stderr,"error: invalid arguments (for reference enter `--help`)\n");
    dctor(&map);
    fclose(file);
    return 1;
}
