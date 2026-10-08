const $=s=>document.querySelector(s);
const KIND={engine:'模型',connect:'连接',list:'上架',buy:'收购',deal:'成交',expire:'到期',error:'错误',scan:'扫描',orphan:'孤儿邮件',control:'控制',protocol:'协议'};
const LV={debug:'调试',info:'信息',warn:'警告',error:'错误'};
const SIDE={human_buy_bot:'玩家买系统货',bot_buy_human:'模型买玩家挂单',human_human:'玩家间成交',virtual:'虚拟成交'};
const STATUS={active:'在售',sold:'已售出',expired:'已到期'};
function ts(t){if(!t)return '-';const d=new Date(t*1000);const p=n=>String(n).padStart(2,'0');return `${d.getFullYear()}-${p(d.getMonth()+1)}-${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}:${p(d.getSeconds())}`}
function ago(t){if(!t)return '-';const s=Math.round(Date.now()/1000-t);if(s<60)return s+' 秒前';if(s<3600)return Math.round(s/60)+' 分钟前';if(s<86400)return (s/3600).toFixed(1)+' 小时前';return (s/86400).toFixed(1)+' 天前'}
async function api(p,body){const r=await fetch('/api/'+p,body?{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)}:{});const j=await r.json();if(j&&j.error)throw new Error(j.error);return j}
function table(el,cols,rows,rowCls){let h='<tr>'+cols.map(c=>`<th>${c[0]}</th>`).join('')+'</tr>';
  for(const r of rows){h+=`<tr class="${rowCls?rowCls(r):''}">`+cols.map(c=>{const v=c[1](r);return `<td class="${c[2]||''}">${v==null?'-':v}</td>`}).join('')+'</tr>'}
  if(!rows.length)h+=`<tr><td colspan="${cols.length}" class="hint">无数据</td></tr>`;el.innerHTML=h}
function card(k,v){return `<div class="card"><div class="k">${k}</div><div class="v">${v}</div></div>`}
let curTab='overview';
document.querySelectorAll('#tabs a').forEach(a=>a.onclick=()=>{document.querySelectorAll('#tabs a').forEach(x=>x.classList.remove('on'));a.classList.add('on');
  document.querySelectorAll('.tab').forEach(x=>x.classList.remove('on'));$('#tab-'+a.dataset.tab).classList.add('on');curTab=a.dataset.tab;load(curTab)});
function load(t){({overview:loadOverview,engine:loadEngine,logs:()=>loadLogs(),market:loadWatch,listings:loadListings,orphans:loadOrphans,config:loadConfig}[t]||(()=>{}))()}

// ---------- 概览 ----------
const logCols=[['时间',r=>ts(r.ts)],['级别',r=>LV[r.level]||r.level],['类型',r=>KIND[r.kind]||r.kind],['内容',r=>esc(r.msg),'msg']];
async function loadOverview(){try{const s=await api('status');
  const c=$('#conn');c.textContent=s.connected?(s.ready?'已连接拍卖服':'已连接，等待就绪'):'未连接';c.className='pill '+(s.connected&&s.ready?'ok':'bad');
  $('#ov-cards').innerHTML=card('拍卖服连接',s.connected?(s.service_available?'正常':'服务不可用'):'断开')+card('连接时长',s.connected_at?ago(s.connected_at).replace('前',''):'-')+
    card('自动上架',s.paused?'已暂停':(s.seller_enabled?'运行中':'关闭'))+card('自动收购',s.buyer_blocked?'已强制关闭':(s.buyer_enabled?(s.paused?'已暂停':'运行中'):'关闭（默认）'))+
    card('孤儿邮件清理',s.orphan_enabled?(s.orphan_dry_run?'演练模式':'实删模式'):'关闭')+card('近1小时上架',s.listings_last_hour)+
    card('上次行情扫描',s.last_scan?ago(s.last_scan.ts)+'（'+s.last_scan.listings+' 条）':'-')+card('系统卖家显示名',esc(s.npc_name||'-'))+
    card('收发包',`${s.stats.tx} / ${s.stats.rx}`)+card('超时/重连',`${s.stats.timeouts} / ${s.stats.reconnects}`)+(s.db_error?card('数据库错误',esc(s.db_error)):'');
  table($('#ov-items'),[['道具ID',r=>r.item_id],['名称',r=>esc(r.name)],['基础单价',r=>fmtN(r.base_price),'n'],['目标挂单数',r=>r.target_listings,'n'],
    ['在售挂单',r=>r.active,'n'],['在售数量',r=>fmtN(r.active_qty||0),'n'],['状态',r=>r.disabled?'<span style="color:#ef5350">停用：'+esc(r.disabled)+'</span>':'正常']],s.seller_items);
  table($('#ov-deals'),[['道具',r=>esc(r.name)+' ('+r.item_id+')'],['成交笔数',r=>r.deals,'n'],['成交量',r=>fmtN(r.volume),'n'],['成交额',r=>fmtN(r.turnover),'n'],['最低单价',r=>fmtN(r.low),'n'],['最高单价',r=>fmtN(r.high),'n']],s.overview);
  const lg=await api('logs?limit=15');table($('#ov-logs'),logCols,lg,r=>'lv-'+r.level);
}catch(e){$('#conn').textContent='控制台错误：'+e.message;$('#conn').className='pill bad'}}
document.querySelectorAll('[data-act]').forEach(b=>b.onclick=async()=>{try{await api('control',{action:b.dataset.act});loadOverview()}catch(e){alert(e.message)}});

