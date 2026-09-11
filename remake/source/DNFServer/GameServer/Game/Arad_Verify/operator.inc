// ORIG W 0x08188c56：destroyAradData 的 for_each 删除函子。
// 汇编形态（objdump）：
//   mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je +0x54     ← p.second 判空
//   取 vptr; add $0xc; call *(槽3)                              ← clearScriptData()
//   重读 p.second; test; je +0x4a                               ← 二次判空
//   取 vptr; add $0x4; call *(槽1)                              ← D0 deleting dtor
//   movl $0,0x4(pair)                                          ← p.second = 0
void FN_DeleteGameScript::operator()(
    std::pair<const std::string, Arad_InterfaceGameScript*>& p)
{
    if (p.second != 0)
    {
        p.second->clearScriptData();
        delete p.second;
        p.second = 0;
    }
}
