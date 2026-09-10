// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_DB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar DBAndExchangeServerProc + APSystem CDBAndExchangeServerProc
// 独立注册 TU（2026-08-23；2026-08-27 AltarDB 批追加 DB_Load/DB_Update/DB_StatisticAdvanceAltar/
// convertStringToZip/convertZipToString/g_*Buffer 全局）。实现：
//   advancealtar::DBAndExchangeServerProc::getFromMemory          ORIG T 0x0813394c
//   advancealtar::CharacAdvanceAltarManager::getCharacAdvanceAltarDbData ORIG T 0x08131710
//   advancealtar::CharacAdvanceAltarManager::isUpdated           ORIG T 0x081316a0
//   advancealtar::AdvanceAltarData::{get,getSlot,getBuyItem}     ORIG T 0x088996d8 / ...
//   advancealtar::DB_Update::makeRequest                         ORIG T 0x0813eb38
//   APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory ORIG T 0x08121616（纯空函数）
//   （追加段清单见文件尾注释块）
// ----------------------------------------------------------------------------
// [b28d-D03 判词 · L2Free4to15.S6 本波更新]（fresh `check_tu_routed.sh`：identical=83 ae=149
//   （合计 232）near=0 diff=5 = _Z41__static_initialization…（GI 假缺口：ORIG 该符号有 493 个
//   实例，工具按名匹配取首个 0x0807ee76（属 PacketTypes）⇒ 逐 TU 读数不可比，我方 ioinit 形
//   正确、零改动）+ 下列 4 件；
//   p167 调用点差集对三件实跑：ORIG calls == OUR calls（31/31、26/26、48/48），
//   MISSING/EXTRA 仅为 C1↔C2、D1↔D2 同址别名伪差（P93）⇒ 语义无缺口）
//   · getFromMemory 0x0813394c —— 32/31：唯一残差 = P211 档③ store 装载序
//     （ORIG `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)` RHS-first + edx 中转；
//     我方 LHS-first 直存）。字段侧/callee 侧两旋钮历轮已试（判词见函数体）⇒ 封 N76 族。
//   · requestLoadFromDBAndES 0x08133784 —— **[S6 本波] 122→124（123/124，gap +1→−1）**：
//     尾块 `mov $0x1,%ebx` 差异归零 —— ORIG 两臂各一份 ⇒ 源形为两臂各自 `return true;`
//     （非共享尾块单 return）；改写后该差异消失。唯一残差 = `data->m_characNo` store 的
//     LHS/RHS-first 1 条（4.4 对局部指针字段 store 恒 RHS-first+edx 中转，ORIG LHS-first
//     直存）⇒ N76 族。p167 26/26 且仅 C1/C2、D1/D2 别名差 ⇒ 语义等价封顶。
//   · DB_StatisticAdvanceAltar::dispatch 0x081412b2 —— 312/308：p167 31/31（仅 cMyTrace
//     C1↔C2 别名差）；gap −4 = 跨版本 EH 域形态（sprintf/__cxa_allocate_exception/throw 段
//     的常量地址伪代码化 + 我方少 4 条 cleanup 装载）⇒ N104（EH 尾块相对序）封顶。
//   · DB_Update::update 0x0813ec5e —— **[S6 本波] 469→467（466/467，gap −3→−1）**：
//     `db.m_update==1 && isValidRidableId(...)` 的 4.4 bool 物化岛（`mov $1/$0,%eax; test; je`）
//     已消 —— 拆为两段独立 guard+continue，与 ORIG 0x0813ecb6/0x0813ecce 两处
//     `xor $1; test %al,%al; jne`（各带 nop/jmp continue 中继 +0x6fa/+0x6fd）逐条对齐
//     （探针 t1/t2 实证：`a&&b` 在 4.4 -O0 恒物化，两段 guard 生 ORIG 形）。残差 = a) 帧
//     0xdc vs 0xbc 与全部槽号平移 0x10（N82：4.4 块内对象按序压深 —— insert 分支"重声明 q*"
//     实验使帧 0xbc→0xcc、槽再深 0x10，实测反劣已回滚）；b) `db.m_update != 1` 1 条
//     （ORIG 的 `xor $1` 形要求字段为 bool；现 unsigned char，advancealtar.h:246
//     —— **.h 侧建议：uchar→bool（1B/偏移 0x517/mangled 零影响）本泳道不改，只报告**）。
//     p167 48/48 仅 std::string C1/C2、D1/D2 别名差。
//   · DB_Load::SendResult 0x0813ea46 —— **[S6 本波归零] 71→66（66/66 AE）**：原判 N122/N93附
//     撤销 —— ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp); je +0xe6` 是**正面条件包裹**形
//     `if (ok) { 体 } return true;`（-0x1c 即 bool 形参 spill 槽），非就地取反；探针实证
//     `bool f(bool ok){ if(ok){g();} return true; }` 4.4 生逐条同形（`mov 0x18(%ebp),%eax;
//     mov %al,-0x1c(%ebp); cmpb $0x0,-0x1c(%ebp); je …`），而 `if(!ok) return true;` 生
//     `movzbl; xor $1; test` + return 物化漂移。结构改写后归位（AE 66/66）。
#include <cstring>
#include "CUser.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "advancealtar.h"
#include "PacketTypes.h"  // MSG_CHARAC_SELECT 完整类型（requestLoadFromDBAndES 读 m_slot）
#include <cstdio>
#include "DBDispatcher.h"
#include "AdvanceAltarZip.h"    // compress_zip/uncompress_zip 共享声明（唯一实现在 APSystemOnlyOrig.cpp）
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "CEnvironment.h"       // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP

#include <cstdlib>
#include <ctime>
#include <string>
#include "DBMgr.h"          // DBMgr 完整类型（GetDBHandle）+ MySQL.h
#include "CStreamGuard.h"

namespace advancealtar
{

// [objdump 0x813dc48] b28d-26: AE。修法=两处无符号比较取「否定早返」正向形
//   （zipCap <= zlen / strCap <= rawLen）+ zlen 同型化 unsigned（P209 姊妹：4.4 对
//   unsigned 形参比较以形参为 dst 直发 ja）。ORIG 0x813dc73 / 0x813dcda 实证。
#include "advancealtar_DB/convertZipToString.inc"
                        char* strBuf, unsigned int strCap)
{
    // Ghidra 侧 local_14 为 uint（ORIG 用 ja 无符号序），同型化让 4.4 以形参为 dst。
    const unsigned int zlen = mysql->get_binary_length(col);
    if (zlen == 0)
        return true;
    // ORIG 0x813dc73：`mov 0x14(%ebp),%eax; cmp -0x10(%ebp),%eax; ja <body>`
    // = 早返 `if (zipCap <= (unsigned)zlen) return false;`（!C 取反后 GCC 直接发 ja）。
    if (zipCap <= zlen)
        return false;
    std::memset(zipBuf, 0, zipCap);
    if (!mysql->get_binary(col, zipBuf, static_cast<int>(zlen)))
        return false;
    const unsigned int rawLen = (*reinterpret_cast<unsigned int*>(zipBuf)) & 0x3fffffff;
    // ORIG 0x813dcda：`mov 0x1c(%ebp),%eax; cmp -0xc(%ebp),%eax; ja <body>`（同形）。
    if (strCap <= rawLen)
        return false;
    std::memset(strBuf, 0, strCap);
    unsigned int len = rawLen;
    // P168a①：bool callee 裸负形 = ORIG `call; xor$1,%eax; test%al; je` choose。
    if (!uncompress_zip(strBuf, len, zipBuf, zlen))
        return false;
    return true;
}

