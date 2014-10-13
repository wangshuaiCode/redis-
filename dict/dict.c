#include "dict.h"
#include "zmalloc.h"
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<sys/time.h>
#include<ctype.h>
#include<string.h>

static int dict_can_resize = 1;


#define DICT_HT_INITIAL_SIZE 4

void _dictReset(dictht *ht)
{
	ht->table = NULL;
	ht->size = 0;
	ht->used = 0;
	ht->sizemask = -1;
}

dict *dictCreate(dictType *type, void *privDataPtr)
{
	dict *d = zmalloc(sizeof(*d));

	_dictInit(d, type, privDataPtr);
	
	return d;
}


void _dictInit(dict *d, dictType *type, void *privDataPtr)
{
	_dictReset(d->ht[0]);
	_dictReset(d->ht[1]);

	d->type = type;
	d->privdata = privDataPtr;
	d->rehashidx = -1;

	return DICT_OK;
}

int dictResize(dict *d)
{
	int minimal;
	if (!dict_can_resize || dictIsRehashing(d)) return DICT_REE;

	minimal = d->ht[0]->used;

	if (minimal < DICT_HT_INITIAL_SIZE)
	    minimal = DICT_HT_INITIAL_SIZE;
	
	return dictExpand(d, minimal);

}

int dictExpand(dict *d, unsigned long size)
{
    unsigned long realsize= _dictNextPower(size);

    dictht n;

    if (dictIsRehashing(d) || d->ht[0]->used > size)
	return DICT_ERR;
    n->table = zcalloc(realsize * sizeof(dictEntry *));
    n->used = 0;
    n->size = realsize;
    n->sizemask = realsize - 1;

    
    if (d->ht[0] == NULL) {
	d->ht[0] = n;
	return DICT_OK;
    }

    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;
    
}

unsigned long _dictNextPower(unsigned long size)
{
	unsigned long realsize = DICT_HT_INITIAL_SIZE;

	if (size > LONG_MAX) return LONG_MAX;
	while(1) {
	    if(realsize >= size) {
		return realsize;
	    }
	    realsize *= 2;
	}

}

int dictRehas(dict *d, int n)
{
    if (!dictIsRehashing(d))
	return 0;

   while(n--) {
	dictEntry *de, *nextde;
	  if (d->ht[0].used == 0) {
	      zfree(d->ht[0].table);
	      d->ht[0] = d->ht[1];
	      _dictReset(d->ht[1]);
	      d->reshidx = -1;
	      return 0; 
          }
    while(d->ht[0].table[d->rehashidx] == NULL) d->rehashidx++;

    de = d->ht[0].table[d->rehashidx];

    while(de) {
	nextde = de->next;

	unsigned int h = dictHashKey(d, de->key) & d->ht[1].sizemask;
	de->next = d->ht[1].table[h];
	d->ht[1].table[h] = de;
	d->ht[0].used--;
	d->ht[1].used++;
	de = nextde;
    }
    d->ht[0].table[rehashidx] = NULL;

    d->rehashidx++;  
   }

    return 1;
}



long long timeInMilliseconds(void) 
{
	struct timeval tv;
	gettimeofday(*tv, NULL);

	return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec/1000);

} 
int dictRehashMilliseconds(dict *d, int ms)
{
	long long start = timeInMilliseconds();

	int rehashes = 0;
	while(dictRehash(d, 100)) {
	    rehashes += 100;
	    if (timeInMilliseconds() - start > ms) break;
	}

	return rehashes;
}

static unsigned int _dictKeyIndex(dict *d, const void *key)
{
	dictEntry *he;
	unsigned int h, table, idx;
	h = dictHashKey(d, key);

	for(table = 0; table <= 1; ++table)
	{
	    idx = h & d->ht[table].sizemask;
	    he = ht[table].table[idx];
	    while(he) {
		if (dictCompareKeys(d, key, he->key))
		    return -1;
		de = de->next;
	    }
	    if (!dictIsRehashing(d)) break;
	}

	return idx;
}


