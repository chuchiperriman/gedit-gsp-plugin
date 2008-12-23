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
#include "snippet-proposal.h"

G_DEFINE_TYPE(SnippetProposal, snippet_proposal, GSC_TYPE_PROPOSAL);

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
	G_OBJECT_CLASS (klass)->finalize = snippet_proposal_finalize;
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
		   gint id)
{
	SnippetProposal *self = SNIPPET_PROPOSAL (g_object_new (TYPE_SNIPPET_PROPOSAL, 
								"label", label,
								"info", info,
								"icon", icon,
								NULL));
	self->id = id;
	return self;
}

