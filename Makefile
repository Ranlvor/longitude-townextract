

OSMBINARYSRC = OSM-binary/src
OSMBINARYINC = OSM-binary/include
CC = g++
LD = g++
CCargs = -g -c -Wall -I$(OSMBINARYINC)
LDargs = -Wall 
LDlibs = -lprotobuf
PROTOC = protoc

all: bin/longitude-townextract

bin/longitude-townextract: bin/main.o bin/osmformat.pb.o bin/fileformat.pb.o
	$(LD) $(LDargs) -o bin/longitude-townextract bin/main.o bin/osmformat.pb.o bin/fileformat.pb.o $(LDlibs)

bin/main.o: src/main.cpp generated/osmformat.pb.cc generated/fileformat.pb.cc
	$(CC) $(CCargs) -o bin/main.o src/main.cpp

generated/osmformat.pb.cc: $(OSMBINARYSRC)/osmformat.proto
	$(PROTOC) --cpp_out=generated -I$(OSMBINARYSRC) $(OSMBINARYSRC)/osmformat.proto
	cp generated/osmformat.pb.h $(OSMBINARYINC)/osmpbf

bin/osmformat.pb.o: generated/osmformat.pb.cc
	$(CC) $(CCargs) -o bin/osmformat.pb.o generated/osmformat.pb.cc

generated/fileformat.pb.cc: $(OSMBINARYSRC)/osmformat.proto
	$(PROTOC) --cpp_out=generated -I$(OSMBINARYSRC) $(OSMBINARYSRC)/fileformat.proto
	cp generated/fileformat.pb.h $(OSMBINARYINC)/osmpbf

bin/fileformat.pb.o: generated/fileformat.pb.cc
	$(CC) $(CCargs) -o bin/fileformat.pb.o generated/fileformat.pb.cc

clean:
	rm -r bin generated
	mkdir bin generated
	touch bin/.dummyfile generated/.dummyfile
