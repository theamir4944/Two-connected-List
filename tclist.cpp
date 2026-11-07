#include "tclist_operations.cpp"


const size_t MAX_COMAND_LEN = 50;
const size_t MAX_TABLE_NAME = 50;
ssize_t GLOBAL_PICT_COUNTER = 0; // Number of pictures
ssize_t GLOBAL_DUMP_COUNTER = 0; // Number of dumps
#define TYPE_SPEC "%10lg"


void tc_graphviz_dump(tclist_t* thelist, bool in_queue) {

    tc_make_graph(thelist, in_queue);
    char comand_string[MAX_COMAND_LEN] = { };
    sprintf(comand_string, "dot -Tpng graph.dot -o .\\debug_pictures\\graph%zd.png", GLOBAL_PICT_COUNTER);
    system(comand_string);

    return;

}


void tc_make_graph(tclist_t* thelist, bool in_queue) {

    FILE* graph_file = fopen("graph.dot", "w");
    GLOBAL_PICT_COUNTER++;
    assert(graph_file != NULL);

    fprintf(graph_file, "digraph {\n\n");
    
    form_header(graph_file, thelist);

    horizon_straight(graph_file, thelist);

    if (in_queue) { blocks_in_line(graph_file, thelist); }

    form_blocks(graph_file, thelist);

    form_arrows(graph_file, thelist);

    fprintf(graph_file, "\n}");

    fclose(graph_file);

    return;

}


void form_header(FILE* graph_file, tclist_t* thelist) {

    fprintf(graph_file, "rankdir = TB\n");
    fprintf(graph_file, "node [shape = Mrecord, fontcolor=\"black\", style=\"filled\"]\n");
    fprintf(graph_file, "splines = ortho;\n");
    fprintf(graph_file, "nodesep = 1.0;\n");
    fprintf(graph_file, "ranksep = 1.0;\n");

}


void horizon_straight(FILE* graph_file, tclist_t* thelist) {

    fprintf(graph_file, "{ rank = same;");
    for (ssize_t index = 0; index <= thelist->list_size; index++) {
        fprintf(graph_file, " \"node%zd\";", index);
    }
    fprintf(graph_file, "}\n");

}


void form_blocks(FILE* graph_file, tclist_t* thelist) {

    const char* Color = NULL;
    const char* Fillcolor = NULL;

    for (ssize_t index = 0; index <= thelist->list_size; index++) {

        if (index == 0) { 
            Color = "orange"; 
            Fillcolor = "lightyellow"; 
        }
        else if (thelist->data[index].prev == -1) { 
            Color = "green"; 
            Fillcolor = "lightgreen"; 
        }
        else if (index == thelist->data[0].prev) { 
            Color = "red"; 
            Fillcolor = "lightpink"; 
        }
        else if (index == thelist->data[0].next) { 
            Color = "blue"; 
            Fillcolor = "lightblue"; 
        }
        else { 
            Color = "gray"; 
            Fillcolor = "lightgray"; 
        }
        
        fprintf(graph_file, 
            "\"node%zd\" [label = \"{index: %zd | value: " TYPE_SPEC " | { next: %zd | prev: %zd }}\", color=\"%s\", fillcolor=\"%s\"]\n", 
            index, index, thelist->data[index].value, thelist->data[index].next, thelist->data[index].prev, Color, Fillcolor);
            
    }

    fprintf(graph_file, "trash [label = \"TRASH\", color=\"black\", fillcolor=\"white\"]\n");

    return;

}


void blocks_in_line(FILE* graph_file, tclist_t* thelist) {

    for (ssize_t index = 0; index <= thelist->list_size - 1; index++) {
        fprintf(graph_file, "\"node%zd\" -> \"node%zd\" [style = invis];\n", index, index + 1);
    }

    return;

}


void form_arrows(FILE* graph_file, tclist_t* thelist) {

    for (ssize_t index = 0; index <= thelist->list_size; index++) {
        draw_an_arrow(graph_file, thelist, index);
    }

    return;

}


bool is_free_node(tclist_t* thelist, ssize_t index) {

    return (index != 0 && thelist->data[index].prev == -1);

}


