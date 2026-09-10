void DisPatcher_FinishLoad::Party_hack_check(CUser* user)

{   // ORIG 栈位映射：-0x40 sameIp / -0x3f multiBox / -0x3e job(sbyte) /
    // -0x3d growType / -0x50 baseIpPrefix[16] / -0x3c,-0x38,-0x34 = 技能三槽 index。
    // -0x50：0 号成员 IP 最后一个 '.' 前缀。ORIG 函数头 4×movl $0 预清零
    // （0x08264c3a/41/48/4f）⇒ 原码数组带 {0} 初始化；缺清零会令 strncpy
    // 短写后残留栈垃圾（越界读隐患）。
    // [H5DisPatcher] 修正 [b28e-L5]：ORIG 头部**初始化指令序**为
    // 0x08264c32 movb $0x1,-0x40 → 0x08264c36 movb $0x1,-0x3f → 0x08264c3a..4f
    // basePrefix 的 4×movl $0，即 sameIp/multiBox 的初始化在 basePrefix 之前。
    // 槽址仍为 sameIp@-0x40 / multiBox@-0x3f / basePrefix@-0x50..-0x41（不变）。
    char sameIp = 1;
    char multiBox = 1;
    char basePrefix[16] = {0};
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
    // ORIG 0x08264c8b movl $0x0,-0x2c / 0x08264c9d mov %eax,-0x2c /
    // 0x08264ca0 cmpl $0x2,-0x2c ⇒ 具名局部 int memberCount（声明处 =0 再赋值），
    // 槽 -0x2c 与循环变量 idx 的 -0x28 分离。
    int memberCount = 0;
    memberCount = party->get_member_count();
    if (memberCount <= 2)
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
            // ORIG 0x08264cfc `movsbl -0x3e(%ebp),%ebx` + 0x08264d0b `cmp %eax,%ebx`
            // ⇒ jobByte 先求值（提升 int，跨 call 保活 ebx=callee-saved）、
            // get_charac_job() 为 **int** 返回值（不可写成 (signed char) 转换，
            // 否则退化为 `cmp %al` 字节比较并丢掉 push %ebx）。
            if (jobByte != member->get_charac_job() ||
                member->getCurCharFirstGrowType() != growType)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            // ORIG 0x08264d3c `movl $0x0,-0x20` 先于 0x08264d43..58 的 4×movl $0
            // ⇒ 具名 size_t ipLen 声明在本 16B 缓冲之前；0x08264d8a
            // `mov %eax,-0x20` 复用它存 strlen(basePrefix)。
            size_t ipLen = 0;
            char peerIp[16] = {0};      // ORIG 0x08264d43..58 4×movl 预清零
            member->m_network.GetPeerIP2(peerIp, 0x10);
            ipLen = strlen(basePrefix);
            if (strncmp(basePrefix, peerIp, ipLen) != 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            // ORIG 0x08264db7 `movl $0x0,-0x1c` 后 0x08264dc9 `mov %eax,-0x1c`
            // ⇒ 具名局部声明处 =0 再赋值（不可直接初始化）。
            SkillSlot* skill = 0;
            skill = reinterpret_cast<SkillSlot*>(const_cast<char*>(member->getCurCharacSkillR()));
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
            // ORIG 0x08264e60 `movl $0x0,-0x18` 在缓冲清零之前 ⇒ 具名 char* dot
            // 声明于 peerIpBuf 之前；0x08264ee1 才 `mov %eax,-0x18` 赋 strrchr 结果。
            char* dot = 0;
            char peerIpBuf[16] = {0};   // ORIG 0x08264d43..58 4×movl 预清零
            // ORIG 0x08264e83 `movl $0x0,-0x14` / 0x08264e8a `movl $0x0,-0x10`
            // ⇒ 两个具名 size_t 声明处 =0（fullLen / dotLen），且声明在缓冲之后。
            size_t fullLen = 0;
            size_t dotLen = 0;
            member->m_network.GetPeerIP2(peerIpBuf, 0x10);
            if (strlen(peerIpBuf) <= 8)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            // 数组名退化为 char* ⇒ 命中 std::strrchr(char*,int) 非 const 重载
            // （mangled _ZSt7strrchrPci，匹配 ORIG 0x08264edc 的调用目标）；
            // ORIG 无额外中间指针具名量（其 -0x20 槽属 idx!=0 段的 ipLen）。
            dot = std::strrchr(peerIpBuf, '.');
            if (dot == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            fullLen = strlen(peerIpBuf);
            dotLen = strlen(dot);
            // ORIG 0x08264f13..1f：差值全程寄存器（eax/ecx/edx）直入 0x8(%esp)，
            // 无具名 prefLen 槽 ⇒ 实参处内联 fullLen - dotLen。
            strncpy(basePrefix, peerIpBuf, fullLen - dotLen);
            // 预清零已保证 null 终止（ORIG 无显式终止 store）
            // ORIG 0x08264f35 `movl $0x0,-0xc` 后 0x08264f47 `mov %eax,-0xc`
            SkillSlot* skill = 0;
            skill = reinterpret_cast<SkillSlot*>(const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            skillIdx0 = skill->get_skill_index(0);
            skillIdx1 = skill->get_skill_index(1);
            skillIdx2 = skill->get_skill_index(2);
            // [H5DisPatcher 负结论] ORIG 0x08264f9c 的 `jmp 08264f9f`（循环回边，
            // else 块末尾唯一跳转）此处 4.4.6 后端恒多发射一条 `jmp 13f5e`；
            // 显式补 `continue;` 无效（编译产物逐字节不变），故不补。
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
    // [H5DisPatcher 负结论] 改写为 early-return（`if (!multiBox) return;` +
    // `if (!sameIp) return;`）尝试消去块尾 `jmp 出口`：实测 ours 257→261（更差），
    // 已回滚。ORIG 0x08264fb2 / 0x08264ff2 两处 `cmpb; je 同一出口` 的形态在
    // 嵌套 if 写法下已复现，残余为块尾多余 `jmp+nop`（4.4.6 后端布局，N 类）。
    // [H5DisPatcher p167 复核] 本件唯一调用点差集 = ORIG `_ZSt7strrchrPci`(W, 0x08265244)
    // ↔ 我方 `strrchr`。前者是 **weak/COMDAT** 的内联实例（判据库第 8 条：命中
    // weak/COMDAT 的差异**不计**），且两者行为等价 ⇒ 不改。
}
