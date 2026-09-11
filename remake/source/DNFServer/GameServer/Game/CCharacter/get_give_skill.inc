// ORIG get_give_skill @ 0x8348798。
bool CCharacter::get_give_skill(int firstGrow, int secondGrow,
                                std::vector<std::pair<int, int> >& out,
                                unsigned int flag) const
{
    // ORIG 0x08348798：所有分支经 bool r 槽（-0x9，1 字节直存）物化后统一 movzbl 尾返；
    // flag==0 支双赋值（r=f1(); r=f2(); 死写照抄 P171）。
    bool r;
    if (flag == 0)
    {
        r = get_give_skill_firstGrowtype(firstGrow, out);
        r = get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
    }
    else if (flag == 1)
        r = get_give_skill_firstGrowtype(firstGrow, out);
    else if (flag == 2)
        r = get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
    else
        r = false;
    return r;
}