// ORIG 0x08654768 _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
// 将 CUserCharacInfo 角色内存 +0x1254 处（getSkillCommand()）的 skill command map
// （std::map<int,STSkillCommandData>）逐项压缩为 STCommand（32 位位域）写入 out[]，最多 50 项。
// map 为空指针时 out 不变。
// [R10a04 第十轮 ④#7] 返回类型 void：ORIG 0x08654768 **所有出口块均不设置返回值**
//   （0x08654780 `je 0x0865491e` → `nop; jmp 0x08654922`；三条 epilogue 边皆不写 eax，
//   P159 判据）⇒ 原码无 return 语句。我方原写 `return out;` 两处 ⇒ 多出
//   `mov 0xc(%ebp),%eax` 物化 + 早退极性反（我方 jne 续 vs ORIG je 跳尾）。
//   唯一调用点 CUser/GetSkill.inc:35 `get_skill_command_buf(cmds);` 忽略返回值 ⇒ void 合法；
//   mangled 不含返回类型 ⇒ 符号名/尺寸不变。
void CUser::get_skill_command_buf(STSkillCommandData::STCommand* out) const
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap != 0)
    {
        // ORIG 0x08654768：`end()` 提到循环外（循环体内无 end() 调用），`idx > 0x31`
        //   走循环尾 `jg`（= 显式 break 形，避免 `&&` 的 setne/test bool 物化，P133/P59）。
        std::map<int, STSkillCommandData>::iterator it = pMap->begin();
        std::map<int, STSkillCommandData>::iterator itEnd = pMap->end();
        int idx = 0;
        for (; it != itEnd; )
        {
            // b28d-solo-CUser p167：ORIG 0x0865459a/0x08654768 每字段各发一次
            //   `_Rb_tree_iterator::operator->()`（共 6 次，我方缓存引用 v 缺 6 次）⇒
            //   去掉具名引用，逐字段 `it->second.m_xxx`（P167 判据/P183 反向）。
            STSkillCommandData::STCommand cmd;
            cmd.m_command = it->second.m_command;   // bits 0-13（& 0x3fff）
            cmd.m_count   = it->second.m_count;     // bits 29-31
            cmd.m_key0    = it->second.m_key0;      // bits 14-16
            cmd.m_key1    = it->second.m_key1;      // bits 17-19
            cmd.m_key2    = it->second.m_key2;      // bits 20-22
            cmd.m_key3    = it->second.m_key3;      // bits 23-25
            cmd.m_key4    = it->second.m_key4;      // bits 26-28
            out[idx] = cmd;
            ++idx;
            if (idx > 0x31)
                break;
            ++it;
        }
    }
}