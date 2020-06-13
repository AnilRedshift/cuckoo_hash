#Compiler and Linker
CC          := gcc

#The Target Binary Program
TARGET      := cuckoo_hash

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
TESTDIR		:= tests
BUILDDIR    := obj
TARGETDIR   := bin
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -Wall -g
LIB         := -lm
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
OBJECTSNOMAIN := $(filter-out $(BUILDDIR)/main.o,$(OBJECTS))

TEST_SOURCES := $(shell find $(TESTDIR) -type f -name test*.$(SRCEXT))
TEST_TARGETS := $(patsubst $(TESTDIR)/%.$(SRCEXT),%,$(TEST_SOURCES))

#Defauilt Make
all: directories $(TARGET)

test: $(TEST_TARGETS) run_tests

run_tests: $(TEST_TARGETS)
	$(info $(shell for file in $(TARGETDIR)/tests/*; do $$file 2>&1; done))

#Remake
remake: cleaner all

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(TARGETDIR)/tests
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

$(TEST_TARGETS): $(OBJECTSNOMAIN)
	$(CC) -o $(TARGETDIR)/tests/$@ $(TESTDIR)/$@.$(SRCEXT) -I src $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp


#Non-File Targets
.PHONY: all remake clean cleaner resources test run_tests
