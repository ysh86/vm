TARGET = vm
OBJS = main.o gestalt.o regs68k.o mmu030.o kernel.o

CFLAGS = -D_POSIX_SOURCE
LDFLAGS = -lmac

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.s.o:
	$(CC) -c $<

clean:
	@rm -f *.bak *.o core $(TARGET)
