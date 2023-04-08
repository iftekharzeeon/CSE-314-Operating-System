// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  int referenceCount [(PHYSTOP)/PGSIZE]; //A new array for the Copy on Write implementation that will keep track of the reference count of each page
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");

  //Initiliaze with 1, because freerange calls kfree without calling kalloc first
  acquire(&kmem.lock);
  for(int i = 0; i < ((PHYSTOP)/PGSIZE); i++)
  {
    kmem.referenceCount[i] = 1;
  }
  release(&kmem.lock);
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  //Kfree is modified in such way that it will only free the page if the reference count is 0
  
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
  

  struct run *r;

  acquire(&kmem.lock);
  //Get the index of the page
  int index = (((uint64)pa))/PGSIZE;
  if (index < 0) {
    release(&kmem.lock);
    return;
  }
  //Check if the page is already free
  if (kmem.referenceCount[index] <= 0)
    panic("Freeing a page that is already free");
  //Decrease the reference count
  kmem.referenceCount[index] = kmem.referenceCount[index] - 1;
  release(&kmem.lock);
  //If the reference count is 0, free the page
  if (kmem.referenceCount[index] <= 0) {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}

//This function is implemented for CoW
void 
increaseRefCount(void *pa)
{  
  int index = ((uint64)pa)/PGSIZE;
  if (index < 0)
    return;
  //printf("i %d\n",index);
  acquire(&kmem.lock);
  int c = kmem.referenceCount[index];
  kmem.referenceCount[index] = c + 1;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  // printf("here\n");
  increaseRefCount((void*)r);
  return (void*)r;
}