dictEntry *dictAddRaw(dict *d, void *key)
{
	dictEntry *he;
	dictht ht;
	unsigned int idx; 

	if ((idx = _dictKeyIndex(d, key)) == -1)
	    return NULL;
	he = zmalloc(sizeof(dictEntry));
	ht = dictIsRehashing(d) ? ht[1] : ht[0];
	he->next = ht.table[idx];
	ht.table[idx] = he;
	ht.used++;

	dictSetKey(d, he, key);
	return he;

}

int dictAdd(dict *d, void *key, void *value)
{
	dictEntry *entry = dictAddRaw(d, value);
	if (!entry) return DICT_ERR;

	dictSetValue(d, entry, value);
	return DICT_OK;
}

int dictReplace(dict *d, void *key, void *value)
{
	dictEntry *entry, oldentry;
	if (dictAdd(d, key) == DICT_OK)
	    return 1;
	entry = dictFind(d, key);

	oldentry = *entry;
	dictSetValue(d, entry, value);
	dictFreeVal(d, &oldentry);
	return 0;
}

dictEntry *dictFind(dict *d, const void *key)
{
	unsigned int idx, h, table;
	dictEntry *entry;
	if (d->ht[0].size == 0)
	    return NULL:

	h = dictHashKey(d, key);
	for(table = 0; table <= 1; ++table)
	{
	    idx = h & d->ht[table].sizemask;
	    entry = d->ht[table].table[idx];
	    while(entry) {
		if (dictCompareKeys(d, key, entry->key))
		    return entry;
		entry = entry->next;
	    }
	    if (!dictIsRehashing(d)) break;
	}

	return NULL;

	
}

dictEntry *dictReplaceRaw(dict *d, const void *key)
{
	dictEntry *entry = dictFind(d, key);

	return entry ? entry : dictAddRaw(d, key);
}

static ind dictGenericDelete(dict *d, const void *key, int nofree)
{
	dictEntry *entry, *preventry;
	unsigned int h, idx, table;
	if (d->ht[0].size == 0) return DICT_ERR;

	h = dictHashKey(d, key);
	
	for(table = 0; table <= 1; ++table)
	{
	    idx = h & d->ht[table].sizemask;
	    entry = d->ht[table].table[idx];
	    preventry = NULL;
	    while(entry) {
		if (dictCompareKeys(d, key, entry->key)) {
		    if (preventry != NULL) 
			preventry->next = entry->next;
		    else
			d->ht[table].table[idx] = entry->next;
		    if (!nofree) {
			dictFreeVal(d, entry);
			dictFreeKey(d, entry);
		    }
		    zfree(entry);
		    d->ht[table].used--;
		    return DICT_OK;
		}
		preventry = entry;
		entry = entry->next;
	    }

	    if (!dictIsRehashing(d)) break;
	}

	return DICT_ERR;
}

int dictDelete(dict *d, const void *key)
{
	return dictGenericDelete(d, key, 0);
}

int dictDeleteNofree(dict *d, const void *key)
{
	return dictGenericDelete(d, key, 1);
}

int _dictclear(dict *d, dictht *ht)
{
	unsigned long i;
       
	for(i = 0; i < ht->size && ht->used > 0; i++)
	{
                dictEntry *entry, *nextentry; 
		if ((entry=ht->table[i]) == NULL) continue;
		while(entry) {
		    nextentry = entry->next;

		    dictFreeVal(d, entry);
		    dictFreeKey(d, entry);
		    zfree(entry);
		    ht-used--;
		    entry = nextentry;
		    
		}

	}
	zfree(ht->table);
	_dictReset(ht);
	return DICT_OK;
}       


void dictRelease(dict *d)
{
	_dictclear(d, d->ht[0]);
	_dictclear(d, d->ht[1]);

	zfree(d);
}


void *dictFetchValue(dict *d, const void *key)
{
	dictEntry *entry;
	entry = dictFind(d, key);
	return entry ? dictGetVal(entry) : NULL;
}


void dictEmpty(dict *d)
{
	_dictclear(d, &d->ht[0]);
	_dictclear(d, &d->ht[1]);
	d->rehashidx = -1;
}



