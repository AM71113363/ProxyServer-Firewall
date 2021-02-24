# Project: AM71113363


CC   = gcc.exe -s
WINDRES = windres.exe
RES  = main.res
OBJ  = main.o Display.o filter.o server.o net.o file.o $(RES)
LINKOBJ  = main.o Display.o filter.o server.o net.o file.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lcomctl32 -lws2_32  
INCS =  -I"C:/Dev-Cpp/include" 
BIN  = ProxyServer.exe
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ProxyServer.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "ProxyServer.exe" $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

Display.o: Display.c
	$(CC) -c Display.c -o Display.o $(CFLAGS)

filter.o: filter.c
	$(CC) -c filter.c -o filter.o $(CFLAGS)

server.o: server.c
	$(CC) -c server.c -o server.o $(CFLAGS)

net.o: net.c
	$(CC) -c net.c -o net.o $(CFLAGS)

file.o: file.c
	$(CC) -c file.c -o file.o $(CFLAGS)

main.res: main.rc 
	$(WINDRES) -i main.rc --input-format=rc -o main.res -O coff 
