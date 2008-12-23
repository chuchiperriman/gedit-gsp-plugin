 /* gsc-snippets-provider.c - Type here a short description of your plugin
 *
 * Copyright (C) 2008 - chuchi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gsc-snippets-provider.h"
 
#include <glib/gprintf.h>
#include <string.h>
#include <gsnippets/gsnippets-db.h>
#include <gsnippets/gsnippets-item.h>
#include <gtksnippets/gtksnippets-inplaceparser.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourcecompletion/gsc-utils.h>
#include "snippet-proposal.h"

#define ICON_FILE ICON_DIR"/snippets.png"
#define PAGE_NAME "Snippets"


/* Signals */
enum
{
	PARSER_START,
	PARSER_END,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

struct _GscSnippetsProviderPrivate {
	GdkPixbuf *icon;
	GSnippetsDb *db;
	GtkSnippetsInPlaceParser *parser;
	GtkTextView *view;
};

#define GSC_SNIPPETS_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_SNIPPETS_PROVIDER, GscSnippetsProviderPrivate))
enum  {
	GSC_SNIPPETS_PROVIDER_DUMMY_PROPERTY,
};
static const gchar* gsc_snippets_provider_real_get_name (GscProvider* self);
static GList* gsc_snippets_provider_real_get_data (GscProvider* base, GscTrigger *trigger);
static void gsc_snippets_provider_real_end_completion (GscProvider* base);
static void gsc_snippets_provider_real_data_selected (GscProvider* base, GscProposal* data);
static gchar* gsc_snippets_provider_real_get_item_info_markup (GscProvider *self, GscProposal *item);
static gpointer gsc_snippets_provider_parent_class = NULL;
static GscProviderIface* gsc_snippets_provider_gtk_source_completion_provider_parent_iface = NULL;


static gboolean
_is_valid_word(gchar *doc_word, gchar *comp_word)
{
	glong len_doc,len_comp;
	
	
	len_doc = g_utf8_strlen(doc_word,-1);
	len_comp = g_utf8_strlen(comp_word,-1);
	if (len_doc>len_comp)
		return FALSE;
	
	gchar temp[len_comp+1];
	
	g_utf8_strncpy(temp,comp_word,len_doc);
	if (g_utf8_collate(temp,doc_word)==0)
	{
		return TRUE;
	}
	
	return FALSE;
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

static gboolean
_apply_cd(GscProposal* proposal, GtkTextView *view, gpointer user_data)
{
	GscSnippetsProvider *self = GSC_SNIPPETS_PROVIDER(user_data);
	SnippetProposal *prop = SNIPPET_PROPOSAL(proposal);
	GSnippetsItem* snippet = gsnippets_db_load(
			self->priv->db,
			snippet_proposal_get_id(prop));
	g_assert(snippet!=NULL);
	
	const gchar* content = gsnippets_item_get_content(snippet);
	gsc_replace_actual_word(self->priv->view,"");
	gboolean parser_active = gtksnippets_inplaceparser_activate(self->priv->parser,content);
	
	g_object_unref(snippet);
	return TRUE;
}


static const gchar* gsc_snippets_provider_real_get_name (GscProvider* self)
{
	return GSC_SNIPPETS_PROVIDER_NAME;
}

static gchar*
_get_item_info_markup(GSnippetsItem *snippet)
{
	gchar* content = g_markup_escape_text(gsnippets_item_get_content(snippet),-1);
	return content;
}

static GList* 
gsc_snippets_provider_real_get_data (GscProvider* base, GscTrigger *trigger)
{
	g_debug("snippets get data");
	GscSnippetsProvider *self = GSC_SNIPPETS_PROVIDER(base);
	
	if (self->priv->parser==NULL)
	{
		self->priv->parser = gtksnippets_inplaceparser_new(self->priv->view);
		g_signal_connect(self->priv->parser,"parser-start",G_CALLBACK(parser_start_cb),self);
		g_signal_connect(self->priv->parser,"parser-end",G_CALLBACK(parser_end_cb),self);
	}
	
	gchar *current_word = gsc_get_last_word(self->priv->view);
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
	GscProposal *item;
	gchar *markup;
	
	if (list!=NULL){
		list_temp = list;	
		do{
			snippet = (GSnippetsItem*)list_temp->data;
 			name = g_strdup(gsnippets_item_get_name(snippet));
			/*TODO filter the selection by name instead of compare all snippet names*/
			if (_is_valid_word(current_word,name))
			{
				markup = _get_item_info_markup(snippet);
				item = GSC_PROPOSAL(snippet_proposal_new(name,
							markup,
							(gpointer)self->priv->icon,
							gsnippets_item_get_id(snippet)));
				
				gsc_proposal_set_page_name(item,
							     PAGE_NAME);
				
				g_signal_connect(item, 
					 "apply",
					 G_CALLBACK(_apply_cd),
					 self);
		
				item_list = g_list_append(item_list,item);
				g_free(markup);
			}
			g_free(name);
			g_object_unref(snippet);
		}while((list_temp = g_slist_next(list_temp))!= NULL);
		
		g_slist_free(list);
		
	}else{
		g_debug("There are no snippets");
	}

	return item_list;
}

static void gsc_snippets_provider_real_end_completion (GscProvider* base)
{

}

static void gsc_snippets_provider_real_data_free (GscProvider* self, GscProposal* data)
{
}

static void gsc_snippets_provider_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_snippets_provider_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_snippets_provider_finalize(GObject *object)
{
	GscSnippetsProvider *self;
	
	self = GSC_SNIPPETS_PROVIDER(object);
	gdk_pixbuf_unref (self->priv->icon);	
	gsnippets_db_disconnect(self->priv->db);
	g_object_unref(self->priv->db);
	self->priv->view = NULL;
	
	G_OBJECT_CLASS(gsc_snippets_provider_parent_class)->finalize(object);
}


static void gsc_snippets_provider_class_init (GscSnippetsProviderClass * klass)
{
	gsc_snippets_provider_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_snippets_provider_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_snippets_provider_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_snippets_provider_finalize;
	
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


static void gsc_snippets_provider_gtk_source_completion_provider_interface_init (GscProviderIface * iface)
{
	gsc_snippets_provider_gtk_source_completion_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_proposals = gsc_snippets_provider_real_get_data;
	/*iface->data_selected = gsc_snippets_provider_real_data_selected;*/
	iface->get_name = gsc_snippets_provider_real_get_name;
	iface->finish = gsc_snippets_provider_real_end_completion;
	/*iface->get_item_info_markup = gsc_snippets_provider_real_get_item_info_markup;*/
}


static void gsc_snippets_provider_init (GscSnippetsProvider * self)
{
	self->priv = g_new0(GscSnippetsProviderPrivate, 1);
	self->priv->icon = gdk_pixbuf_new_from_file(ICON_FILE,NULL);
	self->priv->parser = NULL;
	self->priv->view = NULL;
	g_assert(self->priv->icon!=NULL);
	self->priv->db = gsnippets_db_new();
	gsnippets_db_connect(self->priv->db);
}

GType gsc_snippets_provider_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscSnippetsProviderClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_snippets_provider_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscSnippetsProvider), 0, (GInstanceInitFunc) gsc_snippets_provider_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscSnippetsProvider", &g_define_type_info, 0);
		static const GInterfaceInfo gtk_source_completion_provider_info = { (GInterfaceInitFunc) gsc_snippets_provider_gtk_source_completion_provider_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_PROVIDER, &gtk_source_completion_provider_info);
	}
	return g_define_type_id;
}


GscSnippetsProvider*
gsc_snippets_provider_new(GtkTextView *view)
{
	GscSnippetsProvider *self = GSC_SNIPPETS_PROVIDER (g_object_new (TYPE_GSC_SNIPPETS_PROVIDER, NULL));
	self->priv->view = view;
	return self;
}

