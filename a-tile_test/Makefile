BUILDDIR = $(CURDIR)/build
INSTALLDIR = $(CURDIR)/install

ifdef LOCAL_BUILD
OPTIONS=BUILDDIR=$(BUILDDIR) INSTALLDIR=$(INSTALLDIR)
GVSOC=$(INSTALLDIR)/bin/gvsoc
else
GVSOC=../install/bin/gvsoc
endif

TEST_SRCS += ../main.c

INC += -I../inc
INC += -I../utils
INC += -I../hal

FLAGS ?=

CC_OPTS += -march=rv32imafc -mabi=ilp32f -D__riscv__ -O2 -g -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections -MMD -MP
LD_OPTS += -march=rv32imafc -mabi=ilp32f -D__riscv__ -MMD -MP -nostartfiles -nostdlib -Wl,--gc-sections

CRT = $(BUILDDIR)/crt0.o
OBJ = $(BUILDDIR)/verif.o
BIN = $(BUILDDIR)/verif
DUMP = $(BUILDDIR)/verif.dump
ODUMP = $(BUILDDIR)/verif.objdump
ITB = $(BUILDDIR)/verif.itb
STIM_INSTR = $(BUILDDIR)/stim_instr.txt
STIM_DATA = $(BUILDDIR)/stim_data.txt

clean:
	rm -rf $(BUILDDIR)

clean_gvsoc:
	make -C .. TARGETS=a-tile_testbench MODULES=$(CURDIR) clean

all:
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR) 																	&& \
	riscv32-unknown-elf-gcc $(CC_OPTS) -c ../kernel/crt0.S -o $(CRT)				&& \
	riscv32-unknown-elf-gcc $(CC_OPTS) -c $(TEST_SRCS) $(FLAGS) $(INC) -o $(OBJ)
	riscv32-unknown-elf-gcc $(LD_OPTS) -o $(BIN) $(CRT) $(OBJ) -Tkernel/link.ld

dis:
	riscv32-unknown-elf-objdump -d -S $(BIN) > $(DUMP)

objdump:
	riscv32-unknown-elf-objdump -d -l -s $(BIN) > $(ODUMP)

gvsoc:
	make -C .. TARGETS=a-tile_testbench MODULES=$(CURDIR) $(OPTIONS) build

run:
	$(GVSOC) --target-dir=$(CURDIR) --target=a-tile_testbench --work-dir=$(BUILDDIR) --binary=$(BIN) run $(runner_args)