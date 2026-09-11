// ============================================================================
// md5_hmac_starts —— ORIG T 0x080b3f62，size 0x115。
//
// 反汇编要点（objdump -d ORIG 0x080b3f62..0x080b4077）：
//   ① `cmpl $0x40,0x10(%ebp); jle` ⇒ `if (keylen > 0x40)`；体内
//      md5(key,keylen,sum)（sum@-0x1c，0x10 字节），随后两条直写形参槽：
//      `movl $0x10,0x10(%ebp)` / `lea -0x1c; mov %eax,0xc(%ebp)` ⇒ `keylen = 0x10;
//      key = sum;`（P322 参数对象语义：写的是形参槽，不是新局部）。
//   ② memset(ctx+0x58,0x36,0x40) / memset(ctx+0x98,0x5c,0x40)：ipad/opad 初值
//      （md5_hmac_ctx 视图 +0x58 / +0x98）。
//   ③ for 循环 i@-0xc（`movl $0x0,-0xc` + 入口 jmp 条件 + 尾 `cmp 0x10(%ebp),%eax;
//      setl %al; test %al; jne`，P2/P25）：体内两条独立读改写
//      `c->m_ipad[i] ^= key[i];` / `c->m_opad[i] ^= key[i];`（索引各读 3 次、
//      movzbl 两侧、`xor %edx,%ecx`）。
//   ④ md5_starts(ctx) → md5_update(ctx, ctx+0x58, 0x40) → memset(sum,0,0x10)。
//   帧：sub $0x38（无 callee-saved push）。
// ============================================================================
void md5_hmac_starts(md5_context* ctx, unsigned char* key, int keylen)
{
    unsigned char sum[16];
    if (keylen > 0x40)
    {
        md5(key, keylen, sum);
        keylen = 0x10;
        key = sum;
    }
    // P183：不缓存视图指针（`md5_hmac_ctx* c = (…)ctx;` 会让 4.4 多占一个指针
    // 槽并把后续每次访问改成槽回读；ORIG 每次就地 `mov 0x8(%ebp),%eax; add $0x58`）。
    memset(((md5_hmac_ctx*)ctx)->m_ipad, 0x36, 0x40);
    memset(((md5_hmac_ctx*)ctx)->m_opad, 0x5c, 0x40);
    int i;
    for (i = 0; i < keylen; ++i)
    {
        ((md5_hmac_ctx*)ctx)->m_ipad[i] ^= key[i];
        ((md5_hmac_ctx*)ctx)->m_opad[i] ^= key[i];
    }
    md5_starts(ctx);
    md5_update(ctx, ((md5_hmac_ctx*)ctx)->m_ipad, 0x40);
    memset(sum, 0, 0x10);
}