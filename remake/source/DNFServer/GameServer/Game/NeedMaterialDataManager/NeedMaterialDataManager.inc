// [b28c-31] N76：ORIG 0x081b5a4f 先 `lea 0x18(%eax),%edx` 再物化实参（LHS 地址
// 先载），4.4 恒 RHS 先；显式列全成员初始化列表无效 ⇒ 封顶。
// [b28d-11b] N75/P75：this+0x18 地址与实参常量物化序跨版本不可翻（4 形探针：4.4 恒先物化 -1/0 再 lea）；残余 4 条顺序差 @0x081b5a3c
// [P1T5 复核] 444/446 后端 A/B（/tmp/tu_NeedMaterialDataManager_{444,446}.o）同形（34/33，
// 差值同 4.4.7）⇒ 非后端版本差异，确为 4.4 系 RTL 求值序（P211 LHS-first）不可翻，维持封顶。
NeedMaterialDataManager::NeedMaterialDataManager()
    // [b28c-31] ORIG 0x081b5a4f：map 构造后先 `lea 0x18(%eax),%edx` 再物化实参
    // （P211 LHS 地址先载形）⇒ 显式列全成员试翻装载序。
    : m_map(), m_default(-1, 0)
{
}