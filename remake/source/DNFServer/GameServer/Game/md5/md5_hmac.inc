// ============================================================================
// md5_hmac —— ORIG T 0x080b4123，size 0x76（一步式 HMAC-MD5）。
//
// 反汇编要点（objdump -d ORIG 0x080b4123..0x080b4199）：
//   ctx 局部 @-0xe0（0xd8 字节 ⇒ 与 md5_hmac_ctx 视图同尺寸）；
//   md5_hmac_starts(&ctx,key,keylen) → md5_hmac_update(&ctx,in,inlen) →
//   md5_hmac_finish(&ctx,out) → memset(&ctx,0,0xd8)；`leave; ret` 不置 %eax ⇒ void。
//   帧：sub $0xf8。
// ============================================================================
void md5_hmac(unsigned char* key, int keylen, unsigned char* in, int inlen, unsigned char* out)
{
    // 用 md5_context 本体（非 0xd8 承载的 cast 形）：cast 形虽使槽基 -0xe0 与 ORIG 相同，
    // 但 P277「cast 前置」会把 ctx 的 lea 提前到实参装载之前，助记符序列反 DIFF；
    // 本体形保持 NEAR（仅 lea 槽基 -0xe8 浅 8，N95 族）。
    md5_context ctx;
    md5_hmac_starts(&ctx, key, keylen);
    md5_hmac_update(&ctx, in, inlen);
    md5_hmac_finish(&ctx, out);
    memset(&ctx, 0, 0xd8);
}