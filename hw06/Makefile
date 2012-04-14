CC = gcc
# -g : allows use of GNU Debugger
# -Wall : show all warnings
FLAGS = -g -Wall -Werror
LIBS = # None yet...
SOURCE = main.c paralloc.c child.c list.c
OUTPUT = main

build: $(SOURCE)
	@# Call the compiler with source & output arguments
	$(CC) $(LIBS) $(FLAGS) -o $(OUTPUT) $(SOURCE)
	@# Make the output file executable
	chmod u+x $(OUTPUT)

CLEAN_MSG = "All resources closed/removed."
UNCLEAN_MSG = "Please clean resources using 'ipcrm'"
# Make and run program
run: build
	@echo
	-@./$(OUTPUT)
#@echo
#@echo "Checking for open resources..."
# @ipcs | grep -q -w $(USER); if [ $$? = 1 ] ; then echo $(CLEAN_MSG); else ipcs && echo $(UNCLEAN_MSG); fi

# 'clean' rule for remove non-source files
# To use, call 'make clean'
# Note: you don't have to call this in between every
#       compilation, it's only in case you need to
#       clean out your directory for some reason.
clean:
	@# Using the '@' sign suppresses echoing
	@# the line while the command is run
	@rm -f $(OUTPUT) *.s

