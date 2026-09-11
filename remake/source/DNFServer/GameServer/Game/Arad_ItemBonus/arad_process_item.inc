// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_process_item —— ORIG 0x081973fa（arad_process_item.md 转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x0819741c..0x081974de 两枚 int 局部——
// restrictCode=-1（-0x18：尾判 ==-1 与 SendUpdateItemList 实参来源）与 code=0
// （-0x1c：&code 作 ProcessSpecialItem 第 4 参出参）；ORIG 未把 code 写回
// restrictCode（原码如此，邮件路径恒走）。P168a①：`if (!call())` 直产
// call;xor$1;test;je 且 then 块内联、log 段外置（ORIG +0x194）。
bool ARAD_ITEM::arad_process_item(CUser* user, Inven_Item const& item)
{
    if (user == 0 || item.m_addInfo == 0)
        return false;

    int restrictCode = -1;
    int code = 0;

    // ORIG 构造序：items vector → sysTime → GetRestrictCode。
    std::vector<Inven_Item> items;
    const int secTime = GlobalData::s_systemTime_.getCurSec();  // getCurSec()
    // P167：ORIG 直接读 G_CDataManager()+0xc（m_itemList）调 GetRestrictCode，
    // 不经 getItemList()（后者 -O0 未内联反产外联 call）
    const int restrictSrc = G_CDataManager()->m_itemList->GetRestrictCode(
        item.m_addInfo);

    WongWork::CCeraShop::stCeraShopItemParam_t param;

    // ORIG 第 3 参为 GetRestrictCode 的返回值、第 4 参为 code 出参引用；
    // curSec 以 long 低半传递（ORIG 仅赋值低半栈槽）。
    if (!GlobalData::s_pCeraShop->ProcessSpecialItem(
            user, item.m_addInfo, restrictSrc, code, secTime, true,
            items, param, '\0', '\0', false, 1))
    {
        // P47/P222：ORIG `cmp $-1; je <mail 外置块>`，update 直落 —— 故取反极性。
        if (restrictCode != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                     restrictCode);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, item, 0, user->getCurCharacNo(), true, msg, strlen(msg));
        }
    }
    else
    {
        LogManager::logFormat(
            // §6.5 串盲区：ORIG .rodata 实读
            // "bool ARAD::ARAD_ITEM::arad_process_item(CUser*, const Inven_Item&)"
            // （ORIG 侧 ARAD_ITEM 为 namespace 形，无 `static` 前缀）；本仓以
            // class+static 重建 ⇒ __PRETTY_FUNCTION__ 展开多 7 字节 "static "。
            // 改手写字面量逐字节对齐（同 db_dispatch 既有实践；指令形/mangled 不变）。
            // [P6AuditOldN 2026-09-11 · Main 修订] 本「手写照抄」**非最优解，仅当时的权宜**：
            // 正解 = 把 `class ARAD_ITEM { public: static … };` 改回 `namespace ARAD_ITEM { … }`
            // （mangled 逐字符不变、零布局/零 ABI，prett 便不再带 `static `），见 `ARAD_Database.h`
            // 的 class+static→namespace 转换（检查点 345 实证：strings 归零 + prett OURS-only=0 +
            // 四数逐值不变）。若后续统一转换，本处手写串应一并撤除改回 `__PRETTY_FUNCTION__`。
            // [AradPrett 2026-09-11 · A 类实测负结论] 试换宏 ⇒ 展开
            // `static bool ARAD::ARAD_ITEM::arad_process_item(CUser*, const Inven_Item&)`
            // size **0x4a**（vsz 实测），≠ ORIG 0x08b9da00 size **0x43**（ORIG 串
            // `bool ARAD::ARAD_ITEM::arad_process_item(CUser*, const Inven_Item&)`）；
            // 差 7 字节 = 类内 `static` 前缀（判据②）⇒ 本件**维持手写串**。
            1, "localjapan/Arad_Function_Set.cpp",
            "bool ARAD::ARAD_ITEM::arad_process_item(CUser*, const Inven_Item&)",
            0x187,
            "Error process Item (charac:%u, item:%u, count:%d)",
            (unsigned int)user->getCurCharacNo(),
            (unsigned int)item.m_addInfo, item.m_addInfo2);
        return false;
    }
    return true;
}