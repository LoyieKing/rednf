# GenConf — `formmog_conf_tsssdk_conf.cpp` + `formmog_conf_tsssdk_conf_path.cpp`

Two CUs of the TDR XML configuration generator of `libtss_sdk.so.1.0.394`
(tag 394, ORIG compiled by `GCC 4.1.0 (SUSE Linux)` per `readelf -p .comment`;
the recon toolchain is the vendor `c5root g++ 4.1.2 20080704 (Red Hat 4.1.2-55)`):

* `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_conf_tsssdk_conf.cpp`
  — 74 function symbols = 72 `tss_sdk_conf_desc::` (6 classes × 12) +
  the CU's weak `TdrXmlWriter::textize` @ `0x000ee1b0` and
  `TdrOutStream::textize` @ `0x000ee1f0` (0x40 each).
* `/data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag394/src/protocol/formmog/formmog_conf_tsssdk_conf_path.cpp`
  — 13 function symbols = 12 `tss_sdk_conf_path_desc::path_config::` +
  the same weak `TdrOutStream::textize` @ `0x000ee1f0`.

## Artifacts (new files, no commits)

| file | lines | content |
|---|---|---|
| `recon/src/origsrc/formmog_conf_tsssdk_conf.cpp` | 1839 | the conf TU (74 definitions) |
| `recon/src/origsrc/formmog_conf_tsssdk_conf.h` | 396 | 6 classes, `#pragma pack(1)`, 60 `sizeof`/`offsetof` assertions |
| `recon/src/origsrc/formmog_conf_tsssdk_conf_path.cpp` | 202 | the path TU (13 definitions) |
| `recon/src/origsrc/formmog_conf_tsssdk_conf_path.h` | 71 | `path_config`, 2 layout assertions |
| `recon/src/origsrc/tsf4g_tdr_compat_cfg_conf.h` | 442 | this CU's ABI model of the XML runtime (`TdrXml.h`/`TdrIO.h`/`TdrParse.h` + the `rapidxml.hpp` revision they pull in) |
| `recon/src/origsrc/genconf_matrix.tsv` | 88 | 87 symbols, addr/size/ratio/name |

`recon/src/origsrc/tsf4g_tdr_compat.h` (shared binary-TDR model) and
`formmog_conf_tsssdk_dp.*` (the sibling DP CU) were **not** touched; the XML
runtime got its own header, so no concurrent edits were needed.

## Symbol table used

`python3 cu_symbols.py <ORIG> formmog_conf_tsssdk_conf.cpp --sizes` →
74 rows, `..._path.cpp` → 13 rows; both recorded in `genconf_matrix.tsv`
(`nm` verified: 0 missing, 0 extra, 0 size mismatch).

`cu_layout.py` / `dwarf_full_extract.py` on the two CUs gave the class layouts
that the headers assert:

```
AddrInfo          38  : plugin_id_ +0, ip_[32] +4, port_ +36
CommConfig      1226  : use_ipc_ +0, use_socket_ +4, server_num_ +8,
                        server_addrs_info_[32] +10 (38 bytes each)
LogConfig          4  : log_level_ +0
UserControlconfig  8  : use_idx_ +0, max_client_obj_count_ +4
UicConfig        277  : use_backend_check_ +0, user_input_code_ +4 (int8),
                        master_tss_uic_ip_[32] +5, master_tss_uic_port_ +37,
                        slave_tss_uic_ip_[32] +41, slave_tss_uic_port_ +73,
                        max_var_len_queue_len_ +77, max_fix_len_queue_len_ +81,
                        use_dirty_file_flag_ +85, dirty_file_[128] +89,
                        bus_id_ +217 (uint32), queue_timeout_ +221,
                        connect_timeout_ +225, connect_intval_ +229,
                        recv_timeout_ +233, send_timeout_ +237,
                        max_recv_count_onetime_ +241,
                        max_send_count_onetime_ +245,
                        max_recv_time_gap_onetime_ +249,
                        max_send_time_gap_onetime_ +253, stop_timeout_ +257,
                        tcp_max_recv_buf_len_ +261, tcp_max_send_buf_len_ +265,
                        send_intval_ +269 (uint32), recv_intval_ +273 (uint32)
config          1515  : comm_config_ +0, log_config_ +1226,
                        usercontrol_config_ +1230, uic_config_ +1238
path_config     1024  : config_path[1024] +0
```

