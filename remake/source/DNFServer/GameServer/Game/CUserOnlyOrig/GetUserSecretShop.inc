void* CUser::GetUserSecretShop()
{
    CParty* party = static_cast<CParty*>(GetParty());
    // ORIG 0x086885e8-0x08688607：`cmpl $0; je 8602(mov$0 尾岛)`；body call 后 `jmp 8607`。
    // = 正向 `if (p) return f();` + 尾 `return 0;`（fall-through=then 在前，技巧17）
    if (party != 0)
        return party->GetUserSecretShop(this);
    return 0;
