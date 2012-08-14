/*
 * common.cpp
 *
 * Written by Rodrigo Chiossi.
 *
 * Buffered String Search <rodrigochiossi@gmail.com>
 */

#include <string.h>  
#include <stdlib.h>
#include <stdio.h>
  
#define CHUNK_SIZE 16348
#define LIST_START_SIZE 4096

#define MAX_STR_SIZE 4096
#define MIN_STR_SIZE 4

char read_buffer[CHUNK_SIZE];
unsigned bytes_in_buffer;
unsigned read_ptr;
unsigned eof;
  
char* string_list = NULL;

void reset_buffer(FILE* file) {
  bytes_in_buffer = fread(read_buffer,1,CHUNK_SIZE,file);
  read_ptr = 0;
  eof = 0;
}

char read_char(FILE* file) {
  if (bytes_in_buffer != CHUNK_SIZE && read_ptr >= bytes_in_buffer) {
    eof = 1;
    return 0xff;
  }
  
  if (read_ptr == CHUNK_SIZE) {
    reset_buffer(file);
  }
  
  return read_buffer[read_ptr++];
}

int strings_file(const char* path) {
  FILE* file = fopen(path,"r");
  
  unsigned max_size = LIST_START_SIZE;
  unsigned cur_size = 0;
  
  char buffer[MAX_STR_SIZE];
  char* next_str;
  unsigned char c_byte;
  unsigned length;
  
  unsigned num_strings = 0;
  
  if (file == NULL) {
    printf("[-] Error opening file : %s\n",path);
    return -1;
  }
  
  string_list = (char*) malloc(sizeof(char)*max_size);
  
  reset_buffer(file);
  
  c_byte = read_char(file);
  length = 0;
  while (!eof) {
    /* check if character is printable */
    while (0x20 <= c_byte && c_byte <= 0x7E && length < MAX_STR_SIZE - 1 && !eof) {
      buffer[length++] = c_byte;
      c_byte = read_char(file);
    }
    
    if (length >= MIN_STR_SIZE) {
      /* grow string list buffer if needed */
      if (cur_size+length >= max_size) {
	string_list = (char*) realloc(string_list, sizeof(char)*max_size*2);
	if (string_list == NULL) {
	  printf("[-] Error: Failed to allocate memory!\n");
	  return -1;
	}
	max_size *= 2;
      }
      
      next_str = &string_list[cur_size];
      strncpy(next_str,buffer,length);
      next_str[length] = '\n';
      
      cur_size+=length+1;
      
      num_strings++;
    }

    /* start a new string */
    length = 0;
    
    c_byte = read_char(file);
  }

  string_list[cur_size] = '\0';
  
  fclose(file);

  return num_strings;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("[!] Usage: ./stringscout file\n");
    exit(0);
  }

  strings_file(argv[1]);

  if (string_list != NULL)
    printf("%s\n",string_list);

  return 0;
}
