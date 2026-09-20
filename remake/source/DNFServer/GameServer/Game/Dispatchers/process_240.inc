// ORIG 0x081d33a0（报告 Dispatcher_Delete_GuildBoard_Text/process.md）：
// param+0x89(unsigned)=待删公告文本索引；无状态门槛。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d33a0 orig/ours=39/35 ndiff-blocks=6
//   帧差 sub $2c vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
int Dispatcher_Delete_GuildBoard_Text::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d33a0 槽形直证（objdump）：
    //   81d33a9 `mov 0x14(%ebp),%eax; mov %eax,-0x20(%ebp)` → param(+0x14) 死引用槽（此后从未被读）
    //   81d33af `mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)` → msg(+0x10) 引用槽
    //   81d33cf `mov -0x1c(%ebp),%eax; mov 0x89(%eax),%edi` → 字段读自 **msg** 而非 param
    //   msg+0x89 = MSG_GUILD_BOARD::m_info(+0xd) 的 STGuildBoardDBInfo::m_7c(+0x7c)；
    //   同源直证：read_243 ORIG 0x081d3354 `mov 0x10(%ebp),%eax; add $0x89,%eax`（同一 +0x10 槽）。
    //   [J1·pack 例检负结论] 原按 param+0x89 建的 packed GuildBoardParamView 系对象误判（槽 -0x1c ← +0x10
    //   = msg），ParamBase 侧无该字段；该视图已删。
    ParamBase& deadParam = param;   // -0x20 死槽照抄（声明序居首 ⇒ 槽序与 ORIG 同）
    (void)deadParam;
    // 局部视图：m_7c 取 unsigned int（SendGuildBoardDelete 第 1 形参即 unsigned int，
    //   省掉 int→unsigned 的中间 mov；ORIG 0x081d33d2 `mov 0x89(%eax),%edi` 单条直载。
    MSG_GUILD_BOARD& boardMsg = reinterpret_cast<MSG_GUILD_BOARD&>(msg);
    // ORIG 0x081d33a0 段：guildkey→esi / characNo→ebx / textIdx→edi 三实参右到左求值，
    //   跨 receiver 链（P52）保活，无具名槽 ⇒ 全内联进实参位
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildBoardDelete(
            static_cast<unsigned int>(boardMsg.m_info.m_7c),
            user->getCurCharacNo(),
            (unsigned int)user->get_charac_guildkey());
    return 0;
}