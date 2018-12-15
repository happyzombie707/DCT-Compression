#ifndef TIMG_H
#define TIMG_H

#define BLOCK_SIZE 8


#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../PPM.h"
#include "../DCT/DCT.h"
#include "../RLE/RLE.h"
#include <sys/time.h>

//-------------------
//|    VARIABLES    |
//-------------------

//define arrays to hold a sequence of 8x8 sub images
double*** squares;
//and the array for the 8x8 DCT sub images
double*** square_dct;
//quantised dct
double*** quant_dct;

bool loaded = false; 
int b_height, b_width;
int height, width;

//
// UI elements
//
//parent window and file chooser
GtkWidget* parent_window;
GtkWidget *c_file_chooser;

//frame container
GtkWidget *c_frame_cont;

//compress and save frames
GtkWidget *c_compress_frame;
GtkWidget *c_save_frame;

//containers
GtkWidget *c_container_box;
GtkWidget *c_tool_box;
GtkWidget *c_load_align;
GtkWidget *c_save_box;


//load widgets
GtkWidget *c_button_load;
GtkWidget *c_label_load;
GtkWidget *c_entry_file;

//image widget and pixbuf
GtkWidget *c_image;
GdkPixbuf *img_pixbuf;

//view mode and quantisation controls
GtkWidget *c_radio_orig;
GtkWidget *c_radio_dct;
GtkWidget *c_radio_qdct;
GtkWidget *cbo_quantise;

//save controls
GtkWidget *c_save_label;
GtkWidget *c_save_button;

//image loader
_ppm ppm;

//
// UTILS
//

//clean arrays
void clean_timage()
{
    free_3array(squares, b_width*b_height, 8);
    free_3array(square_dct, b_width*b_height, 8);
    free_3array(quant_dct, b_width*b_height, 8);
}

//set the image on the UI to the specified image
void set_c_image(double*** img_split, int block_height, int block_width, int img_height, int img_width, bool dct)
{
    double** image_data = create_array(img_height, img_width);
    image_data = combine_arrays(img_split, block_height, block_width);

    //if the image is a DCT display it differetly
    if(dct)
        img_pixbuf = get_dct_image(image_data, img_height, img_width);
    else
        img_pixbuf = data_to_image(image_data, img_height, img_width);

    gtk_image_set_from_pixbuf(GTK_IMAGE(c_image), img_pixbuf);
    free_array(image_data, img_height); 
}

//
// CALLBACKS
//

//get quantisation table based on combobox value
double** get_q_table() {

    char *mode = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(cbo_quantise));

    if(strcmp(mode, "Low") == 0){
        return low_matrix;
    }else if(strcmp(mode, "Medium") == 0){
        return med_matrix;
    }else if(strcmp(mode, "High") == 0){
        return high_matrix;
    }

    free (mode);

    return no_matrix;   

}



//apply chosen quantisation when combobox is changed
void apply_quantise(GtkComboBoxText *cbo, gpointer data)
{
    if (!loaded)
        return;
    //quantise
    for (int i = 0; i < b_width * b_height; i++)
        quant_dct[i] = quantise_matrix(square_dct[i], get_q_table(), BLOCK_SIZE);
    //change image
    set_c_image(quant_dct, b_height, b_width, height, width, true);

}

//Change image on radio toggle
void radio_toggled(GtkToggleButton *widget, gpointer data)
{

    if (!loaded)
        return;
    
    int mode = GPOINTER_TO_INT(data);


     if (gtk_toggle_button_get_active(widget) && mode == 0)
        set_c_image(squares, b_height, b_width, height, width, false);
    else if (gtk_toggle_button_get_active(widget) && mode == 1)
        set_c_image(square_dct, b_height, b_width, height, width, true);
    else if (gtk_toggle_button_get_active(widget) && mode == 2) {
         apply_quantise(GTK_COMBO_BOX_TEXT(cbo_quantise), NULL);
    }
}


//callback to save image
void cb_save_dct(GtkWidget *widget, gpointer data)
{
    if(!loaded)
        return;
    //get name and save
    const char* filename = gtk_label_get_text(GTK_LABEL(c_save_label));
    save_compressed(filename, quant_dct, get_q_table(), b_width*b_height, b_width, height, width);
}

//callback to load an image from the filechooser
void cb_load_ppm(GtkWidget *widget, gpointer data)
{
    if (loaded)
    {
        clean_timage();
        loaded = false;
    }
    

    //set file chooser default directory to the current dir
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(c_file_chooser), ".");

    //if an image was selected from the file chooser
    if ( gtk_dialog_run(GTK_DIALOG(c_file_chooser)) == GTK_RESPONSE_ACCEPT)
    {
        //read filename into variable set entry and read image into GtkImage
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(c_file_chooser));
        char *new_filename = (char*)malloc(sizeof(char) * strlen(filename) + 1); memcpy(new_filename, filename, sizeof(char) * strlen(filename)+1);
        char *ext = "dct";
        memcpy(new_filename+strlen(new_filename)-3, ext, sizeof(char)*3);
        
        gtk_entry_set_text(GTK_ENTRY(c_entry_file), filename);
        //gtk_entry_set_text(GTK_ENTRY(c_save_entry), new_filename);
        gtk_label_set_text(GTK_LABEL(c_save_label), new_filename);

        //load image from file and scale it to 256x256
        
        //clear the last image in the GtkImage
        gtk_image_clear(GTK_IMAGE(c_image));

        //set image from the loaded pixbuf
    
        //load ppm image into ppm object
        if(ppm.load_ppm(filename) != 1)
        {
            printf("Error loading file %s\n", filename);
            return;
        }


        //get image height and width
        height = round_up(ppm.get_image_height(), 8); width = round_up(ppm.get_image_width(), 8);
        b_height = height / BLOCK_SIZE; b_width = width / BLOCK_SIZE;


        //free(filename);
        free(new_filename);

        //create a double** to hold the raw values of the image
        double** image_buffer = ppm.get_image_ptr(BLOCK_SIZE);
        

        squares = create_3array(b_height*b_width, BLOCK_SIZE, BLOCK_SIZE);
        square_dct = create_3array(b_height*b_width, BLOCK_SIZE, BLOCK_SIZE);
        quant_dct = create_3array(b_height*b_width, BLOCK_SIZE, BLOCK_SIZE);

        
        //init dct values
        dct_init();
        //print_array(q_matrix);

        int index;
        //for each 8x8 block in image
        for (int y = 0; y < b_height; y++)
        {
            for (int x = 0; x < b_width; x++)
            {   index = y * b_width + x;
                //get 8x8 subimage
                squares[index] = copy_square(image_buffer, x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                //get dct of subimage
                square_dct[index] = fDCT(squares[index]);
                //get quantised dct
                quant_dct[index] = quantise_matrix(square_dct[index], high_matrix, BLOCK_SIZE);
            }
        }

        set_c_image(squares, b_height, b_width, height, width, false);

        free_array(image_buffer, height);
        loaded = true;
    }
    gtk_widget_hide(c_file_chooser);
}

