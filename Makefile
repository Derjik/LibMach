########################
### Compilation flags

CPPFLAGS = -Wall -Wextra -Werror -fPIC
CFLAGS = -O3
CXXFLAGS = $(CFLAGS) --std=c++11 -Iinclude/


########################
### 'Shortcut' macros

# Modules required to build the main .a and .so archives
LIB_MODULES = 	obj/Logger.o \
		obj/Random.o \
		obj/Point.o \
		obj/Exception.o \
		obj/NetComponent.o \
		obj/UDPServer.o \
		obj/UDPClient.o

# Modules required to build the UDPServer demo program 
UDPSERVER_MODULES =	obj/mUDPServer.o \
			obj/DemoUDPServer.o \
			obj/UDPServer.o \
			obj/NetComponent.o \
			obj/Exception.o \
			obj/Logger.o

# Modules required to build the UDPClient demo program 
UDPCLIENT_MODULES =	obj/mUDPClient.o \
			obj/DemoUDPClient.o \
		 	obj/UDPClient.o \
			obj/NetComponent.o \
			obj/Exception.o \
			obj/Logger.o

#####################
### Default target

all:	bin/udpserver \
	bin/udpclient \
	lib/libmach.a \
	lib/libmach.so


######################
### Cleaning target

clean:
	rm -f obj/* bin/* log/* lib/* \
	[^obj/.gitignore] [^bin/.gitignore] [^log/.gitignore] [^lib/.gitignore]


####################################
### Binary targets (mostly tests)

# Logger test
bin/logger:		obj/mLogger.o \
			obj/Logger.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o bin/logger \
			obj/mLogger.o \
			obj/Logger.o

# Random test
#bin/random:		obj/mRandom.o \
#			obj/Random.o
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o bin/die \
#			obj/mRandom.o \
#			obj/Random.o

# UDPServer test
bin/udpserver:		$(UDPSERVER_MODULES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -pthread -o bin/udpserver \
			$(UDPSERVER_MODULES)

# UDPClient test
bin/udpclient:		$(UDPCLIENT_MODULES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o bin/udpclient \
			$(UDPCLIENT_MODULES)


######################
### Library outputs

# Static library
lib/libmach.a:	$(LIB_MODULES)
	ar rcs	lib/libmach.a \
		$(LIB_MODULES)

# Shared library
lib/libmach.so: $(LIB_MODULES)
	$(CXX)	-o lib/libmach.so -shared -Wl,-soname,libmach.so \
		$(LIB_MODULES)

#######################
### Exception module

obj/Exception.o: src/Exception.cpp include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/Exception.o -c src/Exception.cpp


####################
### Logger module

obj/mLogger.o:		src/mLogger.cpp include/Mach/Logger.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/mLogger.o -c src/mLogger.cpp

obj/Logger.o:		src/Logger.cpp include/Mach/Logger.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/Logger.o -c src/Logger.cpp


#################
### Random module

obj/mRandom.o:		src/mRandom.cpp include/Mach/Random.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/mRandom.o -c src/mRandom.cpp

obj/Random.o:		src/Random.cpp include/Mach/Random.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/Random.o -c src/Random.cpp


######################
### Geometry module

obj/Point.o:		src/Point.cpp include/Mach/Point.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/Point.o -c src/Point.cpp


#############################
### Generic network module

obj/NetComponent.o:	src/NetComponent.cpp include/Mach/NetComponent.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o obj/NetComponent.o \
			-c src/NetComponent.cpp


#################
### UDP module

obj/mUDPClient.o:	examples/udpclient/main.cpp \
			examples/udpclient/DemoUDPClient.hpp \
			include/Mach/UDPClient.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/mUDPClient.o \
					-c examples/udpclient/main.cpp

obj/UDPServer.o:	src/UDPServer.cpp \
			include/Mach/UDPServer.hpp \
			include/Mach/Logger.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/UDPServer.o \
					-c src/UDPServer.cpp

obj/DemoUDPServer.o:	examples/udpserver/DemoUDPServer.cpp \
			examples/udpserver/DemoUDPServer.hpp \
			include/Mach/UDPServer.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/DemoUDPServer.o \
					-c examples/udpserver/DemoUDPServer.cpp

obj/mUDPServer.o: 	examples/udpserver/main.cpp \
			examples/udpserver/DemoUDPServer.hpp \
			include/Mach/UDPServer.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp \
			include/Mach/Logger.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/mUDPServer.o \
			-c examples/udpserver/main.cpp

obj/UDPClient.o:	src/UDPClient.cpp \
			include/Mach/UDPClient.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/UDPClient.o  \
					-c src/UDPClient.cpp

obj/DemoUDPClient.o:	examples/udpclient/DemoUDPClient.cpp \
			examples/udpclient/DemoUDPClient.hpp \
			include/Mach/UDPClient.hpp \
			include/Mach/NetComponent.hpp \
			include/Mach/Exception.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)	-o obj/DemoUDPClient.o \
					-c examples/udpclient/DemoUDPClient.cpp
