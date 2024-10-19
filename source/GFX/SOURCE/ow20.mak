# ----- General Definitions -------------------------------------------------
VERSION=ow20
TRANS=-DZIL_TRANS

.EXTENSIONS:
.EXTENSIONS: .exe .o32 .c .cpp

# ----- DOS compiler options ------------------------------------------------
CXX=wcc386 -zq
LINK=wlink op q
LIBRARIAN=wlib -zq

LIB_OPTS=-pa -n

D32_CXX_OPTS=-bt=dos -3r -s -DDOSX16=2 -DPM=32 $(TRANS)
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000

.c.o32:
	$(CXX) $(CXX_OPTS) $(D32_CXX_OPTS) -fo=$@ $<

# ----- Usage ---------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To generate the GFX library for specific environments type:
	@echo wmake -f ow20.mak dos32
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	z_clean.bat

../../../lib/$(VERSION):
	@md $@

# ----- D32 Libraries -------------------------------------------------------

dos32: ../../../lib/$(VERSION) wc_32gfx.lib .SYMBOLIC

wc_32gfx.lib : gfx.o32 ../asm/wcgfxa32.lib
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%copy wc_32gfx.lib ../../../lib/$(VERSION)
