CC     = gcc
WINDRES= windres
RM     = rm -f
OBJS   = ../bin/x64/obj/PesoLib.o \
         ../bin/x64/obj/device/Device.o \
         ../bin/x64/obj/device/DeviceManager.o \
         ../bin/x64/obj/driver/Elgin.o \
         ../bin/x64/obj/driver/Magna.o \
         ../bin/x64/obj/driver/Filizola.o \
         ../bin/x64/obj/driver/Toledo.o \
         ../bin/x64/obj/driver/Alfa.o \
         ../bin/x64/obj/driver/Urano.o \
         ../bin/x64/obj/win/main.o \
         ../bin/x64/obj/win/CommPort.o \
         ../bin/x64/obj/win/Event.o \
         ../bin/x64/obj/win/Mutex.o \
         ../bin/x64/obj/win/Thread.o \
         ../bin/x64/obj/util/StringBuilder.o \
         ../bin/x64/obj/java/br_com_mzsw_PesoLibWrapper.o \
         ../bin/x64/obj/AppResource.res

OBJS32 = ../bin/x86/obj/PesoLib.o \
         ../bin/x86/obj/device/Device.o \
         ../bin/x86/obj/device/DeviceManager.o \
         ../bin/x86/obj/driver/Elgin.o \
         ../bin/x86/obj/driver/Magna.o \
         ../bin/x86/obj/driver/Filizola.o \
         ../bin/x86/obj/driver/Toledo.o \
         ../bin/x86/obj/driver/Alfa.o \
         ../bin/x86/obj/driver/Urano.o \
         ../bin/x86/obj/win/main.o \
         ../bin/x86/obj/win/CommPort.o \
         ../bin/x86/obj/win/Event.o \
         ../bin/x86/obj/win/Mutex.o \
         ../bin/x86/obj/win/Thread.o \
         ../bin/x86/obj/util/StringBuilder.o \
         ../bin/x86/obj/java/br_com_mzsw_PesoLibWrapper.o \
         ../bin/x86/obj/AppResource.res

shared: ../bin/x64/PesoLib.dll ../bin/x86/PesoLib.dll

executable: ../bin/x64/PesoLib.exe ../bin/x86/PesoLib.exe

dll64:
	$(eval LIBS=-shared -Wl,--kill-at,--out-implib,../lib/x64/libPesoLib.dll.a -lwinspool -m64)
	$(eval CFLAGS=-I..\include -I..\src\system -I..\src\comm -I..\src\device -I..\src\driver -I..\src\util -I..\src\java\jni -I..\src\java\jni\win32 -DBUILD_DLL -m64)

dll32:
	$(eval LIBS=-shared -Wl,--kill-at,--out-implib,../lib/x86/libPesoLib.dll.a -lwinspool -m32)
	$(eval CFLAGS=-I..\include -I..\src\system -I..\src\comm -I..\src\device -I..\src\driver -I..\src\util -I..\src\java\jni -I..\src\java\jni\win32 -DBUILD_DLL -m32)

exe64:
	$(eval LIBS=-lwinspool -m64)
	$(eval CFLAGS=-I..\include -I..\src\system -I..\src\comm -I..\src\device -I..\src\driver -I..\src\util -I..\src\java\jni -I..\src\java\jni\win32 -DDEBUGLIB -m64)

exe32:
	$(eval LIBS=-lwinspool -m32)
	$(eval CFLAGS=-I..\include -I..\src\system -I..\src\comm -I..\src\device -I..\src\driver -I..\src\util -I..\src\java\jni -I..\src\java\jni\win32 -DDEBUGLIB -m32)

clean:
	$(RM) $(OBJS) $(OBJS32)

purge: clean
	$(RM) ../bin/x64/PesoLib.dll ../bin/x86/PesoLib.dll ../bin/x64/PesoLib.exe ../bin/x86/PesoLib.exe

../bin/x64/PesoLib.dll: dll64 $(OBJS)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

../bin/x86/PesoLib.dll: dll32 $(OBJS32)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS32) $(LIBS)

../bin/x64/PesoLib.exe: exe64 $(OBJS)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

../bin/x86/PesoLib.exe: exe32 $(OBJS32)
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -o $@ $(OBJS32) $(LIBS)

