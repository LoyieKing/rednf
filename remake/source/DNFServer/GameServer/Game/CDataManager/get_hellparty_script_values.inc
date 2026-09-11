// [b28d-E03 2026-09-10] 返回类型改 stHellParty_t*（见头注释）：函数体仍为单条 mov，
// 仅头/定义签名同步；消费点 static_cast<stHellParty_t*>(…) 为恒等转换、codegen 不变。
stHellParty_t* CDataManager::get_hellparty_script_values()
{
    return reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
}