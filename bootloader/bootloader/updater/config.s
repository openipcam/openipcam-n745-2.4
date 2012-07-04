 
	EXPORT system_config
 AREA sys_config, DATA, READONLY
 KEEP
system_config
	DCD	0x000530C0					; EBICON : not need to change.
	DCD	0xFE040080					; ROMCON(Flash) -Unknown type:0xFE000FF0, A29LV800: 0xFE020080, W28J160: 0xFE030080
	DCD	0x000090E3					; SDCONF0(SDRAM) - 8M(2Mx32): 0x000090E3, 4M(1Mx16x2):0x00009062, 16M(4Mx16x2):0x000090E4 			
	DCD	0x010090E3					; SDCONF1 - The same as SDCONF0 with proper base address   
	DCD	0x0000014B					; SDTIME0 - not need to change
	DCD	0x0000014B					; SDTIME1 - not need to change

 AREA dummy_data, DATA, READONLY
	% 1024
	
 END