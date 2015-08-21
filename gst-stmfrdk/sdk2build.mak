ifeq ($(SDK2_SOURCE_STMFAPP),)
$(error SDK2_SOURCE_STMFAPP must be specified to build the stmfapp module)
endif
ENVIRONMENT+=SDK2_SOURCE_STMFAPP

packages+=stmfapp

ifeq ($(ARCH),armv7)
else ifeq ($(ARCH),armv7_uclibc)
else ifeq ($(ARCH),sh4)
else ifeq ($(ARCH),sh4_uclibc)
else
$(error Wrong arch: $(ARCH))
endif

stmfapp_target_usr:=$(TARGET_DIR)/usr

stmfapp_rel_configure_in:=Makefile.am configure.ac
stmfapp_rel_configure_out:=Makefile Makefile.in aclocal.m4 config.guess config.h config.h.in config.status config.sub configure depcomp install-sh missing src/Makefile src/Makefile.in stamp-h1 compile

stmfapp_configure_in=$(addprefix $(SDK2_SOURCE_STMFAPP)/,$(stmfapp_rel_configure_in))
stmfapp_configure_out=$(addprefix $(SDK2_SOURCE_STMFAPP)/,$(stmfapp_rel_configure_out))


$(stmfapp_configure_out): $(stmfapp_configure_in)
	cd $(SDK2_SOURCE_STMFAPP) ; ./build_module.sh -c && touch config.h

stmfapp: $(stmfapp_configure_out)
	cd $(SDK2_SOURCE_STMFAPP) ; ./build_module.sh -b


.install_stmfapp:
	cd $(SDK2_SOURCE_STMFAPP); ./build_module.sh -i

.clean_stmfapp:
	- cd $(SDK2_SOURCE_STMFAPP); test -f Makefile && $(MAKE) prefix=$(stmfapp_target_usr) clean
	rm -f $(stmfapp_configure_out)
	touch $(stmfapp_configure_in)

.stmfapp_help:
	@printf "\n STMFAPP specific targets:\n"
	@printf "        stmfapp - Build just the STMFAPP application\n"


EXTRA_RULES+=stmfapp
EXTRA_INSTALL_RULES+=.install_stmfapp
CLEAN_RULES+=.clean_stmfapp
HELP_RULES+=.stmfapp_help
