#include "../inc/assembler.h"
#include "../inc/util.h"

/* DO NOT MODIFY THE GIVEN API*/
typedef enum{
  R=1,
  I=2,
  S=3,
  SB=4,
  U=5,
  UJ=6,
  LI=7,
  NON=0
} Type;
typedef struct {
  uint32_t func3; 
  uint32_t func7;
  int32_t imm;
  uint32_t opcode;
  uint32_t rd;
  uint32_t rs1;
  uint32_t rs2;
  int32_t immmin;
  int32_t immmax;
  Type type;
}Instruction;
uint32_t getnum(char* iregister);

int check_reg_error(Instruction* data){
  return data->rs1==0xfff || data->rs2== 0xfff || data->imm>data->immmax||data->imm<data->immmin || data->rd==0xfff;
 }

int getimm(Instruction* data,char* name_code){
  int len=strlen(name_code);
        int exp=1;
        for(int i=len-1;i>=1;--i){
          if(name_code[i]>'9'||name_code[i]<'0'){
            data->rd = 0xfff;
            return 0;
          }
          data->imm+=(name_code[i]-'0')*exp;
          exp*=10;
        }
        
        if(name_code[0]=='-')
          data->imm *=-1;
        else if(name_code[0]>'9'||name_code[0]<'0')
          return 0;
        else
          data->imm+=(name_code[0]-'0')*exp;
        return 1;
}

int R_type(Instruction* data,char* name_code, uint32_t* code){
      name_code=strtok(NULL," ");

      data->rd =getnum(name_code);
      name_code=strtok(NULL," ");

      data->rs1=getnum(name_code);
      name_code=strtok(NULL,"\n");
      
      data->rs2 =getnum(name_code);
      *code=(data->func7<<25) + (data->func3<<12) + (data->opcode) + (data->rd<<7) + (data->rs1<<15) + (data->rs2<<20);
      return check_reg_error(data);
}

int S_type(Instruction* data,char* name_code, uint32_t* code){
  name_code=strtok(NULL," ");
      
      data->rs2 =getnum(name_code); 
      name_code=strtok(NULL," ");
        name_code=strtok(name_code,"(");// imm

        char* temname_code=strtok(NULL,"(");
        name_code = strtok(name_code,"\n");
        if(getimm(data,name_code)==0){
          return 1;
        }
        name_code=strtok(temname_code,")");
        data->rs1=getnum(name_code);
        *code=((data->imm>>5)<<25) + (data->rs2<<20) + (data->rs1<<15) + (data->func3<<12) + ((data->imm &((1<<5)-1))<<7) + data->opcode;
        return check_reg_error(data);
}

int SB_type(Instruction* data,char* name_code, uint32_t* code){
  name_code=strtok(NULL," ");
  
      data->rs1 =getnum(name_code);
      name_code=strtok(NULL," ");
      char* zname_code=name_code;
        data->rs2=getnum(name_code);
        name_code=strtok(NULL," ");
      if(name_code==NULL){ //bnez beqz
        data->rs2=0;
        name_code=zname_code;
      }
      name_code = strtok(name_code,"\n");
      if(getimm(data,name_code)==0){
        return 1;
      }

      int32_t tem = data->imm & ((1ull<<13)-1);
      *code= ((tem>>12)<<31) + (((tem>>5)&((1<<6)-1))<<25) + (data->rs2<<20) + (data->rs1<<15) + (data->func3<<12) + (((tem>>1)&((1<<4)-1))<<8) + (((tem>>11)&1)<<7) + data->opcode;
      return check_reg_error(data);
}

int U_type(Instruction* data,char* name_code, uint32_t* code){
  name_code=strtok(NULL," ");
      data->rd=getnum(name_code);
      name_code=strtok(NULL," ");
      name_code = strtok(name_code,"\n");
      if(getimm(data,name_code)==0)
        return 1;
      int tem = data->imm & ((1<<20)-1);
      *code = (tem<<12) + (data->rd<<7) + data->opcode;
      return check_reg_error(data);
}

