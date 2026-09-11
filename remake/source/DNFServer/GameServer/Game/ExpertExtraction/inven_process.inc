// ============================================================================
// CExtraction::inven_process —— ORIG T 0x084a2cec（报告 inven_process.md）
//   副本背包试删材料 → find_item 构造新物品（make_item + set_add_info + 物品索引）
//   → 副本试插（0x35,0,1）；成功后真实背包删材料再插入，outSlots 输出
//   {插入槽, 物品索引, add_info}。
// ============================================================================
// [b28d-solo-ExpertExtraction] NEAR 修复：声明序改 copy→mem（ORIG 构造序 CInventory
//   先于 InventoryMemory）、delete_item 结果改 bool 具名局部（P158 复现
//   `movzbl; xor $1; test; je`）、slot 单变量复用（ORIG 两次 insert 同落 -0x20，
//   P160-162 槽复用律）。残余 N95（1 字节栈对齐）：mem 槽基 ORIG -0xc957（4.1 按
//   0xc13f 不补齐）vs 4.4 -0xc958（按 4 对齐）；删 CUser.h 的 m_tailC13f 亦不能上移
//   （4.4 强制对齐，且 CUser 中该成员因对齐仍占 0xc140、布局不变）。p167 23/23 CLEAR。
bool CExtraction::inven_process(CUser* user, short param,
                                std::vector<std::pair<int, int> >& needItems,
                                std::vector<int>& outSlots)
{
    CInventory* invenW = user->getCurCharacInvenW();
    CInventory copy;
    InventoryMemory mem;
    copy.SetInventoryMemory(&mem);
    copy.setCopy(*(const CInventory*)user->getCurCharacInvenRefR());
    bool deleted = copy.delete_item((INVEN_TYPE)1, (int)param, 1,
                                    (eItemDelReason)0x2d, 0);
    if (!deleted)
    {
        return false;
    }
    int first = needItems[0].first;
    int second = needItems[0].second;
    CItem* item = G_CDataManager()->find_item(first);
    Inven_Item newItem;
    item->make_item(newItem);
    newItem.set_add_info(second);
    newItem.m_addInfo = first;
    int slot = copy.insertItemIntoInventory(newItem, (eItemAddReason)0x35, 0, 1);
    if (slot == -1)
    {
        return false;
    }
    invenW->delete_item((INVEN_TYPE)1, (int)param, 1,
                        (eItemDelReason)0x2d, 1);
    slot = invenW->insertItemIntoInventory(newItem, (eItemAddReason)0x35, 1, 1);
    outSlots.clear();
    outSlots.push_back(slot);
    outSlots.push_back(first);
    outSlots.push_back(second);
    return true;
}