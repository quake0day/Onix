# Makefile file
# Two mode
# make: output normal version execution file
# make debug: output debug version execution file
CC = g++
DD = gcc
CFLAGS = -lpthread

dec_server: 
	$(CC) $(CFLAGS) -o dec_server main.cpp
	$(DD) $(CFLAGS) -o dec_client soc.c

	

clean:
	@- $(RM) *.o
	@- $(RM) dec_server
	@- $(RM) dec_client
	@- echo "Data Cleansing Done.Ready to Compile"