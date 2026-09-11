// ============================================================================
// md5_hmac_update —— ORIG T 0x080b4077，size 0x21（唯一语义 = 转发 md5_update）。
//
// 反汇编要点（objdump -d ORIG 0x080b4077..0x080b4098）：实参 len→0x8(%esp)、
// data→0x4(%esp)、ctx→(%esp)，`call md5_update`；`leave; ret` 不置 %eax ⇒ void。
// 帧：sub $0x18。
// ============================================================================
void md5_hmac_update(md5_context* ctx, unsigned char* data, int len)
{
    md5_update(ctx, data, len);
}