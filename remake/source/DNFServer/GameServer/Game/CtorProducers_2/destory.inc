void StrikerScript::destory()
{
    // [b28d-48] N82 封顶（NDIFF 65，69/72 条）：ORIG 先 `mov 0x90(%eax),%ebx`
    // 再判空（直接读成员）vs 4.4 先 mov 缓存 eax 再判空；帧/槽平移。
    // ORIG 0x08a9dcbe：delete 文件列表（STScriptFileList::D1 + operator delete）；
    // 遍历技能 map delete 各 vector 并置空；尾部 m_skillMap.clear()。
    // [SweepCtorProd2] 原判「二次装载不可造」作废：ORIG 的两次 `mov 0x90(%eax)`
    // 是「源码显式 if 判空」+「delete 自带空指针守卫」两段（探针实测 4.4 同形）⇒
    // 补显式 if 即逐条复现（`mov 0x90(%eax),%eax; test; je` + `mov 0x90(%eax),%ebx;
    // test %ebx,%ebx; je`）。
    if (m_fileList != 0)
    {
        delete m_fileList;                       // P301：具型指针单段直落
    }
    m_fileList = 0;
    // [b28c-lane17] ORIG 0x08a9dce8：循环谓词每次调用都**重新装载** m_fileList
    // 的迭代器对（`mov this→%eax; mov 0x90(%eax),%eax; test; ...; mov 0x90(%eax),%ebx`
    // 两条独立装载），且 delete 前无二次解引用。原写 `x = m_fileList; if (x)`
    // 被 4.4 折成单段直落（少 1 条）。
    for (std::map<int, std::vector<StrikerSkill>*>::iterator it = m_skillMap.begin();
         it != m_skillMap.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
        // [SweepCtorProd2·jmp 目标下标判据] ORIG 0x08a9dd9c 的 `movl $0,0x4(%eax)`
        // 在 if **外**（无条件重写 0；Ghidra C `if(...){delete} *(iVar2+4)=0;` 实证）：
        // ORIG `je +0x91`（置 0 块）vs 原写 if 内 `je +0xa3`（跳过置 0）——目标不同，
        // 旧 AE 归一化（TGT）掩盖了该差异。移出后 je 目标与 ORIG 逐位一致。
        it->second = 0;
    }
    m_skillMap.clear();   // ORIG 尾部 map clear
}