# -*- Makefile -*-
#  Usage: make prog=[SOURCE_FILE]
#  Example: make prog=WaterJug/main.c

CC = tcc
CFLAGS = -std=c11 -Wall
LIB_OBJS = stack.o queue.o

$(prog).out: $(prog)
	@$(CC) -o $@ $<

