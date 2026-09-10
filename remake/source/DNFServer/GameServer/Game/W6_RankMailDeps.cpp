// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_RankMailDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W6_RankMailDeps.cpp — Wave6 清零 only_orig（WongWork 排行榜 / 邮箱叶子）
//
// 覆盖（ORIG nm -S 权威地址；报告 docs/class_func_reports/ 相应目录）：
//   A) WongWork::CDeathTowerRanking：
//        registRanker / resetRanking / registRanking / updateRanking /
//        stBestRecord_t::operator> / operator==（头内 inline 去 const 后由
//        实例化点产生）/ _getRankingTable / registBestRecord /
//        _getBestRecordTable
//   B) WongWork::CDeathTower::CDungeonMgr：getStandardLevel /
//        getObjectDropItems / getSpecialPassiveObjectItems（三 getter 均转发
//        CDungeon 同名函数；报告目录无独立 md，objdump 0x08469ac0/0x08469ad6/
//        0x08469aec 补证）
//   C) WongWork::CMailBox：stAddNewMailInput C1/C2、DecLoadedLetterCount、
//        DeleteLetterKeepCount、IncNotLoadedMailCount、CMail::Init、
//        CMailBox::getMailLoadCount(const)、CMail::getMailIdx const、
//        CMail::_398 C1/C2（GCC 局部别名类构造：this+0x4 处 Inven_Item 构造）
// ============================================================================
#include "CDeathTowerRanking.h"
#include "CMailBoxHelper.h"

#include <string.h>
#include <new>

#include "Inven_Item.h"          // CMail 成员 / placement ctor
#include "CDungeon.h"            // get_standard_level/getObjectDropItems/getSpecialPassiveObjectItems
#include "CDeathTower.h"         // WongWork::CDeathTower::CDungeonMgr

// ============================================================================
// A) WongWork::CDeathTowerRanking
// ============================================================================

namespace {

// （原 rankSlotBase 无类型字节寻址 helper 已删除——SecuLink 批次 2026-09-05
//  裸偏移清理：塔块 6×0x98 连续 stBestRecord_t 槽证据见 CDeathTowerRanking.h
//  stTowerBlock 注释；registRanker 内改用具类型 stride 寻址 + 具名字段。）

} // namespace

// ORIG T 0x08468614 (size 0x300) —— registRanker.md
// [b28d-solo-W6_RankMailDeps] DIFF(N) — 修法（ndiff 216→~125，insn 170→229/ORIG 224，gap 5）：
//   ① slot 指针缓存与 oldStage/oldTime 具名局部全删（P183），每处槽访问重发完整
//      二维链；② 槽寻址直用真二维下标 `m_towers[towerIdx].m_slots[slotIdx]`
//      （Main 代落头 m_slots[6] 后）复现 ORIG 双 imul 形
//      （探针 /tmp/b28d_w6_probe3.cpp f_2d 逐条同形；此前 TU-local 视图因 cast
//      多 this 中转 236→229）；③ byte lane 改
//      `uchar(*)[0x17]` 数组引用形（探针 probe4 l2：7→6 条/lane）；
//      ④ insert 保持 P178 双层临时链（make_pair+pair C1IjjE/C1IjS3_）。
//      P167 CLEAR(8/8)。残余 gap 5 = 结构性 N 族：a) 3 处 byte lane 源读 4.4
//      恒先载 members 寄存器 + lea（ORIG `add 0x14(%ebp),%eax` 内存操作数，
//      N4/P30，探针 probe4 l2）；b) m_playTime store LHS-first+`mov %eax,%edx`
//      中转 vs ORIG RHS-first（N76/P211）；c) erase 段 `add %edx,%eax` vs
//      ORIG `add 0x8(%ebp),%eax`（this 装载序，N7/N18a）。另槽序
//      -0x1c/-0x20/-0x28… vs ORIG -0x1c/-0x24/-0x2c…（N82/N98）。语义与调用点
//      全等（14 处槽访问、3 处 insert/erase 调用点逐条同形）。ORIG 0x08468614。
#include "W6_RankMailDeps/registRanker.inc"
#define W6_RANKER_SLOT(idx) (m_towers[towerIdx][idx])   // [b28d-Main·检查点14] 二维化
        // asm +0x064..+0x0a3: 槽已占位 → 先按旧 stage 行、旧时间值 erase 排名
        // 记录（读取发生在改写之前；ORIG 直传 &slot->m_playTime 引用，
        // 无 oldStage/oldTime 局部槽）。
        if (W6_RANKER_SLOT(slotIdx).m_flag90 != 0)
        {
            m_rankMap[towerIdx][W6_RANKER_SLOT(slotIdx).m_stage]
                .erase(W6_RANKER_SLOT(slotIdx).m_playTime);
        }
        // asm +0x0a8..+0x1a9: for i < m_tableType: strncpy(名字,0x1e) +
        // 三 byte lane ← members[i*0x17 + 0x14/15/16]。
        // members 按 ORIG 字节读（stDeathTowerRecordMemberInfo_t 未建模字段，
        // +0x14/15/16 三 byte 无具名证据，保留 stride+偏移形式）。
        for (unsigned int i = 0; i < m_tableType; ++i)
        {
            strncpy(W6_RANKER_SLOT(slotIdx).m_memberData[i],
                    reinterpret_cast<const char*>(members) + i * 0x17, 0x1e);
            // [b28d-solo-W6_RankMailDeps] 数组引用形（`uchar(*)[0x17]` 二维
            //   下标）复现 ORIG `movzbl 0x14(%eax)` disp 读形：探针
            //   /tmp/b28d_w6_probe4.cpp l2 实测 5 条 vs 指针下标 l1 的 7 条
            //   （P30/N4 族，残余 1 条 members 载入差）。
            W6_RANKER_SLOT(slotIdx).m_pad7c[0][i] =
                reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x14];
            W6_RANKER_SLOT(slotIdx).m_pad7c[1][i] =
                reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x15];
            W6_RANKER_SLOT(slotIdx).m_pad7c[2][i] =
                reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x16];
        }
        // asm +0x1af..+0x21b: 写新 stageVal/playTime 并置“有纪录”位=1。
        W6_RANKER_SLOT(slotIdx).m_stage = stageVal;
        W6_RANKER_SLOT(slotIdx).m_playTime = playTime;
        W6_RANKER_SLOT(slotIdx).m_flag90 = 1;
