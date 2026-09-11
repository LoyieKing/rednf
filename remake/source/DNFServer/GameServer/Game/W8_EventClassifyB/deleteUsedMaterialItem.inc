// deleteUsedMaterialItem（ORIG 0x0810eede）
// ORIG 无 failed 标志：delete_item 失败路径内联处理（回包循环 + logFormat
// 0x640 + return 8），成功路径 fall-through push_back+++i。回包循环
// 内 newSlot<0 时 logFormat 0x63d。全部 __PRETTY_FUNCTION__。
// [b28d-solo-W8_EventClassifyB 判词·归零（ae）] 本波真语义修正五处：
// ①item.m_addInfo2 = it->second 直写（ORIG +0x16c `mov %eax,-0x9b(%ebp)`
//   写 item+0x07 4B），并回读该字段作 create_item 第三参与 0x63d 日志实参
//   （原实现第三参直传 it->second、日志重调 it->first/second）；
// ②失败路径 logFormat 改两次独立 materials[i] 取用（ORIG +0x2f5/+0x302
//   两次 operator[]，P48 无 CSE；原实现缓存 `const pair& bad` 致少一次调用）；
// ③returnItems.size() != 0（P197④：empty() 恒 out-of-line call 无 setne 段）；
// ④delete_item 改正条件 if/else（ORIG +0x0b0 `test %al,%al; je +0x112`
//   成功路径直落，原 `if(!...)` 反置块序）；
// ⑤newSlot 循环外预清（ORIG +0x12a `movl $0x0,-0xc(%ebp)`，P198①）。
// check_tu 已归 ae；残余仅 make_pair 模板实参 `IRmRiE`(unsigned long&) vs
// `IRjRiE`(unsigned int&) —— Inven_Item::m_addInfo 声明宽度的共享头旋钮，
// 已上报 Main 决策（本 TU 单文件不可改）。
// [L14 round2 复核] 223/223、strict ndiff=**0**：前轮 3 行「差异」全为 .o 未解析
// 重定位的显示形（ORIG `$0x8b4c2c0/$0x8b4d400/$0x8b4c264` 串地址 vs 我方节内偏移
// `$0x2f/$0x240/$0xe8`）⇒ 实为等价，工具 NEAR 为伪差（P264/.o 口径）。
int CExchangeRandomItemReward::deleteUsedMaterialItem(
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
    CUser* user, IntPairVector& materials,
    IntPairVector& returnItems)
{
    size_t i = 0;
    while (materials.size() > i)
    {
        Inven_Item slotItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, materials[i].first);
        // ORIG +0x0b0 `test %al,%al; je +0x112`：成功路径（push_back）直落、
        // 失败路径后置 ⇒ 正条件 if/else（4.4 对 if(!f()) 恒失败直落 + xor）。
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, materials[i].first, materials[i].second,
                (eItemDelReason)3, true))
        {
            returnItems.push_back(std::make_pair(slotItem.m_addInfo,
                                                 materials[i].second));
            ++i;
        }
        else
        {
            // ORIG +0x118 `call size; test %eax,%eax; setne; test; je`（P197④：
            // empty() 恒 out-of-line call 无 setne 段）。
            if (returnItems.size() != 0)
            {
                // ORIG +0x12a `movl $0x0,-0xc(%ebp)`（P198①预清必写）⇒
                // newSlot 声明在循环外并初始化为 0，循环内赋值。
                int newSlot = 0;
                for (std::vector<std::pair<int, int> >::iterator it =
                         returnItems.begin();
                     it != returnItems.end(); ++it)
                {
                    Inven_Item item;
                    item.m_addInfo = it->first;
                    // ORIG +0x16c 直写 item+0x07（m_addInfo2）4 字节，随后
                    // create_item 第三参与该日志实参均从该槽重读。
                    item.m_addInfo2 = it->second;
                    G_CDataManager()->m_itemList->create_item(
                        (ENUM_ITEM_CREATE_TYPE)0, item, item.m_addInfo2);
                    newSlot =
                        user->getCurCharacInvenW()->insertItemIntoInventory(
                            item, (eItemAddReason)0xd, true, true);
                    if (newSlot < 0)
                    {
                        LogManager::logFormat(
                            1, "EventClassify.cpp", __PRETTY_FUNCTION__,
                            0x63d,
                            "CExchangeRandomItemReward::"
                            "deleteUsedMaterialItem, return_item failed , "
                            "User ch =%d , %d %d",
                            (int)user->getCurCharacNo(), item.m_addInfo,
                            item.m_addInfo2);
                    }
                }
            }
            // ORIG +0x2f5/+0x302 两次独立 operator[]（P48 无 CSE，禁引用缓存）。
            LogManager::logFormat(
                1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x640,
                "CExchangeRandomItemReward::deleteUsedMaterialItem failed "
                ", User ch =%d , %d %d",
                (int)user->getCurCharacNo(), materials[i].first,
                materials[i].second);
            return 8;
        }
    }
    return 0;
}