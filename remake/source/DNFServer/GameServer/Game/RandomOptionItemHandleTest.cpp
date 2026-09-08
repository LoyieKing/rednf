// ============================================================================
// LinkFinal2 核销（/tmp/undef_r26.txt）—— random_option::CRandomOptionItemHandleTest
// 声明：CRandomOptionItemHandleTest.h。ORIG nm -S 核实：
//   C1/C2                  W 0x0822cb70 (0x15)：+0x00/+0x04 两 dword 清零
//   test_regist_auction(CUser*) W 0x085f3f94 (0x5)：纯 ret（ORIG 即空体）
//   test_drop              W 0x085f3cb8 (0x5)：纯 ret
//   test_unseal            W 0x085f3cbe (0x5)：纯 ret
//   test_change            W 0x085f3f9a (0x5)：纯 ret
//   test_reset             W 0x085f3fa0 (0x5)：纯 ret
//（_get_auction_id 0x085f3cc4 / _regist_auction 0x085f3dba 不在本批清单，未转写。）
// ============================================================================

#include "CRandomOptionItemHandleTest.h"

#include "CItemAmplifier.h"   // stAmplifyOption_t（Inven_Item::m_amp 完整类型）
#include "DBMgr.h"           // DBMgr::GetDBHandle / MySQL（含 GlobalData::s_db_mgr）
#include "Inven_Item.h"      // Inven_Item 完整布局（_regist_auction 逐字段读取）

namespace random_option
{

// ORIG C1 0x0822cb70：mov this 一次缓存后连发两 store（P29 初始化列表形态）。
CRandomOptionItemHandleTest::CRandomOptionItemHandleTest()
{
    // ORIG 0x0822cb70：一次 this 读共享两 store（8 字节整块）
    *(long long*)this = 0;
}

void CRandomOptionItemHandleTest::test_regist_auction(CUser* /*user*/) {}  // ORIG 纯 ret

void CRandomOptionItemHandleTest::test_drop() {}       // ORIG 纯 ret
void CRandomOptionItemHandleTest::test_unseal() {}     // ORIG 纯 ret
void CRandomOptionItemHandleTest::test_change() {}     // ORIG 纯 ret
void CRandomOptionItemHandleTest::test_reset() {}      // ORIG 纯 ret
// ============================================================================
// CRandomOptionItemHandleTest::_get_auction_id —— ORIG T 0x085f3cc4
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_get_auction_id.md）。
//   缓存命中判定为 ORIG 单次 or+test（mov this+4→edx; mov this→eax; or; test; je，
//   gcc4.1 对 a==0&&b==0 的德摩根合并；gcc4.4 不合并，故显式 or 保持形态）。
//   空缓存走 DB：GetDBHandle(s_db_mgr, 0xc, 0) → set_query("seLect max(auction_id)…")
//   → exec(true)/fetch()/is_valid_data(0)/get_longlong(0, this) 四级成功链，
//   任一级失败返回 0（ORIG call 后 xor $1/test/je + mov $0,%eax/%edx）。
// ============================================================================
long long CRandomOptionItemHandleTest::_get_auction_id()
{
    // ORIG 布局：or/test/je(DB) —— 非空缓存直落返回，DB 链后置（if-else 排布）。
    // [b28c-lane17] 判词：本形已复现 ORIG 的 or/test 折叠；唯一残余是
    // ORIG 0x085f3ccd 用**单次 this 装载**读两字段（`mov +4→%edx; mov +0→%eax`），
    // 4.4 恒先 `mov +4→%eax` 再回读 this（+2 条）——N18a 单段 regmove 不可造。
    if ((reinterpret_cast<unsigned int>(m_slot4)
         | reinterpret_cast<unsigned int>(m_slot0)) != 0)
    {
        return *(long long*)this;
    }
    else
    {
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
        db->set_query("seLect max(auction_id) from auction_main");
        if (!db->exec(true))
            return 0;
        if (!db->fetch())
            return 0;
        if (!db->is_valid_data(0))
            return 0;
        if (!db->get_longlong(0, *(long long*)this))
            return 0;
        return *(long long*)this;
    }
}

// ============================================================================
// CRandomOptionItemHandleTest::_regist_auction —— ORIG T 0x085f3dba
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_regist_auction.md）。
//   测试辅助：向 auction_main 表插入一条拍卖记录。blob_to_str 编码
//   item+0x25（random_option 段 0xe 字节）/ item+0x15（amplify 段 10 字节）；
//   23 个 SQL 变参按 ORIG 压栈序（0x8(%esp) 首参 → 0x64 尾参）照抄。
// ============================================================================
int CRandomOptionItemHandleTest::_regist_auction(Inven_Item& item, long long auctionId,
                                                 int occTime, int expireTime,
                                                 char const* ownerName)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    char* randomOptionStr = db->blob_to_str(1, (char*)&item + 0x25, 0xe);
    char* guidStr = db->blob_to_str(0, (char*)&item + 0x15, 10);
    // unsigned char：ORIG getAbilityType 后 movzbl 零扩展（P11；char 会 movsbl）。
    unsigned char ampType = item.m_amp.getAbilityType();
    unsigned short ampValue = item.m_amp.getAbilityValue();

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
        (unsigned char)item.m_field0,
        item.m_addInfo,
        item.m_addInfo2,
        (int)(item.m_attr & 0x1f),
        (int)(item.m_attr >> 5),
        item.m_fieldb,
        item.m_fieldd,
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

}  // namespace random_option
