#build frei0r in mingw

PREFIX?=$(shell pwd)/../libffmpeg

CC=gcc
CXX=gcc
AR=ar rcs
RANLIB=ranlib
STRIP=strip
RM=-rm -rf

CFLAGS=" -mmacosx-version-min=10.9   -I. -c -O2 -Wall -std=c99 -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64 "
CPPFLAGS=" -mmacosx-version-min=10.9  -I. -c -O2 -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64 "
CXXFLAGS=" -mmacosx-version-min=10.9  -I. -c -O2 -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64 "

EXPORT_STATIC_LIB=frei0r.a
OBJS=3dflippo.o B.o G.o IIRblur.o R.o RGB.o alpha0ps.o alphagrad.o alphainjection.o alphaspot.o balanc0r.o bgsubtract0r.o brightness.o bw0r.o c0rners.o cluster.o colgate.o coloradj_RGB.o colordistance.o colorhalftone.o colorize.o colortap.o composition.o contrast0r.o curves.o defish0r.o distort0r.o dither.o emboss.o flippo.o gamma.o glitch0r.o glow.o host_param_test.o hqdn3d.o hueshift0r.o invert0r.o  ising0r.o keyspillm0pup.o lenscorrection.o letterb0xed.o levels.o luminance.o mask0mate.o medians.o perspective.o pixeliz0r.o posterize.o pr0be.o pr0file.o rgbnoise.o rgbsplit0r.o saturat0r.o select0r.o sharpness.o sigmoidaltransfer.o softglow.o spillsupress.o squareblur.o tehRoxx0r.o test_pat_B.o test_pat_C.o test_pat_G.o test_pat_I.o test_pat_L.o test_pat_R.o three_point_balance.o threshold0r.o tint0r.o transparency.o uvmap.o vertigo.o addition.o addition_alpha.o alphaatop.o alphain.o alphaout.o alphaover.o alphaxor.o baltan.o blend.o bluescreen0r.o burn.o cartoon.o color_only.o d90stairsteppingfix.o darken.o delay0r.o delaygrab.o difference.o divide.o dodge.o edgeglow.o equaliz0r.o grain_extract.o grain_merge.o hardlight.o hue.o lighten.o lightgraffiti.o lissajous0r.o multiply.o ndvi.o nervous.o nois0r.o nosync0r.o onecol0r.o overlay.o partik0l.o plasma.o primaries.o saturation.o scanline0r.o screen.o sobel.o softlight.o sopsat.o subtract.o threelay0r.o timeout.o tutorial.o twolay0r.o value.o vignette.o xfade0r.o  
	
all: $(OBJS)
	$(AR)  $(EXPORT_STATIC_LIB)  $(OBJS)
	
install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp -f $(EXPORT_STATIC_LIB)  $(PREFIX)/lib
	cp -f frei0r.h $(PREFIX)/include

	
.PHONY : clean
clean: 
	$(RM)  -f $(EXPORT_DYLIB_LIB) $(OBJS) $(EXPORT_STATIC_LIB)  
	
