cc = arch -x86_64 msp430-elf-gcc

flags = -mmcu=msp430fr5994
flags += -nostdlib
flags += -nostartfiles
flags += -T linker.ld

src = src
obj = obj

sources = $(shell find $(src) -name "*.c")
objects = $(patsubst $(src)/%,$(obj)/%,$(sources:.c=.o))

target = soren

$(obj)/%.o: $(src)/%.c
	@mkdir -p $(dir $@)
	@echo "compiling $<..."
	@$(cc) $(flags) -c $< -o $@

$(obj)/$(target).o: $(target).s
	@mkdir -p $(dir $@)
	@echo "assembling $<..."
	@$(cc) $(flags) -c $< -o $@

all:
	@echo "available build options for soren"
	@echo "make build    build firmware"
	@echo "make clean    clean compiled assets"

build: $(objects) $(obj)/$(target).o
	@echo "linking $(target)..."
	@$(cc) $(flags) -o $(target).elf $(objects) $(obj)/soren.o

flash:
	@echo "flashing $(target)..."
	@printf "prog $(target).elf\nverify $(target).elf\n" | mspdebug ezfet

start:
		@echo "starting $(target)..."
		@printf "run" | mspdebug ezfet

clean:
	@echo "cleaning up..."
	@rm -rf $(obj) $(target).elf
