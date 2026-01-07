#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* ---------- GTK WIDGETS ---------- */
GtkWidget *vol_name, *vol_skill, *vol_avail;
GtkWidget *task_name, *task_skill, *task_urgency;
GtkWidget *output;

/* ---------- HELPER FUNCTION ---------- */
void print_output(const char *text) {
    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(output));
    gtk_text_buffer_set_text(buffer, text, -1);
}

/* ---------- BUTTON CALLBACKS ---------- */

void add_volunteer(GtkWidget *widget, gpointer data) {
    Volunteer* v = malloc(sizeof(Volunteer));

    strcpy(v->name, gtk_entry_get_text(GTK_ENTRY(vol_name)));
    strcpy(v->skill, gtk_entry_get_text(GTK_ENTRY(vol_skill)));
    v->available = atoi(gtk_entry_get_text(GTK_ENTRY(vol_avail)));

    v->next = volunteerHead;
    volunteerHead = v;

    print_output("Volunteer added successfully!");
}

void add_task(GtkWidget *widget, gpointer data) {
    Task* t = malloc(sizeof(Task));

    strcpy(t->name, gtk_entry_get_text(GTK_ENTRY(task_name)));
    strcpy(t->requiredSkill, gtk_entry_get_text(GTK_ENTRY(task_skill)));
    t->urgency = atoi(gtk_entry_get_text(GTK_ENTRY(task_urgency)));

    t->next = taskHead;
    taskHead = t;

    print_output("Task added successfully!");
}

void display_volunteers(GtkWidget *widget, gpointer data) {
    char buffer[1024] = "--- Volunteers ---\n";
    Volunteer* temp = volunteerHead;

    if (!temp) {
        strcat(buffer, "No volunteers available.\n");
    }

    while (temp) {
        char line[256];
        sprintf(line,
                "Name: %s | Skill: %s | Available: %s\n",
                temp->name,
                temp->skill,
                temp->available ? "Yes" : "No");
        strcat(buffer, line);
        temp = temp->next;
    }

    print_output(buffer);
}

void display_tasks(GtkWidget *widget, gpointer data) {
    char buffer[1024] = "--- Tasks ---\n";
    Task* temp = taskHead;

    if (!temp) {
        strcat(buffer, "No tasks available.\n");
    }

    while (temp) {
        char line[256];
        sprintf(line,
                "Task: %s | Skill: %s | Urgency: %d\n",
                temp->name,
                temp->requiredSkill,
                temp->urgency);
        strcat(buffer, line);
        temp = temp->next;
    }

    print_output(buffer);
}

void match_volunteers(GtkWidget *widget, gpointer data) {
    char buffer[1500] = "--- Volunteer Matches ---\n";
    Task* t = taskHead;

    if (!t) {
        strcat(buffer, "No tasks to match.\n");
    }

    while (t) {
        char line[256];
        sprintf(line,
                "\nTask: %s (Skill: %s)\n",
                t->name,
                t->requiredSkill);
        strcat(buffer, line);

        Volunteer* v = volunteerHead;
        int found = 0;

        while (v) {
            if (v->available &&
                strcmp(v->skill, t->requiredSkill) == 0) {
                strcat(buffer, "-> ");
                strcat(buffer, v->name);
                strcat(buffer, " is suitable\n");
                found = 1;
            }
            v = v->next;
        }

        if (!found) {
            strcat(buffer, "No volunteer available\n");
        }

        t = t->next;
    }

    print_output(buffer);
}


void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background-color: #f5f5f5; }\n"
        "button { background: #4CAF50; color: white; font-weight: bold; }\n"
        "entry { padding: 6px; }\n"
        "textview { background: white; }\n",
        -1, NULL);

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
}
/* ---------- MAIN ---------- */

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    load_css();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),
                         "Volunteer Skill Matcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 650, 500);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(window), grid);

    /* Volunteer inputs */
    vol_name = gtk_entry_new();
    vol_skill = gtk_entry_new();
    vol_avail = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(vol_name),
                                   "Volunteer Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(vol_skill),
                                   "Skill");
    gtk_entry_set_placeholder_text(GTK_ENTRY(vol_avail),
                                   "Available (1 or 0)");

    /* Task inputs */
    task_name = gtk_entry_new();
    task_skill = gtk_entry_new();
    task_urgency = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(task_name),
                                   "Task Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(task_skill),
                                   "Required Skill");
    gtk_entry_set_placeholder_text(GTK_ENTRY(task_urgency),
                                   "Urgency (1-5)");

    /* Buttons */
    GtkWidget *btn_add_vol =
        gtk_button_new_with_label("Add Volunteer");
    GtkWidget *btn_add_task =
        gtk_button_new_with_label("Add Task");
    GtkWidget *btn_show_vol =
        gtk_button_new_with_label("Display Volunteers");
    GtkWidget *btn_show_task =
        gtk_button_new_with_label("Display Tasks");
    GtkWidget *btn_match =
        gtk_button_new_with_label("Match Volunteers");

    /* Output box */
    output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(output),
                                GTK_WRAP_WORD_CHAR);

    GtkWidget *scroll =
        gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), output);
    gtk_widget_set_vexpand(scroll, TRUE);

    /* Layout */
    gtk_grid_attach(GTK_GRID(grid), vol_name, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vol_skill, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vol_avail, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_add_vol, 3, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), task_name, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), task_skill, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), task_urgency, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_add_task, 3, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), btn_show_vol, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_show_task, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_match, 2, 2, 2, 1);

    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 3, 4, 1);


    /* Signals */
    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_add_vol, "clicked",
                     G_CALLBACK(add_volunteer), NULL);
    g_signal_connect(btn_add_task, "clicked",
                     G_CALLBACK(add_task), NULL);
    g_signal_connect(btn_show_vol, "clicked",
                     G_CALLBACK(display_volunteers), NULL);
    g_signal_connect(btn_show_task, "clicked",
                     G_CALLBACK(display_tasks), NULL);
    g_signal_connect(btn_match, "clicked",
                     G_CALLBACK(match_volunteers), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
