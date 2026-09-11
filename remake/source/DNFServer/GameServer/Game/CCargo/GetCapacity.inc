// ---- 槽位访问 ----
// [G08 2026-09-11] 签名随 CUserCharacInfo.h 声明改 unsigned→int（Main 批准；
// 依据 = respond_special_item_cargo_abs 的 lea/add 差；同宽、不入 mangling）。函数体不动。
int CCargo::GetCapacity() const
{
    return m_capacity;
}