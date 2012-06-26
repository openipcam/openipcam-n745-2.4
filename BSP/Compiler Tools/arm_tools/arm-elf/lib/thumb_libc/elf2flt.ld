ENTRY(_start)
STARTUP(crt0.o)

MEMORY {
        flatmem : ORIGIN = 0x0, LENGTH = 0x100000
}

SECTIONS {
        .text 0 : {
                . = ALIGN(0x4) ;
                _stext = . ;
				_start = . ;
                *(.text)
                *(.text.*)
                *(.gnu.warning)
                *(.stub)
                *(.gnu.linkonce.t*)
                *(.glue_7t)
				*(.glue_7)
                *(.init)
                . = ALIGN(0x10) ;
                _etext = . ;
        } > flatmem
        .data : {
                . = ALIGN(0x4) ;
                _sdata = . ;
				__data_start = . ;
				data_start = . ;
				*(.rela.got)
				*(.rel.got)
				*(.got.plt)
				*(.got)
				LONG(-1)
                *(.rodata)
                *(.rodata.*)
                *(.gnu.linkonce.r*)
                *(.rodata1)
                *(.data)
                *(.data.*)
                *(.gnu.linkonce.d*)
                *(.data1)
                *(.eh_frame)
                *(.gcc_except_table)
                *(.sdata) 
                *(.sdata.*)
                *(.gnu.linkonce.s.*)
				. = ALIGN(4) ;
				*(.ctors.*)
				*(.ctors)
				LONG(0)
				*(.dtors.*)
				*(.dtors)
				LONG(0)
                . = ALIGN(0x10) ;
                _edata = . ;
        } > flatmem
        .bss : {
                . = ALIGN(0x4) ;
                _sbss = ALIGN(0x4) ;
				__bss_start__ = . ;
                *(.dynsbss)
                *(.sbss)
                *(.sbss.*)
                *(.scommon)
                *(.dynbss)
                *(.bss)
                *(.bss.*)
                *(COMMON)
                . = ALIGN(0x4) ;
                __bss_end__ = . ;
				_end = . ;
				end = . ;
        } > flatmem
}
