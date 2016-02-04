#include <libnus.h>
#include <regsinternal.h>
#include <stdbool.h>

nus_scheduler_t nus_scheduler;
extern void _nus_context_switch(nus_thread_t*, nus_thread_t*);

#define IN_INTERRUPT() (nus_c0_get_status() & 0x06)

/* Scheduler functions */

void nus_scheduler_init(void *irq_area, size_t irq_size){
	static nus_thread_t mainthread;

	nus_scheduler.current = &mainthread;
	nus_scheduler.ready = NULL;
	nus_scheduler.irq_sp = (intptr_t) irq_area + irq_size;

	mainthread.next = NULL;
	mainthread.prio = 128;
	mainthread.name = "main";
	mainthread.status = RUNNING;

}

/* Internal thread functions */

nus_thread_t *_nus_tlist_pop(nus_thread_t **start){
	nus_thread_t *tp = *start;
	if(tp != NULL) *start = tp->next;
	return tp;
}

void _nus_tlist_insert(nus_thread_t *tp, nus_thread_t **start){
	nus_thread_t **cp = start;

	while(*cp != NULL && tp->prio <= (*cp)->prio){
		cp = &((*cp)->next);
	}

	tp->next = *cp;
	*cp = tp;
}

void _nus_thread_ready(nus_thread_t *tp){
	tp->status = READY;
	_nus_tlist_insert(tp,&nus_scheduler.ready);
}

void _nus_thread_block() {
	nus_thread_t *otp = nus_scheduler.current;
	nus_thread_t *ntp = _nus_tlist_pop(&nus_scheduler.ready);
	ntp->status = RUNNING;
	nus_scheduler.current = ntp;

	_nus_context_switch(ntp, otp);
}

void _nus_thread_switch(){
	nus_thread_t *otp = nus_scheduler.current;
	nus_thread_t *ntp = _nus_tlist_pop(&nus_scheduler.ready);

	otp->status = READY;
	ntp->status = RUNNING;
	nus_scheduler.current = ntp;

	_nus_tlist_insert(otp,&nus_scheduler.ready);

	_nus_context_switch(ntp, otp);
}

void _nus_thread_reschedule(){
	if(nus_scheduler.ready && nus_scheduler.ready->prio > nus_scheduler.current->prio){
		_nus_thread_switch();
	}
}

/* Thread functions */

void nus_thread_init(nus_thread_t *tp, const char *name, nus_prio_t prio, void (*entry)(void), void *area, size_t size){
	tp->next = NULL;
	tp->name = name;
	tp->prio = prio;
	tp->status = SUSPENDED;
	tp->sp = (nus_context_t*) (area + size - sizeof (nus_context_t));
	tp->sp->ra = (intptr_t) entry; 
	tp->sp->status = nus_c0_get_status();
}

nus_result_t nus_thread_wakeup(nus_thread_t *tp){
	nus_interrupt_disable();

	if(tp->status == SUSPENDED){
		_nus_thread_ready(tp);

		if(!IN_INTERRUPT()){
			_nus_thread_reschedule();
		}
	}
	else {
		nus_interrupt_enable();
		return NUS_ERR_THREAD_STATUS;
	}

	nus_interrupt_enable();

	return NUS_OK;
}

nus_result_t nus_thread_suspend(){
	if(IN_INTERRUPT()) return NUS_ERR_CONTEXT; //nonsense in exception context

	nus_interrupt_disable();

	nus_scheduler.current->status = SUSPENDED;
	_nus_thread_block();

	nus_interrupt_enable();

	return NUS_OK;
}


nus_result_t nus_thread_yield(){
	if(IN_INTERRUPT()) return NUS_ERR_CONTEXT; //nonsense in exception context

	nus_interrupt_disable();

	if(nus_scheduler.ready && nus_scheduler.ready->prio >= nus_scheduler.current->prio){
		_nus_thread_switch();
	}

	nus_interrupt_enable();

	return NUS_OK;
}

nus_prio_t nus_thread_getprio(nus_thread_t *tp){
	return tp->prio;
}

nus_result_t nus_thread_setprio(nus_prio_t newprio){
	if(IN_INTERRUPT()) return NUS_ERR_CONTEXT; //nonsense in exception context

	nus_interrupt_disable();

	nus_scheduler.current->prio = newprio;

	_nus_thread_reschedule();

	nus_interrupt_enable();

	return NUS_OK;
}

/* Queue functions */

void nus_queue_init(nus_queue_t *mq, intptr_t *array, int size){
	mq->array = array;
	mq->size = size;
	mq->count = 0;
	mq->front = 0;
	mq->back = 0;
	mq->blockrecv = NULL;
	mq->blocksend = NULL;
}

