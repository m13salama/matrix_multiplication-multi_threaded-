#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

//create matrices and rows, columns
int arow,acol,brow,bcol;
int** arra ;
int** arrb ;
long long int** arrc;
char *files[3];

//struct to store row, column of a element
typedef struct Elementclear
{
	int row;
	int col;
} element ;

//function to generate random matrix a,b
void generateMatrix(){
	char* extension = ".txt";
    char fileSpec1[strlen(files[0])+strlen(extension)+1];
	snprintf( fileSpec1, sizeof( fileSpec1 ), "%s%s", files[0], extension );
	char fileSpec2[strlen(files[1])+strlen(extension)+1];
	snprintf( fileSpec2, sizeof( fileSpec2 ), "%s%s", files[1], extension );
	FILE *fptr;
	//Creating the Matrix A
	printf("Enter the row size for martix A(<1001):");
	scanf("%d", &arow);
	printf("Enter the column size for martix A(<1001):");
	scanf("%d", &acol);
	fptr = fopen(fileSpec1, "w");
	fprintf(fptr, "row=%d col=%d\n", arow, acol);
	for(int i=0; i<arow; i++){
		for(int j=0; j<acol; j++){
			fprintf(fptr, "%d ", rand()%10);
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);

	//Creating the Matrix B
	printf("Enter the row size for martix B(<1001):");
	scanf("%d", &brow);
	printf("Enter the column size for martix B(<1001):");
	scanf("%d", &bcol);
	fptr = fopen(fileSpec2, "w");
	fprintf(fptr, "row=%d col=%d\n", brow, bcol);
	for(int i=0; i<brow; i++){
		for(int j=0; j<bcol; j++){
			fprintf(fptr, "%d ", rand()%10);
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);	
}
// print the result matrix to the result file
void write_file(char *extension){
	FILE *fptr;
	char *extend, *print;
	//write the header line and no of rows, columns
	if(strcmp(extension,"row")==0){
		extend = "_per_row.txt";
		strcpy(print, "Method: A thread per row");
	}
	else if(strcmp(extension,"element")==0){
		extend = "_per_element.txt";
		strcpy(print, "Method: A thread per element");
	}
	else if(strcmp(extension,"matrix")==0){
		extend = "_per_matrix.txt";
		strcpy(print, "Method: A thread per matrix");
	}
	//write the result matrix
    char fileSpec1[strlen(files[2])+strlen(extend)+1];
	snprintf( fileSpec1, sizeof( fileSpec1 ), "%s%s", files[2], extend );
	fptr = fopen(fileSpec1, "w");
	fprintf(fptr, "%s\n", print);
	fprintf(fptr, "row=%d col=%d\n", arow, bcol);
	for (int i = 0; i < arow; i++){
		for (int j = 0; j < bcol; j++)
		{
			fprintf(fptr, "%lld ", arrc[i][j]);
		}
		fprintf(fptr, "\n");	
	}
	fclose(fptr);
}
//get the first line of the file and get the no of rows and columns from it
void set_rows_cols(int id, char *str){
	char ch; 
	int rows,i,digit,number=0; 
	for(i=0;i<strlen(str);i++) 
	{ 
		ch = str[i]; 
		if(ch>='0' && ch<='9') //to confirm it's a digit 
		{ 
			digit = ch - '0'; 
			number = number*10 + digit; 
		}
		if(ch == ' ' || ch == '\t'){
			rows = number;
			number = 0;
		} 
	}
	if(id == 1){
		arow = rows;
		acol = number;
	}else{
		brow = rows;
		bcol = number;
	}
}
//read the file and store the matrix in 2d array
void read_matrix(){
	char* extension = ".txt";
    char fileSpec1[strlen(files[0])+strlen(extension)+1];
	snprintf( fileSpec1, sizeof( fileSpec1 ), "%s%s", files[0], extension );
	char fileSpec2[strlen(files[1])+strlen(extension)+1];
	snprintf( fileSpec2, sizeof( fileSpec2 ), "%s%s", files[1], extension );
	char str[1000];
	int num;

	FILE *fptr;

	//Accessing file a.txt and storing value in matrixA
    snprintf( fileSpec1, sizeof( fileSpec1 ), "%s%s", files[0], extension );
	fptr = fopen(fileSpec1, "r");
    fscanf(fptr, "%[^\n]", str);
	set_rows_cols(1, str);
	arra = (int**)malloc(arow * sizeof(int*));
    for (int i = 0; i < arow; i++)
        arra[i] = (int*)malloc(acol * sizeof(int));
	for(int i=0; i<arow; i++){
	 	for(int j=0; j<acol; j++){
			fscanf(fptr, "%d", &num);
			arra[i][j] = num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	snprintf( fileSpec2, sizeof( fileSpec2 ), "%s%s", files[1], extension );	
	fptr = fopen(fileSpec2, "r");
	fscanf(fptr, "%[^\n]", str);
	set_rows_cols(2, str);
	arrb = (int**)malloc(brow * sizeof(int*));
    for (int i = 0; i < brow; i++)
        arrb[i] = (int*)malloc(bcol * sizeof(int));
	for(int i=0 ; i<brow; i++){
		for(int j=0; j<bcol; j++){
			fscanf(fptr, "%d", &num);
			arrb[i][j] = num;
		}
	}
	fclose(fptr);
	//create matrix c to use it later in storing the result
	if(arrc == 0){
		arrc = (long long int**)malloc(arow * sizeof(long long int*));
		for (int i = 0; i < arow; i++){
			arrc[i] = (long long int*)malloc(bcol * sizeof(long long int));
		}
	}

}
//the columns of first matrix must equal the rows of the second else exit
void check_dimension(){
	if(acol != brow){
	 	printf("Matix Multiplication is not possible due to dimension conflicts\n");
		exit(5);
	}
}
//first approach thread per matrix
void *mat_mat(){
	check_dimension();
	//Matrix Multiplication
	for(int i=0; i<arow; i++){
		for(int j=0; j<bcol; j++){
			long long int sum = 0;
			for(int k=0; k<acol; k++){
				sum = sum + arra[i][k] * arrb[k][j];
			}
			arrc[i][j] = sum;
		}
	}	
}

//first approach thread per row
void *row_mat(int row){
	//Matrix Multiplication by row
	check_dimension();
	for(int j=0; j<bcol; j++){
		long long int sum = 0;
		for(int k=0; k<acol; k++){
			sum = sum + arra[row][k] * arrb[k][j];
		}
		arrc[row][j] = sum;
	}
}

//first approach thread per element
void *element_mat(void *ele){
		check_dimension();
	element* e1=(element*)ele;
	//Matrix Multiplication by row
	long long int sum = 0;
	for(int k=0; k<acol; k++){
		sum = sum + arra[(*e1).row][k] * arrb[k][(*e1).col];
	}
	arrc[(*e1).row][(*e1).col] = sum;
	free(ele);
}

//create threads of the first approach
void mult_per_matrix(){
	pthread_t thread;
	if (pthread_create(&thread, NULL, &mat_mat, NULL) != 0) {
		perror("Failed to created thread");
	}
	if (pthread_join(thread, NULL) != 0) {
		perror("Failed to join thread");
	}
	//write to file c
	char* extension = "matrix";
	write_file(extension);
}
//create threads of the second approach
void mult_per_row(){
	pthread_t th[arow];
    for (int i = 0; i < arow; i++) {
        if (pthread_create(&th[i], NULL, &row_mat, i) != 0) {
            perror("Failed to created thread");
        }
    }
    for (int i = 0; i < arow; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
	//write to file c
	char* extension = "row";
	write_file(extension);
}

//create threads of the third approach
void mult_per_element(){
	pthread_t th[arow][bcol];
    int i;
    for (i = 0; i < arow; i++) {
		for (int j = 0; j < bcol; j++)
		{
			element *e = (element*)malloc(sizeof(element*));
        	e->row = i;
			e->col = j;
			if (pthread_create(&th[i][j], NULL, &element_mat,(void *) e) != 0) {
				perror("Failed to created thread");
			}
		}

    }
    for (i = 0; i < arow; i++) {
		for (int j = 0; j < bcol; j++)
			if (pthread_join(th[i][j], NULL) != 0) {
				perror("Failed to join thread");
			}
    }
	//write to file c
	char* extension = "element";
	write_file(extension);
}

int main(int argc, char *argv[]){
	struct timeval stop, start;
    if( argc == 4) {
		for (int i = 0; i < 3; i++){
			files[i]= (char*)malloc((strlen(argv[i+1]))*sizeof(char));
			strcpy(files[i],argv[1+i]);
		}

	}else if(argc == 1){
		files[0]= (char*)malloc((1)*sizeof(char));
		strcpy(files[0],"a");
		files[1]= (char*)malloc((1)*sizeof(char));
		strcpy(files[1],"b");
		files[2]= (char*)malloc((1)*sizeof(char));
		strcpy(files[2],"c");

	}else {
		printf("you should enter name of the 3 files\n");
		exit(1);
	}
	//un comment next line if you want to generate random matrices
	//generateMatrix();
	read_matrix();
	
	//calculate time of first approach
	gettimeofday(&start, NULL); //start checking time
	mult_per_matrix();
	gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken for matrix threads %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for matrix threads : %lu\n", stop.tv_usec - start.tv_usec);

	//calculate time of second approach
	gettimeofday(&start, NULL); //start checking time
	mult_per_row();
	gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken for row threads %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for row threads : %lu\n", stop.tv_usec - start.tv_usec);

	//calculate time of third approach
	gettimeofday(&start, NULL); //start checking time
	mult_per_element();
	gettimeofday(&stop, NULL); //end checking time
    printf("Seconds taken for element threads %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for element threads : %lu\n", stop.tv_usec - start.tv_usec);

	//free the arrays
    for (int i = 0; i < arow; i++)
        free(arra[i]);
	free(arra);

    for (int i = 0; i < brow; i++)
        free(arrb[i]);
	free(arrb);

    for (int i = 0; i < arow; i++)
        free(arrc[i]);
	free(arrc);

    for (int i = 0; i < 3; i++)
		free(files[i]);
	return 0;
}