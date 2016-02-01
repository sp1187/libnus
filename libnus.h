#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <regsinternal.h>

typedef uint8_t nus_prio_t;

typedef enum {
	NUS_OK = 0,
	NUS_ERR_CONTEXT,
	NUS_ERR_THREAD_STATUS
} nus_result_t;

typedef struct {
	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	uint64_t ra;
	double f20;
	double f22;
	double f24;
	double f26;
	double f28;
	double f30;
	uint32_t status;
	uint32_t fcr31;
} nus_context_t;


typedef struct {
	uint64_t at;
	uint64_t v0;
	uint64_t v1;
	uint64_t a0;
	uint64_t a1;
	uint64_t a2;
	uint64_t a3;
	uint64_t t0;
	uint64_t t1;
	uint64_t t2;
	uint64_t t3;
	uint64_t t4;
	uint64_t t5;
	uint64_t t6;
	uint64_t t7;
	uint64_t t8;
	uint64_t t9;
	uint64_t ra; //TODO: figure out why/if needed
	uint64_t hi;
	uint64_t lo;
	uint64_t epc;
	double f0;
	double f2;
	double f4;
	double f6;
	double f8;
	double f10;
	double f12;
	double f14;
	double f16;
	double f18;
	double f1;
	double f3;
	double f5;
	double f7;
	double f9;
	double f11;
	double f13;
	double f15;
	double f17;
	double f19;
	double f21;
	double f23;
	double f25;
	double f27;
	double f29;
	double f31;
} nus_irq_context_t;

typedef enum {
	RUNNING,
	READY,
	SUSPENDED,
	BLOCKED
} nus_status_t;

typedef struct nus_thread {
	nus_context_t *sp;
	struct nus_thread *next;
	const char* name;
	nus_prio_t prio;
	nus_status_t status;
} nus_thread_t;

typedef struct {
	nus_thread_t* current;
	nus_thread_t* ready;
	intptr_t irq_sp;
} nus_scheduler_t; 

typedef struct {
	intptr_t *array;
	int size;
	int count;
	int front;
	int back;
	nus_thread_t *blockrecv;
	nus_thread_t *blocksend;
} nus_queue_t;

void nus_scheduler_init(void*, size_t);

void nus_thread_init(nus_thread_t *, const char *, nus_prio_t, void (*)(void), void*, size_t);

nus_result_t nus_thread_wakeup(nus_thread_t*);
nus_result_t nus_thread_suspend(void);
nus_result_t nus_thread_yield(void);

nus_prio_t nus_thread_getprio(nus_thread_t*);
nus_result_t nus_thread_setprio(nus_prio_t);

void nus_queue_init(nus_queue_t*, intptr_t*, int);
nus_result_t nus_queue_send(nus_queue_t*, intptr_t);
nus_result_t nus_queue_recv(nus_queue_t*, intptr_t*);


#define MFC0(reg)                                                \
	({                                                           \
	 int __res;                                               \
	 __asm__ volatile ("mfc0 %0, " #reg ";" : "=r" (__res));  \
	 __res;                                                   \
	 })

#define MTC0(reg,value) __asm__ volatile ("mtc0 %z0, " #reg ";" : : "Jr"((unsigned int)value));

static inline int nus_c0_get_count() { return MFC0($9); }
static inline int nus_c0_get_compare() { return MFC0($11); }
static inline int nus_c0_get_status() { return MFC0($12); }
static inline int nus_c0_get_cause() { return MFC0($13); }
static inline int nus_c0_get_epc() { return MFC0($14); }

static inline void nus_c0_set_count(int val) { MTC0($9, val); }
static inline void nus_c0_set_compare(int val) { MTC0($11, val); }
static inline void nus_c0_set_status(int val) { MTC0($12, val); }

/* COP0 functions */

static inline void nus_interrupt_disable(void) {
	uint32_t status;

	__asm__ __volatile__(
			"mfc0 %[status], $12\n\t"
			"srl %[status], %[status], 1\n\t"
			"sll %[status], %[status], 1\n\t"
			"mtc0 %[status], $12\n\t"

			: [status] "=r"(status)
			);
}

//
// Enables VR4300 interrupts.
//
static inline void nus_interrupt_enable(void) {
	uint32_t status;

	__asm__ __volatile__(
			"mfc0 %[status], $12\n\t"
			"ori %[status], %[status], 1\n\t"
			"mtc0 %[status], $12\n\t"

			: [status] "=r"(status)
			);
}

static inline void nus_interrupt_set_mask(uint8_t mask) {
	uint32_t status = (nus_c0_get_status() & (~0xff00)) | (mask << 8);
	nus_c0_set_status(status);
}


/* RCP functions */

static inline void nus_mi_interrupt_set_mask(int32_t mask) {
	((volatile MI_regs_t*) 0xa4300000)->mask  = mask;
}

static inline void nus_vi_set_vintr(uint32_t vi){
	((volatile VI_regs_t*) 0xa4400000)->v_int = vi;
}

static inline void nus_vi_flush_state(const VI_regs_t *state) {
	*((volatile VI_regs_t*) 0xa4400000) = *state;
}


