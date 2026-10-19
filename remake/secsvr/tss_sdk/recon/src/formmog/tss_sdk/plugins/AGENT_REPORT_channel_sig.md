# ChannelSig — channel packet struct namespace fix (`send_pkg_to_*` mangling)

Scope: make the three `tss_sdk_impl::TssSdkChannelPlugin::send_pkg_*` mangled names
match the shipped object by moving the `TssSdkPluginChannel*Pkg` family out of
`namespace tss_sdk_impl` to **global** scope. No function body, layout, or call
target was changed.

## 1. Shipped symbols (ORIG `libtss_sdk.so.1.0.394`, raw `nm -D`)

```
0009cba0 T _ZN12tss_sdk_impl19TssSdkChannelPlugin23send_pkg_to_ipc_channelERK32TssSdkPluginChannelIpcChannelPkg
0009ca50 T _ZN12tss_sdk_impl19TssSdkChannelPlugin24send_pkg_to_game_channelERK33TssSdkPluginChannelGameChannelPkg
0009cb50 T _ZN12tss_sdk_impl19TssSdkChannelPlugin26send_pkg_to_socket_channelERK35TssSdkPluginChannelSocketChannelPkg
```

The parameter types are spelled **unqualified** after `RK` (length 32/33/35 =
`TssSdkPluginChannel{Ipc,Socket,Game}ChannelPkg`). If they were nested in
`tss_sdk_impl` the mangling would be `RKN12tss_sdk_impl32TssSdkPluginChannelIpcChannelPkgE`.
`nm -DC` renders the same: `send_pkg_to_ipc_channel(TssSdkPluginChannelIpcChannelPkg const&)`.

## 2. DWARF corroboration (ORIG)

DIE parents in CU `.../src/formmog/tss_sdk/plugins/channel/channel_plugin.cpp`
(enumerated with pyelftools directly on the `.so`):

| DIE | tag | parent | byte_size |
|---|---|---|---|
| `._488` (InitData) | structure_type | compile_unit | 12 |
| `._490` (Head) | structure_type | compile_unit | 4 |
| `._491` (IpcChannelPkg) | structure_type | compile_unit | 8 |
| `._493` (SocketChannelPkg) | structure_type | compile_unit | 10 |
| `._494` (GameChannelPkg) | structure_type | compile_unit | 14 |
| `._495` (Body) | union_type | compile_unit | 14 |
| `._496` (Pkg) | structure_type | compile_unit | 18 |

No `DW_TAG_namespace` parent → global scope. The same seven DIEs hang directly
off the CU in `.../framework/tss_sdk_comm_svc.cpp`. Type *names* were stripped by
the vendor (the DIEs are literally named `._488` …), so scope is proven by
nesting + mangling, not by `DW_AT_name`. Extra corroboration: the
`antibotinterface.cpp` CU spells the template argument unqualified
(`CTssCoreDumpPkgTrace<TssSdkPluginChannelGameChannelPkg>`), while namespaced
types in the same object are fully qualified (e.g. `Singleton<tss_sdk_impl::TssSdkCommSvc>`).

## 3. Edits

| File | Change |
|---|---|
| `recon/src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp` | Moved `enum ETssSdkPluginChannelTypeId` + `TssSdkPluginChannel{InitData,Head,IpcChannelPkg,SocketChannelPkg,GameChannelPkg,Body,Pkg}` (former lines 385-442, inside `namespace tss_sdk_impl`) to global scope immediately before `namespace tss_sdk_impl {`. Header comment now records the mangling/DWARF evidence. Layout-check block (former lines 564-569) no longer writes `tss_sdk_impl::` on those six sizeof terms. |
| `recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_agent.cpp` | Same family (former lines 215-258) moved out of `namespace tss_sdk_impl` to global scope before `namespace tss_sdk_impl {` (now line 157); the two users inside the namespace (`send_buf_to_client` pkg at ~line 519, `send_buf_to_game` pkg at ~line 577) resolve outward to the global types unchanged. |
| `recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_exports_timer_agent.cpp` | No change needed: `TssSdkPluginChannelInitData` was already declared at global scope (line 421), used at line 458. |
| `recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_export_api_agent.cpp` | No change needed: `TssSdkPluginChannelGameChannelPkg` already at global scope (line 116); sizeof check at line 872 unqualified. |

