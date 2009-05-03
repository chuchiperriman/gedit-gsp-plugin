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
	GtkWidget *statusbar;
	guint context_id;
	GtkActionGroup *action_group;
	guint ui_id;
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
parser_start_cb(GspProviderSnippets *provider, gpointer user_data)
{
	GspPlugin *self = GSP_PLUGIN (user_data);
	gtk_statusbar_push(GTK_STATUSBAR(self->priv->statusbar),
			   self->priv->context_id,
			   "Parsing On");
}

static void
parser_end_cb(GspProviderSnippets *provider, gpointer user_data)
{
	GspPlugin *self = GSP_PLUGIN (user_data);
	gtk_statusbar_pop(GTK_STATUSBAR(self->priv->statusbar),
			  self->priv->context_id);
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
	GspPlugin *self = GSP_PLUGIN (user_data);
	GeditView *view = gedit_tab_get_view (tab);
	GtkSourceCompletion *comp = gtk_source_view_get_completion (GTK_SOURCE_VIEW (view));
	
	GspProviderSnippets *sp = gsp_provider_snippets_new (GTK_TEXT_VIEW (view));
	gtk_source_completion_add_provider (comp,GTK_SOURCE_COMPLETION_PROVIDER(sp), NULL);
	g_signal_connect(sp,"parser-start",G_CALLBACK(parser_start_cb),self);
	g_signal_connect(sp,"parser-end",G_CALLBACK(parser_end_cb),self);
	
	g_object_unref(sp);
	
	g_debug ("snippets provider registered");
}

static void
impl_activate (GeditPlugin *plugin,
	       GeditWindow *window)
{
	GspPlugin *self = (GspPlugin*)plugin;
	self->priv->gedit_window = window;
	gedit_debug (DEBUG_PLUGINS);
	
	g_signal_connect (window, "tab-added",
			  G_CALLBACK (tab_added_cb),
			  self);
	
	/**********Adding menu items*************/
	GtkUIManager *manager;
	GError *error = NULL;
	
	manager = gedit_window_get_ui_manager (window);

	self->priv->action_group = gtk_action_group_new ("GspActions");
	
	gtk_action_group_set_translation_domain (self->priv->action_group, 
						 GETTEXT_PACKAGE);
	gtk_action_group_add_actions (self->priv->action_group,
				      action_entries,
				      G_N_ELEMENTS (action_entries), 
				      window);

	gtk_ui_manager_insert_action_group (manager, self->priv->action_group, -1);

	self->priv->ui_id = gtk_ui_manager_add_ui_from_string (manager,
							 submenu,
							 -1,
							 &error);
	if (self->priv->ui_id == 0)
	{
		g_warning ("UI Error: %s",error->message);
		return;
	}

	/********* Adding statusbar ************/
	GtkWidget *gedit_statusbar = gedit_window_get_statusbar(window);
	self->priv->statusbar = gtk_statusbar_new ();
        gtk_widget_show (self->priv->statusbar);
        gtk_widget_set_size_request (self->priv->statusbar,
                                     100,
                                     10);
        gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (self->priv->statusbar),
                                           FALSE);
        gtk_box_pack_end (GTK_BOX (gedit_statusbar),
                          self->priv->statusbar,
                          FALSE, TRUE, 0);
        self->priv->context_id =
        	 gtk_statusbar_get_context_id(GTK_STATUSBAR (self->priv->statusbar),
        	 				"Parser Status Bar");

}

static void
impl_deactivate (GeditPlugin *plugin,
		 GeditWindow *window)
{
	gedit_debug (DEBUG_PLUGINS);
	GspPlugin *self = GSP_PLUGIN (plugin);
	GtkUIManager *manager;
	manager = gedit_window_get_ui_manager (window);

	gtk_ui_manager_remove_ui (manager, self->priv->ui_id);
	gtk_ui_manager_remove_action_group (manager, self->priv->action_group);

	GtkWidget *gedit_statusbar = gedit_window_get_statusbar(window);
	gtk_container_remove(GTK_CONTAINER(gedit_statusbar),self->priv->statusbar);
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