#undef W6_RANKER_SLOT
    }
    // asm +0x21b..+0x2ae: 无条件——m_rankMap[towerIdx][stageVal].insert(
    //   pair(playTime→slotIdx))；键已存在则改写值为 slotIdx（ORIG fresh 插入与
    //   dup 改写均收敛为同一语义对）。
    RankMap& row = m_rankMap[towerIdx][stageVal];
    std::pair<RankMap::iterator, bool> r =
        // b28d-55（P178/P86）：ORIG 双层临时链 make_pair → value_type 右值构造
        // （P167 差集：make_pair<unsigned&,unsigned&> + pair<const unsigned,unsigned>
        //  ::C1IjjE）；4.4 对 value_type(a,b) 直构折叠掉 make_pair。
        row.insert(RankMap::value_type(std::make_pair(playTime, slotIdx)));
    if (!r.second)
        r.first->second = slotIdx;
    // asm +0x2b4..+0x2f5: 表级最优纪录比较，更优则整块拷贝（rep movsl 0x26*4）。
    stBestRecord_t cur;
    cur.m_stage = stageVal;
    cur.m_playTime = playTime;
    if (m_tableBest > cur)
        m_tableBest = cur;   // P83：ORIG rep movsl 0x26 dword = 结构整体赋值
}

// ORIG T 0x08468594 (size 0x80) —— resetRanking.md
// [b28d-solo-W6_RankMailDeps] AE（compare_tu clean，DIFF→AE；旧 b28d-55
//   「P209 附无源码解」判词作废）— 修法：内层局部改 int + 显式
//   `(unsigned int)s < 200` 比较，复现 ORIG 两段寄存器尾测
//   `mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe; test; jne`（探针
//   /tmp/b28d_w6_probe5.cpp f2 逐条同形；unsigned 局部恒折 cmpl mem 单段）。
//   语义不变（0..199）。ORIG 0x08468594。
#include "W6_RankMailDeps/resetRanking.inc"
}

// ORIG T 0x084683da (size 0xf3) —— registRanking.md
// [b28d-55] DIFF(N) — 修法：value_type(make_pair(key,rank)) 复现 ORIG
//   make_pair<unsigned&,unsigned&> + pair<const unsigned,unsigned>::C1IjjE
//   （P178/P86），P167 CLEAR(10/10)。残余 = 寄存器/槽位差（N82 族）。
//   ORIG 0x084683da。
#include "W6_RankMailDeps/registRanking.inc"
}

// ORIG T 0x084684ce (size 0x83) —— updateRanking.md
#include "W6_RankMailDeps/updateRanking.inc"
}

