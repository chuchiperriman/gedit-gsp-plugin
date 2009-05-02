/*
 * gsp-plugin.c - Adds (auto)completion support to gedit
 *
 * Copyright (C) 2007 - chuchiperriman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gsp-plugin.h"

#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <gedit/gedit-debug.h>
#include "gsp-provider-snippets.h"

#include <gsnippets/gsnippets-func-manager.h>
#include <gtksnippets/gtksnippets-dialog.h>

static void open_snippets_manager_cb (GtkAction   *action, gpointer user_data);

#define GSP_PLUGIN_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSP_TYPE_PLUGIN, GspPluginPrivate))

#define GSP_PROVIDERS_KEY "gsp-providers"

const gchar submenu[] =
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu name='EditMenu' action='Edit'>"
"      <placeholder name='EditOps_6'>"
"        <menuitem action='SnippetsManager'/>"
"      </placeholder>"
"    </menu>"
"  </menubar>"
"</ui>";

static const GtkActionEntry action_entries[] =
{
	{ "SnippetsManager", NULL, N_("Snippets Manager"), NULL,
	  N_("Opens Snippets Manager dialog to add/remove snippets"),
	  G_CALLBACK (open_snippets_manager_cb) }
};

struct _GspPluginPrivate
{
	GeditWindow *gedit_window;
	GtkWidget *window;
};

typedef struct _ViewAndCompletion ViewAndCompletion;

GEDIT_PLUGIN_REGISTER_TYPE (GspPlugin, gsp_plugin)

static void
manager_destroy_cb(GtkWidget *w, gpointer user_data)
{
	gtk_widget_destroy(w);
}


static void
open_snippets_manager_cb (GtkAction   *action,
               gpointer user_data)
{
	g_debug("Open Snippets Manager");
	GtkWidget *w = GTK_WIDGET(gtksnippets_dialog_new());
	/* Signals */
	g_signal_connect(
		w,
		"destroy",
		G_CALLBACK(manager_destroy_cb),
		NULL);
	g_signal_connect(
		w,
		"hide",
		G_CALLBACK(manager_destroy_cb),
		NULL);
		
	gtk_widget_show_all(w);

}

static void
gsp_plugin_init (GspPlugin *plugin)
{
	plugin->priv = GSP_PLUGIN_GET_PRIVATE (plugin);
	gedit_debug_message (DEBUG_PLUGINS,
			     "GspPlugin initializing");
}

static void
gsp_plugin_finalize (GObject *object)
{
	gedit_debug_message (DEBUG_PLUGINS,
			     "GspPlugin finalizing");
	G_OBJECT_CLASS (gsp_plugin_parent_class)->finalize (object);
}

static void
tab_added_cb (GeditWindow *geditwindow,
	      GeditTab    *tab,
	      gpointer     user_data)
{
	GList *providers = NULL;
	GeditView *view = gedit_tab_get_view (tab);
	GtkSourceCompletion *comp = gtk_source_view_get_completion (GTK_SOURCE_VIEW (view));
	
	GspProviderSnippets *sp = gsp_provider_snippets_new (GTK_TEXT_VIEW (view));
	gtk_source_completion_add_provider (comp,GTK_SOURCE_COMPLETION_PROVIDER(sp), NULL);
	g_object_unref(sp);
	
	providers = g_list_append (providers, sp);
	
	g_object_set_data_full (G_OBJECT (comp),
				GSP_PROVIDERS_KEY,
				providers,
				(GDestroyNotify) g_list_free);
	
	g_debug ("snippets provider registered");
}

static void
impl_activate (GeditPlugin *plugin,
	       GeditWindow *window)
{
	GspPlugin * dw_plugin = (GspPlugin*)plugin;
	dw_plugin->priv->gedit_window = window;
	gedit_debug (DEBUG_PLUGINS);
	
	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  NULL);

}

static void
impl_deactivate (GeditPlugin *plugin,
		 GeditWindow *window)
{
	gedit_debug (DEBUG_PLUGINS);
}

static void
impl_update_ui (GeditPlugin *plugin,
		GeditWindow *window)
{
}

static void
gsp_plugin_class_init (GspPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = gsp_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;

	g_type_class_add_private (object_class, 
				  sizeof (GspPluginPrivate));
}

