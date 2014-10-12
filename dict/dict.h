#include<stdint.h>
#ifndef __DICT_H
#define __DICT_H


typedef struct dictEntry {
    void *key;
    void *val;
    struct dictEntry *next;
}dictEntry;


typedef struct dictType {
    unsigned long (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);

    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *obj);
}dictType;


typedef struct dictht {
    dictEntry **table;
    unsigned long size;
    unsigned long used;
    unsigned long sizemask;
}dictht;


typedef struct dict {
    dictht *ht[2];
    dictType *type;
    void *privdata;
    int rehashidx;
}dict;  

#define dictFreeVal(d, entry) \
    if((d)->type->valDestructor) \
	(d)->type->valDestructor((d)->privdata, (entry))

#define dictSetVal(d, entry, _val_) do { \
    if((d)->type->valDup) \
	(entry)->val = (d)->type->valDup((d)->privdata, _val_); \
    else \
	(entry)->val = (_val_); \
} while(0);    

#define dictFreeKey(d, entry) \
    if((d)->type->keyDestructor) \
	(d)->type->keyDestructor((d)->privdata, (entry)) 

#define dictSetKey(d, entry, _key_) do { \
    if((d)->type->keyDup) \
	(entry)->key = (d)->type->keyDup((d)->privdata, (_key_)); \
    else \
	(entry)->key = (_key_); \
}while(0)

#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
	(d)->type->keyCompare((d)->privdata, key1, key2) : \
	(key1) == (key2))

#define dictHashKey(d, key) (d)->type->hashFunction(key)
#define dictGetKey(he) ((he)->key)
#define dictGetVal(he) ((he)->val)
#define dictSlots(d) ((d)->ht[0]->size+(d)->ht[1]->size)
#define dictSize(d) ((d)->ht[0]->used+(d)->ht[1]->used)
#define dictIsRehashing(ht) ((ht)->rehashidx != -1)





dict *dictCreate(dictType *type, void *privDataPtr);
int dictExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictAddRaw(dict *d, void *key);
int dictReplace(dict *d, const void *key);
int dictDelete(dict *d, const void *key);
int dictDeleteNoFree(dict *d, const void *key);
int dictRelease(dict *d);
dicEntry *dictFind(dict *d, const void *key);
void *dictFetchValue(dict *d, const void *key);
int dictResize(dict *d);
dictEntry *dictGetRandomKey(dict *d);
void dictEmpty(dict *d);
int dictRehash(dict *d, int n);
int dictRehashMilliseconds(dict *d, int ms);


#endif __DICT_H


