// warning guys: this code runs only in linux since gtk env is set only for linux :)

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

typedef struct Volunteer {
    char name[50];
    char skill[50];
    int available;
    struct Volunteer* next;
} Volunteer;

typedef struct Task {
    char name[50];
    char requiredSkill[50];
    int urgency;
    struct Task* next;
} Task;

Volunteer* volunteerHead = NULL;
Task* taskHead = NULL;


GtkWidget *vol_name, *vol_skill, *vol_available;
GtkWidget *task_name, *task_skill, *task_urgency;
GtkWidget *output_view;
GtkTextBuffer *output_buffer;


void append_output(const char *text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(output_buffer, &end);
    gtk_text_buffer_insert(output_buffer, &end, text, -1);
}

void add_volunteer_gui(GtkWidget *widget, gpointer data) {
    Volunteer *v = malloc(sizeof(Volunteer));

    strcpy(v->name, gtk_entry_get_text(GTK_ENTRY(vol_name)));
    strcpy(v->skill, gtk_entry_get_text(GTK_ENTRY(vol_skill)));
    v->available = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(vol_available));

    v->next = volunteerHead;
    volunteerHead = v;

    append_output("Volunteer added successfully!\n");
}

void add_task_gui(GtkWidget *widget, gpointer data) {
    Task *t = malloc(sizeof(Task));

    strcpy(t->name, gtk_entry_get_text(GTK_ENTRY(task_name)));
    strcpy(t->requiredSkill, gtk_entry_get_text(GTK_ENTRY(task_skill)));
    t->urgency = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(task_urgency));

    t->next = taskHead;
    taskHead = t;

    append_output("Task added successfully!\n");
}

void match_gui(GtkWidget *widget, gpointer data) {
    Task *t = taskHead;
    append_output("\n--- Matching Results ---\n");

    while(t) {
        Volunteer *v = volunteerHead;
        int found = 0;
        char line[200];

        sprintf(line, "Task: %s (Skill: %s)\n", t->name, t->requiredSkill);
        append_output(line);

        while(v) {
            if(v->available && strcmp(v->skill, t->requiredSkill) == 0) {
                sprintf(line, "  -> %s matched\n", v->name);
                append_output(line);
                found = 1;
            }
            v = v->next;
        }
        if(!found) append_output("  No volunteer available\n");
        t = t->next;
    }
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Volunteer Skill Matcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_container_add(GTK_CONTAINER(window), grid);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Volunteer Name"), 0, 0, 1, 1);
    vol_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), vol_name, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Skill"), 0, 1, 1, 1);
    vol_skill = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), vol_skill, 1, 1, 1, 1);

    vol_available = gtk_check_button_new_with_label("Available");
    gtk_grid_attach(GTK_GRID(grid), vol_available, 1, 2, 1, 1);

    GtkWidget *add_vol_btn = gtk_button_new_with_label("Add Volunteer");
    gtk_grid_attach(GTK_GRID(grid), add_vol_btn, 1, 3, 1, 1);
    g_signal_connect(add_vol_btn, "clicked", G_CALLBACK(add_volunteer_gui), NULL);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Task Name"), 3, 0, 1, 1);
    task_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), task_name, 4, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Required Skill"), 3, 1, 1, 1);
    task_skill = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), task_skill, 4, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Urgency"), 3, 2, 1, 1);
    task_urgency = gtk_spin_button_new_with_range(1, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), task_urgency, 4, 2, 1, 1);

    GtkWidget *add_task_btn = gtk_button_new_with_label("Add Task");
    gtk_grid_attach(GTK_GRID(grid), add_task_btn, 4, 3, 1, 1);
    g_signal_connect(add_task_btn, "clicked", G_CALLBACK(add_task_gui), NULL);

    GtkWidget *match_btn = gtk_button_new_with_label("Match Volunteers");
    gtk_grid_attach(GTK_GRID(grid), match_btn, 2, 4, 2, 1);
    g_signal_connect(match_btn, "clicked", G_CALLBACK(match_gui), NULL);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), output_view);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 5, 6, 1);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
