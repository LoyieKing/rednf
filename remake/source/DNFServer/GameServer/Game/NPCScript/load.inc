// ORIG 0x85809a6:
//   ret = importNpcScript((NpcScript*)(this+4), path);
//   if (ret == 1) *(int*)this = *(int*)(this+4);
//   return ret == 1;
bool CNPCScript::load(char const* path)
{
    // ORIG：importNpcScript 结果寄存器直测（xor $1/test，无 ret 槽）；
    // 尾 mov $0x1 常量返回（bool 导入成功即 1，P56 形）。
    if (!importNpcScript(&m_npcScript, path))
    {
        return false;
    }
    m_resultCopy = m_npcScript.m_scriptFileIndex;  // ORIG mov 0x4(%eax) disp 直读首 int
    return true;
}