No header in the workbench declares these types (they are TU-local in each of the
four TUs); `grep` over the workbench confirms it.

## 4. Verified semantics of the three functions (unchanged by this edit)

| Function | ORIG addr | size | decoded body |
|---|---:|---:|---|
| `send_pkg_to_ipc_channel` | `0x9cba0` | `0x36` | `m_ipc_channel_.send(pkg.buf_, pkg.buf_size_)` — `CIpcChannel` sits at `this+0x30`, args `+0`, `+4` of the pkg |
| `send_pkg_to_socket_channel` | `0x9cb50` | `0x44` | `SocketChannel::instance()->send_data(pkg.business_id_, pkg.buf_)` (`business_id_ +0`, `buf_ +4`) |
| `send_pkg_to_game_channel` | `0x9ca50` | `0x14` | `this->callback_proc_((void *)&pkg)` — indirect call through the callback pointer at `ITssSdkPlugin+0x04` |

Own-object disassembly of `send_pkg_to_ipc_channel` (`obj_fn.sh /tmp/ch_plugin.o`):
`mov 0x4(%edx),%eax` (buf_size_ +4) / `mov (%edx),%eax` (buf_ +0) pushed as args,
`mov 0x8(%ebp),%eax; add $0x30,%eax` as the `CIpcChannel` `this`, then
`call …CIpcChannel::send(void const*, unsigned int)`. Entry at `0x280`, `ret` at
`0x2b5` → 0x36 bytes, same as ORIG.

## 5. Verification actually run

| Command | Observed |
|---|---|
| `sh c5build.sh recon/src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp /tmp/ch_plugin.o` | rc=0; only the pre-existing `-Wnonnull` offsetof warnings from the layout-check block |
| `nm /tmp/ch_plugin.o`, `nm -D $ORIG` (raw, filtered `send_pkg`) | identical mangled strings for all three functions |
| `sh symdiff.sh $ORIG /tmp/ch_plugin.o "TssSdkChannelPlugin::send_pkg"` | `== ORIG-only ==` empty, `== NEW-only ==` empty, counts 3 / 3 |
| `bash sizecmp.sh /tmp/ch_plugin.o "TssSdkChannelPlugin::send_pkg"` | `matched=3 identical=3 differing=0` |
| `sh c5build.sh recon/src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_agent.cpp /tmp/comm_svc.o` | rc=0 (warnings only) |
| `grep -rn "tss_sdk_impl::TssSdkPluginChannel" tools/tss_sdk_recon` | no matches (rc=1) — whole workbench, including `build/`, `patches/`, reports |
| `cd recon && sh link_check.sh /tmp/tss_recon_all.o` (WSL g++ 13.3, `-m32`, `-D_GLIBCXX_USE_CXX11_ABI=0`) | rc=0 |
| g++ `-O0 -g` DWARF probe of both edited TUs (pyelftools) | channel TU: Head 4, Ipc 8, Socket 10, Game 14, Body 14, Pkg 18, InitData 12; comm_svc TU: same seven; member offsets `buf_ +0 / buf_size_ +4`, `business_id_ +0 / buf_ +4 / buf_size_ +8`, `busi_type_ +0 / uin_ +4 / buf_ +8 / buf_size_ +12`, `head_ +0 / body_ +4`; all `enclosing=None` (global scope) |

Export of the probe: run python3 with `/tmp/lay_comm_svc.o /tmp/lay_channel.o`,
both produced by
`g++ -m32 -std=gnu++98 -O0 -g -fPIC -fno-stack-protector -D_GLIBCXX_USE_CXX11_ABI=0 -I . -I include -c <TU>`.

## 6. Unresolved / out of scope

- The `TssSdkPluginUserControl*` packet family in `tss_sdk_comm_svc_agent.cpp`
  (~lines 140-213) is still inside `tss_sdk_impl`; its ORIG scope was not
  audited here (different ticket).
- ORIG DWARF carries no type names for these DIEs; the reconstructed names come
  from the mangled symbols. Nothing else about the structs is inferred.
- `_GLOBAL__I__…`/static-init ordering and body-level byte identity were not
  re-checked here (untouched by this change).
