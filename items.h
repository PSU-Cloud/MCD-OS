// #define HOT_LRU 0
// #define WARM_LRU 64
// #define COLD_LRU 128
// #define TEMP_LRU 192
#define HOT_LRU 0
#define WARM_LRU 1
#define COLD_LRU 2
#define TEMP_LRU 3

#define CLEAR_LRU(id) (id & ~(3<<6))
#define GET_LRU(id) (id & (3<<6))

/* See items.c */
uint64_t get_cas_id(void);

/*@null@*/
item *do_item_alloc(char *key, const size_t nkey, const unsigned int flags, const rel_time_t exptime, const int nbytes, LIBEVENT_THREAD *thread);
item_chunk *do_item_alloc_chunk(item_chunk *ch, const size_t bytes_remain, LIBEVENT_THREAD *thread);
item *do_item_alloc_pull(const size_t ntotal, const unsigned int id, LIBEVENT_THREAD *thread);
void item_free(item *it);
bool item_size_ok(const size_t nkey, const int flags, const int nbytes);

int  do_item_link(item *it, LIBEVENT_THREAD *thread, const uint32_t hv, bool new);     /** may fail if transgresses limits */
void do_item_unlink(item *it, LIBEVENT_THREAD *thread, const uint32_t hv, bool first);
void do_item_unlink_nolock(item *it, LIBEVENT_THREAD *thread, const uint32_t hv);
void do_item_unlink_inall(item *it, const uint32_t hv);
void do_item_remove(item *it);
void do_item_update(item *it, LIBEVENT_THREAD *thread);   /** update LRU time to current and reposition */
void do_item_update_nolock(item *it, int thread_id);
int  do_item_replace(item *it, item *new_it, LIBEVENT_THREAD *thread, const uint32_t hv);

int item_is_flushed(item *it);
unsigned int do_get_lru_size(uint32_t id);

void do_item_linktail_q(item *it);
void do_item_unlinktail_q(item *it);
item *do_item_crawl_q(item *it);

void *item_lru_bump_buf_create(void);

#define LRU_PULL_EVICT 1
#define LRU_PULL_CRAWL_BLOCKS 2
#define LRU_PULL_RETURN_ITEM 4 /* fill info struct if available */

struct lru_pull_tail_return {
    item *it;
    uint32_t hv;
};

struct thread_set   // set of threads containing an item
{
    struct thread_set *prev;
    struct thread_set *next;
    LIBEVENT_THREAD *thread;
};

struct counting   // 
{
 int N;
 struct thread_set *P;
};

// int lru_pull_tail(const int orig_id, const int cur_lru,
//         const uint64_t total_bytes, const uint8_t flags, const rel_time_t max_age,
//         struct lru_pull_tail_return *ret_it);
int lru_pull_tail(LIBEVENT_THREAD *thread, const int orig_id, const int cur_lru,
        const uint64_t total_bytes, const uint8_t flags, const rel_time_t max_age,
        struct lru_pull_tail_return *ret_it);

/*@null@*/
char *item_cachedump(const unsigned int slabs_clsid, const unsigned int limit, unsigned int *bytes);
void item_stats(ADD_STAT add_stats, void *c);
void do_item_stats_add_crawl(const int i, const uint64_t reclaimed,
        const uint64_t unfetched, const uint64_t checked);
void item_stats_totals(ADD_STAT add_stats, void *c);
/*@null@*/
void item_stats_sizes(ADD_STAT add_stats, void *c);
void item_stats_sizes_init(void);
void item_stats_sizes_enable(ADD_STAT add_stats, void *c);
void item_stats_sizes_disable(ADD_STAT add_stats, void *c);
void item_stats_sizes_add(item *it);
void item_stats_sizes_remove(item *it);
bool item_stats_sizes_status(void);

/* stats getter for slab automover */
typedef struct {
    int64_t evicted;
    int64_t outofmemory;
    uint32_t age;
} item_stats_automove;
void fill_item_stats_automove(item_stats_automove *am);

item *do_item_get(const char *key, const size_t nkey, const uint32_t hv, conn *c, const bool do_update);
item *do_item_touch(const char *key, const size_t nkey, uint32_t exptime, const uint32_t hv, conn *c);
void item_stats_reset(void);
extern pthread_mutex_t lru_locks[POWER_LARGEST];

/* global lock */
extern pthread_mutex_t global_lock;

int start_lru_maintainer_thread(void *arg);
int stop_lru_maintainer_thread(void);
int init_lru_maintainer(void);
void lru_maintainer_pause(void);
void lru_maintainer_resume(void);

void *lru_bump_buf_create(void);

#ifdef EXTSTORE
#define STORAGE_delete(e, it) \
    do { \
        if (it->it_flags & ITEM_HDR) { \
            item_hdr *hdr = (item_hdr *)ITEM_data(it); \
            extstore_delete(e, hdr->page_id, hdr->page_version, \
                    1, ITEM_ntotal(it)); \
        } \
    } while (0)
#else
#define STORAGE_delete(...)
#endif

struct counting *check_item(item *it); // return the number of LRUs containing this item
bool LRU_item(item *it, int thread_id);   // check whether this LRU contains this item
void item_link_LRU(item *it, int thread_id);  // it's item_link_q() actually
void item_unlink_LRU(item *it, int thread_id);  // it's item_unlink_q() actually
void inflation(item *it, struct counting *result);
void deflation(item *it, LIBEVENT_THREAD *thread, struct counting *result);
item *eviction_candidate(LIBEVENT_THREAD *thread);
void free_result(struct counting *result);
void slab_pull_tail(int clsid);
void check_S_LRU(uint8_t cur_lru, LIBEVENT_THREAD *thread);
extern LIBEVENT_THREAD *get_thread(int thread_id);
extern int slabs_size(int clsid);
extern slabclass_t *get_slab(int clsid);
