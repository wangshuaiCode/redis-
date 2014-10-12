#ifndef __AD_LIST_H__
#define __AD_LIST_H__






typedef struct listNode {
	struct listNode *prev;
	struct listNode *next;
	
	void *value;
}listNode;

typedef struct listInter {
	struct listNode *next;
	int direction;
}listIter;

typedef struct list {
	struct listNode *head;
	struct listNode *tail;
	unsigned long len;
}list;

#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

list listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
void listDelete(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);


#define AL_START_HEAD 0
#define AL_START_TAIL 1



#endif