// ---- AdvanceAltarData 数据模型（advancealtar__AdvanceAltarData 报告）----
#include "advancealtar_DB/AdvanceAltarData.inc"
    : m_stages(), m_buyItems1(), m_buyItems2(), m_buyItems3(),
      m_slots1(), m_slots2(), m_rewards(), m_update(0)
{
}

#include "advancealtar_DB/dtor_AdvanceAltarData.inc"
{
}

// ORIG T 0x08899b4e：把 map<uchar,_Slot> 拷入定长槽数组（槽 0 长度 10）。
#include "advancealtar_DB/getSlot.inc"
{
    int local_10 = 0;
    for (std::map<unsigned char,_Slot>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        // [b28d-solo-advancealtar_DB] AE（orig 83 / ours 83，clean resid=0）。
        //   语义（je 条件码逐条推导）：ORIG 0x08899b85 `test eax,eax; sete %al;
        //   test %al,%al; je <+0x5d>` —— sete 后 test 使 ZF=(x!=0)，je 在 x!=0 时
        //   跳第二检查 ⇒ **x==0 才 ++it**（勿按 setcc 字面反推）。
        //   归零钥匙：① 第一检查 `==0` 直写（ORIG sete 形）；② 第二检查按 P222
        //   正极性 `!=0` 写（ORIG 0x08899bbf je 落 memcpy）——单独改任一处各留残差。
        //   P183：ORIG 无具名 `_Slot& slot` 槽（每次 ptEv 重算）⇒ 直接写
        //   `it->second.m_int1`；P28：增量段调 ppEi ⇒ 源形必写后置 `it++`。
        if (it->second.m_int1 == 0)          // 源槽空 → 跳过
        {
            it++;
        }
        // P222 正极性翻转（语义不变）：ORIG 0x08899bbf `test; je <memcpy>` ——
        // 目标非空走 ++local_10 分支，写成 `!=0` 正极性才产 je 落 memcpy。
        else if (out[local_10].m_int1 != 0)  // 目标槽非空 → 前进目标下标
        {
            ++local_10;
        }
        else
        {
            std::memcpy(&out[local_10], &it->second, 10);
            it++;
            ++local_10;
        }
    }
}

// ORIG T 0x08899a3a：把 map<int,_BuyItem> 拷入定长购买项数组（每项 12）。
// [b28d-solo-advancealtar_DB] AE（orig 83 / ours 83，clean resid=0）。同 getSlot：
//   语义 ORIG 0x08899a75 `test; setle %al; test; je <+0x5d>` ⇒ **x<=0 才 ++it**。
//   归零钥匙：① 第一检查 `< 1` 直写（setle 形）；② 第二检查按 P222 正极性 `>0`
//   写（ORIG 0x08899ab0 jle 落 memcpy）。两处联动才归零。
#include "advancealtar_DB/getBuyItem.inc"
{
    int local_10 = 0;
    for (std::map<int,_BuyItem>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        // [b28d-solo-advancealtar_DB] 语义核对（同 getSlot）：ORIG 0x08899a75
        //   `test; setle %al; test; je <+0x5d>` ⇒ x<=0 才 ++it（setle 后 test 使
        //   ZF=(x>0)，je 在 x>0 时跳第二检查）。第二检查 0x08899ab0
        //   `test; jle <memcpy>` ⇒ 目标 m_field4<=0 才写入。P183/P28 同上。
        if (it->second.m_field4 < 1)          // 源项无效 → 跳过
        {
            it++;
        }
        // P222 正极性翻转（语义不变）：ORIG 0x08899ab0 `test; jle <memcpy>`。
        else if (out[local_10].m_field4 > 0)  // 目标项有效 → 前进目标下标
        {
            ++local_10;
        }
        else
        {
            std::memcpy(&out[local_10], &it->second, 0xc);
            it++;
            ++local_10;
        }
    }
}

// ORIG T 0x088996d8：把 AdvanceAltarData 序列化到 _AdvanceAltarDbData（0x518）。
#include "advancealtar_DB/get.inc"
{
    out.m_update = m_update;                          // +0x517 = this[0xcc]
    // [b28d-solo-advancealtar_DB] P68 cast 形：ORIG 0x088996e4 先载 this→edx 再载
    //   out→eax 再压长度；无 cast 的 &out 先求值使 4.4 交错（本行 cast 试形）。
    std::memcpy((char*)&out.m_ridable, (const char*)this, 0x21);  // 前 0x21（_RidableBase 扁平区）

    int local_10 = 0;
    for (std::map<int,_Stage>::const_iterator it = m_stages.begin();
         it != m_stages.end() && local_10 <= 0x31; )
    {
        // [b28d-solo-advancealtar_DB] P157/P183：ORIG 0x0889972f 段每次
        //   `call ptEv; mov 0x4(%eax),%eax` 重发完整访问链，无具名引用槽
        //   （旧 `const _Stage& st` 多 -0x10 槽 + add 链）。
        if (it->second.m_field0 < 1)                  // value.m_field0 < 1 → 跳过
        {
            it++;   // P28：ORIG 增量段调 ppEi（后置）⇒ 源形必写 it++
        }
        else
        {
            // ORIG 目标地址 = &out + 0x21 + local_10*8 → m_stages[local_10]
            std::memcpy(&out.m_stages[local_10], &it->second, sizeof(_Stage));
            it++;
            ++local_10;
        }
    }

    // ORIG 目标基址 +0x1b1 / +0x229 → 具名数组成员（元素宽 10 / 12 与 pack(1) 布局一致）。
    getSlot(m_slots1, out.m_slots, 0xc);
    getSlot(m_slots2, out.m_slots, 0xc);
    getBuyItem(m_buyItems1, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems2, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems3, out.m_buyItems, 0x32);

    local_10 = 0;
    for (std::map<int,_Reward>::const_iterator it = m_rewards.begin();
         it != m_rewards.end() && local_10 <= 0x1d; )
    {
        if (it->second.m_field0 < 1)
        {
            it++;
        }
        else
        {
            // ORIG 目标地址 = &out + 0x481 + local_10*5 → m_rewards[local_10]
            std::memcpy(&out.m_rewards[local_10], &it->second, sizeof(_Reward));
            it++;
            ++local_10;
        }
    }
}

// ---- CharacAdvanceAltarManager（CUser.h 声明）----
// ORIG T 0x08131710：把管理器内单个 AdvanceAltarData 拷到 _CharacAdvanceAltarDbData.m_db。
#include "advancealtar_DB/getCharacAdvanceAltarDbData.inc"
{
    // ORIG 循环 local_10 <= 0（一次），m_data 按 stride 0xd0 取第 i 行
    for (int local_10 = 0; local_10 <= 0; ++local_10)
    {
        m_data[local_10].get(out.m_db[local_10]);
    }
}

// ORIG T 0x08899244（objdump 全文核对）：memset 前 0x21（_RidableBase）→
// m_update=0 → 7 个 map clear（stages/buyItems1..3/slots1/slots2/rewards）。
#include "advancealtar_DB/reset.inc"
{
    std::memset((char*)this, 0, 0x21);
    m_update = 0;
    m_stages.clear();
    m_buyItems1.clear();
    m_buyItems2.clear();
    m_buyItems3.clear();
    m_slots1.clear();
    m_slots2.clear();
    m_rewards.clear();
}

// ORIG T 0x081316a0：检查首个 AdvanceAltarData 的更新标志（manager+0xd0 == m_data.m_update）。
#include "advancealtar_DB/isUpdated.inc"
{
    for (int local_8 = 0; local_8 <= 0; ++local_8)
    {
        if (m_data[local_8].m_update != 0)
            return true;
    }
    return false;
}

