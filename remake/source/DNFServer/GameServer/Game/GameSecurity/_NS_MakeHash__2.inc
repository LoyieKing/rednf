// ---------------------------------------------------------------------------
// _NS_MakeHash_  ORIG T 0x0808c976（报告 _NS_MakeHash__0808c976.md）
// unsigned 形参薄包装：len 经 cwtl 符号扩展为 short 转调 int& 版本。
// ---------------------------------------------------------------------------
void _NS_MakeHash_(char* data, unsigned short len, unsigned int& out)
{
    _NS_MakeHash_(data, static_cast<short>(len),
                  reinterpret_cast<int&>(out));
}