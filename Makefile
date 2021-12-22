#.PHONY: all clean

CXX = g++
DEBUGFLAGS = -g
RELEASEFLAGS = -O3

ifeq ($(OS), Windows_NT)
	SHELL = CMD
	CFLAGS = -c -Wall -std=c++20 -I./curl/include
	LDFLAGS = -L./curl/lib -lssl -lcrypto -lcurl
	EXE_main4_2d = ./Debug/main4_2.exe
	EXE_main4_2 = ./Release/main4_2.exe
	EXE_main1d = ./Debug/main1.exe
	EXE_main1 = ./Release/main1.exe
	EXE_main2d = ./Debug/main2.exe
	EXE_main2 = ./Release/main2.exe
	EXE_main3d = ./Debug/main3.exe
	EXE_main3 = ./Release/main3.exe
	EXE_main4d = ./Debug/main4.exe
	EXE_main4 = ./Release/main4.exe
	EXE_main5d = ./Debug/main5.exe
	EXE_main5 = ./Release/main5.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		CFLAGS = -c -Wall -std=c++20 -I./
		LDFLAGS = -L/usr/lib/ -lcurl -lssl -lcrypto -lpthread
		EXE_main4_2d = ./Debug/main4_2
		EXE_main4_2 = ./Release/main4_2
		EXE_main1d = ./Debug/main1
		EXE_main1 = ./Release/main1
		EXE_main2d = ./Debug/main2
		EXE_main2 = ./Release/main2
		EXE_main3d = ./Debug/main3
		EXE_main3 = ./Release/main3
		EXE_main4d = ./Debug/main4
		EXE_main4 = ./Release/main4
		EXE_main5d = ./Debug/main5
		EXE_main5 = ./Release/main5
    endif
endif

$(info init..)
main4_2d: ./Source/main4_2.cpp $(EXE_main4_2d)
main4_2: ./Source/main4_2.cpp $(EXE_main4_2)
main1d: ./Source/main1.cpp $(EXE_main1d)
main1: ./Source/main1.cpp $(EXE_main1)
main2d: ./Source/main2.cpp $(EXE_main2d)
main2: ./Source/main2.cpp $(EXE_main2)
main3d: ./Source/main3.cpp $(EXE_main3d)
main3: ./Source/main3.cpp $(EXE_main3)
main4d: ./Source/main4.cpp $(EXE_main4d)
main4: ./Source/main4.cpp $(EXE_main4)
main5d: ./Source/main5.cpp $(EXE_main5d)
main5: ./Source/main5.cpp $(EXE_main5)

./Source/fraction.o: ./Source/fraction.cpp ./Source/fraction.h
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/CP_main1.o: ./Source/CP_main1.cpp ./Source/fraction.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/main1.o: ./Source/main1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/main2.o: ./Source/main2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/main3.o: ./Source/main3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/main4.o: ./Source/main4.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Source/main4_2.d.o: ./Source/main4_2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
./Source/main5.o: ./Source/main5.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/main1.d.o: ./Source/main1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
./Source/main2.d.o: ./Source/main2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
./Source/main3.d.o: ./Source/main3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
./Source/main4.d.o: ./Source/main4.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
./Source/main5.d.o: ./Source/main5.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
$(EXE_main4_2d): ./Source/main4_2.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main4_2): ./Source/main4_2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@
	
$(EXE_main1d): ./Source/main1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main1): ./Source/main1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@
	
$(EXE_main2d): ./Source/main2.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main2): ./Source/main2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@
	
$(EXE_main3d): ./Source/main3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main3): ./Source/main3.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@
	
$(EXE_main4d): ./Source/main4.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main4): ./Source/main4.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@
	
$(EXE_main5d): ./Source/main5.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main5): ./Source/main5.o
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