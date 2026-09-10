void ServerParameterScript::setDungeonOpen(int dungeonIdx, bool open)
{
    // b28d-solo-ServerParameterScript: 真语义 bug 修正 — ORIG 0x08268698
    // `mov 0x10(%ebp),%eax; mov %al,-0x3c; cmpb $0x1,-0x3c; je 0x826874d`
    // ⇒ **open==1 走 erase 路径**（je 目标段：ne(find,end) 真则 erase），
    // open==0 落 fall-through 段：eq(find,end) 真则 push_back。旧源码
    // `if (!open)` 把两段整段互换（erase↔push_back），行为与原码相反。
    // 分支形态照 ORIG：erase 段 `if (it != end) erase`（ne）；
    // push_back 段 `if (it == end) push_back`（eq，ORIG 0x82688da 实例名）。
    // 残差（N 封顶）：ORIG `cmpb $0x1,-0x3c` vs 4.4 `cmpb $0x0,-0x3c`（bool 条件码
    // 规范化方向跨版本）+ erase/push_back 两段 fall-through↔je 目标互换（N93附 块序；
    // 两形互试实测 101 vs 103 不闭合）。
    // [b28d-L28] 块序/条件码翻正：ORIG 段序 = push_back 段 fall-through +
    // `cmpb $0x1,-0x3c; je <erase 段>`（erase 为后置 else 块）。旧 `if (open)`
    // 形产 `cmpb $0x0; je <push_back>`（条件码与目标段互换，N93附 账面 101 vs 103）。
    // 写 `open != true` + if/else 使 4.4 出 `cmpb $1,%al; je <else>` 与 ORIG 同向。
    // [b28d-L28] 块序已翻正（push 段 fall-through / erase 段后置 else），唯一残差 =
    // ORIG 0x08268698 `cmpb $0x1,-0x3c(%ebp); je <erase 段>` vs 4.4 对 bool 取反的
    // `movzbl -0x3c; xor $0x1; test %al; je`（N122 恒 xor；4.4 无 bool 取反的 cmpb 直比
    // 形）。三形实测：①`open != true`+if/else → ourN=103（本形，diff 块 1 个，多 2 条）；
    // ②`(unsigned char)open != 1` → 同 xor（ourN=103）；③正向 `open == 1`+goto 后置块
    // → 出 `cmpb $1; jne`（极性反，N93附）+ 尾 jmp/nop 岛（N121）ourN=105 更差 ⇒ 回退。
    // 判据①复核：jcc 目标下标逐位 0 mismatch（仅 cmp 助记符形态差）。判 N（N122）。
    if (open != true)
    {
        std::vector<int>::iterator it2 = std::find(
            m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
        if (it2 == m_dungeonOpenE0.end())
        {
            m_dungeonOpenE0.push_back(dungeonIdx);
        }
    }
    else
    {
        std::vector<int>::iterator it = std::find(
            m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
        if (it != m_dungeonOpenE0.end())
        {
            m_dungeonOpenE0.erase(it);
        }
    }
