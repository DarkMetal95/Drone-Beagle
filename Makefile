CC=gcc

BDIR=build
INCDIR=
SRCDIR=src
LIBDIR=lib

CFLAGS=-Wall
LDFLAGS=-lbluetooth -lm -lpthread

EXEC=drone

all: setup $(EXEC)

$(EXEC): $(BDIR)/drone.o $(BDIR)/libbluetooth.o $(BDIR)/libsensors.o $(BDIR)/libkalman.o $(BDIR)/libi2c.o $(BDIR)/libpwm.o
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
