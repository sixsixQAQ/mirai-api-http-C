CC 	= gcc
LIBS = -lcjson -lsqlite3
BIN = main
OBJ = main.o err.o tcp.o request.o mirai.o std.o queue.o db.o
FLAGS = -Wall

main:$(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)
main.o: main.c err.h mirai.h queue.h request.h tcp.h
	$(CC) -c main.c -o main.o $(FLAGS)
err.o: err.c err.h	
	$(CC) -c err.c -o err.o	$(FLAGS)
std.o: std.c std.h err.h
	$(CC) -c std.c -o std.o $(FLAGS)
tcp.o: tcp.c err.h
	$(CC) -c tcp.c -o tcp.o $(FLAGS)
request.o: request.c request.h err.h std.h tcp.h
	$(CC) -c request.c -o request.o $(FLAGS)
mirai.o: mirai.c mirai.h queue.h request.h err.h tcp.h
	$(CC) -c mirai.c -o mirai.o $(FLAGS)
queue.o: queue.c queue.h std.h
	$(CC) -c queue.c -o queue.o $(FLAGS)
db.o:db.c db.h
	$(CC) -c db.c -o db.o $(FLAGS)


.PHONY:clean
clean:
	rm  $(OBJ) $(BIN)
