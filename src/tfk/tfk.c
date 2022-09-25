#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////
// --- keyboard ---

#define KEY_SEEN     1
#define KEY_RELEASED 2

enum Keytype {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_MAX
};

unsigned char key[KEY_MAX];

//////////////////////////////////////////////////////////////////////////////
static void
draw_arrows (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    cairo_scale(cr, width, height);

    // Set the line width and save the cairo drawing state.
    cairo_set_line_width(cr, 0.02);
    cairo_save(cr);

    // Re-set the screen
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    cairo_paint(cr);

    for (int i = 0; i < KEY_MAX; i++) {
        if (key[i] ==  3) {
            cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 1.0);
            cairo_arc(cr, 0.1 + 0.2*i, 0.5, 0.08, 0.0, 2.0 * M_PI);
            cairo_stroke(cr);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// --- keyboard ---
void
debug_keytype(guint keyval, guint keytype)
{
    printf("key: %5d %3d / ", keyval, keytype);
    printf("key[]: %d %d %d %d %d\n",
           key[0], key[1], key[2], key[3], key[4]);
}

static int
keyval_to_keytype(guint keyval)
{
  if(keyval == GDK_KEY_Left)
      return KEY_LEFT;
  if(keyval == GDK_KEY_Right)
    return KEY_RIGHT;
  if(keyval == GDK_KEY_Up)
    return KEY_UP;
  if(keyval == GDK_KEY_Down)
    return KEY_DOWN;
  if(keyval == GDK_KEY_space)
    return KEY_SPACE;

  return -1;
}

static void
key_pressed (GtkEventControllerKey *controller,
             guint keyval,
             guint keycode,
             GdkModifierType state,
             gpointer user_data)
{
    int keytype = keyval_to_keytype(keyval);
    if(keytype > -1)
    key[keytype] = KEY_SEEN | KEY_RELEASED;

    // Trigger window redraw
    gtk_widget_queue_draw((GtkWidget*)user_data);

    //  if(keyval == GDK_KEY_Escape)
    //      gtk_window_close(GTK_WINDOW(app));

    // DEGUB
    // debug_keytype(keyval, keycode);
}

static void
key_released (GtkEventControllerKey *controller,
              guint keyval,
              guint keycode,
              GdkModifierType state,
              gpointer user_data)
{
    int keytype = keyval_to_keytype(keyval);
    if(keytype > -1)
        key[keytype] &= KEY_RELEASED;

    // Trigger window redraw
    gtk_widget_queue_draw((GtkWidget*)user_data);

    // Debug
    // debug_keytype(keyval, keycode);
}

//////////////////////////////////////////////////////////////////////////////
static void
app_activate (GApplication *app, gpointer user_data)
{
    GtkWidget *win;
    GtkWidget *drawable;
    GtkBuilder *build;

    build = gtk_builder_new_from_resource ("/com/github/ToshioCP/tfc/tfk.ui");
    win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
    gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

    drawable = GTK_WIDGET (gtk_builder_get_object (build, "drawable"));
    g_object_unref(build);

    // --- add keyboard callback ---
    GtkEventController *keyboard = gtk_event_controller_key_new();
    gtk_widget_add_controller(win, keyboard);
    g_signal_connect(keyboard, "key-pressed",  G_CALLBACK(key_pressed),  drawable);
    g_signal_connect(keyboard, "key-released", G_CALLBACK(key_released), drawable);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (drawable), draw_arrows, NULL, NULL);

    gtk_widget_show(win);
}

static void
app_open (GApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
    app_activate(app,user_data);
}

int
main (int argc, char **argv) {
    GtkApplication *app;
    int stat;

    app = gtk_application_new ("com.github.ToshioCP.tfk", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);

    stat = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}
