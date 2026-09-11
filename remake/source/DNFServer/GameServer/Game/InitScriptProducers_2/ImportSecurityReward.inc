// ============================================================================
// ImportSecurityReward —— ORIG T 0x089113ec（init.md:1123，this+0x50d8）。
// 三段各 4 个 int；返回 bool（真=成功）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N1 封顶（原 DIFF 102）ORIG
//   0x089113ec orig/ours=125/120，P167 CLEAR（22/22）。本波修真语义 bug：
//   ①ORIG 用 `bool ScanInt(int*)`（`_Z7ScanIntPi`）**语句调用**写字段（每段 4 次
//   展开，非 for 循环），原写成 `int ScanInt(bool*)`+ok 槽+循环；②P164 goto 循环。
//   残余：5 处地址 `add $8; add $4` 两步形（4.1 常量乘法展开），4.4 恒折单条
//   `add $0xc`（探针 /tmp/probe2d.cpp g2d/g3/g4/g5/g6 全折）⇒ N1 跨版本常量算术。
// [sweep2-InitScriptProducers_2] 复核确认 N1：orig/ours=123/118 gap=5 ndiff=15，
//   ORIG-only 全为 5 对 `add $X; add $4`（0x089114b2/b5、0x089114ea/ed、0x08911509/0c、
//   0x08911548/4b、0x08911567/6a —— 即奇数索引 [1]/[3] 的两步地址）。探针
//   /tmp/sw2/p7.cpp 两形（`&a[3]` 与 `&a[2]+1`）在 4.4.7 与 4.4.6 后端下均折单条
//   `add $0xc` ⇒ 源形不可择，地址值等价，N1 封顶。
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
}