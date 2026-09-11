// ---- ORIG 0x84a48f8 loadData(CUser*, char*) --------------------------------
// b28d-solo-CFairPvPScore 判词：DIFF·N 封顶（§6.2 三件套：①p167 CLEAR 91=91；
// ②gap（ORIG 848 vs 872）；③帧 0x22c vs 0x27c、全帧槽号平移 = N82/N95 族）。
// [L6S4 round5b] gap 改善：872 → **867**（ORIG 849；gap −24 → −18），修法 =
//   区块 1 条目访问宏改权威成员数组下标（`sig->m_jobScore[i]`）：旧 cast 指针算术
//   把 sig 折进基址 ⇒ 6 个访问点各多 1 条 `lea (%ecx,%eax,1)`；改后复现 ORIG
//   「索引寄存器 ecx + 基址寄存器 eax + disp 0x10」形（常量拆分差仍在：4.4
//   `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`）。⇒ 本 TU 双条目视图
//   FairPvpScoreEntryPair 已无使用点，删除。
// 残余 −18 归因（超大函数，§6.2 口径）：total 段 ORIG 用槽 -0x24 逐项累加
//   （`mov -0x24,%eax; lea (%edx,%eax,1),%eax; mov %eax,-0x24`，每项 6 条）vs
//   我方单表达式寄存器累加（每项 3~4 条）⇒ 该段我方**更省**；净差全部来自块间
//   帧/槽平移（N82/N95，帧 0x22c vs 0x27c）与 N4/P30 寻址折叠、N7 寄存器分配。
// 修法（含 4 处真语义 bug）：
//   ①新键插入：第二实参直接引用线格式计数（ORIG 084a4929 `lea 0x14(%eax),%ebx`）
//     + P178 嵌套 make_pair/pair 移动构造（mangled make_pair<JobKey,COUNT&> 实证）；
//   ②区块 2 零检查改循环体内 break（ORIG 084a4a5c test/je 在 body 首）；
//   ③区块 3 改整体结构赋值（ORIG 084a4a9b 源/目标基址各算一次、3 读 3 写）；
//   ④MakeItemPacket 的 receiver 改 getCurCharacInvenR（旧用 W，**数据源真 bug**，
//     p167 MISSING `_ZNK15CUserCharacInfo18getCurCharacInvenREv` 实证）；
//   ⑤0x152 包 put_short（旧 put_byte，**包字段宽度真 bug**，MISSING put_short(int)）；
//   ⑥0x161 包复用同一 PacketGuard + clear()（消多余 C1/D1 与 _Unwind_Resume）；
//   ⑦第二封补 `strncpy(msgBuf,msg,0xff)`（旧漏调用，MISSING strncpy）；
//   ⑧缓冲改 `char[N]={0}` 数组零初始化（消 memset×4，P214③）；
//   ⑨G_CDataManager 每次重调（消 sps/dailyLimit 具名缓存，MISSING×2）；
//   ⑩`pair<int,int>(0xcf0, sealGive)` 直构（mangled `C1IiRiE` 实证；sealGive 非 const）；
//   ⑪单 Inven_Item + 单组缓冲复用两封（ORIG 084a51ec 单对象）、字段序
//     addInfo→find_item→addInfo2→make_item（ORIG 084a52a8/52de）。
// 残余 N 归因：N82/N95 帧·槽全帧平移（0x50）、N4/P30 寻址折叠、N7 寄存器分配。
// [L6S5 round] 867 → **857**（ORIG 848/849；gap −19 → **−9**；帧 0x27c → 0x24c，
//   槽平移 0x38 → 0x08）：
//   ① P329：两处 `const char* title/msg = findString(...)` 具名中转删掉、表达式就地
//      内联进 strncpy 实参位（ORIG `call findString; mov %eax,0x4(%esp)` 实证）⇒ −8 条；
//   ② P183：8 个 `const int bonus*` 各占 4B 具名槽（-0x28..-0x48）⇒ 删名内联 ⇒ −16 条；
//   ③ 形态复原：total 改回 ORIG 的**逐项 += 累加**（`movl $0x0,-0x24` 起步，序列
//      2*A0 → (A1+A2) → 3*B0 → (B1+B2) → bonusA×4 → sumB → bonusB×4 →
//      **2*A0 → 2*B0（尾部两项）**）⇒ +14 条，但块 65 差 −15 → −1、帧再降。
//      数值逐项核对 = 与旧单表达式**等价**（尾部 2*A0/2*B0 即旧式开头两项的位移），
//      无语义差。
//   残余 −9（15 块，幅 ±1~3）：块 0/6 的 N4/P30 常量拆分（我方 `(idx+3)*0x10+4` vs
//      ORIG `(idx+2)*0x10+0x14`）、块 44-49 EH/析构尾块序（净 −1）、块 68 失败返回
//      多 `mov $0x0,%esi`（N104 族）、块 18/20/79/93 寄存器角色（N7）。
// ORIG 0x84a48f8。
bool CFairPvPScore::loadData(CUser* pUser, char* pData)
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;

    // 区块 1：职业计分表（entry stride 0x10，起于 (idx+2)*0x10）
    // ORIG 084a491d..084a4a45：count 每轮重读 sig->m_jobScoreCount（无局部）；
    // key 为 make_pair 实参临时（mangled make_pair<JobKey,COUNT&> 实证）；
    // 第二实参直接引用线格式计数（`lea 0x14(%eax),%ebx`），旧版复制到局部 value
    // 会多 3 store 且丢失引用语义。
