// ORIG T 0x0820582b _Z20checkValidCharacNamePKc sz 0x6c（objdump 全文转写）：
// G_CDataManager()->hasSpecialCharacter / hasPreventString / hasPreventStringName 三连。
bool checkValidCharacName(const char* name)
{
    // b28c-l23 根因：ORIG 尾段 `mov $0x1,%eax / jmp / mov $0,%eax / test %al,%al /
    // je →mov $1 / fall→mov $0`（@0x08205879..0x08205890）= **短路结果先物化、
    // 再整体取反返回**，且无 bool 局部槽（帧 sub $0x18）。我方具名
    // `bool invalid` 使 4.4 出 `mov %al,-0x9; cmpb $0,-0x9` 双段并撑帧 +0x10；
    // 改写 `if(...) return true` 则 mov $1/mov $0 两岛顺序反转（ndiffX 5→4 后唯一残差）。
    // 正形 = `return !(A || B || C);`：语义与调用点 `if (!checkValidCharacName(..))`
    // （true=名字有效）一致。
    if (G_CDataManager()->hasSpecialCharacter(name) ||
        G_CDataManager()->hasPreventString(name) ||
        G_CDataManager()->hasPreventStringName(name))
        return false;
    return true;
}