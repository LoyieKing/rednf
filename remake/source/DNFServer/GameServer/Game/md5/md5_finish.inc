// ============================================================================
// md5_finish —— ORIG T 0x080b3b8d，size 0x1ca。
//
// 反汇编要点（objdump -d ORIG 0x080b3b8d..0x080b3d57）：
//   ① bit 长度高低 32 位（total 为字节计数）：
//      hi@-0x10 = `(total[0] >> 29) | (total[1] << 3)`；lo@-0xc = `total[0] << 3`。
//   ② lenbytes[8] @-0x20..-0x19：lo 4 字节在下、hi 4 字节在上（逐字节 `shr $8/$0x10/$0x18`）。
//   ③ used@-0x18 = `total[0] & 0x3f`；padlen@-0x14 三目
//      `cmpl $0x37,-0x18; ja else` ⇒ `used <= 0x37 ? 0x38 - used : 0x78 - used`（无符号）。
//   ④ `md5_update(ctx, md5_padding, padlen)`（0x080b3c33，立即数 $0x8b1f0e0）
//      → `md5_update(ctx, lenbytes, 8)`。
//   ⑤ 摘要按字节展开直写 out（16 条 `mov state[i]` + `shr`；每次重载 out，无循环）。
//   帧：sub $0x38；hi/lo/lenbytes/used/padlen 五个局部。
// ============================================================================
void md5_finish(md5_context* ctx, unsigned char* out)
{
    // 全头部声明 + 声明序 = ORIG 槽深→浅（P212/P339；4.4 对函数头标量按
    // 「先声明更深」分配）：lenbytes(-0x20) → used(-0x18) → padlen(-0x14)
    // → hi(-0x10) → lo(-0xc)。执行序仍按 ORIG 反汇编（hi、lo、lenbytes、used、padlen）。
    unsigned char lenbytes[8];
    unsigned int used;
    unsigned int padlen;
    unsigned int hi;
    unsigned int lo;
    hi = (((md5_ctx*)ctx)->total[0] >> 29) | (((md5_ctx*)ctx)->total[1] << 3);
    lo = ((md5_ctx*)ctx)->total[0] << 3;
    lenbytes[0] = (unsigned char)lo;
    lenbytes[1] = (unsigned char)(lo >> 8);
    lenbytes[2] = (unsigned char)(lo >> 16);
    lenbytes[3] = (unsigned char)(lo >> 24);
    lenbytes[4] = (unsigned char)hi;
    lenbytes[5] = (unsigned char)(hi >> 8);
    lenbytes[6] = (unsigned char)(hi >> 16);
    lenbytes[7] = (unsigned char)(hi >> 24);
    used = ((md5_ctx*)ctx)->total[0] & 0x3f;
    padlen = used <= 0x37 ? 0x38 - used : 0x78 - used;
    md5_update(ctx, (unsigned char*)md5_padding, (int)padlen);
    md5_update(ctx, lenbytes, 8);
    out[0] = (unsigned char)(((md5_ctx*)ctx)->state[0]);
    out[1] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 8);
    out[2] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 16);
    out[3] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 24);
    out[4] = (unsigned char)(((md5_ctx*)ctx)->state[1]);
    out[5] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 8);
    out[6] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 16);
    out[7] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 24);
    out[8] = (unsigned char)(((md5_ctx*)ctx)->state[2]);
    out[9] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 8);
    out[10] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 16);
    out[11] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 24);
    out[12] = (unsigned char)(((md5_ctx*)ctx)->state[3]);
    out[13] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 8);
    out[14] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 16);
    out[15] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 24);
}