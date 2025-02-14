/*
 * Copyright (c) 2009 Corey Tabaka
 * Copyright (c) 2013 Travis Geiselbrecht
 * Copyright (c) 2015 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

ENTRY(_start)
SECTIONS
{
    . = %KERNEL_BASE% + %KERNEL_LOAD_OFFSET%;

    .text : AT(%MEMBASE% + %KERNEL_LOAD_OFFSET%) {
        PROVIDE_HIDDEN(__code_start = .);
        KEEP(*(.text.boot))
        *(.text* .sram.text)
        PROVIDE_HIDDEN(__code_end = .);
    } =0x9090

    /*
     * This symbol marks the beginning of the read-only data (RODATA) segment,
     * from here to __rodata_end.
     *
     * The RODATA segment comes first so it can start with the ELF file and
     * program headers, which are always at the beginning of the file anyway.
     * That means the next available space is not page-aligned, so it's best to
     * stuff in non-aligned bits there to minimize the zero-padding required to
     * align the RODATA sections that need to be page-aligned.
     */
    .rodata : ALIGN(CONSTANT(MAXPAGESIZE)) {
        PROVIDE_HIDDEN(__rodata_start = .);

        PROVIDE_HIDDEN(__start_commands = .);
        *(commands)
        PROVIDE_HIDDEN(__stop_commands = .);

        PROVIDE_HIDDEN(__start_lk_init = .);
        *(lk_init)
        PROVIDE_HIDDEN(__stop_lk_init = .);

        *(.rodata*)
        . = ALIGN(8);
    }

    /*
     * extra linker scripts tend to insert sections just after .rodata,
     * so we want to make sure this symbol comes after anything inserted above,
     * but not aligned to the next section necessarily.
     */
    .dummy_post_rodata : {
        PROVIDE_HIDDEN(__rodata_end = .);
    }

    .data : ALIGN(CONSTANT(MAXPAGESIZE)) {
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
    }

    .ctors : ALIGN(4) {
        __ctor_list = .;
        KEEP(*(.ctors .init_array))
        __ctor_end = .;
    }
    .dtors : ALIGN(4) {
        __dtor_list = .;
        KEEP(*(.dtors .fini_array))
        __dtor_end = .;
    }

    .stab   : { *(.stab) }
    .stabst : { *(.stabstr) }

    /*
     * extra linker scripts tend to insert sections just after .data,
     * so we want to make sure this symbol comes after anything inserted above,
     * but not aligned to the next section necessarily.
     */
    .dummy_post_data : {
        PROVIDE_HIDDEN(__data_end = .);
    }

    .bss : ALIGN(4096) {
        PROVIDE_HIDDEN(__bss_start = .);
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    PROVIDE_HIDDEN(_end = .);

    /* put a symbol arbitrarily 4MB past the end of the kernel */
    /* used by the heap and other early boot time allocators */
    _end_of_ram = . + (4*1024*1024);

    /DISCARD/ : { *(.comment .note .eh_frame) }
}
