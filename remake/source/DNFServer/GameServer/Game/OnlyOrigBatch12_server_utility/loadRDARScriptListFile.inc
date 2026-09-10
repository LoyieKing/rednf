bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);

// ===========================================================================
// server_utility::get_rand_int  ORIG T 0x0808eafc：
//   range<0 → 0；range==0 → rand()；r=rand() 若 r<=range 走 LCG 三拍
//   （x = x*0x41c64e6d+0x3039，每拍取 (x/65536) 掩码位拼 c），
//   否则 rand()%range。
// （本文件首强实体 ⇒ GI 键名，ORIG TU = ServerUtility.cpp）
// ===========================================================================
