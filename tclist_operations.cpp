#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>


// #define AUTO_VERIFY
#define AUTO_DUMP


typedef double etype_t;
#define TYPE_SPEC "%10lg"


const ssize_t DEFAULT_LENGTH = 10;
const etype_t PZN = (etype_t) 0xD01B0EB;


struct elem_t {
    
    ssize_t next;
    ssize_t prev;
    etype_t value;

};


struct tclist_t {

    elem_t* data;
    ssize_t list_size;
    ssize_t data_size;
    ssize_t next_free;

};


void tc_init(tclist_t* thelist, ssize_t list_lenght);
etype_t tc_pop(tclist_t* thelist, ssize_t position);
etype_t tc_pop_first(tclist_t* thelist);
etype_t tc_pop_last(tclist_t* thelist);
ssize_t tc_get_next(tclist_t* thelist);
void tc_insert_index(tclist_t* thelist, etype_t value, ssize_t position);
void tc_insert_first(tclist_t* thelist, etype_t value);
void tc_insert_last(tclist_t* thelist, etype_t value);
void tc_insert_number(tclist_t* thelist, etype_t value, ssize_t position);
void tc_dump_terminal(tclist_t* thelist);
bool tc_verify(tclist_t* thelist);

void tc_graphviz_dump(tclist_t* thelist, bool in_queue);
void tc_make_graph(tclist_t* thelist, bool in_queue);
void form_header(FILE* graph_file, tclist_t* thelist);
void horizon_straight(FILE* graph_file, tclist_t* thelist);
void form_blocks(FILE* graph_file, tclist_t* thelist);
void blocks_in_line(FILE* graph_file, tclist_t* thelist);
void form_arrows(FILE* graph_file, tclist_t* thelist);
bool is_free_node(tclist_t* thelist, ssize_t index);
void draw_an_arrow(FILE* graph_file, tclist_t* thelist, ssize_t pos);
void tc_make_html(tclist_t* thelist);
void tc_make_table(tclist_t* thelist, char* table_name, FILE* graph_html);
void paste_pngs(char* image_1, char* image_2, ssize_t counter, FILE* graph_file);


void tc_init(tclist_t* thelist, ssize_t list_lenght = DEFAULT_LENGTH) {

    assert(thelist != NULL);
    assert(list_lenght > 0);
    assert(thelist->data == NULL);
    
    thelist->data = (elem_t*) calloc (1 + list_lenght, sizeof(elem_t));
    if (thelist->data == NULL) {
        printf("Error!\nNot enough memory!\n");
        return;
    }
    
    thelist->data[0] = { .next = 0, .prev = 0, .value = PZN };
    thelist->list_size = list_lenght;
    thelist->data_size = 0;
    thelist->next_free = 1;
    
    for (ssize_t counter = 1; counter <= thelist->list_size; counter++) {
        
        thelist->data[counter].value = PZN;
        thelist->data[counter].next = (counter == thelist->list_size) ? 0 : counter + 1;
        thelist->data[counter].prev = -1;
        
    }

    return;
    
}


etype_t tc_pop(tclist_t* thelist, ssize_t position) {

    assert(tc_verify(thelist));
    assert(0 < position && position <= thelist->list_size);
    assert(thelist->data[position].prev != -1);

    ssize_t next_e = thelist->data[position].next;
    ssize_t prev_e = thelist->data[position].prev;
    thelist->data[prev_e].next = next_e;
    thelist->data[next_e].prev = prev_e;

    etype_t theresult = thelist->data[position].value;
    thelist->data[position].value = PZN;
    thelist->data[position].next = thelist->next_free;  
    thelist->data[position].prev = -1;  
    thelist->next_free = position;

    thelist->data_size--;

    return theresult;

}


etype_t tc_pop_first(tclist_t* thelist) {

    ssize_t position = thelist->data[0].next;
    return tc_pop(thelist, position);

}


etype_t tc_pop_last(tclist_t* thelist) {

    ssize_t position = thelist->data[0].prev;
    return tc_pop(thelist, position);

}


