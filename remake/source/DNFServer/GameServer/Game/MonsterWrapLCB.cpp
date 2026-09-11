// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MonsterWrapLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "MonsterIndependentDrop.h"

#include "CItemList.h"   // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMonster.h"   // CMonster::getMonsterScript / STMonsterScript 完整类型

// 全局生成函数自由声明（此前仅 mangled 符号存在于 ORIG；本 TU 为权威 owner）
#include "MonsterWrapLCB/generateDropAvatarItem.inc"
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out);   // ORIG 0x08349da0
#include "MonsterWrapLCB/generateCatchMonsterItem.inc"
                              std::vector<Inven_Item>& out);  // ORIG 0x08349f4b

// ----------------------------------------------------------------------------
// ORIG W 0x08539de8 _ZNK12CAICharacter13IsHellMonsterEv（0xc）：
//   movzbl 0x70(this) 直返。mirror 类 +0x30..+0x6b 为 m_dropAvatarInfo，
//   +0x70 rarity 标志未具名建模，按字节视图读取。
// 返回类型 bool：ORIG 体单条 `movzx eax,byte`（无 0/1 规范化 ⇒ 视图成员本身
// bool），调用点 0x08535b08 `call;test %al,%al;je` 无 setne（P292）。
#include "MonsterWrapLCB/IsHellMonster.inc"
{
    // [b28c-35] ORIG 0x08539de8 为单条 `movzbl 0x70(%eax),%eax`；裸偏移算术链
    // （reinterpret_cast + 0x70）4.4 发 `add $0x70,%eax; movzbl (%eax)`（+1 条）。
    // 改具名尾部视图成员访问（P82）即折叠单条 disp；视图只读该字节，不改类布局。
    struct Tail { char m_pad[0x70]; bool m_flag; };
    return reinterpret_cast<const Tail*>(this)->m_flag;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834a15e _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   lea 0x30(this) 取 m_dropAvatarInfo 后转发自由函数。
#include "MonsterWrapLCB/generateDropAvatarItem_1.inc"
                                          std::vector<Inven_Item>& out) const
{
    ::generateDropAvatarItem(m_dropAvatarInfo, rarity, grade, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8a6 _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0xcf8 —— 即 m_script(+0x60) 内
//   m_dropAvatarInfo(+0xc98) 子对象，具名访问后转发自由函数。
#include "MonsterWrapLCB/generateDropAvatarItem_2.inc"
                                      std::vector<Inven_Item>& out) const
{
    // ORIG 无 call：this+0x60+0xc98=+0xcf8 直算（成员直访折叠，无 script 槽）。
    ::generateDropAvatarItem(m_script.m_dropAvatarInfo, rarity, grade, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8e8 _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0x70(script) 取 m_catchItems 转发。
#include "MonsterWrapLCB/generateCatchMonsterItem_1.inc"
{
    // P134：ORIG call 后结果寄存器直用（lea 0x70(%eax)），无 script 局部槽。
    ::generateCatchMonsterItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_catchItems, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349da0 _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
// （size 0x1ab）头像随机掉落主逻辑：
//   ① 入门阈值：threshold = (int)((double)m_field00 * T_A[(u8)rarity]
//                                 * T_B[(u8)grade - 1])；
//     T_A 基址 info+0x18（flds 0x8(%eax,%edx,4)，edx=(u8)rarity+4）、T_B 基址
//     info+0x8、下标 (u8)grade-1 —— 两片均为 clear 清零区浮点表的重叠视图，
//     按指针视图逐指令转写；fistpll 截断取低 dword。
//     get_rand_int(100000) > threshold 时直接返回。
//   ② 共 rounds = *(unsigned*)(info+0x04)（无符号 seta 比较）轮：
//       每轮 r = get_rand_int(*(int*)(info+0x2c))；
//       自 m_elements.begin() 重新遍历：首个 elem.m_field04 > r（无符号）时，
//       Inven_Item{}；item.m_addInfo(ORIG 写 +0x02 视图) ← elem.m_field00；
//       G_CDataManager()->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)1,
//       item, 1)；out.push_back(item)；命中后不推进迭代器即退出本轮
//       （ORIG jmp +0x170 → 轮数 +1 继续）。
// [b28d-L14 N18a] ORIG 0x08598f34；p167 缺 1 次 const_iterator ptEv；残差=槽深/寄存器。
// [F2e-Singles2 C05② 2026-09-10] 旋钮①删 rounds 具名局部（P0：ORIG 尾测每轮重读
//   `mov 0x8(%ebp),%eax; mov 0x4(%eax),%eax; cmp -0x10(%ebp),%eax; seta`，而我方缓存
//   rounds 于 -0x14 并折 `setb`）⇒ 尾测与 threshold 槽(-0x18→-0x14)一次对齐 ORIG；
//   旋钮②base/tableA/tableB 引用内联 + longlong 整数域（fildll/fistpll）⇒ thresholdLL。
//   实测 our 129→121（ORIG 122，gap **−7→−1**），TU 四数 27/38/0/2 两态逐位一致。
//   残余 = 4.4 寻址折叠（我方 `add $0x6; shl $0x2; add; flds (%eax)` vs ORIG
//   `add $0x4,%edx; flds 0x8(%eax,%edx,4)`，复现需 .h 成员数组视图，本任务禁改 .h；
//   N125 族）+ 槽号差（N82）。
#include "MonsterWrapLCB/generateDropAvatarItem_3.inc"
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out)
{
    // ① 入门阈值（ORIG fildll/flds/fmulp/fistpll 序列：64 位整数域 + float 乘法，
    //    表引用与 base 指针内联 —— ORIG 每访问点 `mov 0x8(%ebp),%eax` 重读参数，
    //    无 base/tableA/tableB 具名槽）。
    long long thresholdLL =
        static_cast<long long>(static_cast<unsigned int>(info.m_field00));
    // [Q7S5 2026-09-11 形态轴·triage] ORIG 两处表访问为成员数组寻址形
    //   0x08349dd2/0x08349de2 `add $0x4,%edx; flds 0x8(%eax,%edx,4)` /
    //   `lea -0x1(%eax),%edx; flds 0x8(%eax,%edx,4)`；旧稿用
    //   `reinterpret_cast<char*>(&info)+0x8+4*(…)` 指针算术 ⇒ 4.4 折成
    //   `add $0x6; shl $0x2; add; flds (%eax)`（N125 族）。改为 **既有视图成员**
    //   `m_field08[i]`（STMonsterScript.h:+0x08 float[9]，与 union 的
    //   m_field08v 同址）—— **地址与语义逐值不变**，仅寻址形态对齐 ORIG。
    thresholdLL = static_cast<long long>(
        static_cast<float>(thresholdLL) *
        info.m_field08[static_cast<unsigned>(rarity) + 4] *
        info.m_field08[static_cast<unsigned>(grade) - 1]);
    const int threshold = static_cast<int>(thresholdLL);
    if (get_rand_int(100000) > threshold)
        return;

    // ② roll 轮次
    for (unsigned int round = 0; round < (unsigned int)info.m_field04; ++round)
    {
        const int r = get_rand_int(info.m_field2c);
        std::vector<stEnamyDropAvatarElement_t>::const_iterator it =
            info.m_elements.begin();
        while (it != info.m_elements.end())
        {
            if (static_cast<unsigned int>(it->m_field4) >
                static_cast<unsigned int>(r))
            {
                Inven_Item item;
                item.m_addInfo = it->m_field0;              // ORIG 写 +0x02 视图
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)1, item, 1);
                // [Q7S5 2026-09-11 语义轴] ORIG 0x08349ec0（create_item 之后、
                //   push_back 之前）：`ptEv; movzbl 0x8(%eax),%eax; movzbl %al,%eax;
                //   mov %ax,-0x52(%ebp)` —— -0x52(%ebp) = &item(-0x5d)+0xb = Inven_Item
                //   ::m_fieldb(endurance, ushort) ⇒ 原码此处把 elem.m_field8 写入
                //   item 耐久字段；缺失 ⇒ 掉落头像物品 m_fieldb 保持 create_item 后
                //   的 0，玩家可见耐久为空。p167 该处缺的 1 次 const_iterator::ptEv
                //   亦由本行补回。
                item.m_fieldb = it->m_field8;
                out.push_back(item);
                break;                                      // 不推进迭代器（ORIG jmp +0x170）
            }
            it++;
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349f4b _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
// （size 0x11a）：CatchItemInfo 三成员均 int 存储，第 3 成员由 flds 按 float
// 位型消费，故以 reinterpret 视图承载。
//   count(m_field4)==0 → 跳过；
//   rate(m_field8 视 float)==0.0f → 跳过（fnstsw+sahf 仅判 ZF：NaN 继续走）；
//   r = get_rand_int(0x64)；(double)rate > (double)r 才生成；
//   Inven_Item{}；item.m_addInfo = m_field0；create_item((T)1,item,1)；
//   push_back count 次（signed setg 比较）。
// [b28d-L14 P167/N90] ORIG 0x08598d2c；已改 m_itemList 直取（消 EXTRA getItemList）；
// 残差=12B 结构体按值拷贝形（N90 族）。
// [F2e-Singles2 C05③ 2026-09-10] D08 `||` 合并两守卫（m_field4==0 ‖ rate==0.0f）
//   **实测零效果**：我方汇编改前/改后逐条相同、our_insns 93=93、岛数不变
//   （3×`jmp+nop` 仍在）⇒ 已回滚（属 4.4 块布局/A28 族，非「多守卫同目标」可治）。
#include "MonsterWrapLCB/generateCatchMonsterItem_2.inc"
                              std::vector<Inven_Item>& out)
{
    for (std::vector<CatchItemInfo>::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const CatchItemInfo entry = *it;                    // [0x8349f86 de 三字段拷贝]
        Inven_Item item;                                    // [0x8349f9c C1 -0x61：先于三守卫]
        const float rate =
            *reinterpret_cast<const float*>(&entry.m_field8);

        // [laneE1 2026-09-11 P296/P223 复现] 旧形三条 `continue` 使 4.4 在环尾发
        //   3×`jmp <tail>; nop` 中继岛；ORIG 只有唯一 tail 0x0834a03e，三守卫各
        //   一条 `je tail` 直达（0x0834fa7/0x8349fbe+0x8349fc0/0x8349fe3）
        //   ⇒ 改**正向嵌套 if 包裹**（P296 已在 CheckDungeonEventStoryPause 实证；
        //   旧 D08 `||` 合并实测零效果，见上方判词）。
        if (entry.m_field4 != 0)                            // [0x8349fa7 test/je tail]
        {
            if (rate != 0.0f)                               // [0x8349fb2 fucompp: jp/je tail]
            {
                const int r = get_rand_int(0x64);           // [0x8349fc2 $0x64]
                if (static_cast<double>(rate) > static_cast<double>(r))
                {                                           // [0x8349fd1 seta; je tail]
                    item.m_addInfo = entry.m_field0;        // [0x8349fe5 写 +0x02 视图]
                    G_CDataManager()->m_itemList->create_item(
                        (ENUM_ITEM_CREATE_TYPE)1, item, 1); // [0x8349ffd create_item(1,.,1)]
                    for (int j = 0; j < entry.m_field4; ++j) // [0x834a031 setg signed]
                        out.push_back(item);
                }
            }
        }
    }
}
