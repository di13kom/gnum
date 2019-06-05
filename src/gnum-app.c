/* gnum-app.c
 *
 * Copyright 2017 di13kom
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gnum-config.h"
#include "gnum-app.h"
#include "gnum-app-window.h"

struct _GnumApp
{
	GtkApplication	parent_instance;

	GtkWidget		*window;
};

G_DEFINE_TYPE (GnumApp, gnum_app, GTK_TYPE_APPLICATION);

	static void
gnum_app_activate (GApplication *app)
{
	GnumApp *self;

	self = GNUM_APP(app);

	if(!self->window)
	{
		self->window = gnum_app_window_new ();
		gtk_widget_show(self->window);
	}

	gtk_window_present (GTK_WINDOW (self->window));
}

	static void
gnum_app_class_init(GnumAppClass *klass)
{
	//GObjectClass *object_class;
	GApplicationClass *g_application_class;

	//object_class = G_OBJECT_CLASS(klass);

	g_application_class = G_APPLICATION_CLASS(klass);
	g_application_class->activate = gnum_app_activate;
}

	static void
gnum_app_init(GnumApp *self)
{
}

	GnumApp*
gnum_app_new(void)
{
	return g_object_new(gnum_app_window_get_type(),
			"application-id", "org.gnome.gnum",
			"flags", G_APPLICATION_FLAGS_NONE,
			NULL);
}
