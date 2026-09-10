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
    opt.sec  = m_option2d8.m_sec16;
    opt.flag = m_option2d8.m_field2;
    return opt;
