all:
	gcc -w -c errorHandling.c -lncurses -lpthread
	gcc -w -c utilities.c -lncurses -lpthread
	gcc -w -c menuModel.c -lncurses -lpthread
	gcc -w -c server.c -lncurses -lpthread
	gcc -w -c client.c -lncurses -lpthread
	gcc -w -c gameLogic.c -lncurses -lpthread
	gcc -w typeracer.c -lncurses -lpthread utilities.o menuModel.o server.o client.o gameLogic.o errorHandling.o -o typeracer

clean:
	rm *.o