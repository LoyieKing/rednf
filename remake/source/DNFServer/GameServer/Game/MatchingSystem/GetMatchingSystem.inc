int CMatchingSystem::GetMatchingSystem(ENUM_PVP_MATCHING_TYPE type)
{
    // ORIG 0x085dfba0：type>5 返回 0（`ja` 直通直返 eax=0）；否则跳表逐 case 内查
    // 空槽并懒构造缓存于 m_systems[type]，共同出口 `mov (this+idx*4),%eax; mov -0x1c`
    // 走 result 局部（-0x1c）。判空在 case 内，非 switch 前一次性预查。
    // [b21 N·N90] 结构性修正：跳表逐 case 判空 + result 局部（-0x1c）对齐 ORIG。
    //   残余 N90 值初始化 codegen：ORIG(4.1) 对三个 4 字节空 IMatch 子类
    //   （CNormalMatch/CLadderMatch/CTournamentMatch）在 `new <T>()` 后、ctor 前发
    //   `movl $0x0,(%eax)` value-init 预清，4.4 对该 trivial 用户 ctor 类不发（ctor 直设
    //   vptr）。源形不可复现（probe 实证 4.4 恒不预零）。父子母各 case new+ctor+槽位
    //   写回、返回统一走 m_systems[type]，语义闭合，封 N。
    int result = 0;
    switch (type)
    {
    case 0:
        if (!m_systems[type]) m_systems[type] = new CNormalMatch();
        break;
    case 1:
        if (!m_systems[type]) m_systems[type] = new CLadderMatch();
        break;
    case 2:
        if (!m_systems[type]) m_systems[type] = new CLeagueMatch();
        break;
    case 3:
        if (!m_systems[type]) m_systems[type] = new CTournamentMatch();
        break;
    case 4:
        if (!m_systems[type]) m_systems[type] = new online_preliminary::COnlinePreliminary();
        break;
    case 5:
        if (!m_systems[type]) m_systems[type] = new fair_pvp::CFairMatch();
        break;
    default:
        return 0;
    }
    result = (int)m_systems[type];
    return result;
