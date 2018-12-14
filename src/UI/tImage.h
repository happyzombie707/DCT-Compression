#ifndef TIMG_H
#define TIMG_H

#define BLOCK_SIZE 8


#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../PPM.h"
#include "../DCT/DCT.h"
#include "../RLE/RLE.h"

//-------------------
//|    VARIABLES    |
//-------------------

//define arrays to hold a sequence of 8x8 sub images
double*** squares;
//and the array for the 8x8 DCT sub images
double*** square_dct;
//quantized dct
double*** quant_dct;

bool loaded = false; 
int b_height, b_width;
int height, width;

GtkWidget* parent_window;

GtkWidget *c_frame_cont;

//load widgets
GtkWidget *c_load_frame;
GtkWidget *button;
GtkWidget *halign;
GtkWidget *test;
GtkWidget *c_load_button;
GtkWidget *c_load_label;
GtkWidget *innerBox;

//sample widgets
GtkWidget *sampleFrame;
GtkWidget *scale_container;
GtkWidget *sample_container;

GtkWidget *c_file_chooser;
GtkWidget *c_entry_file;
GdkPixbuf *initial_image;
GdkPixbuf *dct_pixbuf;
GdkPixbuf *dct_q_pixbuf;

GtkWidget *c_original_image;


GtkWidget *c_container_box;
GtkWidget *c_image_box;
GtkWidget *c_tool_box;

GtkWidget *c_image;
GtkWidget *c_radio_orig;
GtkWidget *c_radio_dct;
GtkWidget *cbo_quantize;

GtkWidget *c_save_frame;
GtkWidget *c_save_box;
GtkWidget *c_save_label;
GtkWidget *c_save_entry;
GtkWidget *c_save_button;



_ppm ppm;


//get quantisation table based on combobox value
double** get_q_table() {

    char *mode = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(cbo_quantize));

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

//set the compressed image
void set_c_image(double*** img_split, int block_height, int block_width, int img_height, int img_width)
{
    double** image_data = create_array(img_height, img_width);
    image_data = combine_arrays(img_split, block_height, block_width);

    dct_pixbuf = get_dct_image(image_data, img_height, img_width);
    gtk_image_set_from_pixbuf(GTK_IMAGE(c_image), dct_pixbuf);

    free_array(image_data, img_height); 
}

//apply chosen quantization
void apply_quantize(GtkComboBoxText *cbo, gpointer data)
{
    if (!loaded)
        return;

    for (int i = 0; i < b_width * b_height; i++)
        quant_dct[i] = quantize_matrix(square_dct[i], get_q_table(), BLOCK_SIZE);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(c_radio_dct)))
        set_c_image(quant_dct, b_height, b_width, height, width);

}

//display quantised DCT image
void show_q_dct(GtkToggleButton *widget, gpointer data)
{

    if (!loaded || !gtk_toggle_button_get_active(widget))
        return;
    
    //ca
    apply_quantize(GTK_COMBO_BOX_TEXT(cbo_quantize), NULL);
    
    set_c_image(quant_dct, b_height, b_width, height, width);

}

//Display DCT image
void show_dct(GtkToggleButton *widget, gpointer data)
{
    if (!loaded || !gtk_toggle_button_get_active(widget))
        return;

    set_c_image(square_dct, b_height, b_width, height, width);
    
}

//clean arrays
void clean_timage()
{
    free_3array(squares, b_width*b_height, 8);
    free_3array(square_dct, b_width*b_height, 8);
    free_3array(quant_dct, b_width*b_height, 8);
}


//callback to save image
void cb_save_dct(GtkWidget *widget, gpointer data)
{
    if(!loaded)
        return;
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
        initial_image = gdk_pixbuf_new_from_file(filename, NULL);
        initial_image = gdk_pixbuf_scale_simple(initial_image, 256, 256, GDK_INTERP_NEAREST);
        
        //clear the last image in the GtkImage
        gtk_image_clear(GTK_IMAGE(c_original_image));

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

        gtk_image_set_from_pixbuf(GTK_IMAGE(c_original_image), data_to_image(ppm.get_image_ptr(8), height, width));

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
                //get quantized dct
                quant_dct[index] = quantize_matrix(square_dct[index], high_matrix, BLOCK_SIZE);
            }
        }

        //combine dct into a complete image, create pixbuf and set image
        double** dct_image = combine_arrays((double***)square_dct, b_height, b_width);
        dct_pixbuf = get_dct_image(dct_image, height, width);
        gtk_image_set_from_pixbuf(GTK_IMAGE(c_image), dct_pixbuf);

        free_array(image_buffer, height);
        loaded = true;
    }
    gtk_widget_hide(c_file_chooser);
}


