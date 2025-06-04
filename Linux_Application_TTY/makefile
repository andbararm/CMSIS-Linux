CC="C:\arm-gnu-toolchain-14.2.rel1-mingw-w64-i686-arm-none-linux-gnueabihf\bin\arm-none-linux-gnueabihf-gcc.exe"
CFLAGS=-g -gdwarf-5

OUTDIR=out

VPATH=\
  ./src

INCLUDE=\
  -I./src

OBJECTS=\
  $(OUTDIR)/LinuxTTY.o

LIBS= 

default: $(OUTDIR)/LinuxTTY

$(OUTDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(OUTDIR)/LinuxTTY: $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)
