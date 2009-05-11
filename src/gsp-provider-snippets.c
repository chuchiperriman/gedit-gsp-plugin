/* 
 *  gsp-provider-snippets.c - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gsp-provider-snippets.h"
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletionitem.h>

#include <gsnippets/gsnippets-db.h>
#include <gsnippets/gsnippets-item.h>
#include <gtksnippets/gtksnippets-inplaceparser.h>

#include "chcompletionutils.h"

#define GSP_PROVIDER_SNIPPETS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GSP_TYPE_PROVIDER_SNIPPETS, GspProviderSnippetsPrivate))

/* Signals */
enum
{
	PARSER_START,
	PARSER_END,
	LAST_SIGNAL
};

static void	 gsp_provider_snippets_iface_init	(GtkSourceCompletionProviderIface *iface);

static guint signals[LAST_SIGNAL] = { 0 };

struct _GspProviderSnippetsPrivate
{
	GtkTextView			*view;
	GdkPixbuf			*provider_icon;
	GdkPixbuf			*proposal_icon;
	GSnippetsDb 			*db;
	GtkSnippetsInPlaceParser	*parser;
};

G_DEFINE_TYPE_WITH_CODE (GspProviderSnippets,
			 gsp_provider_snippets,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		gsp_provider_snippets_iface_init))

static gchar*
get_item_info_markup(GSnippetsItem *snippet)
{
	gchar* content = g_markup_escape_text(gsnippets_item_get_content(snippet),-1);
	return content;
}

static void
parser_start_cb(GtkSnippetsInPlaceParser *parser, gpointer user_data)
{
	g_signal_emit (G_OBJECT (user_data), signals[PARSER_START], 0);
}

static void
parser_end_cb(GtkSnippetsInPlaceParser *parser, gpointer user_data)
{
	g_signal_emit (G_OBJECT (user_data), signals[PARSER_END], 0);
}

static const gchar * 
gsp_provider_snippets_get_name (GtkSourceCompletionProvider *self)
{
	return "Snippets";
}

static GdkPixbuf * 
gsp_provider_snippets_get_icon (GtkSourceCompletionProvider *self)
{
	return GSP_PROVIDER_SNIPPETS (self)->priv->provider_icon;
}

static gboolean
gsp_provider_snippets_activate_proposal (GtkSourceCompletionProvider	*provider,
					   GtkSourceCompletionProposal	*proposal,
					   GtkTextIter			*iter)
{
	GspProviderSnippets *self = GSP_PROVIDER_SNIPPETS (provider);
	gint id = (gint)g_object_get_data (G_OBJECT (proposal), "id");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->priv->view);

	GSnippetsItem* snippet = gsnippets_db_load(
			self->priv->db,
			id);
	
	const gchar* content = gsnippets_item_get_content(snippet);
	ch_completion_replace_current_word (GTK_SOURCE_BUFFER (buffer), "", 0);
	gtksnippets_inplaceparser_activate (self->priv->parser,
					    content);
	
	return TRUE;
}

static GList *
gsp_provider_snippets_get_proposals (GtkSourceCompletionProvider *base,
                                 GtkTextIter                 *iter)
{
	GspProviderSnippets *self = GSP_PROVIDER_SNIPPETS (base);
	
	if (self->priv->parser==NULL)
	{
		self->priv->parser = gtksnippets_inplaceparser_new(self->priv->view);
		g_signal_connect(self->priv->parser,"parser-start",G_CALLBACK(parser_start_cb),self);
		g_signal_connect(self->priv->parser,"parser-end",G_CALLBACK(parser_end_cb),self);
	}
	
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(self->priv->view);
	
	GSList *list = NULL;
	GList *item_list = NULL;
	gchar *name;
	
	if (GTK_IS_SOURCE_BUFFER(buffer))
	{
		/*TODO Filter by snippet name in all the selections*/
		GtkSourceLanguage *lang = gtk_source_buffer_get_language(GTK_SOURCE_BUFFER(buffer));
		if (lang!=NULL)
		{
			list = gsnippets_db_get_by_lang_name(
						self->priv->db,
						gtk_source_language_get_id(lang));
		}
		else
		{
			list = gsnippets_db_get_all(self->priv->db);
		}
		
	}
	else
	{
		list = gsnippets_db_get_all(self->priv->db);
	}
	
	GSList *list_temp;
	GSnippetsItem* snippet;
	GtkSourceCompletionProposal *item;
	gchar *markup;
	
	if (list!=NULL){
		list_temp = list;	
		do{
			snippet = (GSnippetsItem*)list_temp->data;
 			name = g_strdup(gsnippets_item_get_name(snippet));
			/*TODO filter by current word*/
			markup = get_item_info_markup(snippet);
			
			item = GTK_SOURCE_COMPLETION_PROPOSAL (gtk_source_completion_item_new((gchar*)name,
						       NULL,
						       self->priv->proposal_icon,
						       markup));

			g_object_set_data (G_OBJECT (item), "id", gsnippets_item_get_id(snippet));
			
			item_list = g_list_append(item_list,item);
			g_free(markup);
			g_free(name);
			g_object_unref(snippet);
		}while((list_temp = g_slist_next(list_temp))!= NULL);
		
		g_slist_free(list);
	}

	return item_list;
	
}

