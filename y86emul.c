#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "y86emul.h"

int mypow(int base, int exp)
{
    int result=1;
    for(int i=0; i<exp; i++)
    {
        result = result*base;
    }
    
    return result;
}

int convert_to_num(unsigned char c) 
//converts hex values A-F into their numerical equivalents.
{
    
    switch(c)
    {
    case 'A':
    case 'a':
        return 10;
    case 'B':
    case 'b':
        return 11;
    case 'C':
    case 'c':
        return 12;
    case 'D':
    case 'd':
        return 13;
    case 'E':
    case 'e':
        return 14;
    case 'F':
    case 'f':
        return 15;
    }
    
    return c-48;
}


int convert_to_dec(const char* in)
{
    unsigned int mul=16;
    unsigned int res=0;
    int len=strlen(in);
    for(int i=0; i<len; i++)
    {
        char c=in[len-i-1];
        int num=convert_to_num(c);
        
        if(num<0 || num > mul-1)
        {
            
            return 0;
        }
        
        res = res + mypow(mul, i)*num;
    }
    
    return res;
}

int regs[8];
int prog_counter;

int stack[10];
int stack_top;
int zf;
int sf;
int of;

int mem_size;

int main(int argc, char* argv[])
{

    if(argc < 2)
    {
        fprintf(stderr,"Argument given is not in valid form\n");
        exit(0);
    }

    if (argv[1][0] == '-' && argv[1][1] == 'h')
    {
      printf("Usage: y86emul <y86 input file>\n");
      exit(0);
    }
    char* filename=argv[1];
    FILE* inFile=fopen(filename, "r");
    if(inFile==NULL)
    {
        fprintf(stderr,"The filename could not be found\n");
	exit(0);
    }
    
    stack_top=-1;
    

    char * line = NULL;
    unsigned char * mem = NULL;
    size_t len;
    prog_counter = -1;     
    while(getline(&line, &len, inFile) != -1)
    {
        char token1[10];
        char token2[10];
        char token3[1000];
        int num_tokens=sscanf(line, "%s %s %s", token1, token2, token3);
       
        if(strcmp(token1,".size")== 0)
        {
            
            
            if(num_tokens!=2)
            {
               
            }
            
            mem_size = convert_to_dec(token2);
            mem = malloc(  mem_size * sizeof(char));
            //printf("\nMEM size %d", num);
        }
        
        if(strcmp(token1,".byte") == 0)
        {
           
            if(num_tokens!=3)
            {
               
            }
            
            int addr = convert_to_dec(token2);
           
            if(strlen(token3)!=2)
            {
              
            }
            if(mem==NULL)
            {
               
            }

            unsigned char first=convert_to_num(token3[0]);
            first = first << 4;
            unsigned char second=convert_to_num(token3[1]);
            first = first | second;

            mem[addr]=first;
        }
        
        if(strcmp(token1,".string") == 0)
        {
            if(num_tokens!=3)
            {
                
            }

            if(mem==NULL)
            {
                
            }
            int addr = convert_to_dec(token2);

            char temp[len+1];
            strcpy(temp, line);
            char* token=strtok(temp, " \t\n");
            token=strtok(NULL, " \t\n");
            token=strtok(NULL, " \t\n");
            token++;
            while(token)
            {
                for(int i=0; i<strlen(token); i++)
                {
                    mem[addr]= token[i];
                    addr++;
                }
                mem[addr++]=' ';

                token=strtok(NULL, " \t\n");
            }
            
        }
        
        if(strcmp(token1,".long") == 0)
        {
           
            if(num_tokens!=3)
            {
                
            }
            
            int addr = convert_to_dec(token2);
            
           
            if(strlen(token3)!=4)
            {
               
            }
            
            if(mem==NULL)
            {
                // error 
            }
            
            mem[addr]=token3[0];
            mem[addr+1]=token3[1];
            mem[addr+2]=token3[2];
            mem[addr+3]=token3[3];
        }
        if(strcmp(token1, ".text") == 0)
        {
            if(num_tokens != 3)
            {
               
            }
            
            int  addr = convert_to_dec(token2);
            prog_counter = addr;
            
            for(int i = 0; i < strlen(token3); i=i+2)
            {
                unsigned char first = convert_to_num(token3[i]);
                
                first = first << 4;
                unsigned char second = convert_to_num(token3[i+1]);
                
                first = first | second;
                
                mem[addr + i/2] = first;
              
            }
        }
    }
    
    if(prog_counter==-1)
    {
        fprintf(stderr,".text not found.\n");
	    exit(0);
    }
    
    fclose(inFile);
    

    execute(mem);
}