int UJ_type(Instruction* data,char* name_code, uint32_t* code){
  name_code=strtok(NULL," ");
      char* j_namecode=name_code;
      data->rd=getnum(name_code);
      name_code=strtok(NULL," ");
      if(name_code==NULL) {// j label
        data->rd=0;
        name_code=j_namecode; 
      }
      name_code = strtok(name_code,"\n");
      if(getimm(data,name_code)==0)
        return 1;
      if(check_reg_error(data)==1)
        return 1;
      else{
        int32_t tem = data->imm;
        *code=0;
        uint32_t negimm=0;
        if (tem<0){
          negimm=(-tem);
          int i=0;
          tem=0;
          while(i<21){
            tem+=(((negimm>>i)+1)&1)<<i;
            ++i;
          }
          ++tem;
        }  
        tem>>=1;
        negimm=tem; 
        *code += ((negimm>>19)<<31) + (((negimm)&((1<<10)-1))<<21) + (((negimm >>10)&1)<<20) + (((negimm >> 11)&((1<<8)-1))<<12) + (data->rd<<7) + data->opcode;
        name_code = strtok(NULL," ");
      }
      return check_reg_error(data);
}

int LI_type(Instruction* data,char* name_code, uint32_t* code,FILE* output_file){
    name_code=strtok(NULL," ");
      data->rd=getnum(name_code);
      name_code=strtok(NULL," ");
      name_code = strtok(name_code,"\n");
      if(getimm(data,name_code)==0||check_reg_error(data)==1)
        return 1;
          if(data->imm>=-2048 && data->imm <=2047){//addi
            *code = (data->imm<<20) + (0<<15) + (0<<12) + (data->rd<<7) + 0x13;
            dump_code(output_file,*code);
          } 
          else{
              int32_t upper = data->imm >> 12;
              int32_t lower = data->imm & 0xfff;
              if((lower>>11) == 1){
                upper++;
                lower-=(1<<12);
              }
              *code = (upper << 12) + (data->rd << 7) + 0x37; 
              dump_code(output_file, *code);
              *code = (lower << 20) + (data->rd << 15) + (0 << 12) + (data->rd << 7) + 0x13; 
              dump_code(output_file, *code);
          }
          return 0;
}

int I_type(Instruction* data,char* name_code, uint32_t* code){
  name_code=strtok(NULL," ");
      if(data->func7==2){
        name_code=strtok(name_code,"\n");
      }
      data->rd =getnum(name_code);
      name_code=strtok(NULL," ");
      char* temname_code = name_code;
      name_code=strtok(NULL," ");
      if(name_code==NULL&&data->func7==0){// offset
        name_code=strtok(temname_code,"(");// imm
        temname_code=strtok(NULL,"(");
        name_code = strtok(name_code,"\n");
        if(getimm(data,name_code) ==0){
          return 1;
        }
        name_code=strtok(temname_code,")");
        data->rs1=getnum(name_code); 
      }
      else{
        if(data->func7==1){// mv
          name_code=strtok(temname_code,"\n");
          data->rs1=getnum(temname_code);
          data->imm=0;
        }
        else if(data->func7==2){// jr rs1
          data->rs1= data->rd;
          data->rd=0;
          data->imm=0;
        }
        else{
          data->rs1=getnum(temname_code);
          name_code = strtok(name_code,"\n");
          if(getnum(name_code)!=0xfff){ // addi rd rs1 rs2
            return 1;
          }
          name_code = strtok(name_code,"\n");
          if(getimm(data,name_code)==0){
            return 1;
          }
        }
      }
      *code= ((data->imm & ((1ull<<12)-1))<<20) + (data->rs1<<15) + (data->func3<<12) + (data->rd<<7) + data->opcode;
      return check_reg_error(data);
}

