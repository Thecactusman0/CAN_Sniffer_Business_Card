/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, hathach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "board_api.h"
#include "ch32v20x.h"

//--------------------------------------------------------------------+
// newlib read()/write() retarget
//--------------------------------------------------------------------+
#ifdef __ICCARM__
  #define sys_write   __write
  #define sys_read    __read
#elif defined(__MSP430__) || defined(__RX__) || TU_CHECK_MCU(OPT_MCU_NUC120, OPT_MCU_NUC121, OPT_MCU_NUC126, OPT_MCU_NUC505)
  #define sys_write   write
  #define sys_read    read
#else
  #define sys_write   _write
  #define sys_read    _read
#endif

int sys_write(int fhdl, const char *buf, size_t count) TU_ATTR_USED;
int sys_read(int fhdl, char *buf, size_t count) TU_ATTR_USED;

#if defined(LOGGER_RTT)
// Logging with RTT

// If using SES IDE, use the Syscalls/SEGGER_RTT_Syscalls_SES.c instead
#if !(defined __SES_ARM) && !(defined __SES_RISCV) && !(defined __CROSSWORKS_ARM)
#include "SEGGER_RTT.h"

int sys_write(int fhdl, const char *buf, size_t count) {
  (void) fhdl;
  return (int) SEGGER_RTT_Write(0, buf, (int) count);
}

int sys_read(int fhdl, char *buf, size_t count) {
  (void) fhdl;
  int rd = (int) SEGGER_RTT_Read(0, buf, count);
  return (rd > 0) ? rd : -1;
}
#endif

#elif defined(LOGGER_SWO)

#define ITM_BASE 0xE0000000
#define ITM_STIM0 (*((volatile uint8_t*)(ITM_BASE + 0)))
#define ITM_TER *((volatile uint32_t*)(ITM_BASE + 0xE00))
#define ITM_TCR *((volatile uint32_t*)(ITM_BASE + 0xE80))

#define ITM_TCR_ITMENA (1 << 0)

// Logging with SWO for ARM Cortex-M
int sys_write (int fhdl, const char *buf, size_t count) {
  (void) fhdl;
  uint8_t const* buf8 = (uint8_t const*) buf;

  if ((ITM_TCR & ITM_TCR_ITMENA) && (ITM_TER & 1ul)) {
    for(size_t i=0; i < count; i++) {
      while (!(ITM_STIM0 & 1ul)) {
        asm("nop");
      }
      ITM_STIM0 = buf8[i];
    }
  }

  return (int) count;
}

int sys_read (int fhdl, char *buf, size_t count) {
  (void) fhdl;
  (void) buf;
  (void) count;
  return 0;
}

#else

// Default logging with on-board UART
int sys_write (int fhdl, const char *buf, size_t count) {
  (void) fhdl;
  return board_uart_write(buf, (int) count);
}

int sys_read (int fhdl, char *buf, size_t count) {
  (void) fhdl;
  int rd = board_uart_read((uint8_t*) buf, (int) count);
  return (rd > 0) ? rd : -1;
}

#endif

// Clang use picolibc
#if defined(__clang__)
static int cl_putc(char c, FILE *f) {
  (void) f;
  return sys_write(0, &c, 1);
}

static int cl_getc(FILE* f) {
  (void) f;
  char c;
  return sys_read(0, &c, 1) > 0 ? c : -1;
}

static FILE __stdio = FDEV_SETUP_STREAM(cl_putc, cl_getc, NULL, _FDEV_SETUP_RW);
FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);
#endif

//--------------------------------------------------------------------+
// UART driver implementation
//--------------------------------------------------------------------+
#define UART1 USART1

int board_uart_write(void const *buf, int len) {
  const uint8_t *data = (const uint8_t *) buf;
  for (int i = 0; i < len; i++) {
    while (!(UART1->STATR & USART_FLAG_TXE));
    UART1->DATAR = data[i];
  }
  return len;
}

int board_uart_read(uint8_t *buf, int len) {
  int count = 0;
  while (count < len) {
    if (UART1->STATR & USART_FLAG_RXNE) {
      buf[count++] = UART1->DATAR;
    } else {
      break;
    }
  }
  return count;
}

static void uart_init(void) {
  // Enable USART1 clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  USART_InitTypeDef USART_InitStructure = {
    .USART_BaudRate = CFG_BOARD_UART_BAUDRATE,
    .USART_WordLength = USART_WordLength_8b,
    .USART_StopBits = USART_StopBits_1,
    .USART_Parity = USART_Parity_No,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
  };
  USART_Init(UART1, &USART_InitStructure);
  USART_Cmd(UART1, ENABLE);
}

//--------------------------------------------------------------------+
// Weak board API (to be optionally implemented by board)
//--------------------------------------------------------------------+
TU_ATTR_WEAK void board_init(void) {
  // Initialize UART
  uart_init();
}

TU_ATTR_WEAK size_t board_get_unique_id(uint8_t id[], size_t max_len) {
  (void) max_len;
  // fixed serial string is 01234567889ABCDEF
  uint32_t* uid32 = (uint32_t*) (uintptr_t)id;
  uid32[0] = 0x67452301u;
  uid32[1] = 0xEFCDAB89u;
  return 8;
}

TU_ATTR_WEAK void board_init_after_tusb(void) {
  // USB pins init for CH32V USBFS
  GPIO_InitTypeDef GPIO_InitStructure;/* = {
    .GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12,
    .GPIO_Speed = GPIO_Speed_50MHz,
    .GPIO_Mode = GPIO_Mode_IN_FLOATING,
  };
  GPIO_Init(GPIOA, &GPIO_InitStructure); */

  // UART1 pins init (PA9=TX, PA10=RX)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

TU_ATTR_WEAK void board_reset_to_bootloader(void) {
  // not implemented
}

//--------------------------------------------------------------------+
// Board API
//--------------------------------------------------------------------+
int board_getchar(void) {
  char c;
  return (sys_read(0, &c, 1) > 0) ? (int) c : (-1);
}

void board_putchar(int c) {
  (void) sys_write(0, (const char*)&c, 1);
}

uint32_t tusb_time_millis_api(void) {
  return board_millis();
}
