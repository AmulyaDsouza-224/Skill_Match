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

GtkWidget *volunteer_list_box;
GtkWidget *task_list_box;
GtkWidget *match_text_view;
GtkTextBuffer *match_buffer;

/* ---------- CSS STYLING ---------- */

void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    
    const char *css_data = 
        "window {"
        "    background-color: #f5f5f5;"
        "    font-family: monospace;"
        "}"
        ".header-bar {"
        "    background: linear-gradient(to bottom, #4a90e2, #357abd);"
        "    color: white;"
        "    padding: 15px;"
        "    font-size: 25px;"
        "    font-weight: bold;"
        "}"
        ".card {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "    margin: 10px;"
        "    box-shadow: 0 2px 4px rgba(0,0,0,0.1);"
        "}"
        ".primary-button {"
        "    background: linear-gradient(to bottom, #4a90e2, #357abd);"
        "    color: white;"
        "    border-radius: 6px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    border: none;"
        "}"
        ".primary-button:hover {"
        "    background: linear-gradient(to bottom, #5ba3f5, #4a90e2);"
        "}"
        ".list-item {"
        "    background-color: #ffffff;"
        "    border-radius: 6px;"
        "    padding: 12px;"
        "    margin: 5px;"
        "    border: 1px solid #e0e0e0;"
        "}"
        ".list-item:hover {"
        "    background-color: #f8f9fa;"
        "    border-color: #4a90e2;"
        "}"
        ".urgency-high {"
        "    color: #e74c3c;"
        "    font-weight: bold;"
        "}"
        ".urgency-medium {"
        "    color: #f39c12;"
        "    font-weight: bold;"
        "}"
        ".urgency-low {"
        "    color: #27ae60;"
        "}"
        "notebook {"
        "    background-color: white;"
        "}"
        "notebook tab {"
        "    padding: 12px 24px;"
        "    font-weight: 600;"
        "}"
        "notebook tab:checked {"
        "    background-color: #4a90e2;"
        "    color: white;"
        "}"
        ".monospace {"
        "    font-family: monospace;"
        "    font-size: 11pt;"
        "}";
    
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

/* ---------- UI UPDATE FUNCTIONS ---------- */

void refresh_volunteer_list() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(volunteer_list_box));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    Volunteer *v = volunteerHead;
    while (v) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(box), 10);
        
        char label_text[200];
        sprintf(label_text, "👤 <b>%s</b> - Skill: %s - %s", 
                v->name, v->skill, v->available ? "✓ Available" : "✗ Unavailable");
        
        GtkWidget *label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), label_text);
        gtk_label_set_xalign(GTK_LABEL(label), 0);
        
        gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_container_add(GTK_CONTAINER(row), box);
        gtk_list_box_insert(GTK_LIST_BOX(volunteer_list_box), row, -1);
        
        v = v->next;
    }
    
    gtk_widget_show_all(volunteer_list_box);
}

void refresh_task_list() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(task_list_box));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    Task *t = taskHead;
    while (t) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(box), 10);
        
        const char *urgency_color = t->urgency >= 4 ? "#e74c3c" : 
                                     t->urgency >= 3 ? "#f39c12" : "#27ae60";
        
        char label_text[250];
        sprintf(label_text, "📋 <b>%s</b> - Requires: %s - Urgency: <span foreground='%s' weight='bold'>%d/5</span>", 
                t->name, t->requiredSkill, urgency_color, t->urgency);
        
        GtkWidget *label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), label_text);
        gtk_label_set_xalign(GTK_LABEL(label), 0);
        
        gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_container_add(GTK_CONTAINER(row), box);
        gtk_list_box_insert(GTK_LIST_BOX(task_list_box), row, -1);
        
        t = t->next;
    }
    
    gtk_widget_show_all(task_list_box);
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

/* ---------- BUTTON CALLBACKS ---------- */

