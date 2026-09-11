// ---------------------------------------------------------------------------
// importCashShopPurcahseBonusItemList —— ORIG 0x088b62ec
// （docs/class_func_reports/ARAD/importCashShopPurcahseBonusItemList.md）。
// 错误码口径与汇编一致；EOF 路径 ORIG `mov $0,%ebx`（+0x34c）⇒ 正常结束返回 0。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG 无 result/again 局部（P0：错误码经 %ebx 直返、
// 循环条件仅 ScanType；%esi 为 EH 清理用的返回标志）；`int key = 0;` 先于
// bonus/list 构造、`key = ScanInt(&ok);` 独立赋值（ORIG +0x76/+0x93）。
int importCashShopPurcahseBonusItemList(
    char* path,
    std::map<int, std::vector<PurcahseBonus> >& out)
{
    if (!loadRDARScriptFile("", path))
        return 10;

    typedef std::vector<PurcahseBonus> BonusVec;

    std::string token;
    bool ok = false;

    // ORIG +0x41 条件头在循环顶（入口 `jmp +0x41`、body 尾 `jmp +0x41`），
    // token 判等为 `if (token == "[bonus]") { body }`（`_ZSteq`，je 落 +0x40 nop）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[bonus]")
        {
            int key = 0;
            PurcahseBonus bonus;
            BonusVec list;

            key = ScanInt(&ok);
            if (!ok)
                return 27;  // 0x1b：goods 行首列缺失

            while (true)
            {
                bonus.m_flag = (unsigned char)ScanInt(&ok);
                if (!ok)
                    break;                   // 直接进入查重/插入段

                bonus.m_field4 = ScanInt(&ok);
                if (!ok) return 35;          // 0x23：type/item id 缺失
                bonus.m_field8 = (short)ScanInt(&ok);
                if (!ok) return 38;          // 0x26：数量缺失

                // 具名 found 强制 LHS（find）先求值，RHS end() 在 find 后（ORIG 序）。
                std::vector<PurcahseBonus>::iterator found =
                    std::find(list.begin(), list.end(), bonus);
                if (found != list.end())
                {
                    LogManager::logFormat(
                        1, "../localjapan/RDARScriptCeraShop.cpp",
                        __FUNCTION__, 0x2b,
                        "[PURCAHSE BONUS] invalid purcahse count "
                        "(goods:%u, count:%d)",
                        key, (int)bonus.m_flag);
                    return 44;               // 0x2c：重复门槛档
                }
                list.push_back(bonus);
            }

            std::map<int, BonusVec>::iterator it = out.find(key);
            if (it != out.end())
            {
                LogManager::logFormat(
                    1, "../localjapan/RDARScriptCeraShop.cpp",
                    __FUNCTION__, 0x36,
                    "[PURCAHSE BONUS] duplicate purcahse item (goods:%d)", key);
                return 55;                   // 0x37：key 重复
            }

            out.insert(std::make_pair(key, list));
        }
    }

    return 0;
}
