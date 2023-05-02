TARGET = hello
OBJS = hello.o

CFLAGS = -D_POSIX_SOURCE
LDFLAGS = 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.s.o:
	$(CC) -c $<

clean:
	@rm -f *.bak *.o core $(TARGET)
