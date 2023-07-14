//  Author - Harshawardhan Mane


#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <getopt.h>
#include <sys/param.h>
#include <sys/stat.h>
//DECLARING HASHTABLE SIZE
#define HASHTABLE_SIZE 9999997
//Declare Global Functions
extern char *strSHA2(char *filename);

//HASH TABLE Structure to Store Primitive Data about file address and file size keyed by their strSHA2 values
typedef struct data{
    char *address;
    int  size;
    struct data *next;
} DATA;
typedef DATA *HASHTABLE;