// ORIG T 0x813159a：槽位坐骑有效时把免费票补为配置值（CDataManager+0x6364
// = m_altarConfig.m_0）并置 m_data.m_update=1。
#include "advancealtar_DB/refillTicketFree.inc"
{
    for (int i = 0; i < 1; ++i)
    {
        // [b28d-solo-advancealtar_DB] P183：ORIG 0x081315aa 段每次访问都重发
        //   `mov i,%eax; mov this,%edx; imul $0xd0,%eax,%eax; lea (%edx,%eax,1),%eax`
        //   全链（三处 m_data[i] 各自展开），常量下标 m_data[0] 会被折成单 disp。
        if (isValidRidableId(static_cast<advancealtar::RidableId::T>(m_data[i].m_ridableId)))
        {
            m_data[i].m_ticket = G_CDataManager()->m_altarConfig.m_0;
            m_data[i].m_update = 1;
        }
    }
}

// ORIG T 0x08131660（docs/class_func_reports/advancealtar__CharacAdvanceAltarManager/
// reset.md 全文）：current(+0x00)=0；循环 local_10<=0 一次 reset +i*0xd0+4 处 AdvanceAltarData。
#include "advancealtar_DB/reset_1.inc"
{
    m_currentAdvanceAltarData = 0;
    for (int local_c = 0; local_c <= 0; ++local_c)
    {
        // 单实例布局：m_data@+0x04、stride 0xd0 == ORIG `this + local_c*0xd0 + 4`。
        m_data[local_c].reset();
    }
}

// ----------------------------------------------------------------------------
// +0x00 承载 AdvanceAltarData* current（setCurrentAdvanceAltarData 0x08131254
// 实证：命中槽位写 this+4+i*0xd0，无匹配置 0；reset 置 0）。权威头 advancealtar.h
// 以 union（int m_field0 ∪ AdvanceAltarData* m_currentAdvanceAltarData）承载，
// 以下 const 访问器直接取具名指针成员。
// ----------------------------------------------------------------------------

// ORIG T 0x081312d0（getCurrentRidableId.md 全文）：current 空 → 0；
// 否则 *current（AdvanceAltarData::m_ridableId，_RidableBase+0x00）。
#include "advancealtar_DB/getCurrentRidableId.inc"
{
    if (m_currentAdvanceAltarData == 0)
        return 0;
    return m_currentAdvanceAltarData->m_ridableId;
}

// ORIG T 0x081312ec（isAvailableTciket.md 全文）：current 空 → 0；
// 否则 *(int*)(cur+4) + *(int*)(cur+8) > 0（免费票 + 现持票）。
#include "advancealtar_DB/isAvailableTciket.inc"
{
    if (m_currentAdvanceAltarData == 0)
    {
        return false;
    }
    // ORIG 4 字节读 +0x04 / +0x08 两票池（DB 列对照：+0x04=ticket_free、
    // +0x08=ticket_cera，见 _RidableDbHead；_RidableBase 现有成员名与该列名
    // 相反，属命名口径问题，本批只去裸偏移不改共享成员名）。
    // ORIG 无任何槽：指针逐次重读、和与比较全寄存器直算（P0/P183）。
    return (m_currentAdvanceAltarData->m_ticket +
            m_currentAdvanceAltarData->m_freeTicket) > 0;
}

// ORIG T 0x0813131a（isEnterStage.md 全文）：current 空 → 0；在
// current->m_stages(+0x24, map<int,_Stage>) find(stageId) 命中 → 1。
#include "advancealtar_DB/isEnterStage.inc"
{
    // [b28d-solo-advancealtar_DB] P0/P183：ORIG 0x08131320/3336/1366 三处各自
    //   `mov 0x8(%ebp),%eax; mov (%eax),%eax` 重读 current（无具名局部槽）。
    //   P96/P220：ORIG 两次 const_iterator 转换构造 + `eq` 调用 ⇒ 具名
    //   const_iterator + `if (it == end()) return false; return true;`（P56 分支形）。
    if (m_currentAdvanceAltarData == 0)
        return false;
    std::map<int, _Stage>::const_iterator it =
        m_currentAdvanceAltarData->m_stages.find(stageId);
    if (it == m_currentAdvanceAltarData->m_stages.end())
        return false;
    return true;
}

// ---- getTowerInfo（ORIG T 0x081328ea，getTowerInfo.md 全文）----
// current(+0x00) 空→0；current->m_buyItems3(+0x6c) find(towerIdx) 未命中→0；
// G_CDataManager()+0x631c（m_altarShop）.getBuyUpgradeDataVec(*current,2) 空→0；
// 向量中 m_nItemId==towerIdx 项未命中→0；getFieldDataPoint(买项.m_short8,
// FT_INDEX=1) 空→0；买项计数 m_short8==0→0；否则 *out1=FieldData+4(m_value)、
// *out2=(int)m_short8，返回 1。
#include "advancealtar_DB/getTowerInfo.inc"
                                             int& outCount) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return false;
    }
    const std::map<int, _BuyItem>& buys = cur->m_buyItems3;
    std::map<int, _BuyItem>::const_iterator it = buys.find(towerIdx);
    if (it != buys.end())
    {
        std::vector<BuyUpgradeData>* vec =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(
                cur->m_ridableId, AdvanceAltarShopType::T_2);
        if (vec == 0)
        {
            return false;
        }
        BuyUpgradeData* data = 0;
        for (std::vector<BuyUpgradeData>::iterator vi = vec->begin();
             vi != vec->end(); vi++)
        {
            if (vi->m_nItemId == towerIdx)
            {
                data = &*vi;
                break;
            }
        }
        if (data == 0)
        {
            return false;
        }
        const FieldData* point =
            data->getFieldDataPoint((int)it->second.m_short8, FieldType::FT_INDEX);
        if (point == 0)
        {
            return false;
        }
        if (it->second.m_short8 == 0)
        {
            return false;
        }
        outValue = point->m_value;
        outCount = (int)it->second.m_short8;
        return true;
    }
    return false;
}

