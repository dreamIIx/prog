#.PHONY: all clean

CXX = g++
DEBUGFLAGS = -g
RELEASEFLAGS = -O3

ifeq ($(OS), Windows_NT)
	SHELL = CMD
	CFLAGS = -c -Wall -std=c++20
	LDFLAGS = 
	EXE_vmain1d = ./Debug/vmain1.exe
	EXE_vmain1 = ./Release/vmain1.exe
	EXE_emain1d = ./Debug/emain1.exe
	EXE_emain1 = ./Release/emain1.exe
	EXE_emain2d = ./Debug/emain2.exe
	EXE_emain2 = ./Release/emain2.exe
	EXE_emain3d = ./Debug/emain3.exe
	EXE_emain3 = ./Release/emain3.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		CFLAGS = -c -Wall -std=c++20
		LDFLAGS = 
		EXE_vmain1d = ./Debug/vmain1
		EXE_vmain1 = ./Release/vmain1
		EXE_emain1d = ./Debug/emain1
		EXE_emain1 = ./Release/emain1
		EXE_emain2d = ./Debug/emain2
		EXE_emain2 = ./Release/emain2
		EXE_emain3d = ./Debug/emain3
		EXE_emain3 = ./Release/emain3
    endif
endif

$(info init..)
vmain1d: ./Source/vmain1.cpp $(EXE_vmain1d)
vmain1: ./Source/vmain1.cpp $(EXE_vmain1)
emain1d: ./Source/excmain1.cpp $(EXE_emain1d)
emain1: ./Source/excmain1.cpp $(EXE_emain1)
emain2d: ./Source/excmain2.cpp $(EXE_emain2d)
emain2: ./Source/excmain2.cpp $(EXE_emain2)
emain3d: ./Source/excmain2.cpp $(EXE_emain3d)
emain3: ./Source/excmain2.cpp $(EXE_emain3)
	
./Source/vmain1.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/vmain1.d.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain1.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain1.d.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain2.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain2.d.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain3.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain3.d.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
$(EXE_vmain1d): ./Source/vmain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain1): ./Source/vmain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1d): ./Source/emain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1): ./Source/emain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2d): ./Source/emain2.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2): ./Source/emain2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3d): ./Source/emain3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3): ./Source/emain3.o
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