//
// UI
//

//init tab
void init_tcompress(GtkWidget *win)
{
    parent_window = win;

    c_file_chooser = gtk_file_chooser_dialog_new("Open File",
                                GTK_WINDOW(parent_window),
                                GTK_FILE_CHOOSER_ACTION_OPEN,
                                "Cancel", GTK_RESPONSE_CANCEL,
                                "Accept", GTK_RESPONSE_ACCEPT,
                                      NULL);


    //setup file filter to only allow ppm files and add it to the chooser
    GtkFileFilter *ff = gtk_file_filter_new();
    gtk_file_filter_add_pattern(ff, "*.ppm");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(c_file_chooser), ff);


    c_frame_cont = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(c_frame_cont), 10);


    //
    // LOAD AND COMPRESS FRAME
    //

    //init frame
    c_compress_frame = gtk_frame_new("Compress");

    //setup containers
    c_container_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    c_tool_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    //init load controls
    c_button_load = gtk_button_new_with_label("Load");
    c_label_load = gtk_label_new("Image");
    c_entry_file = gtk_entry_new();

    //init image
    c_image = gtk_image_new_from_file("mem.bmp");

    //init radio buttons
    c_radio_orig = gtk_radio_button_new_with_label(NULL, "Original");
    c_radio_dct = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(c_radio_orig), "DCT");
    c_radio_qdct = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(c_radio_orig), "Quantised");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_radio_orig), TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_radio_dct), FALSE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_radio_qdct), FALSE);

    //init combobox
    cbo_quantise = gtk_combo_box_text_new();
    const char *compression_modes[] = {"Select quantiser", "Low", "Medium", "High"};
    for (int i = 0; i < G_N_ELEMENTS (compression_modes); i++) //G_N_ELEMENTS finds the length of an array
  	    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(cbo_quantise), compression_modes[i]);
    gtk_combo_box_set_active (GTK_COMBO_BOX (cbo_quantise), 0);


    //pack load controls into a horizontal box
    c_load_align = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(c_load_align), c_label_load, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(c_load_align), c_entry_file, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(c_load_align), c_button_load, FALSE, FALSE, 5);

    //pack load controls, radio buttons and combobox into the toolbox container
    gtk_box_pack_start(GTK_BOX(c_tool_box), c_load_align, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), c_radio_orig, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), c_radio_dct, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), c_radio_qdct, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), cbo_quantise, FALSE, FALSE, 10);

    //pack tool box and image into container
    gtk_box_pack_start(GTK_BOX(c_container_box), c_tool_box, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(c_container_box), c_image, TRUE, TRUE, 0);

    //add compress box to frame
    gtk_container_add(GTK_CONTAINER(c_compress_frame), c_container_box);


    //
    //  SAVE FRAME
    //

    //set up frame, container and UI controls
    c_save_frame = gtk_frame_new("Save");
    c_save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    c_save_label = gtk_label_new("Save image");
    c_save_button = gtk_button_new_with_label("Save");

    //add  save controls to container
    gtk_box_pack_start(GTK_BOX(c_save_box), c_save_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(c_save_box), c_save_button, FALSE, FALSE, 5);
    //add container to frame
    gtk_container_add(GTK_CONTAINER(c_save_frame), c_save_box);

    //add compress and save frames to final container
    gtk_box_pack_start(GTK_BOX(c_frame_cont), c_compress_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(c_frame_cont), c_save_frame, FALSE, FALSE, 5);


    //setup callbacks
    g_signal_connect(c_button_load, "clicked", G_CALLBACK(cb_load_ppm), NULL);
    g_signal_connect (GTK_TOGGLE_BUTTON (c_radio_orig), "toggled", G_CALLBACK (radio_toggled), (gpointer)0);
    g_signal_connect (GTK_TOGGLE_BUTTON (c_radio_dct), "toggled", G_CALLBACK (radio_toggled), (gpointer)1);
    g_signal_connect (GTK_TOGGLE_BUTTON (c_radio_qdct), "toggled", G_CALLBACK (radio_toggled), (gpointer)2);
    g_signal_connect (GTK_TOGGLE_BUTTON (cbo_quantise), "changed", G_CALLBACK (apply_quantise), NULL);
    g_signal_connect(c_save_button, "clicked", G_CALLBACK(cb_save_dct), NULL);

}

GtkWidget* get_tcompress()
{
    //return top level container
    return c_frame_cont;

}



#endif