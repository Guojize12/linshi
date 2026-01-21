# Makefile for refactored app_user modules

CC = gcc
CFLAGS = -Wall -Wextra -I.

# Source files - all modules
SOURCES = \
app_user_globals.c \
app_user_packet.c \
app_user_resend.c \
app_user_event.c \
app_user_status.c \
app_user_inspect.c \
app_user_proto.c \
app_user_utils.c \
app_user_main.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Header files
HEADERS = \
app_user.h \
app_user_globals.h \
app_user_packet.h \
app_user_resend.h \
app_user_event.h \
app_user_status.h \
app_user_inspect.h \
app_user_proto.h \
app_user_utils.h \
app_user_main.h

# Default target
all: info

info:
@echo "==================================================="
@echo "Refactored app_user modules"
@echo "==================================================="
@echo "Source files:"
@for src in $(SOURCES); do echo "  - $$src"; done
@echo ""
@echo "Header files:"
@for hdr in $(HEADERS); do echo "  - $$hdr"; done
@echo "==================================================="
@echo "Add these source files to your project build."

clean:
rm -f $(OBJECTS)

.PHONY: all info clean
