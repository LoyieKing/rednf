bool CSyncScript::GetType(std::string name, ENUM_CHARACTERJOB job, int idx,
                          int& out1, int& out2, bool flag)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；290=290）。
    //   归位要点：`name.compare("creature")==0`（int 直测）、SWEAPONTYPE/SARMORTYPE
    //   改 operator[] 实参处临时、`m_vec1.size()!=0`（免 empty 的 xor 段）、
    //   比较操作数交换使 local.m_field1 先载 %ebx。ORIG @0x0861cf0a。
    if (name.empty())
    {
        return false;
    }
    if (name.find_first_of("[", 0) != 0)
    {
        return false;
    }
    // ORIG 0x0861cf5f..0x0861d037：迭代器区间 erase（begin,begin+1 去 '['、
    // end-1,end 去尾 ']'，__normal_iterator begin/operator+/operator- 各带
    // sret 临时；索引 erase 形指令数不符）。
    name.erase(name.begin(), name.begin() + 1);
    name.erase(name.end() - 1, name.end());
    std::string key = "stackable ";
    // ORIG 0x0861d08c：`call compare("creature"); test %eax,%eax; jne <else>`
    //（int 直测 + 手动 bool 物化）⇒ 源形是 compare(...)==0，不是 operator==
    //（后者产 `test %al,%al; je`，与 ORIG 差 4 条）。
    if (!flag && name.compare("creature") == 0)
    {
        key += name;
    }
    else
    {
        key = name;
    }
    SITEMTYPE local = m_itemTypeMap[key];
    // ORIG 0x0861d10e：weapon 命中 = size()!=0 && vec[0]==local.m_field1。
    // ORIG 为 `call size; test %eax,%eax; je <else>`（32 位直测）⇒ 源形
    // `size() != 0`；`!empty()` 在 4.4 产 `xor $1; test %al` 两段（P197④ 镜像）。
    if (m_vec1.size() != 0 && local.m_field1 == m_vec1[0])
    {
        // ORIG 0x0861d14b：SWEAPONTYPE 是 operator[] 实参处的**临时**
        //（lea -0x1c 直接在实参位构造）；具名局部会把对象排到深槽。
        local = m_weaponMap[SWEAPONTYPE(job, idx)];
    }
    else if (m_vec1.size() >= 2 && local.m_field1 == m_vec1[1])
    {
        // ORIG 0x861d1d5：SARMORTYPE((uchar)idx, key)，key 为 "stackable "/名
        // 组合串（拷贝自 local_50），非去括号原名。
        local = m_armorMap[SARMORTYPE((unsigned char)idx, key)];
    }
    out1 = local.m_field1;
    out2 = local.m_field4;
    return true;
