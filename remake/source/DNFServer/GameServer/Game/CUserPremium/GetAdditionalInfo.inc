// ORIG W 0x8694a88 GetAdditionalInfo() const：sret ← 隐式拷贝 ctor 的
// 内联块移（对齐检查 + rep movsl + word/byte 尾，0x52 字节，无 clear 调用
// ⇒ ORIG 源即 return 成员表达式，非 局部+memcpy——后者多 out ctor 与 call）。
_Additioal_info CUserPremium::GetAdditionalInfo() const
{
    return m_additionalInfo;
}