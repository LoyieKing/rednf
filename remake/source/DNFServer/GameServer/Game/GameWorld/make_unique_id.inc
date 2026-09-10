unsigned int GameWorld::make_unique_id()
{
    // ORIG 0x86c4b0a: 入口 `jmp 0x86c4b13`；回跳落点 = 空块 nop@0x86c4b12（技巧24 goto 落点岛）
    goto COND;
LOOP:
    ;
COND:
    m_uniqueId++;                              // ORIG 0x86c4b13 前缀 ++（技巧4 半字版）
    if (m_uniqueId == 0xffff)                  // ORIG 0x86c4b2b cmp $0xffff,%ax
    {
        m_uniqueId = 0;
    }
    // ORIG 调用序 = 0x086c4b57 `map::find` → 0x086c4b6f `map::end` → eq：
    // 合并成一句 `find(...) == end()` 时 4.4 把 end() 排到前面（实测首个 call
    // 变 end）⇒ 拆成两个具名 iterator 局部、按求值序声明钉住 find 先于 end。
    // b28d-lane11 判词 N82（0x086c4b49..0x086c4b84 实证）：ORIG 4.1 按声明序
    // 先深（it=-0x10、last=-0xc）；4.4 按首次使用序先浅（it=-0xc、last=-0x10），
    // find/end 两处 sret lea 与 operator== 的 this/实参随之互换。试形：逗号同句
    // 声明（实测同形）、直接初始化、声明/赋值分离（多 emit iterator 默认构造
    // call+copy 共 5 条）均不可翻 ⇒ 双 sret 迭代器槽互换封 N82/N98。
    // ORIG 尾 `test %al,%al; je 0x086c4b12` ⇒ 用 eq（真则 fall-through 到 return）。
    // [b28d-solo-GameWorld] _ZN9GameWorld14make_unique_idEv verdict=NEAR 45/45
    // N82/N98 封顶（b28d-solo 本轮归一化复核：助记符/操作数序列全等，仅双 sret
    // 迭代器槽互换）：ORIG 0x86c4b49 lea -0x10(it)/0x86c4b68 lea -0xc(last) 先深，
    // 4.4 先浅 -0xc/-0x10；operator== 两侧 this/实参随之互换（语义等价）。
    // 本轮复测：逗号同句声明/拆两句/声明赋值分离（多 C1Ev）三形均不翻（N98 定案）。
    std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.find(m_uniqueId);
    if (it == std::map<unsigned short, CUser*>::iterator(m_UserInWorld.end()))
    {
    }
    else
    {
        goto LOOP;
    }
    // b28c-lane3：历史 register asm("eax") 绑定按 P0/N96 清除。
    // ORIG 0x086c4b8f-0x086c4b97：`mov 0x8(%ebp),%eax; movzwl 0x24(%eax),%eax;
    // movzwl %ax,%eax` 双零扩展；返回点直读字段的自然形保留该两段。
    return m_uniqueId;
