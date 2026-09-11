// ORIG 0x082fec2c：遍历对象表，依次 push (m_x, m_y)。
// ORIG 汇编为 pair<int,int>::pair<int&,int&> 模板构造（左值引用实例，非
// make_pair 值传递），故此处用局部左值 + 直接 pair 构造对齐符号集。
void CRidable::GetObjectPos(std::vector<std::pair<int, int> >& out)
{
    // b28d-40 的「N82 封顶」判词作废：p167 实证 ORIG 侧全为**非 const**
    //   `_ZNSt17_Rb_tree_iteratorI…E{ptEv,ppEv,ne}`（我方 const_iterator 额外引入
    //   2×转换构造 C1ERKS_ +2 条）⇒ 换非 const 迭代器（P96/P270）。
    // 另：ORIG 0x082fec54..0x082fec75 六条 `mov N(%eax),%edx` 是 `DungeonRidableObject
    //   obj = it->second;` 的整体拷贝（-0x30..-0x1c），随后 pair 实参取
    //   `&obj.m_x`/`&obj.m_y`（`lea -0x30` + `add $0x4`）⇒ 恢复整体拷贝形。
    for (std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
         it != m_objects.end(); ++it)
    {
        // ORIG 0x082fec54..0x082fec75 只拷 6 个 dword（源 +0x4..+0x18 = value 的
        //   +0x00..+0x14）⇒ 原码按**基类切片**拷贝 `RidableObject obj = it->second;`
        //   （派生 m_state@+0x18 不拷；我们写 DungeonRidableObject 会多 1 个 dword）。
        RidableObject obj = it->second;
        out.push_back(std::pair<int, int>(obj.m_x, obj.m_y));
    }
}