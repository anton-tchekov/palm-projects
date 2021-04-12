PROJECT = periodic_table
APP_NAME = "PSE"
CREATOR_ID = MPSE
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
