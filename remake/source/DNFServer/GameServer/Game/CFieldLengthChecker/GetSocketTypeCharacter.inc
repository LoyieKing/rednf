// [b28d-Lane16] TU 归属迁移（否则 GI 键名不匹配）：
// ORIG 本 TU（= SyncScript.cpp，`orig_tu_names.tsv:107`
// `086269a2 _GLOBAL__I__Z22GetSocketTypeCharactert`）中 `GetSocketTypeCharacter`
// @0x0861ca1b 是首个全局函数声明 ⇒ GI 键名取它；定义必须置于本文件最前。
// ORIG T 0x0861ca1b：socket 类型 → 字符。1→'1' 2→'2' 4→'3' 8→'4'
// 0x10→'5' 0xffef→'6' 0xffff→'7'，其余 0（决策树见 ORIG 反汇编）。
int GetSocketTypeCharacter(unsigned short type)
{
    switch (type)
    {
    case 1:
        return '1';
    case 2:
        return '2';
    case 4:
        return '3';
    case 8:
        return '4';
    case 0x10:
        return '5';
    case 0xffef:
        return '6';
    case 0xffff:
        return '7';
    default:
        return 0;
    }
}