// ---- DB_Update（advancealtar__DB_Update 报告）----
// ORIG T 0x0813eb38：opcode 0x1a | uid | memcpy 0x51c → DB_Q。
#include "advancealtar_DB/makeRequest.inc"
{
    // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传 CStreamGuard（P0/P89）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1d9),
                       true);
    guard.operator*()->operator<<(0x1a);
    guard.operator*()->operator<<(uid);
    _CharacAdvanceAltarDbData* sig =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memcpy(sig, &data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DBAndExchangeServerProc（advancealtar__DBAndExchangeServerProc 报告）----
// ORIG T 0x0813394c：characNo→data.m_characNo；拷 AdvanceAltarData；isUpdated 则 DB_Update。
// [b28d-solo-advancealtar_DB] N76 封顶（orig 31 / ours 30，clean 归一 resid=5）：
//   ORIG 0x0813395d `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)`（RHS-first
//   +edx 中转）；4.4 对 `data.m_characNo = (int)getCurCharacNo()` 恒 LHS-first
//   `mov 0xc(%ebp),%edx; mov %eax,(%edx)`。已试形：(unsigned)cast/(int)cast/裸形/
//   P226 语句表达式（后者升格 ebx 破帧、`push %ebx; sub $0x14`）——字段与 callee
//   返回同为 unsigned 无可动旋钮（P211 反例）⇒ 无 C++98 源形可复现。语义等价。
//   if/else 空 else 复现 ORIG 体尾 jmp+nop 岛（P285）保留。
#include "advancealtar_DB/getFromMemory.inc"
{
    // [b28d-solo-advancealtar_DB] P211 档③ callee/RHS 侧旋钮：ORIG 0x0813395d
    //   `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)` = RHS-first + edx 中转；
    //   字段 m_characNo 为 unsigned int 与 getCurCharacNo()（unsigned int）同型时 4.4
    //   恒 LHS-first 直存。RHS 显式 (int) 造符号性异型即翻正（mangled/布局零影响）。
    // [b28d-solo-advancealtar_DB] ORIG 0x0813395d 为 RHS-first+edx 中转（N76 族）；
    //   实测 (int)/(unsigned) cast、语句表达式三形 4.4 恒 LHS-first 或升格 ebx，
    //   无 C++98 源形可复现 ⇒ N76 封顶（详见函数头判词）。
    data.m_characNo = (int)user.getCurCharacNo();
    user.m_advanceAltar.getCharacAdvanceAltarDbData(data);
    if (user.m_advanceAltar.isUpdated())
    {
        DB_Update::makeRequest((int)user.GetUID(), data);
    }
    else
    {
        return;
    }
}

// ---- requestLoadFromDBAndES（advancealtar__DBAndExchangeServerProc 报告全文）----
// ORIG T 0x08133784：reset 管理器 → Acquire stream(opcode 0x19) → uid →
// DisableCharacInfo(10) →（ValidLastLogin 时补 channelNo/characNo）→
// GetInBuffer<_CharacAdvanceAltarDbData> 清零 0x51c 后写 m_characNo →
// ValidLastLogin ? put 队列5 : put DB_Q(2) → 返回 1。
// [S6 本波] 123/124（我方 122→124）：原 N93附 尾块差异已消 —— ORIG 两臂各一份
//   `mov $0x1,%ebx` ⇒ 源形为两臂各自 `return true;`；改写后该差异归零。
// [b28d-solo-advancealtar_DB] 余 N76+N104 封顶（orig 123 / ours 124，clean resid=25）：
//   (1) `data->m_characNo = get_charac_no(...)`：ORIG 0x081338d6 LHS-first 直存，
//       4.4 对**局部指针**字段 store 恒 RHS-first+edx 中转（探针 f2 形参指针=LHS-first，
//       局部指针形恒值先）⇒ N76 族；
//   (2) 尾块 `mov $0x1,%ebx`：ORIG 两臂各一份（4.1 把 return true 内联到两臂），
//       4.4 只在共享尾块一份 ⇒ N93附/跨版本 CFG；
//   (3) EH landing 块相对序（ORIG 居中、4.4 压尾）⇒ N104。
//   P167 CLEAR（26/26）、语义等价。
#include "advancealtar_DB/requestLoadFromDBAndES.inc"
{
    user.m_advanceAltar.reset();  // ORIG CharacAdvanceAltarManager::reset @0x08131660

    // [b28d-solo-advancealtar_DB] P89/P259：ORIG 无 stream 具名槽（0x0813379e
    //   `mov &s_stream_pool,%eax` → Acquire 结果经 eax 直压 ctor 的 0x4(%esp)，
    //   guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar.cpp", 0xc3a), true);
    guard.operator*()->operator<<(0x19);
    guard.operator*()->operator<<(static_cast<int>(user.GetUID()));
    user.DisableCharacInfo(CACHE_ADVANCE_ALTAR);  // ORIG push 0xa（枚举名 Prod_DispatcherHelpers 落地）

    if (user.IsValidLastLoginCharac())
    {
        guard.operator*()->operator<<(user.GetLastLoginChannelNo());
        guard.operator*()->operator<<(user.GetLastLoginCharacNo());
    }

    _CharacAdvanceAltarDbData* data =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memset(data, 0, 0x51c);
    // ORIG 0x081338d6 `call get_charac_no; mov -0xc(%ebp),%edx; mov %eax,(%edx)`
    //   = LHS-first 直存（P211 同型）⇒ 去掉旧 static_cast<unsigned int>（异型化反产
    //   eax→edx 中转）。
    data->m_characNo = user.get_charac_no(static_cast<int>(msg.m_slot));

    // ORIG 0x081338ee `test %al,%al; je <put(2)>` ⇒ 正极性：IsValid 真走 fall-through
    //   put(队列5)，假走 DB_Q(2)。
    // [L2Free4to15.S6 本波] ORIG 两臂各一份 `mov $0x1,%ebx`（0x0813392x/0x0813394x）
    //   ⇒ 源形为两臂各自 `return true;`（非共享尾块单 return）。
    if (user.IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);  // 第 6 槽（ORIG 无枚举名）
        return true;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return true;
    }
}

} // namespace advancealtar

namespace APSystem
{

// ORIG T 0x08121616（纯空函数）：行动力从内存缓存加载——本版本 ORIG 体为空。
#include "advancealtar_DB/GetActionPointFromMemory.inc"
{
}

} // namespace APSystem

// ---- CUser 跨类叶子助手（Prod_GlobalAdv；全局作用域，置于文件尾避免命名空间混淆）----

// ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION：
// movb $0,(this+0x8d0fd+section) —— m_pad8d0fd[section] 清零。
#include "advancealtar_DB/DisableCharacInfo.inc"
{
    m_pad8d0fd[static_cast<int>(section)] = 0;
}

// ============================================================================
// 2026-08-27 AltarDB 批追加段（ORIG 全真转写；报告 docs/class_func_reports/
// advancealtar__DB_Load|DB_Update|DB_StatisticAdvanceAltar/ + objdump）
//   DB_Load      : C1/C2 W 0x844ddea、D1/D2 W 0x1420a0、D0 W 0x1420d0、
//                  dispatch T 0x813ddb2、Select T 0x813de40、SendResult T 0x813ea46、
//                  parseStage/parseSlot/parseBuyItem/parseReward T 0x813e3e8/...
//   DB_Update    : C1/C2 W 0x844de06、D1/D2 0x1420ee、D0 0x14211e、
//                  dispatch 0x813ec24、update 0x813ec5e、make*String 0x813f380..
//   DB_Statistic : C1/C2 W 0x844fc3a、D1/D2 0x142952、D0 0x142982、dispatch T 0x81412b2
//   convertStringToZip  T 0x813dd3e   convertZipToString T 0x813dc48
//   g_Zip*/g_Str* 8 个 B 全局（各 0x800，ORIG @0x09400380..0x09403b80）
// convert* 前向声明（实现于本 TU 文件尾 namespace advancealtar 内；Select/update 先用）
namespace advancealtar
{
#include "advancealtar_DB/convertStringToZip.inc"
                        unsigned int srcLen, unsigned int& realLen);
#include "advancealtar_DB/convertZipToString_1.inc"
                        char* strBuf, unsigned int strCap);
}  // namespace advancealtar

#include "SigTypes.h"  // STAdvanceAltarEntranceData / STAdvanceAltarLayerStatData（GetOutBuffer 实例化需要 sizeof）

// ORIG _Z19__GetCurrentDNFDateR2tm（T 0x86b1fc0，定义 GameUtils.cpp）——仓库暂无头声明。
#include "advancealtar_DB/__GetCurrentDNFDate.inc"

