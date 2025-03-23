#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Convert a list_head pointer to its containing element_t pointer */
static element_t *list_to_element(struct list_head *pos)
{
    if (!pos)
        return NULL;
    return (element_t *) ((char *) pos - offsetof(element_t, list));
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_list = malloc(sizeof(struct list_head));
    if (!new_list)
        return NULL;
    new_list->next = new_list;
    new_list->prev = new_list;
    return new_list;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *current = head->next;
    while (current != head) {
        struct list_head *next = current->next;
        element_t *real_pos =
            (element_t *) ((char *) current - offsetof(element_t, list));
        free(real_pos->value);
        free(real_pos);
        current = next;
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    int len = 0;
    while (*(s + len) != '\0') {
        len++;
    }
    len++;
    char *value = malloc(len);
    if (!value)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        free(value);
        return false;
    }
    for (int i = 0; i < len; i++)
        *(value + i) = *(s + i);
    new->value = value;
    new->list.prev = head;
    new->list.next = head->next;
    head->next->prev = &new->list;
    head->next = &new->list;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    int len = 0;
    while (*(s + len) != '\0') {
        len++;
    }
    len++;
    char *value = malloc(len);
    if (!value)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        free(value);
        return false;
    }
    for (int i = 0; i < len; i++)
        *(value + i) = *(s + i);
    new->value = value;
    new->list.next = head;
    new->list.prev = head->prev;
    head->prev->next = &new->list;
    head->prev = &new->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || (head->next == head))
        return NULL;
    head->next->next->prev = head;
    element_t *tmp =
        (element_t *) ((char *) head->next - offsetof(element_t, list));
    head->next = head->next->next;
    tmp->list.next = NULL;
    tmp->list.prev = NULL;
    if (sp) {
        int i = 0;
        while (i < bufsize - 1 && (tmp->value[i] != '\0')) {
            sp[i] = tmp->value[i];
            i++;
        }
        sp[i] = '\0';
    }
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || (head->next == head))
        return NULL;
    head->prev->prev->next = head;
    element_t *tmp =
        (element_t *) ((char *) head->prev - offsetof(element_t, list));
    head->prev = head->prev->prev;
    tmp->list.prev = NULL;
    tmp->list.next = NULL;
    if (sp) {
        int i = 0;
        while (i < bufsize - 1 && (tmp->value[i] != '\0')) {
            sp[i] = tmp->value[i];
            i++;
        }
        sp[i] = '\0';
    }
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head)
        return false;

    struct list_head *fast, *slow;
    fast = slow = head->next;
    while ((fast != head) && (fast->next != head)) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // update pointers
    slow->prev->next = slow->next;
    slow->next->prev = slow->prev;

    // delete the node
    element_t *del = (element_t *) ((char *) slow - offsetof(element_t, list));
    free(del->value);
    free(del);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || head->next == head)
        return false;

    // true for one node
    if (head->next->next == head)
        return true;

    // more than two nodes
    struct list_head *a, *b;
    a = head->next;
    b = head;
    while (a != b) {
        b = a->next;
        element_t *ae = list_to_element(a);
        bool clear = false;
        while (b != head) {
            // compare the strings of a & b
            element_t *be = list_to_element(b);
            if (strcmp(ae->value, be->value) == 0) {
                b->prev->next = b->next;
                b->next->prev = b->prev;
                b = b->next;
                free(be->value);
                free(be);
                clear = true;
            } else
                b = b->next;
        }
        if (clear) {
            a->prev->next = a->next;
            a->next->prev = a->prev;
            a = a->next;
            free(ae->value);
            free(ae);
        } else
            a = a->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || head->next == head)
        return;

    struct list_head *c;
    c = head->next;
    while (c->next != head) {
        c->prev->next = c->next;
        c->next->prev = c->prev;
        c->prev = c->next;
        c->next = c->next->next;
        c->prev->next = c;
        c->next->prev = c;
        if (c->next != head)
            c = c->next;
        else
            break;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
