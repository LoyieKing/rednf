// [b28 判词 N77] ORIG 4.1 `new (nothrow) T()` value-init 前置 rep stos 12 dword
// 清零+临时槽；4.4 CWG434 不 emit——placement/显式 memset 变 call 均不可翻，封顶。
// [b28d-18 N77] ORIG new T() value-init 前置 rep stos + 临时指针槽；4.4 不 emit（N77/P90） | ORIG 0x0858860c
online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()
    : m_factory(0)
{
    // ORIG 0x0858860c：+0x00=0；map@+0x04 由编译器构造；new(0x30, nothrow)
    // CTeamFactory；失败 logFormat 后继续。
    // ORIG 0x0858860c 首两条 `mov 0x8(%ebp),%eax; movl $0x0,(%eax)` 先于 map
    // ctor —— m_factory 清零是 mem-init（声明序）而非 body 赋值。
    // N77 定论（DL7 复测）：ORIG 0x0858867e rep stos $0xc dword（0x30 全清）
    // ＝ `new T()` value-init；4.4 CWG434 对 user-ctor 类不 emit（加括号
    // 实测仍无 rep stos）⇒ 连同 0x10 帧差与 ebx/esi/edi 缓存差 N77+N82附 封顶。
    // [b28d-solo-GlobalData] P299/P318：用户空 ctor 已删，此处补括号 value-init
    // 即触发 4.4 前置 rep stos（ORIG 0x0858867e rep stos $0xc dword）。
    m_factory = new (std::nothrow) CTeamFactory();
    if (m_factory == 0)
    {
        LogManager::logFormat(1, "OnlinePreliminaryTeam.cpp",
                              __PRETTY_FUNCTION__,
                              0xd9, "memory error func(%s) line(%d)",
                              "online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()",
                              0xd9);
    }
}