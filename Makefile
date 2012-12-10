# Makefile file
# Two mode
# make: output normal version execution file
# make debug: output debug version execution file
CC = g++
CFLAGS = -lboost_system-mt -lboost_thread-mt -L/usr/local/lib

dec_server: 
	$(CC) $(CFLAGS) -o dec_server echo.cpp
	$(CC) $(CFLAGS) -o dec_client echoc.cpp

	

clean:
	@- $(RM) *.o
	@- $(RM) dec_server
	@- $(RM) dec_client
	@- echo "Data Cleansing Done.Ready to Compile"