namespace advancealtar
{

// ---- ORIG namespace 内 static 常量副本（本 TU 使用处引用；各 TU 独立发射
//      _ZN12advancealtarL...E，与 ORIG 多副本形态一致）----
static const int kMaxStrBuffer = 2048;          // 0x08b6d3d0
static const int kMaxZipBuffer = 2048;          // 0x08b6d3cc
static const char* const kDelimterItem = "|";   // 0x08b6d3d8 指针 → '|'(0x7c)
static const char* const kDelimterField = ",";  // 0x08b6d3dc 指针 → ','(0x2c)
static const int kMaxStageCount = 50;          // 0x08af2e6c
static const int kMaxSlotCount = 12;           // 0x08af2e70
static const int kMaxBuyItemCount = 50;        // 0x08af2e74
static const int kMaxRewardCount = 30;         // 0x08af2e78

// ---- 8 个全局扁平缓冲区（ORIG B 符号，各 0x800 字节）----
char g_ZipStageBuffer[0x800];     // 0x09400380
char g_StrStageBuffer[0x800];     // 0x09400b80
char g_ZipSlotBuffer[0x800];      // 0x09402380
char g_StrSlotBuffer[0x800];      // 0x09402b80
char g_ZipBuyItemBuffer[0x800];   // 0x09401380
char g_StrBuyItemBuffer[0x800];   // 0x09401b80
char g_ZipRewardBuffer[0x800];    // 0x09403380
char g_StrRewardBuffer[0x800];    // 0x09403b80

// charac_advance_altar 行头字段视图：改用共享头 advancealtar.h 的 _RidableDbHead
// （union 叠加在 _AdvanceAltarDbData::m_ridable 上，字段偏移/宽度按 ORIG
//  Select.md/update.md 指令实证）。原 TU 本地视图 DbHeadView 因自然对齐与
// ORIG 指令宽度不符（+0x04/+0x08 ORIG 为 get_short(int,int&) 的 4 字节槽，
// 且 +0x0c 起三列 ORIG 间隔 4 字节）而删除。

// ---- DB_Load ----
class DB_Load : public DBDispatcher
{
public:
    DB_Load();
    ~DB_Load();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
    bool Select(_CharacAdvanceAltarDbData& data);
    bool SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok);
    bool parseStage(char* str, _Stage* out, int count);
    bool parseSlot(char* str, _Slot* out, int count);
    bool parseBuyItem(char* str, _BuyItem* out, int count);
    bool parseReward(char* str, _Reward* out, int count);
};

// [objdump 0x844ddea] base ctor + 设派生 vptr —— 编译器生成。
#include "advancealtar_DB/DB_Load.inc"
#include "advancealtar_DB/dtor_DB_Load.inc"

// [objdump 0x813ddb2]
#include "advancealtar_DB/dispatch.inc"
{
    if (!DBDispatcher::dispatch(uid, uniqueId, stream))
        return false;
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    bool ok = Select(*data);
    return SendResult(uid, uniqueId, *data, ok);   // ORIG eax = SendResult 返回值
}

// [report Select.md / objdump 0x813de40]
#include "advancealtar_DB/Select.inc"
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    mysql->set_query(
                     "select ridable_id, ticket_free, ticket_cera, \t\t\t\t  star_game, star_cera, "
                     "star_usable, \t\t\t\t  survival_best, star_reset_count, is_unlock_stage_effect, "
                     "\t\t\t\t  stage_list, slot_list, buy_item_list, reward_list \t\t\t\t  from "
                     "charac_advance_altar\t\t\t\t\t  where charac_no = %u",
                     data.m_characNo);
    if (!mysql->exec(true))
        return false;

    const int nRows = mysql->get_n_rows();
    for (int row = 0; row < nRows && row < 1; ++row)
    {
        if (!mysql->fetch())
            return false;

        // ORIG 行基址 local_14 = &data + row*0x518 + 4 → m_db 起
        //（行距 = sizeof(_AdvanceAltarDbData) = 0x518）。
        // [b28d-solo-advancealtar_DB] ORIG 0x0813decc `imul $0x518,%eax,%eax;
        //   add 0xc(%ebp),%eax; add $0x4,%eax`（add 链）；char* 加法形产 lea 链。
        //   指针算术 `&m_db[0] + row` 试形。
        _AdvanceAltarDbData* dbRow = &data.m_db[0] + row;
        // [b28d-solo-advancealtar_DB] P89/P0：ORIG 无 view 槽（行头字段以
        //   dbRow+disp 直取，_RidableBase 与 _RidableDbHead 逐字节同布局）⇒ 直接
        //   用 m_ridable 具名成员，删 m_head() 调用与引用槽。
        int col = 0;

        int ridableId = 0;
        if (!mysql->get_int(col++, ridableId))                       // [0x0811692c]
            return false;
        if (!isValidRidableId(static_cast<RidableId::T>(ridableId)))
        {
            LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp",
                                  __PRETTY_FUNCTION__,
                                  0xb1, "invalid RidableId=%d", ridableId);
            return false;
        }
        dbRow->m_ridable.m_ridableId = static_cast<RidableId::T>(ridableId);

        if (!mysql->get_short(col++, dbRow->m_ridable.m_ticket))      // [0x0814201c get_short(int,int&)]
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_freeTicket))
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_curStar))       // [0x0811692c]
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_starPoolBase))
            return false;
        if (!mysql->get_int(col++, dbRow->m_ridable.m_starPoint))
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_maxClearRound))  // [0x080fddec get_short(int,short&)]
            return false;
        if (!mysql->get_short(col++, dbRow->m_ridable.m_resetCount))
            return false;
        short unlockFlag = 0;
        if (!mysql->get_short(col++, unlockFlag))                     // [0x080fddec get_short(int,short&)]
            return false;
        // ORIG：`movzwl` 零扩展 unlockFlag → `test %ax,%ax; jle`（有符号16位 ≤0→0，
        // 即 0x8000..0xFFFF 负值判 0，1..0x7fff 判 1）→ 两臂各自
        // `mov -0x10(%ebp),%eax; movb $0x1/$0x0,0x20(%eax)`。三目形会产
        // `mov $1,%eax; jmp; mov $0,%eax; mov %al,...` 物化岛（P47/P214①）⇒ if/else 直存。
        if (unlockFlag > 0)
            dbRow->m_ridable.m_field20 = 1;
        else
            dbRow->m_ridable.m_field20 = 0;

        std::memset(g_StrStageBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrSlotBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrBuyItemBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrRewardBuffer, 0, kMaxStrBuffer);

        if (!convertZipToString(mysql, col++, g_ZipStageBuffer, kMaxZipBuffer, g_StrStageBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipSlotBuffer, kMaxZipBuffer, g_StrSlotBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipBuyItemBuffer, kMaxZipBuffer, g_StrBuyItemBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipRewardBuffer, kMaxZipBuffer, g_StrRewardBuffer, kMaxStrBuffer))
            return false;

        // [b28d-solo-advancealtar_DB] P183：ORIG 每个调用点重发
        //   `mov row; imul $0x518; add &data; add $0x25` 全链（基址为 &data 非 dbRow）
        //   ⇒ 参数写 data.m_db[row].m_* 而非 dbRow->m_*。
        if (!parseStage(g_StrStageBuffer, data.m_db[row].m_stages, kMaxStageCount))
            return false;
        if (!parseSlot(g_StrSlotBuffer, data.m_db[row].m_slots, kMaxSlotCount))
            return false;
        if (!parseBuyItem(g_StrBuyItemBuffer, data.m_db[row].m_buyItems, kMaxBuyItemCount))
            return false;
        if (!parseReward(g_StrRewardBuffer, data.m_db[row].m_rewards, kMaxRewardCount))
            return false;
    }
    return true;
}

// [report SendResult.md / objdump 0x813ea46]
// [L2Free4to15.S6 本波归零] 原判 N122 撤销：ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp);
// je +0xe6` 是**正面条件包裹**形（`if (ok) { 体 } return true;`），非就地取反。
// 探针实证（4.4.7 -m32 -O0）：`bool f(bool ok){ if(ok){g();} return true; }` 生
// `mov 0x8(%ebp),%eax; mov %al,-0xc(%ebp); cmpb $0x0,-0xc(%ebp); je …` —— 与 ORIG
// 逐条同形（-0x1c 即 bool 形参 spill 槽）；而 `if (!ok) return true;` 生
// `movzbl; xor $0x1; test` 且 return 物化位置漂移（N93附）。
#include "advancealtar_DB/SendResult.inc"
{
    if (ok)
    {
        // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传（P0/P89）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1bf),
                           true);
        guard.operator*()->operator<<(uid);
        guard.operator*()->operator<<(uniqueId);
        guard.operator->()->put_binary(&data, 0x51c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);   // ORIG QUEUE_IDX=1
    }
    return true;
}

