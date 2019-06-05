/* gnum-dialog-win.c
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
#include "gnum-dialog-win.h"

struct _GnumDialogWin
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget *vBox;
  GtkWidget *OutTxtView;
  GtkWidget *Frame;
  GtkWidget *InTxtView;
};

G_DEFINE_TYPE (GnumDialogWin, gnum_dialog_win, GTK_TYPE_WINDOW)


static void
gnum_dialog_win_class_init (GnumDialogWinClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/Gnum/gnum-dialog-win.ui");

  gtk_widget_class_bind_template_child (widget_class, GnumDialogWin, vBox);
  gtk_widget_class_bind_template_child (widget_class, GnumDialogWin, OutTxtView);
  gtk_widget_class_bind_template_child (widget_class, GnumDialogWin, Frame);
  gtk_widget_class_bind_template_child (widget_class, GnumDialogWin, InTxtView);


}

static void
gnum_dialog_win_init (GnumDialogWin *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GnumDialogWin*
gnum_app_window_new (void )
{
	return g_object_new(GNUM_TYPE_DIALOG_WIN, NULL);
}
