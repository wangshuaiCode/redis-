
sds sdsnewlen(const void *init, size_t initlen) {
	struct sdshdr *sh;
	if(init) {
		sh = zmalloc(sizeof(struct sdshdr) + initlen + 1);
	} else {
		sh = zcalloc(sizeof(struct sdshdr) + initlen + 1);
	}

	if(sh == NULL)
		return NULL;
	if(init && initlen)
		memcpy(sh -> buf, init, initlen);
	sh -> len = initlen;
	sh -> free = 0;
	sh -> buf[initlen] = '\0';
	
	return (char *)sh -> buf;

}

sds sdsdup(const sds s) {
	return sdsnewlen(s, sdslen(s));
}

sds sdsempty(void) {
	return sdsnewlen("", 0);
}

sds sdsnew(const char *init) {
	size_t initlen = (init == NULL) ? 0 : strlen(init);
	return sdsnewlen(init, initlen);
}

void sdsfree(sds s) {
	if(s == NULL) 
		return;
	zfree(s - sizeof(struct sdshdr));
}

void sdsupdatelen(sds s) {
	struct sdshdr *sh = (void *)(s - sizeof(struct sdshdr));

	int reallen = sdslen(s);

	sh -> free += (sh -> len - reallen);
	sh -> len = reallen;
}


void sdsclear(sds s) {
	struct sdshdr *sh = (void *)(s - (sizeof(struct sdshdr)));

	sh -> free += sh -> len;
	sh -> len = 0;
	sh -> buf[0] = '\0';
}

sds MakeRoomFor(sds s, size_t addlen) {
	struct sdshdr *newsh, *sh;
	size_t len, newlen;
	size_t free = sdslen(s);
	if(free > addlen) {
		return s;
	}
	sh = (void *)(s - (sizeof(struct sdshdr)));
	newlen = sh -> len + addlen;
	if(newlen < SDS_MAX_PREALLOC)
		newlen *= 2;
	else
		newlen += SDS_MAX_PREALLOC;
	newsh = zrelloc(sh, sizeof(struct sdshdr) + newlen + 1);
	if(newsh == NULL)
		return NULL:
	newsh -> free = newlen - len;
	return newsh -> buf;

	
}

sds RemoveFreeSpace(sds s) {
	struct sdshdr *sh = (void *)(s - (sizeof(struct sdshdr)));

	sh = zrealloc(sh, sizeof(struct sdshdr) + sh -> len + 1);
	sh -> free = 0;

	return sh -> buf;
}


size_t sdsAllocSize(sds s) {
	struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));

	return sizeof(*sh) + sh -> free + sh -> len + 1;
}

void sdsIncrLen(sds s, int incr) {
	struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));

	assert(sh -> free >= incr);
	sh -> len += incr;
	sh -> free -= incr;

	s[sh -> len] = '\0';
}


sds sdsgrowzero(sds s, size_t len) {
	struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));

	size_t curlen = sh -> len;
	if(curlen >= len) 
		return s;
	size_t addlen = len - curlen;
	s = sdsMakeRoomFor(s, addlen);
	if(s == NULL) return;
	sh = (void *) (s - (sizeof(struct sdshdr)));
	memset(s + curlen, 0, addlen + 1);
	size_t totlen = sh -> len + sh -> free;
	sh -> len = len;
	sh -> free = totlen - sh -> len;

	return s;
}
sds sdscatlen(sds s, const void *t, size_t len) {
	size_t currlen = strlen(s);
	s = sdsMakeRoomFor(s, len);
	if(s == NULL) 
		return NULL;
	struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));
	memcpy(s + currlen, t, len);
	s[currlen + len] = '\0';
	sh -> len = currlen + len;
	sh -> free = sh -> free - len;
	return s;
}

sds sdscatsds(sds s, const sds t) {
	return sdscatlen(s, t, sdslen(t));
}

sds sdscpylen(sds s, const char *t, size_t len) {
	struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));

	size_t totlen = sh -> len + sh -> free;
	if(len > totlen) {
		s = sdsMakeRoomFor(s, len - totlen);
		if(s == NULL) return NULL:
		sh = (void *) (s - (sizeof(struct sdshdr)));
		totlen = sh -> free + sh -> len;
	}
	memcpy(s, t, len);
	sh -> len = len;
	sh -> free = totlen - len;
	s[len] = '\0';
	return s;

}

sds sdscpy(sds s, const char *t) {
	return sdscpylen(s, t, strlen(t));
}

sds sdsfromlonglong(long long value) {
	unsigned long long v = (value < 0) ? -value : value;
	char buf[32];
	char *p = buf + 31;
	do {
		*p -- = v % 10;
		v = v / 10;
	} while(v);
	if(value < 0) *p -- = '-';
	p++;
	return sdsnewlen(p, 32 - (p - buf));
}