// ORIG T 0x08468914 (size 0x238) —— registBestRecord.md
// [b28d-solo-W6_RankMailDeps] DIFF(N) — 修法（ndiff 133→~93，insn 152→153/ORIG 150）：
//   ① 删 src 指针缓存（P183），每 lane 重发 `imul $0x17 + add members` 链；
//   ② byte lane 改 `uchar(*)[0x17]` 数组引用形复现 ORIG `movzbl 0x14(%edx)`
//      disp 读（探针 /tmp/b28d_w6_probe4.cpp l2）；③ insert 保持 P178
//      value_type(make_pair(key,rec)) 双层临时链（pair C1IjS3_）。P167 CLEAR(14/14)。
//   残余 = 结构性 N 族：a) 每 lane 多 1 条 `mov 0x14(%ebp),%ecx` + lea（ORIG
//      `add 0x14(%ebp),%edx` 内存操作数，N4/P30 基址恒先物化，探针 l2 实测）；
//   b) 局部槽序 i/rt/bt = -0x20/-0x1c/-0x24 vs ORIG -0x1c/-0x24/-0x20
//      （N82/N98 首次使用序分配，声明序不可翻）；c) cur 槽 -0xd0 vs ORIG -0xcc
//      连带 stage/playTime -0x48/-0x44 vs -0x44/-0x40（同族）。语义全等。
//      ORIG 0x08468914。
#include "W6_RankMailDeps/registBestRecord.inc"
}

// ORIG W 0x08469d90 (size 0x1e) —— objdump 补证（无独立 md）：
// return this + 0x11d0 + idx*0x14 == &m_rankingTables[idx]（stride 已验证）。
WongWork::CDeathTowerRanking::RankTable*
#include "W6_RankMailDeps/_getRankingTable.inc"
}

// ORIG W 0x08469dae (size 0x1e) —— 同构，基址 0x1234 == m_bestTables。
WongWork::CDeathTowerRanking::BestMap*
#include "W6_RankMailDeps/_getBestRecordTable.inc"
}

// ============================================================================
// B) WongWork::CDeathTower::CDungeonMgr — 三 getter（转发 CDungeon）
// ============================================================================

#include "W6_RankMailDeps/getStandardLevel.inc"
}

std::vector<std::pair<int, int> >*
#include "W6_RankMailDeps/getObjectDropItems.inc"
}

std::vector<stDungeonAssignItem_t>*
#include "W6_RankMailDeps/getSpecialPassiveObjectItems.inc"
}

// ============================================================================
// C) WongWork::CMailBox / CMail
// ============================================================================

// ORIG W 0x085578ae (size 0x65)：前 0x28 字节清零（7 dword + byte@0x1c +
// dword@0x20/0x24）。语义映射（AddNewMail.md 装载证据）：+0x00 mailIndex、
// +0x04 senderCharacNo、+0x08 packageMoney/senderNamePtr（CMailBox.cpp 与
// CMailBoxHelper.cpp 两 TU 对该槽/后段视图分歧，见 NOTES 上报）、+0x0c letterId、
// +0x10 remainSec、+0x14 packageType、+0x18 period、+0x1c unlimit(byte)、
// +0x20/+0x24 指针字段。
#include "W6_RankMailDeps/stAddNewMailInput.inc"
}

// ORIG W 0x084ed2fc (size 0x34)：m_loadedLetterCount-1；负数归零。
#include "W6_RankMailDeps/DecLoadedLetterCount.inc"
}

// ORIG W 0x084ed280 (size 0x7c)：letterKeepCount erase；命中返回 1 否则 0。
#include "W6_RankMailDeps/DeleteLetterKeepCount.inc"
}

// ORIG W 0x084ed330 (size 0x14)：m_notLoadedMailCount+1。
#include "W6_RankMailDeps/IncNotLoadedMailCount.inc"
}

// ORIG W 0x084ed216 (size 0xb)：return this+0xc 的 int（现头具名字段
// m_mailLoadCount，原 m_pad0c 区）。
#include "W6_RankMailDeps/getMailLoadCount.inc"
}

// ORIG T 0x085519e2 (size 0x1c)：m_mailIndex=0 后调 ResetPackage()。
#include "W6_RankMailDeps/Init.inc"
}

// ORIG W 0x0869782e (size 0xa)：return m_mailIndex。
#include "W6_RankMailDeps/getMailIdx.inc"
}

// CMail::._398 子对象 ctor 现由 CMailBoxHelper.h 的匿名 m_package 子对象声明确认，
// 编译器自动生成（原先的 asm label 桥违反 §4.5，已移除）。
