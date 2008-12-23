/*
 * gsc-snippets-provider.h - Type here a short description of your plugin
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

#ifndef __SNIPPETS_PROVIDER_H__
#define __SNIPPETS_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-provider.h>

G_BEGIN_DECLS


#define TYPE_GSC_SNIPPETS_PROVIDER (gsc_snippets_provider_get_type ())
#define GSC_SNIPPETS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_SNIPPETS_PROVIDER, GscSnippetsProvider))
#define GSC_SNIPPETS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_SNIPPETS_PROVIDER, GscSnippetsProviderClass))
#define IS_GSC_SNIPPETS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_SNIPPETS_PROVIDER))
#define IS_GSC_SNIPPETS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_SNIPPETS_PROVIDER))
#define GSC_SNIPPETS_PROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_SNIPPETS_PROVIDER, GscSnippetsProviderClass))

#define GSC_SNIPPETS_PROVIDER_NAME "GscSnippets"

typedef struct _GscSnippetsProvider GscSnippetsProvider;
typedef struct _GscSnippetsProviderClass GscSnippetsProviderClass;
typedef struct _GscSnippetsProviderPrivate GscSnippetsProviderPrivate;

struct _GscSnippetsProvider {
	GObject parent;
	GscSnippetsProviderPrivate *priv;	
};

struct _GscSnippetsProviderClass {
	GObjectClass parent;
};

GType gsc_snippets_provider_get_type ();

GscSnippetsProvider* 
gsc_snippets_provider_new(GtkTextView *view);

G_END_DECLS

#endif
