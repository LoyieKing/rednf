// 极简 SVG 图表（无外部依赖）：折线、K 线+成交量、箱线、面积
(function(){
const W=1100, PADL=70, PADR=20, PADT=10, PADB=26;
function esc(s){return String(s).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]))}
function fmtN(v){if(v==null||isNaN(v))return '-';v=Math.round(v);return v.toLocaleString('zh-CN')}
function fmtT(t,span){const d=new Date(t*1000);const p=n=>String(n).padStart(2,'0');
  return span>2*86400?`${p(d.getMonth()+1)}-${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}`:`${p(d.getHours())}:${p(d.getMinutes())}`}
function niceTicks(lo,hi,n){if(!(hi>lo)){hi=lo+1;lo=Math.max(0,lo-1)}const step0=(hi-lo)/n;const mag=Math.pow(10,Math.floor(Math.log10(step0)));
  const st=[1,2,2.5,5,10].map(m=>m*mag).find(s=>s>=step0)||step0;const out=[];for(let v=Math.ceil(lo/st)*st;v<=hi+1e-9;v+=st)out.push(v);return out}
function frame(H,x0,x1,y0,y1,yFmt){
  const sx=t=>PADL+(t-x0)/(x1-x0||1)*(W-PADL-PADR), sy=v=>PADT+(1-(v-y0)/(y1-y0||1))*(H-PADT-PADB);
  let g='';for(const v of niceTicks(y0,y1,5)){const y=sy(v);g+=`<line x1="${PADL}" x2="${W-PADR}" y1="${y}" y2="${y}" stroke="#2a2e37"/><text x="${PADL-6}" y="${y+4}" fill="#889" font-size="11" text-anchor="end">${(yFmt||fmtN)(v)}</text>`}
  const span=x1-x0;const nt=8;for(let i=0;i<=nt;i++){const t=x0+span*i/nt;const x=sx(t);g+=`<text x="${x}" y="${H-8}" fill="#889" font-size="11" text-anchor="middle">${fmtT(t,span)}</text>`}
  return {sx,sy,g};
}
function empty(el,msg){el.innerHTML=`<div class="hint" style="padding:20px">${msg||'暂无数据'}</div>`}
function legend(items){return items.map((it,i)=>`<g transform="translate(${PADL+10+i*150},${PADT+4})"><rect width="12" height="3" y="5" fill="${it.color}"/><text x="16" y="10" fill="#bbc" font-size="11">${esc(it.name)}</text></g>`).join('')}

// series: [{name,color,points:[[t,v],...],dash,step}]
window.lineChart=function(el,series,opt){opt=opt||{};const H=opt.height||260;
  const pts=series.flatMap(s=>s.points.filter(p=>p[1]!=null));if(!pts.length)return empty(el);
  let x0=Math.min(...pts.map(p=>p[0])),x1=Math.max(...pts.map(p=>p[0]));if(x1===x0)x1=x0+60;
  let y0=Math.min(...pts.map(p=>p[1])),y1=Math.max(...pts.map(p=>p[1]));const pad=(y1-y0)*0.08||y1*0.05||1;y0=opt.zero?0:Math.max(0,y0-pad);y1+=pad;
  const f=frame(H,x0,x1,y0,y1);let s=`<svg viewBox="0 0 ${W} ${H}">${f.g}`;
  for(const se of series){const p=se.points.filter(q=>q[1]!=null);if(!p.length)continue;
    let d='';p.forEach((q,i)=>{const x=f.sx(q[0]),y=f.sy(q[1]);if(se.step&&i)d+=`H${x}`;d+=(i?'L':'M')+x+','+y});
    if(se.area){s+=`<path d="${d}L${f.sx(p[p.length-1][0])},${f.sy(y0)}L${f.sx(p[0][0])},${f.sy(y0)}Z" fill="${se.color}" opacity="0.18"/>`}
    s+=`<path d="${d}" fill="none" stroke="${se.color}" stroke-width="1.6" ${se.dash?'stroke-dasharray="5,4"':''}/>`;
    if(p.length<80)for(const q of p)s+=`<circle cx="${f.sx(q[0])}" cy="${f.sy(q[1])}" r="2.5" fill="${se.color}"><title>${esc(se.name)} ${fmtT(q[0],x1-x0)}  ${fmtN(q[1])}</title></circle>`}
  s+=legend(series)+'</svg>';el.innerHTML=s};