The packing is required (DWARF puts `server_addrs_info_` at +10 and
`log_config_` at +1226).

## What the ORIG emits (recovered rules)

* `construct()` — literal defaults only: LogConfig `3`;
  UserControlconfig `1/20000`; UicConfig `1, 0, ip[0]=0, 0, ip[0]=0, 0,
  1228800, 1200, 0, dirty_file_[0]=0, 0, 300, 3000, 5, 0, 50, 1000, 1000, 100,
  100, 5, 64000, 64000, 500, 500` (the two `[0]=0` stores are a single
  `movb`, i.e. no `strncpy` for empty defaults); AddrInfo `0` + inline
  `strncpy(ip_, "127.0.0.1", 32)` + `0`; CommConfig `use_ipc_=1`,
  `use_socket_=0`, `server_num_=0` + `AddrInfo::construct()` ×32;
  `config` chains the four sub-`construct()`s; `path_config` is
  `memset(config_path, 0, 1024)`.
* `entryFromXml()` — one `reader.getEntryValue("<name>")` per field:
  missing value re-applies the *construct* default (CommConfig::server_num_
  falls back to `32`, not 0), present scalar value goes to
  `TdrParse::parseInt8/parseInt32/parseUInt16/parseUInt32(dest, value, NULL,
  default, NULL, NULL)`; char arrays are
  `strlen(value) > sizeof(array)-1 → TDR_ERR_STR_LEN_TOO_BIG` then
  `strncpy(dest, value, sizeof(array))`; nested structs are
  `stepIn(name) == WS_NORMAL ? entryFromXml + stepOut : construct()`
  (`config`, names `comm_config_`, `log_config_`, `usercontrol_config_`,
  `uic_config_`, cutVer 1); the `AddrInfo[32]` array is
  `server_num_ > 32 → TDR_ERR_REFER_SURPASS_COUNT`, then
  `unsigned short i` 0..`server_num_` with `stepIn/entryFromXml/stepOut`,
  writing `server_num_ = i` and returning when a node is missing.
* `entryToAttrXml()` — `writer.openAttr("<name>")` / value /
  `writer.closeAttr("<name>")` (the close call carries the name but the body
  only emits `"`), formats `%d` (int8/int16/int32) and `%u`
  (`bus_id_`, `send_intval_`, `recv_intval_`), char arrays via
  `writer.textizeStr(array, false)`, each call checked as
  `if (ret != TDR_NO_ERROR) return ret;`.
* `entryToListXml()` — same field order with `writer.openNode`/`closeNode`,
  formats `%d `/`%u `, strings via `writer.textizeStr(array, true)`, and —
  the distinguishing ORIG detail — the tag writers are checked through the
  writer state: the ORIG emits
  `mov %eax,0x1c(%esi); mov 0x1c(%esi),%eax; test %eax,%eax` at every
  openNode/closeNode site, i.e. the generated code is
  `if (writer.state() != TDR_NO_ERROR) return writer.state();`
  (the ATTR sites only emit `test %eax,%eax; mov %eax,0x1c(%esi)`).
* `toXml()` — `<root>` via `openSimple/closeSimple` for the four leaf classes
  (AddrInfo, LogConfig, UicConfig, UserControlconfig) and
  `openComplex/closeComplex` for `config` and `CommConfig`; then
  `format == ATTR_ENTRY ? entryToAttrXml : entryToListXml`, cutVer forwarded.
* `fromXml` family / `toXml*` family — the generator wrappers verbatim:
  `TDR_ERR_NULL_POINTER_PARAMETER` (-19) guards,
  `TdrXmlFile` + `load()/content()` for files, `TdrXmlReader` + `state()` for
  buffers/strings, `TDR_ERR_INVALID_ROOT_NODE` (-28) root `stepIn/stepOut`,
  `TdrXmlWriter` + `state()`, `declare("GBK")`,
  `*usedSize = writer.getUsedSize()` when `usedSize != NULL`, and
  `TDR_ERR_OPEN_FILE_WRITE_FAILED` (-23) around `fopen(file, "wb")`/`fclose`.

