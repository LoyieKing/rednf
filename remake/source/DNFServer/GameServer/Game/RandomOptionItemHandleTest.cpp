// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RandomOptionItemHandleTest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CRandomOptionItemHandleTest.h"

#include "CItemAmplifier.h"   // stAmplifyOption_t（Inven_Item::m_amp 完整类型）
#include "DBMgr.h"           // DBMgr::GetDBHandle / MySQL（含 GlobalData::s_db_mgr）
#include "Inven_Item.h"      // Inven_Item 完整布局（_regist_auction 逐字段读取）

namespace random_option
{

// ORIG C1 0x0822cb70：mov this 一次缓存后连发两 store（P29 初始化列表形态）。
#include "RandomOptionItemHandleTest/CRandomOptionItemHandleTest.inc"
{
    // ORIG 0x0822cb70：一次 this 读共享两 store（8 字节整块）
    *(long long*)this = 0;
}

#include "RandomOptionItemHandleTest/test_regist_auction.inc"

#include "RandomOptionItemHandleTest/test_drop.inc"
#include "RandomOptionItemHandleTest/test_unseal.inc"
#include "RandomOptionItemHandleTest/test_change.inc"
#include "RandomOptionItemHandleTest/test_reset.inc"
// ============================================================================
// CRandomOptionItemHandleTest::_get_auction_id —— ORIG T 0x085f3cc4
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_get_auction_id.md）。
//   缓存命中判定为 ORIG 单次 or+test（mov this+4→edx; mov this→eax; or; test; je，
//   gcc4.1 对 a==0&&b==0 的德摩根合并；gcc4.4 不合并，故显式 or 保持形态）。
//   空缓存走 DB：GetDBHandle(s_db_mgr, 0xc, 0) → set_query("seLect max(auction_id)…")
//   → exec(true)/fetch()/is_valid_data(0)/get_longlong(0, this) 四级成功链，
//   任一级失败返回 0（ORIG call 后 xor $1/test/je + mov $0,%eax/%edx）。
// ============================================================================
#include "RandomOptionItemHandleTest/_get_auction_id.inc"
{
    // ORIG 布局：or/test/je(DB) —— 非空缓存直落返回，DB 链后置（if-else 排布）。
    // [b28c-lane17] 判词：本形已复现 ORIG 的 or/test 折叠；残余为单次 this 装载。
    // [b28d-31] 复核：字段 void*→unsigned int 同型化（P254 档③）消两段中转
    // （71→70）；残余 `mov 0x8(%ebp),%eax` this 重载（ORIG 0x085f3ccd 单次装载）
    // 四形（reinterpret 原序/交换、C cast、字段同型化）实测均不可翻 ⇒ N18a 封顶。
    // [b28e-S4] ORIG 0x085f3cca 单次 this 装载（`mov 0x8(%ebp),%eax;
    //   mov 0x4(%eax),%edx; mov (%eax),%eax; or`）——与返回处
    //   `*(long long*)this` 同形；原 `(m_slot4 | m_slot0) != 0` 让 4.4 对两个
    //   成员各发一次 this 重载（+1 条 70 vs 69）。改 8 字节读求 or/test 折叠。
    if (*(long long*)this != 0)
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
// b28d-31: N 封顶（N82）——ORIG 0x085f3dba；123 vs 126（帧/槽 + 变参装载）。
#include "RandomOptionItemHandleTest/_regist_auction.inc"
                                                 int occTime, int expireTime,
                                                 char const* ownerName)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    // [b28e-S4 回滚] ORIG 0x085f3e17 `mov %eax,%esi`（首个 blob_to_str 结果
    //   保活入 callee-saved，第二个落槽 -0x4c）——`register char*` 在真实函数里
    //   实测反劣 124→**125**（首个结果仍落槽 -0x24，且位置变 -0x24 vs ORIG -0x1c）
    //   ⇒ 4.4 -O0 下 register 不改变该变量的栈分配（探针里有效的简化上下文不成立）。
    //   按 N 封顶，保持具名形（124/123，残差 1 条 + 帧 0xac/0xbc）。
    char* randomOptionStr = db->blob_to_str(1, (char*)&item + 0x25, 0xe);
    char* guidStr = db->blob_to_str(0, (char*)&item + 0x15, 10);
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
        (unsigned char)item.m_field0,
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
