// ---------------------------------------------------------------------------
// sha256_update(sha256_context*, unsigned char*, unsigned long) T @0x080878ec
// （size 0xfe，逐指令转写：ilen==0 直接 ret；left=total&0x3f、fill=0x40-left；
//   total+=ilen 且回绕进位 total[1]++；left!=0&&ilen>=fill 时先补满 buffer 并
//   process(buffer)；while ilen>0x3f 对 input 整组 process；余量 memcpy 入
//   buffer+left。）
// ---------------------------------------------------------------------------
void sha256_update(sha256_context* ctx, unsigned char* input, unsigned long ilen)
{
    if (ilen == 0)                       // cmpl $0x0,0x10(%ebp); je
    {
        return;
    }

    unsigned long left = ctx->m_total[0] & 0x3f;    // and $0x3f,%eax → -0x10(%ebp)
    unsigned long fill = 0x40 - left;               // mov $0x40 / sub → -0xc(%ebp)

    ctx->m_total[0] += ilen;            // old+ilen 回绕写入（mov %edx,(%eax)）
    ctx->m_total[0] = ctx->m_total[0];  // ORIG 冗余 reload/store 对（0x08087921-29，无观察效应）
    if (ctx->m_total[0] < ilen)         // cmp ilen,new; jae skip → 溢出时 new<ilen
    {
        ctx->m_total[1] += 1;           // 进位（lea 0x1(%eax),%edx / mov）
    }

    if (left != 0 && ilen >= fill)      // cmpl $0x0,left; je / cmp fill; jb
    {
        std::memcpy(ctx->m_buffer + left, input, fill);
        sha256_process(ctx, ctx->m_buffer);
        ilen -= fill;                   // sub %eax,0x10(%ebp)
        input += fill;                  // add %eax,0xc(%ebp)
        left = 0;                       // movl $0x0,-0x10(%ebp)
    }

    while (ilen > 0x3f)                 // cmpl $0x3f,ilen; seta
    {
        sha256_process(ctx, input);
        ilen -= 0x40;                   // subl $0x40,ilen
        input += 0x40;                  // addl $0x40,input
    }

    if (ilen != 0)                      // cmpl $0x0,ilen; je
    {
        std::memcpy(ctx->m_buffer + left, input, ilen);
    }
}