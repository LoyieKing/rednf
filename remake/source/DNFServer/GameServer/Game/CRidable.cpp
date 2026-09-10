// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CRidable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CRidable 专属 TU（从 GameStubs.cpp 拆出）。
// 语义依据 ORIG CheckAppearRidableObject 0x82fe9c8 / SetRidableObjectState
// 0x82feb5e / SendSpawnRidableObject 0x82fece2 / Clear 0x82fe6bc。
#include "CRidable.h"
#include "PacketBuf.h"
#include "PacketGuard.h"

// ORIG ctor 0x82a4406：默认构造 map<int,DungeonRidableObject>@+0x00；
// dtor 0x82a441a：析构该 map。
// ORIG 0x82a4406：仅 map C1（m_flag18 未初始化，字节一致优先）。
#include "CRidable/CRidable.inc"
#include "CRidable/dtor_CRidable.inc"

// ORIG Clear 0x82fe6bc：直接调 m_objects.clear()。
#include "CRidable/Clear.inc"
{
    m_objects.clear();
}

#include "CRidable/SendSpawnRidableObject.inc"
{
    // ORIG 0x82fece2：连续写 4 个 int。
    p->put_int(a);
    p->put_int(b);
    p->put_int(c);
    p->put_int(d);
}

#include "CRidable/SetRidableObjectState.inc"
{
    // ORIG 0x082feb5e 逐条：find == end → return 0；条件 (m_state==0||==4||==3||>4)
    // 成立 → `test %al,%al; je 赋值段; mov $0,%eax` **return 0**（真语义 bug：旧代码
    // 注释写"直接返回 true"并把 return true 放在条件外，返回码反了，P249 同族第 9 例）；
    // 条件不成立 → `m_state = state; mov $1,%eax` return 1。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.find(idx);
    if (it != m_objects.end())
    {
        if (it->second.m_state == 0 || it->second.m_state == 4 ||
            it->second.m_state == 3 || it->second.m_state > 4)
            return false;
        it->second.m_state = (int)state;
        return true;
    }
    // ORIG 0x082fec25：it==end 的 return 0 落在函数末尾独立块（cond 成立的
    // return 0 在 0x082fec06 中间块）⇒ 用末尾 return 共享该块。
    return false;
}
// ORIG 0x082fec2c：遍历对象表，依次 push (m_x, m_y)。
// ORIG 汇编为 pair<int,int>::pair<int&,int&> 模板构造（左值引用实例，非
// make_pair 值传递），故此处用局部左值 + 直接 pair 构造对齐符号集。
#include "CRidable/GetObjectPos.inc"
{
    // b28d-40 的「N82 封顶」判词作废：p167 实证 ORIG 侧全为**非 const**
    //   `_ZNSt17_Rb_tree_iteratorI…E{ptEv,ppEv,ne}`（我方 const_iterator 额外引入
    //   2×转换构造 C1ERKS_ +2 条）⇒ 换非 const 迭代器（P96/P270）。
    // 另：ORIG 0x082fec54..0x082fec75 六条 `mov N(%eax),%edx` 是 `DungeonRidableObject
    //   obj = it->second;` 的整体拷贝（-0x30..-0x1c），随后 pair 实参取
    //   `&obj.m_x`/`&obj.m_y`（`lea -0x30` + `add $0x4`）⇒ 恢复整体拷贝形。
    for (std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
         it != m_objects.end(); ++it)
    {
        // ORIG 0x082fec54..0x082fec75 只拷 6 个 dword（源 +0x4..+0x18 = value 的
        //   +0x00..+0x14）⇒ 原码按**基类切片**拷贝 `RidableObject obj = it->second;`
        //   （派生 m_state@+0x18 不拷；我们写 DungeonRidableObject 会多 1 个 dword）。
        RidableObject obj = it->second;
        out.push_back(std::pair<int, int>(obj.m_x, obj.m_y));
    }
}
// ORIG 0x82fe9c8：遍历 m_objects；坐标 (m_x,m_y)==(a,b) 且 state==4 或
// (state==3 && m_flag18) 的对象置 state=1、计数并发送生成包；循环后
// p->put_byte(idx, 计数) 收尾。party 形参 ORIG 未使用（保留签名）。
// 成员偏移按 CRidable.h：pair.second 起 +4 → m_x@+4/m_y@+8/m_field0c@+0xc/
// m_field10@+0x10/m_field14@+0x14/m_field18@+0x18/m_state@+0x1c。
// [A28 round5 定稿] 本轮修：①`begin()` 提到 counter 之前（ORIG 0x082fe9d1..de
//   先 begin → counter(-0x1c) → idx(-0x28) → put_byte）；②`p->put_byte(counter)`
//   （ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）。
//   残余 1 条：ORIG `mov 0x18(%eax),%eax; mov %eax,%edi` vs 4.4 折单段
//   `mov 0x18(%eax),%edi`（%edi 直落）⇒ N18a（4.4 单段直落不可造；P254 enum 化
//   需改 CDungeon.h 的 RidableObject（多 TU 共享）且无 ORIG enum 证据，不改）。
//   [A28 round5 · C07 命中形实测 2026-09-10 19:48] 按 Main 转发的「字段读升格
//   callee-saved ⇒ 写具名 int 局部」试一次旋钮：**反劣**（帧 0x3c→0x40、丢
//   `push %edi`、迭代器槽 -0x24→-0x18 整体平移）⇒ 已回滚，N18a 维持（一次旋钮
//   纪律：无改善即回滚封顶）。
//   与 A14 `CPrivateStore::BuyItem` 反例同构：本件的字段读消费者是**被调方的按值
//   实参**（SendSpawnRidableObject 第 5 参），非「call 后仍被本函数复用」⇒ 具名
//   局部必然落槽 +2，符合 C07 命中形的否定边界。
#include "CRidable/CheckAppearRidableObject.inc"
{
    // ORIG 0x082fe9d1 段序（P28/P50「先声明后赋值」）：begin() 最先
    //   （0x082fe9d1..de，it 落 -0x24）→ counter(-0x1c)=0 → idx(-0x28)=get_index()
    //   → `put_byte(counter)`（ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）
    //   → jmp 底部条件。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
    int counter = 0;
    int idx = p->get_index();
    p->put_byte(counter);
    for (; it != m_objects.end(); ++it)
    {
        if (it->second.m_x == a && it->second.m_y == b)
        {
            if (it->second.m_state == 4 ||
                (it->second.m_state == 3 && m_flag18 != 0))
            {
                it->second.m_state = 1;
                ++counter;
                SendSpawnRidableObject(p, it->second.m_field10,
                                       it->second.m_field14,
                                       it->second.m_field0c,
                                       it->second.m_field18);
            }
        }
    }
    p->put_byte(idx, counter);
}
