// ORIG 0x088b0eba loadScript。忠实转写：
//   已装载→0；打开失败→-1；重复键 printf（ORIG 无换行符）后分别 -2/-3/-4 且不卸载脚本；
//   主循环 ScanType 失败即 unloadRDARScriptFile + 置已装载 + 返回 0。
// [recharge use emblem info] 段 grade>=8 的行不消费数值直接退出内层（ORIG 同）。
int ARAD::SCRIPT::AvatarRecharge::loadScript()
{
    if (m_loaded)
        return 0;

    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_recharge_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[avatar recharge limit]")
        {
            m_rechargeLimit = ScanInt((bool*)NULL);
        }
        else if (token == "[emblem recharge stone info]")
        {
            for (;;)
            {
                int idx = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                // b28d-lane25：P167 差集实证 ORIG 三段均为
                // `if (find==end) insert(value_type(k,v)); else {printf;return}`（eq 形）。
                if (m_emblemRechargeStoneMap.find(idx) == m_emblemRechargeStoneMap.end())
                {
                    m_emblemRechargeStoneMap.insert(
                        std::map<int, int>::value_type(idx, value));
                }
                else
                {
                    printf("[emblem recharge stone info] duplicate item_index:%d", idx);
                    return -2;
                }
            }
        }
        else if (token == "[emblem recharge]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeMap.find(grade) == m_emblemRechargeMap.end())
                {
                    m_emblemRechargeMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[emblem recharge] duplicate grade:%d", grade);
                    return -3;
                }
            }
        }
        else if (token == "[recharge use emblem info]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                if (grade >= 8)
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_useEmblemGradeRateMap.find(grade) == m_useEmblemGradeRateMap.end())
                {
                    m_useEmblemGradeRateMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[recharge use emblem info] duplicate grade:%d", grade);
                    return -4;
                }
            }
        }
    }

    unloadRDARScriptFile();
    m_loaded = true;
    return 0;
}