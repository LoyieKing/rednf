void STScriptFileList::Clear() { m_fileMap.clear(); }

// ORIG T 0x088bad4e，mangled _ZNK16STScriptFileList11GetFileNameEi：
//   按 index 查 m_fileMap；命中返回 value.c_str()，未命中返回 0。
//   （证据见 docs/class_func_reports/STScriptFileList/GetFileName.md）
