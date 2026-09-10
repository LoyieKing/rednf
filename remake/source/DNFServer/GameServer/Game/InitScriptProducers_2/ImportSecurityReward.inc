bool ImportSecurityReward(SecurityReward* out)
{
    if (!loadRDARScriptFile("", "Etc/SecurityReward.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x2c36,
                              "CRITICAL ERROR : SecurityReward faild!");
        return false;
    }

    std::string token;

    // ORIG 0x089113ec：每段 4 次**展开**的 `_Z7ScanIntPi`（bool ScanInt(int*)）
    // 语句调用（返回值丢弃，P152/P171），非 for 循环；地址形 `add $8; add $4`
    // 即 4.1 对 arr[3] 的两步常量算术。
    // P164：4.4 会把 `while(cond)` 旋转成尾部条件头；用纯 goto 循环把条件头
    // 复位到前部（ORIG 08911453），失败落尾部岛（0891157a nop; mov $1,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[GoblinPad Reward]")
        {
            ScanInt(&out->m_goblinPad[0]);
            ScanInt(&out->m_goblinPad[1]);
            ScanInt(&out->m_goblinPad[2]);
            ScanInt(&out->m_goblinPad[3]);
        }
        else if (token == "[SecurityCard Reward]")
        {
            ScanInt(&out->m_securityCard[0]);
            ScanInt(&out->m_securityCard[1]);
            ScanInt(&out->m_securityCard[2]);
            ScanInt(&out->m_securityCard[3]);
        }
        else if (token == "[OTP Reward]")
        {
            ScanInt(&out->m_otp[0]);
            ScanInt(&out->m_otp[1]);
            ScanInt(&out->m_otp[2]);
            ScanInt(&out->m_otp[3]);
        }
    }
    goto scan_top;
scan_done:
    return true;
