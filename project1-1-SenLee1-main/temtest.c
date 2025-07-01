#include<stdio.h>
#include<string.h>
int main(){
    char a[]="lb rd rs1 imm\n";
    // char a[]="lb rd imm(rs1)\n";

    // char *b=strtok(a," ");
    //     printf("%s\n",b);

    // char *b=strtok(a,"\n");
    //     printf("%s\n",b);
    char *b=strtok(a," ");
        printf("%s\n",b);

    b=strtok(NULL," ");
        printf("%s\n",b);


        // b=strtok(NULL," ");
        // printf("%s\n",b);
        b=strtok(NULL," ");
        printf("%s\n",b);
        b=strtok(NULL," ");
        printf("%s\n",b);
        printf("%d\n",strlen(b));


    // b=strtok(NULL,"\n");
    //     printf("%s\n",b);
    
    // b=strtok(NULL," ");
    //     printf("%s\n",b);
            
    //     printf("%s\n",b);


    //     printf("%s\n",b);


        // b=strtok(NULL," ");
    // }


    return 0;
}