void split_char(unsigned char in, unsigned char* first, unsigned char* second)
{
    unsigned char mask=15;
    mask = mask<<4;
    *first=in & mask; 
    *first = *first >> 4;
    
   
    mask=15;
    *second = in & mask;
    
}

int execute(unsigned char* mem)
{
    while(1)
    {
        unsigned char value = mem[prog_counter];
        unsigned char first;
        unsigned char second;
        split_char(value, &first, &second);
        
        if(first == 0)
        {
            prog_counter++;
            continue;
        }
        
        if(first == 1)
        {
            return 0;
        }
        
        if(first == 2)
        {
            rrmovl(mem);
        }
        
        if(first == 3)
        {
            irmovl(mem);
        }
        
        if(first == 4)
        {
            rmmovl(mem);
        }
        
        if(first ==5)
        {
            mrmovl(mem);
        }
        
        if(first ==6)
        {
            op1(mem, second);
        }
        
        if(first ==7)
        {
            jxx(mem, second);
        }
        if(first == 8)
        {
            call(mem);
        }
        
        if(first == 9)
        {
            ret(mem);
        }
        
        if(first == 10)
        {
            pushl(mem);
        }
        
        if(first == 11)
        {
            popl(mem);
        }
        
        if(first == 12)
        {
            readx(mem, second);
        }
        
        if(first == 13)
        {
            writex(mem, second);
        }
        
        if(first == 14)
        {
            movsbl(mem);
        }
        
        if(first > 14)
        {
            fprintf(stderr,"INS: Invalid instruction\n");
	    exit(1);
        }
    }
}
void rrmovl(unsigned char* mem)
{   
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value, &first, &second);
    
  
    if(first<0 || first >7 || second <0 || second > 7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
  
    regs[second]=regs[first];
    
    prog_counter++;
}

void irmovl(unsigned char* mem)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);
    
    prog_counter++;
    int intval=read_int(mem);
    if(second <0 || second > 7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
    regs[second] = intval;
    
    prog_counter++;
}

void rmmovl(unsigned char* mem)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    int intval;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);
    
    prog_counter++;
    intval = read_int(mem);
  
    if((regs[second] + intval) >=mem_size)
    {
      printf("\nADR: Out of Bounds Memory Address");
      exit(1);
    }
  
    write_int(mem, regs[first], regs[second] + intval);
    
    prog_counter++;
}

void mrmovl(unsigned char* mem)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    int intval;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);
    
    if(first<0 || first >7 || second <0 || second > 7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
  
    prog_counter++;
    intval = read_int(mem);
    if((regs[second] + intval) >=mem_size)
    {
      printf("\nADR: Out of Bounds Memory Address");
      exit(1);
    }
   
    regs[first]= read_int_mem(mem, regs[second] + intval);
    
    prog_counter++;
}

