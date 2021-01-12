#
# Makefile generated by:
# codebench 0.55
#
# Project: AmiMarkdown
#
# Created on: 12-01-2021 22:50:46
#
#

###################################################################
##
##////  Objects
##
###################################################################

AmiMarkdown_OBJ := \
	 src/amimd.o src/editor_gadget.o src/gui.o \
	 src/md_to_html.o src/settings_gadget.o src/viewer_gadget.o \
	 md4c/src/entity.o md4c/src/md4c.o md4c/src/md4c-html.o \
	 src/prefs.o src/byte_buffer.o src/string_utils.o \
	 src/image_editor.o src/table_editor.o src/hyperlink_editor.o \
	 src/json_util.o src/requester.o


###################################################################
##
##////  Variables and Environment
##
###################################################################

CC := gcc:bin/gcc

INCPATH := -I.

CFLAGS := $(INCPATH) -gstabs  -Werror -Wwrite-strings -I SDK:MUI/C/Include -Iinclude -Imd4c/src -ISDK:include/include_h/jansson -DAMIGA=1 -D_DBUG=1


###################################################################
##
##////  General rules
##
###################################################################

.PHONY: all all-before all-after clean clean-custom realclean

all: all-before AmiMarkdown all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	rm -f -v  $(AmiMarkdown_OBJ)

realclean:
	rm -f -v  $(AmiMarkdown_OBJ) AmiMarkdown


###################################################################
##
##////  Targets
##
###################################################################

AmiMarkdown: $(AmiMarkdown_OBJ)
	gcc:bin/gcc -o AmiMarkdown.debug $(AmiMarkdown_OBJ) -lauto
	cp -f -p AmiMarkdown.debug AmiMarkdown


###################################################################
##
##////  Standard rules
##
###################################################################

src/json_util.o: src/json_util.c
	$(CC) -c src/json_util.c -o src/json_util.o $(CFLAGS)

src/hyperlink_editor.o: src/hyperlink_editor.c
	$(CC) -c src/hyperlink_editor.c -o src/hyperlink_editor.o $(CFLAGS)

src/table_editor.o: src/table_editor.c
	$(CC) -c src/table_editor.c -o src/table_editor.o $(CFLAGS)

src/byte_buffer.o: src/byte_buffer.c
	$(CC) -c src/byte_buffer.c -o src/byte_buffer.o $(CFLAGS)

src/string_utils.o: src/string_utils.c
	$(CC) -c src/string_utils.c -o src/string_utils.o $(CFLAGS)

src/amimd.o: src/amimd.c
	$(CC) -c src/amimd.c -o src/amimd.o $(CFLAGS)

src/editor_gadget.o: src/editor_gadget.c
	$(CC) -c src/editor_gadget.c -o src/editor_gadget.o $(CFLAGS)

src/gui.o: src/gui.c
	$(CC) -c src/gui.c -o src/gui.o $(CFLAGS)

src/md_to_html.o: src/md_to_html.c
	$(CC) -c src/md_to_html.c -o src/md_to_html.o $(CFLAGS)

src/viewer_gadget.o: src/viewer_gadget.c
	$(CC) -c src/viewer_gadget.c -o src/viewer_gadget.o $(CFLAGS)

md4c/src/entity.o: md4c/src/entity.c
	$(CC) -c md4c/src/entity.c -o md4c/src/entity.o $(CFLAGS)

md4c/src/md4c.o: md4c/src/md4c.c
	$(CC) -c md4c/src/md4c.c -o md4c/src/md4c.o $(CFLAGS)

md4c/src/md4c-html.o: md4c/src/md4c-html.c md4c/src/md4c-html.h md4c/src/md4c.h \
	
	$(CC) -c md4c/src/md4c-html.c -o md4c/src/md4c-html.o $(CFLAGS)

src/prefs.o: src/prefs.c
	$(CC) -c src/prefs.c -o src/prefs.o $(CFLAGS)

src/settings_gadget.o: src/settings_gadget.c
	$(CC) -c src/settings_gadget.c -o src/settings_gadget.o $(CFLAGS)

src/image_editor.o: src/image_editor.c
	$(CC) -c src/image_editor.c -o src/image_editor.o $(CFLAGS)

src/requester.o: src/requester.c
	$(CC) -c src/requester.c -o src/requester.o $(CFLAGS)

