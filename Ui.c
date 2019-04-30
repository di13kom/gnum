#include "Ui.h"
#include "net.h"

gint CheckWindowsList(gconstpointer List, gconstpointer Str)
{
	const gchar *StrAdd = (gchar*)Str;
	gchar *Add = (gchar*)(((ConnectionStructure*)List)->AddressString);

	return g_strcmp0(StrAdd, Add);
}

gboolean onDestroyWindow(GtkWidget* Win, GdkEventKey* event, gpointer userdata)
{
	gtk_widget_hide_on_delete(GTK_WIDGET(Win));
	//gtk_widget_hide_on_delete(GTK_WIDGET(Win));
	return TRUE;
}

void MoveTextFromInputBufferToOutput(GtkWidget* InView, gpointer userdata)
{
	GtkTextIter InStartIter,InEndIter;
	GtkTextBuffer* InBuffer;
	gchar *Text;
	GSocket* Sock;
	ConnectionStructure* StructItem = NULL;

	GtkDialogFlags DialogFlag;
	GtkWidget* Dialog;

	//g_printf("Enter pressed\n");
	InBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(InView));
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(InBuffer), &InStartIter, &InEndIter);
	Text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(InBuffer), &InStartIter, &InEndIter, TRUE);

	StructItem = g_object_get_data(G_OBJECT(InView), "List");

	if(strlen(Text) > ((BUFFERSIZE - 1) - HEADERLEN - HEADERLEN))
	{

		DialogFlag = GTK_DIALOG_DESTROY_WITH_PARENT;
		Dialog = gtk_message_dialog_new(GTK_WINDOW(StructItem->Window), DialogFlag, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "String to long");
		gtk_dialog_run(GTK_DIALOG(Dialog));
		gtk_widget_destroy(Dialog);
		g_free(Text);
		return;
	}
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(InBuffer), "", -1);
	InsertTextToBufferWithDate(GTK_WIDGET(userdata), Text, "BlueColored");

	//send
	if(StructItem != NULL)
	{
		Sock = StructItem->Socket;
		if(g_socket_condition_wait(Sock, G_IO_OUT, NULL, NULL) == TRUE)
			net_SendMessage(Sock, Text);
		g_printf("message to %s@%s\n", StructItem->UserName, StructItem->AddressString);
	}
	//
	g_free(Text);
}

void AcceptFileDialogCAllback(GtkDialog* Dialog, gint response_id, gpointer userdata)
{
	if(response_id == GTK_RESPONSE_CANCEL)
	{
		g_printf("file %s rejected for recieving\n", (gchar*)userdata);
		//gtk_widget_destroy(GTK_WIDGET(Dialog));
	}
	else if(response_id == GTK_RESPONSE_OK)
		g_printf("file %s accepted for recieving\n", (gchar*)userdata);
}

gboolean onEnter(GtkWidget* InView, GdkEventKey* event, gpointer userdata)
{
	//g_printf("%#08x\n", event->keyval);
	if(event->keyval == GDK_KEY_F5)
	{
		MoveTextFromInputBufferToOutput(InView, userdata);
		return TRUE;
	}
	return FALSE;
}

void SelectedCallback(GtkMenuItem* Item, gpointer userdata)
{
	//g_printf("menu item selected\n");
	gtk_widget_destroy(GTK_WIDGET(userdata));
}

void AboutCallback(GtkMenuItem* Item, gpointer userdata)
{
	//g_printf("about menu chosed\n");
	GtkWidget* about_dialog;

	const gchar* author[] = {"Komarov Dmitry", NULL};

	about_dialog = gtk_about_dialog_new();

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Lan Messenger");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), author);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "Beta version");

	gtk_window_set_title(GTK_WINDOW(about_dialog), "About");
	gtk_window_set_transient_for(GTK_WINDOW(about_dialog), GTK_WINDOW(userdata));

	g_signal_connect(GTK_DIALOG(about_dialog), "response", G_CALLBACK(gtk_widget_destroy), NULL);

	gtk_widget_show(GTK_WIDGET(about_dialog));

}
GtkWidget* ScrollWindow(GtkWidget* Wdg)
{
	GtkWidget* ScrollWin;
	ScrollWin = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrollWin), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(ScrollWin), 
			GTK_SHADOW_ETCHED_IN);
	gtk_container_add(GTK_CONTAINER(ScrollWin), Wdg);
	gtk_container_set_border_width(GTK_CONTAINER(ScrollWin), 5);

	return ScrollWin;
}

