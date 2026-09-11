// ncl_char_plus(char, int) —— 数字字符加增量后按 0-9 回绕。
// ORIG：movsbl 符号扩展 → sub $0x30 → add n → i586 除法魔法 %10。
int ncl_char_plus(char c, int n)
{
    return (c - '0' + n) % 10;
}