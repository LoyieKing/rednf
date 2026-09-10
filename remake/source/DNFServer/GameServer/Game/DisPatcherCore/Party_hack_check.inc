void DisPatcher_FinishLoad::Party_hack_check(CUser* user)
{   // ORIG 栈位映射：-0x40 sameIp / -0x3f multiBox / -0x3e job(sbyte) /
    // -0x3d growType / -0x50 baseIpPrefix[16] / -0x3c,-0x38,-0x34 = 技能三槽 index。
    // -0x50：0 号成员 IP 最后一个 '.' 前缀。ORIG 函数头 4×movl $0 预清零
    // （0x08264c3a/41/48/4f）⇒ 原码数组带 {0} 初始化；缺清零会令 strncpy
    // 短写后残留栈垃圾（越界读隐患）。
    // [b28e-L5] 判据⑩（声明序 = 槽由深到浅）：ORIG basePrefix@-0x50 是最深槽 ⇒
    // 原码**最先声明**（其 4×movl $0 落在 sameIp 之前）。故声明提到 sameIp 之前。
    char basePrefix[16] = {0};
    char sameIp = 1;
    char multiBox = 1;
    signed char jobByte = 0;
    signed char growType = 0;
    unsigned int skillIdx0 = 0;
    unsigned int skillIdx1 = 0;
    unsigned int skillIdx2 = 0;

    CParty* party = static_cast<CParty*>(user->GetParty());   // ORIG _ZN5CUser8GetPartyEv
    if (party == 0)
    {
        return;
    }
    if (party->get_member_count() <= 2)
    {
        return;
    }
    for (int idx = 0; idx < 4; ++idx)
    {
        if (!party->checkValidUser(idx))
        {
            continue;
        }
        CUser* member = party->get_user(idx);
        if (member == 0)
        {
            continue;
        }
        // ORIG 块序（P272/P59）：0x08264cf2 `cmpl $0,-0x28; je 08264e44` ⇒
        // idx!=0 分支物理在前（fall-through）、idx==0 分支在后（跳转目标）
        // ⇒ 源码写作 if (idx != 0) {…} else {…}。
        if (idx != 0)
        {
            if ((signed char)member->get_charac_job() != jobByte ||
                member->getCurCharFirstGrowType() != growType)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            char peerIp[16] = {0};      // ORIG 0x08264e67..7c 4×movl 预清零
            member->m_network.GetPeerIP2(peerIp, 0x10);
            if (strncmp(basePrefix, peerIp, strlen(basePrefix)) != 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            SkillSlot* skill = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            if (skill->get_skill_index(0) != skillIdx0 ||
                skill->get_skill_index(1) != skillIdx1 ||
                skill->get_skill_index(2) != skillIdx2)
            {
                sameIp = 0;   // 仅清同 IP 标志，循环继续（ORIG jmp +0x377）
                continue;
            }
        }
        else
        {   // 记录基准：job/growType/IP 网络前缀/技能三槽。ORIG 0x21c 段顺序：
            // job(0x08264e4a)/growType(0x08264e58) 先于 peerIp 清零+GetPeerIP2(0x08264e67/91)
            jobByte = (signed char)member->get_charac_job();   // ORIG mov %al + movsbl 比较
            growType = member->getCurCharFirstGrowType();
            char peerIpBuf[16] = {0};   // ORIG 0x08264d43..58 4×movl 预清零
            char* peerIp = peerIpBuf;   // char* 静态类型命中 std::strrchr(char*,int) 非const重载
            member->m_network.GetPeerIP2(peerIp, 0x10);
            if (strlen(peerIp) <= 8)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            // peerIp 静态类型 char* → 命中 std::strrchr(char*,int) 非 const 重载
            // （mangled _ZSt7strrchrPci，匹配 ORIG）；指针间接 call 会被 p167 计为丢失，
            // 故直接调用（数组退化 const char* 会误选 ::strrchr plain）。
            char* dot = std::strrchr(peerIp, '.');
            if (dot == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            size_t fullLen = strlen(peerIp);
            size_t dotLen = strlen(dot);
            size_t prefLen = fullLen - dotLen;
            strncpy(basePrefix, peerIp, prefLen);
            // 预清零已保证 null 终止（ORIG 无显式终止 store）
            // 现声明为 const char*（ORIG 实返回 SkillSlot*），按位转写
            SkillSlot* skill = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            skillIdx0 = skill->get_skill_index(0);
            skillIdx1 = skill->get_skill_index(1);
            skillIdx2 = skill->get_skill_index(2);
        }
    }
    if (multiBox)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x330, 1, 0, 0);
        if (sameIp)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x32f, 1, 0, 0);
        }
    }
