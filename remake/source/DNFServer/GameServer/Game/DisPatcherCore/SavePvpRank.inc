bool DisPatcher_ResPvpRank::SavePvpRank(CUser* user, MSG_RES_PVP_RANK& msg)

{
    // [DpcMisc-R2 2026-09-11 残差说明] 父泳道读数 198/198 持平（verdict 仍 DIFF）。
    // 剩余三处为跨版本族：① update_pvp_rank 具名 bool 槽（ORIG 0x081f07c5
    // `mov %al,-0x1d(%ebp); movzbl -0x1d(%ebp),%eax`）——补则 +2 抵消 recv 块序收益，
    // 暂不补；② ORIG 0x081f06b3 `mov -0x1c(%ebp),%eax; cmp -0x18(%ebp),%eax; jbe`
    // vs 我方 `mov -0x18(%ebp),%eax; cmp -0x1c(%ebp),%eax; jae`（同语义 fold，N18b）；
    // ③ 两处实参 callee-saved 角色（-0x10/-0x18 的 ebx↔edx，N7）。
    // ORIG 0x081f05ea T（报告 DisPatcher_ResPvpRank/SavePvpRank.md 全文转写）：
    // 分数(0x89=视图+0x7c)/时间(0x8d=视图+0x80) 反作弊 → job 3/4-grow2 折算 → update_pvp_rank
    // → PVPRoom SetPVPResult/recv 链。
    // ORIG 0x081f05f2/0x081f05f8 双 `movl $0x0` 预清（P198③ 两句式声明）
    unsigned int score = 0;
    int playSec = 0;
    score = (unsigned int)msg.m_result.m_field7c;   // msg+0x89（视图 +0x7c）
    playSec = (int)msg.m_result.m_field80 / 1000;   // msg+0x8d（视图 +0x80），有符号除
    if (10000 < score)
    {
        // [G5-MiscC] P0/P198③：ORIG 0x081f063b/0x081f0642 双 `movl $0x0` 预清
        // （-0x10=商 / -0xc=余数 两个具名 int），随后 0x081f065e 写商、0x081f067e
        // 写余数，0x081f06b9 `mov -0xc(%ebp),%eax; mov %eax,-0x1c(%ebp)` 才回写 score
        // ⇒ 原码是「声明=0 + 赋值语句」两句式（我方原初始化式 + 直接回写无 -0xc 临时）。
        unsigned int over = 0;
        unsigned int rest = 0;
        over = score / 10000;
        rest = score % 10000;
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x76, 1, (int)over, (int)playSec);
        score = rest;
    }
    // ORIG _DAT_08bd6320=0.5（job3）/ _DAT_08bd6328=0.8（其余）双精度折算（(int) 截断语义）
    // [G5-MiscC] P0/P198②：ORIG 0x081f06ca-0x081f06fc 该 bool 无栈槽（三重条件各自
    // 重发 `call get_charac_job`/`call getCurCharFirstGrowType`，`mov $1/$0,%eax` 寄存器
    // 物化后 `test %al,%al; je <0.8 块>`）⇒ 原码是单条 if 内联短路表达式，非具名 bool。
    if (user->get_charac_job() == 3 ||
        (user->get_charac_job() == 4 && user->getCurCharFirstGrowType() == 2))
    {
        score = (unsigned int)(0.5 * (double)score);
    }
    else
    {
        score = (unsigned int)(0.8 * (double)score);
    }
    // ORIG 折算后直接写回 score（-0x1c），无 adjusted 具名局部（P0）
    // [DPC-SigSmall 2026-09-11] 负结论（probe7/probe7b，c6446r -m32 -O0 直证）：
    // ORIG 0x081f06b3 `mov -0x1c(%ebp),%eax; cmp -0x18(%ebp),%eax; jbe`（eax=score /
    // mem=playSec，无符号）在 4.4.6 下**四种等价写法全部归一**为
    // `mov -0x18,%eax; cmp -0x1c,%eax; jae`：`playSec < score` / `score > playSec` /
    // `score <= (unsigned)playSec` / `(unsigned)playSec >= score` 产物逐字节相同
    // （唯一能改操作数序的 `(int)` 混合比较 s3 反产**有符号** jge，与本处 unsigned
    // jbe 不符）⇒ 维持 N18b 封顶（跨版本操作数选择，源码形不可择）。
    if (playSec < score)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x73, 1, (int)score, (int)playSec);
    }
    // [b28e-DispCore] 负结论：补 `bool rankUpdated = update_pvp_rank(...)` 具名槽
    // 实测**劣化**（197/195 ndiff5 → 197/198 ndiff16：score/playSec 槽整体深移 4
    // 且 -0x1d 未出现）⇒ 已回滚；-0x1d 槽属 4.1 独有形（需 PvP_Room.h bool 化的
    // 净改善前提），判 N7/N18a。
    if (!user->update_pvp_rank(msg.m_result))
    {
        return false;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return false;
    }
    // [DpcMisc-S1] P0/P183 删具名 seat：ORIG 0x081f080e `call get_user_seat` 后
    // 结果**直落实参**（0x081f0817 `mov %eax,0x4(%esp)`，第三实参 &msg.m_result
    // 由 0x081f0801 `lea 0xd(%eax),%ebx` 预装载），全函数无 seat 槽（room 槽
    // = -0x14）；改前我方 `mov %eax,-0x14(%ebp)` + `mov -0x14(%ebp),%eax` 两条
    // 具名槽往返，并令 score/playSec/room 槽整体下移 4B（-0x20/-0x1c/-0x18 vs
    // ORIG -0x1c/-0x18/-0x14）。4.4.6 探针实测（具名 seat 4 条 vs 内联 2 条）
    // ⇒ 预期 our −2（199→197 = ORIG），槽位全对齐。
    // 注：update_pvp_rank 的 -0x1d 具名 bool 槽（ORIG 0x081f07c5）本件**不补**：
    // 补则 +2 抵消本项；且 recv_pvp_rank_all 的 4 条归一岛须先由 PvP_Room.h
    // `int → bool` 修掉（见交付报告 .h 项）才谈得上净改善。
    // p167 实测：ORIG calls=16 / OUR calls=16，**CALLSITE CLEAR**。
    room->SetPVPResult(room->get_user_seat(user), msg.m_result);
    if (!room->set_recv_pvp_rank_flag(user))
    {
        return false;
    }
    // [DpcMisc-P4 2026-09-11] 轴 B 控制流（ORIG 直证）：ORIG 0x081f0853 `call
    // recv_pvp_rank_all; test %al,%al; je 0x081f0862`（= **true 分支在物理前**、
    // false 走 else 段），即原码是**正极性** `if (recv_pvp_rank_all()) { send_pvp_end(); }
    // else { …count==1… }`；旧形 `if (!recv_pvp_rank_all())` 把两分支物理序写反
    // （4.4 产 `sete` 归一 + 相反落点）。翻转后与 ORIG 块序一致。
    // 残余 2 条 = callee `PvP_Room::recv_pvp_rank_all()` 返回类型非 bool（ORIG 调用点
    // 无 setne 归一，P292 档①直证）⇒ 需 PvP_Room.h int→bool，属他泳道写域，已上报。
    if (room->recv_pvp_rank_all())
    {
        room->send_pvp_end();
    }
    else
    {
        if (room->get_recv_pvp_rank_count() == 1)
        {
            room->insert_timer_recv_pvp_rank();
        }
    }
    return true;
}
