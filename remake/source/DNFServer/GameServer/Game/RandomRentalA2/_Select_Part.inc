// ---- _Select_Part —— T 0x085f8ec2（报告缺失，objdump 直转）----------------
void RegenerationROI::_Select_Part(int partIdx, std::vector<int>& out)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 132/131；ORIG 0x085f8ec2）：P0/P183
    // names/nameTable 内联已消引用缓存；残余帧/槽序。旧判词：
    // [b28d-PoolMisc2 修成] P227 trace 临时化 ⇒ 两 call 块序归位，件 **DIFF→NEAR**、
    // 131→132 与 ORIG 等长，结构残差 struct 9→**0**、ndiff 29→15（全为帧 0x40/0x50
    // 与 SLOT 平移）⇒ 余差按 N82附/P190 撑帧封顶（工具 /tmp/structdiff.py）。
    // + 局部槽序。语义等价。
    // [Lane13] 旧 P190 double framePad 撑帧为**误判**：ORIG 085f8ec2 帧 sub $0x40 /
    // find sret -0x30；加 pad 后我方 sub $0x50 / sret -0x40 全帧深 0x10 ⇒ 反向劣化。
    // 删除 pad 后帧/槽与 ORIG 逐一对齐（实测 sub $0x40、sret -0x30、groupName -0x34）。
    std::map<int, str_index_to_itemgroup>::iterator it =
        m_regenerator.m_map0.find(partIdx);

    std::string groupName;
    // [Lane13] P74补：ORIG 085f8ef4 段先 `lea -0x30(&it)→0x4(%esp)` 再
    // `lea -0x2c(end 临时)→(%esp)` ⇒ this=&end_temp、arg=&it ⇒ 原码写
    // `end() != it`（end 在表达式左侧控实参序）；旧 `it != end()` 形实参序相反。
    if (m_regenerator.m_map0.end() != it)
    {
        // [b28d-solo-RandomRentalA2] P0/P183：ORIG 每轮重取 it->second.m_vec8
        // （`call it->second; add $0xc`）且 getItemGroupNameTable() 两次独立调用
        // （find/end 各一次），无 names/nameTable 引用缓存局部。
        for (std::size_t i = 0; i < it->second.m_vec8.size(); ++i)
        {
            groupName = it->second.m_vec8[i];
            std::map<std::string, int>::const_iterator f =
                getItemGroupNameTable().find(groupName);
            if (f != getItemGroupNameTable().end())
            {
                out.push_back(f->second);
            }
        }
    }
    else
    {
        // [b28d-PoolMisc2 P227 修成] ORIG 0x085f8ec2 else 块：先 `lea groupName,%eax;
        // mov %eax,(%esp); call std::string::c_str; mov %eax,%edx` **再**发 trace
        // ctor（`movl $5,0xc; movl $0xd6,0x8; movl $<name>,0x4; lea trace槽,%eax;
        // mov %eax,(%esp); call C1`）⇒ 原码 cMyTrace 为全表达式临时（ctor 在 vararg
        // 求值之后）。原具名 `cMyTrace t(...)` 形把 ctor 提到 vararg 之前 ⇒ 两 call
        // 块序互换（结构残差 9 行的全部）。cMyTrace 无用户析构（GameTypes.h:92）
        // ⇒ 临时化不改可观察行为。
        cMyTrace(__PRETTY_FUNCTION__,
                 0xd6, 5)("packet:%d groupName:%s\n", partIdx, groupName.c_str());
    }
}