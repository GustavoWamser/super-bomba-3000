#pragma once

#define set_bit(reg, bit) ((reg) |= (1 << (bit)))
#define clr_bit(reg, bit) ((reg) &= ~(1 << (bit)))
#define tst_bit(reg, bit) ((reg) & (1 << (bit)))