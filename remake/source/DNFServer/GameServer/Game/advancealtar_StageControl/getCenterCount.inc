// ----------------------------------------------------------------------------
// ORIG W 0x081307a6（getCenterCount.md 全文）：map/user 判空 → 0；
 // 遍历 CMap::get_monster_list()（按值返回 list<_mapMonster>），对每只怪
 // getTowerInfo(monster.m_orderIdx, out, out)（+0x2c），命中即计数。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] getCenterCount：ORIG 103 / ours 99。已对齐：P183 去
//   procStage/characData 具名指针槽（直接 this+4 / this+0x98）；P28 增量段 it++（ppEi
//   后置带 sret 临时槽）；P96 非 const iterator（去转换构造临时）；ORIG -0x10 list 指针
//   具名槽（monsterList = &monsters，begin/end 从槽取 this）逐条复现。
//   唯一残差 = hit 取反物化：ORIG `movzbl -0x9,%eax; xor $1,%eax; test %al,%al; jne
//   <skip>; ++count; jmp; nop`（8 条）vs 4.4 `cmpb $1,-0x9; jne; ++count`（P168a④：
//   寄存器 xor 物化无绑定不可达，X2 两语句落栈槽；`hit^1` 被折 mem 直比 N18b）。
//   语义等价（hit!=0 ⇒ ++count）。N 封顶（P168a④/N18b 族）。
// [b28d-sweep 2026-09-09] 复验：归一化后唯一差异仍是上述 4↔1 条；另试 const 容器 +
//   const_iterator（ORIG 0x08135d88/0x08135dae 段确用 `_ZNKSt4list...` const 版）⇒
//   verdict 无改善（仍 DIFF）且本 TU 少发射 iterator→const_iterator 转换构造
//   ⇒ check_tu identical 80→79 回退，已回退维持非 const iterator。
// [round1-L29 2026-09-10 修复] 真因：callee `getTowerInfo` 权威声明**已是 bool**
//   （advancealtar.h:421 / advancealtar_DB.cpp:314），我方却用 `unsigned char hit` 接收 ⇒
//   4.4 折 `cmpb $1,-0x9`。改为 `const bool hit` + P223 goto 尾空标签形
//   （`if (!hit) goto hit_done; ++count; hit_done:;`）⇒ **AE 102/102 ndiff=0**。
//   ORIG 0x08130869 证据形：`mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax; test %al,%al;
//   jne <skip>; addl $1,-0x14; jmp <join>; <skip>: nop`。
// [W13C 2026-09-11 探针A·负结论·双处同改] 单改 sendMapInfo 的 const 化曾使 identical
//   −1（见该函数判词），故本波试「getCenterCount + sendMapInfo **两处同时** const 化
//   （容器 const ⇒ begin() 走 `_ZNKSt4list...5beginEv`，期望消掉转换构造
//   `_List_const_iteratorC1ERK_List_iterator` 后转净收益）」。影子树实测（routed 口径）：
//   **identical 80→77、ae 198→196、sendMapInfo nd 42→51** ⇒ **反向**，维持非 const
//   （ORIG 0x08130869 段确用 const 版符号，属 P183/符号选择档，不换 verdict）。
int StageControl::getCenterCount() const
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [b28d-solo-StageControl] P183：ORIG 直接 `this+4` 调 getMap、`this+0x98` 调
    //   getUser（无具名 procStage/characData 指针槽）；P28：ORIG 增量段调 ppEi（后置
    //   迭代器带独立 sret 临时槽）⇒ 源形必写 `it++`。
    CMap* map = reinterpret_cast<const ProcStage*>(m_procStage)->getMap();
    if (map == 0)
    {
        return 0;
    }
    CUser* user = reinterpret_cast<const CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return 0;
    }

    int count = 0;                                      // -0x14
    // [回滚记录 b28d-sweep] 曾试 const 容器 + const_iterator（ORIG 0x08135d88 段确用
    //   `_ZNKSt4list...5beginEv`/`_ZNKSt20_List_const_iterator...`）；verdict 无改善
    //   （残差仍是下方 hit 物化，仍 DIFF），且本 TU 不再发射 ORIG 别处实存的非 const
    //   `_ZNSt14_List_iteratorI11_mapMonsterEppEi/EptEv`（0x08311764/0x08311756）⇒
    //   check_tu_routed identical 80→78 回退，故维持非 const iterator 形。
    std::list<_mapMonster> monsters = map->get_monster_list();
    // ORIG 0x0813080e：`lea -0x2c,%eax; mov %eax,-0x10(%ebp)` —— 具名 list 指针槽
    // -0x10，begin/end 均从该槽取 this（Ghidra local_14 = local_30）。
    std::list<_mapMonster>* monsterList = &monsters;
    for (std::list<_mapMonster>::iterator it = monsterList->begin();
         it != monsterList->end(); it++)
    {
        int valueA = 0;                                 // local_88[1]
        int valueB = 0;                                 // local_88[0]
        // ORIG 0x08130869：getTowerInfo 结果先落具名字节槽 -0x9（`mov %al,-0x9`），
        // 再 `movzbl -0x9; xor $1; test; jne <skip>; ++count; jmp; nop` 取反物化。
        const bool hit =
            user->m_advanceAltar.getTowerInfo(it->m_mobId, valueA, valueB);
        // ORIG 0x08130869：jne <skip 岛> + body 尾 jmp <join> ⇒ P223 goto 尾空标签形。
        if (!hit)
            goto hit_done;
        ++count;
    hit_done:;
    }
    return count;
}