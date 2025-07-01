#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int str_to_int(char* str){
  int exp=1;
  int ans=0;
  // if(strlen(str)==1)
  //   return str[0]-'0';
  for(int i= strlen(str)-1; i>=0; --i){
    ans += exp*(str[i]-'0');
    exp*=10;
    if(i==1 && str[0]=='-'){
      ans = -ans;
      break;
    }
  }
  return ans;
}

void getvecsize(char vecsize[1000], int* sizearr){
  int n=0 ,m=0;
  char* ivec = vecsize;//n

  vecsize = strtok(ivec,"\n");//n
  vecsize = strtok(vecsize," ");
  n = str_to_int(vecsize);
  vecsize = strtok(NULL," ");//m
  m = str_to_int(vecsize);

  sizearr[0] = n;
  sizearr[1] = m;
}

void build(FILE* input_file, int*  to_build[], int n, int m){
  char element[1000];
  
  for(int i=0; i<m;++i){ 
    fgets(element,1000,input_file);
    char* str = strtok(element,"\n");
    str = strtok(str," ");
    for(int j=0 ;j<n;++j){
      to_build[i][j] = str_to_int(str);
      str = strtok(NULL," ");
    }
  }
}
int main(int argc,char ** argv){
  if(argc<3){
    printf("Usage:\n");
    printf("./main <input file> <output file>\n");
    exit(0);
  }

  char * input_file_name = argv[1];
  char * output_file_name = argv[2];

  FILE * input_file = fopen(input_file_name,"r");
  FILE * output_file = fopen(output_file_name,"w");
  
  if(input_file == NULL){
    printf("Error: unable to open input file %s\n",input_file_name);
    exit(0);
  }

  if(output_file == NULL){
    printf("Error: unable to open output file %s\n",output_file_name);
    fclose(input_file);
    exit(0);
  }

  /* YOUR CODE HERE */
  char vecsize[1000]="";
  int sizenm1[2];// m x n matrix
  int sizenm2[2];// m x n matrix

  fgets(vecsize,1000,input_file);
  getvecsize(vecsize,sizenm1);
  int m_ima = sizenm1[1];
  int n_ima = sizenm1[0];
 
  int** image = malloc(sizeof(int*)*m_ima);
  for(int i=0; i<m_ima ;++i)
    image[i] = malloc(sizeof(int)*n_ima);
  build(input_file,image,sizenm1[0],sizenm1[1]);
  
  fgets(vecsize,1000,input_file); 
  getvecsize(vecsize,sizenm2);
  int m_ker = sizenm2[1];
  int n_ker = sizenm2[0];
  int ** kernel = malloc(sizeof(int*)*m_ker);
  for(int i=0; i<m_ker ;++i)
    kernel[i] = malloc(sizeof(int)*n_ker);
  build(input_file,kernel,sizenm2[0],sizenm2[1]);
  
  int M = sizenm1[1]-sizenm2[1]+1;
  int N = sizenm1[0]-sizenm2[0]+1;
  int ** convenlution = malloc(sizeof(int*)*M);
  for(int i=0; i<M ;++i)
    convenlution[i] = calloc(N,sizeof(int));

  for(int i = 0; i < M; ++i){
    for(int j = 0; j < N; ++j){
      for(int a = 0; a < m_ker/*3*/; ++a){
        for(int b = 0;b < n_ker/*2*/; ++b){
          convenlution[i][j] += image[a+i][b+j]*kernel[a][b];
        }
      }
    }
  }
  for(int i=0; i < M;++i){
    for(int j=0; j < N;++j)
      fprintf(output_file,"%d ",convenlution[i][j]);
    fprintf(output_file,"\n");
  }

  for(int i=0; i<m_ima ;++i)
    free(image[i]);
  for(int i=0; i<m_ker ;++i)
    free(kernel[i]);
  for(int i=0; i<M ;++i)
    free(convenlution[i]);
  free(image);
  free(kernel);
  free(convenlution);

  fclose(input_file);
  fclose(output_file);

  return 0;
}