static gboolean
gsp_provider_snippets_filter_proposal (GtkSourceCompletionProvider *provider,
                                   GtkSourceCompletionProposal *proposal,
                                   GtkTextIter                 *iter,
                                   const gchar                 *criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}

static gboolean
gsp_provider_snippets_get_interactive (GtkSourceCompletionProvider *provider)
{
	return TRUE;
}

static void 
gsp_provider_snippets_finalize (GObject *object)
{
	GspProviderSnippets *self = GSP_PROVIDER_SNIPPETS (object);
	
	if (self->priv->provider_icon != NULL)
	{
		g_object_unref (self->priv->provider_icon);
	}
	
	if (self->priv->proposal_icon != NULL)
	{
		g_object_unref (self->priv->proposal_icon);
	}
	
	gsnippets_db_disconnect(self->priv->db);
	g_object_unref(self->priv->db);
	self->priv->view = NULL;

	G_OBJECT_CLASS (gsp_provider_snippets_parent_class)->finalize (object);
}

static void 
gsp_provider_snippets_class_init (GspProviderSnippetsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = gsp_provider_snippets_finalize;
	
	g_type_class_add_private (object_class, sizeof(GspProviderSnippetsPrivate));
	
	signals[PARSER_START] =
		g_signal_new ("parser-start",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__VOID, 
			      G_TYPE_NONE,
			      0);
	signals[PARSER_END] =
		g_signal_new ("parser-end",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__VOID, 
			      G_TYPE_NONE,
			      0);
}

static void
gsp_provider_snippets_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = gsp_provider_snippets_get_name;
	iface->get_icon = gsp_provider_snippets_get_icon;

	iface->get_proposals = gsp_provider_snippets_get_proposals;
	iface->filter_proposal = gsp_provider_snippets_filter_proposal;
	iface->get_interactive = gsp_provider_snippets_get_interactive;
	iface->activate_proposal = gsp_provider_snippets_activate_proposal;
}

static void 
gsp_provider_snippets_init (GspProviderSnippets * self)
{
	GtkIconTheme *theme;
	gint width;
	
	self->priv = GSP_PROVIDER_SNIPPETS_GET_PRIVATE (self);
	
	theme = gtk_icon_theme_get_default ();

	gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, NULL);
	self->priv->proposal_icon = gtk_icon_theme_load_icon (theme,
	                                                      GTK_STOCK_YES,
	                                                      width,
	                                                      GTK_ICON_LOOKUP_USE_BUILTIN,
	                                                      NULL);
	
	self->priv->provider_icon = gtk_icon_theme_load_icon (theme,
	                                                      GTK_STOCK_YES,
	                                                      width,
	                                                      GTK_ICON_LOOKUP_USE_BUILTIN,
	                                                      NULL);
	self->priv->parser = NULL;
	self->priv->view = NULL;
	self->priv->db = gsnippets_db_new();
	gsnippets_db_connect(self->priv->db);
}

GspProviderSnippets *
gsp_provider_snippets_new (GtkTextView *view)
{
	GspProviderSnippets *ret = g_object_new (GSP_TYPE_PROVIDER_SNIPPETS, NULL);
	ret->priv->view = view;
	return ret;
}