void AcitvatedRow(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer userdata)
{
	GList* _Win;

	GtkWidget* Win;
	GtkTreeIter iter;
	gchar* name;
	gchar* Addr;
	//gchar *Title;
	GtkTreeModel *model;


	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	if(gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path) == FALSE)
	{
#ifdef DEBUG
	g_printf("no valid iter\n");
#endif
		return;
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, LIST_ITEM, &name, IP_ADDRESS_ITEM, &Addr, -1);

	_Win = g_list_find_custom(ConnectionData, Addr, CheckWindowsList);
	if(_Win != NULL)
	{
		Win = ((ConnectionStructure*)_Win->data)->Window;
		gtk_window_present(GTK_WINDOW(Win));
	}


	g_free(Addr);
	g_free(name);
	//g_free(Title);
}

void ViewModelInit(GtkWidget *list)
{
	GtkCellRenderer* renderer;

	GtkTreeStore* store;

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
			-1,
			"",
			renderer,
			"pixbuf", PICTURE,
			NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
			-1,
			"Name",
			renderer,
			"text", LIST_ITEM,
			NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list),
			-1,
			"Ip-Address",
			renderer,
			"text", IP_ADDRESS_ITEM,
			NULL);

	store = gtk_tree_store_new(N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

	g_object_unref(store);
}

void AddEntry(GtkWidget* list, ConnectionStructure* ConStr)
{
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeRowReference* ref;

	GdkPixbuf* Pic;
	GError* Error = NULL;

	//Pic = gdk_pixbuf_new_from_file("available.png", &Error);
	if((Pic = gdk_pixbuf_new_from_file("available.png", &Error)) == NULL)
	if(Error)
	{
		g_printf("Error on Pic: %s\n", Error->message);
	}

	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, PICTURE, Pic, LIST_ITEM, ConStr->UserName, IP_ADDRESS_ITEM, ConStr->AddressString, -1);

	path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
	ref = gtk_tree_row_reference_new(GTK_TREE_MODEL(store), path);
	ConStr->Row = ref;

	g_object_unref(Pic);
	gtk_tree_path_free(path);
}

void SetupMenu(GtkWidget* Mbar, GtkWidget* Window)
{
	GtkWidget* Menu;
	GtkWidget* Menu1;
	GtkWidget* Mitem1;
	GtkWidget* Mitem2;
	GtkWidget* SubMitem1;
	GtkWidget* QuitItem;
	GtkWidget* AboutItem;

	Menu = gtk_menu_new();
	Menu1 = gtk_menu_new();
	Mitem1 = gtk_menu_item_new_with_mnemonic("_File");
	Mitem2 = gtk_menu_item_new_with_mnemonic("_Help");


	SubMitem1 = gtk_menu_item_new_with_label("New");
	QuitItem = gtk_menu_item_new_with_label("Quit");
	AboutItem = gtk_menu_item_new_with_label("About");

	gtk_menu_shell_append(GTK_MENU_SHELL(Menu), SubMitem1);
	gtk_menu_shell_append(GTK_MENU_SHELL(Menu), QuitItem);

	gtk_menu_shell_append(GTK_MENU_SHELL(Menu1), AboutItem);

	//gtk_menu_attach(GTK_MENU(Menu), Mitem1, 1, 1, 1, 1);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(Mitem1), Menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(Mitem2), Menu1);

	gtk_menu_shell_append(GTK_MENU_SHELL(Mbar), Mitem1);
	gtk_menu_shell_append(GTK_MENU_SHELL(Mbar), Mitem2);

	//
	g_signal_connect(QuitItem, "activate", G_CALLBACK(SelectedCallback), Window);
	g_signal_connect(AboutItem, "activate", G_CALLBACK(AboutCallback), Window);
}

