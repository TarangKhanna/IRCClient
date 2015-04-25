
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

GtkListStore * list_rooms;
GtkListStore * list_users;



gboolean resize_image(GtkWidget *widget, GdkEvent *event, GtkWidget *window)
{
  GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
  if (pixbuf == NULL)
  {
    g_printerr("Failed to resize image\n");
    return 1;
  }
  
  printf("Width: %i\nHeight%i\n", widget->allocation.width, widget->allocation.height);
  
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, widget->allocation.width, widget->allocation.height, GDK_INTERP_BILINEAR);
  
  gtk_image_set_from_pixbuf(GTK_IMAGE(widget), pixbuf);
  
  return FALSE;
}

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

static void entry_toggle_visibility( GtkWidget *checkbutton,
                                     GtkWidget *entry )
{
  gtk_entry_set_visibility (GTK_ENTRY (entry),
          GTK_TOGGLE_BUTTON (checkbutton)->active);
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
    GtkWidget *userName;
    GtkWidget *passWord;
    GtkWidget *labelRoom;
    GtkWidget *labelUser;
    GtkWidget *labelPass;
    GtkWidget *labelMsg;

    gtk_init (&argc, &argv);
   
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy",
                G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 650, 550);
    gdk_color_parse ("brown", &color2);
    gtk_widget_modify_bg (GTK_WIDGET(window), GTK_STATE_NORMAL, &color2);
    
    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    GtkWidget *table = gtk_table_new (14, 12, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 10);
    gtk_table_set_col_spacings(GTK_TABLE (table), 10);
    gtk_widget_show (table);
   
    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_rooms();
    list = create_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), list, 0, 4, 0, 5);
    gtk_widget_show (list);
    gtk_table_set_homogeneous(GTK_TABLE (table), TRUE);
    
    labelMsg = gtk_label_new("Messages:");
    gtk_misc_set_alignment(GTK_MISC(labelMsg),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelMsg,4, 8, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    //gtk_widget_modify_bg (GTK_WIDGET(labelMsg), GTK_STATE_NORMAL, &color2);
    gtk_widget_show(labelMsg);

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    list_users = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_users();
    userList = create_list ("Users in room", list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), userList, 4, 8, 0, 4);
    gtk_widget_show (userList);

    messages = create_text ("Peter: Hi how are you\nMary: I am fine, thanks and you?\nPeter: Fine thanks.\n");
    gtk_table_attach_defaults (GTK_TABLE (table), messages, 2, 10, 5, 11);
    gtk_widget_show (messages);
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 

    myMessage = create_text ("I am fine, thanks and you?\n");
    gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 2, 10, 11, 13);
    gtk_widget_show (myMessage);
    
    //create a text box
    //entry = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    //gtk_table_attach_defaults (GTK_TABLE (table), entry, 2, 6, 5, 8);
    //gtk_widget_show(entry);

    //create a text Room box
    entryRoom = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), entryRoom, 0, 2, 6, 7);
    gtk_widget_show(entryRoom);

    //Label for room
    labelRoom = gtk_label_new("Enter room name:");
    gtk_misc_set_alignment(GTK_MISC(labelRoom),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelRoom,0, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    //gtk_table_attach_defaults(GTK_TABLE(table), labelRoom, 0, 2, 2, 3);
    gtk_widget_show(labelRoom);
    gdk_color_parse ("black", &color3);
    gtk_widget_modify_bg (GTK_WIDGET(labelRoom), GTK_STATE_NORMAL, &color3);
    
    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 2, 4, 13, 14); 
    gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(send_button), GTK_STATE_NORMAL, &color);
    gtk_widget_show (send_button); 

    //GtkWidget *create_account = gtk_button_new_with_label ("Create");
    //gdk_color_parse ("orange", &color);
    //gtk_widget_modify_bg (GTK_WIDGET(create_account), GTK_STATE_NORMAL, &color);
    //gtk_table_attach_defaults(GTK_TABLE (table), create_account, 0, 1, 8, 9);  // create button
    //gtk_widget_show (create_account);

    GtkWidget *create_room = gtk_button_new_with_label ("Create Room");
    gtk_table_attach_defaults(GTK_TABLE (table), create_room, 0, 2, 7, 8); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(create_room), GTK_STATE_NORMAL, &color);
    gtk_widget_show (create_room); 

    GtkWidget *enter_room = gtk_button_new_with_label ("Enter Room");
    gtk_table_attach_defaults(GTK_TABLE (table), enter_room, 0, 2, 8, 9); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(enter_room), GTK_STATE_NORMAL, &color);
    gtk_widget_show (enter_room); 

    GtkWidget *leave_room = gtk_button_new_with_label ("Leave Room");
    gtk_table_attach_defaults(GTK_TABLE (table), leave_room, 0, 2, 9, 10); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(leave_room), GTK_STATE_NORMAL, &color);
    gtk_widget_show (leave_room); 

    // login-if user exists - dont add- if user doesnt exist
    // dont add just login - no signup button - 1 button, two text entry
    GtkWidget *login_button = gtk_button_new_with_label ("Log In");
    gtk_table_attach_defaults(GTK_TABLE (table), login_button, 10, 12, 12, 13); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(login_button), GTK_STATE_NORMAL, &color);
    gtk_widget_show (login_button);

    GtkWidget *signup_button = gtk_button_new_with_label ("Signup");
    gtk_table_attach_defaults(GTK_TABLE (table), signup_button, 10, 12, 13, 14); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(signup_button), GTK_STATE_NORMAL, &color);
    gtk_widget_show (signup_button);  

    entryRoom = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), entryRoom, 0, 2, 3, 4);
    gtk_widget_show(entryRoom);

    labelRoom = gtk_label_new("Enter User Name:");
    gtk_misc_set_alignment(GTK_MISC(labelRoom),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelRoom,9, 11, 5, 7, GTK_FILL, GTK_FILL, 0, 0);

    userName = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), userName, 10, 12, 7, 9);
    gtk_widget_show(userName);

    passWord = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), passWord, 10, 12, 9, 11);
    gtk_widget_show(passWord);
    
    // radio visibility

    GtkWidget * check = gtk_check_button_new_with_label ("Editable");
    g_signal_connect (check, "toggled",
                G_CALLBACK (entry_toggle_visibility), passWord);

    GtkWidget *image = gtk_image_new_from_file("new-user-image-default.png");
    gtk_table_attach_defaults(GTK_TABLE (table), image, 10, 12, 0, 5); 
    gtk_widget_show (image);
    
   // g_signal_connect(image, "expose-event", G_CALLBACK(resize_image), (gpointer)window);

    gtk_widget_show (table);
    gtk_widget_show (window);
    gtk_widget_hide (window);
    gtk_widget_show (window);
    gtk_window_set_title(GTK_WINDOW(window), "Tarang's IRCClient");

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("chat_pic.png"));

    gtk_main ();

    return 0;
}

