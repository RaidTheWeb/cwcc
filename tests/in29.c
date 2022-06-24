#include "../include/std.h"

struct foo {
    int val;
    struct foo *next;
};

struct foo head, mid, tail;

int main() {
    struct foo *ptr;
    tail.val = 20;
    tail.next = NULL;
    mid.val = 15;
    mid.next = &tail;
    head.val = 10;
    head.next= &mid;

    ptr = &head;
    printint(head.val);
    printchar(' ');
    puti(ptr->val);
    printint(mid.val);
    printchar(' ');
    puti(ptr->next->val);
    printint(tail.val);
    printchar(' ');
    puti(ptr->next->next->val); 
    return(0);
}
