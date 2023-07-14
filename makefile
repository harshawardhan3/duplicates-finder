#Makefile for our project to link all the files together

C11 = cc -std=c11
CFLAGS = -Wall -Werror -pedantic

duplicates:duplicates.o strSHA2.o
	$(C11) $(CFLAGS) -o duplicates duplicates.o strSHA2.o

duplicates.o:duplicates.c duplicates.h 
	$(C11) $(CFLAGS) -c duplicates.c 

strSHA2.o:strSHA2.c 
	$(C11) $(CFLAGS) -c strSHA2.c
