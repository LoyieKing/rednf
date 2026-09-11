// ============================================================================
// md5_file —— ORIG T 0x080b3e61，size 0x101（同 TU：md5 0x080b3d57 / md5_merging
// 0x080b3dbf / 本函数 0x080b3e61 / md5_hmac_starts 0x080b3f62，地址连续 ⇒ 本聚合 TU）。
//
// 反汇编要点（objdump -d ORIG 0x080b3e61..0x080b3f62）：
//   ① fopen(filename, "rb")：mode 串立即数 $0x8b1f0a0（.rodata 逐字节 72 62 00 =
//      "rb"），实参装载序 mode→0x4(%esp)、filename→(%esp)；fp@-0x10。
//   ② `fp == 0`：`cmpl $0x0,-0x10; sete %al; test %al,%al; je` ⇒ `if (fp == 0)
//      return 1;`（常量 1 落尾块）。
//   ③ md5_starts(&ctx)：ctx 槽基 -0xe8（0xd8 字节：-0xe8..-0x10，紧贴上邻 fp/len）。
//   ④ 读循环：`fread(buf,1,0x400,fp)` → len@-0xc；尾测 `cmpl $0x0,-0xc; setne %al;
//      test %al,%al; jne <body>` ⇒ `while ((len = fread(buf,1,0x400,fp)) != 0)
//      md5_update(&ctx,buf,len);`（入口先 `jmp` 条件块 = while 前置形，P25）。
//   ⑤ md5_finish(&ctx,out) → memset(&ctx,0,0xd8)（长度立即数 0xd8）。
//   ⑥ `ferror(fp)!=0` ⇒ fclose+return 2；否则 fclose+return 0（两个独立调用点）。
//   帧：sub $0x4f8；buf[0x400]@-0x4e8、ctx@-0xe8、fp@-0x10、len@-0xc。
// ============================================================================
int md5_file(char* filename, unsigned char* out)
{
    FILE* fp = fopen(filename, "rb");
    if (fp == 0)
        return 1;
    // md5_context 已按 ORIG 实尺寸 0xd8 修正（md5.h），直接用本体、不再 cast
    // （Main 新范式「接收者去 cast ⇒ 实参装载序翻回 ORIG 的实参先于 this」）。
    md5_context ctx;
    unsigned char buf[0x400];
    int len;
    md5_starts(&ctx);
    while ((len = (int)fread(buf, 1, 0x400, fp)) != 0)
    {
        md5_update(&ctx, buf, len);
    }
    md5_finish(&ctx, out);
    memset(&ctx, 0, 0xd8);
    if (ferror(fp) != 0)
    {
        fclose(fp);
        return 2;
    }
    fclose(fp);
    return 0;
}