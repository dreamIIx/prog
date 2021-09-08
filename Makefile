#.PHONY: all clean

CXX = g++

ifeq ($(OS), Windows_NT)
	SHELL = CMD
	CFLAGS = -c -Wall -std=c++17
	INCLUDE_DIR = 
	LDFLAGS = 
	EXE_CP1d = ./Debug/CP1.exe
	EXE_CP1 = ./Release/CP1.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		CFLAGS = -c -Wall -std=c++17
		INCLUDE_DIR = 
		LDFLAGS = 
		EXE_CP1d = ./Debug/CP1
		EXE_CP1 = ./Release/CP1
    endif
endif

$(info init..)
CP1d: ./Source/CP_main1.cpp $(EXE_CP1d)
CP1: ./Source/CP_main1.cpp $(EXE_CP1)

./Source/fraction.o: ./Source/fraction.cpp ./Source/fraction.h
	$(info processing....)
	$(CXX) -O2 $(CFLAGS) $< -o $@
	
./Source/CP_main1.o: ./Source/CP_main1.cpp ./Source/fraction.cpp
	$(info processing....)
	$(CXX) -O2 $(CFLAGS) $< -o $@

./Source/fraction.d.o: ./Source/fraction.cpp ./Source/fraction.h
	$(info processing....)
	$(CXX) -g $(CFLAGS) $< -o $@
	
./Source/CP_main1.d.o: ./Source/CP_main1.cpp ./Source/fraction.cpp
	$(info processing....)
	$(CXX) -g $(CFLAGS) $< -o $@
	
$(EXE_CP1d): ./Source/CP_main1.d.o ./Source/fraction.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_CP1): ./Source/CP_main1.o ./Source/fraction.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm ./Source/*.o
#ifeq ($(OS), Windows_NT)
#	copy .\Source\*.o .\*.o
#	del .\Source\*.o
#else
#    UNAME_S := $(shell uname -s)
#    ifeq ($(UNAME_S), Linux)
#		rm *.o
#    endif
#endif