void op1(unsigned char* mem, int func)
{
    zf=0;
    sf=0;
    of=0;
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);
    if(first<0 || first >7 || second <0 || second > 7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
    if(func == 0)
    {

 	
	if(regs[first] > 0 && regs[second] > 0 && regs[first] + regs[second] < 0)
        {
          of = 1;
        }
        else if(regs[first] < 0 && regs[second] < 0 && regs[first] + regs[second] > 0)
        {
          of = 1;
        }
        else 
        {
          of = 0;
        }
        regs[second] = regs[second] + regs[first];
    }
    else if(func == 1)
    {
	regs[second] = regs[second] - regs[first];
       
        
    }
    else if(func == 2)
    {
        regs[second] =  regs[second] & regs[first];
    }
    else if(func ==3)
    {
        regs[second] =  regs[second] ^ regs[first];
    }
    else if(func ==4)
    {
    
        if(regs[first] > 0 && regs[second] > 0 && (regs[first] * regs[second]) < 0)
        {
          of = 1;
        }
        else if(regs[first] < 0 && regs[second] < 0 && (regs[first] * regs[second]) < 0)
        {
          of = 1;
        }
        else if(regs[first] < 0 && regs[second] > 0 && (regs[first] * regs[second]) > 0)
        {
          of = 1;
        }
        else if(regs[first] > 0 && regs[second] < 0 && (regs[first] * regs[second]) > 0)
        {
          of = 1;
        }
        else 
        {
          of = 0;
        }
         regs[second] =  regs[second] * regs[first];
    }
    else if(func == 5)
    {
        of = 0;
        int res = regs[second] - regs[first];
        if(res == 0)
        {
            zf=1;
        }
        else if(res < 0)
        {
            sf=1;
        }
    }
    
    if(func!=5)
    {
        if(regs[second] == 0)
        {
            zf = 1;
        }
        
        if(regs[second] < 0)
        {
            sf = 1;
        }
    }
    
    prog_counter++;
}

void jxx (unsigned char* mem, int func)
{
    ++prog_counter;
    int intval = read_int(mem);

    if(intval >=mem_size)
    {
      printf("\nADR: Out of Bounds Memory Address");
      exit(1);
    }
    
    if(func == 0)
    {
        prog_counter = intval;
        return;
    }
    else if(func == 1)
    {
        if((sf ^ of) == 1 || zf == 1)
        {
            prog_counter = intval;
            return;
        }
    }
    else if(func == 2)
    {
        if((sf ^ of) == 1)
        {
            prog_counter = intval;
            return;
        }
    }
    else if(func == 3)
    {
        if(zf == 1)
        {
            prog_counter = intval;
            return;
        }
    }
    else if(func ==4)
    {
        if(zf == 0)
        {
            prog_counter = intval;
            return;
        }
    }
    else if(func == 5)
    {
        if((sf ^ of) == 0)
        {
            prog_counter = intval;
            return;
        }
    }
    else if(func == 6)
    {
        if(((sf ^ of) & zf) == 0)
        {
            prog_counter = intval;
            return;
        }
    }

    prog_counter++;
}

void call(unsigned char* mem)
{
    prog_counter++;
    int intval = read_int(mem);

    if(intval >=mem_size)
    {
      printf("\nADR: Out of Bounds Memory Address");
      exit(1);
    }
    
    
    regs[4] = regs[4]-4;
    write_int(mem, prog_counter+1, regs[4]);
    
    
    prog_counter=intval;
}

void ret(unsigned char* mem)
{
    prog_counter = read_int_mem(mem, regs[4]);
   
    regs[4] = regs[4]+4;
}

void pushl(unsigned char* mem)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);

    if(first<0 || first >7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
  
    regs[4] = regs[4] - 4;
    
    write_int(mem, regs[first], regs[4]);
    prog_counter++;
}


void popl(unsigned char* mem)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);

    if(first<0 || first >7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
    
    regs[first]=read_int_mem(mem, regs[4]);

 
    
    regs[4] = regs[4] + 4;
   
    prog_counter++;
}

void readx(unsigned char* mem, int func)
{
    zf=0;
    sf=0;
    of=0;
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);

    if(first<0 || first >7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
    prog_counter++;
    
    int intval = read_int(mem);
    
    
    int index=regs[first]+intval;
    
    if(func == 0)
    {
        char val=getchar();
        mem[index] = val;
    }
    
    if(func == 1)
    {
        int val;
        if(scanf("%d", &val)==EOF)
        {
            zf=1;
        }
        else
        {
            zf=0;
            write_int(mem, val, index);
        }
    }
    
    prog_counter++;
}

