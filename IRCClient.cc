
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <algorithm>
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

//#include <curses.h>

char * list_room();
void update_list_rooms();
char * print_users_in_room();
static void insert_text( GtkTextBuffer *buffer, const char * initialText);
int times = 0;
bool changed = FALSE;

using namespace std;

GtkListStore * list_rooms;
GtkWidget *tree_view;
GtkListStore * list_users;
GtkWidget *view;
GtkWidget *viewUser;
GtkWidget *userName; //entry
GtkWidget *passWord; //entry
GtkWidget *currentStatus; //label
GtkWidget *entryRoom; //entry
vector<string> roomVec;
vector<string> roomVecNew;
GtkWidget *roomUser;

char * host = "localhost";
char * user = "Tarang";
char * password = "Khanna";
char * sport;
char * args = "Room42";
int port = 8013;
char * room[30];

int lastMessage = 0;

int open_client_socket(char * host, int port) {
  // Initialize socket address structure
  struct  sockaddr_in socketAddress;

  // Clear sockaddr structure
  memset((char *)&socketAddress, 0, sizeof(socketAddress));

  // Set family to Internet 
  socketAddress.sin_family = AF_INET;

  // Set port
  socketAddress.sin_port = htons((u_short)port);

  // Get host table entry for this host
  struct  hostent  *ptrh = gethostbyname(host);
  if (ptrh == NULL) {
    perror("gethostbyname");
    exit(1);
  }

  // Copy the host ip address to socket address structure
  memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

  // Get TCP transport protocol entry
  struct  protoent *ptrp = getprotobyname("tcp");
  if (ptrp == NULL) {
    perror("getprotobyname");

    exit(1);
  }

  // Create a tcp socket
  int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  // Connect the socket to the specified server
  if (connect(sock, (struct sockaddr *)&socketAddress,
    sizeof(socketAddress)) < 0) {
    perror("connect");
    exit(1);
  }

  return sock;
}

int sendCommand(char * host, int port, char * command, char * user,
  char * password, char * args, char * response) {
  int sock = open_client_socket(host, port);

  // Send command
  write(sock, command, strlen(command));
  write(sock, " ", 1);
  write(sock, user, strlen(user));
  write(sock, " ", 1);
  write(sock, password, strlen(password));
  write(sock, " ", 1);
  write(sock, args, strlen(args));
  write(sock, "\r\n", 2);

  // Keep reading until connection is closed or MAX_REPONSE
  int n = 0;
  int len = 0;
  while ((n = read(sock, response + len, MAX_RESPONSE - len))>0) {
    len += n;
  }

  printf("response:%s\n", response);

  close(sock);
}

void printUsage()
{
  printf("Usage: talk-client host port user password\n");
  exit(1);
}  

void add_user(GtkButton *button, gpointer user_data)
{
  char response[MAX_RESPONSE];
  sendCommand(host, port, "ADD-USER", user, password, "", response);
  if (strcmp(response, "OK\r\n") == 0) {
    //printf("User %s added\n", user);
    //gchar * status2 = (gchar *); 
    gtk_label_set_text(GTK_LABEL(currentStatus),"Signed Up");
  }
}

void login()
{
  char response[MAX_RESPONSE];
  char response3[MAX_RESPONSE];
  user = (char *) gtk_entry_get_text(GTK_ENTRY(userName));
  password = (char *) gtk_entry_get_text(GTK_ENTRY(passWord));
  sendCommand(host, port, "LOG-IN", user, password, "", response);
  //strcpy()
  if (strstr(response, "OK\r\n") != NULL) {
    gtk_label_set_text(GTK_LABEL(currentStatus),"Logged In");
    //printf("User %s added\n", user);
    //list_room(); // to update response 
    update_list_rooms(); // put it in the widget 
  } else {
    gtk_label_set_text(GTK_LABEL(currentStatus),"Incorrect Login");
  }
}

