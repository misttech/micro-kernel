/*
 * Copyright (c) 2014 Travis Geiselbrecht
 * Copyright (c) 2014 Chris Anderson
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lib/pktbuf.h>
#include <stdio.h>
#include <target.h>

#include <dev/gpio.h>
#include <kernel/vm.h>
#include <platform/gem.h>
#include <platform/gpio.h>
#include <platform/interrupts.h>
#include <platform/zynq.h>
#include <target/gpioconfig.h>

zynq_pll_cfg_tree_t zynq_pll_cfg = {.arm =
                                        {
                                            .lock_cnt = 375,
                                            .cp = 2,
                                            .res = 12,
                                            .fdiv = 26,
                                        },
                                    .ddr =
                                        {
                                            .lock_cnt = 475,
                                            .cp = 2,
                                            .res = 12,
                                            .fdiv = 26,
                                        },
                                    .io = {
                                        .lock_cnt = 500,
                                        .cp = 2,
                                        .res = 12,
                                        .fdiv = 20,
                                    }};

const unsigned long zynq_ddr_cfg[] = {
    0XF8006000,  0x00000080U, 0XF8006004,  0x0000107FU, 0XF8006008,  0x03C0780FU, 0XF800600C,
    0x02001001U, 0XF8006010,  0x00014001U, 0XF8006014,  0x0004151AU, 0XF8006018,  0x44E354D2U,
    0XF800601C,  0x720238E5U, 0XF8006020,  0x270872D0U, 0XF8006024,  0x00000000U, 0XF8006028,
    0x00002007U, 0XF800602C,  0x00000008U, 0XF8006030,  0x00040930U, 0XF8006034,  0x00011014U,
    0XF8006038,  0x00000000U, 0XF800603C,  0x00000777U, 0XF8006040,  0xFFF00000U, 0XF8006044,
    0x0FF66666U, 0XF8006048,  0x0003C000U, 0XF8006050,  0x77010800U, 0XF8006058,  0x00000000U,
    0XF800605C,  0x00005003U, 0XF8006060,  0x0000003EU, 0XF8006064,  0x00020000U, 0XF8006068,
    0x00284141U, 0XF800606C,  0x00001610U, 0XF80060A4,  0x10200802U, 0XF80060A8,  0x0670C845U,
    0XF80060AC,  0x000001FEU, 0XF80060B0,  0x1CFFFFFFU, 0XF80060B4,  0x00000200U, 0XF80060B8,
    0x00200066U, 0XF80060C4,  0x00000003U, 0XF80060C4,  0x00000000U, 0XF80060C8,  0x00000000U,
    0XF80060DC,  0x00000000U, 0XF80060F0,  0x00000000U, 0XF80060F4,  0x00000008U, 0XF8006114,
    0x00000000U, 0XF8006118,  0x40000001U, 0XF800611C,  0x40000001U, 0XF8006120,  0x40000001U,
    0XF8006124,  0x40000001U, 0XF800612C,  0x00023C00U, 0XF8006130,  0x00022800U, 0XF8006134,
    0x00022C00U, 0XF8006138,  0x00024800U, 0XF8006140,  0x00000035U, 0XF8006144,  0x00000035U,
    0XF8006148,  0x00000035U, 0XF800614C,  0x00000035U, 0XF8006154,  0x00000077U, 0XF8006158,
    0x0000007CU, 0XF800615C,  0x0000007CU, 0XF8006160,  0x00000075U, 0XF8006168,  0x000000E4U,
    0XF800616C,  0x000000DFU, 0XF8006170,  0x000000E0U, 0XF8006174,  0x000000E7U, 0XF800617C,
    0x000000B7U, 0XF8006180,  0x000000BCU, 0XF8006184,  0x000000BCU, 0XF8006188,  0x000000B5U,
    0XF8006190,  0x00040080U, 0XF8006194,  0x0001FC82U, 0XF8006204,  0x00000000U, 0XF8006208,
    0x000003FFU, 0XF800620C,  0x000003FFU, 0XF8006210,  0x000003FFU, 0XF8006214,  0x000003FFU,
    0XF8006218,  0x000003FFU, 0XF800621C,  0x000003FFU, 0XF8006220,  0x000003FFU, 0XF8006224,
    0x000003FFU, 0XF80062A8,  0x00000000U, 0XF80062AC,  0x00000000U, 0XF80062B0,  0x00005125U,
    0XF80062B4,  0x000012A6U,
};

const unsigned long zynq_ddr_cfg_cnt = countof(zynq_ddr_cfg);

const zynq_ddriob_cfg_t zynq_ddriob_cfg = {
    .addr0 = DDRIOB_OUTPUT_EN(0x3),
    .addr1 = DDRIOB_OUTPUT_EN(0x3),
    .data0 = DDRIOB_INP_TYPE(1) | DDRIOB_TERM_EN | DDRIOB_DCI_TYPE(0x3) | DDRIOB_OUTPUT_EN(0x3),
    .data1 = DDRIOB_INP_TYPE(1) | DDRIOB_TERM_EN | DDRIOB_DCI_TYPE(0x3) | DDRIOB_OUTPUT_EN(0x3),
    .diff0 = DDRIOB_INP_TYPE(2) | DDRIOB_TERM_EN | DDRIOB_DCI_TYPE(0x3) | DDRIOB_OUTPUT_EN(0x3),
    .diff1 = DDRIOB_INP_TYPE(2) | DDRIOB_TERM_EN | DDRIOB_DCI_TYPE(0x3) | DDRIOB_OUTPUT_EN(0x3),
    .ibuf_disable = false,
    .term_disable = false,
};

const uint32_t zynq_mio_cfg[ZYNQ_MIO_CNT] = {
    [0] = MIO_DEFAULT,
    [1] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    [2] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    [3] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    [4] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    [5] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    [6] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    // LED4
    [7] = MIO_IO_TYPE_LVCMOS18 | MIO_DISABLE_RCVR,
    [8] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS33,
    // 16-21 gem0
    [9] = MIO_DEFAULT,
    [10] = MIO_DEFAULT,
    [11] = MIO_DEFAULT,
    [12] = MIO_DEFAULT,
    [13] = MIO_DEFAULT,
    [14] = MIO_DEFAULT,
    [15] = MIO_DEFAULT,
    [16] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    [17] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    [18] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    [19] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    [20] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    [21] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP | MIO_DISABLE_RCVR,
    // 22-27 gem0
    [22] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [23] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [24] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [25] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [26] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [27] = MIO_L0_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_HSTL | MIO_PULLUP,
    [28] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [29] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18 | MIO_TRI_ENABLE,
    [30] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [31] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18 | MIO_TRI_ENABLE,
    [32] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [33] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [34] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [35] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [36] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18 | MIO_TRI_ENABLE,
    [37] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [38] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [39] = MIO_L1_SEL | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [40] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [41] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [42] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [43] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [44] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [45] = MIO_L3_SEL(0x4) | MIO_SPEED_FAST | MIO_IO_TYPE_LVCMOS18,
    [47] = MIO_TRI_ENABLE | MIO_IO_TYPE_LVCMOS18,
    [48] = MIO_L3_SEL(0x7) | MIO_IO_TYPE_LVCMOS18,
    [49] = MIO_TRI_ENABLE | MIO_L3_SEL(0x7) | MIO_IO_TYPE_LVCMOS18,
    // 50-51 are BTN4 and BTN5
    [50] = MIO_IO_TYPE_LVCMOS18 | MIO_DISABLE_RCVR,
    [51] = MIO_IO_TYPE_LVCMOS18 | MIO_DISABLE_RCVR,
    // 52-53 gem0
    [52] = MIO_L3_SEL(0x4) | MIO_IO_TYPE_LVCMOS18 | MIO_PULLUP,
    [53] = MIO_L3_SEL(0x4) | MIO_IO_TYPE_LVCMOS18 | MIO_PULLUP,
};

const zynq_clk_cfg_t zynq_clk_cfg = {
    .arm_clk = ARM_CLK_CTRL_DIVISOR(2) | ARM_CLK_CTRL_CPU_6OR4XCLKACT |
               ARM_CLK_CTRL_CPU_3OR2XCLKACT | ARM_CLK_CTRL_CPU_2XCLKACT |
               ARM_CLK_CTRL_CPU_1XCLKACT | ARM_CLK_CTRL_PERI_CLKACT,
    .ddr_clk = DDR_CLK_CTRL_DDR_3XCLKACT | DDR_CLK_CTRL_DDR_2XCLKACT |
               DDR_CLK_CTRL_DDR_3XCLK_DIV(2) | DDR_CLK_CTRL_DDR_2XCLK_DIV(3),
    .dci_clk = CLK_CTRL_CLKACT | CLK_CTRL_DIVISOR0(52) | CLK_CTRL_DIVISOR1(2),
    .gem0_clk = CLK_CTRL_CLKACT | CLK_CTRL_DIVISOR0(8) | CLK_CTRL_DIVISOR1(1),
    .gem0_rclk = CLK_CTRL_CLKACT,
    .lqspi_clk = CLK_CTRL_CLKACT | CLK_CTRL_DIVISOR0(5),
    .sdio_clk = CLK_CTRL_CLKACT | CLK_CTRL_DIVISOR0(20),
    .uart_clk = CLK_CTRL_CLKACT1 | CLK_CTRL_DIVISOR0(20),
    .pcap_clk = CLK_CTRL_CLKACT | CLK_CTRL_DIVISOR0(5),
    .fpga0_clk = CLK_CTRL_DIVISOR0(10) | CLK_CTRL_DIVISOR1(1),
    .fpga1_clk = CLK_CTRL_SRCSEL(3) | CLK_CTRL_DIVISOR0(6) | CLK_CTRL_DIVISOR1(1),
    .fpga2_clk = CLK_CTRL_SRCSEL(2) | CLK_CTRL_DIVISOR0(53) | CLK_CTRL_DIVISOR1(2),
    .fpga3_clk = CLK_CTRL_DIVISOR1(1),
    .aper_clk = DMA_CPU_CLK_EN | USB0_CPU_CLK_EN | USB1_CPU_CLK_EN | GEM0_CPU_CLK_EN |
                SDI0_CPU_CLK_EN | I2C0_CPU_CLK_EN | I2C1_CPU_CLK_EN | UART1_CPU_CLK_EN |
                GPIO_CPU_CLK_EN | LQSPI_CPU_CLK_EN | SMC_CPU_CLK_EN,
    .clk_621_true = CLK_621_ENABLE,
};

void target_early_init(void) {
  gpio_config(GPIO_LEDY, GPIO_OUTPUT);
  gpio_set(GPIO_LEDY, 0);
}

static enum handler_return toggle_ledy(void *arg) {
  static bool on = false;

  gpio_set(GPIO_LEDY, on);
  on = !on;

  return INT_NO_RESCHEDULE;
}

void target_set_debug_led(unsigned int led, bool on) {
  if (led == 0) {
    gpio_set(GPIO_LEDY, on);
  }
}
void target_init(void) {
  gem_init(GEM0_BASE);

  register_gpio_int_handler(ZYBO_BTN5, toggle_ledy, NULL);
  zynq_unmask_gpio_interrupt(ZYBO_BTN5);
}
