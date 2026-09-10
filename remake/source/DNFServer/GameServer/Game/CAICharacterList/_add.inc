bool CAICharacterList::_add(CAICharacter* aiChar)
{
    // ORIG 0x0834a8be 段用块内具名 ap（-0x58）；头部声明复现其槽深序（P212）。
    int ap;
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
        ap = aiChar->getAppearancePoint();
        std::map<int, std::vector<CAICharacter*> >::iterator group =
            m_groupMap.find(ap);
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
