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
