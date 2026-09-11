// [b28d-solo-DisPatcherCore] 修复：m_rankInfo 由 char[0x14a] 改真实元素数组
// ST_Guild_War_Rank_Info[10]（头 R2 广播），ctor 删手写 placement-new 循环
// ⇒ 编译器隐式数组成员构造复现 ORIG P151 倒计数循环（esi=9→-1、ebx 步进 0x21，
// 无 _ZnwjPv/size 装载）；N83 的 placement-new 多 2 条消除。
Packet_Notice_Guild_War_Info::Packet_Notice_Guild_War_Info()

    : PacketHeader(0x414, 0x15c)
{
    // ORIG +0x2f 循环由编译器隐式发射（数组成员构造）；函数体为空。
}
