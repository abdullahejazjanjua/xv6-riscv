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


// Linked list node (As each page size is fixed, and allocations happen in page size units not variable, we don't need
// to track size left, as free space = number of nodes * pgsize)
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start); // PGROUNDUP would allign the PA to nearest page boundary (4097 -> 8192)
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  // pa: physical address
  struct run *r;

  // Checks if pa is alligned on a page boundary (whether start address is multiple of PGSIZE)
  // or if it is not below the starting address or if it is not after the ending address
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void* kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;  // Store memory address
  if(r) // if r is not NULL, then move the freelist head forward
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) // if our removed node from freelist is not empty, fill it with garbage values till pgsize
    memset((char*)r, 5, PGSIZE); // fill with junk


  return (void*)r; // return the start address
}

uint64 kmemfree(void)
{
  struct run *r;
  uint64 free = 0;

  for(r = kmem.freelist; r; r = r->next)
  {
    free += PGSIZE;
  }

  return free;

}