// ---------- 经济模型 ----------
async function loadEngine(){const e=await api('engine');if(!e.enabled){$('#en-cards').innerHTML=card('经济模型','未开启');return}
  const T=e.today,C=e.caps;
  $('#en-cards').innerHTML=card('参与道具',e.items.length)+card('总量系数',e.volume_scale)+card('步进间隔',e.step_minutes.join('~')+' 分钟')+
    card('上一步',e.last_step_at?ago(e.last_step_at):'-')+card('下一步',e.next_step_at?Math.max(0,Math.round((e.next_step_at-Date.now()/1000)/60))+' 分钟后':'-')+
    card('今日上架',T.listings)+card('今日虚拟成交',`${T.virtual_trades} / ${C.virtual_per_day} 次`)+card('虚拟成交量/额',fmtN(T.virtual_qty)+' / '+fmtN(T.virtual_gold))+
    card('模型买玩家挂单',`${T.user_fills} 次 / ${fmtN(T.user_gold)} 金（日上限 ${fmtN(C.user_gold_per_day)}）`+(C.user_fill_enabled?'':'（未开启）'))+card('买玩家挂单规则','由模型决定；护栏：日上限 '+fmtN(C.user_gold_per_day)+'，单价 ≤ 价格带上限×1.25')+
    card('玩家买系统货',T.human_buys+' 次')+card('今日新增孤儿邮件(估)',T.orphan_rows_est+' 行');
  table($('#en-items'),[['道具',r=>esc(r.name)+' ('+r.item_id+')'],['类别',r=>esc(r.category||'')],['参考价',r=>fmtN(r.ref),'n'],['价格带',r=>r.band?fmtN(r.band[0])+'~'+fmtN(r.band[1]):'-'],
    ['公允价',r=>fmtN(r.fair),'n'],['偏离',r=>r.fair&&r.ref?((r.fair/r.ref-1)*100).toFixed(1)+'%':'-','n'],['世界库存',r=>fmtN(r.stock),'n'],['库存/产出(小时)',r=>r.cover_h!=null?r.cover_h.toFixed(1):'-','n'],
    ['稀缺度',r=>r.scarcity!=null?r.scarcity.toFixed(2):'-','n'],['最近成交价',r=>fmtN(r.last),'n'],['系统挂单',r=>r.sys_listings+' 条 / '+fmtN(r.sys_qty),'n'],['玩家挂单',r=>r.user_listings,'n'],
    ['虚拟成交',r=>r.virtual_trades_today+' 次 / '+fmtN(r.virtual_today),'n'],['买玩家挂单',r=>fmtN(r.user_fill_today),'n'],['玩家买系统货',r=>fmtN(r.human_buy_today),'n'],
    ['状态',r=>r.disabled?'<span style="color:#ef5350">'+esc(r.disabled)+'</span>':'正常']],e.items);
  $('#en-items').querySelectorAll('tr').forEach((tr,i)=>{if(i>0){tr.style.cursor='pointer';tr.onclick=()=>{location.hash='market/'+e.items[i-1].item_id;$('#mk-q').value=e.items[i-1].item_id;document.querySelector('#tabs a[data-tab="market"]').click();loadMarket()}}});
  table($('#en-steps'),[['时间',r=>ts(r.ts)],['dt(分钟)',r=>(r.dt_h*60).toFixed(1),'n'],['虚拟在线',r=>r.online,'n'],['上架',r=>r.new_done+'/'+r.new_planned,'n'],
    ['虚拟成交',r=>r.vf_done+'/'+r.vf_planned,'n'],['买玩家挂单',r=>r.uf_done+'/'+r.uf_planned,'n'],['花费金币',r=>fmtN(r.user_gold),'n'],['错误',r=>r.errors,'n'],['耗时(s)',r=>(r.elapsed||0).toFixed(1),'n']],e.steps);
  table($('#en-skipped'),[['道具',r=>esc(r.name)+' ('+r.item_id+')'],['原因',r=>esc(r.reason)]],e.skipped.concat(e.excluded.filter(x=>!e.skipped.find(y=>y.item_id===x.item_id))))}

