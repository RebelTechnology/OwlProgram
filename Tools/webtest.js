// Headless smoke test for a web patch build (Build/web/patch.js).
// Loads the module the way WebSource/webaudio.js uses it (global Module,
// HEAP32/HEAPF32, cwrap), runs setup and processes audio blocks.
// usage: node Tools/webtest.js Build/web/patch.js
const fs = require('fs');
const vm = require('vm');

const file = process.argv[2];
const fs_ = 48000, bs = 64, blocks = 200;
// Exit by draining the event loop, not process.exit(): node can deadlock in
// process.exit() on Linux (futex wait in its worker threads).
process.exitCode = 1;
class Fail extends Error {}
const fail = (msg) => { throw new Fail(msg); };
const timer = setTimeout(() => {
  console.error('webtest FAIL: runtime did not initialise');
  process.exit(1);
}, 10000);

globalThis.require = require;
globalThis.__dirname = __dirname;
globalThis.Module = {
  print: (s) => console.log(s),
  printErr: (s) => console.error(s),
  onAbort: (what) => console.error('webtest FAIL: abort: ' + what),
  onRuntimeInitialized: run,
};
vm.runInThisContext(fs.readFileSync(file, 'utf8'), { filename: file });

function run() {
  clearTimeout(timer);
  try {
    test();
    process.exitCode = 0;
  } catch (e) {
    if (!(e instanceof Fail)) throw e;
    console.error('webtest FAIL: ' + e.message);
  }
}

function test() {
  const M = globalThis.Module;
  // webaudio.js uses Module.HEAP32 / Module.HEAPF32
  for (const name of ['HEAP32', 'HEAPF32'])
    if (!M[name]) fail('Module.' + name + ' not exported');
  const HEAP32 = M.HEAP32, HEAPF32 = M.HEAPF32;

  const WEB_setup = M.cwrap('WEB_setup', 'number', ['number', 'number']);
  const WEB_processBlock = M.cwrap('WEB_processBlock', 'number', ['number', 'number']);
  const WEB_setParameter = M.cwrap('WEB_setParameter', 'number', ['number', 'number']);
  const WEB_getPatchName = M.cwrap('WEB_getPatchName', 'string', []);
  const WEB_getParameterName = M.cwrap('WEB_getParameterName', 'string', ['number']);
  const WEB_getStatus = M.cwrap('WEB_getStatus', 'string', []);
  const WEB_getMessage = M.cwrap('WEB_getMessage', 'string', []);
  M.cwrap('WEB_processMidi', 'number', ['number', 'number', 'number', 'number']);
  M.cwrap('WEB_getButtons', 'number', []);
  M.cwrap('WEB_setButton', 'number', ['number', 'number']);
  M.cwrap('WEB_getParameter', 'number', ['number']);

  WEB_setup(fs_, bs);
  const name = WEB_getPatchName();
  if (!name) fail('no patch name');
  const params = [];
  for (let i = 0; i < 8; i++) {
    const p = WEB_getParameterName(i);
    if (p) params.push(p);
    WEB_setParameter(i, 0.5);
  }

  const alloc = (n) => {
    const ptrs = M._malloc(4 * n);
    const bufs = [];
    for (let c = 0; c < n; c++) {
      const b = M._malloc(4 * bs);
      HEAP32[(ptrs >> 2) + c] = b;
      bufs.push(b);
    }
    return [ptrs, bufs];
  };
  const [ins, inBufs] = alloc(2);
  const [outs, outBufs] = alloc(2);

  let peak = 0, t = 0;
  for (let k = 0; k < blocks; k++) {
    for (const b of inBufs)
      for (let i = 0; i < bs; i++)
        HEAPF32[(b >> 2) + i] = 0.5 * Math.sin(2 * Math.PI * 440 * (t + i) / fs_);
    t += bs;
    WEB_processBlock(ins, outs);
    for (const b of outBufs)
      for (let i = 0; i < bs; i++) {
        const v = HEAPF32[(b >> 2) + i];
        if (!Number.isFinite(v)) fail('non-finite output in block ' + k);
        peak = Math.max(peak, Math.abs(v));
      }
  }
  const status = WEB_getStatus();
  if (/error/i.test(status)) fail('status: ' + status);
  console.log(`webtest OK: "${name}" params [${params.join(', ')}] ` +
              `${blocks}x${bs} samples, peak ${peak.toFixed(3)}, status "${status}"` +
              (WEB_getMessage() ? `, message "${WEB_getMessage()}"` : ''));
}
