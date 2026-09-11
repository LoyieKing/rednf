// ===========================================================================
// 9. _debugCommandGuildLevelUp —— 0x0858f64c
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] NEAR（N7 封顶，双后端一致）：
// callee-saved 角色互换——ORIG 主线 edi/esi/ebx（guildkey/characNo/UID）+
// STGuildProxy 段 ecx/edx；4.4.7 与 4.4.6 后端均 edi/ebx/esi + edx/eax
// （非路由可解；callee 侧 cast 旋钮双向试过零变化）。已修：链式 receiver 双处
// （P52）+ proxyInfo.m_agitFlag=0 补写 + GuildDbInfoView 具名偏移（消 add→disp）。
// [本波更新] P226 语句表达式包 SendGuildLevelup 三实参 ⇒ 主线寄存器角色**已对齐
// ORIG**（edi/esi/ebx，操作数级 ndiff 33→29）；残余仅 STGuildProxy 段 ecx/edx（ORIG）
// vs edx/eax（我方，4.4 双临时分配族）——该段包语句表达式实测零变化 ⇒ 判 N7 封顶。
// P167 callsite CLEAR 15/15。
bool DisPatcher_DebugCommand::_debugCommandGuildLevelUp(CUser* user)
{
    if (!user->isGMUser())
        return false;

    if (user->get_charac_guildkey() == 0)       // +0x25 guildkey==0
    {
        // P227：ORIG 先求 getCurCharacNo（%ebx）再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x20a, 0)(
            "GM is not Guild Member!( %d )", user->getCurCharacNo());
        return false;
    }

    // P52 链式 receiver：ORIG 实参先物化（%edi/%esi/%ebx）后 GetServerProxy，
    // 具名 proxy 局部反致落槽 + 实参寄存器角色漂移。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))                      // +0xc1 GetServerProxy(...,0)
        ->SendGuildLevelup(({ user->GetUID(); }),
                           ({ user->getCurCharacNo(); }),
                           ({ user->get_charac_guildkey(); }), 0);  // +0xdd

    char* dbInfo = reinterpret_cast<char*>(user->GetGuildDBInfo());  // +0xe2（raw DB blob，
                                                                     // 沿用仓库裸读先例）
    // §4.3 具名视图（本 TU 私有，零新符号）：ORIG 用 disp 形
    // `movzbl 0x95(%eax),%eax`/`movzbl 0x1b(%eax),%eax`，裸 char* 下标在 4.4
    // 产 add+解引用（N107 族）⇒ 视图成员访问折叠回 disp。
    struct GuildDbInfoView
    {
        char m_pad00[0x1b];             // +0x00..0x1a
        unsigned char m_level;          // +0x1b
        char m_pad1c[0x95 - 0x1c];      // +0x1c..0x94
        unsigned char m_powerSide;      // +0x95
    };
    STGuildProxy proxyInfo(dbInfo,                              // ctor(name,lev,powerSide)
                           (unsigned char)(((GuildDbInfoView*)dbInfo)->m_level + 1),  // 等级 +1
                           (unsigned char)((GuildDbInfoView*)dbInfo)->m_powerSide);
    proxyInfo.m_agitFlag = 0;   // ORIG 0x0858f78c `movb $0x0,-0x2d(%ebp)`（+0x19）
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))                      // +0x128 再取一次
        ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(),
                               proxyInfo);                      // +0x158 ModifyGuildInfoProxy
    return true;
}