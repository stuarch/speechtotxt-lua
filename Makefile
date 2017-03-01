CC = clang
#INCLUDE = -I/usr/include/sphinxbase -I/usr/include/pocketsphinx -I/usr/include/sphinxbase -lpocketsphinx -lsphinxbase -lsphinxad -lpulse -lpulse-simple -lpthread -lm -lblas -llapack -I/usr/include/lua5.2 -llua5.2
INCLUDE = `pkg-config --cflags --libs sphinxbase pocketsphinx lua51` 
TARGET = speechtotxt

main: $(TARGET).c
	$(CC) -Wall -fPIC -shared $(INCLUDE) $(TARGET).c -o $(TARGET).so

install:
	cp ./$(TARGET).so /usr/local/lib/lua/5.1/$(TARGET).so

clean:
	rm ./$(TARGET).so

remove:
	rm /usr/local/lib/lua/5.1/$(TARGET).so
