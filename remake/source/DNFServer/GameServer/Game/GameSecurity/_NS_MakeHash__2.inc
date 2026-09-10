void _NS_MakeHash_(char* data, unsigned short len, unsigned int& out)
{
    _NS_MakeHash_(data, static_cast<short>(len),
                  reinterpret_cast<int&>(out));
