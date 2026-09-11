// === BIO_free_all @0x087ec320 ===============================================
// ORIG 循环形态：每轮读 references(+0x2c)/next(+0x24) 后调 BIO_free(b)
//（其内部再 decref），随后手动 dec 本地计数差一次（87ec344 补偿），直到本地
// 计数 ≤0 才前进 next；next 空则退出。
void BIO_free_all(void* head)
{
    bio_st_st* b = static_cast<bio_st_st*>(head);
    if (b == 0) return;                              // 87ec32b
    for (;;) {
        int delta = b->references;                   // 87ec336 [+0x2c]
        bio_st_st* nxt = b->next;                    // 87ec339 [+0x24]
        BIO_free(b);                                 // 87ec33f
        delta -= 1;                                  // 87ec344 sub $1,%ebx
        if (delta > 0) continue;                     // 87ec347 jle→前进
        b = nxt;                                     // 87ec330..334
        if (b == 0) break;
    }
}