include utils/rt.mk

BUILDDIR = $(CURDIR)/build
INSTALLDIR = $(CURDIR)/install

ifdef LOCAL_BUILD
OPTIONS=BUILDDIR=$(BUILDDIR) INSTALLDIR=$(INSTALLDIR)
GVSOC=$(INSTALLDIR)/bin/gvsoc
else
GVSOC=../install/bin/gvsoc
endif

clean:
	rm -f *.o $(BUILDDIR)/test
	make -C .. TARGETS=pcm_testbench MODULES=$(CURDIR) $(OPTIONS) clean

all:
	mkdir -p $(BUILDDIR)
	riscv64-unknown-elf-gcc -g -o $(BUILDDIR)/test main.c $(RT_SRCS) $(RT_FLAGS)

gvsoc:
	make -C .. TARGETS=pcm_testbench MODULES=$(CURDIR) $(OPTIONS) build

run:
	$(GVSOC) --target-dir=$(CURDIR) --target=pcm_testbench --work-dir=$(BUILDDIR) --binary=$(BUILDDIR)/test run $(runner_args)
