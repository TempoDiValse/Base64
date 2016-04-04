#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#define MAX_SIZE 65535
#define PADDING 64

char* base64_encode(const char *plain);
char* base64_decode(const char *encoded);