// 以下四个 parse* 共同模式：外层 strtok_r(str,"|")，内层 strtok_r(0,",")，
// 字段序按结构布局写入 [字节偏移 per 报告 asm shl 表]。

// [report parseStage.md / objdump 0x813e3e8]
#include "advancealtar_DB/parseStage.inc"
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;   // 4.4 将该零初始化提升至 strtok_r 前（P112/N82 族），源形无解
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            // [b28d-solo-advancealtar_DB] P184 死块照抄：ORIG 0x0813e462 在尾部
            //   cond 已判 field!=0 后再 `cmpl $0,-0x10(%ebp); je <+0x109 nop 岛>`。
            //   P17/P251：ORIG 0x0813e46c 为 `test %eax,%eax; je case0; cmp $1; je
            //   case1; jmp` 决策树 ⇒ 源形是 switch(f) 非 if-else 链。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_field0 = atoi(field); break;
            case 1: out[row].m_field4 = static_cast<StageField4_t>(atoi(field)); break;  // [objdump 0x813e494 → +4]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseSlot.md / objdump 0x813e542]
#include "advancealtar_DB/parseSlot.inc"
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    // [b28d-solo-advancealtar_DB] P212/P309：ORIG 槽序 saveOuter(-0x1c)/line(-0x18)/
    //   row(-0x14) 为「line 先声明」形；line 留在 for-init 时 4.4 给 row(-0x18)/
    //   line(-0x14) 互换（parseStage 同形已 AE 实证）。
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call），
            // 勿前置 `const int v = atoi(...)`（会合成单 in-loop call）。
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e696 cond 后
            //   二次 `cmpl $0,-0x10(%ebp); je <+0x176 nop 岛>`）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_byte0 = static_cast<unsigned char>(atoi(field) > 0 ? 1 : 0); break; // [0x813e47a]
            case 1: out[row].m_int1 = static_cast<SlotInt1_t>(atoi(field)); break;          // [0x813e48f +1]
            // ORIG 0x0813e655 段：`call atoi; mov %al,0x5(%ebx)` —— 直接低字节截断
            // 存储（P171 原码死行为照抄），无 bool 化三目。
            case 2: out[row].m_byte5 = static_cast<unsigned char>(atoi(field)); break;      // [0x813e4a7 +5]
            case 3: out[row].m_int6 = atoi(field); break;                                   // [0x813e4bb +6]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseBuyItem.md / objdump 0x813e708]
#include "advancealtar_DB/parseBuyItem.inc"
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call）。
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e859 cond 后
            //   二次 `cmpl $0,-0x10(%ebp); je <+0x173 nop 岛>`）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_field0 = atoi(field); break;                              // [0x813e76c]
            case 1: out[row].m_field4 = atoi(field); break;
            case 2: out[row].m_short8 = static_cast<short>(atoi(field)); break;          // [0x813e79a +8 short]
            case 3: out[row].m_short10 = static_cast<short>(atoi(field)); break;         // [0x813e7ab +10 short]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseReward.md / objdump 0x813e8cc]
#include "advancealtar_DB/parseReward.inc"
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e9d8 二次 test）+
            //   P251 switch 决策树（0x0813e9e0 `test;je case0;cmp $1;je case1`）。
            //   case1：ORIG 0x0813e9b2 段 `call atoi; test %eax,%eax; jne <store 1>;
            //   <store 0>; jmp` ⇒ if/else 双分支直存（禁具名 v + 三目：会产槽+setne）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0:
                out[row].m_field0 = atoi(field);                                     // [0x813e934]
                break;
            case 1:
                // ORIG 0x0813e9b2 段落点：`test; jne <store 1>`（!=0 跳 1），
                //   fall-through store 0 ⇒ 源码按 ==0 先行书写。
                if (atoi(field) == 0)
                    out[row].m_byte4 = false;
                else
                    out[row].m_byte4 = true;
                break;
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}


// [report dispatch.md / objdump 0x813ec24]：不调基类前置，直接取 OutBuffer → update。
#include "advancealtar_DB/dispatch_1.inc"
{
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    update(*data);
    return true;
}

#include "advancealtar_DB/makeStageString.inc"
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // [b28d-26] ORIG 0x0813f3f8 m_field4 两段中转（字段改 enum，P254）；
        //   体尾 `jmp +0xa6` + `nop@0xa5` 用 P223 双源 goto 尾标签串复现。
        if (arr[i].m_field0 == 0 && arr[i].m_field4 == 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, kDelimterField,
                     arr[i].m_field4, kDelimterItem);   // ORIG 经 DAT ","/"|"
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
    }
}

#include "advancealtar_DB/makeSlotString.inc"
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f4ef fmt="%d%s%d%s%d%s%d%s"，"," ×3(0xc/0x14/0x1c esp)+
        // "|"@0x24 为独立 rodata 实参（9 实参槽 outgoing ⇒ 帧 0xc0）。同
        // makeRewardString 配方（batch-13 帧差根因；实证装载序列 0x0813f483..0x0813f4fd，
        // 值序 byte0,int1,byte5,int6 与现 cast 一致）。
        // 【§6.2 标注】gap 5→1（o=73 n=68→72）：残余 1 = ORIG int1 装载两段
        // `mov 0x1(%eax),%eax; mov %eax,%ecx`（0x0813f4a5/4a8，4.1 先载后转），
        // 4.4 直出目标寄存器单段，寄存器分配形态差 N82，不可用 C++98 表达。
        std::sprintf(buf, "%d%s%d%s%d%s%d%s",
                     static_cast<int>(arr[i].m_byte0), ",", arr[i].m_int1, ",",
                     static_cast<int>(arr[i].m_byte5), ",", arr[i].m_int6, "|");
        out += buf;
    }
}

#include "advancealtar_DB/makeRewardString.inc"
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f6e2 fmt="%d%s%d%s"(@0x8b6db4d)，逗号/竖线为独立 rodata 串常量
        // （","@0x8b6d831 / "|"@0x8b6d82f，readelf 实证）→ 6 实参槽 outgoing 区
        // 48B，帧 0xb8；先前 "%d,%d|" 2 参形态使帧缩为 0xa8（Main 指令之缓冲
        // 16B 维度差的真实来源 = sprintf 实参集，非 buf 本体：buf 基址 -0x8c
        // 两侧本就一致）。
        if (arr[i].m_field0 <= 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, ",",
                     static_cast<int>(arr[i].m_byte4), "|");
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
        // [b28d-26] AE。旧「残余 jmp+nop 空块 N 类豁免」判词作废（P223 正例）：
        //   ORIG 0x0813f70d/0f `jmp +0xa6` + `nop@0xa5` 由双源 goto 尾标签串复现
        //   （`if(m<=0) goto L_skip; body; goto L_inc; L_skip:; L_inc:;`）。
    }
}

#include "advancealtar_DB/makeBuyItemString.inc"
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f619 fmt="%d%s%d%s%d%s%d%s"，"," ×3 + "|"（实证装载序列
        // 0x0813f5d5..0x0813f615；双 short 为 movzwl+movswl 符号扩展，与
        // static_cast<int> 同形）。同 makeRewardString 配方。
        // [b28d-26] ORIG 0x0813f644/646 `jmp +0x10a` + `nop@0x109`：P223 双源 goto
        //   尾标签串复现（条件取反形，同 makeRewardString）。
        if (arr[i].m_field4 == 0 && arr[i].m_short8 == 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s%d%s%d%s", arr[i].m_field0, ",",
                     arr[i].m_field4, ",",
                     static_cast<int>(arr[i].m_short8), ",",
                     static_cast<int>(arr[i].m_short10), "|");
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
    }
}

