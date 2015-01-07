# Makefile for libutvideo

# note:
# Currently, this Makefile is not tested except GNU make.

include config.mak

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)
vpath %.asm $(SRCPATH)

ASM_OBJS = $(SRCDIR)/Convert_asm_$(TARGET_ARCH).o \
           $(SRCDIR)/HuffmanCode_asm_$(TARGET_ARCH).o \
           $(SRCDIR)/Predict_asm_$(TARGET_ARCH).o

OBJS = $(SRCDIR)/Codec.o \
       $(SRCDIR)/CodecBase.o \
       $(SRCDIR)/Coefficient.o \
       $(SRCDIR)/Convert.o \
       $(SRCDIR)/DummyCodec.o \
       $(SRCDIR)/FrameBuffer.o \
       $(SRCDIR)/GlobalConfig.o \
       $(SRCDIR)/HuffmanCode.o \
       $(SRCDIR)/Predict.o \
       $(SRCDIR)/Thread.o \
       $(SRCDIR)/TunedFunc.o \
       $(SRCDIR)/UL00Codec.o \
       $(SRCDIR)/ULRACodec.o \
       $(SRCDIR)/ULRGCodec.o \
       $(SRCDIR)/ULYUV420Codec.o \
       $(SRCDIR)/ULYUV422Codec.o \
       $(SRCDIR)/UQ00Codec.o \
       $(SRCDIR)/UQY2Codec.o \
       $(SRCDIR)/utv_core.o \
       $(ASM_OBJS)

#### main rules ####

.PHONY: all install clean distclean

all: $(STATICLIB) $(SHAREDLIB)

$(STATICLIBNAME): $(OBJS)
	$(AR) rc $@ $^
	$(RANLIB) $@

$(SHAREDLIBNAME): $(OBJS)
	$(LD) $(SO_LDFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.asm
	$(ASM) $(ASMFLAGS) -I$(SRCDIR)/ -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

install: libutvideo.pc all
	install -d $(DESTDIR)$(includedir)
	install -d $(DESTDIR)$(includedir)/utvideo
	install -m 644 $(SRCDIR)/Codec.h $(DESTDIR)$(includedir)/utvideo
	install -m 644 $(SRCDIR)/utvideo.h $(DESTDIR)$(includedir)/utvideo
	install -m 644 $(SRCDIR)/version.h $(DESTDIR)$(includedir)/utvideo
	install -d $(DESTDIR)$(libdir)/pkgconfig
	install -m 644 libutvideo.pc $(DESTDIR)$(libdir)/pkgconfig
ifneq ($(STATICLIB),)
	install -m 644 $(STATICLIB) $(DESTDIR)$(libdir)
endif
ifneq ($(SHAREDLIB),)
ifneq ($(IMPLIB),)
	install -m 644 $(IMPLIB) $(DESTDIR)$(libdir)
	install -d $(DESTDIR)$(bindir)
	install -m 755 $(SHAREDLIB) $(DESTDIR)$(bindir)
else
	install -m 644 $(SHAREDLIB) $(DESTDIR)$(libdir)
endif
endif

#All objects should be deleted regardless of configure when uninstall/clean/distclean.
uninstall:
	rm -fr $(DESTDIR)$(includedir)/utvideo
	rm -f $(addprefix $(DESTDIR)$(libdir)/, $(STATICLIB) $(IMPLIB) pkgconfig/libutvideo.pc)
	rm -f $(addprefix $(DESTDIR)$(bindir)/, $(SHAREDLIB))

clean:
	$(RM) */*.o *.a *.so* *.def *.exp *.lib *.dll *.dylib

distclean: clean
	$(RM) config.* *.pc

libutvideo.pc:
	./configure