void application_start_point(GtkApplication *App, gpointer userdata)
{
	GtkWidget *Win;
	GtkWidget *ScrollWin;
	//GtkWidget* TreeView;
	GtkWidget* vBox;
	//GtkWidget* StatusBar;
	GtkWidget* MenuBar;

	Win = gtk_application_window_new(App);
	gtk_window_set_title(GTK_WINDOW(Win), "Application");

	gtk_widget_set_size_request(GTK_WIDGET(Win), 250, 500); //disable shrink
	gtk_window_set_resizable(GTK_WINDOW(Win), FALSE);
	//gtk_window_set_default_size(GTK_WINDOW(Win), 200, 500);
	gtk_container_set_border_width(GTK_CONTAINER(Win), 5);

	//set container
	vBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	TreeView = gtk_tree_view_new();
	ViewModelInit(TreeView);

	ScrollWin = ScrollWindow(TreeView);

	MenuBar = gtk_menu_bar_new();
	SetupMenu(MenuBar, Win);
	//status bar and menu
	StatusBar = gtk_statusbar_new();
	//
	//status bar message
	gtk_statusbar_push(GTK_STATUSBAR(StatusBar),
			gtk_statusbar_get_context_id(GTK_STATUSBAR(StatusBar), "ContextId"),
			"0 Connections in list");

	//pack widgets
	gtk_box_pack_start(GTK_BOX(vBox), MenuBar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vBox), ScrollWin, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(vBox), StatusBar, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(Win), vBox);

	g_signal_connect(TreeView, "row-activated", G_CALLBACK(AcitvatedRow), NULL);

	gtk_widget_show_all(Win);

	net_StartPoint();
}

int main(int argc, char** argv)
{
	ConnectionData = NULL;
	GtkApplication* App;
	int status;
	App = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(App, "activate", G_CALLBACK(application_start_point), NULL);
	status = g_application_run(G_APPLICATION(App), argc, argv);

	g_object_unref(App);

	return status;
}

void CreateDialogWindow(ConnectionStructure* ConStr)
{
	GtkWidget* Win;

	GtkWidget* MenuBar;
	GtkWidget* vBox;
	GtkWidget* TxtView;
	GtkWidget* InTxtView;
	GtkTextBuffer* Buffer;
	GtkTextBuffer* InBuffer;
	GtkWidget* Frame;
	GtkWidget* ScrollWin;
	GtkWidget* ScrollWin2;
	gchar* Title;

	Title = g_strdup_printf("%s@%s", ConStr->UserName, ConStr->AddressString);
	Win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(Win), Title);

	//setup Menu
	MenuBar = gtk_menu_bar_new();
	ConversationMenuSetup(MenuBar, Win);

	//gtk_window_set_resizable(GTK_WINDOW(Win), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(Win), 200, 400);
	gtk_container_set_border_width(GTK_CONTAINER(Win), 5);
	g_signal_connect(Win, "delete-event",G_CALLBACK(onDestroyWindow), NULL);

	//box pack
	vBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	TxtView = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(TxtView), FALSE);
	Buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(TxtView));
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(Buffer), "BlueColored", "foreground", "blue", NULL);
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(Buffer), "RedColored", "foreground", "red", NULL);
	Frame = gtk_frame_new(NULL);
	InTxtView = gtk_text_view_new();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (InTxtView), GTK_WRAP_WORD_CHAR);
	InBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(InTxtView));
	g_signal_connect(InTxtView, "key-press-event", G_CALLBACK(onEnter), TxtView);
	//
	ScrollWin = ScrollWindow(InTxtView);
	ScrollWin2 = ScrollWindow(TxtView);

	gtk_box_pack_start(GTK_BOX(vBox), MenuBar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vBox), ScrollWin2, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(vBox), Frame, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(vBox), ScrollWin, FALSE, TRUE, 2);

	gtk_widget_grab_focus(InTxtView);
	gtk_container_add(GTK_CONTAINER(Win), vBox);
	gtk_widget_show_all(Win);
	gtk_widget_hide(Win);

	//
	ConStr->Window = Win;
	ConStr->OutputView = TxtView;
	ConStr->InputView = InTxtView;
	//g_printf("attached object address before: %p\n",ConStr);
	g_object_set_data(G_OBJECT(Win), "List", (gpointer) ConStr);
	g_object_set_data(G_OBJECT(TxtView), "List", (gpointer) ConStr);
	g_object_set_data(G_OBJECT(InTxtView), "List", (gpointer) ConStr);
	//g_printf("attached object address after: %p\n",g_object_get_data(G_OBJECT(Win), "List"));
}

