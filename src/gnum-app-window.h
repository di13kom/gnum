/* gnum-app-window.h
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

#ifndef _GNUM_APPWINDOW_H
#define _GNUM_APPWINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GNUM_TYPE_APP_WINDOW (gnum_app_window_get_type())

G_DECLARE_FINAL_TYPE (GnumAppWindow, gnum_app_window, GNUM, APP_WINDOW, GtkApplicationWindow)

GnumAppWindow* gnum_app_window_new(void);

G_END_DECLS
#endif /*_GNUM_APPWINDOW_H */
