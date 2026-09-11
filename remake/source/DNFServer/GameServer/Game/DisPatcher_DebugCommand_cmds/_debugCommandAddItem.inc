// ===========================================================================
// 6. _debugCommandAddItem —— 0x0858ec92
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 4.4.6 后端 %edx（与 ORIG 同形）；4.4.7 下恒 %eax 才是
// NEAR（P230 补）。已修：slot>=0 正向包裹翻尾块序（P222）、outSpace 删预清（P171）、
// getCurSec 双调内联实参、`&&` 拆嵌套 if（P225 推广）。P167 CLEAR 16/16。
bool DisPatcher_DebugCommand::_debugCommandAddItem(CUser* user,
                                                   itemIndexOnlyServer_t itemIdxOnlyServer,
                                                   int count)
{
    if (!user->isGMUser())                      // +0x08
        return false;

    if ((int)itemIdxOnlyServer == -1)           // +0x24 cmpl $-1 → PC 房一天高级会员
    {
        // P227 内联实参：ORIG 两次 getCurSec 结果直入 %ebx/%eax，无具名局部。
        WongWork::CHandlePremium::handleSetUserPCRoom(
            user, GlobalData::s_systemTime_.getCurSec(),     // +0x43 再调一次（ORIG 如此）
            GlobalData::s_systemTime_.getCurSec() + 0x15180);  // end=当前秒+86400
    }
    if (count < 1)                              // +0x5b count<1 → 100
        count = 100;

    CItem* item = G_CDataManager()->find_item((int)itemIdxOnlyServer);  // +0x77
    if (item != 0)                              // +0x7f
    {
        // P225 推广：ORIG 两条件各自 setcc 物化（is_stackable 直判 + setl），
        // `&&` 恒折直跳 ⇒ 拆嵌套 if。
        if (item->is_stackable())                                // +0x85 is_stackable
        {
            if (static_cast<const CStackableItem*>(item)
                    ->getStackableLimit() < count)
            {
                // +0xaf 取上限
                count = static_cast<const CStackableItem*>(item)
                            ->getStackableLimit();
            }
        }
        if (item->IsAvatarItem())               // +0xbf vtbl+0x10 虚槽；avatar 类禁加
            return false;
    }

    ENUM_ITEMSPACE outSpace;                    // -0x24 local_28（ORIG 无预清）
    int slot = user->AddItem((int)itemIdxOnlyServer, count,
                             static_cast<eItemAddReason>(6), outSpace, 0);  // +0xff reason6
    // ORIG +0x10a `js <尾 mov$0 块>`：成功条件正向包裹，失败块后置（P222/P59）。
    if (slot >= 0)
    {
        // [P8S2 2026-09-11] P139 修复：ORIG prett 直证 `itemIndexOnlyServer_t`
        // （typedef 已由 P7 落在 GameTypes.h:52）⇒ 参数拼写改用 typedef 名 +
        // 此处改 __PRETTY_FUNCTION__，发射与 ORIG 同名同 size（0x57）的
        // `_ZZN23DisPatcher_DebugCommand20_debugCommandAddItemEP5CUsermiE19__PRETTY_FUNCTION__`。
        cMyTrace(__PRETTY_FUNCTION__, 0xf4, 0)("%s:'%s' GM Mode Add Item(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), slot);
        user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                 outSpace, slot);   // +0x19f SendUpdateItemList(1,space,slot)
        return true;
    }
    return false;
}