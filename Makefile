# Makefile for assignment 5
# Author Shane Keane 17329836

#Initialising flags and compiler
CC = mpicc
CFLAGS=-lgsl -lgslcblas -lm -Wall -Wextra -std=c11

all: tsqr

tsqr: tsqr.c 
	$(CC) -o tsqr tsqr.c $(CFLAGS)

