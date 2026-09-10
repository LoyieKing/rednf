bool ServerParameterScript::CheckUnusableEquipmentInfo(ENUM_UNUSABLE_EQUIPMENT_TYPE type, int idx)
{
    for (std::vector<stUnusableEquipmentInfo>::iterator it =
             m_unusableEC.begin();
         it != m_unusableEC.end(); ++it)
    {
        stUnusableEquipmentInfo* p = &*it;
        // b28d-L3：ORIG 0x08568ac0 段 `mov -0xc,%eax; mov (%eax),%eax;
        // cmp 0xc(%ebp),%eax`（field0 入 eax、type 内存直比）⇒ 比较方向交换。
        // b28d-solo-ServerParameterScript: 去冗余 cast 复现 mem 直比；守卫改
        // 正极 `field4!=0 || field8!=0`（ORIG `jne CHECK` + `je FALSE` 混合形，
        // 德摩根 `&&` 形在 4.4 恒双 jne）。
        // 残差（N 封顶，59 vs 64）：① type 比较 4.4 恒先载形参（`mov 0xc(%ebp),%edx;
        // cmp %eax,%edx`）vs ORIG mem 直比（`cmp 0xc(%ebp),%eax`）——P209 unsigned
        // 形参族；② field4>idx 与 field8>=idx 两处 return 的共享尾块（ORIG `jg TRUE`
        // 与 fall-through 共用 `mov $1`）4.4 恒就地发射——N93附 尾部双 return 块序。
        if (type == p->m_field0)
        {
            // b28c-L8 语义修正（ORIG 0x08568af6 逐条）：field4!=0 时 ORIG 经
            // `jne +0x4f` 落入**同一段** field4/field8 判定（非跳过），旧源码把
            // 该段误置于 field4==0 分支内 ⇒ field4>idx 且 field8>=idx 时分叉。
            // CFG：b!=0→CHECK；b==0&&c==0→false；CHECK: b>idx→true；
            //     c>=idx→false；否则 true。
            if (p->m_field4 != 0 || p->m_field8 != 0)
            {
                // [b28d-solo-C04] P47 命中：ORIG 0x08568b45..0x08568b67 为
                // `cmp; jg <mov $1>; cmp; jge <mov $0>; <共享 mov $1 块>`（TRUE 块
                // 由首条件与 fall-through **共用**）。if-return 双写法 4.4 恒就地
                // 发射两个 mov $1（+2）；嵌套三目
                // `a ? true : (b ? false : true)` 逐条复现 ORIG 共享形
                // （探针 /tmp/p1/probe5.cpp g1 与 ORIG 段全等，g2/g3/g4 均劣）。
                // 零值守卫并入同一表达式（probe8 k2）实测**反劣**（62→70：bool
                // 返回下 4.4 复制守卫链并多发 mov $0），已回退；残余 +2 为
                // 4.4 不共享零值守卫的 `mov $0`（N93附 尾部块序）。
                // [C04 · Main C05② 细化口径（2026-09-10）] 本件另一条残差
                // （`cmp 0xc(%ebp),%eax` ORIG mem 直比 vs 4.4 先载 %edx）属
                // **enum 形参比较**子型 ⇒ 按细化口径「形参比较（enum/底层 signed）
                // 不适用 C05②」归 N18b/P209 **封顶，勿再试**（probe3 d1..d8 +
                // probe4 e1..e4 共 8 形已穷举；唯一可行旋钮=字段同型化
                // `m_field0` int→enum（probe3 d1 实证）只能 62→61 且须改共享头
                // ServerParameterScript.h，收益/成本不成立，留档待 Main 裁决）。
                return (p->m_field4 > idx)
                           ? true
                           : ((p->m_field8 >= idx) ? false : true);
            }
            return false;
        }
    }
    return true;
