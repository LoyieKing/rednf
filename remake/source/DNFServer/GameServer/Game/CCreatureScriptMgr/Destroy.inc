// ORIG Destroy @ 0x833d3bc：遍历 +0x18 map，删除（D1 + operator delete）每个
// CCreatureScript*，最后 clear()。
// [b28d-30] 复核 DIFF：我方少 8 条（析构/clear 路径形差，N82/P12 族），ORIG 0x0833dxxx 见 report。
void CCreatureScriptMgr::Destroy()
{
    // round1 R19：ORIG 0x0833d3c3 段 `call size; test %eax,%eax; sete %al;
    // test %al,%al; jne`（P197④：`size() == 0` 才出 sete 物化，`empty()` 无）；
    // cond 段每次重算 `end()`（无具名 end 槽，临时 -0x14）；增量段调 `ppEi`
    // （P28：ORIG 后置 ⇒ 源写 `it++`）；循环体 p 有预清 `movl $0x0,-0xc` 与
    // delete 后 `p = 0;`（P197③ 两句式）。
    if (m_scripts.size() == 0)
        return;
    for (std::map<int, CCreatureScript*>::iterator it = m_scripts.begin();
         it != m_scripts.end(); it++)
    {
        CCreatureScript* p = 0;
        p = it->second;
        if (p != 0)
        {
            delete p;
            p = 0;
        }
    }
    m_scripts.clear();
}