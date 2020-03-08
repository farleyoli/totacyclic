src = $(wildcard *.c)
obj = $(src:.c=.o)

main: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) main