Root element names: `config`, `CommConfig`, `AddrInfo`, `LogConfig`,
`UicConfig`, `UserControlconfig`, `path_config`.
Field names (= XML element/attribute names) are exactly the member names with
the `_` suffix, except `path_config`'s single field `config_path`.

### XML runtime model (`tsf4g_tdr_compat_cfg_conf.h`)

The CU never contains the runtime — it calls it — so the model only needs the
ABI and the inlined helpers. Class layouts are the CU's DWARF
(`TdrXmlFile` 8, `TdrOutStream` 24, `TdrXmlReader` 65624, `TdrXmlWriter` 76,
`rapidxml::xml_base<char>` 20 / `xml_node<char>` 48 / `memory_pool<char>`
65556 / `xml_document<char>` 65604). The inlined helpers are exactly the ones
the CU records as `DW_TAG_inlined_subroutine`: `TdrXmlFile`
{ctor, dtor, `content`/`buf`}, `TdrXmlReader` {dtor, `state`},
`TdrXmlWriter` {`declare`, `openNode`, `closeNode`, `openAttr`, `closeAttr`,
`state`, `getUsedSize`}, `rapidxml::{clear, init, ~memory_pool,
~xml_document}`. Their bodies were derived from the ORIG's inlined expansions
(argument order, `writer._state` updates, the pending-indent
`">\n%s<%s> "` vs `"%s<%s> "` branch on `_indentStr`/`_isPartial`), and
`memory_pool::clear()/init()/align()` are the rapidxml 1.13 bodies the ORIG
inlines into `~TdrXmlReader` (delete[] `_buffer`, block walk with the
`(4 - (p & 3)) & 3` alignment, then `m_ptr`/`m_end` re-init).
Only symbols that the ORIG itself imports are referenced; mangled names were
checked 1:1 against the ORIG (see Verification 6).

## Verification performed

1. **Build** — `sh c5build.sh recon/src/origsrc/formmog_conf_tsssdk_conf.cpp
   <out> -I recon/src/origsrc` and the same for `..._path.cpp`: rc=0, no
   diagnostics, i.e. every `sizeof`/`offsetof` assertion in both headers holds.
2. **CU symbol coverage** — 74/74 (conf) and 13/13 (path) symbols emitted,
   0 missing, 0 extra, 0 size mismatch.
3. **Symbol/size diff vs ORIG** — scoped `nm -C` diff over
   `tss_sdk_conf_desc::|TdrXmlWriter::textize(char const*, ...)|TdrOutStream::textize(char const*, ...)`
   and `tss_sdk_conf_path_desc::|TdrOutStream::textize(char const*, ...)`:
   ORIG-only = ∅, NEW-only = ∅, `matched=74 identical=74 differing=0` and
   `matched=13 identical=13 differing=0`; the weak/strong binding of the two
   `textize` copies is also `W` as in the ORIG.
4. **Instruction-level equality** — for all 87 functions the ORIG and recon
   instruction streams are equal after normalising only call targets, branch
   targets and literal-pool references (immediates, registers, operands and
   ordering preserved): `functions=74 exact=74 differing=0`,
   `functions=13 exact=13 differing=0`. This covers the inlined
   reader/writer/pool/`strncpy` expansions, the landing pads and the
   `TdrXmlFile` EH path.
5. **Literals** — every literal in the two objects' `.rodata.str1.1`/
   `.rodata.str1.4` is PRESENT in the ORIG (`strfind.sh`; 0 MISSING), including
   the multi-line ones checked byte-wise: `">\n%s<%s> "`, `"</%s>\n"`,
   `"<?xml version=\"1.0\" encoding=\"%s\" standalone=\"yes\" ?>\n"`,
   `" %s=\""`, `"\""`, `"%s<%s> "`, `"%d"`, `"%d "`, `"%u"`, `"%u "`, `"GBK"`,
   `"wb"`, `"127.0.0.1"`.
6. **Imports** — every undefined `tsf4g_tdr::` symbol of both objects is
   defined by the ORIG `.so` (0 unresolved; checked both demangled and by
   mangled name, e.g. `TdrParse::parseInt8(signed char&, ...)` exists with that
   exact signature).
