// ============================================================================
// CRandomOptionItemHandleTest::_regist_auction —— ORIG T 0x085f3dba
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_regist_auction.md）。
//   测试辅助：向 auction_main 表插入一条拍卖记录。blob_to_str 编码
//   item+0x25（random_option 段 0xe 字节）/ item+0x15（amplify 段 10 字节）；
//   23 个 SQL 变参按 ORIG 压栈序（0x8(%esp) 首参 → 0x64 尾参）照抄。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x085f3dba；123 vs 126（帧/槽 + 变参装载）。
int CRandomOptionItemHandleTest::_regist_auction(Inven_Item& item, long long auctionId,
                                                 int occTime, int expireTime,
                                                 char const* ownerName)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    // [b28e-S4 回滚] ORIG 0x085f3e17 `mov %eax,%esi`（首个 blob_to_str 结果
    //   保活入 callee-saved，第二个落槽 -0x4c）——`register char*` 在真实函数里
    //   实测反劣 124→**125**（首个结果仍落槽 -0x24，且位置变 -0x24 vs ORIG -0x1c）
    //   ⇒ 4.4 -O0 下 register 不改变该变量的栈分配（探针里有效的简化上下文不成立）。
    //   按 N 封顶，保持具名形（124/123，残差 1 条 + 帧 0xac/0xbc）。
    char* randomOptionStr = db->blob_to_str(1, reinterpret_cast<char*>(&item.m_random.m_random), 0xe);
    char* guidStr = db->blob_to_str(0, reinterpret_cast<char*>(&item.m_amp.m_guid), 10);
    // unsigned char：ORIG getAbilityType 后 movzbl 零扩展（P11；char 会 movsbl）。
    // [b28e-S4] ORIG 0x085f3e47 先调 +0xa7（getAbilityValue，`movzwl %ax` → -0x48）
    //   再调 +0x67（getAbilityType，`movzbl %al` → -0x44）⇒ 声明序 value 先 type 后
    //   （原 type 先 value 后使两 call 顺序与槽各差一档）。
    unsigned short ampValue = item.m_amp.getAbilityValue();
    unsigned char ampType = item.m_amp.getAbilityType();

    db->set_query(
        "inSert into auction_main(auction_id,occ_time,expire_time,owner_id,owner_name,"
        "buyer_id,buyer_name,price,instant_price,seal_flag,item_id,add_info,upgrade,"
        "seal_cnt,endurance,extend_info,amplify_option,amplify_value,owner_type,"
        "item_guid,unit_price,black_point,random_option) values(%llu,from_unixtime(%d),"
        "%d,%d,'%s',%d,'%s',%d,%d,%d,%u,%d,%hhu,%hhu,%hu,%hu,%hhu,%hu,%hhu,'%s',%d,%d,'%s')",
        auctionId,
        occTime,
        occTime + 0x3f480,
        expireTime,
        ownerName,
        -1,
        "",
        1,
        2,
        (unsigned char)item.m_seal,
        item.m_addInfo,
        item.m_addInfo2,
        // [b28e-S4] /tmp 探针（同旗标 c6root 4.4.7）实证：位域读
        //   `m_attrBits.m_attrLo` 生成 ORIG 0x085f3e92 的
        //   `movzbl 0x6(%eax),%eax; and $0x1f,%eax; movzbl %al,%eax`（位域=4 条，
        //   `(int)(m_attr & 0x1f)` 会多 `mov %eax,%edx` 中转）；`m_attr / 32`
        //   生成 ORIG 0x085f3e96 的 `shr $0x5,%al`（`>> 5` 走 32 位 `sar`）。
        //   两者对 unsigned char 语义等价（非负），仅 lowering 不同。
        (int)item.m_attrBits.m_attrLo,
        (int)(item.m_attr / 32),
        item.m_endurance,
        item.m_extendInfo,
        (int)ampType,
        (int)ampValue,
        0,
        guidStr,
        1,
        0,
        randomOptionStr);
    db->exec(true);
    return 1;
}