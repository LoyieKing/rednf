// +0x2d8 为 CountinuousItemOption 子对象（m_option2d8：3 独立字节）。
CStackableItem::CoolTimeOption CStackableItem::GetCoolTimeOption()
{
    CoolTimeOption opt;
    // b28c-L8：ORIG 0x086946d2 = `movzwl 0x2d8(%edx),%ecx; mov %cx,(%eax)` 16 位
    // 直读（uchar 视图实测反劣 +1：4.4 对 uchar→ushort 追加 movzbl %dl 归一）。
    // 残差 1 条 = 4.4 第二读复用 %edx 覆盖基址 ⇒ 需重载 0xc(%ebp)，而 4.1 用
    // %ecx/%edx 分列（N7/N18a 型寄存器分配固有差，实测两形均不可翻）。
    // [b28d-solo-N7] fresh 10/11 判 N7。本波补测：聚合初始化形 10/13 反劣
    // （先 movw/movb 清零再赋值）已回退；类型旋钮无可用（sec/flag 均 1/2 字节 packed，
    // 改型即改 sizeof=3；返回类型 CoolTimeOption 固定 sret）。
    // [S1Charac 2026-09-11 新 5 形·全部回滚] K1 ptr-arith 视图 13（多 2 条 add）、
    // K2 3 字节子对象整体拷贝 14——**唯一复现 ORIG 读序** `movzwl 0x2d8(%edx),%ecx;
    // mov %cx,(%eax); movzbl 0x2da(%edx),%edx` 者，但 gcc 额外把 sret 指针保存到
    // %ebx（push/mov ebx/mov ebx,eax/pop 共 4 条）⇒ 净 +4 反劣、K3 逗号表达式 11
    // （=基线不变）、K9 同型 aggregate 赋值 16（退化 call memcpy）、K15 具名 const
    // 引用 15（引用落 -0x4 槽）。⇒ 维持 10/11 N7 封顶：4.4 RA 对「同一 this 基址
    // 的两次读」恒把首读结果放 %edx 并 reload this，4.1 的 %ecx 分列形在该后端
    // 只能由 aggregate-copy 路径触发（K2），而该路径的 PARM_DECL 保存代价不可去。
    opt.sec  = m_option2d8.m_sec16;
    opt.flag = m_option2d8.m_field2;
    return opt;
}