void writex(unsigned char* mem, int func)
{
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);

    if(first<0 || first >7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }
    
    prog_counter++;
    int intval = read_int(mem);
    
    int index=regs[first]+intval;
    
    if(func == 0)
    {
        putchar(mem[index]);
    }
    
    if(func == 1)
    {
        int val=read_int_mem(mem, index);
        printf("%d", val);
    }
    prog_counter++;
}

void movsbl(unsigned char* mem)
{
    
    unsigned char first;
    unsigned char second;
    unsigned char value;
    prog_counter++;
    value = mem[prog_counter];
    split_char(value,&first,&second);
    prog_counter++;

    if(first<0 || first >7 || second <0 || second > 7)
    {
      printf("\nADR: Invalid register number");
      exit(1);
    }

    int intval = read_int(mem);
    
    int index=regs[second]+intval;
    
    regs[first] = mem[index];
    prog_counter++;
}

int read_int(unsigned char* mem)
{
    int byte=mem[prog_counter];
    
    int intval=byte;
    
    prog_counter++;
    byte=mem[prog_counter];
    byte=byte<<8;
    intval=intval | byte;
    prog_counter++;
    byte=mem[prog_counter];
    byte=byte<<16;
    intval=intval | byte;
    prog_counter++;
    byte=mem[prog_counter];
    byte=byte<<24;
    intval=intval | byte;
    
    return intval;
}

int read_int_nouse(unsigned char* mem)
{
    int byte=mem[prog_counter];

    byte=byte<<24;
    int intval=byte;
    
    prog_counter++;
    byte=mem[prog_counter];
    byte=byte<<16;
    intval=intval | byte;
    prog_counter++;
    byte=mem[prog_counter];
    byte=byte<<8;
    intval=intval | byte;
    prog_counter++;
    byte=mem[prog_counter];
    
    intval=intval | byte;
    
    return intval;
}

int read_int_mem(unsigned char* mem, int index)
{
    int byte=mem[index];
    
    int intval=byte;
    
    index++;
    byte=mem[index];
    byte=byte<<8;
    intval=intval | byte;
    index++;
    byte=mem[index];
    byte=byte<<16;
    intval=intval | byte;
    index++;
    byte=mem[index];
    byte=byte<<24;
    intval=intval | byte;
    
    return intval;
}

int read_int_mem_nouse(unsigned char* mem, int index)
{
    int byte=mem[index];
    
    byte=byte<<24;
    int intval=byte;
    
    index++;
    byte=mem[index];
    byte=byte<<16;
    intval=intval | byte;
    index++;
    byte=mem[index];
    byte=byte<<8;
    intval=intval | byte;
    index++;
    byte=mem[index];
    
    intval=intval | byte;
    
    return intval;
}

void write_int(unsigned char* mem, int val, int index)
{
    unsigned char byte;
    int mask=255;
    byte=val & mask;
    mem[index] = byte;
    index++;
    mask = mask << 8;
    byte=(val & mask)>>8;
    mem[index] = byte;
    index++;
    mask = mask << 16;
    byte=(val & mask)>>16;
    mem[index] = byte;
    index++;
    mask = mask << 24;
    byte=(val & mask)>>24;
    mem[index] = byte;
    index++;
    
    
}

void write_int_nouse(unsigned char* mem, int val, int index)
{
    unsigned char byte;
    int mask=255;
    mask = mask << 24;
    byte=val & mask;
    
    mem[index] = byte;
    index++;
    mask=255;
    mask = mask << 16;
    byte=val & mask;
    mem[index] = byte;
    index++;
    mask=255;
    mask = mask << 8;
    byte=val & mask;
    mem[index] = byte;
    index++;
    mask=255;
    
    byte=val & mask;
    mem[index] = byte;
    index++;
}
