// ============================================================================
// md5_hmac_finish —— ORIG T 0x080b4098，size 0x8b。
//
// 反汇编要点（objdump -d ORIG 0x080b4098..0x080b4123）：
//   ① md5_finish(ctx, &tmp)（tmp@-0x18，0x10 字节）—— 取 inner 摘要；
//   ② md5_starts(ctx)；
//   ③ md5_update(ctx, ctx+0x98, 0x40) —— opad 块（md5_hmac_ctx +0x98）；
//   ④ md5_update(ctx, &tmp, 0x10)（长度立即数 0x10 落 0x8(%esp)）；
//   ⑤ md5_finish(ctx, out)（out = 0xc(%ebp)）；
//   ⑥ memset(&tmp, 0, 0x10)。
//   帧：sub $0x28。
// ============================================================================
void md5_hmac_finish(md5_context* ctx, unsigned char* out)
{
    unsigned char tmp[16];
    md5_finish(ctx, tmp);
    md5_starts(ctx);
    md5_update(ctx, ((md5_hmac_ctx*)ctx)->m_opad, 0x40);
    md5_update(ctx, tmp, 0x10);
    md5_finish(ctx, out);
    memset(tmp, 0, 0x10);
}