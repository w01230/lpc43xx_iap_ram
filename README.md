# lpc43xx_iap_ram
## iap implement 
ide:keil v5
## swd samples

/* set sp/pc/xpsr/vector table/shadow pointer */ <br>
state |= writeCore(CORTEXMX_WR_R(13), sp);<br>
state |= writeCore(CORTEXMX_WR_R(14), 0xFFFFFFFF);<br>
state |= writeCore(CORTEXMX_WR_R(15), pc | 1);<br>
state |= writeCore(CORTEXMX_WR_XPSR, XPSR_THUMB_BIT);<br>

state |= writeMem(m_NVIC_base_address + VECTOR_TABLE, CODE_DATA);<br>
state |= writeMem(SHADOW_POINTER, CODE_DATA);<br>

/* run the core */ <br>
state |= writeMem(m_NVIC_base_address + DHCSR_ADDR_OFFSET, DHCSR_C_DEBUGEN);<br>

## note
load code to ram by SWD and send data source and command via SWD.

