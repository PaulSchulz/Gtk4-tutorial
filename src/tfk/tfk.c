
#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

float m_radius     = 0.42;
float m_line_width = 0.05;

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

    // Scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
    // the center of the window
    cairo_scale(cr, width, height);
    cairo_translate(cr, 0.5, 0.5);

    // Set the line width and save the cairo drawing state.
    cairo_set_line_width(cr, m_line_width);
    cairo_save(cr);

    // Set the background to a slightly transparent green.
    cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
    cairo_paint(cr);

    if (key[0] ==  3) {
        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1.0);
        cairo_paint(cr);
    }


}

static void
draw_clock (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

    // Scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
    // the center of the window
    cairo_scale(cr, width, height);
    cairo_translate(cr, 0.5, 0.5);

    // Set the line width and save the cairo drawing state.
    cairo_set_line_width(cr, m_line_width);
    cairo_save(cr);


    // Resore back to precious drawing state and draw the circular path
    // representing the clockface. Save this state (including the path) so we
    // can reuse it.
    cairo_restore(cr);
    cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
    cairo_save(cr);

    // Fill the clockface with white
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
    cairo_fill_preserve(cr);
    // Restore the path, paint the outside of the clock face.
    cairo_restore(cr);
    cairo_stroke_preserve(cr);
    // Set the 'clip region' to the inside of the path (fill region).
    cairo_clip(cr);

    // Clock ticks
    for (int i = 0; i < 24; i++)
    {
        // Major tick size
        double inset = 0.05;

        // Save the graphics state, restore after drawing tick to maintain pen
        // size
        cairo_save(cr);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

        // Minor ticks are shorter, and narrower.
        if(i % 3 != 0)
        {
            inset *= 0.8;
            cairo_set_line_width(cr, 0.03);
        }

        // Draw tick mark
        cairo_move_to(
            cr,
            (m_radius - inset) * cos (i * M_PI / 12.0),
            (m_radius - inset) * sin (i * M_PI / 12.0));
        cairo_line_to(
            cr,
            m_radius * cos (i * M_PI / 12.0),
            m_radius * sin (i * M_PI / 12.0));

        cairo_stroke(cr);
        cairo_restore(cr); /* stack-pen-size */

        if (true) {
            cairo_save(cr);
            char text[256];
            cairo_select_font_face(cr, "Courier",
                                   CAIRO_FONT_SLANT_NORMAL,
                                   CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 15);
            cairo_set_source_rgb (cr, 1.0, 0.0, 0.0); /* black */

            snprintf(text, 256, "%d", i);
            //cairo_move_to(cr,
            //             (m_radius - 2 * inset) * cos (i * M_PI / 12.0),
            //              (m_radius - 2 * inset) * sin (i * M_PI / 12.0));
            //cairo_show_text(cr, text);
            cairo_move_to(cr,0.0,0.0);
            cairo_show_text(cr, "*");
            //cairo_stroke(cr);
            cairo_restore(cr); /* stack-pen-size */
            cairo_stroke(cr);
        }
    }

    // Draw the analog hands

    // Get the current Unix time, convert to the local time and break into time
    // structure to read various time parts.
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime (&rawtime);

    // Calculate the angles of the hands of our clock
    double hours   = timeinfo->tm_hour * M_PI / 12.0;
    double minutes = timeinfo->tm_min * M_PI / 30.0;
    double seconds = timeinfo->tm_sec * M_PI / 30.0;

    // Save the graphics state
    cairo_save(cr);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_save(cr);

    // Draw the seconds hand
    cairo_set_line_width(cr, m_line_width / 3.0);
    cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr,
                  sin(seconds) * (m_radius * 0.9),
                  -cos(seconds) * (m_radius * 0.9));
    cairo_stroke(cr);
    cairo_restore(cr);

    // Draw the minutes hand
    cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr,
                  sin(minutes + seconds / 60) * (m_radius * 0.8),
                  -cos(minutes + seconds / 60) * (m_radius * 0.8));
    cairo_stroke(cr);

    // draw the hours hand
    cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr,
                  sin(hours + minutes / 12.0) * (m_radius * 0.5),
                  -cos(hours + minutes / 12.0) * (m_radius * 0.5));
    cairo_stroke(cr);
    cairo_restore(cr);

    // Draw a little dot in the middle
    cairo_arc(cr, 0.0, 0.0, m_line_width / 3.0, 0.0, 2.0 * M_PI);
    cairo_fill(cr);
}

//////////////////////////////////////////////////////////////////////////////
// --- keyboard ---
void
debug_keytype(guint keyval, guint keytype)
{
    printf("key_pressed: %5d %d / ", keyval, keytype);
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
    debug_keytype(keyval, keycode);
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


  // DEGUB
  debug_keytype(keyval, keycode);
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
