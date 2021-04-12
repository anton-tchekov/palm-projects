PROJECT = reversi
APP_NAME = "Reversi"
CREATOR_ID = RVRS
CC = m68k-palmos-gcc
CFLAGS = -O2 -Wall

all: $(PROJECT).prc

$(PROJECT).prc: $(PROJECT) bin.stamp
	build-prc $(PROJECT).prc $(APP_NAME) $(CREATOR_ID) $(PROJECT) *.bin

$(PROJECT): $(PROJECT).o
	$(CC) $(CFLAGS) -o $(PROJECT) $(PROJECT).o

$(PROJECT).o: $(PROJECT).c $(PROJECT).h
	$(CC) $(CFLAGS) -c $(PROJECT).c

bin.stamp: $(PROJECT).rcp $(PROJECT).h
	pilrc $(PROJECT).rcp

clean:
	rm -f *.[oa] $(PROJECT) *.bin *.stamp

install:
	pilot-xfer --port /dev/ttyUSB0 --install $(PROJECT).prc
