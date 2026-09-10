int CUserPremium::GetReturnItemRate(int type) const
{
    // ORIG 0x86ae7a8：先 empty() 预检；find/end 结果落 const_iterator 临时
    // 槽后 operator!= 比较（迭代器不直呼内联比较形）
    // [b28c-L7] P59 反向：ORIG 0x86ae7b9 `call empty; test %al,%al; je +0x21`
    // （空→原地 fall 到 `mov $0,%eax; jmp end`、非空→je 跳 body）⇒ 原码是
    // 正极性 guard `if (empty()) return 0;`；`if(!empty()){}` 形 4.4 恒产
    // `xor $1` 反演 + body 前置。map::empty 为 bool callee（P168a①裸形）。
    if (m_returnItemMap.empty())
        return 0;
    std::map<int, int>::const_iterator it = m_returnItemMap.find(type);
    // [b28c-L7] 残差 2 处 = N93（控制流/装载形已全等）：find 结果槽 ORIG
    // -0x10 vs 4.4 -0xc、end 结果槽 ORIG -0xc vs 4.4 -0x10——同块内两个直接
    // 初始化迭代器，4.1 按声明序占低→高槽、4.4 相反（N93 原例即本仓
    // DB_LoadInventory::_GetTagCharacInven 族）。排除形实测：交换声明序 ⇒
    // end() call 前置于 find()（ORIG 调用序反，新增 2 处失配）；提头声明
    // + 块内赋值 ⇒ 触发 _Rb_tree_const_iterator 默认 C1 + sret 临时拷贝
    // （N86，+3 条）。源形不可翻，语义与条数等价 ⇒ 封 N93。
    // [b28c-L7] P222：ORIG 0x86ae81a `call ne; test %al; je <mov$0 尾岛>` +
    // SUCC(ptEv+mov 0x4) fall-through 在前 ⇒ 正极性 `if(it!=e) return …`，
    // 否定早返形把 SUCC 推到岛后（+1 jmp）。
    // [b28d-solo-CUserPremium] P326 试形：ORIG 0x086ae7e3 find→-0x10、
    // 0x086ae7fe end→-0xc（4.1 先声明更深），4.4 具名双迭代器恒相反
    // （find→-0xc/end→-0x10）。把 end() 包成右值临时内联进比较，看是否
    // 复现 ORIG 槽阶梯。
    if (it != std::map<int, int>::const_iterator(m_returnItemMap.end()))
        return it->second;
    return 0;