// ---------- 日志 ----------
let lgMin=null;
async function loadLogs(more){const p=new URLSearchParams({kind:$('#lg-kind').value,level:$('#lg-level').value,q:$('#lg-q').value,limit:200});
  if(more&&lgMin)p.set('before',lgMin);const rows=await api('logs?'+p);if(rows.length)lgMin=rows[rows.length-1].id;
  if(more){const t=document.createElement('table');table(t,logCols,rows,r=>'lv-'+r.level);[...t.querySelectorAll('tr')].slice(1).forEach(tr=>$('#lg-table').appendChild(tr))}
  else table($('#lg-table'),logCols,rows,r=>'lv-'+r.level)}
$('#lg-go').onclick=()=>loadLogs();$('#lg-more').onclick=()=>loadLogs(true);

// ---------- 行情 ----------
let mkItem=null;
async function loadWatch(){const w=await api('watch');$('#mk-watch').innerHTML=w.map(x=>`<span class="${x.seller?'seller':''}" data-id="${x.item_id}" title="成交 ${x.deals} 笔">${esc(x.name)}</span>`).join('');
  $('#mk-watch').querySelectorAll('span').forEach(s=>s.onclick=()=>{$('#mk-q').value=s.dataset.id;loadMarket()});
  if(!mkItem&&w.length){const s=w.find(x=>x.seller)||w[0];$('#mk-q').value=s.item_id;loadMarket()}}
$('#mk-q').oninput=async()=>{const q=$('#mk-q').value.trim();if(q.length<1||/^\d+$/.test(q))return;try{const r=await api('items?q='+encodeURIComponent(q));
  $('#mk-dl').innerHTML=r.map(x=>`<option value="${x.item_id}">${esc(x.name)}</option>`).join('')}catch(e){}};
