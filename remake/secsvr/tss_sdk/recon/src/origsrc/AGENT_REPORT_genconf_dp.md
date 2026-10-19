# CU `src/protocol/formmog/formmog_conf_tsssdk_dp.cpp` — reconstruction report

ORIG: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394` (tag 394),
DWARF CU name `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_conf_tsssdk_dp.cpp`.
CU code range `0x000eebd0`–`0x0010160a`, 228 DWARF-attributed `tss_dp_comm::` functions plus the
two out-of-line weak `tsf4g_tdr` definitions the CU emits
(`TdrXmlWriter::textize` `0x000ee1b0`, `TdrOutStream::textize` `0x000ee1f0`, 0x40 bytes each) = 230 symbols.

## Deliverables

| file | content |
|---|---|
| `recon/src/origsrc/formmog_conf_tsssdk_dp.h` | 19 `#pragma pack(1)` configuration structures + the 12 member functions of each + compile-time ABI assertions (sizes and member offsets, all DWARF values) |
| `recon/src/origsrc/formmog_conf_tsssdk_dp.cpp` | the 230 symbols: per class `construct`, `fromXmlFile`, `fromXmlBuffer`, `fromXmlString`, `fromXml`, `entryFromXml`, `toXmlFile(const char*)`, `toXmlFile(FILE*)`, `toXmlBuffer`, `toXml`, `entryToListXml`, `entryToAttrXml` |
| `recon/src/origsrc/tsf4g_tdr_compat_cfg.h` | XML runtime ABI model (`TdrXmlFormat`, `rapidxml::{xml_base,xml_attribute,xml_node,memory_pool,xml_document}`, `TdrOutStream`, `TdrXmlFile`, `TdrXmlReader`, `TdrXmlWriter`, `TdrParse`) + the `TDR_ERR_INVALID_ROOT_NODE` code (compensation, see below) |
| `recon/src/origsrc/genconf_dp_matrix.tsv` | `addr, size_orig, size_recon, ratio, name` for all 230 symbols |

`tsf4g_tdr_compat.h` (shared) is included, never edited. No other agent's file was touched.

## Method

