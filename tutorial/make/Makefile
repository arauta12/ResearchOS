vpath %.c src
vpath %.h include
.PHONY: srcs test

main: main.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c $< -o $@ -I include

test:
	echo "TESTING!"
