void CPowerManager::SetPowerWarUserStatueInfo(SIG_LOAD_POWER_WAR_STATUE_INFO* info)
{
    // ORIG 0x0847ed4e：按 characNo（info+0x00）匹配条目，拷贝雕像字段并置 flag，
    // 完成后若 DB 加载齐则 SendPowerWarUserStatueInfo(0)。
    // SIG_LOAD_POWER_WAR_STATUE_INFO 仅建模为 m_pad[0x100]，字段按 ORIG 偏移访问：
    // +0x00 characNo / +0x04 name(0x1e) / +0x22 job / +0x23 level / +0x24 grow /
    // +0x25 guildName(0x28) / +0x50 guildId / +0x54 equipment(0x2c)。
    // [b28d-solo] P183：ORIG 每字段重读形参并 disp 折叠（`mov 0xc(%ebp),%eax;
    // movzbl 0x22(%eax),%edx`），禁 src 指针缓存（缓存生 add 链且帧 +4）。
    // [b28d-solo] P222/P59：ORIG `cmp; jne <循环尾>` 匹配即直落 body ⇒ 正向
    // `if (匹配) { body; break; }`（原 `if (!=) continue;` 产反向 je 块布局）。
    for (int i = 0; (unsigned int)i < 3; ++i)
    {
        if (m_rankings[i].m_characNo == ((const PowerWarStatueView*)info)->m_characNo)
        {
            m_rankings[i].m_job = ((const PowerWarStatueView*)info)->m_job;
            m_rankings[i].m_level = ((const PowerWarStatueView*)info)->m_level;
            m_rankings[i].m_grow = ((const PowerWarStatueView*)info)->m_grow;
            m_rankings[i].m_guildId = ((const PowerWarStatueView*)info)->m_guildId;
            strncpy(m_rankings[i].m_name,
                    ((const PowerWarStatueView*)info)->m_name, 0x1e);
            strncpy(m_rankings[i].m_guildName,
                    ((const PowerWarStatueView*)info)->m_guildName, 0x28);
            memcpy(m_rankings[i].m_equipment,
                   ((const PowerWarStatueView*)info)->m_equipment, 0x2c);
            // [b28d-solo] 真语义缺口修复：ORIG 0x0847f0xx `movb $0x1,0x8(%eax)`
            // （this+i*0x84+0x28 = 元素 +0x00 m_flag）在 memcpy 之后、cMyTrace 之前。
            m_rankings[i].m_flag = 1;
            // ORIG 0x0847f0e5：cMyTrace(3 参) + "Set Statue Ranker ..." 变参日志。
            // 注意 ORIG 压栈 job/lev 槽位互换（lev 值在 job 槽、job 值在 lev 槽），
            // 且 EQUP 用 m_equipment[0..8] 与 [10]（跳过 [9]）——忠实保留。
            // [b28d-solo] P227/P139：ORIG 在 ctor 前求值全部 varargs 并跨 ctor 保活
            // （`mov 0x378…` 类装载先于 `call cMyTrace::C1`），且 file 串为
            // `_ZZ…__PRETTY_FUNCTION__` 局部静态符号（ORIG 0x08c72f80）⇒ 全表达式
            // 临时链 + __PRETTY_FUNCTION__。
            cMyTrace(__PRETTY_FUNCTION__, 0xa4, 0)
                ("Set Statue Ranker %s(%d), job:%d, lev:%d, grow:%d, guild_name:%s, guild_id:%d, EQUP [head:%d][hair:%d][face:%d][jacket:%d][pants:%d][shoes:%d][breast:%d][waist:%d][skin:%d][weapon:%d]",
                  m_rankings[i].m_name, m_rankings[i].m_characNo,
                  static_cast<int>(m_rankings[i].m_level),
                  static_cast<int>(m_rankings[i].m_job),
                  static_cast<int>(m_rankings[i].m_grow),
                  m_rankings[i].m_guildName, m_rankings[i].m_guildId,
                  m_rankings[i].m_equipment[0], m_rankings[i].m_equipment[1],
                  m_rankings[i].m_equipment[2], m_rankings[i].m_equipment[3],
                  m_rankings[i].m_equipment[4], m_rankings[i].m_equipment[5],
                  m_rankings[i].m_equipment[6], m_rankings[i].m_equipment[7],
                  m_rankings[i].m_equipment[8], m_rankings[i].m_equipment[10]);
            break;
        }
    }
    if (CheckCompleteDBLoadStatueInfo())
        SendPowerWarUserStatueInfo(0);
