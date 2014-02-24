

OSMBINARYSRC = OSM-binary/src
OSMBINARYINC = OSM-binary/include
CC = g++
LD = g++
CCargs = -g -c -Wall -I$(OSMBINARYINC)
LDargs = -Wall
LDlibs = -pthread -lz -lprotobuf
PROTOC = protoc

all: bin/longitude-townextract

bin/longitude-townextract: bin/main.o bin/InformationExtractor.o bin/osmformat.pb.o bin/fileformat.pb.o
	$(LD) $(LDargs) -o bin/longitude-townextract bin/main.o bin/InformationExtractor.o bin/osmformat.pb.o bin/fileformat.pb.o $(LDlibs)

bin/main.o: src/main.cpp src/InformationExtractor.h generated/osmformat.pb.cc generated/fileformat.pb.cc
	$(CC) $(CCargs) -o bin/main.o src/main.cpp

bin/InformationExtractor.o: src/InformationExtractor.cpp src/InformationExtractor.h generated/osmformat.pb.cc generated/fileformat.pb.cc src/output.h
	$(CC) $(CCargs) -o bin/InformationExtractor.o src/InformationExtractor.cpp

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
