#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

/* ---------- DATA STRUCTURES ---------- */

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

/* ---------- GLOBAL UI ---------- */

GtkWidget *text_view;
GtkTextBuffer *buffer;

/* ---------- UTILITY ---------- */

void append_text(const char *text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, text, -1);
}

/* ---------- CORE LOGIC ---------- */

void add_volunteer_data(const char *name, const char *skill, int available) {
    Volunteer* v = malloc(sizeof(Volunteer));
    strcpy(v->name, name);
    strcpy(v->skill, skill);
    v->available = available;
    v->next = volunteerHead;
    volunteerHead = v;
}

void add_task_data(const char *name, const char *skill, int urgency) {
    Task* t = malloc(sizeof(Task));
    strcpy(t->name, name);
    strcpy(t->requiredSkill, skill);
    t->urgency = urgency;
    t->next = taskHead;
    taskHead = t;
}

/* ---------- DIALOGS ---------- */

void add_volunteer(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *grid;
    GtkWidget *name_entry, *skill_entry, *avail_entry;

    dialog = gtk_dialog_new_with_buttons(
        "Add Volunteer",
        NULL,
        GTK_DIALOG_MODAL,
        "Add",
        GTK_RESPONSE_OK,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        NULL
    );

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    name_entry = gtk_entry_new();
    skill_entry = gtk_entry_new();
    avail_entry = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(skill_entry), "Skill");
    gtk_entry_set_placeholder_text(GTK_ENTRY(avail_entry), "Available (1/0)");

    gtk_grid_attach(GTK_GRID(grid), name_entry, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), skill_entry, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), avail_entry, 0, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char *skill = gtk_entry_get_text(GTK_ENTRY(skill_entry));
        int avail = atoi(gtk_entry_get_text(GTK_ENTRY(avail_entry)));

        add_volunteer_data(name, skill, avail);
        append_text("Volunteer added successfully.\n");
    }

    gtk_widget_destroy(dialog);
}

void add_task(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *grid;
    GtkWidget *name_entry, *skill_entry, *urg_entry;

    dialog = gtk_dialog_new_with_buttons(
        "Add Task",
        NULL,
        GTK_DIALOG_MODAL,
        "Add",
        GTK_RESPONSE_OK,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        NULL
    );

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

    name_entry = gtk_entry_new();
    skill_entry = gtk_entry_new();
    urg_entry = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Task Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(skill_entry), "Required Skill");
    gtk_entry_set_placeholder_text(GTK_ENTRY(urg_entry), "Urgency (1-5)");

    gtk_grid_attach(GTK_GRID(grid), name_entry, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), skill_entry, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), urg_entry, 0, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        add_task_data(
            gtk_entry_get_text(GTK_ENTRY(name_entry)),
            gtk_entry_get_text(GTK_ENTRY(skill_entry)),
            atoi(gtk_entry_get_text(GTK_ENTRY(urg_entry)))
        );
        append_text("Task added successfully.\n");
    }

    gtk_widget_destroy(dialog);
}

void match_volunteers(GtkWidget *widget, gpointer data) {
    Task *t = taskHead;
    append_text("\n--- Matching Volunteers ---\n");

    while (t) {
        Volunteer *v = volunteerHead;
        int found = 0;

        char header[200];
        sprintf(header, "Task: %s (Skill: %s)\n", t->name, t->requiredSkill);
        append_text(header);

        while (v) {
            if (v->available && strcmp(v->skill, t->requiredSkill) == 0) {
                char line[100];
                sprintf(line, "  -> %s is suitable\n", v->name);
                append_text(line);
                found = 1;
            }
            v = v->next;
        }

        if (!found)
            append_text("  No suitable volunteer found\n");

        t = t->next;
        append_text("\n");
    }
}

/* ---------- MAIN ---------- */

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Volunteer Skill Matcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *btn_vol = gtk_button_new_with_label("Add Volunteer");
    GtkWidget *btn_task = gtk_button_new_with_label("Add Task");
    GtkWidget *btn_match = gtk_button_new_with_label("Match Volunteers");

    text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);

    gtk_grid_attach(GTK_GRID(grid), btn_vol, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_task, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_match, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 3, 1);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_vol, "clicked", G_CALLBACK(add_volunteer), NULL);
    g_signal_connect(btn_task, "clicked", G_CALLBACK(add_task), NULL);
    g_signal_connect(btn_match, "clicked", G_CALLBACK(match_volunteers), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
