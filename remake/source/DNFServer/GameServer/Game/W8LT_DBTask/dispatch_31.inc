// ORIG 0x8418bde：-0xd bool 槽函数首条 movb $0 初始化 → GetOutBuffer<SIG_
// UPDATE_QUEST> 存 -0xc → cmpl $0/jne body（空指针 fall-through `return
// false`）→ body call _updateQuests 后 movzbl -0xd 返 bool 槽（值恒 0）。
bool DB_UpdateQuest::dispatch(int userIdx, int slot, Stream* stream)
{
    bool bResult = false;
    SIG_UPDATE_QUEST* sig = stream->GetOutBuffer<SIG_UPDATE_QUEST>();
    if (sig == 0)
        return false;
    _updateQuests(sig);
    return bResult;
}