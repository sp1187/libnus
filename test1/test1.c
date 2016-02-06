
#include <libnus.h>
#include <libfb.h>
#include <stdint.h>
#include <regsinternal.h>

int counter = 0;

uint64_t draw_thread_stack[1024];
uint64_t irq_stack[512];

nus_thread_t draw_thread;
uintptr_t test_queue_array[1];
nus_queue_t test_queue;

void NUS_ISR_count(){
	nus_c0_set_compare(nus_c0_get_count() + 46875000);

	nus_queue_send(&test_queue,counter++);
	//nus_thread_wakeup(&draw_thread);
}

void draw_thread_func(){
	intptr_t counter;
	nus_c0_set_compare(nus_c0_get_count() + 100);
	for(;;){
		nus_queue_recv(&test_queue,&counter);
		//nus_thread_suspend();

		if((counter & 1) == 0) {
			fb_red_background();
		}
		else {
			fb_blue_background();
		}

		fb_print_int(48,48,counter,10);
	}
}

int main(){
	fb_init();

	nus_scheduler_init(irq_stack, sizeof irq_stack);

	nus_interrupt_set_mask(COUNT_INTR);

	nus_queue_init(&test_queue, test_queue_array, sizeof test_queue_array / sizeof (*test_queue_array));
	nus_thread_init(&draw_thread,"draw", 192, &draw_thread_func, draw_thread_stack, sizeof draw_thread_stack);

	nus_thread_wakeup(&draw_thread);

	nus_scheduler_start();

	for(;;); //main thread will busy wait when no other threads are ready

}
