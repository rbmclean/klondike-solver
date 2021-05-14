# Adapted from https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile
BIN=klondike-solver
RM=rm -f

SRCS=$(shell find . -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
