BUILDDIR ?= build
DIRNAME ?= dirname
LATEXMK ?= latexmk
LN ?= ln
MKDIR ?= mkdir
TOUCH ?= touch
LATEX ?= xelatex
SED ?= sed
LATEXMK_FLAGS += -logfilewarninglist -interaction=$(PDFLTX_INTERACTION) -halt-on-error -pdf -M -MP
LATEXMK_FLAGS += -bibtex --xelatex
# $(@D) always evaluates to the directory containing the current target
LATEXMK_FLAGS += -use-make -e '$$auxdir = "$(@D)"; $$out_dir = "$(@D)"; $$tmpdir = "$(@D)"; $$recorder = 1; '
XELATEX = xelatex
XELATEX_FLAGS = --output-directory=$(BUILDDIR)
CLEAN_TEST_INSTALL ?= /usr

# Root documents
ROOT_DOC = main.tex
TOPLVL_TGTS = $(foreach tex,$(ROOT_DOC),$(BUILDDIR)/$(tex:.tex=.pdf))

# Include local overwrites (if any)
-include Make.local


# determine toplevel goal: if no target was specified, it is the default target
ifeq ($(MAKECMDGOALS),)
MAIN_TOPLVL_TARGET = $(.DEFAULT_GOAL)
else
MAIN_TOPLVL_TARGET = $(MAKECMDGOALS)
endif

# determine the set of toplvl pdfs to be built
ifeq ($(filter-out all,$(MAIN_TOPLVL_TARGET)),)
CURRENT_ROOT_TGTS=$(ROOT_TGT_FINAL) $(ROOT_TGT_DRAFT)
endif

# Files of the make env that the build process depends on
# This does, on purpose, not contain Make.local
MAKEFILE_DEPS=Makefile

.PHONY: all clean demo-config
all: demo-config $(MAKEFILE_DEPS) $(TOPLVL_TGTS)

# Include generated dependencies
ifneq ($(MAKECMDGOALS),clean)
${foreach file,${shell find ${BUILDDIR} -type f -name "*.d" 2>/dev/null},${eval -include $(file)}}
endif

define MK_TARGETDIR
	$(MKDIR) -p $$($(DIRNAME) $@)
endef

$(BUILDDIR):
	$(MKDIR) -p $(@)/demos

clean:
	$(RM) -rf $(BUILDDIR)

demo-config:
	cmake demos -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$(CLEAN_TEST_INSTALL)" -GNinja -B "$(BUILDDIR)/demos"
#	Some demos are not included as console, but they should be verified (compile) still.
	cmake --build "$(BUILDDIR)/demos"

$(BUILDDIR)/%.pdf: %.tex
	$(MK_TARGETDIR)
	@export CURRENT_PDF_TARGET=$(@) && \
	$(LATEXMK) $(LATEXMK_TOPLVL_FLAGS) $(LATEXMK_FLAGS) $(FORCE_REBUILD_FLAG) -MF $(@:.pdf=.d) $(<);

$(BUILDDIR)/%.pdf: %.drawio
	$(MK_TARGETDIR)
	drawio --crop --transparent -x -o "$(@)" "$(<)"

$(BUILDDIR)/%.console.tex: %.cpp $(BUILDDIR)/%
	$(MK_TARGETDIR)
	cd "$(BUILDDIR)/demos" && ninja $$(realpath --relative-to "$(BUILDDIR)/demos" "$(@:.console.tex=)")
	./convert-console.py $(@:.console.tex=) > "$(@)"