gboolean DialogCreation(ConnectionStructure* Data)
{
	CreateDialogWindow(Data);
	AddEntry(TreeView, Data);
	StatusBarCorrection();
	return FALSE;
}

void StatusBarCorrection()
{
	gchar* Mess;

	Mess = g_strdup_printf("%d Connections in List", g_list_length(ConnectionData));

	gtk_statusbar_pop(GTK_STATUSBAR(StatusBar), gtk_statusbar_get_context_id(GTK_STATUSBAR(StatusBar), "ContextId"));
	gtk_statusbar_push(GTK_STATUSBAR(StatusBar),
			gtk_statusbar_get_context_id(GTK_STATUSBAR(StatusBar), "ContextId"),
			Mess);
	g_free(Mess);
}
void DialogRemove(ConnectionStructure* Data)
{
	GList* List;
	gboolean isDestroyed;
	gchar* StringToDel;

	gtk_widget_destroy(Data->Window);
	isDestroyed = gtk_widget_in_destruction(Data->Window);
	if(isDestroyed == TRUE)
		g_print("Window deleted\n");

	RemoveTreeViewRow(Data->Row);
	g_free(Data->UserName);
	if((List = g_list_find_custom(ListOfUsedAddresses,
				Data->AddressString,
				net_CompareFunc)) != NULL)
	{
		StringToDel = List->data;
		g_mutex_lock(&SharedMutex);
#ifdef DEBUG
		g_printf("element before deleting: %d\n",g_list_length(ListOfUsedAddresses));
		g_printf("mutex got\n");
#endif
		ListOfUsedAddresses = g_list_remove(ListOfUsedAddresses, List->data);
		g_free(StringToDel);
#ifdef DEBUG
		g_printf("element after deleting: %d\n",g_list_length(ListOfUsedAddresses));
		g_printf("release mutex\n");
#endif
		g_mutex_unlock(&SharedMutex);
		//g_free(Data->Address); on stack
	}
	//remove sturct
	ConnectionData = g_list_remove(ConnectionData, Data);
	StatusBarCorrection();
	g_free(Data);
}
void RemoveTreeViewRow(GtkTreeRowReference* ref)
{
	GtkTreeModel* model;
	GtkTreePath* path;
	GtkTreeIter iter;

	model = gtk_tree_row_reference_get_model(ref);
	path = gtk_tree_row_reference_get_path(ref);

	if((gtk_tree_model_get_iter(model, &iter, path)) == FALSE)
	{
#ifdef DEBUG
	g_printf("no valid iter\n");
#endif
		return;
	}
	gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
}

void HandleInputMessage(ConnectionStructure* Data, gchar* Buffer)
{
	GtkWidget* Dialog;

	if(g_str_has_prefix(Buffer, FILE_REQUEST_START) && g_str_has_suffix(Buffer, FILE_REQUEST_END))
	{
		Dialog = gtk_message_dialog_new(GTK_WINDOW(((ConnectionStructure*)Data)->Window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, "Do you want to a %s file", Buffer);
		//gtk_dialog_add_button(GTK_DIALOG(Dialog), "Cancel", 151515);
		g_signal_connect(GTK_DIALOG(Dialog), "response", G_CALLBACK(AcceptFileDialogCAllback), Buffer);
		gtk_dialog_run(GTK_DIALOG(Dialog));
		gtk_widget_destroy(Dialog);
		return;
	}
	InsertTextToBufferWithDate(((ConnectionStructure*)Data)->OutputView, Buffer, "RedColored");

	gtk_window_present(GTK_WINDOW(((ConnectionStructure*)Data)->Window));
}

void InsertTextToBufferWithDate(GtkWidget* OutTextView, gchar* Text, gchar* TagName)
{
	GtkTextBuffer* InBuffer;
	GtkTextIter OutEndIter;
	gchar NewLineText[3] = "\n";

	gchar *DateVal;
	GDateTime *DT;

	/*
	const gchar* ValidationCursor;
	gchar* TmpCursor;

	g_printf("text to send is: %s\n", Buffer);

	g_printf("text to send before validate: %s\n", Buffer);
	if(g_utf8_validate(Buffer, -1, &ValidationCursor) == FALSE)
	{
		g_printf("not valid text\n");
		TmpCursor = ValidationCursor;
		*TmpCursor= '\0';
	}
	g_printf("text to send after validate: %s\n", Buffer);
	*/

	InBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(OutTextView));
	DT = g_date_time_new_now_local();
	DateVal = g_strdup(g_date_time_format(DT, "%H:%M:%S "));

	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(InBuffer), &OutEndIter);
	gtk_text_buffer_insert_with_tags_by_name(GTK_TEXT_BUFFER(InBuffer)
			, &OutEndIter, DateVal,-1, TagName, NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(InBuffer)
			, &OutEndIter);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(InBuffer)
			, &OutEndIter, Text, -1);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(InBuffer)
			, &OutEndIter);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(InBuffer)
			, &OutEndIter, NewLineText, -1);

	g_free(DateVal);
	g_date_time_unref(DT);
}

