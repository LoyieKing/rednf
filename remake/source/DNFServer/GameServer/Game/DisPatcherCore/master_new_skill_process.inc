// ORIG 0x081bf952 T _ZN19DisPatcher_BuySkill24master_new_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/master_new_skill_process.md 全文转写）：
// 非公会分支（+0x1bf..+0x2ad）：树门 treeIdx∈{-1,0} 时 treeKind==1 返回 / 其余
// treeKind!=1 返回 → SkillSlot::master_new_skill(idx, level, job, first, second,
// (s8)m_1)，结果写 m_4，≥0 时 m_8←getCurCharacSkillR().get_skill_level(m_4)。
// 公会分支（+0x02b..+0x1ba）：IsGuildMaster/IsSubGuildMaster 皆假直落尾部（沿用
// 调用前遗留 m_4 判定）；否则 master_new_guild_skill(...,needGold,&info.m_8,needGSP,
// *(u16*)(GetGuildDBInfo()+0x42))，成功经 GlobalData::s_guild_proxy_mgr 发送
// SendBuyGuildSkill(get_charac_no(-1), get_charac_guildkey(), m_0, info.m_8,
// needGSP, needGold)。
// [b28d-29] N82 + P168a：帧 0x7c vs 0x9c + 取反物化方向（xor $1 位置），调用点全等  (ORIG 0x081bf952 insn 239->248)
// 尾部（+0x2b3）：m_4>=0 时 HistoryLog::WriteLearnSkill(m_historyLogFp, m_0, 当前等级)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 297（ORIG 234 / ours 243），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
void DisPatcher_BuySkill::master_new_skill_process(CUser* user, char treeKind,

                                                   buySkillInfo& info)
{
    // [DPC_CheckErr 负结论 2026-09-11] 本件 ORIG 239 / 我方 235（我方少 4，负 gap）。
    // 逐槽核对后，ORIG 独有段全部是 4.1 的**实参物化临时槽**族，4.4 直接入栈不可造：
    //  · 0x081bf9c1-0x081bf9c7 `mov 0x14(%ebp),%eax; add $0x8,%eax; mov %eax,-0x58(%ebp)`
    //    （&info.m_8 先落 -0x58 槽，0x081bfa36 才从槽写入实参 0x20(%esp)）；
    //  · 0x081bfa84/0x081bfa8a/0x081bfa92 把 needGold(-0x20)/needGSP(-0x22)/info.m_8
    //    回读后**再存** -0x44/-0x40/-0x3c 三个新槽，SendBuyGuildSkill 实参再从槽读。
    // 我方链式调用（P52/P183 已内联 guildLevel/job/skillR）4.4 直接 `lea`+`mov` 入栈，
    // 无等价源码形；另 ORIG 0x081bf967/0x081bf975 `cmp $0xc7,%al`/`cmp $0xd0,%al` 为
    // 8 位比较（源码 (unsigned char)info.m_0 判定），4.4 亦产 8 位比较，非差异源。
    // ⇒ N 封顶（4.1 实参求值/物化策略），不做消指令（越改越远）。
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // ---- 公会技能分支（ORIG +0x02b..+0x1ba；正区间判定使公会块内联、
        //      ja→非公会，与 ORIG 块排布一致）----
        if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
        {
            // 权限不足：ORIG 直落 +0x2af→尾部（不写 m_4，沿用调用前遗留值）
        }
        else
        {
            // STGuildDBInfoOnly::m_guildSkillPoint（CUser.h:1297 @+0x42 ushort，
            // 与 ORIG 0x081bf9ba `movzwl 0x42(%eax)` 同宽；ORIG 两次独立
            // GetGuildDBInfo 调用（0x081bf9b5 与 guildDB），故不合并成一次调用）
            // [G5-MiscC] P0/P52：ORIG 0x081bf9b5 `call GetGuildDBInfo;
            // movzwl 0x42(%eax),%eax; movzwl %ax,%ebx`（guildSkillPoint 值仅存
            // callee-saved %ebx 跨后续全部调用）、0x081bfa08 `call get_charac_job;
            // mov %eax,%edi`、0x081bfa1e `call getCurCharacSkillR; mov %eax,%edx`
            // —— 三者均**无槽**（我方原具名 guildLevel/job/skillRG 各多占一槽）⇒ 内联
            // 回实参位。内联后 4.4 右→左求值序 = ORIG：guildLevel → info.m_8 → money
            // → guildDB → secondGrow → firstGrow → job → info.m_0 → skillR。
            int money = user->getCurCharacMoney();                      // -0x54
            STGuildDBInfoOnly* guildDB =
                static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo());// -0x50
            int secondGrow =
                static_cast<int>(user->getCurCharSecondGrowType());           // -0x4c
            int firstGrow =
                static_cast<int>(user->getCurCharFirstGrowType());            // -0x48
            int needGold;   // ORIG -0x20：调用前未初始化，由出参引用写出
            short needGSP;  // ORIG -0x22：同上（&别名传参）
            info.m_4 = ((SkillSlot*)(user->getCurCharacSkillR()))
                           ->master_new_guild_skill(
                static_cast<unsigned char>(info.m_0), user->get_charac_job(),
                firstGrow, secondGrow, *guildDB, money, needGold,
                /*learnCnt=*/info.m_8,  // ORIG &info+8 别名 int& 出参
                needGSP,
                static_cast<int>(static_cast<STGuildDBInfoOnly*>(
                    user->GetGuildDBInfo())->m_guildSkillPoint));
            if (info.m_4 >= 0)
            {   // ORIG +0x132..+0x1ba：读回出参并发公会服务器包
                CGuildServerProxy* proxy =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy(
                        user->GetServerGroup());
                proxy->SendBuyGuildSkill(
                    static_cast<unsigned int>(user->get_charac_no(-1)),
                    static_cast<unsigned int>(user->get_charac_guildkey()),
                    static_cast<unsigned char>(info.m_0),
                    /*level(=出参 learnCnt 回读)=*/info.m_8,
                    /*needGSP=*/needGSP, static_cast<unsigned int>(needGold));
            }
        }
    }
    else
    {   // ---- 非公会技能分支（ORIG +0x1bf..+0x2ad）----
        // [DpcMisc-P8 负结论 2026-09-11，已回滚] ORIG 0x081bfb2x 两处
        // `mov -0x1c(%ebp),%eax` + `cmp`/`test` 两段形 vs 4.4 `cmpl $imm,-0x28(%ebp)`
        // 单段。volatile 试形读数 235→**240**（ORIG 239，|Δ|=1）——虽拉近条数，
        // 但父泳道判其属失真写法（volatile 强制每次访问落内存），且同源件
        // try_master 已证明超调（83→88）⇒ 一并回滚，判 N。
        // 其余残差：-0x44/-0x40/-0x3c 三个具名副本（ORIG 0x081bfa84-a98 的
        // gold/gsp/level 回读），我方槽集与 ORIG 分布式错位（我方 needGold@-0x40、
        // needGSP@-0x42 vs ORIG -0x20/-0x22）⇒ 补副本会触发整段槽重排，暂不动。
        int treeIdx = user->GetCurCharacSkillTreeIndex();               // -0x1c
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (treeKind == 1)
            {
                return;
            }
        }
        else if (treeKind != 1)
        {
            return;
        }
        int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));       // -0x38
        int secondGrow =
            static_cast<int>(user->getCurCharSecondGrowType());                       // -0x34
        int firstGrow =
            static_cast<int>(user->getCurCharFirstGrowType());                        // -0x30
        // [G5-MiscC] P0/P52：ORIG 0x081bfb75 `call get_charac_job; mov %eax,%esi` /
        // 0x081bfb82 `call get_charac_level; mov %eax,%ebx` / 0x081bfb98
        // `call getCurCharacSkillW`（this 直入 (%esp)）—— 均无槽 ⇒ 内联回实参位；
        // 求值序右→左 = ORIG：addLevel → secondGrow → firstGrow → job → level
        // → info.m_0 → skillW。
        info.m_4 = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                       ->master_new_skill(static_cast<unsigned char>(info.m_0),
                                          user->get_charac_level(),
                                          user->get_charac_job(), firstGrow,
                                          secondGrow, addLevel);
        if (info.m_4 >= 0)
        {
            // [G5-MiscC] P0/P52：ORIG 0x081bfbe2 `call getCurCharacSkillR;
            // mov %eax,(%esp)` 后直读 `mov 0x4(%eax),%ebx`(=info.m_4) 传参，
            // 结果直接 `mov %eax,0x8(%edx)` 回写 —— 无 skillR 具名槽 ⇒ 内联链式。
            info.m_8 = ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4);
        }
    }
    if (info.m_4 >= 0)
    {   // ---- 尾部（ORIG +0x2b3）----
        // [G5-MiscC] P0/P52：ORIG 0x081bfc15 `call getCurCharacSkillR; mov %eax,(%esp)`
        // / 0x081bfc27 `call get_skill_level`（结果直接 `mov %eax,0x8(%esp)`）——
        // 无 skillRT/curLevel 具名槽 ⇒ 内联链式。
        HistoryLog::WriteLearnSkill(
            reinterpret_cast<FILE*>(user->m_historyLogFp),
            static_cast<unsigned char>(info.m_0),
            ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4));
    }
}
