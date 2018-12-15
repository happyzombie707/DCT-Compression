#ifndef TDC_H
#define TDC_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../PPM.h"
#include "../Util.h"
#include "../DCT/DCT.h"

GtkWidget *d_parent_window;

GtkWidget *d_container_box;
GtkWidget *d_image_box;
GtkWidget *d_tool_box;
GtkWidget *d_file_chooser;

GdkPixbuf *d_decomp_image;

GtkWidget *d_file_entry;
GtkWidget *d_load_button;


//
//callbacks for UI elements
//

//callback to load an image from the filechooser
void d_cb_load(GtkWidget *widget, gpointer data)
{
    char *filename;
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(d_file_chooser), ".");

    //get file name from browser
    if ( gtk_dialog_run(GTK_DIALOG(d_file_chooser)) == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(d_file_chooser));
        gtk_widget_hide(d_file_chooser);
    }else{
        gtk_widget_hide(d_file_chooser);
        return;
    }

    //init values to hold image info
    short b, b_w, h, w;
    double*** dct_img = load_compressed(filename, &h, &w, &b, &b_w);
    double*** img = create_3array(b, 8, 8);

    double** temp = create_array(8, 8);
    double** temp_dct = create_array(8, 8);
    
    //init dct
    dct_init();

    //for each block
    for(int k = 0; k < b; k++)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                temp[x][y] = dct_img[k][x][y];  //copy into temp array
            }
        }
        
        //inverse DCT temp
        iDCT(temp);

        //for each value in temp copy to img
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                img[k][x][y] = temp[x][y];
            }
        }
    }
    //free memory
    free_array(temp, 8);
    free_3array(dct_img, b, 8);
    
    //create and display image
    double** final_image = combine_arrays(img, h/8, w/8);
    d_decomp_image = data_to_image(final_image, h, w);
    gtk_image_set_from_pixbuf(GTK_IMAGE(d_image_box), d_decomp_image);

}


//init tab
void init_tdecompress(GtkWidget *win)
{
    
    d_parent_window = win;

    d_file_chooser = gtk_file_chooser_dialog_new("Open File",
                    GTK_WINDOW(d_parent_window),
                    GTK_FILE_CHOOSER_ACTION_OPEN,
                    "Cancel", GTK_RESPONSE_CANCEL,
                    "Accept", GTK_RESPONSE_ACCEPT,
                    NULL);


        //setup file filter to only allow ppm files and add it to the chooser
        GtkFileFilter *ff = gtk_file_filter_new();
        gtk_file_filter_add_pattern(ff, "*.dct");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(d_file_chooser), ff);



    d_container_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    //d_image_box = ;  
    d_tool_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    d_image_box = gtk_image_new_from_file("mem.bmp");

    //d_decomp_image;

    d_file_entry = gtk_entry_new();
    d_load_button = gtk_button_new_with_label("Load");

    gtk_box_pack_start(GTK_BOX(d_tool_box), d_file_entry, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(d_tool_box), d_load_button, TRUE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(d_container_box), d_image_box, TRUE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(d_container_box), d_tool_box, TRUE, FALSE, 0);

    g_signal_connect(d_load_button, "clicked", G_CALLBACK(d_cb_load), (gpointer) "epic button!");


}

//return tab
GtkWidget* get_tdecompress()
{
    return d_container_box;
}

#endif