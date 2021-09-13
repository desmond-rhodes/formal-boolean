OUT := logic
OBJS := main.o

CXX := g++
CXXFLAGS := -std=c++17

TMP := .$(OUT)

$(OUT): $(OBJS)
	touch $(TMP).cc
	make $(TMP)
	mv $(TMP) $@
	rm -f $(TMP)*

$(TMP): $(TMP).cc $(OBJS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f $(TMP)*
