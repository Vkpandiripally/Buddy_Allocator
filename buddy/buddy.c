/**
 * Buddy Allocator
 *
 * For the list library usage, see http://www.mcs.anl.gov/~kazutomo/list/
 */

/**************************************************************************
 * Conditional Compilation Options
 **************************************************************************/
#define USE_DEBUG 0

/**************************************************************************
 * Included Files
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "buddy.h"
#include "list.h"

/**************************************************************************
 * Public Definitions
 **************************************************************************/
#define MIN_ORDER 12
#define MAX_ORDER 20

#define PAGE_SIZE (1<<MIN_ORDER)
/* page index to address */
#define PAGE_TO_ADDR(page_idx) (void *)((page_idx*PAGE_SIZE) + g_memory)

/* address to page index */
#define ADDR_TO_PAGE(addr) ((unsigned long)((void *)addr - (void *)g_memory) / PAGE_SIZE)

/* find buddy address */
#define BUDDY_ADDR(addr, o) (void *)((((unsigned long)addr - (unsigned long)g_memory) ^ (1<<o)) \
									 + (unsigned long)g_memory)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#if USE_DEBUG == 1
#  define PDEBUG(fmt, ...) \
	fprintf(stderr, "%s(), %s:%d: " fmt,			\
		__func__, __FILE__, __LINE__, ##__VA_ARGS__)
#  define IFDEBUG(x) x
#else
#  define PDEBUG(fmt, ...)
#  define IFDEBUG(x)
#endif

/**************************************************************************
 * Public Types
 **************************************************************************/
typedef struct {
	struct list_head list;
	/* TODO: DECLARE NECESSARY MEMBER VARIABLES */
	int index;
	char* address; 
	int flag;
} page_t;

/**************************************************************************
 * Global Variables
 **************************************************************************/
/* free lists*/
struct list_head free_area[MAX_ORDER+1];

/* memory area */
char g_memory[1<<MAX_ORDER];

/* page structures */
page_t g_pages[(1<<MAX_ORDER)/PAGE_SIZE];

/**************************************************************************
 * Public Function Prototypes
 **************************************************************************/

/**************************************************************************
 * Local Functions
 **************************************************************************/

/**
 * Initialize the buddy system
 */
void buddy_init()
{
	int i;
	int n_pages = (1<<MAX_ORDER) / PAGE_SIZE;
	for (i = 0; i < n_pages; i++) {
		/* TODO: INITIALIZE PAGE STRUCTURES */
		INIT_LIST_HEAD(&g_pages[i].list);
		g_pages[i].index = i;
		g_pages[i].address = PAGE_TO_ADDR(i);
		CLEAR(g_pages[i].order);
	}

	/* initialize freelist */
	for (i = MIN_ORDER; i <= MAX_ORDER; i++) {
		INIT_LIST_HEAD(&free_area[i]);
	}

	/* add the entire memory as a freeblock */
	list_add(&g_pages[0].list, &free_area[MAX_ORDER]);
}

/**
 * Allocate a memory block.
 *
 * On a memory request, the allocator returns the head of a free-list of the
 * matching size (i.e., smallest block that satisfies the request). If the
 * free-list of the matching block size is empty, then a larger block size will
 * be selected. The selected (large) block is then splitted into two smaller
 * blocks. Among the two blocks, left block will be used for allocation or be
 * further splitted while the right block will be added to the appropriate
 * free-list.
 *
 * @param size size in bytes
 * @return memory block address
 */
void *buddy_alloc(int size)
{
	#if USE_DEBUG == 1
			PDEBUG("Split size %dK on order %d\n", order, (int)pow(2,order)/1024);
	#endif
	page_t *left_temp_ptr;
	if(order == i){
		left_temp_ptr = list_entry(free_area[i].next, page_t, list);
		list_del(&left_temp_ptr->list);
	}
	/* TODO: IMPLEMENT THIS FUNCTION */
	else {
		#if USE_DEG == 1
			PDEBUG("Recursing.\n");
		#endif
		void *block_address = buddy_alloc(1 << (order + 1));
		left_temp_ptr = &g_pages[ADDR_TO_PAGE(block_address)];

		list_add(&g_pages[left_temp_ptr->index + (1 << order) / PAGE_SIZE].list, &free_area[order]);
	}

	left_temp_ptr->order = order;	
	return NULL;
}

/**
 * Free an allocated memory block.
 *
 * Whenever a block is freed, the allocator checks its buddy. If the buddy is
 * free as well, then the two buddies are combined to form a bigger block. This
 * process continues until one of the buddies is not free.
 *
 * @param addr memory block address to be freed
 */
void buddy_free(void *addr)
{
	/* TODO: IMPLEMENT THIS FUNCTION */
	char* address = (char*)addr;

	int index = ADDR_TO_PAGE (address);
	int order = g_pages[index].order;

	do
	{
		page_t *const current_page = find_buddy(address, order);

		if( NULL == current_page || BUDDY_ADDR(address, order) != current_page->address)
		{
			CLEAR(g_pages[index].order);
			list_add(&g_pages[index].list, &free_area[order]);
			return;
		}

		if(address > current_page->address)
		{
			address = current_page->address;
			index = ADDR_TO_PAGE(address);
		}

		list_del(&current_page->list);
	} while (order++ < MAX_ORDER);
}

/**
 * Print the buddy system status---order oriented
 *
 * print free pages in each order.
 */
void buddy_dump()
{
	int o;
	for (o = MIN_ORDER; o <= MAX_ORDER; o++) {
		struct list_head *pos;
		int cnt = 0;
		list_for_each(pos, &free_area[o]) {
			cnt++;
		}
		printf("%d:%dK ", cnt, (1<<o)/1024);
	}
	printf("\n");
}
