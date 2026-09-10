void CBattle_Field::check_random_appear_hell_dungeon(const CDungeon* dungeon,
                                                     ENUM_DUNGEON_MODE mode,
                                                     ENUM_DUNGEON_TYPE type,
                                                     bool& flag)
{
    if (m_party->m_hellWorldMap == 0)
        return;

    // ORIG 三条件各自 je 直达统一尾部（mode 检查 @0x1c4）：正向嵌套 if 的
    // 失败分支 4.4 亦归一到 body 后统一出口，同形。&& 链会折 bool 聚合岛、
    // `!call` 会多 sete/xor 物化，均不用。内层重复测试 m_hellWorldMap != 0
    // （外层已保证非零）系 ORIG 原样（@0x026-0x033）。
    // uchar 返回真值测试：`!= 0`/裸调用在 4.4 折 setne 物化（P73），
    // `> 0` 无符号下限形可回 test/je 直跳（对齐 ORIG @0x1e）。
    if (IsEnableHellDungeon())
    {
    if (m_party->m_hellWorldMap != 0)
    {
    if (m_party->m_hellWorldMap->IsInHellDungeon())
    {
        int minLevel = 1;
        int maxLevel = 200;
        int level = 0;                    // ORIG 循环前初始化槽 -0x14
        int validCount = 0;
        for (int i = 0; i <= 3; ++i)
        {
            if (!m_party->checkValidUser(i))
                continue;
            // ORIG @0x097/@0x0bc 双臂各自重发 get_user+getCurCharacR 完整链
            // （P48 无 CSE）；m_level +0x27 short（movzwl+cwtl 为 4.1 符号
            // 扩展两段形态，跨版本 N 豁免）。
            if (m_party->get_user(i)->getCurCharacR() != 0)
                level = (int)m_party->get_user(i)->getCurCharacR()->m_level;
            else
                level = 1;
            // ORIG @0x0e9/@0x0fc：三目物化岛（两臂各载一侧、统一 store）；
            // 目标值必须写在三目第一操作数位（P243 族：ORIG 先载目标槽）。
            minLevel = (minLevel > level) ? minLevel : level;
            maxLevel = (maxLevel < level) ? maxLevel : level;
            validCount += 1;
        }

        // ORIG @0x134：get_standard_level 返回值（eax）直接参与减法，不落槽
        // （P160 勿缓存）。
        if (minLevel - dungeon->get_standard_level() <= 5)
        {
            // ORIG @0x147-0x166：`jle <flag=1 块>` 使 validCount<=1 的
            // true 块落在远位、else-if 判断直落（4.1 块序）；4.4 对
            // if/else-if 恒反 ⇒ 反向包裹复现（P222/P47）。
            if (validCount > 1)
            {
                if (minLevel - maxLevel <= 8)
                    flag = true;
            }
            else
            {
                flag = true;
            }
        }

        if ((minLevel - maxLevel > 8 && validCount > 1) ||
            (!m_party->m_field1864 && type == 1) ||
            (!m_party->m_sealDoorFlag && type == 1))
        {
            m_party->m_hellWorldMap = 0;
        }
    }
    }
    }

    if (mode == 1)
    {
        m_party->m_hellWorldMap = 0;
        flag = false;
    }
