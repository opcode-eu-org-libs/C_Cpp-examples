/**
 * bardzo prosty przykład GUI w GTK+
 *
 * kompilacja (GTK+ 2): gcc `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` gtk.c
 * (wymaga pakietu: libgtk2.0-dev)
 *
 * kompilacja (GTK+ 3): gcc `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` gtk.c
 * (wymaga pakietu: libgtk-3-dev)
 *
 * analogiczny program w Pythonie znajdziesz na: http://vip.opcode.eu.org/#Graficzny_interfejs_użytkownika
 */

#include <stdio.h>
#include <gtk/gtk.h> // nagłówki GTK

void showDialog(GtkWidget *widget, gpointer window) {
	GtkWidget *dialog = gtk_message_dialog_new(NULL, 0,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Ważne pytanie !!!");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Tak czy nie?");
	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_YES) {
		puts("TAK !!!");
	} else if (response == GTK_RESPONSE_NO) {
		puts("NIE !!!");
	} else {
		puts("bez odpowiedzi!?");
	}
	
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Hello World");
	
	GtkWidget *button1 = gtk_button_new_with_label("Naciśnij mnie");
	g_signal_connect(button1, "clicked", G_CALLBACK(showDialog), NULL);
	
	gtk_container_add(GTK_CONTAINER(window), button1);
	
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_main();
}
