// ---- 全局 IMethod 工具成员（ORIG T 0x080b459e/0x080b4616；声明 CEnvironment.h:42-43 static，
// ServerLib 侧为 nsl::IMethod 不同 mangled ⇒ game 全局形定义须落本 TU——Main 按 DBMW/Method.cpp
// :134-149 语义复制，G7 算法豁免面，big-endian 字组双向转换）----
void IMethod::BytesToWord(unsigned char const* src, unsigned int& dst)
{
    dst = 0;
    dst |= (unsigned int)src[0] << 24;
    dst |= (unsigned int)src[1] << 16;
    dst |= (unsigned int)src[2] << 8;
    dst |= (unsigned int)src[3];
}