nus_result_t nus_queue_send(nus_queue_t *mq, intptr_t msg){
	nus_interrupt_disable();

	if(mq->count == mq->size){
		if(IN_INTERRUPT()){
			nus_interrupt_enable();
			return NUS_ERR_CONTEXT;
		}
		else {
			_nus_tlist_insert(nus_scheduler.current,&(mq->blocksend));
			nus_scheduler.current->status = BLOCKED;
			_nus_thread_block();
		}

	}

	nus_thread_t *tp = _nus_tlist_pop(&(mq->blockrecv));
	if(tp != NULL) {
		_nus_thread_ready(tp);
	}

	mq->array[mq->back++] = msg;
	if(mq->back == mq->size) mq->back = 0;
	mq->count++;

	if(!IN_INTERRUPT()){
		_nus_thread_reschedule();
	}

	nus_interrupt_enable();

	return NUS_OK;
}

nus_result_t nus_queue_recv(nus_queue_t *mq, intptr_t *msg){
	nus_interrupt_disable();

	if(mq->count == 0){
		if(IN_INTERRUPT()){
			nus_interrupt_enable();
			return NUS_ERR_CONTEXT;
		}
		else {
			_nus_tlist_insert(nus_scheduler.current,&(mq->blockrecv));
			nus_scheduler.current->status = BLOCKED;
			_nus_thread_block();
		}
	}

	nus_thread_t *tp = _nus_tlist_pop(&(mq->blocksend));
	if(tp != NULL) {
		_nus_thread_ready(tp);
	}

	*msg = mq->array[mq->front++];
	if(mq->front == mq->size) mq->front = 0;
	mq->count--;

	if(!IN_INTERRUPT()){
		_nus_thread_reschedule();
	}

	nus_interrupt_enable();

	return NUS_OK;
}

/* RCP interfaces */

static volatile AI_regs_t* const AI_regs = (AI_regs_t*)0xa4500000;
static volatile MI_regs_t* const MI_regs = (MI_regs_t*)0xa4300000;
static volatile VI_regs_t* const VI_regs = (VI_regs_t*)0xa4400000;
static volatile PI_regs_t* const PI_regs = (PI_regs_t*)0xa4600000;
static volatile SI_regs_t* const SI_regs = (SI_regs_t*)0xa4800000;
static volatile SP_regs_t* const SP_regs = (SP_regs_t*)0xa4040000;


/* Interrupt handling */

void NUS_ISR_sw1(void) __attribute__((weak));
void NUS_ISR_sw2(void) __attribute__((weak));
void NUS_ISR_cart(void) __attribute__((weak));
void NUS_ISR_prenmi(void) __attribute__((weak));
void NUS_ISR_count(void) __attribute__((weak));

void NUS_ISR_sp(void) __attribute__((weak));
void NUS_ISR_si(void) __attribute__((weak));
void NUS_ISR_ai(void) __attribute__((weak));
void NUS_ISR_vi(void) __attribute__((weak));
void NUS_ISR_pi(void) __attribute__((weak));
void NUS_ISR_dp(void) __attribute__((weak));

static void (*nus_isr_table[])(void);

void _nus_handle_interrupt(){
	uint32_t cause = nus_c0_get_cause();
	uint32_t exc_code = (cause & 0x7c) >> 2;
	if(exc_code == 0){
		uint32_t ip = (cause & 0xff00) >> 8;
		int irq = __builtin_ctz(ip);

		if(nus_isr_table[irq]) {
			nus_isr_table[irq]();
		}
	}
	else {
		//TODO
	}

}

void nus_handle_rcp_interrupt(){
	uint32_t status = MI_regs->intr;

	if(status & MI_INTR_SP) {
		SP_regs->status = SP_CLEAR_INTERRUPT;
		if(NUS_ISR_sp) NUS_ISR_sp();
	}

	if(status & MI_INTR_SI) {
		SI_regs->status = SI_CLEAR_INTERRUPT;
		if(NUS_ISR_si) NUS_ISR_si();
	}

	if(status & MI_INTR_AI) {
		AI_regs->status = AI_CLEAR_INTERRUPT;
		if(NUS_ISR_ai) NUS_ISR_ai();
	}

	if(status & MI_INTR_VI) {
		VI_regs->cur_line = 0; 
		if(NUS_ISR_vi) NUS_ISR_vi();
	}

	if(status & MI_INTR_PI) {
		PI_regs->status = PI_CLEAR_INTERRUPT;
		if(NUS_ISR_pi) NUS_ISR_pi();
	}

	if(status & MI_INTR_DP) {
		MI_regs->mode = DP_CLEAR_INTERRUPT;
		if(NUS_ISR_dp) NUS_ISR_dp();
	}
}

static void (*nus_isr_table[])(void) = {
	NUS_ISR_sw1,
	NUS_ISR_sw2,
	nus_handle_rcp_interrupt,
	NUS_ISR_cart,
	NUS_ISR_prenmi,
	NULL,
	NULL,
	NUS_ISR_count
};


