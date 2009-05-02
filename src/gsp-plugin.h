/*
 * gsp-plugin.h - Adds (auto)completion support to gedit
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

#ifndef __GSP_PLUGIN_H__
#define __GSP_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gedit/gedit-plugin.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GSP_TYPE_PLUGIN		(gsp_plugin_get_type ())
#define GSP_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GSP_TYPE_PLUGIN, GspPlugin))
#define GSP_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GSP_TYPE_PLUGIN, GspPluginClass))
#define IS_GSP_PLUGIN(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GSP_TYPE_PLUGIN))
#define IS_GSP_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GSP_TYPE_PLUGIN))
#define GSP_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GSP_TYPE_PLUGIN, GspPluginClass))

/* Private structure type */
typedef struct _GspPluginPrivate	GspPluginPrivate;

/*
 * Main object structure
 */
typedef struct _GspPlugin		GspPlugin;

struct _GspPlugin
{
	GeditPlugin parent_instance;

	/*< private >*/
	GspPluginPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GspPluginClass	GspPluginClass;

struct _GspPluginClass
{
	GeditPluginClass parent_class;
};

/*
 * Public methods
 */
GType	gsp_plugin_get_type	(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gedit_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GSP_PLUGIN_H__ */
