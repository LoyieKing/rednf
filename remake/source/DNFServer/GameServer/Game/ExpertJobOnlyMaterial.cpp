#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobOnlyMaterial/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <utility>
#include <vector>

#include "CDataManager.h"
#include "CExpertJob.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CUser.h"
#include "GameTypes.h"     // cMyTrace
#include "PacketGuard.h"   // PacketGuard : InterfacePacketBuf

namespace expert_job
{

// ---------------------------------------------------------------------------
// ExpertJobMaterial 成员
// ---------------------------------------------------------------------------

#include "ExpertJobOnlyMaterial/Empty.inc"


#include "ExpertJobOnlyMaterial/SetData.inc"


#include "ExpertJobOnlyMaterial/IsRecipe.inc"


#include "ExpertJobOnlyMaterial/CheckType.inc"


#include "ExpertJobOnlyMaterial/IsRight.inc"


#include "ExpertJobOnlyMaterial/FindMonsterCard.inc"


#include "ExpertJobOnlyMaterial/GetEnchantItem.inc"


#include "ExpertJobOnlyMaterial/IsExistMaterialOfRecipe.inc"


#include "ExpertJobOnlyMaterial/DelMaterialOfRecipe.inc"


#include "ExpertJobOnlyMaterial/UseMaterialOfRecipe.inc"


// ---------------------------------------------------------------------------
// 自由函数
// ---------------------------------------------------------------------------

// ORIG _ZN10expert_job15FindMonsterCardEP5CItem @0x0849be32。
// b28d-solo-ExpertJobOnlyMaterial: AE — 匿名 pair 值临时取 first（双段装载）
// + P296 嵌套正向包裹消 continue 岛；P49 逐点内联 m_recipe174.m_vec0。
CItem* FindMonsterCard(CItem* recipe)
{
    // b28d-36（P49）：ORIG 0x0849be32 内联 `lea 0x174(%eax),%edx`（helper 被
    // 4.1 前端内联）；4.4 -O0 发真实 call（DIFF+missing）⇒ 逐点内联成员访问。
    for (std::vector<std::pair<int, int> >::const_iterator it =
             reinterpret_cast<CStackableItem*>(recipe)
                 ->m_recipe174.m_vec0.begin();
         it != reinterpret_cast<CStackableItem*>(recipe)
                   ->m_recipe174.m_vec0.end();
         ++it)
    {
        CStackableItem* found = static_cast<CStackableItem*>(
            G_CDataManager()->find_item(std::pair<int, int>(*it).first));
        // b28d-solo-ExpertJobOnlyMaterial（P296）：嵌套正向包裹消 continue 岛
        // （ORIG 0x0849be92/0x0849bea1/0x0849beb6 三处 je 直落 0x0849bebd）。
        if (found != 0)
        {
            if (found->is_stackable())
            {
                if (found->get_sub_type() == 1)
                    return found;
            }
        }
    }
    return 0;
}

#include "ExpertJobOnlyMaterial/GetRecipeInputItems.inc"


#include "ExpertJobOnlyMaterial/IsExistRecipeItem.inc"


#include "ExpertJobOnlyMaterial/DelMaterialOfRecipe_1.inc"


#include "ExpertJobOnlyMaterial/UseMaterialOfRecipe_1.inc"


}  // namespace expert_job
