// 技能校验和比对（不等记 hack 0x25c）；从战场 m_apcList(party+0xc3c) 删除
// 上报 boss 序号(msg+0x10)，未命中 param=0x15；删空后记时刻、给队长挂 100ms
// 定时器(param3=characNo) 并置 param.m_8=1；msg+0xe==-1 记 hack 0xcb。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd7a2 orig/ours=185/197 ndiff-blocks=25
//   帧差 sub $4c vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x4c,%esp | mov    %eax,-0x24(%ebp) | mov    %eax,%edx
int Dispatcher_BossDieCheck::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x259e,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }

    // [round5 A01] P0/P183：ORIG 仅 -0x24(out 引用)/-0x20(msg 指针) 两槽，无 req/bossSeq/
    //   unkKind 引用别名（各占指针槽 ⇒ 帧 0x4c→0x6c）；msg 指针在**首用点**才声明。
    // [L7DispG1 2026-09-11] ORIG 直证 0x081cd83d `mov 0x14(%edx),%edx`（msg 槽 -0x20 → +0x14）：
    //   ORIG 的 MSG_BOSS_DIE_CHECK **非** pack(1)（int 需 4 对齐 ⇒ m_skillCheckSum 落 +0x14），
    //   而 .h 把该类放进 #pragma pack(1) ⇒ 成员落 +0x12（读成 m_field10+pad 两字节），
    //   属真错位（.h 修正待父泳道裁决，见 report.md）。read_43 侧早已用 pReq 裸偏移 +0x14 规避；
    //   此处用**局部非 packed 视图**（同字段名 + 同 ORIG 偏移），既修语义又保 disp 形。
    struct BossDieCheckMsgView
    {
        char m_head[0xd];          // MSG_BASE 头
        char m_gap0d;              // +0x0d
        unsigned short m_field0e;  // +0x0e
        unsigned short m_field10;  // +0x10
        int m_skillCheckSum;       // +0x14（4 对齐 ⇒ 自动 pad2）
    };
    BossDieCheckMsgView& req = reinterpret_cast<BossDieCheckMsgView&>(msg);   // -0x20
    if (user->getCurCharacSkillCheckSum() != req.m_skillCheckSum)

    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }

    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    // party+0xc3c = CBattle_Field 起始（m_battleField 偏移 0）；每次重发 `lea 0xc3c`（P183）。
    std::vector<int>::iterator it = std::find(party->m_battleField.m_apcList.begin(),
                                              party->m_battleField.m_apcList.end(),
                                              req.m_field10);

    if (it != party->m_battleField.m_apcList.end())
    {
        party->m_battleField.m_apcList.erase(it);
        out.m_a = (short)req.m_field10;

    }
    else
    {
        out.m_4 = 0x15;
    }

    if ((int)party->m_battleField.m_apcList.size() == 0)   // party+0xc48 = BF+0x124
    {
        party->m_battleField.m_apcAllKilledSec = GlobalData::s_systemTime_.getCurSec();
        // [G1Dproc 2026-09-10] ORIG 0x081cd9a4 起：characNo 直接入 %edi（`call get_charac_no;
        //   mov %eax,%edi`）、apcAllKilledSec 重读入 %esi、GetPartyIndex 入 %ebx —— 三者**无具名槽**
        //   （P0/P183）；实参右→左求值序 = characNo → apcAllKilledSec → … → GetPartyIndex ✓ 同 ORIG。
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
            (TIMER_MESSAGE)0x64, (long long)1000,
            party->m_battleField.m_apcAllKilledSec,
            (unsigned int)user->get_charac_no(-1));
        out.m_8 = 1;
    }

    if (req.m_field0e == 0xffff)   // movzwl 直证

    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xcb, 1, 0, 0);
    }
    return 0;
}