// ORIG 0x88908c6：按 "attack type physical/magical/absolute" 返回 0/1/2。
// [b28c-L7] 判词：AE 修法=具名局部累加形（ORIG `movl $0x7,-0xc` 初值 +
// 各分支 `movl $N,-0xc` + 单出口 `mov -0xc,%eax`）；早返 return 形多 jmp 岛。
int AttackInfo::getAttackType(std::string& tag)
{
    int type = 7;
    if (tag == "attack type physical")
        type = 0;
    else if (tag == "attack type magical")
        type = 1;
    else if (tag == "attack type absolute")
        type = 2;
    return type;
}