void on_add_volunteer_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget**)data;
    
    const char *name = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *skill = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    int available = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entries[2]));
    
    if (strlen(name) == 0 || strlen(skill) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please fill in all fields!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    add_volunteer_data(name, skill, available);
    refresh_volunteer_list();
    
    gtk_entry_set_text(GTK_ENTRY(entries[0]), "");
    gtk_entry_set_text(GTK_ENTRY(entries[1]), "");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entries[2]), TRUE);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "✓ Volunteer added successfully!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_add_task_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget**)data;
    
    const char *name = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *skill = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    int urgency = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(entries[2]));
    
    if (strlen(name) == 0 || strlen(skill) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please fill in all fields!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    add_task_data(name, skill, urgency);
    refresh_task_list();
    
    gtk_entry_set_text(GTK_ENTRY(entries[0]), "");
    gtk_entry_set_text(GTK_ENTRY(entries[1]), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(entries[2]), 3);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "✓ Task added successfully!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_match_volunteers_clicked(GtkWidget *widget, gpointer data) {
    if (!match_buffer) return;
    
    gtk_text_buffer_set_text(match_buffer, "", -1);
    
    Task *t = taskHead;
    int total_tasks = 0, matched_tasks = 0;
    
    if (!t) {
        gtk_text_buffer_set_text(match_buffer, "No tasks available. Please add tasks first.\n", -1);
        return;
    }
    
    GtkTextIter iter;
    
    while (t) {
        total_tasks++;
        char header[300];
        sprintf(header, "================================\nTask: %s\n   Required Skill: %s | Urgency: %d/5\n================================\n\n", 
                t->name, t->requiredSkill, t->urgency);
        
        gtk_text_buffer_get_end_iter(match_buffer, &iter);
        gtk_text_buffer_insert(match_buffer, &iter, header, -1);
        
        Volunteer *v = volunteerHead;
        int found = 0;
        
        while (v) {
            if (v->available && strcmp(v->skill, t->requiredSkill) == 0) {
                char line[150];
                sprintf(line, "   -> %s is available and suitable\n", v->name);
                gtk_text_buffer_get_end_iter(match_buffer, &iter);
                gtk_text_buffer_insert(match_buffer, &iter, line, -1);
                found = 1;
            }
            v = v->next;
        }
        
        if (!found) {
            gtk_text_buffer_get_end_iter(match_buffer, &iter);
            gtk_text_buffer_insert(match_buffer, &iter, 
                "   -> No suitable Participants found\n", -1);
        } else {
            matched_tasks++;
        }
        
        gtk_text_buffer_get_end_iter(match_buffer, &iter);
        gtk_text_buffer_insert(match_buffer, &iter, "\n", -1);
        t = t->next;
    }
    
    char summary[200];
    sprintf(summary, "\n================================\nSummary: %d/%d tasks have available volunteers\n================================\n", 
            matched_tasks, total_tasks);
    gtk_text_buffer_get_end_iter(match_buffer, &iter);
    gtk_text_buffer_insert(match_buffer, &iter, summary, -1);
}

/* ---------- TAB BUILDERS ---------- */

GtkWidget* build_volunteer_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    // Input section
    GtkWidget *input_frame = gtk_frame_new("Add New Participant");
    gtk_widget_set_name(input_frame, "card");
    GtkWidget *input_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_grid), 15);
    
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *skill_label = gtk_label_new("Skill:");
    GtkWidget *avail_label = gtk_label_new("Available:");
    
    gtk_label_set_xalign(GTK_LABEL(name_label), 0);
    gtk_label_set_xalign(GTK_LABEL(skill_label), 0);
    gtk_label_set_xalign(GTK_LABEL(avail_label), 0);
    
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *skill_entry = gtk_entry_new();
    GtkWidget *avail_check = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(avail_check), TRUE);
    
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter Participant name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(skill_entry), "Enter skill (e.g., Programming, Design)");
    
    static GtkWidget *volunteer_entries[3];
    volunteer_entries[0] = name_entry;
    volunteer_entries[1] = skill_entry;
    volunteer_entries[2] = avail_check;
    
    GtkWidget *add_btn = gtk_button_new_with_label("Add Participant");
    gtk_widget_set_name(add_btn, "primary-button");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_volunteer_clicked), volunteer_entries);
    
    gtk_grid_attach(GTK_GRID(input_grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), name_entry, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(input_grid), skill_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), skill_entry, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(input_grid), avail_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), avail_check, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), add_btn, 2, 2, 1, 1);
    
    gtk_widget_set_hexpand(name_entry, TRUE);
    gtk_widget_set_hexpand(skill_entry, TRUE);
    
    gtk_container_add(GTK_CONTAINER(input_frame), input_grid);
    gtk_box_pack_start(GTK_BOX(vbox), input_frame, FALSE, FALSE, 0);
    
    // List section
    GtkWidget *list_frame = gtk_frame_new("Current Participants");
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    volunteer_list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(volunteer_list_box), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(scroll), volunteer_list_box);
    gtk_container_add(GTK_CONTAINER(list_frame), scroll);
    
    gtk_box_pack_start(GTK_BOX(vbox), list_frame, TRUE, TRUE, 0);
    
    return vbox;
}

