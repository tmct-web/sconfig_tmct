mt_sconfig: main.o fs.o bb_gpio.o as.o
	gcc -Wall -O2 -o mt_sconfig main.o fs.o bb_gpio.o as.o

main.o:
	gcc -c main.c
fs.o:
	gcc -c fs.c
bb_gpio.o:
	gcc -c bb_gpio.c
as.o:
	gcc -c as.c
clean:
	rm -f mt_sconfig main.o fs.o bb_gpio.o as.o
