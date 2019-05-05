all: project

project: main.o server.o user.o utilities.o log.o
			gcc main.o server.o user.o utilities.o log.o -o banking -Wextra


main.o : main.c 
			gcc -c main.c -Wextra

server.o: forensic.c parse.c log.c
				gcc -c forensic.c -Wextra

user.o: user.c
		gcc -c user.c -Wextra

utilities.o: utilities.c
			gcc -c utilities.c -Wextra

log.o: log.c
					gcc -c log.c -Wextra

clean: 
		rm -rf *o banking