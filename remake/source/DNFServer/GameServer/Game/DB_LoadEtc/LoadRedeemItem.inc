// [b29-Lane21] 已修：`unsigned int srcLen` + 去 cast ⇒ dst 先算，ORIG 装载序复现。
// 残差（N 封顶）：N7 寄存器角色（ORIG `mov sig,%eax; add $0x40ce,%eax` 就地加；4.4
//   `mov sig,%eax; lea 0x40ce(%eax),%edx`）——成员名/`&arr[0]`/cast/匿名 struct 视图
//   四形实测均不翻，语义等价（161=161）。
// [b28d-solo-DB_LoadEtc] N封顶（N7）：ORIG 0x08406a6a；161=161、p167 CLEAR 14/14。
// 已修（本波）：①`char blob[0x6c7] = { 0 };` 替 memset call（P214③/P253；ORIG 内联
//   rep stos 慢路 + 尾字节，指令数 129→161 与 ORIG 全等）；②get_binary 第 3 参复用
//   outLen 具名槽（P244；原直填 0x6c7 使 4.4 折常量）。
// 残差（不可翻，N7）：uncompress_zip 实参装载的整组寄存器角色互换——ORIG
//   `mov 0xc(%ebp),%eax; add $0x40ce,%eax` + `mov -0xc(%ebp),%edx`（dst→eax）；
//   4.4 恒 `mov -0xc(%ebp),%eax` + `mov 0xc(%ebp),%edx; add $0x40ce,%edx`（dst→edx）。
// 助记符序列与槽位全等，仅 eax/edx 角色互换（P230 补 N7 族；446 后端已应用）。
// 已修（前波）：exec/fetch 具名 ok + 三处 trace 改 P227 临时链。
bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc14LoadRedeemItem…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect redeem_info from charac_inven_expand where charac_no=%u",
                  sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b16, 5)(
            "DB_LoadRedeemItemList::dispatch, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b20, 5)(
            "DB_LoadRedeemItemList::dispatch, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    // [b28d-solo-DB_LoadEtc] `={0}` 触发 4.4 内联 rep stos（P214③；原 memset 生 call），
    //   与 LoadPvPMissionList 同法；get_binary 第 3 参复用 outLen 具名槽（P244）。
    char blob[0x6c7] = { 0 };
    unsigned int outLen = 0x6c7;
    if (!db->get_binary(0, blob, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b28, 5)(
            "DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    // [K2ADBLogin 2026-09-11] 负结论（本波实测）：ORIG 0x08406c88 的 dst 形
    //   `mov 0xc(%ebp),%eax; add $0x40ce,%eax`（原地 add、结果留 %eax）源侧两种写法
    //   均不可复现——成员数组退化与显式指针算术 `(char*)sig + 0x40ce` 都产
    //   `lea 0x40ce(%eax),%edx`（4.4 地址生成恒 lea + 结果入 %edx）⇒ N107 反向
    //   （ORIG add / 4.4 lea）+ N7 寄存器角色，判封顶（读数 161/161 不变）。
    if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_redeemInfo), outLen, blob, srcLen))
    {
        return false;
    }
    return true;
}