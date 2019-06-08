/* gnum-app-window.c
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
#include "gnum-app-window.h"
#include "gnum-app.h"

struct _GnumAppWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget *vBox;
  GtkWidget *MenuBar;
  GtkWidget *ScrollWin;
  GtkWidget *StatusBar;
};

G_DEFINE_TYPE (GnumAppWindow, gnum_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
gnum_app_window_class_init (GnumAppWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/Gnum/gnum-app-window.ui");

  gtk_widget_class_bind_template_child (widget_class, GnumAppWindow, vBox);
  gtk_widget_class_bind_template_child (widget_class, GnumAppWindow, MenuBar);
  gtk_widget_class_bind_template_child (widget_class, GnumAppWindow, ScrollWin);
  gtk_widget_class_bind_template_child (widget_class, GnumAppWindow, StatusBar);


  gtk_widget_class_bind_template_callback (widget_class, RowActivated);
}

static void
gnum_app_window_init (GnumAppWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));


  gtk_statusbar_push (GTK_STATUSBAR (self->StatusBar),
                      gtk_statusbar_get_context_id (GTK_STATUSBAR(self->StatusBar),"ContextId"),
                      "0 Connections in list");
}

GnumAppWindow*
gnum_app_window_new (GnumApp *app)
{
	return g_object_new(GNUM_TYPE_APP_WINDOW, "application", app, NULL);
}




void RowActivated(GtkTreeView *treeView, GtkTreePath *path,
                  GtkTreeViewColumn *column, gpointer userData)
{

}
