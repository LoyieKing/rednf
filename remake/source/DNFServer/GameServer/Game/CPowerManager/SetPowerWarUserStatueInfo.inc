// [b28d-solo-CPowerManager] 语义完成 / byte 层 N | ORIG 0x0847ed4e |
// 新口径（n1_v.py）：DIFF orig 352 / our 353（gap −1）；旧 game_func_report 口径 352=352。
// 此前修复：真语义缺口 m_flag=1（ORIG memcpy 后 `movb $0x1,0x8(%eax)`）+ pack(1)
// 具名视图消 add 链（P80/P99/P183）+ P222 正向 if 包裹 + P227/P139 全表达式临时链
// + __PRETTY_FUNCTION__ + P211 档③ 同型化。本轮：P310 消 N18b 尾测 1 条。
// 残差 = N4/N86b（13 处 4.1 `lea 0x0(,%eax,4),%edx` vs 4.4 `shl $0x2,%eax` 强度
// 归约形；探针 /tmp/probe2d{,2}.cpp 八形——一维/二维[3][1]/`*(arr+i)`/
// `(&arr[0])[i]`/指针局部/unsigned 索引/int 数组下标——4.4 恒 shl，源形不可择）。
// P167 CLEAR 7/7。
// [b28d-solo 2026-09-11 本波新钥匙复扫] P291（具名 Entry 数组成员视图产 scaled
// 索引）/P235（匿名 union 双视角）针对的是「scaled 索引被折成 shl+add」；本件
// 两侧指令数全等（352=352，gap 0），残差只是 `x*4` 的 lea(4.1) vs shl(4.4) 选择
// ⇒ 属 multiply expansion 版本形，非视图/索引轴。P290（int→unsigned 升格）无对应
// callee-saved 跨 receiver 形；P339/P331（声明序）本件无「整体平移/槽映射互换」残差
// ⇒ 维持 N4/N86b 封顶，零改动。
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
}