1. `cu_symbols.py <ORIG> formmog_conf_tsssdk_dp.cpp --sizes` → the authoritative 230-symbol list
   (`/tmp/gcdp/syms.txt`, copied into the report's numbers below).
2. `python3 cu_layout.py <ORIG> formmog_conf_tsssdk_dp.cpp` → class/member/offset/size of the CU's own
   DWARF: DP classes *and* the XML runtime they use, so the compat header carries the ORIG's real
   numbers, not a guess: `TdrOutStream{_fp+0,_buf+4,_state+20}=24`,
   `TdrXmlReader{_buffer+0,_format+4,_state+8,_ducument+12,_currRoot+65616,_currNode+65620}=65624`,
   `TdrXmlWriter{_out+0,_format+24,_state+28,_isSimple+32,_isPartial+33,_indentLvl+36,_indentStr[33]+40}=76`,
   `TdrXmlFile{_buffer+0,_state+4}=8`, `rapidxml::xml_base<char>=20`, `xml_node<char>=48`,
   `memory_pool<char>=65556`, `xml_document<char>=65604`.
3. `python3 dwarf_rets.py` (return types) and the PLT call set of the whole CU range
   (`objdump -d` + `grep @plt`) → every external function the CU calls, with its exact demangled
   signature: `TdrXmlReader::{ctor,state,stepIn,stepOut,getEntryValue,getNodeValue}`,
   `TdrXmlWriter::{ctor,openSimple,closeSimple,openComplex,closeComplex,textizeStr,textize}`,
   `TdrOutStream::{textize,vtextize}`, `TdrXmlFile::load`,
   `TdrParse::{parseUInt8,parseUInt16,parseUInt32}` (value and array-pointer overloads),
   `memset/strlen/strncpy/fopen/fclose/operator delete[]`.
4. Per function: annotated disassembly (`orig_fn.sh` / `dis_annotate.py`) read field by field;
   the writer bodies were read with a literal+call filter (`fmt.py`), the readers in full.
5. The TU is emitted by a table-driven generator (`gen_cfg.py`, kept outside the repo) whose tables hold
   one row per field: `(kind, name, missing-entry default, parse default)` for scalars,
   `(size, default literal)` for `char[N]`, count field + capacity for arrays, child class for
   nested objects. Every row is a value read out of the ORIG code, not a copy of the reference corpus.

## Recovered generated-code semantics (all measured)

* Reader, scalar field: `value4X = reader.getEntryValue("X"); if (value4X == NULL) this->X = <missing-default>; else { ret = TdrParse::parseUIntN(this->X, value4X, NULL, <parse-default>, NULL, NULL); if (ret != TDR_NO_ERROR) return ret; }`.
  The two defaults differ exactly where the ORIG differs: for a scalar that is an *array count field*
  the missing-entry default is the capacity (`length` 8/128/32, `lists_count` 4, `fragment_count` 128,
  `cmd_list_buf_size` 10240, `code_dir_count` 2, `drop_subcmd_count`/`forward_subcmd_count` 8) while the
  `parseUIntN` `_default` argument is 0 — measured in `DPValueArray_8::entryFromXml`
  (`movw $0x8,(%esi)` on the NULL path, `movl $0x0,0xc(%esp)` before the call).
* Reader, count clamp: inside the non-NULL branch of every count field,
  `if (this->count > capacity) return TDR_ERR_REFER_SURPASS_COUNT;` (`cmp; ja` → `0xfffffff9`).
* Reader, array of scalars: `value = reader.getNodeValue("name"); if (NULL) { this->count = 0; } else { tempCount = 0; ret = TdrParse::parseUIntN(this->array, this->count, value, &tempCount, 0, NULL, NULL); if (ret) return ret; this->count = tempCount; }`.
  Fixed-size arrays without a count field (`hack_list[8]`, `actions[8]`) pass `NULL` for `parsed` and the
  array extent as the count, and their NULL path is `memset(this->array, 0, sizeof(this->array))`.
* Reader, array of objects (`hack_fragments`, `cmd_lists`, `drop_subcmd_list`, `forward_subcmd_list`):
  capacity clamp, `if (count != 0)`, then
  `if (reader.stepIn(name) != WS_NORMAL) { this->count = i; return ret; }`,
  `ret = this->array[i].entryFromXml(reader, 1); if (ret) return ret; reader.stepOut(name);`,
  with the element name held in the `value4name` local (measured: the literal is loaded once, before the loop).
* Reader, `char[N][M]` (`code_dir_path[2][256]`): `reader.getNodeValue(name)` *inside* the loop, NULL →
  `this->count = i; return ret;`, `strlen(value) > M-1` → `TDR_ERR_STR_LEN_TOO_BIG`, `strncpy(dst[i], value, M)`.
* Reader, nested object: `if (reader.stepIn("name") != WS_NORMAL) { ret = this->name.construct(); if (ret) return ret; } else { ret = this->name.entryFromXml(reader, 1); if (ret) return ret; reader.stepOut("name"); }`
  (cutVer is the literal 1, measured `movl $0x1,0x8(%esp)`).
* Reader, string: `strlen(value) > N-1` → `TDR_ERR_STR_LEN_TOO_BIG`, else `strncpy(this->X, value, N)`;
  missing entry → `strncpy(this->X, "<default>", N)`.
* Writer, list mode: element open `if (writer._isPartial) { ret = writer._out.textize(">\n%s<%s> ", writer._indentStr, "name"); writer._isPartial = false; writer._state = ret; if (writer._state != TDR_NO_ERROR) return writer._state; } else { …"%s<%s> "… }`,
  then `writer.textize("0x%02x "|"%d "|"%u ", this->X)` (uint8/uint16/uint32) or `writer.textizeStr(this->X, true)`,
  then `ret = writer._out.textize("</%s>\n", "name"); writer._state = ret; if (…) return writer._state;`.
* Writer, attr mode: scalars and strings first (field order) as ` %s="` + `"0x%02x"|"%d"|"%u"` /
  `textizeStr(this->X, false)` + `"`, then the array/object fields in field order — measured on
  `DPCmdHack::entryToAttrXml` (`safemode_to_client`, `safemode_hint_client`, `cf_room_qtalk_enable`,
  `drop_subcmd_count`, `forward_subcmd_count` before `delay_cmd`, `up_cmd_hack`, …) and on
  `DPCodeInfo::entryToAttrXml` (`cmd_list_buf_size`, `code_dir_count` before the buffers).
* Arrays are elements in *both* formats; nested objects use `openComplex(name)` /
  `child.entryToXml(writer, 1)` / `closeComplex(name)` with a `ret != TDR_NO_ERROR → return ret` check after each.
* `construct()`: measured per class — the classes whose defaults are all zero compile to a single
  `memset(this, 0, sizeof(<Class>))` (`DPCodeInfo` 14884, `DPCmdLists` 2064, `DPHackFilter` 2050,
  `DPValueArray_128` 514, `DPBanMode` 130, `DPDropCmd` 42, `DPSubCmdList` 42, `DPValueArray_8` 34,
  `DPHackFragment` 16, `DPGameCmdStruct` 2 — the small ones expand to `rep stos`/word stores);
  the others keep per-field code (`DPCmdHack` 7100 with `DPSubCmdList` array loops, `DPConfig`,
  `DPLogConf` with `strncpy(bill_path, "../log/", 256)` and `strncpy(monitor_address, "10.129.139.156", 32)`
  which GCC 4.1 folds into immediate stores — the default string is therefore *not* a `.rodata` literal in
  the ORIG, and the same spelling here produces the same store sequence; verified by size 145/145).
* Wrappers: `fromXmlFile` → `TdrXmlFile xmlFile; ret = xmlFile.load(file);`, `fromXmlBuffer`/`fromXmlString`
  → local `TdrXmlReader`, `ret = reader.state()`, `fromXml(reader, cutVer)`; `fromXml` →
  `ret = TDR_ERR_INVALID_ROOT_NODE; if (reader.stepIn("<Class>") == WS_NORMAL) { ret = entryFromXml(reader, cutVer); if (ret == TDR_NO_ERROR) reader.stepOut("<Class>"); }`;
  `toXml*` → `writer.state()` check, `writer._out.textize("<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n", "GBK")`,
  `*usedSize = writer.getUsedSize()`, `fopen(file, "wb")` + `fclose(fp)`;
  `toXml` → `openSimple("<Class>")`, `format == ATTR_ENTRY ? entryToAttrXml : entryToListXml`, `closeSimple("<Class>")`.
* The two weak `textize` bodies are spelled inline in `tsf4g_tdr_compat_cfg.h` so this TU emits them
  (`_state = _out.vtextize(format, ap);` / `_state = vtextize(format, ap);`) — sizes 64/64, ratio 1.000.

## Verification (all commands re-runnable)

| command | result |
|---|---|
| `sh c5build.sh recon/src/origsrc/formmog_conf_tsssdk_dp.cpp /tmp/gcdp/gcdp.o -I recon/src/origsrc` | rc=0, no diagnostics; all ABI assertions in the header pass |
| symbol diff over the CU list (`cu_symbols.py` + `nm -C -S --defined-only`) | 230/230 present, **0 missing, 0 unexpected** (only `DW.ref.__gxx_personality_v0`, which the ORIG itself carries as a local hidden symbol, `readelf -sW` line 1549) |
| `bash sizecmp.sh /tmp/gcdp/gcdp.o "tss_dp_comm"` | `matched=228 identical=115 differing=113`; bands over all 230: exact 117, ≤3 % 51, 3–12 % 24, **>12 % 38** (all `fromXmlBuffer`/`fromXmlString`, see divergences) |
| literal check (`.rodata` symbols of the object vs the ORIG image) | 125 distinct literals, **0 absent from ORIG** |
| undefined-reference set (`nm -u` vs `nm -D` of the ORIG) | 29 undefined, all 20 `tsf4g_tdr::`/`tss_dp_comm::` references resolve against ORIG dynamic symbols; the remaining 9 are libc/libstdc++/libgcc (`memset`, `strlen`, `strncpy`, `fopen`, `fclose`, `operator delete[]`, `_Unwind_Resume`, `__gxx_personality_v0`, `_GLOBAL_OFFSET_TABLE_`) |
| **per-function literal multiset** (ORIG annotated disassembly vs the object linked into a `.so` and annotated the same way) | **228/228 functions identical** — every generated function references exactly the same XML names/format strings, with the same multiplicity |
| per-function literal *first-occurrence order* | 211/228 identical; the 17 differences are all the same benign block placement inside one list element (see divergences) |
| `seqdiff.py` on `DPGameCmdStruct::fromXmlBuffer` | instruction sequences identical modulo register naming (127 ORIG vs 123 recon instructions) |
| `python3 genconf_dp_matrix.tsv` | written: 230 rows |

## Divergences and compensations

1. **38 outliers, all `fromXmlBuffer`/`fromXmlString` (ratio 0.822 / 0.853).** Measured cause, not a
   missing behaviour: the ORIG (GCC 4.1.0) spills the `ret` local into the frame slot and tail-duplicates
   the inlined `~memory_pool` loop, GCC 4.1.2 keeps `ret` in a callee-saved register and emits one loop
   body. Region measurement on `DPGameCmdStruct::fromXmlBuffer` (ORIG `0xf5600`–`0xf56c5` vs recon
   `0x0dfc0`–`0x0e04c`): ORIG 58 instructions / 198 bytes incl. `movl $0xffffffed,-0x1007c(%ebp)` +
   `mov -0x1007c(%ebp),%eax`, recon 46 instructions / 139 bytes; EH landing pad 45/155 vs 39/118.
   `seqdiff.py` shows the same instruction sequence otherwise. Both `fromXml*` shapes that reproduce the
   generated idiom (nested `if`, early `return`) compile to the same 370 bytes here, so this is not
   source-controllable; reported rather than faked. `toXmlFile(const char*)` is 122/112 (0.918, inside
   tolerance) for the same reason (`ret` spill).
2. **17 list-mode literal-order differences** (e.g. `DPValueArray_8::entryToListXml`): ORIG places the
   non-pending `"%s<%s> "` block directly after the pending `">\n%s<%s> "` block, recon places the value
   `textize()` in between. Same literals, same multiplicity, same branch structure
   (`cmpb $0x0,0x21(%esi); je` non-pending for both), only the block layout differs.
3. **`TDR_ERR_INVALID_ROOT_NODE`** is used by `fromXml` (ORIG constant `0xffffffe4` = −28). The shared
   `tsf4g_tdr_compat.h` models only the first 21 `TdrError::ErrorType` codes and is fixed, so
   `tsf4g_tdr_compat_cfg.h` declares the ORIG's own enumerator name and value
   (`const TdrError::ErrorType TDR_ERR_INVALID_ROOT_NODE = -28;`, documented there). The `.cpp` spells the
   bare name (the qualified `TdrError::` spelling cannot be added to the frozen header).
4. **Writer member spelling**: the CU's DWARF names the pending-indent flag `_isPartial` at +33
   (the secagent corpus calls the same slot `_indentPending`); the model and the generated code use the
   DWARF name of this CU.
5. The 12 `DPConfig`-family objects are emitted in the ORIG's own source order
   (`construct`, wrappers, `entryFromXml`, `entryToListXml`, `entryToAttrXml`); the object's symbol order
   is irrelevant for the size/literal checks above.

## Notes for the integrator

* `tsf4g_tdr_compat_cfg.h` only *declares* the XML runtime entry points; their definitions belong to
  `TdrXml.cpp`/`TdrIO.cpp`/`TdrParse.cpp` (the reference tree's `src/protocol/common/*` is ABI-compatible,
  note `TdrXmlFile` gains `_state` at +4 in this tag). The two `textize` bodies are weak (`W`), so they
  coexist with a sibling CU emitting the same weak copies (confirmed compatible with `GenConf`'s CU).
* `memset/fopen/fclose/strncpy/strlen` and the EH helpers stay undefined in the object exactly as in the
  ORIG's compilation unit.