void signup(GtkWidget *widget, gpointer data)
{
  // Try first to add user in case it does not exist.
  char response[MAX_RESPONSE];
  user = (char *) gtk_entry_get_text(GTK_ENTRY(userName));
  password = (char *) gtk_entry_get_text(GTK_ENTRY(passWord));
  sendCommand(host, port, "ADD-USER", user, password, "", response);
  if (strstr(response, "OK\r\n") != NULL) {
    //printf("User %s added\n", user);
    gtk_label_set_text(GTK_LABEL(currentStatus),"Signed Up");
  } else {
    //printf("User %s taken\n", user);
    gtk_label_set_text(GTK_LABEL(currentStatus),"UserName Taken");
  }
}

void create_room2() {
  char response[MAX_RESPONSE];
  args = (char *) gtk_entry_get_text(GTK_ENTRY(entryRoom)); //strcat with message?
  sendCommand(host, port, "CREATE-ROOM", user, password, args, response);
  //create_room2("localhost", 8013, "user", "password", "Room43");
  if (strstr(response, "OK\r\n") != NULL) {
    update_list_rooms();
    //char * room = (char *) malloc(sizeof(args)+1);
    //strcpy(room, args);
    //strcat("Room Created: ", args);
    gtk_label_set_text(GTK_LABEL(currentStatus),"Room Created");
    //printf("Room %s added\n", args);
  }
}


char * list_room() {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "LIST-ROOMS", user, password, "", response);
  
  if (!(strstr(response, "OK\r\n") != NULL)) {
    //printf(response);
    //printf("OK\n");
 } else {
    //printf(response);
    printf("Denied Listing\n");
    //gtk_label_set_text(GTK_LABEL(currentStatus),"Room Created");
 }
  return response;
}

void enter_room(char * host, int port, char * user,
  char * password, char * args) {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "ENTER-ROOM", user, password, args, response);

  if (strcmp(response, "OK\r\n") == 0) {
    printf("User %s added\n", user);
  }
}

void leave_room(char * host, int port, char * user,
  char * password, char * args) {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "LEAVE-ROOM", user, password, args, response);

  if (strcmp(response, "OK\r\n") == 0) {
    printf("User %s added\n", user);
  }
}

void get_messages() {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "GET-MESSAGES", user, password, args, response);

  if (strcmp(response, "OK\r\n") == 0) {
    printf("User %s added\n", user);
  }
}

void send_message(char * host, int port, char * user,
  char * password, char * args) {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "SEND-MESSAGE", user, password, args, response);

  if (strcmp(response, "OK\r\n") == 0) {
    printf("User %s added\n", user);
  }
}

char * print_users_in_room() {
  char response[MAX_RESPONSE];
  sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, args, response);

  if (!(strstr(response, "OK\r\n") != NULL)) {
    printf("Print User  = %s\n", user);
    return response;
  } else {
    printf("Denied Print User = %s\n", user);
    return "";
  }

}

void print_users() {
  char response[MAX_RESPONSE];
  // Try first to add user in case it does not exist.
  sendCommand(host, port, "GET-ALL-USERS", user, password, "", response);

  if (strcmp(response, "OK\r\n") == 0) {
    printf("User %s added\n", user);
  }
}

void printPrompt() {
  printf("talk> ");
  fflush(stdout);
}

void printHelp() {
  printf("Commands:\n");
  printf(" -who   - Gets users in room\n");
  printf(" -users - Prints all registered users\n");
  printf(" -help  - Prints this help\n");
  printf(" -quit  - Leaves the room\n");
  printf("Anything that does not start with \"-\" will be a message to the chat room\n");
}

void * getMessagesThread(void * arg) {
  // This code will be executed simultaneously with main()
  // Get messages to get last message number. Discard the initial Messages

  while (1) {
    // Get messages after last message number received.

    // Print messages

    // Sleep for ten seconds
    usleep(2 * 1000 * 1000);
  }
}

