// ORIG W 0x0822f91a：m_selected 非空时置 +0xee9 伪装标志，再依次
// setDisguiseKind(kind)（W 0x0822f976）/ setDisguiseIndex(index)（W 0x0822f99e）。
// [b28d-solo-CUserCharacInfo P210] 本定义必须前置于两个 callee 定义之前：
// 4.4 对 TU 内已见定义的 callee 精确继承 outgoing ⇒ caller 帧塌缩 sub $0x14
// + spill -0x4/-0x8/-0xc；前置后恢复 ORIG 保守帧 sub $0x28 + spill
// -0xc/-0x10/-0x14（探针 /tmp/s156_probe/p11.cpp f0/f1 对照）。
void CUserCharacInfo::setDisguiseCharac(bool flag, unsigned char kind,
                                        unsigned short index)
{
    if (m_selected)
    {
    m_selected->m_fieldEE9 = flag;
    setDisguiseKind(kind);
    setDisguiseIndex(index);
    }
}