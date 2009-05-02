/* 
 *  gsp-provider-snippets.h - Type here a short description of your plugin
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
#ifndef __PROVIDER_SNIPPETS_H__
#define __PROVIDER_SNIPPETS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

#define GSP_TYPE_PROVIDER_SNIPPETS (gsp_provider_snippets_get_type ())
#define GSP_PROVIDER_SNIPPETS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSP_TYPE_PROVIDER_SNIPPETS, GspProviderSnippets))
#define GSP_PROVIDER_SNIPPETS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSP_TYPE_PROVIDER_SNIPPETS, GspProviderSnippetsClass))
#define GSP_IS_PROVIDER_SNIPPETS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSP_TYPE_PROVIDER_SNIPPETS))
#define GSP_IS_PROVIDER_SNIPPETS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSP_TYPE_PROVIDER_SNIPPETS))
#define GSP_PROVIDER_SNIPPETS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSP_TYPE_PROVIDER_SNIPPETS, GspProviderSnippetsClass))

#define GSP_PROVIDER_SNIPPETS_NAME "GspProviderSnippets"

typedef struct _GspProviderSnippets GspProviderSnippets;
typedef struct _GspProviderSnippetsPrivate GspProviderSnippetsPrivate;
typedef struct _GspProviderSnippetsClass GspProviderSnippetsClass;

struct _GspProviderSnippets
{
	GObject parent;
	
	GspProviderSnippetsPrivate *priv;
};

struct _GspProviderSnippetsClass
{
	GObjectClass parent;
};

GType		 gsp_provider_snippets_get_type	(void) G_GNUC_CONST;

GspProviderSnippets *gsp_provider_snippets_new (GtkTextView *view);

G_END_DECLS

#endif
