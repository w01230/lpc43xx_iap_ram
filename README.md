# lpc43xx_iap_ram
## iap implement 
ide:keil v5
## swd samples

```C
  /* set sp/pc/xpsr/vector table/shadow pointer */
  state |= writecore(CORTEXMX_WR_R(13), sp);
  state |= writecore(CORTEXMX_WR_R(14), 0xFFFFFFFF);
  state |= writecore(CORTEXMX_WR_R(15), pc | 1);
  state |= writecore(CORTEXMX_WR_XPSR, XPSR_THUMB_BIT);

  state |= writemem(nvic_base_address + VECTOR_TABLE, CODE_DATA);
  state |= writemem(SHADOW_POINTER, CODE_DATA);

  /* run the core */
  state |= writemem(nvic_base_address + DHCSR_ADDR_OFFSET, DHCSR_C_DEBUGEN);
 ```
## note
load code to ram by SWD and send data source and command via SWD.

