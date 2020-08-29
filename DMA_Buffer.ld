SECTIONS
{
    .dma_buffer : /* Space before ':' is critical */
    {
    *(.dma_buffer)
    } >RAM_D2
}