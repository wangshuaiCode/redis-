/*
 * sdshdr 结构
 */
#define SDS_MAX_PREALLOC (1024 * 1024)
struct sdshdr {
	int len;
	
	int free;

	char buf[];
};

typedef char * sds;

static inline size_t sdslen(const sds s) {
	struct sdshdr *sh = (void *)(s - (sizeof(struct sdshdr)));
	return sh -> len;
}

static inline size_t sdsavail(const sds s) {
	struct sdshdr *sh = (void *)(s - (sizeof(struct sdshdr)));
	return sh -> free;
}

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
sds sdsempty();
sds sdsdup(const sds s);
void sdsfree(sds s);
size_t sdsavail(const sds s);
sds sdsgrowzero(sds s, size_t len);

sds sdscpylen(sds s, const char *t, size_t len);
sds sdscpy(sds s, const char *t);
sds sdscatsds(sds s, const sds t);
sds sdscatlen(sds s, const char *t, size_t len);

void sdsupdatelen(sds s);
sds sdsfromlonglogn(long long value);

sds sdsMakeRoomFor(sds s, size_t len);
sds sdsIncrLen(sds s, int incr);
sds sdsRemoveFreeSpace(sds s);
size_t sdsAllocSize(sds s);