$('#mk-go').onclick=()=>loadMarket();
async function loadMarket(){let q=$('#mk-q').value.trim();if(!q)return;if(!/^\d+$/.test(q)){const r=await api('items?q='+encodeURIComponent(q));if(!r.length){alert('找不到物品');return}q=r[0].item_id}
  mkItem=+q;const bucket=+$('#mk-bucket').value;const d=await api(`market?item_id=${q}&bucket=${bucket}&days=${$('#mk-days').value}`);
  $('#mk-title').textContent=`${d.name}（${d.item_id}）`+(d.excluded?`  —— 排除名单：${d.excluded}，机器人不买卖`:'');const S=d.summary,C=d.current||{};
  $('#mk-cards').innerHTML=card('参考价',fmtN(d.base_price))+card('模型公允价',fmtN(d.fair_value))+(d.ref?card('参考区间',fmtN(d.ref.low)+' ~ '+fmtN(d.ref.high)):'')+
    card('当前最低挂单',fmtN(C.min))+card('在售数量',fmtN(C.supply)+(C.bot_supply!=null?'（系统 '+fmtN(C.bot_supply)+'）':''))+card('挂单条数',C.listings||0)+
    card('成交笔数',S.deals)+card('成交量',fmtN(S.volume))+card('成交均价(VWAP)',fmtN(S.vwap))+card('成交算术均价',fmtN(S.avg));
  const B=d.buckets;
  const base=d.base_price&&B.length?[[B[0].t,d.base_price],[B[B.length-1].t+bucket,d.base_price]]:[];
  lineChart($('#ch-trend'),[{name:'最低挂单价',color:'#7986cb',points:d.scans.map(s=>[s.ts,s.min]),step:true},
    {name:'成交均价 VWAP',color:'#ef5350',points:B.filter(b=>b.vwap).map(b=>[b.t+bucket/2,b.vwap])},
    {name:'挂单中位数',color:'#f0c674',points:B.filter(b=>b.ask_box).map(b=>[b.t+bucket/2,b.ask_box.p50])},
    {name:'模型公允价',color:'#81c784',points:(d.fair_series||[]).map(x=>[x.ts,x.fair]),dash:true},
    {name:'参考价',color:'#888',points:base,dash:true}]);
  klineChart($('#ch-kline'),B.filter(b=>b.deal_ohlc||b.ask_ohlc).map(b=>b.deal_ohlc?Object.assign({t:b.t,v:b.volume,src:'deal'},b.deal_ohlc):Object.assign({t:b.t,v:0,src:'ask'},b.ask_ohlc)),bucket);
  boxChart($('#ch-box'),B.filter(b=>b.ask_box).map(b=>Object.assign({t:b.t},b.ask_box)),bucket);
  lineChart($('#ch-supply'),[{name:'总在售数量',color:'#4db6ac',points:d.scans.map(s=>[s.ts,s.supply]),step:true,area:true},
    {name:'系统在售数量',color:'#f0c674',points:d.scans.map(s=>[s.ts,s.bot_supply]),step:true}],{zero:true,height:200});
  const qn={'5':'P5','10':'P10','25':'P25（下四分位）','50':'P50（中位数）','75':'P75（上四分位）','90':'P90','95':'P95'};
  table($('#mk-q-deal'),[['分位',r=>r[0]],['成交单价',r=>fmtN(r[1]),'n']],Object.entries(S.q||{}).map(([k,v])=>[qn[k]||k,v]));
  table($('#mk-q-ask'),[['分位',r=>r[0]],['挂单单价',r=>fmtN(r[1]),'n']],Object.entries(C.q||{}).map(([k,v])=>[qn[k]||k,v]));
  table($('#mk-deals'),[['时间',r=>ts(r.ts)],['类型',r=>SIDE[r.side]||r.side],['数量',r=>fmtN(r.count),'n'],['单价',r=>fmtN(r.unit_price),'n'],['总价',r=>fmtN(r.total),'n'],['拍卖号',r=>r.auction_id]],[...d.deals_list].reverse())}

// ---------- 检索 ----------
$('#sr-go').onclick=doSearch;$('#sr-q').onkeydown=e=>{if(e.key==='Enter')doSearch()};
async function doSearch(){$('#sr-info').textContent='检索中…';try{const r=await api(`search?q=${encodeURIComponent($('#sr-q').value)}&mode=${$('#sr-mode').value}`);
  $('#sr-info').textContent=`${r.mode==='live'?'实时协议检索':'数据库'}：${r.rows.length} 条`+(r.ids&&r.ids.length?'，道具 ID：'+r.ids.join(','):'')+(r.elapsed!=null?`，耗时 ${r.elapsed}s`:'')+'。黄色行 = 系统（机器人）挂单。';
  table($('#sr-table'),[['拍卖号',r=>r.auction_id],['道具ID',r=>r.item_id],['名称',r=>esc(r.name)+(r.upgrade?' +'+r.upgrade:'')],['数量',r=>fmtN(r.count),'n'],
    ['单价',r=>fmtN(r.unit_price),'n'],['一口价',r=>fmtN(r.instant_price),'n'],['竞拍价',r=>r.price==-1?'仅一口价':fmtN(r.price),'n'],
    ['卖家',r=>esc(r.owner_name||'')+(r.owner_id!=null?' ('+r.owner_id+')':'')],['剩余',r=>r.expire_time?ago(r.expire_time).replace('前','后').replace(/^-/,''):(r.expire_hours!=null?r.expire_hours+' 小时内':'-')]],r.rows,x=>x.is_bot?'bot':'')
}catch(e){$('#sr-info').textContent='检索失败：'+e.message}}