GtkWidget* build_task_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    // Input section
    GtkWidget *input_frame = gtk_frame_new("Add New Task");
    GtkWidget *input_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_grid), 15);
    
    GtkWidget *name_label = gtk_label_new("Task Name:");
    GtkWidget *skill_label = gtk_label_new("Required Skill:");
    GtkWidget *urg_label = gtk_label_new("Urgency:");
    
    gtk_label_set_xalign(GTK_LABEL(name_label), 0);
    gtk_label_set_xalign(GTK_LABEL(skill_label), 0);
    gtk_label_set_xalign(GTK_LABEL(urg_label), 0);
    
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *skill_entry = gtk_entry_new();
    GtkWidget *urg_spin = gtk_spin_button_new_with_range(1, 5, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(urg_spin), 3);
    
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter task name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(skill_entry), "Required skill");
    
    static GtkWidget *task_entries[3];
    task_entries[0] = name_entry;
    task_entries[1] = skill_entry;
    task_entries[2] = urg_spin;
    
    GtkWidget *add_btn = gtk_button_new_with_label("Add Task");
    gtk_widget_set_name(add_btn, "primary-button");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_task_clicked), task_entries);
    
    gtk_grid_attach(GTK_GRID(input_grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), name_entry, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(input_grid), skill_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), skill_entry, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(input_grid), urg_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), urg_spin, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(input_grid), add_btn, 2, 2, 1, 1);
    
    gtk_widget_set_hexpand(name_entry, TRUE);
    gtk_widget_set_hexpand(skill_entry, TRUE);
    
    gtk_container_add(GTK_CONTAINER(input_frame), input_grid);
    gtk_box_pack_start(GTK_BOX(vbox), input_frame, FALSE, FALSE, 0);
    
    // List section
    GtkWidget *list_frame = gtk_frame_new("Current Tasks");
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    task_list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(task_list_box), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(scroll), task_list_box);
    gtk_container_add(GTK_CONTAINER(list_frame), scroll);
    
    gtk_box_pack_start(GTK_BOX(vbox), list_frame, TRUE, TRUE, 0);
    
    return vbox;
}

GtkWidget* build_match_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *match_btn = gtk_button_new_with_label("Match Participants to Tasks");
    gtk_widget_set_name(match_btn, "primary-button");
    g_signal_connect(match_btn, "clicked", G_CALLBACK(on_match_volunteers_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), match_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_box, FALSE, FALSE, 0);
    
    GtkWidget *frame = gtk_frame_new("Matching Results");
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    match_text_view = gtk_text_view_new();
    match_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(match_text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(match_text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(match_text_view), GTK_WRAP_WORD);
    
    // Set monospace font via CSS instead of deprecated function
    GtkStyleContext *context = gtk_widget_get_style_context(match_text_view);
    gtk_style_context_add_class(context, "monospace");
    
    gtk_container_add(GTK_CONTAINER(scroll), match_text_view);
    gtk_container_add(GTK_CONTAINER(frame), scroll);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
    
    return vbox;
}

/* ---------- MAIN ---------- */

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    apply_css();
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Skill Match");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), 
        "<span size='x-large' weight='bold'>Skill Match</span>");
    gtk_widget_set_name(header, "header-bar");
    gtk_widget_set_size_request(header, -1, 60);
    gtk_box_pack_start(GTK_BOX(main_vbox), header, FALSE, FALSE, 0);
    
    // Notebook with tabs
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                             build_volunteer_tab(), 
                             gtk_label_new("Participants"));
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                             build_task_tab(), 
                             gtk_label_new("Tasks"));
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                             build_match_tab(), 
                             gtk_label_new("Match"));
    
    gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(window);
    gtk_main();
    
    return 0;
}