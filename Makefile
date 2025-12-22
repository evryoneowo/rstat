CC = clang
CFLAGS = -Wall -Wextra
SRC = main.c mem.c uptime.c cpu.c
TARGET = rstat
PREFIX = /usr/local
INSTALL_DIR = $(PREFIX)/bin

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(INSTALL_DIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(INSTALL_DIR)/$(TARGET)

.PHONY: main clean install uninstall