//-------------------
//|       UI        |
//------------------

void init_timage(GtkWidget *win)
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
    c_load_frame = gtk_frame_new("Load");
    gtk_container_set_border_width(GTK_CONTAINER(c_frame_cont), 10);


    /*-----------------------------------
    ---          LOAD FRAME           ---
    -----------------------------------*/
    //init load frame
    c_load_button = gtk_button_new_with_label("Load");

    //gtk_widget_set_pr
    c_load_label = gtk_label_new("Image");
    c_entry_file = gtk_entry_new();

    c_original_image = gtk_image_new_from_file("mem.bmp");
    test = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    halign = gtk_grid_new();
    innerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    g_object_set(innerBox, "margin", 10, NULL);

    gtk_grid_set_column_spacing(GTK_GRID(halign), 10);

    gtk_widget_set_valign(halign, GTK_ALIGN_START);

    gtk_grid_attach(GTK_GRID(halign), c_load_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(halign), c_entry_file, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(halign), c_load_button, 4, 0, 2, 1);
    gtk_container_add(GTK_CONTAINER(innerBox), halign);
    GtkWidget *pic_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(pic_wrapper), c_original_image, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(innerBox), pic_wrapper);

    //gtk_grid_attach(GTK_GRID(halign), c_original_image, 6, 0, 10, 10);

    gtk_container_add(GTK_CONTAINER(test), innerBox);
    gtk_container_add(GTK_CONTAINER(c_load_frame), test);
    gtk_box_pack_start(GTK_BOX(c_frame_cont), c_load_frame, TRUE, TRUE, 20);

    //init sample frame
    sampleFrame = gtk_frame_new("Compress");
    //stuff

    g_signal_connect(c_load_button, "clicked", G_CALLBACK(cb_load_ppm), (gpointer) "epic button!");



    /*-----------------------------------
    ---        SAMPLING FRAME         ---
    -----------------------------------*/

    c_container_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    c_image_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    c_tool_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    c_image = gtk_image_new_from_file("mem.bmp");

    c_radio_orig = gtk_radio_button_new_with_label(NULL, "DCT");
    c_radio_dct = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(c_radio_orig), "Quantized");
    cbo_quantize = gtk_combo_box_text_new();
      
    const char *distros[] = {"Select quantizer", "Low", "Medium", "High"};
    
    /* G_N_ELEMENTS is a macro which determines the number of elements in an array.*/ 
    for (int i = 0; i < G_N_ELEMENTS (distros); i++){
  	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(cbo_quantize), distros[i]);
    }

  /* Choose to set the first row as the active one by default, from the beginning */
  gtk_combo_box_set_active (GTK_COMBO_BOX (cbo_quantize), 0);

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_radio_orig), TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_radio_dct), TRUE);

    g_signal_connect (GTK_TOGGLE_BUTTON (c_radio_orig), "toggled", G_CALLBACK (show_dct), (gpointer)0);
    g_signal_connect (GTK_TOGGLE_BUTTON (c_radio_dct), "toggled", G_CALLBACK (show_q_dct), (gpointer)1);
    g_signal_connect (GTK_TOGGLE_BUTTON (cbo_quantize), "changed", G_CALLBACK (apply_quantize), NULL);

    gtk_box_pack_start(GTK_BOX(c_tool_box), c_radio_orig, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), c_radio_dct, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(c_tool_box), cbo_quantize, FALSE, FALSE, 10);


    gtk_box_pack_start(GTK_BOX(c_container_box), c_tool_box, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(c_container_box), c_image, TRUE, TRUE, 0);


    gtk_container_add(GTK_CONTAINER(sampleFrame), c_container_box);
    gtk_box_pack_start(GTK_BOX(c_frame_cont), sampleFrame, TRUE, TRUE, 0);

    c_save_frame = gtk_frame_new("Save");
    c_save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    c_save_label = gtk_label_new("Save image");
    c_save_entry = gtk_entry_new();
    c_save_button = gtk_button_new_with_label("Save");

    gtk_box_pack_start(GTK_BOX(c_save_box), c_save_label, FALSE, FALSE, 5);
    //gtk_box_pack_start(GTK_BOX(c_save_box), c_save_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(c_save_box), c_save_button, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(c_save_frame), c_save_box);

    gtk_box_pack_start(GTK_BOX(c_frame_cont), c_save_frame, TRUE, TRUE, 5);

    g_signal_connect(c_save_button, "clicked", G_CALLBACK(cb_save_dct), (gpointer) "epic button!");

}

GtkWidget* get_timage()
{
    
    return c_frame_cont;

}



#endif