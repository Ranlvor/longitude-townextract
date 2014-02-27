

OSMBINARYSRC = OSM-binary/src
OSMBINARYINC = OSM-binary/include
CC = g++
LD = g++
CCargs = -g -c -Wall -I$(OSMBINARYINC) -std=c++11
LDargs = -g -Wall  -std=c++11
PROTOC = protoc

all: bin/importer bin/lookupcli

bin/lookupcli: bin/lookupcli.o bin/Database.o bin/Lookup.o
	$(LD) $(LDargs) -o bin/lookupcli bin/lookupcli.o bin/Database.o bin/Lookup.o -pthread -lz -lsqlite3 -lgeos -lprotobuf-lite -losmpbf

bin/importer: bin/importer.o bin/InformationExtractor.o bin/Database.o bin/osmformat.pb.o bin/fileformat.pb.o 
	$(LD) $(LDargs) -o bin/importer bin/importer.o bin/InformationExtractor.o bin/Database.o bin/osmformat.pb.o bin/fileformat.pb.o -pthread -lz -lprotobuf -lsqlite3

bin/importer.o: src/importer.cpp src/InformationExtractor.h src/Database.h generated/osmformat.pb.cc generated/fileformat.pb.cc
	$(CC) $(CCargs) -o bin/importer.o src/importer.cpp

bin/lookupcli.o: src/lookupcli.cpp src/Database.h src/Lookup.h
	$(CC) $(CCargs) -o bin/lookupcli.o src/lookupcli.cpp

bin/InformationExtractor.o: src/InformationExtractor.cpp src/InformationExtractor.h src/Database.h generated/osmformat.pb.cc generated/fileformat.pb.cc src/output.h src/stringToNumber.h
	$(CC) $(CCargs) -o bin/InformationExtractor.o src/InformationExtractor.cpp

bin/Database.o: src/Database.cpp src/Database.h 
	$(CC) $(CCargs) -o bin/Database.o src/Database.cpp

bin/Lookup.o: src/Lookup.cpp src/Lookup.h src/Database.h src/stringToNumber.h
	$(CC) $(CCargs) -o bin/Lookup.o src/Lookup.cpp

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
