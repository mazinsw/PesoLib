RM     = rm -f
OBJS   = build/obj/PesoLib.o \
         build/obj/device/Device.o \
         build/obj/device/DeviceManager.o \
         build/obj/driver/Elgin.o \
         build/obj/driver/Magna.o \
         build/obj/driver/Filizola.o \
         build/obj/driver/Toledo.o \
         build/obj/driver/Alfa.o \
         build/obj/driver/Urano.o \
         build/obj/win/main.o \
         build/obj/win/CommPort.o \
         build/obj/win/Event.o \
         build/obj/win/Mutex.o \
         build/obj/win/Thread.o \
         build/obj/util/StringBuilder.o \
         build/obj/java/br_com_mzsw_PesoLibWrapper.o \
         build/obj/AppResource.res

shared64: build/x64/PesoLib.dll
shared32: build/x86/PesoLib.dll

static64: build/x64/PesoLib.exe
static32: build/x86/PesoLib.exe

build64:
	$(eval RCFLAGS=-O coff -F pe-x86-64)
	$(eval CC=x86_64-w64-mingw32-gcc)
	$(eval WINDRES=x86_64-w64-mingw32-windres)

build32:
	$(eval RCFLAGS=-O coff -F pe-i386)
	$(eval CC=i686-w64-mingw32-gcc)
	$(eval WINDRES=i686-w64-mingw32-windres)

dll64: build64
	$(eval LIBS=-shared -Wl,--kill-at,--out-implib,lib/x64/libPesoLib.dll.a -lwinspool -m64)
	$(eval CFLAGS=-Iinclude -Isrc/system -Isrc/comm -Isrc/device -Isrc/driver -Isrc/util -Isrc/java/jni -Isrc/java/jni/win32 -DBUILD_DLL -m64)

dll32: build32
	$(eval LIBS=-shared -Wl,--kill-at,--out-implib,lib/x86/libPesoLib.dll.a -lwinspool -m32)
	$(eval CFLAGS=-Iinclude -Isrc/system -Isrc/comm -Isrc/device -Isrc/driver -Isrc/util -Isrc/java/jni -Isrc/java/jni/win32 -DBUILD_DLL -m32)

exe64: build64
	$(eval LIBS=-lwinspool -m64)
	$(eval CFLAGS=-Iinclude -Isrc/system -Isrc/comm -Isrc/device -Isrc/driver -Isrc/util -Isrc/java/jni -Isrc/java/jni/win32 -DDEBUGLIB -m64)

exe32: build32
	$(eval LIBS=-lwinspool -m32)
	$(eval CFLAGS=-Iinclude -Isrc/system -Isrc/comm -Isrc/device -Isrc/driver -Isrc/util -Isrc/java/jni -Isrc/java/jni/win32 -DDEBUGLIB -m32)

clean:
	$(RM) $(OBJS)

purge: clean
	$(RM) build/x64/PesoLib.dll build/x86/PesoLib.dll build/x64/PesoLib.exe build/x86/PesoLib.exe

build/x64/PesoLib.dll: dll64 $(OBJS)
	mkdir -p $(dir $@)
	mkdir -p lib/x64
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

build/x86/PesoLib.dll: dll32 $(OBJS)
	mkdir -p $(dir $@)
	mkdir -p lib/x86
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

build/x64/PesoLib.exe: exe64 $(OBJS)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

build/x86/PesoLib.exe: exe32 $(OBJS)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

build/obj/PesoLib.o: src/PesoLib.c src/system/Thread.h src/system/Mutex.h src/system/Event.h src/comm/CommPort.h src/device/DeviceManager.h src/util/StringBuilder.h src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/device/Device.o: src/device/Device.c src/device/Device.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/device/DeviceManager.o: src/device/DeviceManager.c src/device/DeviceManager.h src/driver/Filizola.h src/driver/Toledo.h src/driver/Alfa.h src/driver/Magna.h src/driver/Urano.h src/driver/Elgin.h src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Elgin.o: src/driver/Elgin.c src/driver/Elgin.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Magna.o: src/driver/Magna.c src/driver/Magna.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Filizola.o: src/driver/Filizola.c src/driver/Filizola.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Toledo.o: src/driver/Toledo.c src/driver/Toledo.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Alfa.o: src/driver/Alfa.c src/driver/Alfa.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/driver/Urano.o: src/driver/Urano.c src/driver/Urano.h src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/win/main.o: src/win/main.c include/PesoLib.h src/comm/CommPort.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/win/CommPort.o: src/win/CommPort.c src/comm/CommPort.h src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/win/Event.o: src/win/Event.c src/system/Event.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/win/Mutex.o: src/win/Mutex.c src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/win/Thread.o: src/win/Thread.c src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/util/StringBuilder.o: src/util/StringBuilder.c src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/java/br_com_mzsw_PesoLibWrapper.o: src/java/br_com_mzsw_PesoLibWrapper.c src/java/br_com_mzsw_PesoLibWrapper.h include/PesoLib.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

build/obj/AppResource.res: res/AppResource.rc
	mkdir -p $(dir $@)
	$(WINDRES) -i res/AppResource.rc -J rc -o $@ $(RCFLAGS)
