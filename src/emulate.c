#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  FILE *file;
  char *buffer; //pointer to the array of binary object code read from file
  int fileLen;

  file = fopen(argv[1], "rb");
  if (!file){
       printf("Error! opening file\n");
       exit(1);
   }

   fseek(file, 0, SEEK_END);
	 fileLen = ftell(file);
	 fseek(file, 0, SEEK_SET);

   buffer = malloc(fileLen + 1);

   if (!buffer) {
     printf("Memory Error\n");
     exit(1);
   }

   fread(buffer, fileLen, 1, file);
   fclose(file);

   return 0;
}