void startGetMessageThread()
{
  //pthread_t threads;
  pthread_create(NULL, NULL, getMessagesThread, NULL);
}

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
    char * response2 = strdup(list_room());
    char * tok;
    //printf("Reached room = %s\n", response2);
     if(changed) {
      tok = strtok (response2,"\r\n");
      while (tok != NULL) {
          string stok(tok); 
          roomVecNew.push_back(stok);
          tok = strtok (NULL, "\r\n");
      }
      int rn = roomVecNew.size();
      int r = roomVec.size();
      //printf("RN SIZE = %d\n", rn );
      //printf("R SIZE = %d\n", r );
      int i2;
      int y2;
      int count2 = 0;
      for(i2 = 0; i2 < roomVecNew.size(); i2++) {
        count2 = 0;
        for(y2 = 0; y2 < roomVec.size(); y2++) {
          if(roomVecNew[i2].compare(roomVec[y2]) != 0) {// diff 
             count2++;
            // printf("count2 diff! = %d, size = %d\n", count2, roomVec.size());
          }
          if(count2 == roomVec.size()) {
            //printf("Adding room2 = %s\n", roomVecNew[i2].c_str());
            gchar *msg = g_strdup_printf (roomVecNew[i2].c_str());
            gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
            gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
                            &iter,
                                  0, msg,
                            -1);
            g_free (msg);
          }
        }
      }
          //list is sorted!
      roomVec.swap(roomVecNew);
      //printf("R SIZE after = %d\n", roomVec.size() );
      roomVecNew.clear(); 
       } else {
       tok = strtok (response2,"\r\n");
       while (tok != NULL) { 
        gchar *msg = g_strdup_printf (tok);
        gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
        gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
                      &iter,
                            0, msg,
                      -1);
        g_free (msg);
        //printf ("%s\n",tok);
        roomVec.push_back(tok);
        tok = strtok (NULL, "\r\n");
       }
       if(roomVec.size() > 0) { // changed
          changed = true;
       }
  }
  //times++;
}

void room_changed(GtkWidget *widget, gpointer text) {
  
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;
  GtkTextBuffer *buffer;

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {
    gtk_tree_model_get(model, &iter, 0, &value,  -1);
    gtk_label_set_text(GTK_LABEL(currentStatus), value);
    //printf("Selected = %s\n",value); // updated response
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (viewUser));
    char * response2 = strdup(print_users_in_room()); //crashes here
    printf("reposnse is = %s\n", response2);
    if(response2 != "" && response2 != NULL && response2 != " ") {
       insert_text(buffer ,response2);
    } else {
      // nothing to list- update currentstatus 
    }
    g_free(value);
  }
}
/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    
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

/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text_User( const char * initialText )
{
   GtkWidget *scrolled_window;
   
   GtkTextBuffer *buffer;

   viewUser = gtk_text_view_new ();
   buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (viewUser));

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                   GTK_POLICY_AUTOMATIC,
           GTK_POLICY_AUTOMATIC);

   gtk_container_add (GTK_CONTAINER (scrolled_window), viewUser);
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

