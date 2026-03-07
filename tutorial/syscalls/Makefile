TARGET=main

.PHONY: all

all: $(TARGET)

$(TARGET): main.o
	$(CC) -o $@ $^ -static

# demo.o: demo.S Makefile
# 	$(GCC) -o $@ $<

main.o: main.c Makefile
	$(CC) -c $< -o $@ -g