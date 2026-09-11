// clear @ 0x8acf550：逐元素释放 strings_（COW refcount 递减，8acf56a 检查
// empty rep 0x948ccf0 后 _M_destroy）→ finish 重置为 start（8acf57e）→
// string_map_ 整树 _M_erase（8acf58b）→ header left/right=自身、parent=0、
// node_count=0（8acf590-8acf5a0）。标准 strings_.clear(); string_map_.clear();
// 语义完全等价（二者均保留 capacity）。
void ScriptStringManager::clear()
{
    strings_.clear();
    string_map_.clear();
}