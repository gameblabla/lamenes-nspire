LAMENES		= main.c lamenes.c lame6502/lame6502.c lame6502/disas.c lame6502/debugger.c lib/str_chrchk.c lib/str_cut.c sdl_functions.c romloader.c ppu.c input.c lib/str_replace.c
CC		= gcc
OBJ_FLAG	= -Ofast -fomit-frame-pointer -fno-math-errno -fgcse-sm -fgcse-las -funsafe-loop-optimizations -fno-fat-lto-objects -frename-registers -fprefetch-loop-arrays -ffunction-sections -fdata-sections -Wall -I/usr/local/include -lSDL -pthread;

lamenes:	$(LAMENES)
		$(CC) $(LAMENES) $(OBJ_FLAG)
		strip a.out
		mv a.out lamenes

install:	$(LAMENES)
		cp lamenes /usr/local/bin

uninstall:	$(LAMENES)
		rm /usr/local/bin/lamenes

clean:		$(LAMENES)
		rm -f lamenes lamenes.core core.*
