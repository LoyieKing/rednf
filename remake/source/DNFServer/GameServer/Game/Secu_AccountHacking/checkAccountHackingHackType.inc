// [SweepCHackAnalyzer P327] 定义后移至文件末尾：让 CHackAnalyzer::setUserP 成为本
// TU 第一个「强符号·外部链接·本 TU 定义」实体，对齐 ORIG GI 键名
// `_GLOBAL__I__ZN8WongWork13CHackAnalyzer8setUserPEP5CUser`（ORIG 0x080f97f4）；
// 本函数 ORIG T 0x08278a48（0x4f 字节，0x2c6..0x2cb/0x2cd 线性判定）。
bool Secu_AccountHacking::checkAccountHackingHackType(WongWork::ENUM_HACKTYPE type)
{
    if (type == 0x2c6 || type == 0x2c7 || type == 0x2c8 ||
        type == 0x2c9 || type == 0x2ca || type == 0x2cb ||
        type == 0x2cd)
        return true;
    return false;
}