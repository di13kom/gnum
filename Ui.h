#ifndef Ui_H
#define Ui_h
#include <gtk/gtk.h>
#include <glib/gprintf.h>

enum
{
	PICTURE = 0,
	LIST_ITEM,
	IP_ADDRESS_ITEM,
	N_COLUMNS
};

typedef struct
{
	gchar* AddressString;
	gchar* UserName;
	GSocket* Socket;
	GtkWidget* Window;
	GtkWidget* InputView;
	GtkWidget* OutputView;
	GtkTreeRowReference* Row;
	GSource* Source;
}ConnectionStructure;

GList* ConnectionData;

GtkWidget* TreeView;
GtkWidget* StatusBar;

gint CheckWindowsList(gconstpointer, gconstpointer);
gboolean onEnter(GtkWidget*, GdkEventKey*, gpointer);
void SelectedCallback(GtkMenuItem*, gpointer);
void AboutCallback(GtkMenuItem*, gpointer);
GtkWidget* ScrollWindow(GtkWidget*);
void AcitvatedRow(GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*, gpointer);
void ViewModelInit(GtkWidget*);
void AddEntry(GtkWidget*, ConnectionStructure*);
void CreateDialogWindow(ConnectionStructure*);
void SetupMenu(GtkWidget*, GtkWidget*);
void application_start_point(GtkApplication*, gpointer);
gboolean DialogCreation(ConnectionStructure*);
void ConversationMenuSetup(GtkWidget*, GtkWidget*);
void StatusBarCorrection();


void HandleInputMessage(ConnectionStructure* Data, gchar* Buffer);
void DialogRemove(ConnectionStructure*);
void RemoveTreeViewRow(GtkTreeRowReference*);
void InsertTextToBufferWithDate(GtkWidget*, gchar*, gchar*);
void MoveTextFromInputBufferToOutput(GtkWidget*, gpointer);


void HandleMenuSendMessageItem(GtkMenuItem*, gpointer);
void HandleMenuSendFileItem(GtkMenuItem*, gpointer);
void AcceptFileDialogCAllback(GtkDialog* , gint , gpointer);
#endif