Instruction* Getdata(char* name, Instruction* ret);
int assembler(FILE *input_file,
              FILE *output_file) {
  /*YOUR CODE HERE*/
  char istr[30]="";
  uint32_t* code = malloc(sizeof(uint32_t));
  Instruction* data = malloc(sizeof(Instruction));
  while(fgets(istr,30,input_file)){
    int error = 0;
    if(istr[0]=='e'){
      *code=(7<<4)+3;
      dump_code(output_file,*code);
      continue;
    }
    char* name_code=strtok(istr," ");
    *code=0;
    data = Getdata(name_code,data);
    if(data->type==NON){//Error
      dump_error_information(output_file);
      continue;
    }
    if(data->type==R)
      error = R_type(data,name_code,code);
    else if(data->type==I)
      error = I_type(data,name_code,code);
    else if(data->type==S)
      error = S_type(data,name_code, code);
    else if(data->type==SB)
      error = SB_type(data,name_code,code);
    else if(data->type==U)
      error = U_type(data,name_code,code);
    else if(data->type==UJ)
      error = UJ_type(data,name_code,code);
    else if(data->type==LI)
      error = LI_type(data,name_code,code,output_file);
    if(error == 1)
      dump_error_information(output_file);
    else if(data->type != LI)
      dump_code(output_file,*code);
  }
  free(code);
  free(data);
  return 0;
}
uint32_t getnum(char* iregister){
  if(strcmp(iregister,"zero")==0||strcmp(iregister,"x0")==0||strcmp(iregister,"x00")==0) return 0;
  if(strcmp(iregister,"ra")==0||strcmp(iregister,"x1")==0||strcmp(iregister,"x01")==0) return 1; 
  if(strcmp(iregister,"sp")==0||strcmp(iregister,"x2")==0||strcmp(iregister,"x02")==0) return 2;
  if(strcmp(iregister,"gp")==0||strcmp(iregister,"x3")==0||strcmp(iregister,"x03")==0)  return 3;
  if(strcmp(iregister,"tp")==0||strcmp(iregister,"x4")==0||strcmp(iregister,"x04")==0) return 4;
  if(strcmp(iregister,"t0")==0||strcmp(iregister,"x5")==0||strcmp(iregister,"x05")==0||strcmp(iregister,"t-0")==0)return 5;
  if(strcmp(iregister,"t1")==0||strcmp(iregister,"x6")==0||strcmp(iregister,"x06")==0||strcmp(iregister,"t-1")==0)return 6;
  if(strcmp(iregister,"t2")==0||strcmp(iregister,"x7")==0||strcmp(iregister,"x07")==0||strcmp(iregister,"t-2")==0)return 7;
  if(strcmp(iregister,"s0")==0||strcmp(iregister,"x8")==0||strcmp(iregister,"x08")==0||strcmp(iregister,"s-0")==0)return 8; 
  if(strcmp(iregister,"s1")==0||strcmp(iregister,"x9")==0||strcmp(iregister,"x09")==0||strcmp(iregister,"s-1")==0)return 9;
  if(strcmp(iregister,"a0")==0||strcmp(iregister,"x10")==0||strcmp(iregister,"a-0")==0)return 10;    
  if(strcmp(iregister,"a1")==0||strcmp(iregister,"x11")==0||strcmp(iregister,"a-1")==0)return 11;    
  if(strcmp(iregister,"a2")==0||strcmp(iregister,"x12")==0||strcmp(iregister,"a-2")==0)return 12;    
  if(strcmp(iregister,"a3")==0||strcmp(iregister,"x13")==0||strcmp(iregister,"a-3")==0)return 13;    
  if(strcmp(iregister,"a4")==0||strcmp(iregister,"x14")==0||strcmp(iregister,"a-4")==0)return 14;    
  if(strcmp(iregister,"a5")==0||strcmp(iregister,"x15")==0||strcmp(iregister,"a-5")==0)return 15;    
  if(strcmp(iregister,"a6")==0||strcmp(iregister,"x16")==0||strcmp(iregister,"a-6")==0)return 16;    
  if(strcmp(iregister,"a7")==0||strcmp(iregister,"x17")==0||strcmp(iregister,"a-7")==0)return 17;    
  if(strcmp(iregister,"s2")==0||strcmp(iregister,"x18")==0||strcmp(iregister,"s-2")==0)return 18;    
  if(strcmp(iregister,"s3")==0||strcmp(iregister,"x19")==0||strcmp(iregister,"s-3")==0)return 19;   
  if(strcmp(iregister,"s4")==0||strcmp(iregister,"x20")==0||strcmp(iregister,"s-4")==0) return 20;   
  if(strcmp(iregister,"s5")==0||strcmp(iregister,"x21")==0||strcmp(iregister,"s-5")==0)return 21;
  if(strcmp(iregister,"s6")==0||strcmp(iregister,"x22")==0||strcmp(iregister,"s-6")==0)return 22;    
  if(strcmp(iregister,"s7")==0||strcmp(iregister,"x23")==0||strcmp(iregister,"s-7")==0) return 23;   
  if(strcmp(iregister,"s8")==0||strcmp(iregister,"x24")==0||strcmp(iregister,"s-8")==0) return 24;   
  if(strcmp(iregister,"s9")==0||strcmp(iregister,"x25")==0||strcmp(iregister,"s-9")==0) return 25;  
  if(strcmp(iregister,"s10")==0||strcmp(iregister,"x26")==0||strcmp(iregister,"s-10")==0) return 26;   
  if(strcmp(iregister,"s11")==0||strcmp(iregister,"x27")==0||strcmp(iregister,"s-11")==0)return 27;    
  if(strcmp(iregister,"t3")==0||strcmp(iregister,"x28")==0||strcmp(iregister,"t-3")==0)return 28;    
  if(strcmp(iregister,"t4")==0||strcmp(iregister,"x29")==0||strcmp(iregister,"t-4")==0)  return 29;  
  if(strcmp(iregister,"t5")==0||strcmp(iregister,"x30")==0||strcmp(iregister,"t-5")==0)return 30;    
  if(strcmp(iregister,"t6")==0||strcmp(iregister,"x31")==0||strcmp(iregister,"t-6")==0)return 31;
    
  return 0xfff;
}

