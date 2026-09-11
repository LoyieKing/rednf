// getString(int,string*) @ 0x8acf160：先 _M_mutate(0,len,0) 清空输出串
// （8acf194），再 assign(strings_[index])（8acf1aa），成功返回 true。
// ORIG 的负 index 分支检查是死代码（负 index 恒满足 index<=size 而走
// 越界 assign，UB），此处按 DWARF 意图以越界返回 false 作护栏；
// outputString==NULL 亦为 ORIG UB，护栏返回 false。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（44 vs 44 条，p167 CLEAR）。
// 残差 = ①装载寄存器角色互换（ORIG `mov 0x4(%edi),%eax; lea (%eax,%esi,4),%eax`
// vs 我方 %ecx 中转）；②`xor %eax,%eax`（false 常量）提前物化（ORIG 在 epilogue
// 前）。实测旋钮均不翻：纯 register 声明（P231）、const 引用局部、具名 count 中转。
bool ScriptStringManager::getString(int index, std::string* outputString) const
{
    outputString->clear();  // ORIG: _M_mutate(0, length, 0)
    // [b23S3C] ORIG 0x8acf160：正索引无界检查直 assign(start[index])；
    // 负索引仅 (int)size()<index 死检查（js 分离形）。勿加 unsigned 界保护改行为。
    if (index < 0)
    {
        if (static_cast<int>(strings_.size()) < index)
            return false;
    }
    outputString->assign(strings_[index]);  // [b23S3C] begin() 形实测反劣化（+2 out-of-line），保 operator[]
    return true;
}