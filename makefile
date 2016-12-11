all:
	gcc -Wall -o server simplehttpd.c -I. -lpthread
	gcc -Wall -o writer writer.c -lpthread