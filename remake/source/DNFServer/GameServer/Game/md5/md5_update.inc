// ============================================================================
// md5_update —— ORIG T 0x080b3a8d，size 0x100（**字节计数**版，与 md5_process 配套）。
//
// 反汇编要点（objdump -d ORIG 0x080b3a8d..0x080b3b8d）：
//   ① `cmpl $0x0,0x10(%ebp); jle` ⇒ `if (len <= 0) return;`
//   ② used@-0xc = `ctx->total[0] & 0x3f`；fill@-0x10 = `0x40 - used`
//      （total[0] 为**字节**计数，故不 >>3）。
//   ③ `ctx->total[0] += len`；`cmp %eax,%edx; jae` ⇒ `if (total[0] < len) ++total[1]`（无符号）。
//   ④ `cmpl $0x0,-0xc; je tail` + `cmp -0x10(%ebp),%eax; jl tail` ⇒
//      `if (used != 0 && len >= (int)fill)` 内：memcpy(buffer+used, data, fill) →
//      md5_process(ctx, buffer)（0x080b3b24）→ `data += fill; len -= fill; used = 0;`。
//   ⑤ while 前置 `jmp 条件` + 尾 `cmpl $0x3f,0x10(%ebp); setg %al; test; jne` ⇒
//      `while (len > 0x3f) { md5_process(ctx, data); data += 0x40; len -= 0x40; }`（普通 while，
//      调用点 0x080b3b4b 直接传 data，不经 buffer）。
//   ⑥ 尾 `cmpl $0x0,0x10(%ebp); jle` 守卫 + memcpy(buffer+used, data, len) ⇒
//      `if (len > 0) memcpy(ctx->buffer + used, data, len);`
//   帧：sub $0x28；used@-0xc、fill@-0x10（无 callee-saved push、无指针局部）。
// ============================================================================
void md5_update(md5_context* ctx, unsigned char* data, int len)
{
    if (len <= 0)
        return;
    // 槽序：ORIG used@-0xc / fill@-0x10（4.4 同尺寸标量区按声明序「先声明更深」，
    // 故 fill 先声明、used 后声明；执行序仍是 used 赋值在前）。
    int fill;
    int used = (int)(((md5_ctx*)ctx)->total[0] & 0x3f);
    fill = 0x40 - used;
    ((md5_ctx*)ctx)->total[0] += (unsigned int)len;
    if (((md5_ctx*)ctx)->total[0] < (unsigned int)len)
        ++((md5_ctx*)ctx)->total[1];
    if (used != 0 && len >= fill)
    {
        memcpy(((md5_ctx*)ctx)->buffer + used, data, fill);
        md5_process(ctx, ((md5_ctx*)ctx)->buffer);
        data += fill;
        len -= fill;
        used = 0;
    }
    while (len > 0x3f)
    {
        md5_process(ctx, data);
        data += 0x40;
        len -= 0x40;
    }
    if (len > 0)
        memcpy(((md5_ctx*)ctx)->buffer + used, data, len);
}