7. **Link smoke test** — `g++ -m32 -shared -nostdlib` over both objects: links
   cleanly, the duplicated weak `TdrOutStream::textize` collapses to one `W`
   symbol (the same COMDAT situation as the ORIG, where the symbol is
   attributed to both CUs).

`genconf_matrix.tsv` therefore has ratio `1.000` on all 87 rows; there are no
size outliers, so no per-symbol ORIG cause had to be explained.

## Divergences / compensations (all documented in the sources)

1. `TdrXmlFile::loadState(file)` — the ORIG's generated code stores the
   imported `TdrXmlFile::load()` result into `_state` at every call site
   (`mov %eax,4(%ebx)` right after the PLT call, and `TdrXmlFile`'s DWARF has
   `_state` at +4); that store came from an inline `TdrIO.h` wrapper which the
   ORIG inlined everywhere, so its name is not recoverable. It is modelled as
   an inline wrapper of that name in
   `tsf4g_tdr_compat_cfg_conf.h`; it is fully inlined, so no such symbol
   reaches the objects (verified: 0 extra symbols).
2. `TDR_ERR_OPEN_FILE_WRITE_FAILED` (-23) and `TDR_ERR_INVALID_ROOT_NODE`
   (-28) are namespace-level `static const TdrError::ErrorType` constants in
   the compat header, because the shared `tsf4g_tdr_compat.h` enumerates
   `ErrorType` only down to -20 and must not be edited. Same values, same
   code (they are compile-time int constants).
3. `TdrWriteBuf` is reused from the shared `tsf4g_tdr_compat.h`. Its member
   names there (`data_/pos_/limit_/allocated_`) differ from this CU's DWARF
   (`beginPtr/position/length/needFree`) while the 16-byte layout and the
   `~TdrWriteBuf → _free()` inline expansion are identical, so the emitted
   writer destructor code matches the ORIG byte for byte.
4. `TdrXmlWriter::openNode/closeNode/openAttr/closeAttr/declare` and
   `TdrXmlReader::~TdrXmlReader` are declaration-level models: the ORIG never
   emits them (they only appear as inlined subroutines), so their bodies are
   reconstructed to produce the ORIG's inlined instruction sequences, not taken
   from a header.
5. `-fno-implement-inlines` in `c5build.sh` does not suppress the two weak
   `textize` copies, because GCC cannot inline a variadic call; they are
   declared inline in the modelled headers exactly where the ORIG has them
   (`TdrXml.h:209`, `TdrIO.h:93`).
6. No source-level `throw()` compensations were needed: EH frame content is
   identical (the landing pads of `fromXmlString`/`fromXmlBuffer`/
   `toXmlBuffer`/`toXmlFile` are part of the instruction-level comparison).

## Commands (reproducible)

```sh
cd tools/tss_sdk_recon
python3 cu_symbols.py $ORIG formmog_conf_tsssdk_conf.cpp --sizes
python3 cu_symbols.py $ORIG formmog_conf_tsssdk_conf_path.cpp --sizes
python3 cu_layout.py  $ORIG formmog_conf_tsssdk_conf.cpp onfig
python3 cu_layout.py  $ORIG formmog_conf_tsssdk_conf_path.cpp
sh c5build.sh recon/src/origsrc/formmog_conf_tsssdk_conf.cpp      /tmp/genconf.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_conf_tsssdk_conf_path.cpp /tmp/genpath.o -I recon/src/origsrc
sh symdiff.sh $ORIG /tmp/genconf.o 'tss_sdk_conf_desc::|tsf4g_tdr::Tdr(Xml|Out)'
sh symdiff.sh $ORIG /tmp/genpath.o 'tss_sdk_conf_path_desc::|tsf4g_tdr::TdrOut'
sh sizecmp.sh /tmp/genconf.o 'tss_sdk_conf_desc::'
sh sizecmp.sh /tmp/genpath.o 'tss_sdk_conf_path_desc::'
sh strfind.sh < <literals extracted from the two .o files>
```
(`$ORIG` = `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`;
`symdiff.sh`/`sizecmp.sh` write fixed files under `/tmp`, which was occupied by
root-owned files during this session, so the identical comparisons were run
with a private copy of both scripts that redirects those temporaries to a
writable directory — the regexes and outputs quoted above are the same.)
