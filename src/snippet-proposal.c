 /* 
 *
 * Copyright (C) 2008 - perriman
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
 
#include <glib/gprintf.h>
#include <string.h>
#include <gsnippets/gsnippets-db.h>
#include "snippet-proposal.h"

#define PAGE_NAME "Snippets"

G_DEFINE_TYPE(SnippetProposal, snippet_proposal, GSC_TYPE_PROPOSAL);

static gboolean
snippet_proposal_apply(GscProposal* proposal, GtkTextView *view)
{
	SnippetProposal *self = SNIPPET_PROPOSAL(proposal);
	GSnippetsItem* snippet = gsnippets_db_load(
			self->db,
			self->id);
	
	const gchar* content = gsnippets_item_get_content(snippet);
	gsc_replace_actual_word(view,"");
	gboolean parser_active = gtksnippets_inplaceparser_activate (self->parser,
								     content);
	
	g_object_unref(snippet);
	return TRUE;
}

static void snippet_proposal_finalize(GObject *object)
{
	SnippetProposal *self;
	
	self = SNIPPET_PROPOSAL(object);
	
	g_debug("Snippet proposal finalize");
	
	G_OBJECT_CLASS(snippet_proposal_parent_class)->finalize(object);
}


static void snippet_proposal_class_init (SnippetProposalClass * klass)
{
	snippet_proposal_parent_class = g_type_class_peek_parent (klass);
	GscProposalClass *proposal_class = GSC_PROPOSAL_CLASS (klass);
	G_OBJECT_CLASS (klass)->finalize = snippet_proposal_finalize;
	
	proposal_class->apply = snippet_proposal_apply;
}

static void snippet_proposal_init (SnippetProposal * self)
{
	g_debug("Snippet proposal init");
}

gint 
snippet_proposal_get_id(SnippetProposal *self)
{
	return self->id;
}

SnippetProposal*
snippet_proposal_new(const gchar *label,
		   const gchar *info,
		   const GdkPixbuf *icon,
		   gint id,
		   GSnippetsDb *db,
		   GtkSnippetsInPlaceParser *parser)
{
	SnippetProposal *self = SNIPPET_PROPOSAL (g_object_new (TYPE_SNIPPET_PROPOSAL, 
								"label", label,
								"info", info,
								"icon", icon,
								"page-name", PAGE_NAME,
								NULL));
	self->id = id;
	self->db = db;
	self->parser = parser;
	return self;
}

