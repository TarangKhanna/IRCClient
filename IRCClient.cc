
#include <stdio.h>
#include <gtk/gtk.h>

GtkListStore * list_rooms;
GtkListStore * list_users;
void update_list_rooms() {
    GtkTreeIter iter;
    int i;
    // update 
    /* Add some messages to the window */
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("Room %d", i);
        gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
                      &iter,
                            0, msg,
                      -1);
  g_free (msg);
    }
}

void update_list_users() {
    GtkTreeIter iter;
    int i;
    // update 
    /* Add some messages to the window */
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("User %d", i);
        gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_users), 
                      &iter,
                            0, msg,
                      -1);
  g_free (msg);
    }
}

/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    //GtkListStore *model;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
            GTK_POLICY_AUTOMATIC, 
            GTK_POLICY_AUTOMATIC);
   
    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);
   
    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
                                                       cell,
                                                       "text", 0,
                                                       NULL);
  
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
                 GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static void insert_text( GtkTextBuffer *buffer, const char * initialText )
{
   GtkTextIter iter;
 
   gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
   gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}
   

GdkPixbuf *create_pixbuf(const gchar * filename)
{
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  pixbuf = gdk_pixbuf_new_from_file(filename, &error);
  if (!pixbuf) {
    fprintf(stderr, "%s\n", error->message);
    g_error_free(error);
  }

  return pixbuf;
}

/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText )
{
   GtkWidget *scrolled_window;
   GtkWidget *view;
   GtkTextBuffer *buffer;

   view = gtk_text_view_new ();
   buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                   GTK_POLICY_AUTOMATIC,
           GTK_POLICY_AUTOMATIC);

   gtk_container_add (GTK_CONTAINER (scrolled_window), view);
   insert_text (buffer, initialText);

   gtk_widget_show_all (scrolled_window);

   return scrolled_window;
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *userList;
    GtkWidget *messages;
    GtkWidget *myMessage;
    GdkColor color;
    GdkColor color2;
    GdkColor color3;
    GtkWidget *frame;
    GtkWidget *entry;
    GtkWidget *entryRoom;
    GtkWidget *labelRoom;
    gtk_init (&argc, &argv);
   
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy",
                G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 550, 450);
    gdk_color_parse ("brown", &color2);
    gtk_widget_modify_bg (GTK_WIDGET(window), GTK_STATE_NORMAL, &color2);
    
    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    GtkWidget *table = gtk_table_new (8, 6, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 10);
    gtk_table_set_col_spacings(GTK_TABLE (table), 10);
    gtk_widget_show (table);
   
    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_rooms();
    list = create_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), list, 0, 2, 0, 2);
    gtk_widget_show (list);
    gtk_table_set_homogeneous(GTK_TABLE (table), TRUE);
    

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    list_users = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_users();
    userList = create_list ("Users in room", list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), userList, 2, 4, 0, 2);
    gtk_widget_show (userList);

    messages = create_text ("Peter: Hi how are you\nMary: I am fine, thanks and you?\nPeter: Fine thanks.\n");
    gtk_table_attach_defaults (GTK_TABLE (table), messages, 2, 6, 2, 5);
    gtk_widget_show (messages);
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 

    //myMessage = create_text ("I am fine, thanks and you?\n");
    //gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 2, 6, 5, 7);
    //gtk_widget_show (myMessage);
    
    //create a text box
    entry = gtk_entry_new_with_max_length(20);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), entry, 2, 6, 5, 8);
    gtk_widget_show(entry);

    //create a text Room box
    entryRoom = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), entryRoom, 0, 2, 3, 4);
    gtk_widget_show(entryRoom);

    //Label for room
    labelRoom = gtk_label_new("Enter room name:");
    gtk_misc_set_alignment(GTK_MISC(labelRoom),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelRoom,0, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    //gtk_table_attach_defaults(GTK_TABLE(table), labelRoom, 0, 2, 2, 3);
    gtk_widget_show(labelRoom);
    gdk_color_parse ("black", &color3);
    gtk_widget_modify_bg (GTK_WIDGET(labelRoom), GTK_STATE_NORMAL, &color3);
    
    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 5, 6, 7, 8); 
    gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(send_button), GTK_STATE_NORMAL, &color);
    gtk_widget_show (send_button); 

    GtkWidget *create_account = gtk_button_new_with_label ("Create");
    gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(create_account), GTK_STATE_NORMAL, &color);
    gtk_table_attach_defaults(GTK_TABLE (table), create_account, 0, 1, 7, 8);  // create button
    gtk_widget_show (create_account);

   
    gtk_widget_show (table);
    gtk_widget_show (window);
    
    gtk_window_set_title(GTK_WINDOW(window), "Tarang's IRCClient");

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("chat-icon_small.png"));

    gtk_main ();

    return 0;
}