void ConversationMenuSetup(GtkWidget* Mbar, GtkWidget* Window)
{
	GtkWidget* ConversationMenu;
	GtkWidget* Mitem;
	GtkWidget* SendMessageItem;
	GtkWidget* SendFileItem;

	ConversationMenu = gtk_menu_new();
	Mitem = gtk_menu_item_new_with_mnemonic("Conversation");

	SendMessageItem = gtk_menu_item_new_with_label("Send Message	F5");
	SendFileItem = gtk_menu_item_new_with_label("Send File	F6");

	gtk_menu_shell_append(GTK_MENU_SHELL(ConversationMenu), SendMessageItem);
	gtk_menu_shell_append(GTK_MENU_SHELL(ConversationMenu), SendFileItem);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(Mitem), ConversationMenu);

	gtk_menu_shell_append(GTK_MENU_SHELL(Mbar), Mitem);

	//
	g_signal_connect(SendMessageItem, "activate", G_CALLBACK(HandleMenuSendMessageItem), Window);
	g_signal_connect(SendFileItem, "activate", G_CALLBACK(HandleMenuSendFileItem), Window);
}

void HandleMenuSendMessageItem(GtkMenuItem* Item, gpointer WindowData)
{
	//GList* ListItem;
	GtkWidget* InView;
	GtkWidget* OutView;
	ConnectionStructure* StructItem = NULL;

	StructItem = (ConnectionStructure*)g_object_get_data(G_OBJECT(WindowData), "List");
	if(StructItem != NULL)
	{
		InView = StructItem->InputView;
		OutView = StructItem->OutputView;
		//
		MoveTextFromInputBufferToOutput(InView, (gpointer)OutView);

	}
}

void HandleMenuSendFileItem(GtkMenuItem* Item, gpointer WindowData)
{
	GtkWidget* Dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	guint res;
	ConnectionStructure* StructItem = NULL;

	Dialog = gtk_file_chooser_dialog_new("Choose file to send",
			GTK_WINDOW(WindowData), action, ("Ok"), GTK_RESPONSE_OK,("Cancel"), GTK_RESPONSE_CANCEL, NULL);
	res = gtk_dialog_run(GTK_DIALOG(Dialog));

	if(res == GTK_RESPONSE_OK)
	{
		GFile* File;
		gchar* filename;
		gchar* FullFileName;
		gchar* StringToSend;
		GtkFileChooser* chooser = GTK_FILE_CHOOSER(Dialog);
		FullFileName = gtk_file_chooser_get_filename(chooser);
		File = gtk_file_chooser_get_file(chooser);
		filename = g_file_get_basename(File);
		g_printf("choosed filename is: %s\n", filename);


		StringToSend = g_strconcat(FILE_REQUEST_START, filename, FILE_REQUEST_END, NULL);

		g_free(filename);
		g_free(FullFileName);
		gtk_widget_destroy(GTK_WIDGET(Dialog));

		//CopyFile(File, userdata);
		g_object_unref(File);

		StructItem = (ConnectionStructure*)g_object_get_data(G_OBJECT(WindowData), "List");
		net_SendMessage(StructItem->Socket, StringToSend);
		g_free(StringToSend);
	}
	if(res == GTK_RESPONSE_CANCEL)
		gtk_widget_destroy(GTK_WIDGET(Dialog));
}
