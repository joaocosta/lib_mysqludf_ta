#Things you might need to change depending on your environment
MYSQL_PLUGIN_DIR=/usr/lib/mysql/plugin
CC=gcc
CFLAGS=-Wall -I /usr/include/mysql -c
LD=ld
LDFLAGS=-shared


#Things you shouldn't need to change if you just mean to build this
SOURCES=ta_ema.c ta_sma.c ta_tr.c ta_sum.c ta_rsi.c ta_previous.c ta_max.c ta_min.c
OBJECTS=$(SOURCES:.c=.o)
BINARY=lib_mysqludf_ta.so

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

install: $(BINARY)
	cp $(BINARY) $(MYSQL_PLUGIN_DIR)
	service mysqld restart

tidy:
	uncrustify -c ./uncrustify.cfg --no-backup --replace $(SOURCES)

clean:
	rm -f $(BINARY) *.o
