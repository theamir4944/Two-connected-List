#include "tclist.cpp"


int main() {

    tclist_t main_list = { };

    tc_init(&main_list, 16);

    tc_insert_last(&main_list, 6);
    tc_insert_last(&main_list, 7);
    tc_insert_last(&main_list, 8);
    tc_insert_last(&main_list, 9);
    tc_insert_index(&main_list, 67, 0);
    tc_insert_first(&main_list, 5);
    tc_pop(&main_list, 1);
    tc_insert_number(&main_list, 76, 1);
    tc_insert_number(&main_list, 7556, 0);
    tc_pop_first(&main_list);
    tc_pop_last(&main_list);

    tc_make_html(&main_list);

    return 0;

}