ssize_t tc_get_next(tclist_t* thelist) {

    ssize_t res = thelist->next_free;
    if (res <= 0 || res > thelist->list_size) {
        printf("No free place\n");
        return -1;
    }

    ssize_t next_e = thelist->data[res].next;
    thelist->next_free = next_e;

    return res;
    
}


void tc_insert_index(tclist_t* thelist, etype_t value, ssize_t position) {

    assert(tc_verify(thelist));
    assert(0 <= position && position <= thelist->list_size);
    
    elem_t* prev_element = &(thelist->data[position]);
    elem_t* next_element = &(thelist->data[prev_element->next]);

    if (prev_element->prev == -1) {
        printf("This element is empty\n");
        return;
    }
    
    ssize_t new_element_index = tc_get_next(thelist);
    if (new_element_index == -1) {
        return;
    }

    elem_t* new_element = &(thelist->data[new_element_index]);
    
    new_element->next = prev_element->next;
    new_element->prev = next_element->prev;
    prev_element->next = new_element_index;
    next_element->prev = new_element_index;
    
    new_element->value = value;

    thelist->data_size++;

    return;
    
}


void tc_insert_first(tclist_t* thelist, etype_t value) {

    tc_insert_index(thelist, value, 0);
    return;

}


void tc_insert_last(tclist_t* thelist, etype_t value) {

    ssize_t listlast = thelist->data[0].prev;
    tc_insert_index(thelist, value, listlast);
    return;

}


void tc_insert_number(tclist_t* thelist, etype_t value, ssize_t position) {

    assert(thelist != NULL && thelist->data != NULL);
    assert(position >= 0 && position <= thelist->data_size);

    ssize_t element_index = 0;
    for (ssize_t index = 0; index < position; index++) {
        element_index = thelist->data[element_index].next;
    }

    tc_insert_index(thelist, value, element_index);
    return;

}


void tc_dump_terminal(tclist_t* thelist) {

    assert(tc_verify(thelist));

    printf("\nList size: %4zd\n\n", thelist->list_size);
    for (ssize_t index = 0; index <= thelist->list_size; index++) {
        
        elem_t dur_element = (thelist->data)[index];
        printf(
            "[%3zd]\t=>\tValue: " TYPE_SPEC " ; Next: %4zd ; Prev: %4zd\n",
            index, dur_element.value, dur_element.next, dur_element.prev
        );
        
    }
    
    printf("\nData-size: %4zd\n", thelist->data_size);
    printf("\nNext free: %4zd\n\n\n", thelist->next_free);

    return;
    
}


bool tc_verify(tclist_t* thelist) {

#ifdef AUTO_DUMP
    tc_make_html(thelist);
#endif // AUTO_DUMP

#ifndef AUTO_VERIFY
    return true;
#endif // AUTO_VERIFY

    if (thelist == NULL || thelist->data == NULL) {
        return false;
    }

    if (thelist->list_size <= 0 || thelist->list_size < thelist->next_free || thelist->data_size) {
        return false;
    }

    ssize_t element_index = 0;
    for (ssize_t index = 0; index < thelist->data_size; index++) {
        element_index = thelist->data[element_index].next;
    }
    if (thelist->data[element_index].next != 0) {
        return false;
    }

    element_index = 0;
    for (ssize_t index = 0; index < thelist->data_size; index++) {
        if (thelist->data[element_index].prev == -1) {
            return false;
        }
        element_index = thelist->data[element_index].prev;
    }
    if (thelist->data[element_index].prev != 0) {
        return false;
    }

    element_index = thelist->next_free;
    ssize_t free_size = thelist->list_size - thelist->data_size;
    for (ssize_t index = 1; index < free_size; index++) {
        if (thelist->data[element_index].prev != -1) {
            return false;
        }
        element_index = thelist->data[element_index].next;
    }
    if (thelist->next_free == 0 and free_size != 0) {
        return false;
    }

    return true;

}
