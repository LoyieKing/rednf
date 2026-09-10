int StageControl::getCenterCount() const
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