// ---------- 系统挂单/成交 ----------
$('#bl-go').onclick=loadListings;
async function loadListings(){const r=await api('bot_listings?status='+$('#bl-status').value);
  table($('#bl-table'),[['拍卖号',r=>r.auction_id],['名称',r=>esc(r.name)+' ('+r.item_id+')'],['初始数量',r=>r.count0,'n'],['剩余',r=>r.count_now,'n'],['单价',r=>fmtN(r.unit_price),'n'],
    ['总价',r=>fmtN(r.instant_price),'n'],['上架时间',r=>ts(r.listed_at)],['到期时间',r=>ts(r.expire_at)],['状态',r=>STATUS[r.status]||r.status]],r);
  const d=await api('deals?limit=200');table($('#dl-table'),[['时间',r=>ts(r.ts)],['类型',r=>SIDE[r.side]||r.side],['道具',r=>esc(r.name)+' ('+r.item_id+')'],['数量',r=>fmtN(r.count),'n'],
    ['单价',r=>fmtN(r.unit_price),'n'],['总价',r=>fmtN(r.total),'n'],['卖家',r=>r.seller_id],['买家',r=>r.buyer_id],['来源',r=>({snapshot:'挂单快照',history:'拍卖记录',bot:'机器人'}[r.source]||r.source)]],d)}

// ---------- 孤儿邮件 ----------
$('#or-refresh').onclick=loadOrphans;
$('#or-run').onclick=async()=>{if(!confirm('确定执行孤儿邮件清理？（若配置为演练模式，只会记录日志，不会删除）'))return;try{const r=await api('orphans/cleanup',{});alert(r.msg+`（postal ${r.postal} 条，letter ${r.letters} 条）`);loadOrphans()}catch(e){alert(e.message)}};
async function loadOrphans(){try{const r=await api('orphans');$('#or-cid').textContent=r.charac_no;
  $('#or-state').innerHTML=r.enabled?(r.dry_run?'当前：<b>演练模式</b>（不会删除）':'当前：<b style="color:#ef5350">实删模式</b>'):'当前：<b>已关闭</b>（执行也只是演练）';
  table($('#or-postal'),[['postal_id',r=>r.postal_id],['时间',r=>ts(+r.ts)],['发件人',r=>esc(r.send_charac_name)+' ('+r.send_charac_no+')'],['物品',r=>r.item_id&&r.item_id!='0'?esc(r.item_name)+' ('+r.item_id+') ×'+r.add_info:'-'],['金币',r=>fmtN(+r.gold),'n'],['letter_id',r=>r.letter_id],['拍卖号',r=>r.auction_id]],r.postal);
  table($('#or-letter'),[['letter_id',r=>r.letter_id],['时间',r=>ts(+r.ts)],['发件人',r=>esc(r.send_charac_name)],['内容',r=>esc(r.letter_text),'msg']],r.letters)}catch(e){$('#or-state').textContent='读取失败：'+e.message}}

// ---------- 配置 ----------
async function loadConfig(){const r=await api('config');$('#cf-path').textContent=r.path;$('#cf-text').textContent=r.text}

loadOverview();
setInterval(()=>{if(curTab==='overview')loadOverview();else if(curTab==='engine'){loadEngine();loadOverview_conn()}else if(curTab==='logs'&&$('#lg-auto').checked)loadLogs();else loadOverview_conn()},10000);
async function loadOverview_conn(){try{const s=await api('status');const c=$('#conn');c.textContent=s.connected?(s.ready?'已连接拍卖服':'已连接，等待就绪'):'未连接';c.className='pill '+(s.connected&&s.ready?'ok':'bad')}catch(e){}}
if(location.hash.length>1){const [tb,arg]=location.hash.slice(1).split('/');if(tb==='market'&&arg){$('#mk-q').value=arg;mkItem=+arg;loadMarket()}const a=document.querySelector(`#tabs a[data-tab="${tb}"]`);if(a)a.click()}
