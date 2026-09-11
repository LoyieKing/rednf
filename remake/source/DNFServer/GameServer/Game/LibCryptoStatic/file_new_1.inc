// ---- file_new（ORIG t 0x087ec6d0）----
int file_new(bio_st_st* bi)
{
    bi->init = 0;       // +0x0c
    bi->num = 0;        // +0x1c
    bi->ptr = 0;        // +0x20
    bi->flags = 0;      // +0x14
    return 1;
}