// [report update.md / objdump 0x813ec5e]
// [b28d-solo-advancealtar_DB] N82/P212 封顶（orig 466 / ours 469，clean resid=525 全为
//   槽号平移；帧 0xdc vs 0xbc）：
//   (1) ORIG 槽序 mysql(-0x28)/result(-0x21)/row(-0x20)/db(-0x1c)/strings(-0x2c..-0x38)；
//       4.4 块内对象恒更深（strings -0x50..-0x5c）⇒ 全帧平移，P212 全头部声明对该件
//       实测不消（strings 提头后帧仍差 0x20，且改变 string 生命周期）；
//   (2) `db.m_update == 1`：ORIG 0x0813f2xx `movzbl 0x517; xor $0x1; test; jne`（bool
//       化），4.4 恒 `cmp $0x1`；m_update 为 uchar 且 AdvanceAltarData::m_update/
//       get/reset 多消费点共用（跨函数联动），本件不改型；
//   (3) `&&` 短路链物化岛（P225 推广需拆嵌套 if；槽序未定时不单独翻）。
//   P167 CLEAR（48/48；4 处 `data()`→`c_str()` 已修正 EXTRA）、语义等价。
#include "advancealtar_DB/update.inc"
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    bool result = true;
    // [b28d-solo-advancealtar_DB] P283：ORIG 0x0813f25a `movl $0,-0x20; jmp <cond>`
    //   = for 底部条件布局（row<=0 继续）；旧 `for(;;){if(row>0)return result;}` 使
    //   4.4 条件置顶（多 cmpl/jle + jmp 岛）。
    for (int row = 0; row <= 0; ++row)
    {
        // ORIG 基址 local_20 = &data + 4（= m_db）；各列以 m_db 内具名成员寻址
        //（update.md 0x0813ec5e：`movzbl 0x517` 取 m_update、`mov 0x4/0x8/0xc/0x10/0x14`
        //  取 m_head 前 5 列、`movswl 0x18`、`movzwl 0x1a`、`movzbl 0x20`、
        //  数组指针 `add $0x21/0x1b1/0x229/0x481`）。
        // [b28d-solo-advancealtar_DB] ORIG 0x0813f25f `imul $0x518,%eax,%eax` ⇒ 行下标
        //   是变量 row（旧 m_db[0] 常量下标被折成单条 add，且丢失 imul）。
        const _AdvanceAltarDbData& db = data.m_db[row];
        // [b28d-solo-advancealtar_DB] P89/P0：m_head() 在 -O0 是真实 call 且引用占槽；
        //   ORIG 行头字段以 db+disp 直取（无 view 槽）⇒ 直接用 m_ridable 成员。
        // [L2Free4to15.S6 本波] P133/P63：ORIG 0x0813ecb6(+0x058) 与 0x0813ecce(+0x070)
        //   是两段独立取反 guard（`xor $1; test %al,%al; jne`）+ 各自 continue 中继
        //   （+0x6fa nop/jmp、+0x6fd nop，均落到 +0x6fe `addl $1`）。`a && b` 在 4.4
        //   -O0 被物化为汇合 bool（`mov $1/$0,%eax; test; je`，探针 t1 实证）⇒ 拆回
        //   两段 guard。第一段 ORIG 的 xor 形要求 m_update 为 bool —— 字段已改 bool
        //   （advancealtar.h:246，证据即 ORIG 0x0813ecb6 `movzbl 0x517; xor $1;
        //   test %al,%al; jne`），故此处按 `!` 书写以生同形；bool 值域 0/1（写点
        //   全为 0/1）⇒ 与旧「非 1 即跳」逐值等价。
        if (!db.m_update)
            continue;
        if (!isValidRidableId(db.m_ridable.m_ridableId))
            continue;
        {
            std::string stageStr;
            std::string slotStr;
            std::string buyItemStr;
            std::string rewardStr;
            // [b28d-solo-advancealtar_DB] P183：ORIG 各调用点重发 `add $0x21/0x1b1/
            //   0x229/0x481` 全链（无 stages/slots/... 具名指针槽）。
            makeStageString(stageStr, db.m_stages, 0x32);
            makeSlotString(slotStr, db.m_slots, 0xc);
            makeBuyItemString(buyItemStr, db.m_buyItems, 0x32);
            makeRewardString(rewardStr, db.m_rewards, 0x1e);

            unsigned int zipStageLen = 0, zipSlotLen = 0, zipBuyItemLen = 0, zipRewardLen = 0;
            std::memset(g_ZipStageBuffer, 0, 0x800);
            std::memset(g_ZipSlotBuffer, 0, 0x800);
            std::memset(g_ZipBuyItemBuffer, 0, 0x800);
            std::memset(g_ZipRewardBuffer, 0, 0x800);

            // [b28d-solo-advancealtar_DB] P167：ORIG 调 `_ZNKSs5c_strEv`（我方 data()
            //   是 EXTRA 发明调用点）⇒ 4 处 data() 全改 c_str()。
            if (!convertStringToZip(g_ZipStageBuffer, 0x800, stageStr.c_str(),
                                    static_cast<unsigned>(stageStr.size()), zipStageLen))
                return false;
            if (!convertStringToZip(g_ZipSlotBuffer, 0x800, slotStr.c_str(),
                                    static_cast<unsigned>(slotStr.size()), zipSlotLen))
                return false;
            if (!convertStringToZip(g_ZipBuyItemBuffer, 0x800, buyItemStr.c_str(),
                                    static_cast<unsigned>(buyItemStr.size()), zipBuyItemLen))
                return false;
            if (!convertStringToZip(g_ZipRewardBuffer, 0x800, rewardStr.c_str(),
                                    static_cast<unsigned>(rewardStr.size()), zipRewardLen))
                return false;

            char* qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
            char* qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
            char* qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
            char* qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
            mysql->set_query(
                             "update charac_advance_altar \t\t\t\t  set ticket_free = %d, ticket_cera = %d, "
                             "\t\t\t\t  star_game = %d, star_cera = %d, star_usable = %d, \t\t\t\t  "
                             "survival_best = %d, star_reset_count = %d, is_unlock_stage_effect = %d, "
                             "\t\t\t\t  stage_list = '%s', \t\t\t\t  slot_list = '%s', \t\t\t\t  "
                             "buy_item_list = '%s', \t\t\t\t  reward_list = '%s' \t\t\t\t  where "
                             "charac_no = %u and ridable_id = %d",
                             db.m_ridable.m_ticket, db.m_ridable.m_freeTicket, db.m_ridable.m_curStar,
                             db.m_ridable.m_starPoolBase,
                             db.m_ridable.m_starPoint, static_cast<int>(db.m_ridable.m_maxClearRound),
                             static_cast<int>(static_cast<unsigned short>(db.m_ridable.m_resetCount)),
                             static_cast<int>(db.m_ridable.m_field20),
                             qStage, qSlot, qBuyItem, qReward,
                             data.m_characNo, (int)db.m_ridable.m_ridableId);
            result = mysql->exec(true);
            if (!(result && mysql->getAffectedRowCount() != 0))
            {
                // [L2Free4to15.S6 本波负结论] ORIG insert 分支的 4 个 blob_to_str 结果确实
                //   落在独立槽区（0x0813f1xx `mov %eax,-0x9c(%ebp)`，区别于 update 分支的
                //   -0x80..-0x8c）；但**在 4.4 下改为分支内重新声明实测反劣**（帧 0xbc→0xcc、
                //   mysql/result/row 槽 -0x38→-0x48，离 ORIG 的 -0x28/-0x21/-0x20 更远）⇒
                //   4.4 块内对象按序压深、4.1 反之，属 N82/N115 版本槽策略，回滚保持赋值形。
                qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
                qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
                qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
                qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
                mysql->set_query(
                                 "insert into charac_advance_altar \t\t\t\t\t\t  (charac_no, ridable_id, "
                                 "\t\t\t\t\t\t  ticket_free, ticket_cera, \t\t\t\t\t\t  star_game, "
                                 "star_cera, star_usable, \t\t\t\t\t\t  survival_best, star_reset_count, "
                                 "is_unlock_stage_effect, \t\t\t\t\t\t  stage_list, \t\t\t\t\t\t  "
                                 "slot_list, \t\t\t\t\t\t  buy_item_list, \t\t\t\t\t\t  reward_list) "
                                 "\t\t\t\t\t\t  values \t\t\t\t\t\t  (%u, %d, \t\t\t\t\t\t  %d, %d, "
                                 "\t\t\t\t\t\t  %d, %d, %d,\t\t\t\t\t\t  %d, %d, %d, \t\t\t\t\t\t  '%s', "
                                 "\t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s')",
                                 data.m_characNo, (int)db.m_ridable.m_ridableId,
                                 db.m_ridable.m_ticket, db.m_ridable.m_freeTicket,
                                 db.m_ridable.m_curStar, db.m_ridable.m_starPoolBase,
                                 db.m_ridable.m_starPoint,
                                 static_cast<int>(db.m_ridable.m_maxClearRound),
                                 static_cast<int>(static_cast<unsigned short>(db.m_ridable.m_resetCount)),
                                 static_cast<int>(db.m_ridable.m_field20),
                                 qStage, qSlot, qBuyItem, qReward);
                result = mysql->exec(true);
            }
            result = true;   // ORIG movl $1（处理完成位）
        }
    }
    return result;
}

