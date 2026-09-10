int file_free(bio_st_st* b)
{
    // 87ec700..74d 逐指令还原：
    //   b==0                    → ret 0
    //   shutdown(+0x10)==0      → ret 1（未托管 fp）
    //   init(+0xc)!=0 且 fp!=0 → fclose(fp); ptr=0; flags(+0x14)=0; init=0; ret 1
    //   否则仅 init=0; ret 1
    if (b == 0)
        return 0;
    if (b->shutdown == 0)
        return 1;
    if (b->init != 0) {
        if (b->ptr != 0) {
            std::fclose(static_cast<std::FILE*>(b->ptr));   // fclose@plt
            b->ptr   = 0;
            b->flags = 0;               // ORIG: movl $0,0x14(%ebx)（flags 槽）
        }
    }
    b->init = 0;
    return 1;
