void CRidable::SetRidableObject(RidableScript& script)
{
    if (script.m_0 <= 0)
        return;

    // ORIG 0x082fe6d0：obj（DungeonRidableObject local_c8）与 tmp（RidableObject
    // local_e0）在函数头各构造一次、两分支复用；count < size 分支顺序遍历
    // m_objects（循环条件每次重调 size()），否则拷贝 pool 随机抽取（循环条件用
    // script.m_0）；p167 实证 ORIG operator=(RidableObject) 仅 1 次（IF 分支），
    // ELSE 分支走基类隐式整体赋值（6 dword）。
    DungeonRidableObject obj;
    RidableObject tmp;
    // ORIG 槽图：循环1 计数器 -0xc（浅）、循环2 计数器 -0x10（深）⇒ 头部声明序
    // 实测 4.4 头部标量「后声明浅」：j 先声明确保 i 落浅槽 -0xc（P309 反向）；
    // ELSE 分支的 `j = 0;` 在 pool 构造之前发射（ORIG 0x082fe81c `movl $0x0,
    // -0x10(%ebp)` 先于 C1ERKS_ 0x082fe836）。
    int j;
    int i;

    m_flag18 = script.m_4;                     // ORIG 0x082fe707 movzbl 0x4(%eax),%edx
    m_count1c = script.m_0;

    // ORIG 0x082fe72f：`cmp %eax,%ebx; setge %al; test %al,%al; je <ELSE(0x14c)>`
    // ⇒ fall-through（m_0 >= size）走「顺序遍历 m_objects（上界 size）」，ELSE 走
    // 「拷贝 pool 随机抽取（上界 m_0）」。旧写 `<` 把两分支对象来源互换（真语义 bug）。
    if (script.m_0 >= static_cast<int>(script.m_objects.size()))
    {
        for (i = 0; i < static_cast<int>(script.m_objects.size()); ++i)
        {
            obj = script.m_objects[i];
            obj.m_state = 4;
            m_objects.insert(
                std::pair<int, DungeonRidableObject>(obj.m_field0c, obj));
        }
    }
    else
    {
        j = 0;
        std::vector<RidableObject> pool(script.m_objects);
        for (; j < script.m_0; ++j)
        {
            std::random_shuffle(pool.begin(), pool.end());
            tmp = pool.back();
            pool.pop_back();
            // ORIG 0x082fe8d3..0x082fe915 拷贝序（tmp@-0xdc → obj@-0xc4）：
            // +0xc,+0x10,+0x00,+0x04,+0x08,+0x14（4.1 对 24B 结构赋值的分块展开，
            // 直接写 `*(RidableObject*)&obj = tmp;` 4.4 恒按声明序 +0,+4,+8,+0xc,
            // +0x10,+0x14 发射 ⇒ 逐字段按 ORIG 序书写复现）。
            obj.m_field10 = tmp.m_field10;
            obj.m_field14 = tmp.m_field14;
            obj.m_x = tmp.m_x;
            obj.m_y = tmp.m_y;
            obj.m_field0c = tmp.m_field0c;
            obj.m_field18 = tmp.m_field18;
            obj.m_state = 4;
            m_objects.insert(
                std::pair<int, DungeonRidableObject>(obj.m_field0c, obj));
        }
    }