../bin/x64/obj/PesoLib.o: ../src/PesoLib.c ../src/system/Thread.h ../src/system/Mutex.h ../src/system/Event.h ../src/comm/CommPort.h ../src/device/DeviceManager.h ../src/util/StringBuilder.h ../src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/device/Device.o: ../src/device/Device.c ../src/device/Device.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/device/DeviceManager.o: ../src/device/DeviceManager.c ../src/device/DeviceManager.h ../src/driver/Filizola.h ../src/driver/Toledo.h ../src/driver/Alfa.h ../src/driver/Magna.h ../src/driver/Urano.h ../src/driver/Elgin.h ../src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Elgin.o: ../src/driver/Elgin.c ../src/driver/Elgin.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Magna.o: ../src/driver/Magna.c ../src/driver/Magna.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Filizola.o: ../src/driver/Filizola.c ../src/driver/Filizola.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Toledo.o: ../src/driver/Toledo.c ../src/driver/Toledo.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Alfa.o: ../src/driver/Alfa.c ../src/driver/Alfa.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/driver/Urano.o: ../src/driver/Urano.c ../src/driver/Urano.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/win/main.o: ../src/win/main.c ../include/PesoLib.h ../src/comm/CommPort.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/win/CommPort.o: ../src/win/CommPort.c ../src/comm/CommPort.h ../src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/win/Event.o: ../src/win/Event.c ../src/system/Event.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/win/Mutex.o: ../src/win/Mutex.c ../src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/win/Thread.o: ../src/win/Thread.c ../src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/util/StringBuilder.o: ../src/util/StringBuilder.c ../src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/java/br_com_mzsw_PesoLibWrapper.o: ../src/java/br_com_mzsw_PesoLibWrapper.c ../src/java/br_com_mzsw_PesoLibWrapper.h ../include/PesoLib.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x64/obj/AppResource.res: AppResource.rc
	mkdir -p $(dir $@)
	$(WINDRES) -i AppResource.rc -J rc -o $@ -O coff -F pe-x86-64


../bin/x86/obj/PesoLib.o: ../src/PesoLib.c ../src/system/Thread.h ../src/system/Mutex.h ../src/system/Event.h ../src/comm/CommPort.h ../src/device/DeviceManager.h ../src/util/StringBuilder.h ../src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/device/Device.o: ../src/device/Device.c ../src/device/Device.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/device/DeviceManager.o: ../src/device/DeviceManager.c ../src/device/DeviceManager.h ../src/driver/Filizola.h ../src/driver/Toledo.h ../src/driver/Alfa.h ../src/driver/Magna.h ../src/driver/Urano.h ../src/driver/Elgin.h ../src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Elgin.o: ../src/driver/Elgin.c ../src/driver/Elgin.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Magna.o: ../src/driver/Magna.c ../src/driver/Magna.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Filizola.o: ../src/driver/Filizola.c ../src/driver/Filizola.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Toledo.o: ../src/driver/Toledo.c ../src/driver/Toledo.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Alfa.o: ../src/driver/Alfa.c ../src/driver/Alfa.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/driver/Urano.o: ../src/driver/Urano.c ../src/driver/Urano.h ../src/device/DevicePrivate.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/win/main.o: ../src/win/main.c ../include/PesoLib.h ../src/comm/CommPort.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/win/CommPort.o: ../src/win/CommPort.c ../src/comm/CommPort.h ../src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/win/Event.o: ../src/win/Event.c ../src/system/Event.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/win/Mutex.o: ../src/win/Mutex.c ../src/system/Mutex.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/win/Thread.o: ../src/win/Thread.c ../src/system/Thread.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/util/StringBuilder.o: ../src/util/StringBuilder.c ../src/util/StringBuilder.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/java/br_com_mzsw_PesoLibWrapper.o: ../src/java/br_com_mzsw_PesoLibWrapper.c ../src/java/br_com_mzsw_PesoLibWrapper.h ../include/PesoLib.h
	mkdir -p $(dir $@)
	$(CC) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

../bin/x86/obj/AppResource.res: AppResource.rc
	mkdir -p $(dir $@)
	$(WINDRES) -i AppResource.rc -J rc -o $@ -O coff -F pe-i386
