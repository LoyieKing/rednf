// ORIG T 0x08264b66 _ZN21DisPatcher_FinishLoad4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/read.md ## 汇编 + ## 反编译 C
// 全文转写）。两次 PacketBuf::get_int 分别写 m_0d/m_11（msg+0x0d/+0x11 具名
// 字段）；任一失败即以 LineFunc(0x5f7/0x5f8) 返回（第三、四参恒 0），全成返回 0。
int DisPatcher_FinishLoad::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_FINISH_LOADING& req = *reinterpret_cast<MSG_FINISH_LOADING*>(&msg);
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x5f7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_11))
    {
        return LineFunc(0x5f8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
