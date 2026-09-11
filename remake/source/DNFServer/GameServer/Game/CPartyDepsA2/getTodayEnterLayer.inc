// ============================================================================
// TOD_UserState::getTodayEnterLayer —— ORIG T 0x0864383e
//   （报告 TOD_UserState/getTodayEnterLayer.md）：sret 返回
//   *(const TOD_Layer*)(this+0x14) + 1（经 TOD_Layer(ushort) 构造，>100 回卷 1；
//   构造回卷语义见 TOD_Layer/TOD_Layer.md 与 operator+ 尾部 resetLayer）。
// ============================================================================
TOD_Layer TOD_UserState::getTodayEnterLayer() const
{
    // ORIG：return *(const TOD_Layer*)(this+0x14) + 1
    //   （经 TOD_Layer::operator+(int)，sret 链与 ORIG 一致）；
    //   this+0x14 = TOD_UserState 内嵌 m_data.m_todayLayer（Sec 批 SIG 内嵌化，
    //   字段本体即 TOD_Layer。ORIG 0x864383e 单 call：operator+ 的 sret 直通
    //   本函数外层 sret（0x8(%ebp)），无临时/二段拷贝 ⇒ 源形为成员直 + 1。
    return m_data.m_todayLayer + 1;
}