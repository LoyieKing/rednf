// ORIG T _Z10isDbcsLeadc 0x88bb367：DBCS 首字节判定（最高位为 1）。
// 写回 `(c & 0x80) != 0` 使 4.4.x 生成 ORIG 完全相同序列
// （movzbl -0x4(%ebp),%eax; shr $0x7,%al; 无 setne）。
bool isDbcsLead(char c)
{
    return (c & 0x80) != 0;
}