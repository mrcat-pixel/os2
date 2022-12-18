.PHONY: clean, test
default: os2-get

clean:
	$(RM) os2-get

test: os2-get
	sudo ./os2-get -p 1
	sudo ./os2-get -d enp0s2
	sudo ./os2-get -p 234234
	sudo ./os2-get -d asdasdasd
	sudo ./os2-get g a r b a g e