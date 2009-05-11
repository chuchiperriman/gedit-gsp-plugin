plugindir = $(libdir)/gedit-2/plugins

INCLUDES = \
	-I$(top_srcdir) 						\
	$(GEDIT_CFLAGS) 						\
	$(WARN_CFLAGS)							\
	$(DISABLE_DEPRECATED_CFLAGS)					\
	-DICON_DIR=\""$(datadir)/gedit-gsp-plugin/pixmaps"\" \
	`pkg-config --cflags gtksnippets-1.0` 	\
	-DGEDIT_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

AM_CFLAGS =\
         -Wall\
         -g

plugin_LTLIBRARIES = libgsp.la

libgsp_la_SOURCES = \
	chcompletionutils.h		\
	chcompletionutils.c		\
	gsp-provider-snippets.h		\
	gsp-provider-snippets.c		\
	gsp-plugin.h			\
	gsp-plugin.c

libgsp_la_LDFLAGS = $(PLUGIN_LIBTOOL_FLAGS) `pkg-config --libs gtksnippets-1.0`

# Glade files (if you use glade for your plugin, list those files here)
gladedir = $(datadir)/gedit-2/glade
glade_DATA =

plugin_in_files = gsp.gedit-plugin.desktop.in

%.gedit-plugin: %.gedit-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.gedit-plugin.desktop.in=.gedit-plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA) $(glade_DATA)

DISTCLEANFILES = $(plugin_DATA) $(glade_DATA)
