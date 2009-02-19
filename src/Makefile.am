plugindir = $(libdir)/gedit-2/plugins

INCLUDES = \
	-I$(top_srcdir) 						\
	$(GEDIT_CFLAGS) 						\
	$(WARN_CFLAGS)							\
	$(DISABLE_DEPRECATED_CFLAGS)					\
	-DICON_DIR=\""$(datadir)/gedit-snippetscompletion-plugin/pixmaps"\" \
	`pkg-config --cflags gtksourcecompletion-2.0`			\
	`pkg-config --cflags gtksnippets-1.0` 	\
	-DGEDIT_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = libsnippetscompletion.la

libsnippetscompletion_la_SOURCES = \
	snippet-proposal.h		\
	snippet-proposal.c		\
	snippetscompletion-plugin.h	\
	snippetscompletion-plugin.c	\
	gsc-snippets-provider.h		\
	gsc-snippets-provider.c

libsnippetscompletion_la_LDFLAGS = $(PLUGIN_LIBTOOL_FLAGS) `pkg-config --libs gtksourcecompletion-2.0` `pkg-config --libs gtksnippets-1.0`

# Glade files (if you use glade for your plugin, list those files here)
gladedir = $(datadir)/gedit-2/glade
glade_DATA =

plugin_in_files = snippetscompletion.gedit-plugin.desktop.in

%.gedit-plugin: %.gedit-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gedit-plugin.desktop.in=.gedit-plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA) $(glade_DATA)

DISTCLEANFILES = $(plugin_DATA) $(glade_DATA)
