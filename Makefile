#
# Makefile for mont program
#

# Parameters
INSTDIR = bin
MONT = tool
CFLAGS = -g
LEX = lex
CLI_OBJS = bin/lex.yy.o bin/y.tab.o

csrc = $(wildcard common/*.c)	\
		$(wildcard bgp/*.c)		

obj = $(csrc:.c=.o)

LDFLAGS = -Lbin -ljsmn -lexpat -lpthread -ll -lm

# Targets 
all : OPENSRC $(MONT)
	mkdir -p bin        
	mv $(MONT) ${INSTDIR}

OPENSRC: 
	(cd jsmn; make all)
	(cd cli; make)

$(MONT): $(obj)
	$(CXX) -g -o $@ $^ $(CLI_OBJS) $(LDFLAGS)

clean:
	$(RM) core*  bgp/*.o common/*.o
	$(RM) bin/parse bin/libjsmn.a bin/tool
	(cd jsmn; make clean)
	(cd cli; make clean)

