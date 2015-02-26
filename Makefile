CC=gcc

BDIR=build
INCDIR=
SRCDIR=src
LIBDIR=lib

CFLAGS=-Wall
LDFLAGS=-lbluetooth

EXEC=motor_control

all: setup $(EXEC)

$(EXEC): $(BDIR)/motor_control.o $(BDIR)/libio.o $(BDIR)/libbluetooth.o
	$(CC) -o $@ $^ $(LDFLAGS)
$(BDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
$(BDIR)/%.o: $(LIBDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
setup:
	mkdir -p build
clean:
	rm -rf build
mrproper: clean
	rm -f $(EXEC)