void draw_an_arrow(FILE* graph_file, tclist_t* thelist, ssize_t pos) {

    ssize_t next_pos = thelist->data[pos].next;
    ssize_t prev_pos = thelist->data[pos].prev;

    bool curr_free = is_free_node(thelist, pos);
    bool next_free = is_free_node(thelist, next_pos);
    bool prev_free = is_free_node(thelist, prev_pos);
    const char* color = NULL;

    bool missed_1 = (!curr_free && !next_free && thelist->data[next_pos].prev != pos);
    bool missed_2 = (!curr_free && !prev_free && thelist->data[prev_pos].next != pos);
    bool missed_3 = (next_pos < 0 || next_pos > thelist->list_size);

    if (missed_1 || missed_2 || missed_3) {
        color = "red";
    }
    else if (pos != 0 && thelist->data[pos].prev == -1) {
        color = "green";
    }
    else {
        color = "black";
    }

    if (color == "green") {
        fprintf(
                graph_file,
                "\"node%zd\" -> \"node%zd\" [color=\"%s\"];\n",
                pos, next_pos, color
                );
    }
    else if (color == "black") {
        fprintf(
                graph_file,
                "\"node%zd\" -> \"node%zd\" [dir = both, color=\"%s\"];\n",
                pos, next_pos, color
                );
    }
    else if (missed_3) {
        fprintf(
                graph_file,
                "\"node%zd\" -> trash [color=\"%s\"];\n",
                pos, color
                );
    }
    else if (missed_1) {
        fprintf(
                graph_file,
                "\"node%zd\" -> \"node%zd\" [color=\"%s\"];\n",
                pos, next_pos, color
                );
    }
    else if (missed_2) {
        fprintf(
                graph_file,
                "\"node%zd\" -> \"node%zd\" [color=\"%s\"];\n",
                prev_pos, pos, color
                );
    }

    return;

}


#define htmprintf(...) fprintf(graph_html, ##__VA_ARGS__)

void tc_make_html(tclist_t* thelist) {

    tc_graphviz_dump(thelist, true);
    tc_graphviz_dump(thelist, false);
    
    char table_name[MAX_TABLE_NAME] = { };
    sprintf(table_name, "Dump number %zd", GLOBAL_DUMP_COUNTER + 1);
    
    FILE* graph_html = (GLOBAL_DUMP_COUNTER) ? fopen("graph.htm", "a") : fopen("graph.htm", "w");
    assert(graph_html != NULL);
    
    htmprintf("<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n");
    htmprintf("<style>\n");
    htmprintf("table { border-collapse: collapse; }\n");
    htmprintf("table, th, td { border: 3px solid black; }\n");
    htmprintf("th, td { padding: 2px 8px; }\n");
    
    htmprintf(".row-fictive { background-color: #ffff00; color: #010000; }\n");
    htmprintf(".row-head    { background-color: #ff0000; color: #010000; }\n");
    htmprintf(".row-tail    { background-color: #0000ff; color: #010000; }\n");
    htmprintf(".row-free    { background-color: #00ff00; color: #010000; }\n");
    htmprintf(".row-just    { background-color: #adacac; color: #010000; }\n");
    
    htmprintf("</style>\n</head>\n<body>\n");

    tc_make_table(thelist, table_name, graph_html);

    char image_1[MAX_COMAND_LEN] = { };
    char image_2[MAX_COMAND_LEN] = { };

    paste_pngs(image_1, image_2, GLOBAL_DUMP_COUNTER, graph_html);
    htmprintf("<hr style=\"border: 2px dashed #000000; margin: 100px 0;\">\n");
    GLOBAL_DUMP_COUNTER++;
    
    htmprintf("</body>\n</html>\n");
    fclose(graph_html);

    return;
    
}


void tc_make_table(tclist_t* thelist, char* table_name, FILE* graph_html) {
    htmprintf("\n<h3>%s</h3>\n", table_name);
    htmprintf("<table>\n");
    htmprintf("<thead><tr><th>Index</th><th>Value</th><th>Next</th><th>Prev</th><th>Status</th></tr></thead>\n");
    htmprintf("<tbody>\n");
    const char* status = NULL;
    const char* row_class = NULL;
    for (ssize_t index = 0; index <= thelist->list_size; index++) {

        if (index == 0) { 
            status = "FICTIVE"; 
            row_class = "row-fictive";
        }
        else if (index == thelist->data[0].prev) { 
            status = "HEAD"; 
            row_class = "row-head";
        }
        else if (index == thelist->data[0].next) { 
            status = "TAIL"; 
            row_class = "row-tail";
        }
        else if (thelist->data[index].prev == -1) { 
            status = "FREE"; 
            row_class = "row-free";
        }
        else { 
            status = "JUST ELEMENT"; 
            row_class = "row-just";
        }
        
        htmprintf("<tr class=\"%s\">", row_class);
        htmprintf("<td>%zd</td><td>" TYPE_SPEC "</td><td>%zd</td><td>%zd</td><td>%s</td>",
                  index, thelist->data[index].value, thelist->data[index].next, thelist->data[index].prev,
                  status);
        htmprintf("</tr>");

    }

    htmprintf("</tbody>\n");
    htmprintf("</table>");

}

#undef htmprintf


void paste_pngs(char* image_1, char* image_2, ssize_t counter, FILE* graph_file) {

    sprintf(image_1, ".\\debug_pictures\\graph%zd.png", 2 * GLOBAL_DUMP_COUNTER + 1);
    sprintf(image_2, ".\\debug_pictures\\graph%zd.png", 2 * GLOBAL_DUMP_COUNTER + 2);

    fprintf(graph_file, "<img src=\"%s\" alt=\"image\" />", image_1);
    fprintf(graph_file, "<img src=\"%s\" alt=\"image\" />", image_2);

}