void show_error(GtkWidget *widget, gpointer window)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error loading file");
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
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
    GtkWidget *labelRoom;
    GtkWidget *labelUser;
    GtkWidget *labelUserRoom;
    GtkWidget *labelPass;
    GtkWidget *status;
    GtkWidget *labelMsg;
    GtkTreeSelection *treeSel;

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
    //update_list_rooms();
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
    //list_users = gtk_list_store_new (1, G_TYPE_STRING);
    //update_list_users();
    //userList = create_list ("Users in room", list_users);
    //gtk_table_attach_defaults (GTK_TABLE (table), userList, 4, 8, 0, 4);
    //gtk_widget_show (userList);
    roomUser = create_text_User ("User 1\n");
    gtk_table_attach_defaults (GTK_TABLE (table), roomUser, 4, 8, 1, 4);
    gtk_widget_show (roomUser);

    labelUserRoom = gtk_label_new("Users In Room:");
    gtk_misc_set_alignment(GTK_MISC(labelUserRoom),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelUserRoom,4, 8, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(labelUserRoom);

    messages = create_text ("Peter: Hi how are you\nMary: I am fine, thanks and you?\nPeter: Fine thanks.\n");
    gtk_table_attach_defaults (GTK_TABLE (table), messages, 2, 10, 5, 11);
    gtk_widget_show (messages);
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    //wcout << L"Hello, \u0444!\n";
    myMessage = create_text ("Hello\n");
    gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 2, 10, 11, 13);
    gtk_widget_show (myMessage);

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
    g_signal_connect (create_room, "clicked", G_CALLBACK (create_room2), NULL);

    //GtkWidget *enter_room = gtk_button_new_with_label ("Enter Room");
    //gtk_table_attach_defaults(GTK_TABLE (table), enter_room, 0, 2, 8, 9); 
    //gdk_color_parse ("orange", &color);
    //gtk_widget_modify_bg (GTK_WIDGET(enter_room), GTK_STATE_NORMAL, &color);
    //gtk_widget_show (enter_room); 

    GtkWidget *leave_room = gtk_button_new_with_label ("Leave Room");
    gtk_table_attach_defaults(GTK_TABLE (table), leave_room, 0, 2, 8, 9); 
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
    g_signal_connect (login_button, "clicked", G_CALLBACK (login), NULL);

    GtkWidget *signup_button = gtk_button_new_with_label ("Signup");
    gtk_table_attach_defaults(GTK_TABLE (table), signup_button, 10, 12, 13, 14); 
    //gdk_color_parse ("orange", &color);
    gtk_widget_modify_bg (GTK_WIDGET(signup_button), GTK_STATE_NORMAL, &color);
    gtk_widget_show (signup_button);  
    g_signal_connect (signup_button, "clicked", G_CALLBACK (signup), (gpointer) "Signup");


    labelRoom = gtk_label_new("Enter User Name:");
    gtk_misc_set_alignment(GTK_MISC(labelRoom),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelRoom,9, 11, 5, 7, GTK_FILL, GTK_FILL, 0, 0);

    userName = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), userName, 10, 12, 7, 9);
    gtk_widget_show(userName);
   
    labelUser = gtk_label_new("Username:");
    gtk_misc_set_alignment(GTK_MISC(labelUser),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelUser,10, 12, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(labelUser);

    labelPass = gtk_label_new("Password:");
    gtk_misc_set_alignment(GTK_MISC(labelPass),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), labelPass,10, 12, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(labelPass);

    passWord = gtk_entry_new_with_max_length(0);
    //gtk_entry_set_max_length (GTK_ENTRY (entry),3);
    gtk_table_attach_defaults (GTK_TABLE (table), passWord, 10, 12, 9, 11);
    gtk_widget_show(passWord);
    
    // radio visibility

    GtkWidget *check = gtk_check_button_new_with_label ("Editable");
    g_signal_connect (check, "toggled",
                G_CALLBACK (entry_toggle_visibility), passWord);

    GtkWidget *image = gtk_image_new_from_file("new-user-image-default.png");
    gtk_table_attach_defaults(GTK_TABLE (table), image, 10, 12, 0, 5); 
    gtk_widget_show (image);
    
    // g_signal_connect(image, "expose-event", G_CALLBACK(resize_image), (gpointer)window);
    
    // STATUS
    status = gtk_label_new("Status:");
    gtk_misc_set_alignment(GTK_MISC(status),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), status,8, 9, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(status);

    currentStatus = gtk_label_new("Start Up");
    gtk_misc_set_alignment(GTK_MISC(currentStatus),0.0,0.5);
    gtk_table_attach(GTK_TABLE(table), currentStatus,8, 9, 2, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_show(currentStatus);

    //selected rows
    treeSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    g_signal_connect(treeSel, "changed",  
      G_CALLBACK(room_changed), currentStatus); 

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
 