// ---- DB_StatisticAdvanceAltar ----
class DB_StatisticAdvanceAltar : public DBDispatcher
{
public:
    DB_StatisticAdvanceAltar();
    ~DB_StatisticAdvanceAltar();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
};

#include "advancealtar_DB/DB_StatisticAdvanceAltar.inc"
#include "advancealtar_DB/dtor_DB_StatisticAdvanceAltar.inc"

namespace
{
// 入场统计行视图（SigTypes.h 目前 pad(0x10)；字段化读取按报告偏移）
struct EntranceView { unsigned int stageId, entranceCount, successCount, obtainStar; };  // 0x10
struct LayerView                                        // 0xe 有效 + 2 pad = 0x10
{
    unsigned short itemType;   // +0x00
    unsigned short m_pad02;
    unsigned int itemId;       // +0x04
    unsigned short itemLevel;  // +0x08
    unsigned short useCount;   // +0x0a
    unsigned short useStar;    // +0x0c
};
}  // namespace

// [report dispatch.md / objdump 0x81412b2]
// [b28d-solo-advancealtar_DB] dead-test 族封顶（orig 312 / ours 308，clean resid=4）：
//   ORIG 两处 insert 分支 `mysql->exec(true)` 后各留 `xor $0x1,%eax; test %al,%al`
//   （4.1 对未使用 bool 返回值的归一化残留，P168a③ dead test）；4.4 tree-DCE 删除。
//   P167 CLEAR（31/31）、语义等价。
#include "advancealtar_DB/dispatch_2.inc"
{
    try
    {
        MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        struct tm now;
        __GetCurrentDNFDate(now);
        // ORIG 0x0814128c 段三条 `movl $0x0,-0x5c/-0x58/-0x54` = date[12] 预清。
        char date[12] = {0};
        std::sprintf(date, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

        int entranceCount = 0;
        *stream >> entranceCount;                                   // [0x861c5c4 operator>>(int&)]
        for (int i = 0; i < entranceCount; ++i)
        {
            // [b28d-solo-advancealtar_DB] P89：ORIG 单槽 -0x28（GetOutBuffer 结果
            //   即视图指针），旧 rawE+e 双局部多一槽并致全帧平移 8。
            EntranceView* e = reinterpret_cast<EntranceView*>(
                stream->GetOutBuffer<STAdvanceAltarEntranceData>());
            if (e == 0)
                throw 0x624;
            // P118/P139：ORIG 0x081412d5 `movl $<rodata>,0x4(%esp)` 直接传函数签名串
            //   （_ZZ…E19__PRETTY_FUNCTION__），非 static 指针变量（多一条装载）。
            cMyTrace(__PRETTY_FUNCTION__, 0x626, 0)("[AdvanceAltar] %u,%u,%u,%u",
                                                     e->stageId, e->entranceCount, e->successCount, e->obtainStar);
            mysql->set_query(
                             " upDate log_advance_alter_entrance  set entrance_count = entrance_count + %u"
                             "  , success_count = success_count + %u  , obtain_star = obtain_star + %u"
                             "  where occ_date = '%s' and stage_id = %u",
                             e->entranceCount, e->successCount, e->obtainStar, date, e->stageId);
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_entrance(occ_date,  stage_id,"
                                 " entrance_count, success_count, obtain_star)"
                                 "  values('%s', %u, %u, %u, %u) ",
                                 date, e->stageId, e->entranceCount, e->successCount, e->obtainStar);
                mysql->exec(true);
            }
        }

        int layerCount = 0;
        *stream >> layerCount;
        for (int i = 0; i < layerCount; ++i)
        {
            LayerView* l = reinterpret_cast<LayerView*>(
                stream->GetOutBuffer<STAdvanceAltarLayerStatData>());
            if (l == 0)
                throw 0x64d;
            cMyTrace(__PRETTY_FUNCTION__, 0x64f, 0)("[AdvanceAltar] %u,%u,%u,%u,%u",
                                                     static_cast<unsigned>(l->itemType), l->itemId,
                                                     static_cast<unsigned>(l->itemLevel),
                                                     static_cast<unsigned>(l->useCount),
                                                     static_cast<unsigned>(l->useStar));
            mysql->set_query(
                             " upDate log_advance_alter_layer_stat  set use_count = use_count + %u,"
                             " use_star = use_star + %u  where occ_date = '%s' and item_type = %u and"
                             " item_id = %u and item_level = %u",
                             static_cast<unsigned>(l->useCount), static_cast<unsigned>(l->useStar),
                             date, static_cast<unsigned>(l->itemType), l->itemId,
                             static_cast<unsigned>(l->itemLevel));
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_layer_stat(occ_date,  item_type,"
                                 " item_id, item_level, use_count, use_star)"
                                 " values('%s', %u, %u, %u, %u, %u)",
                                 date, static_cast<unsigned>(l->itemType), l->itemId,
                                 static_cast<unsigned>(l->itemLevel),
                                 static_cast<unsigned>(l->useCount),
                                 static_cast<unsigned>(l->useStar));
                mysql->exec(true);
            }
        }
        return true;
    }
    catch (int err)
    {
        LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp", __PRETTY_FUNCTION__,
                              0x674, "[AdvanceAltar] query error line(%u)", err);   // [0x814174e]
        return false;
    }
}

// ---- 压缩转换自由函数 ----

// [objdump 0x813dd3e]
#include "advancealtar_DB/convertStringToZip_1.inc"
                        unsigned int srcLen, unsigned int& realLen)
{
    if (srcLen == 0)
    {
        realLen = 0;
        return true;
    }
    realLen = outCap;                       // ORIG 0x813dd5d：写入 outCap（容量占位）
    std::memset(out, 0, outCap);
    // [b28d-solo-advancealtar_DB] P0/N96 清绑：原 `register unsigned char c asm("eax")`
    //   为禁用形（-O0 局部恒占栈槽、寄存器仅承载表达式值）。P168a①：callee 返回
    //   bool 时裸负形 `if (!call()) return false;` 直产 ORIG 0x0813dd5d 的
    //   `call; xor $0x1,%eax; test %al,%al; je`（零扩展+取反全在 eax）。
    if (!compress_zip(out, realLen, src, srcLen))
        return false;
    return true;
}

} // namespace advancealtar
