#!/bin/bash

CC= gcc -std=gnu99
CCFLAGS= -Wall
OPTIMIZE= -O3
LIBS= -pthread

SERVER= server
CLIENT= client
TARGET_SERVER= server.c
TARGET_CLIENT= client.c
GIT_HOOKS := .git/hooks/pre-commit
EXEC= target

.PHONY: all
all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

target:
	$(CC) -o $(SERVER) $(CCFLAGS) $(OPTIMIZE) $(TARGET_SERVER) $(LIBS)
	$(CC) -o $(CLIENT) $(CCFLAGS) $(OPTIMIZE) $(TARGET_CLIENT)

debug:
	$(CC) -o $(SERVER) $(CCFLAGS) -g $(TARGET_SERVER) $(LIBS)
	$(CC) -o $(CLIENT) $(CCFLAGS) -g $(TARGET_CLIENT)

prof:
	$(CC) -o $(SERVER) $(CCFLAGS) -g -pg $(TARGET_SERVER) $(LIBS)
	$(CC) -o $(CLIENT) $(CCFLAGS) -g -pg $(TARGET_CLIENT)

clean:
	rm -rf $(SERVER)
	rm -rf $(CLIENT)
	rm -rf gmon.out
