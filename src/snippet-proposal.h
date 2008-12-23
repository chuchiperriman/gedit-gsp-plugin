/*
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

#ifndef __SNIPPETS_PROPOSAL_H__
#define __SNIPPETS_PROPOSAL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-proposal.h>
#include <gtksnippets/gtksnippets-inplaceparser.h>

G_BEGIN_DECLS


#define TYPE_SNIPPET_PROPOSAL (snippet_proposal_get_type ())
#define SNIPPET_PROPOSAL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_SNIPPET_PROPOSAL, SnippetProposal))
#define SNIPPET_PROPOSAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_SNIPPET_PROPOSAL, SnippetProposalClass))
#define IS_SNIPPET_PROPOSAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_SNIPPET_PROPOSAL))
#define IS_SNIPPET_PROPOSAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_SNIPPET_PROPOSAL))
#define SNIPPET_PROPOSAL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_SNIPPET_PROPOSAL, SnippetProposalClass))

#define SNIPPET_PROPOSAL_NAME "GscSnippets"

typedef struct _SnippetProposal SnippetProposal;
typedef struct _SnippetProposalClass SnippetProposalClass;

struct _SnippetProposal {
	GscProposal parent;
	gint id;
	GSnippetsDb *db;
	GtkSnippetsInPlaceParser *parser;
};

struct _SnippetProposalClass {
	GscProposalClass parent;
};

GType snippet_proposal_get_type ();

gint 
snippet_proposal_get_id(SnippetProposal *self);

SnippetProposal*
snippet_proposal_new(const gchar *label,
		     const gchar *info,
		     const GdkPixbuf *icon,
		     gint id,
		     GSnippetsDb *db,
		     GtkSnippetsInPlaceParser *parser);

G_END_DECLS

#endif
