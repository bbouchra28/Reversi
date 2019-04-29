#  Variables
EXE=reversi

#  Special  rules  and  targets
.PHONY: all  build  clean  help

#  Rules  and  targets
all: build

build:
	@cd  src && $(MAKE)

clean:
	@cd src && $(MAKE) clean
	@rm -f $(EXE)

help:
	@echo " Usage: "
	@echo -e " make [all]\t\ Run the target build"
	@echo -e " make build\t\ Build the executable file reversi recursively "
	@echo -e " make clean\t\ Remove all files produced by the compilation"
	@echo -e " make help\t\ Display the main of this Makefile with description"
