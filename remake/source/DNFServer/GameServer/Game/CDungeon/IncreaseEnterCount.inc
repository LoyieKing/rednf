void TOD_UserState::IncreaseEnterCount()
{
    // [b28d-solo-CDungeon 判词 · N4/P30 封顶] ORIG 0x0864388a / our 34（ORIG 35，
    // gap 1）；p167 4=4 CLEAR。残余唯一：ORIG `movzwl 0x16; add $0x1,%eax;
    // mov %eax,%edx` vs 4.4 `lea 0x1(%eax),%edx`（add-vs-lea 融合）。本轮再试
    // `++` / `+= 1` / `(unsigned short)(x+1)` 三形全产 lea，与 q1-q6 六形探针
    //（f+1 / +=1 / 1+f / (ushort)cast / register int / register ushort）一致 ⇒ 封顶。
    // [b28d-solo-CDungeon2 复验] fresh 编译仍唯一残差；T1 直证 movzwl/mov %dx ⇒ 宽度
    // 确定（unsigned short），残差仅 add-vs-lea ⇒ 按 P321-T5 禁改类型，N18a 封顶。
    m_data.m_enterCount = m_data.m_enterCount + 1;
    m_data.m_lastEnterLayer = m_data.m_lastEnterLayer + 1;
    m_data.m_lastEnterTime = GlobalData::s_systemTime_.getCurSec();
    if (m_data.m_todayLayer.GetLayer() == 0)
        m_data.m_firstEnterTime = GlobalData::s_systemTime_.getCurSec();
    ((charac_expand::CData*)this)->alter();
