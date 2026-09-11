// ==================================================================
// CBattle_Field::ConsistTournament @ 0x083035c8
// 报告：docs/class_func_reports/CBattle_Field/ConsistTournament.md
// [SweepBattleMonsterC3A] DIFF→NEAR（gap 209/209=0；calls 30=30，差集仅 C1/C2 同址
// 别名）：①P227 错误分支改 cMyTrace 全表达式临时链（ORIG 0x083036e9 在 ctor 前
// `call get_member_count; mov %eax,%ebx` 跨 ctor 保活）；②P198① MakeBasicRoundInfo
// 结果用具名 bool 局部（ORIG 0x0830377c `mov %al,-0x9; movzbl -0x9,%eax; xor $1`）。
// 残差 = P264（.o 数据立即数/重定位显示差）+ N82 槽号。
// ==================================================================
// [P8S5 2026-09-11] P139 prett 缺口（ORIG 有 / 我方无，REF 0x83035e9+0x830363d）：
//   ORIG 串 `bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)`
//   size 0x6a@0x08c23960；我方手写串与其逐字节相同，但定义签名拼作 `std::list<_mapMonster>&` /
//   `std::map<short, std::vector<TournamentInfo> >&` ⇒ 换 __PRETTY_FUNCTION__ 会发射展开形（≠ ORIG）。
//   需先补 typedef MOB_REF_LIST / CBattle_Field::TournamentRoundInfoMap（**头文件改动，越界：只报告**）⇒ 未改。
// [prett-A 2026-09-11] **已落地**（R2PrettAClass 授权 + 探针实证；原「需头文件改动 ⇒ 维持」判断作废）：
//   ① .cpp 文件作用域（非 namespace）加 `typedef std::list<_mapMonster> MOB_REF_LIST;`（宏打裸名）；
//   ② CBattle_Field.h 类内**纯增量**加 `typedef std::map<short, std::vector<TournamentInfo> > TournamentRoundInfoMap;`
//      —— GCC 4.4 探针（/tmp/r2/probe2.cpp）实证：类内 typedef 形参在类外定义 ⇒ 宏打印**全限定名**；
//   ③ 定义侧两处拼写改 typedef 名（同类型 ⇒ mangled 不变）；
//   ④ 三处 cMyTrace 手写串 → `__PRETTY_FUNCTION__`（删字面量、调用点内联）。
//   ORIG 符号 `_ZZN13CBattle_Field17ConsistTournamentEPK4CMapRSt4listI11_mapMonsterSaIS4_EERSt3mapIsSt6vectorINS_14TournamentInfoESaISA_EESt4lessIsESaISt4pairIKsSC_EEEE19__PRETTY_FUNCTION__`
//   @0x08c23960 size 0x6a，串 `bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)`。
//   ⚠ 宏为整串匹配 ⇒ 三处调用点必须同轮改（任一残留旧串即发射不匹配文本）。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
bool CBattle_Field::ConsistTournament(const CMap* pMap, MOB_REF_LIST& mobList,
                                      TournamentRoundInfoMap& roundMap)
{
    if (pMap == NULL)
    {
        // ORIG 0x083035f7 段：trace 槽 -0x50/-0x40 浅于容器槽（-0x78..-0x5c）
        // ⇒ P221 扩展：全表达式临时链（具名形 4.4 恒先声明更浅、槽方向相反）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb5a, 5)
            ("CBattle_Field::ConsistTournament ERROR!! : pMap is NULL pointer");
        return false;
    }

    // CMap+0x114：m_tournamentMap（map<char, vector<TournamentMonster>>）。
    if (pMap->m_tournamentMap.empty())
    {
        // ORIG 0x0830364b 段：同上，临时链形（槽 -0x40）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb61, 5)
            ("CBattle_Field::ConsistTournament ERROR!! : tournamentMonster is empty");
        return false;
    }

    std::vector<TournamentMonster> monsters;
    const int memberCount = m_party->get_member_count();

    std::map<char, std::vector<TournamentMonster> >::const_iterator group =
        pMap->m_tournamentMap.find(static_cast<char>(memberCount));
    if (group == pMap->m_tournamentMap.end())
    {
        // ORIG 0x083036e9 `call get_member_count; mov %eax,%ebx` 在 cMyTrace ctor 之前
        // 求值并以 %ebx 跨 ctor 保活（无具名槽）⇒ 全表达式临时链（P227）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb6c, 5)
            ("CBattle_Field::ConsistTournament ERROR!! : party member count is invalid(%d)",
             m_party->get_member_count());
        return false;
    }

    // ORIG 0x08303734：monsters = group->second（map 中该人数对应的怪物列表拷贝）。
    monsters = group->second;

    std::vector<TournamentInfo> roundInfo;
    // ORIG 0x0830377c `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax; xor $1` ⇒ 具名
    // bool 局部（P198①），勿直写 if(!call())。
    const bool basicOk = MakeBasicRoundInfo(memberCount, monsters, roundInfo);
    if (!basicOk)
        return false;

    std::sort(roundInfo.begin(), roundInfo.end());

    std::vector<TournamentInfo> firstGroup;
    ChoiceFirstGroup(roundInfo, firstGroup);
    ConsistTournamentRound(firstGroup, roundInfo, roundMap, mobList);
    return true;
}