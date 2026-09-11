// ORIG 0x08898ff0（报告 docs/class_func_reports/advancealtar/isValidRidableId.md
// ## 汇编：mov 0x8(%ebp),%eax; cmp $0x1,%eax; sete）：
// 仅 RidableId::RIDEABLE（值 1）有效。ORIG 比较为 reg 形（mov 0x8(%ebp),%eax;
// cmp $0x1,%eax），按 P209 源侧保持 unsigned 语义触发 4.4 载寄存器。
bool isValidRidableId(RidableId::T ridableId)
{
    if ((unsigned int)ridableId == 1u)
        return true;
    return false;
}