Instruction* Getdata(char* name,Instruction* ret){
  ret->type = NON;
  ret->imm=0;
  ret->func7=0;
  ret->func3=0;
  ret->immmax=2047;
  ret->immmin=-2048;
  ret->rd = 0;
  ret->rs1=0;
  ret->rs2=0;
  if(strcmp(name,"jal")==0||strcmp(name,"j")==0){
    ret->type=UJ;
    ret->immmax=1048575;
    ret->immmin=-1048576;
    if(strcmp(name,"j")==0)
      ret->func7=1;
    ret->opcode=0x6f;
  }
  if(ret->type !=NON)
    return ret;
  ret->opcode = 51;
  if(strcmp(name,"add")==0)
    ret->type=R;
  else if(strcmp(name,"mul")==0){
    ret->type=R;
    ret->func7=1;
  }
  else if(strcmp(name,"sub")==0){
    ret->type=R;
    ret->func7=1ull<<5;
  }
  else if(strcmp(name,"sll")==0){
    ret->type=R;
    ret->func3=1;
  }
  else if(strcmp(name,"mulh")==0){
    ret->type=R;
    ret->func3=1;
    ret->func7=1;
  }
  else if(strcmp(name,"slt")==0){
    ret->type=R;
    ret->func3=2;
  }
  else if(strcmp(name,"sltu")==0){
    ret->type=R;
    ret->func3=3;
  }
  else if(strcmp(name,"xor")==0){
    ret->type=R;
    ret->func3=4;
  }
  else if(strcmp(name,"div")==0){
    ret->type=R;
    ret->func3=4;
    ret->func7=1;
  }
  else if(strcmp(name,"srl")==0){
    ret->type=R;
    ret->func3=5;
  }
  else if(strcmp(name,"sra")==0){
    ret->type=R;
    ret->func3=5;
    ret->func7=2<<4;
  }
  else if(strcmp(name,"or")==0){
    ret->type=R;
    ret->func3=6;
  }
  else if(strcmp(name,"rem")==0){
    ret->type=R;
    ret->func3=6;
    ret->func7=1;
  }
  else if(strcmp(name,"and")==0){
    ret->type=R;
    ret->func3=7;
  }
  if(ret->type !=NON)
    return ret;
  ret->opcode = 3;
  if(strcmp(name,"lb")==0)
    ret->type=I;
  
  else if(strcmp(name,"lh")==0){
    ret->type=I;
    ret->func3=1;
  }
  else if(strcmp(name,"lw")==0){
    ret->type=I;
    ret->func3=2;
  }
  else if(strcmp(name,"lbu")==0){
    ret->type=I;
    ret->func3=4;
  }
  else if(strcmp(name,"lhu")==0){
    ret->type=I;
    ret->func3=5;
  }
  if(ret->type!=NON)  
    return ret;
  ret->opcode = (1<<4)+3;
  if(strcmp(name,"addi")==0||strcmp(name,"mv")==0){
    ret->type=I;
    if(strcmp(name,"mv")==0)
      ret->func7=1;
  }
  else if(strcmp(name,"slli")==0){
    ret->type=I;
    ret->func3=1;
    ret->immmax=31;
    ret->immmin=0;
  }
  else if(strcmp(name,"slti")==0){
    ret->type=I;
    ret->func3=2;
  }
  else if(strcmp(name,"sltiu")==0){
    ret->type=I;
    ret->func3=3;
  }
  else if(strcmp(name,"xori")==0){
    ret->type=I;
    ret->func3=4;
  }
  else if(strcmp(name,"srli")==0){
    ret->type=I;
    ret->func3=5;
    ret->immmax=31;
    ret->immmin=0;
  }
  else if(strcmp(name,"srai")==0){
    ret->type=I;
    ret->func3=5;
    ret->imm=1<<10;
    ret->immmax=31+(1<<10);
    ret->immmin=1<<10;
  }
  else if(strcmp(name,"ori")==0){
    ret->type=I;
    ret->func3=6;
  }
  else if(strcmp(name,"andi")==0){
    ret->type=I;
    ret->func3=7;
  }
  else if(strcmp(name,"jalr")==0||strcmp(name,"jr")==0){
    ret->type=I;
    if(strcmp(name,"jr")==0)
      ret->func7=2;
    ret->opcode=0x67;
  }
  else if(strcmp(name,"ecall")==0){
    ret->type=I;
    ret->opcode=(7<<4)+3;
  }
  else if(strcmp(name,"sb")==0){
    ret->type=S;
    ret->opcode=(2<<4)+3;
  }
  else if(strcmp(name,"sh")==0){
    ret->type=S;
    ret->func3=1;
    ret->opcode=(2<<4)+3;
  }
  else if(strcmp(name,"sw")==0){
    ret->type=S;
    ret->func3=2;
    ret->opcode=(2<<4)+3;
  }
  if(ret->type!=NON)
    return ret;
  ret->immmin = -4096;
  ret->immmax = 4094;
  ret->opcode = 0x63;
  if(strcmp(name,"beq")==0||strcmp(name,"beqz")==0){
    ret->type=SB;
    if(strcmp(name,"beqz")==0)
      ret->func7=1;
  }

  else if(strcmp(name,"bne")==0||strcmp(name,"bnez")==0){
    ret->type=SB;
    ret->func3=1;
    if(strcmp(name,"bnez")==0)
      ret->func7=1;
  }
  else if(strcmp(name,"blt")==0){
    ret->type=SB;
    ret->func3=4;
  }
  else if(strcmp(name,"bge")==0){
    ret->type=SB;
    ret->func3=5;
  }
  else if(strcmp(name,"bltu")==0){
    ret->type=SB;
    ret->func3=6;
  }
  else if(strcmp(name,"bgeu")==0){
    ret->type=SB;
    ret->func3=7;
  }
  else if(strcmp(name,"auipc")==0){
    ret->type=U;
    ret->immmax=1048575;
    ret->immmin=0;
    ret->opcode=(1<<4)+7;
  }
  else if(strcmp(name,"lui")==0){
    ret->type=U;
    ret->immmax=1048575;
    ret->immmin=0;
    ret->opcode=(3<<4)+7;
  }
  else if(strcmp(name,"li")==0){
    ret->type=LI;
    ret->immmax = 0x7fffffff;
    ret->immmin = -ret->immmax;
    ret->func7=(3<<4)+7;//opcode if lui
  }
  else
    ret->type=NON;
return ret;
}