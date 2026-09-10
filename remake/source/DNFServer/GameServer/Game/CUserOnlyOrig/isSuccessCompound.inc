bool CUser::isSuccessCompound(int chance)
{
    if (getRecipeProbability() == 1)
        return true;
    if (getRecipeProbability() == 2)
        return false;
    // [b28d-solo-CUserOnlyOrig P258/P214①] ORIG 0x0867201c `cmpl $0,chance; je <mov$1>; …
    // call; cmp chance,%eax; jge <mov$0>` = `||` 短路直跳形；嵌套 if 形在 4.4 恒物化
    // setge+test（旧 34 条）。改 `(chance==0) || (rand < chance)` 复现直跳（探针 p_rand f5）。
    return (chance == 0) || (get_rand_int(100) < chance);
