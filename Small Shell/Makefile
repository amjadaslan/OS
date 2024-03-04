CC = g++
OBJS = Commands.o smash.o signals.o
EXEC = smash
COMP_FLAG = -std=c++11

$(EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $@
Commands.o : Commands.cpp Commands.h
	$(CC) -c $(COMP_FLAG) $*.cpp 
signals.o : signals.cpp signals.h Commands.h
	$(CC) -c $(COMP_FLAG) $*.cpp 
smash.o : smash.cpp signals.h Commands.h
	$(CC) -c $(COMP_FLAG) $*.cpp
clean: rm -f $(OBJS) $(EXEC)