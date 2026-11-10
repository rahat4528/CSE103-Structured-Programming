# ==============================================================================
# CSE103 Root Makefile: Delegates to OS-C-Programming-Lab
# ==============================================================================

OS_DIR := OS-C-Programming-Lab

.PHONY: all practice class_work assignments labs prep final_examples test clean help

all:
	@$(MAKE) -C $(OS_DIR) all

practice:
	@$(MAKE) -C $(OS_DIR) practice

class_work:
	@$(MAKE) -C $(OS_DIR) class_work

assignments:
	@$(MAKE) -C $(OS_DIR) assignments

labs:
	@$(MAKE) -C $(OS_DIR) labs

prep:
	@$(MAKE) -C $(OS_DIR) prep

final_examples:
	@$(MAKE) -C $(OS_DIR) final_examples

test:
	@$(MAKE) -C $(OS_DIR) test

clean:
	@$(MAKE) -C $(OS_DIR) clean

help:
	@$(MAKE) -C $(OS_DIR) help
