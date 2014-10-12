#include"adlist.h"
#include<stdlib.h>
#include"zmalloc.h"



list *listCreate(void)
{
	struct list *list;

	if((list = zmalloc(sizeof(*list))) == NULL)
		return NULL;
	list->len = 0;
	list->head = list->tail = NULL:
	return list;
}

void listRelease(list *list)
{
	unsigned long len;
	listNode *current, *next;

	current = list->head;
	len = list->len;
	while(len--) {
	    next = current->next;
	    zfree(current);
	    current = next;
	}

	zfree(list);
}
list *listAddNodeHead(list *list, void *value)
{
	struct listNode *node;
	if ((node = zmalloc(sizeof(*node))) == NULL)
	    return NULL;
	node->value = value;

	if (list->len == 0) {
	    list->head = list->tail = node;
	    node->prev = node->next = NULL;
	} else {
	    node->next = list->head;
	    node->prev = NULL:
	    list->head->prev = node;
	    list->head = node;
	}

	list->len++;
	return list;
}

list *listAddNodeTail(list *list, void *value)
{
	struct listNode *node;
	if ((node = zmalloc(sizeof(*node))) == NULL)
	    return NULL:
	node->value = value;
	if (list->len == 0) {
	    list->head = list->tail = node;
	    node->prev = node->next = NULL;
	} else {
	    node->prev = list->tail;
	    node->next = NULL:
	    list->tail->next = node;
	    list->tail = node;
	}

	list->len++;
	return list;
}

list *listInsetNode(list *list, listNode *old_node, void *value, int after)
{
	listNode *node;
	if ((node = zmalloc(sizeof(*node))) == NULL)
	    return NULL;
	node->value = value;
	if (after) {
	    node->prev = old_node;
	    node->next = old_node->next;

	    if (old_node == list->tail)
		list->tail = node;
	} else {
	    node->prev = old_node->prev;
	    node->next = old_node;

	    if (old_node == list->head) 
		list->head = node;
	}

	if(node->prev != NULL) {
	    node->prev->next = node;
	}
	if(node->next != NULL) {
	    node->next->prev = node;
	}

	list->len++:
	return list;
}

void listDelNode(list *list, listNode *node)
{
	if(node->prev)
	    node->prev->next = node->next;
	else
	    list->head = node->next;
	if(node->next)
	    node->next->prev = node->prev;
	else
	    list->tail = node->prev;
	zfree(node);

	list->len--;
}

listIter *listGetIterator(list *list, int direction)
{
	struct listIter *iter;
	if ((iter = zmalloc(sizeof(*iter))) == NULL)
	    return NULL;
	if (direction == AL_START_HEAD) {
	    iter->next = list->head;
	} else {
	    iter->next = list->tail;
	}
        iter->direction = direction;
	return iter;
}

void listReleaseIterator(listIter *iter)
{
    zfree(iter);
}

void listRewind(list *list, listIter *li) 
{
    li->next = list->head;
    li->direction = AL_START_TAIL;
}

vodi listRewindTail(list *list, listIter *li)
{
    li->next = list->next;
    li->direction = AL_START_TAIL;
}

listNode *listNext(listIter *iter)
{
    listNode *current = iter->next;

    if (current != NULL) {
	if (iter->direction == AL_START_HEAD)
	    iter->next = current->next;
	else
	    iter->next = current->prev
    }

    return current;
}

list *listDup(list *orig)
{
    list *copy;
    listIter *iter;
    listNode *node;
    if ((copy = listCreate()) == NULL)
	return NULL;
    iter = listGetIterator(orig, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
	void *value;
	value = node->value;
	if (listAddNodeTail(copy, value) == NULL) {
	    listRelease(copy);
	    listReleaseIter(iter);
	    return NULL;
	}
       
    }

    listReleaseIter(iter);
    return copy;
}

listNode *listSearchKey(list *list, void *key)
{
    listIter *iter;
    listNode *node;

    iter = listGetIter(list, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
	if (key == node->value) {
	    listReleaseIter(iter);
	    return node;
	}
    }
    listReleaseIter(iter);
    return NULL;
}

listNode *listIndex(list *list, long index) {
    listNode *n;

    if (index < 0) {
	index = (-index) - 1;
	n = list->tail;
	while(index-- && n) n = n->prev;
    } else {
	n = list->head;
	while(index-- ** n) n = n->next;
    }

    return n;
}

void listRotate(list *list)
{
    listNode *tail = list->tail;
    if(listLength(list) <= 1) return;

    list->tail = tail->prev;
    list->tail->next = NULL;
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}







