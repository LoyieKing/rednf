void DisPatcher_DieCharacInDungeon::send(CUser* user, ParamBase& param)

{
    // ORIG 0x081c2baa：指针局部落栈 -0x4 → 取 +0x4 m_errNo → test %eax,%eax
    // 后 `return`（tail-merge 后残留 dead test）。
    stDieCharacInDungeon* pParam = static_cast<stDieCharacInDungeon*>(&param);
    if (pParam->m_errNo)
        return;
}