// [L6S4 P291/P183] ORIG 084a4a1c 段为「索引寄存器 + 基址寄存器 + disp」形：
//   `mov -0x34(%ebp),%ecx; mov -0x38(%ebp),%eax; add $0x2,%ecx; shl $0x4,%ecx;
//    movzbl 0x10(%ecx,%eax,1),%eax`（eax=sig 独立于索引 ecx）；旧 cast 指针
//   算术形把 sig 折进基址 ⇒ 每个访问点多 1 条 `lea (%ecx,%eax,1)`（6 处）。
// 改用权威成员数组下标：m_jobScore@+0x30 与 (idx+2)*0x10+0x10 同址。
#define FAIR_PVP_ENTRY(i) (sig->m_jobScore[i])
    for (idx = 0;
         (idx < (int)(unsigned char)sig->m_jobScoreCount) && (idx <= 99);
         ++idx)
    {
        std::pair<std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator, bool>
            ret = m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(
                    std::make_pair(
                        JobKey((ENUM_CHARACTERJOB)(unsigned char)
                                   FAIR_PVP_ENTRY(idx).m_job,
                               FAIR_PVP_ENTRY(idx).m_sub),
                        *(FAIR_PVP_SCORE_MODE_COUNT*)&FAIR_PVP_ENTRY(idx).m_cnt0)));
        if (!ret.second)
        {
            LogManager::logFormat(
                1, "FairPvPScore.cpp",
                __PRETTY_FUNCTION__,
                0x20, "job_score_.insert error(%d,%d)",
                (int)(unsigned char)FAIR_PVP_ENTRY(idx).m_job,
                (int)(unsigned char)FAIR_PVP_ENTRY(idx).m_sub);
        }
    }

    // 区块 2：最近对局结果列表（ORIG 084a4a45..084a4a92：零检查在体内 break，
    // 循环条件只剩 idx <= 9；遇 0 截断）
    for (idx = 0; idx <= 9; ++idx)
    {
        if (sig->m_recentResults[idx] == '\0')
            break;
        m_recentResults_.push_back(sig->m_recentResults[idx]);
    }

    // 区块 3：两份模式统计记录（ORIG 084a4a92..084a4ae3 为整体结构赋值：
    // 源/目标基址各算一次、3 读 3 写）
    for (idx = 0; idx <= 1; ++idx)
    {
        *(FAIR_PVP_SCORE_MODE_COUNT*)&m_modeStats[idx][0] =
            *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_modeStats[idx][0];
    }
    m_field40 = sig->m_field1c;
    m_field44 = sig->m_field20;
    m_winStreak = sig->m_winStreak;
    m_bestWinStreak = sig->m_bestWinStreak;
    m_dailyPlayCount = sig->m_dailyPlayCount;
    m_field54 = sig->m_lastPlayTime;   // int→unsigned 异型：ORIG 084a4b25 两段中转

    // 训练特训渠道（0xf/0x10）且无上次结算时间 → 补发入场奖励
    if ((G_GameWorld()->GetChannelType() == 0xf ||
         G_GameWorld()->GetChannelType() == 0x10) &&
        (int)sig->m_lastPlayTime == 0)
    {
        std::vector<std::pair<int, int> > items;
        // ORIG 084a4b86..084a4ba3：getPVPResultRefR 实参先求值（读 +0x14
        // m_pvpGrade），receiver 后求值；G_CDataManager 每次重调（禁 sps 缓存）。
        int sealGive =
            G_CDataManager()->m_serverParameter.GetTraningSealGive(
                pUser->getPVPResultRefR()->m_pvpGrade);
        if (sealGive > 0)
            // ORIG mangled pair<int,int>::C1<int,int&>(int&&,int&)：第一参字面量右值
            items.push_back(std::pair<int, int>(0xcf0, sealGive));
        G_CDataManager()->m_serverParameter.GetTraningFirstGive(
            pUser->get_charac_job(),
            (int)(signed char)pUser->getCurCharFirstGrowType(),
            pUser->get_pvp_grade(), items);

        if (!items.empty())
        {
            std::vector<int> slots;
            // ORIG 084a4c65 getCurCharacInvenW 作 insert_event_items 的 receiver；
            // 084a4d76 循环内每次重调 getCurCharacInvenR（P183 禁缓存）。
            int inserted = pUser->getCurCharacInvenW()->insert_event_items(
                items, slots, (eMoneyAddReason)3, (eItemAddReason)8, true,
                "game_server_msg_171", "game_server_msg_172");
            if (inserted >= 1 && slots.size() >= 3)
            {
                PacketGuard pkt;
                pkt.put_header(0, 0xe);
                pkt.put_byte(0);
                int fillIndex = pkt.get_index();
                int made = 0;
                pkt.put_short(made);           // ORIG 084a4d51 put_short(made) 占位
                for (size_t j = 0; j < slots.size(); j += 3)
                {
                    if (pUser->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)1, slots[j], pkt))
                        ++made;
                }
                pkt.put_short(fillIndex, made);
                pkt.finalize(true);
                pUser->Send(pkt);
            }
        }
        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }

    // 剩余可领训练印章数（ORIG 读 CDataManager+0x49e0 =
    // m_serverParameter(+0x68)+0x4978 → FairMatchRooms 批已具名
    // m_schoolMatchDailyPlayLimit）。ORIG 084a4eaa/084a4ed1 两分支各重调
    // G_CDataManager（禁 dailyLimit 具名缓存）。
    if ((unsigned int)m_dailyPlayCount <
        (unsigned int)G_CDataManager()->m_serverParameter
            .m_schoolMatchDailyPlayLimit)
        m_remainBonusCount =
            (unsigned char)(G_CDataManager()->m_serverParameter
                                .m_schoolMatchDailyPlayLimit -
                            (unsigned char)m_dailyPlayCount);  // ORIG sub %al,%dl
    else
        m_remainBonusCount = 0;
    {
        // ORIG 084a4ef6 构造 -0x88 的 PacketGuard 后一直活到函数尾；
        // 0x152/0x161 两包复用同一对象（0x084a4fe5 clear）。
        PacketGuard pkt;
        pkt.put_header(0, 0x152);
        pkt.put_short((int)m_remainBonusCount);   // ORIG 084a4f30 put_short(int)
        pkt.finalize(true);
        pUser->Send(pkt);

        m_giveItem = sig->m_giveItem;

        // 特训渠道：可领但尚未领取的道具（发 0x161 提示包）
        if (G_GameWorld()->GetChannelType() == 0xf ||
            G_GameWorld()->GetChannelType() == 0x10)
        {
            if ((int)m_giveItem <=
                G_CDataManager()->m_serverParameter.get_pvp_item_give(
                    pUser->get_pvp_grade()))
            {
                pkt.clear();
                pkt.put_header(0, 0x161);
                pkt.finalize(true);
                pUser->Send(pkt);
            }
        }

    // 上次结算时间早于维护点 → 按累计成绩补发两封结算邮件
    if (m_field54 != 0 && m_field54 <= 0x4ddd89efu)
    {
        // [L6S5 P183 + 形态复原] ORIG 084a4fxx..084a51xx：total 为**逐项 += 累加**
        //   （`movl $0x0,-0x24` 起步；每项 `mov -0x24,%eax; lea (%edx,%eax,1),%eax;
        //   mov %eax,-0x24`）。ORIG 累加序列（行号=块内序）：
        //   2*A0 → (A1+A2) → 3*B0 → (B1+B2) → bonusSumA50/60 → bonusA15/90
        //   → sumB → bonusSumB50/60 → bonusB15/90 → **2*A0 → 2*B0（尾部两项）**。
        //   旧版写成含 8 项的单一表达式（寄存器累加，更省）⇒ 与 ORIG 形态背离且
        //   无法对齐 -0x24/-0x20 槽（帧差主体）；数值等价（尾部 2*A0/2*B0 即旧式
        //   开头两项的位移）。8 个 bonus 项不用具名槽（ORIG 直接 `add %eax,-0x24`）。
        int total = 0;
        total += m_modeStats[0][0] * 2;
        total += m_modeStats[0][1] + m_modeStats[0][2];
        total += m_modeStats[1][0] * 3;
        total += m_modeStats[1][1] + m_modeStats[1][2];
        const int sumA = m_modeStats[0][0] + m_modeStats[0][1] + m_modeStats[0][2];
        total += (sumA >= 10 && sumA <= 0x31) ? 10 : 0;
        total += (sumA > 0x31) ? 60 : 0;
        total += (m_modeStats[0][0] >= 10 && m_modeStats[0][0] <= 0x31) ? 15 : 0;
        total += (m_modeStats[0][0] > 0x31) ? 90 : 0;
        const int sumB = m_modeStats[1][0] + m_modeStats[1][1] + m_modeStats[1][2];
        total += (sumB >= 10 && sumB <= 0x31) ? 10 : 0;
        total += (sumB > 0x31) ? 60 : 0;
        total += (m_modeStats[1][0] >= 10 && m_modeStats[1][0] <= 0x31) ? 15 : 0;
        total += (m_modeStats[1][0] > 0x31) ? 90 : 0;
        total += m_modeStats[0][0] * 2;
        total += m_modeStats[1][0] * 2;

        // ORIG 084a51ec..084a54cd：单个 Inven_Item + 单组缓冲复用两封；
        // 缓冲为数组零初始化（4.1 内联 rep stos/慢路；旧版 memset 调用多 4 次）；
        // 字段序 = addInfo → find_item → addInfo2 → make_item（084a52a8/52de）。
        Inven_Item item;                       // 默认构造清零
        char titleBuf[21] = {0};               // ORIG 0x15 字节
        char msgBuf[256] = {0};
        item.m_addInfo = 0xcf0;                // ORIG -0x104(%ebp)= item+0x02
        CItem* def = G_CDataManager()->find_item(item.m_addInfo);
        if (def == NULL)
            return false;                      // ORIG ebx=0 早退（仍析构包守卫）
        item.m_addInfo2 = total + 200;         // ORIG -0xff(%ebp) = item+0x07

        def->make_item(item);
        {
            // [L6S5 P329] ORIG 084a51xx：findString 结果**直接**入 0x4(%esp) 喂
            //   strncpy（无具名 `title`/`msg` 中转槽）。
            strncpy(titleBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", 0), 0x14);
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_177", 0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        // —— 第二封：印章道具 0x289788，数量 1（ORIG 复用同一 item/缓冲） ——
        item.m_addInfo = 0x289788;
        CItem* def2 = G_CDataManager()->find_item(item.m_addInfo);
        if (def2 == NULL)
            return false;
        item.m_addInfo2 = 1;

        def2->make_item(item);
        {
            // [L6S5 P329] 同上：第二封（178）同样内联。
            strncpy(titleBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", 0), 0x14);
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_178", 0), 0xff);  // [b28d-solo] 旧版漏此调用（p167 MISSING strncpy）
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }
    }  // pkt 作用域（ORIG PacketGuard 活到函数尾）

#undef FAIR_PVP_ENTRY
    return true;
}