# Project: AM71113363
# Makefile created by Dev-C++ 4.9.9.2

CC   = gcc.exe -s
WINDRES = windres.exe
RES  = main.res
OBJ  = main.o ListView.o filter.o allsocklib.o socket.o $(RES)
LINKOBJ  = main.o ListView.o filter.o allsocklib.o socket.o $(RES)
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

ListView.o: ListView.c
	$(CC) -c ListView.c -o ListView.o $(CFLAGS)

filter.o: filter.c
	$(CC) -c filter.c -o filter.o $(CFLAGS)

allsocklib.o: allsocklib.c
	$(CC) -c allsocklib.c -o allsocklib.o $(CFLAGS)
	
socket.o: socket.c
	$(CC) -c socket.c -o socket.o $(CFLAGS)

main.res: main.rc 
	$(WINDRES) -i main.rc --input-format=rc -o main.res -O coff 
