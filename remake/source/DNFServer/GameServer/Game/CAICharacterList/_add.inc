// ---- _add（ORIG 0x0834a61c）：charMap 按 idx insert，重复返 false；
//      byLevel 桶 [level-5, level+5) 无符号循环盲写（lv>0x62 时 cMyTrace
//      告警仍写入）；ap!=0 时 groupMap find-or-insert、m_maxAP 取无符号
//      min、apStatMap 以 ap 为键计数（value 恒 1）；最后按 getRarity
//      （+0x08 int）入 m_byAPList 桶。----
bool CAICharacterList::_add(CAICharacter* aiChar)
{
    std::pair<std::map<int, CAICharacter*>::iterator, bool> ins =
        m_charMap.insert(std::make_pair(
            static_cast<unsigned int>(aiChar->getIdx()), aiChar));   // ORIG make_pair<unsigned int,...>
    if (!ins.second)
        return false;

    // ORIG 0x0834a697/0x0834a70e：getLevel 两次独立求值（P48，无 level 缓存）。
    for (unsigned int lv = (unsigned int)aiChar->getLevel() - 5;
         (unsigned int)aiChar->getLevel() + 5 > lv; ++lv)
    {
        if (lv > 0x62)   // ORIG cmp $0x62,jbe：仅告警不拦截
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x537, 0)(
                "aiCharacter Script file load failed");
        }
        m_byLevelList[lv].push_back(aiChar);   // this+lv*12+0x18 盲写
    }

    // ORIG 0x0834a726：外层判定、m_maxAP 段、apStatMap 段均逐次调用
    // getAppearancePoint（P48）；仅 find 用具名 ap。
    if (aiChar->getAppearancePoint() != 0)
    {
        // [S2CAICharList] ORIG 0x0834a749 `mov %eax,-0x58(%ebp)` + 0x0834a75b
        // `lea -0x58(%ebp),%edx`（find 实参）⇒ -0x58 是**表达式临时**而非具名局部。
        // 负结论（三个写法的实测）：具名 ap 声明在函数头 → 槽 -0x7c；声明在 if 块首
        // → 槽 -0x84；两者都让整函数槽位全线偏移（ndiff 39 行）。本写法（直接传
        // getAppearancePoint()）令槽位逐条对齐：ndiff 39 → 5（余仅 m_maxAP 类型差）。
        std::map<int, std::vector<CAICharacter*> >::iterator group =
            m_groupMap.find(aiChar->getAppearancePoint());
        // ORIG 0x0834a79c 用 operator!=（then=push_back / else=fresh，P220）。
        if (group != m_groupMap.end())
        {
            group->second.push_back(aiChar);
        }
        else
        {
            std::vector<CAICharacter*> fresh;
            fresh.push_back(aiChar);
            m_groupMap.insert(std::make_pair(aiChar->getAppearancePoint(), fresh));
        }

        // ORIG 0x0834a8be：三元（cond 走 jae 无 bool 物化、两臂共用尾部 store；
        // then 臂重载 m_maxAP，else 臂重调 getAppearancePoint）。m_maxAP 为
        // int 与 unsigned 右操作数比较 ⇒ 无符号序（jae），勿写成有符号。
        // 残余 1 条（ORIG 0x0834a8c1 `mov 0x4bc(%eax),%ebx` 直落 vs 我方
        // `mov 0x4bc(%eax),%eax; mov %eax,%ebx`）根因 = **m_maxAP 的声明类型 int**：
        // 探针 A/B（同表达式、同编译器）实证 int 成员恒多一条中转、unsigned 成员直落。
        // 本 TU 内三个等价写法实测无效（负结论）：①双侧显式 (unsigned) cast 不变；
        // ②reinterpret_cast<unsigned int&> → `add $0x4bc,%eax; mov (%eax),%ebx`（2 条）；
        // ③*(unsigned int*)((char*)this+0x4bc) → 同上 add 形式。**已落地**：
        // CDataManager.h 的 `m_maxAP` 改 `unsigned int`（2026-09-11，父泳道授权后）。
        m_maxAP = (m_maxAP < (unsigned int)aiChar->getAppearancePoint())
                      ? m_maxAP
                      : aiChar->getAppearancePoint();

        // ORIG 0x0834a8f5：make_pair 第一实参 = getAppearancePoint()（key），
        // 第二实参 = 常量 1（value）——0x0834a910 arg2=&(-0x14)=1、
        // 0x0834a917 arg1=&(-0x10)=ap。插入失败时把该 AP 的计数 +1。
        std::pair<std::map<int, int>::iterator, bool> stat =
            m_apStatMap.insert(std::make_pair(aiChar->getAppearancePoint(), 1));
        if (!stat.second)
            stat.first->second = stat.first->second + 1;
    }

    m_byAPList[aiChar->getRarity()].push_back(aiChar);   // this+rarity*12+0x4d8
    return true;
}