// candles: [{t,o,h,l,c,v,src}]
window.klineChart=function(el,candles,bucket){const H=320,VH=80;if(!candles.length)return empty(el);
  const x0=candles[0].t-bucket/2,x1=candles[candles.length-1].t+bucket*1.5;
  let y0=Math.min(...candles.map(c=>c.l)),y1=Math.max(...candles.map(c=>c.h));const pad=(y1-y0)*0.08||y1*0.05||1;y0=Math.max(0,y0-pad);y1+=pad;
  const f=frame(H-VH,x0,x1,y0,y1);const vmax=Math.max(1,...candles.map(c=>c.v||0));
  const bw=Math.max(2,Math.min(18,(W-PADL-PADR)/((x1-x0)/bucket)*0.65));
  let s=`<svg viewBox="0 0 ${W} ${H}">${f.g}`;
  for(const c of candles){const x=f.sx(c.t+bucket/2);const up=c.c>=c.o;const col=c.src==='ask'?(up?'#7986cb':'#9575cd'):(up?'#ef5350':'#26a69a');
    const yo=f.sy(c.o),yc=f.sy(c.c);
    s+=`<g><title>${fmtT(c.t,x1-x0)} ${c.src==='ask'?'(挂单)':'(成交)'}\n开 ${fmtN(c.o)} 高 ${fmtN(c.h)} 低 ${fmtN(c.l)} 收 ${fmtN(c.c)}\n成交量 ${fmtN(c.v||0)}</title>`+
      `<line x1="${x}" x2="${x}" y1="${f.sy(c.h)}" y2="${f.sy(c.l)}" stroke="${col}"/>`+
      `<rect x="${x-bw/2}" y="${Math.min(yo,yc)}" width="${bw}" height="${Math.max(1,Math.abs(yc-yo))}" fill="${col}"/></g>`;
    if(c.v){const vh=(c.v/vmax)*(VH-14);s+=`<rect x="${x-bw/2}" y="${H-PADB+12-vh}" width="${bw}" height="${vh}" fill="#546e7a"><title>成交量 ${fmtN(c.v)}</title></rect>`}}
  s+=`<text x="${PADL-6}" y="${H-PADB+10}" fill="#889" font-size="11" text-anchor="end">量 ${fmtN(vmax)}</text>`;
  s+=legend([{name:'成交 涨/跌',color:'#ef5350'},{name:'挂单（无成交）',color:'#7986cb'},{name:'成交量',color:'#546e7a'}])+'</svg>';el.innerHTML=s};

// boxes: [{t,min,p25,p50,p75,max,mean}]
window.boxChart=function(el,boxes,bucket){const H=280;boxes=boxes.filter(b=>b&&b.min!=null);if(!boxes.length)return empty(el);
  const x0=boxes[0].t-bucket/2,x1=boxes[boxes.length-1].t+bucket*1.5;
  let y0=Math.min(...boxes.map(b=>b.min)),y1=Math.max(...boxes.map(b=>b.max));const pad=(y1-y0)*0.08||y1*0.05||1;y0=Math.max(0,y0-pad);y1+=pad;
  const f=frame(H,x0,x1,y0,y1);const bw=Math.max(3,Math.min(22,(W-PADL-PADR)/((x1-x0)/bucket)*0.6));
  let s=`<svg viewBox="0 0 ${W} ${H}">${f.g}`;
  for(const b of boxes){const x=f.sx(b.t+bucket/2);
    s+=`<g><title>${fmtT(b.t,x1-x0)}\n最高 ${fmtN(b.max)}\n上四分位 ${fmtN(b.p75)}\n中位数 ${fmtN(b.p50)}\n下四分位 ${fmtN(b.p25)}\n最低 ${fmtN(b.min)}\n均值 ${fmtN(b.mean)}</title>`+
    `<line x1="${x}" x2="${x}" y1="${f.sy(b.max)}" y2="${f.sy(b.min)}" stroke="#90a4ae"/>`+
    `<line x1="${x-bw/3}" x2="${x+bw/3}" y1="${f.sy(b.max)}" y2="${f.sy(b.max)}" stroke="#90a4ae"/><line x1="${x-bw/3}" x2="${x+bw/3}" y1="${f.sy(b.min)}" y2="${f.sy(b.min)}" stroke="#90a4ae"/>`+
    `<rect x="${x-bw/2}" y="${f.sy(b.p75)}" width="${bw}" height="${Math.max(1,f.sy(b.p25)-f.sy(b.p75))}" fill="#37474f" stroke="#90a4ae"/>`+
    `<line x1="${x-bw/2}" x2="${x+bw/2}" y1="${f.sy(b.p50)}" y2="${f.sy(b.p50)}" stroke="#f0c674" stroke-width="2"/>`+
    `<circle cx="${x}" cy="${f.sy(b.mean)}" r="2" fill="#4fc3f7"/></g>`}
  s+=legend([{name:'中位数',color:'#f0c674'},{name:'均值',color:'#4fc3f7'},{name:'四分位箱 / 极值',color:'#90a4ae'}])+'</svg>';el.innerHTML=s};
window.fmtN=fmtN;window.esc=esc;
})();
