// ORIG T 0x080ef190：[option] 区选项名 → itemData 字节域写入。
// 返回 false 当且仅当 "upgrade" 的整数扫描失败；未知名返回 true 且不写。
bool eventReward::getOptionType(std::string& token, itemData& out)
{
    bool ok = true;
    if (token == "unseal")
        out.m_seal = 0;
    else if (token == "seal")
        out.m_seal = 1;
    else if (token == "upgrade")
    {
        // ok 初值 true；ScanInt 自身写失败标志（ORIG 无预置 false）。
        // P0：ORIG `call ScanInt; mov %eax,%edx; mov %dl,(%eax)` 结果无槽往返 ⇒
        // 原码为右值直传，禁具名 int 缓存（缓存多 store/load 两条）。
        out.m_upgrade = ScanInt(&ok);
        if (!ok)
            return false;
    }
    else if (token == "unsealROI")
        out.m_roi = 0;
    else if (token == "sealROI")
        out.m_roi = 1;
    return ok;
}