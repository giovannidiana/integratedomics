CC	=	g++ -std=c++11
CFLAGS=	
LIBS    =       -lgsl -lblas -lGL -lglut -L/usr/lib/x86_64-linux-gnu 

VERSION=v2.3
EXECUTABLES = SprinGraph EDiff2
OBJECT_FILES= 
all: $(EXECUTABLES)

$(EXECUTABLES):	% : %.c $(OBJECT_FILES) 
	$(CC) $(CFLAGS) -o $@ $< $(OBJECT_FILES) $(LIBS)

$(OBJECT_FILES): %.o : %.cpp 
	$(CC) $(CFLAGS) -c $< 

clean:
	rm EDiff
	rm *.o


