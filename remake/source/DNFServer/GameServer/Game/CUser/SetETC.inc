int CUser::SetETC(short key, int value)
{
    // ORIG W 0x080f9968 _ZN5CUser6SetETCEsi：
    // key 截断 short → m_mapEtc.operator[](key) → *ret = value → return value。
    // （m_mapEtc 已在 CUser.h +0x8eaa4 建模，旧 TODO 注释已过时。）
    m_mapEtc[key] = value;
    return value;
