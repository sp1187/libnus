#pragma once

//Most things here are straight-copied from various libdragon files

#include <stdint.h>

#define MI_INTR_SP 0x01
#define MI_INTR_SI 0x02
#define MI_INTR_AI 0x04
#define MI_INTR_VI 0x08
#define MI_INTR_PI 0x10
#define MI_INTR_DP 0x20

#define MI_INTR_CLEAR_SP                    (1 <<  0)
#define MI_INTR_SET_SP                      (1 <<  1)
#define MI_INTR_CLEAR_SI                    (1 <<  2)
#define MI_INTR_SET_SI                      (1 <<  3)
#define MI_INTR_CLEAR_AI                    (1 <<  4)
#define MI_INTR_SET_AI                      (1 <<  5)
#define MI_INTR_CLEAR_VI                    (1 <<  6)
#define MI_INTR_SET_VI                      (1 <<  7)
#define MI_INTR_CLEAR_PI                    (1 <<  8)
#define MI_INTR_SET_PI                      (1 <<  9)
#define MI_INTR_CLEAR_DP                    (1 << 10)
#define MI_INTR_SET_DP                      (1 << 11)

#define PI_CLEAR_INTERRUPT 0x02
#define SI_CLEAR_INTERRUPT 0
#define SP_CLEAR_INTERRUPT 0x08
#define DP_CLEAR_INTERRUPT 0x0800
#define AI_CLEAR_INTERRUPT 0

#define SW1_INTR 0x01
#define SW2_INTR 0x02
#define RCP_INTR 0x04
#define CART_INTR 0x08
#define PRENMI_INTR 0x10
#define COUNT_INTR 0x80

typedef struct {
    volatile void * address;
    uint32_t length;
    uint32_t control;
    uint32_t status;
    uint32_t dacrate;
    uint32_t samplesize;
} AI_regs_t;


typedef struct {
    uint32_t mode;
    uint32_t version;
    uint32_t intr;
    uint32_t mask;
} MI_regs_t;


typedef struct {
    uint32_t control;
    void * framebuffer;
    uint32_t width;
    uint32_t v_int;
    uint32_t cur_line;
    uint32_t timing;
    uint32_t v_sync;
    uint32_t h_sync;
    uint32_t h_sync2;
    uint32_t h_limits;
    uint32_t v_limits;
    uint32_t color_burst;
    uint32_t h_scale;
    uint32_t v_scale;
} VI_regs_t;


typedef struct {
    volatile void * ram_address;
    uint32_t pi_address;
    uint32_t read_length;
    uint32_t write_length;
    uint32_t status;
} PI_regs_t;


typedef struct {
    volatile void * DRAM_addr;
    volatile void * PIF_addr_read;
    uint32_t reserved1;
    uint32_t reserved2;
    volatile void * PIF_addr_write;
    uint32_t reserved3;
    uint32_t status;
} SI_regs_t;


typedef struct {
    volatile void * RSP_addr;
    volatile void * RDAM_addr;
    uint32_t rsp_read_length;
    uint32_t rsp_write_length;
    uint32_t status;
    uint32_t rsp_dma_full;
    uint32_t rsp_dma_busy;
    uint32_t rsp_semaphore;
} SP_regs_t;



