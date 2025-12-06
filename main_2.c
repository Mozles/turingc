#define _CRT_SECURE_NO_WARNINGS 1
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#define PAGESIZE 4096

typedef enum {
  SYMBOL_DOLAR,
  SYMBOL_HASH,
  SYMBOL_A,
  SYMBOL_B,
  SYMBOL_C,
  SYMBOL_D,
  SYMBOL_0,
  SYMBOL_1,
  SYMBOL_2,
} Symbol;

typedef struct {
  int next_set;
  char symbol;
  char dir;
} Instruction;

typedef struct {
  Instruction *instructions;
  char *data;

  int table_width;
  int table_height;
  int current_set;

  int data_size;
  int data_start_offset;
  int data_cur_ptr;
} TuringMachine;

int linecount(char* str) {
  int ret = 0;
  while (*str++) if (str[0] == '\n') ret++;
  return ret;
}

char* strclean(char* str) {
  char *base_str = str;
  str--;
  while (*str++) if (*str == '$') *str = ' ';
  while (*base_str++ == ' ') continue;
  return base_str;
}

Instruction collect_instruction(char *str) {
  Instruction in = {0};
  //printf("parsing %s\n", str);
  int len = strlen(str);
  char symbol = str[0];
  
  in.symbol = symbol;
  in.dir = 'N';

  if (len>2) {
    str = strchr(str, ' ');
    // printf("finding string int %s\n", str);
    char set = atoi(str);
    str = strchr(str+1, ' ');
    char direction = str[1];

    in.next_set = set;
    switch (direction) {
      case 'L':
        in.dir = 'L';
        break;
      case 'R':
        in.dir = 'R';
        break;
      case 'N':
        in.dir = 'N';
        break;
      default:
        in.dir = 'N';
    }
  }
  return in;
}

TuringMachine create_machine(char *data) {
  TuringMachine tm = {0};
  int table_len = -1;
  int i = 0;

  char * tok = strtok(data, "\n");
  do {
    if (table_len == -1) {
      char *sub = tok;
      tm.table_width = atoi(sub);

      sub = strchr(sub, ' ');
      tm.table_height = atoi(sub);
      table_len = tm.table_width*tm.table_height;
      
      tm.instructions = malloc(sizeof(Instruction)*table_len);
      continue;
    }

    tm.instructions[i] = collect_instruction(tok);
    i++;
  } while ((tok = strtok(NULL, "\n")));
  tm.current_set = 1;
  return tm;
}

void tm_execute(TuringMachine* tm) {
  char current_char = tm->data[tm->data_cur_ptr];

  Symbol cur_symbol = tm->data[tm->data_cur_ptr];
  // printf("Current symbol %c\n", cur_symbol);
  for (int i = 1; i<tm->table_height; i++) {
      if (cur_symbol == tm->instructions[i].symbol) {
      Instruction cur_instruction = tm->instructions[i+ tm->table_height*tm->current_set];
      if (cur_instruction.symbol == ' ') continue;
      tm->data[tm->data_cur_ptr] = cur_instruction.symbol;
      char replaced_with = tm->data[tm->data_cur_ptr];
      char *direction;

      switch (cur_instruction.dir) {
        case 'L':
          tm->data_cur_ptr--;
          direction = "LEFT";
          break;
        case 'R':
          tm->data_cur_ptr++;
          direction = "RIGHT";
          break;
        case 'N':
          direction = "NONE";
          break;
      }
      if (current_char == '$' && current_char == replaced_with && tm->current_set == cur_instruction.next_set && cur_instruction.dir == 'N') tm->data_cur_ptr = -1;
      tm->current_set = cur_instruction.next_set > 0 ? cur_instruction.next_set : 1;
      //if (current_char != '$')
      //printf("Current char: %c, replaced with %c, going %s, current_set %d, changing set to %d\n", current_char, replaced_with, direction, tm->current_set, cur_instruction.next_set);
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  size_t program_size = 0,
          data_size = strlen(argv[2]);

  char *program_path = NULL,
        *program_buf = NULL;
        //*data_path = NULL,
  char *data_buf = NULL;

  FILE *pProg = NULL;
        //*pData = NULL;

  if (argc == 1) {
    printf("Usage: main.exe [program file] [data file]\n");
    return 0;
  }
  program_path = argv[1]; // to be used later
  //data_path = argv[2];

  // get file sizes;
  if (program_path) pProg = fopen(program_path, "r");
  fseek(pProg, 0, SEEK_END);
  program_size = ftell(pProg);
  rewind(pProg);

  /*
  if (data_path) pData = fopen(data_path, "r");
  fseek(pData, 0, SEEK_END);
  data_size = ftell(pData);
  rewind(pData);
  */


  program_buf = calloc(program_size, 1);
  //data_buf = malloc(2*PAGESIZE + data_size); // to be filled with $
  data_buf = calloc(2*PAGESIZE + data_size, 1);
  
  fread(program_buf, 1, program_size, pProg);

  memset(data_buf, '$', 2*PAGESIZE+data_size);
  //fread(data_buf + PAGESIZE, 1, data_size-2, pData);
  memcpy(data_buf + PAGESIZE, argv[2], data_size);

  // close files
  //fclose(pProg);
  //fclose(pData);
  // pProg = NULL;
  //pData = NULL;


  TuringMachine tm = create_machine(program_buf);
  tm.data = data_buf;
  tm.data_size = PAGESIZE*2 + data_size;
  tm.data_start_offset = PAGESIZE + data_size;
  tm.data_cur_ptr = tm.data_start_offset;

  /* */
  for (int i = 0; i<tm.table_width*tm.table_height; i++) {
    Instruction in = tm.instructions[i];
    //printf("Instruction %d, %c, %d, %c\n", i, in.symbol, in.next_set, in.dir);
  }
  /* */

  int i = 0;
  while (tm.data_cur_ptr > 0 && tm.data_cur_ptr < tm.data_size) {
    tm_execute(&tm);
    //printf("\r%.50s", tm.data + PAGESIZE - 10);
    printf("\r");
    for (int j = 0; j<data_size+20; j++) {
      if (j == tm.data_cur_ptr - PAGESIZE + 10){
        printf("\x1b[41m");
      } 
      else printf("\x1b[0m");
      putchar(tm.data[PAGESIZE-10+j]);
    }
    Sleep(250);
    if (i++ > 2000) break;
  }
  //printf("Data:\n%s\n", data_buf);

  free(data_buf);
  free(program_buf);
  return 0;
}
