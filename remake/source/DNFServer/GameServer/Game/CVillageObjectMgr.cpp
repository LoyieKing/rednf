// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CVillageObjectMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CVillageObjectMgr.cpp
#include "CVillageObjectMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"
#include "PacketBuf.h"   // put_header/get_index/put_byte/finalize

namespace village_object
{

// ORIG 0x86b6f30：仅默认构造成员 std::map<Zone, list<IObject*>> m_objects。
#include "CVillageObjectMgr/CVillageObjectMgr.inc"
    : m_objects()
{
}

#include "CVillageObjectMgr/Zone.inc"
    : m_a(a)
    , m_b(b)
{
}

// ORIG 0x86b6f44：调用 std::map<...>::~map（成员自动析构）。
#include "CVillageObjectMgr/dtor_CVillageObjectMgr.inc"
{
}

#include "CVillageObjectMgr/on_move_area.inc"
{
    // [M5E round6 判据 ① 已改 · 旧判词 N82/N93 被推翻] ORIG 0x086b71da 直证：
    //  · +0x095 ptEv 后 `add $0x4,%eax; mov %eax,-0x14(%ebp)`（具名 `list* pList
    //    = &it->second;`），begin/end 由 +0x0f6/+0x11d `mov -0x14(%ebp),%edx`
    //    取用 ⇒ 旧码 `it->second.begin()` 内联 ptEv 两次；
    //  · -0x10 = count 具名（`movl $0x0,-0x10` @+0x0da），+0x0e1 `mov -0x10(%ebp),%eax`
    //    ⇒ ORIG 的占位写下的是**变量 count** 而非字面 0（旧码 `put_byte(0)`）；
    //  · -0xc = obj 具名且声明即 0（`movl $0x0,-0xc` @+0x141）：先 `obj = *iter;`
    //    再 `if (!obj->put_object_info(packet)) continue;`（+0x173 `xor $0x1,%eax`
    //    ⇒ 取反形态），旧码 `if (...put...) count += 1;`；
    //  · +0x0f3/+0x11a begin/end 各自先出裸 iterator（-0x1c/-0x18）再转成
    //    `_List_const_iterator`（-0x3c/-0x40）⇒ ORIG 用两个具名 const_iterator；
    //  · get_index 结果 -0x38 **无前置零初始化**（旧码 `int countIdx = 0;` 多余 1 条）；
    //  · 帧 `sub $0x50,%esp`（旧码 0x40）⇒ 补 3 个具名局部后同 ORIG。
    // [M5E round6 复验] 改后 157/157、字节长 0x215=0x215，DIFF→NEAR；残差仅 1 条
    //   寄存器对：循环体内虚调用 `obj->put_object_info(packet)` 的调用目标
    //   ORIG 落 %edx（+0x162 `mov (%eax),%edx`）而我方落 %eax（+0x162
    //   `mov (%eax),%eax`），实参随之 %eax/%edx 互换（各 5 条、同长）。属 4.4
    //   local-alloc 伪号序不可控（P-batch 无对应旋钮，已试形 B/C 同形）⇒ 封顶 NEAR。
    // ORIG 0x86b71da：Zone(curVill, curArea) 查找，命中则向该玩家发送 0xca 视图包。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
        return;

    std::list<IObject*>* pList = &it->second;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xca);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(count);
    std::list<IObject*>::const_iterator iter = pList->begin();
    std::list<IObject*>::const_iterator iterEnd = pList->end();
    IObject* obj = 0;
    for (; iter != iterEnd; ++iter)
    {
        obj = *iter;
        if (!obj->put_object_info(packet))
            continue;
        count += 1;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
}

#include "CVillageObjectMgr/register_object.inc"
{
    // [M5E round6 判据 ①/② 已改] ORIG 0x086b6f58 三条直证：
    //  · 极性：+0x046 `call _ZNKSt17_Rb_tree_iteratorI...EneERKSA_`（ne）+ `je` 落
    //    插入分支 ⇒ ORIG 源码是**正极性** `if (it != m_objects.end())`（旧码 eqERKSA_
    //    + 交换分支）。
    //  · 具名列表指针：+0x055 ptEv 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)`
    //    （`list* pList = &it->second;`），随后 +0x067/+0x087 两次
    //    `mov -0xc(%ebp),%eax; mov %eax,(%esp)` ⇒ 全程只 1 次 ptEv（旧码 2 次）。
    //  · 帧：ORIG `sub $0x50,%esp`（旧码 0x40，少 1 个 4B 具名局部 ⇒ 差 0x10）。
    // ORIG 0x86b6f58：无该 Zone 则新建列表插入，否则先 detect_collision 碰撞
    // 检测（命中返 0），通过才 push_back；成功返 1。
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it != m_objects.end())
    {
        std::list<IObject*>* pList = &it->second;
        if (collision_detection::detect_collision(*pList, object))
            return 0;
        pList->push_back(object);
    }
    else
    {
        std::list<IObject*> newList;
        newList.push_back(object);
        m_objects.insert(std::make_pair(zone, newList));
    }
    return 1;
}

}  // namespace village_object
