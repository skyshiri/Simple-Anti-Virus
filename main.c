#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_OF_VIRUSES 10
#define SIGNATURES_FILE_NAME "signatures"
#define INFECTED_FILE_MAX_SIZE 10000
#define VIRUS_MAX_BYTES 136

typedef struct virus virus;
 
struct virus {
        int length;
        char *signature;
        char *name;
        virus *next;
};

void PrintHex(char *buffer,int length){
 int i;
 for(i=0;i<length;i++)
   printf("%02X ",(unsigned char)(buffer[i]));
}

void list_print(virus *virus_list){
  virus *v = virus_list;  
  int count=1;
  while(v != 0){
    printf("Virus no. %d:\n====================\n",count);
    printf("Virus name: %s\n",v->name);
    printf("Virus length: %d\n", v->length);
    printf("Virus Signature:\n");
    PrintHex(v->signature,v->length);
    printf("\n\n");
    count++;
    v = v->next;
  }
}

virus *list_append(virus *virus_list, virus *data){
  data->next = virus_list->next;
  virus_list->next = data;
 return virus_list;
}

void list_free(virus *virus_list){
  virus *v1=0;
  while(virus_list != 0){
    v1 = virus_list;
    virus_list = virus_list->next;
    free(v1->signature);
    free(v1->name);
    free(v1);
  }
}

/* assuming the virus size is given in little endian */
int toDecimal(char *code,int size){
  int i,num = 0;
  for(i=0;i<size;i++)
    if(i == 0)
      num = code[i];
    else
      num |= code[i]<<i;
  return num;
}

int copyName(char* buffer,char* c){
  int i=0;
  while(buffer[i] != 0)
    i++;
  strncpy(c,buffer,i);
  return i;
}

int VirusDetails(char *buffer,FILE* f,virus *data){
  int N=0,virus_name_length=0;
  fread(buffer,1,VIRUS_MAX_BYTES,f);
  memcpy(&N,buffer,4);
  /*data->length = toDecimal(c,4); // assuming the virus size is in big endian*/
  data->length = N;
  data->name = calloc(100,1);
  virus_name_length = copyName(buffer+N+4,data->name);
  data->signature = calloc(1,N);
  memcpy(data->signature,buffer+4,N);
  
  return (N+virus_name_length+4);
}

virus* getVirusesList(FILE* f){
  char buffer[VIRUS_MAX_BYTES];
  int count=1,i=0;
  virus *top,*data,*last;
   while(count <= NUM_OF_VIRUSES){
    data = calloc(1,sizeof(virus));
    i += VirusDetails(buffer,f,data);
    fseek(f,i+count,SEEK_SET);	   
    if(count == 1){
      top = data;
      last = data;
    }else{
      last = list_append(last,data);
      last = data;
    }
    count++;  
  }
  return top;
}

void detect_virus(char *buffer, virus *virus_list, unsigned int size){
    int i=0;
    while(virus_list != 0){
      i=0;
      while(i < size){
     	if(memcmp(virus_list->signature,buffer+i+4,virus_list->length) == 0)
			printf("Virus Found: %s\nStarting Byte: %d\nsize: %d\n",virus_list->name,i+4,virus_list->length);
      i++;
    }
    virus_list = virus_list->next;
  }
}

int main(int argc, char **argv) {
  virus *sig_list=0;
  int inf_file_size = 0;
  char inf_buffer[INFECTED_FILE_MAX_SIZE];
  memset(inf_buffer,0,INFECTED_FILE_MAX_SIZE);
  FILE *sig_file = 0,*inf_file=0;
  
  if(argc != 2){
    printf("Wrong parameters!\n");
    return 0;
  }
  
  sig_file = fopen(SIGNATURES_FILE_NAME,"r");
  inf_file = fopen(argv[1],"r");
  
  if(inf_file == NULL || sig_file == NULL){
    printf("Invalid file name!\n");
    return 0;
  }
  
  inf_file_size = fread(inf_buffer,1,10000,inf_file);
  if(inf_file_size > INFECTED_FILE_MAX_SIZE)
    inf_file_size = INFECTED_FILE_MAX_SIZE;
  
  sig_list = getVirusesList(sig_file);
  detect_virus(inf_buffer,sig_list,inf_file_size);
  

  list_free(sig_list);
  fclose(sig_file);
  fclose(inf_file);
  return 0;
} 
