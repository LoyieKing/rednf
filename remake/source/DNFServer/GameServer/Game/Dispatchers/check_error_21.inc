// b28d-solo-Dispatchers verdict=AE(routed) [P310/P209附] ORIG 0x08264538 orig/ours=20/23
//   ORIG 两段形 `mov -0x4(%ebp),%eax; cmp $0x351,%eax; jle`（P209 附：signed 局部
//   被 4.4 折 cmpl mem,imm）。翻案 = 局部 unsigned + 使用点 `(int)wv` 比较
//   （P310 变体；探针 /tmp/b28d_disp/probe_wv.cpp k1 逐条命中两段+jle 方向）。
int Dispatcher_ScanBotByDll::check_error(CUser* user, MSG_BASE& msg)
{
    (void)user;
    // ORIG 0x08264538（报告 Dispatcher_ScanBotByDll/check_error.md 全文已读）：
    // v = msg+0xd + 0x352（slot -0x4）；v<=0x351 || v>0x383 → 0x407；否则 0。
    // ORIG 比较走「load 槽→reg cmp→jle 短路共享体」。
    // 槽位对齐（技巧21 声明序）：ORIG req ptr@-0x8（先声明远槽）、window@-0x4。
    const MSG_SCAN_DLL* req = reinterpret_cast<const MSG_SCAN_DLL*>(&msg);
    // [b28d-solo-S162] 单一局部 wv（=ORIG -0x4 槽）；unsigned 声明 + (int) 比较
    //   破 4.4 的 cmpl-mem 折叠（P310/P209 附，禁 register 绑定）。
    //   短路 `||` 双条件形（探针 probe_wv.cpp k8）逐条复现 ORIG 两段+jle 布局。
    unsigned int wv = req->m_field0d + 0x352;
    // ORIG 0x826455a `jle → 407` / 0x8264564 `jle → LOK`（两守卫共享 407 岛）。
    if ((int)wv <= 0x351 || (int)wv > 0x383)
        return 0x407;    // 0x8264566：407 岛
    return 0;            // 0x826456d
}