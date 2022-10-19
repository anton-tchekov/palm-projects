PROJECT = bad_apple
APP_NAME = "Bad Apple"
CREATOR_ID = TEST
CC = m68k-palmos-gcc
CFLAGS = -O2

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
	-rm -f *.[oa] $(PROJECT) *.bin *.stamp
