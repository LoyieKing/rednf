void rdar_init(char** argv)
{
    // [P6S4 2026-09-11 §6.5 串审计] ORIG 0x0829e5c5 `movl $0x8c18171,(%esp)` ⇒ ORIG 串
    // 实际字节 `./pid`（.rodata 0x8c18171 后紧跟 `pid/%s.pid`，与 rdar_terminate 的
    // `./pid/%s.pid` 同前缀）—— 原写 `./rdar`（注释标「推断」）为错值；且 ORIG 该串在
    // **.rodata**（`movl $<rodata addr>,(%esp)` 直传）⇒ 字面量直传 make_dir（原 static
    // 可写数组令串落 .data，strall 报 O-only）。
    make_dir((char*)"./pid");
    daemon_init(argv);
}