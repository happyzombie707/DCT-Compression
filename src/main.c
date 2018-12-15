#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "PPM.h"
#include "UI/tCompress.h"
#include "UI/tDecompress.h"


int main(int argc, char *argv[])
{

  //main window
  GtkWidget *window;    //GtkWindow
  GtkWidget *tab_view;   //GtkNotebook

  GtkWidget *tab_container; //GtkBox

  gtk_init(&argc, &argv);

  //init window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Image Compressor");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 350);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);

  //set up conntainer for the notebook
  tab_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  //set up tab view
  tab_view = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tab_view), GTK_POS_TOP);

  //init tabs
  init_tcompress(window);
  init_tdecompress(window);

  //add tabs to notebook and set labels
  gtk_container_add(GTK_CONTAINER(tab_view), get_tcompress());
  gtk_notebook_set_tab_label (GTK_NOTEBOOK(tab_view), get_tcompress(), gtk_label_new("Compress"));
  
  gtk_container_add(GTK_CONTAINER(tab_view), get_tdecompress());
  gtk_notebook_set_tab_label (GTK_NOTEBOOK(tab_view), get_tdecompress(), gtk_label_new("Decompress"));
  
  //add notebook to container and add container to window
  gtk_box_pack_start(GTK_BOX(tab_container), tab_view, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), tab_container);

  //show
  gtk_widget_show_all(window);

  //set up close callback
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_main();

  clean_timage();

  return 0;
}
