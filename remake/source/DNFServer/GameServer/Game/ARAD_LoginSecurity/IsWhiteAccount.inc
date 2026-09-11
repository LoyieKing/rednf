bool AradWhiteAccount::IsWhiteAccount(unsigned int accId)
{
    const_iterator it = find(accId);                 // std::set::find（const 重载经由 this）
    // P56：ORIG choose 岛（test;je→mov$1;mov$0）＝ `if(==end) return false; return true;`
    if (it == end())
        return false;
    return true;
}