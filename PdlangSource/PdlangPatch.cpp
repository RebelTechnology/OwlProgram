#include "PdlangPatch.hpp"
#include <string.h>

// pdlang ctx ABI (pdlang docs/STATE-ABI.md)
extern "C" {
  typedef struct {
    const char *name; uint32_t hash; int32_t type; float min, max, def; const char *raw; const char *vtype;
  } pdlang_param_info_t;
  int32_t pdlang_ctx_size(void);
  void pdlang_init(void* ctx, double sr);
  void pdlang_process(void* ctx, float* out, int32_t n);
  void pdlang_process_in(void* ctx, const float* in, float* out, int32_t n);
  int32_t pdlang_num_inputs(void);
  int32_t pdlang_send_message(void* ctx, uint32_t hash, double delay_ms, const float* atoms, int32_t n);
  int32_t pdlang_param_info(int32_t index, pdlang_param_info_t* info);
  void pdlang_host_send(void* ctx, uint32_t hash, uint32_t ts, const float* atoms, int32_t n);
  // ctx blocks (STATE-ABI "Blocks")
  typedef struct {
    const char *name; uint32_t size; uint32_t align; int32_t access; uint32_t per_sample; uint32_t offset;
  } pdlang_block_info_t;
  int32_t pdlang_block_info(int32_t index, pdlang_block_info_t* info);
  void pdlang_init_blocks(void* const* blocks, double sr);
}
enum { PDLANG_ACCESS_SAMPLE = 0, PDLANG_ACCESS_CONTROL = 1 };

// Weak fallbacks for the entry points only some pdlang patches export. When the patch's object
// defines one, its strong definition wins at link time; otherwise these are used. This keeps every
// symbol resolved on every linker (no weak undefined references).
//  - a patch without adc~ exports pdlang_process and no pdlang_num_inputs / pdlang_process_in;
//  - a patch with adc~ exports pdlang_process_in and pdlang_num_inputs, and no pdlang_process;
//  - a patch without host parameters exports neither pdlang_param_info nor pdlang_send_message.
extern "C" {
  __attribute__((weak)) void pdlang_process(void* ctx, float* out, int32_t n){}
  __attribute__((weak)) void pdlang_process_in(void* ctx, const float* in, float* out, int32_t n){}
  __attribute__((weak)) int32_t pdlang_num_inputs(void){ return 0; }
  __attribute__((weak)) int32_t pdlang_send_message(void* ctx, uint32_t hash, double delay_ms, const float* atoms, int32_t n){ return 0; }
  __attribute__((weak)) int32_t pdlang_param_info(int32_t index, pdlang_param_info_t* info){ return 0; }
  // an object from a pdlang without ctx blocks: one flat allocation. -1, not 0: GCC 14's LTO identical code
  // folding crashes (internal compiler error) when this fallback is identical to pdlang_param_info's.
  __attribute__((weak)) int32_t pdlang_block_info(int32_t index, pdlang_block_info_t* info){ return -1; }
  __attribute__((weak)) void pdlang_init_blocks(void* const* blocks, double sr){}

  // pdlang's ARM object carries .ARM.exidx entries referencing the EHABI personality routines.
  // OWL patches never unwind (-fno-exceptions), so satisfy them here instead of pulling libgcc's
  // unwinder (and abort/_exit/_kill) into the patch.
#ifdef __arm__
  void __aeabi_unwind_cpp_pr0(void){}
  void __aeabi_unwind_cpp_pr1(void){}
#endif
}

// Host maths for -fmath=host (pdlang.mk's default): the object calls pdlang_<libm name> for every transcendental
// function it uses, float and double, and leaves the call undefined. On the device, the functions OWL has kernels
// for go to them: OWL's table-based fast_* (tables installed by the firmware at patch start) and CMSIS's
// arm_sin_f32 / arm_cos_f32, the double versions through float. Everything else, and every native and web build
// (which have no OWL tables), is libm. --gc-sections drops the ones a patch doesn't use.
#undef exp
#undef expf
#undef exp2
#undef exp2f
#undef log
#undef logf
#undef log2
#undef log2f
#undef log10
#undef log10f
#undef pow
#undef powf
#undef sin
#undef sinf
#undef cos
#undef cosf
#define PDLANG_LIBM1(n) \
  float pdlang_##n##f(float x){ return ::n##f(x); } \
  double pdlang_##n(double x){ return ::n(x); }
#define PDLANG_LIBM2(n) \
  float pdlang_##n##f(float x, float y){ return ::n##f(x, y); } \
  double pdlang_##n(double x, double y){ return ::n(x, y); }
#ifdef ARM_CORTEX
#define PDLANG_OWL1(n, k) \
  float pdlang_##n##f(float x){ return k(x); } \
  double pdlang_##n(double x){ return k((float)x); }
#define PDLANG_OWL2(n, k) \
  float pdlang_##n##f(float x, float y){ return k(x, y); } \
  double pdlang_##n(double x, double y){ return k((float)x, (float)y); }
#else
#define PDLANG_OWL1(n, k) PDLANG_LIBM1(n)
#define PDLANG_OWL2(n, k) PDLANG_LIBM2(n)
#endif
extern "C" {
  PDLANG_OWL1(exp, fast_expf) PDLANG_OWL1(exp2, fast_exp2f)
  PDLANG_OWL1(log, fast_logf) PDLANG_OWL1(log2, fast_log2f) PDLANG_OWL1(log10, fast_log10f)
  PDLANG_OWL2(pow, fast_powf)
  PDLANG_OWL1(sin, arm_sin_f32) PDLANG_OWL1(cos, arm_cos_f32)
  PDLANG_LIBM1(expm1) PDLANG_LIBM1(log1p) PDLANG_LIBM1(tan)
  PDLANG_LIBM1(asin) PDLANG_LIBM1(acos) PDLANG_LIBM1(atan) PDLANG_LIBM2(atan2)
  PDLANG_LIBM1(sinh) PDLANG_LIBM1(cosh) PDLANG_LIBM1(tanh)
  PDLANG_LIBM1(asinh) PDLANG_LIBM1(acosh) PDLANG_LIBM1(atanh)
  PDLANG_LIBM1(cbrt) PDLANG_LIBM1(erf) PDLANG_LIBM1(erfc)
}

static bool parseParam(const char* r, uint8_t* id){
  // A..H, AA..DH
  size_t len = strlen(r);
  if(len == 1 && r[0] >= 'A' && r[0] <= 'H'){ *id = PARAMETER_A + (r[0] - 'A'); return true; }
  if(len == 2 && r[0] >= 'A' && r[0] <= 'D' && r[1] >= 'A' && r[1] <= 'H'){
    *id = PARAMETER_AA + (r[0] - 'A') * 8 + (r[1] - 'A'); return true;
  }
  return false;
}

static bool parseButton(const char* r, uint8_t* id){
  if(strcmp(r, "Push") == 0){ *id = PUSHBUTTON; return true; }
  if(strcmp(r, "Green") == 0){ *id = GREEN_BUTTON; return true; }
  if(strcmp(r, "Red") == 0){ *id = RED_BUTTON; return true; }
  if(r[0] == 'B' && r[1] >= '1' && r[1] <= '8' && r[2] == 0){ *id = BUTTON_1 + (r[1] - '1'); return true; }
  return false;
}

PdlangPatch* PdlangPatch::instance = NULL;

PdlangPatch::PdlangPatch() : nmap(0), buttonsPending(0) {
  instance = this;
  addParameters();
  // the audio buffers are touched every sample: allocate them before the ctx's large blocks
  blocksize = getBlockSize();
  nin = pdlang_num_inputs();
  in = nin > 0 ? new float[nin * blocksize] : NULL;
  out = new float[2 * blocksize];
  allocateContext();
}

PdlangPatch::~PdlangPatch() {
  instance = NULL;
  delete[] out;
  delete[] in;
  for(int i = 0; i < nblocks; ++i)
    delete[] blockmem[i];
}

static void* align16(uint8_t* p){
  return (void*)(((uintptr_t)p + 15) & ~(uintptr_t)15);
}

// Allocate the ctx and initialise it. The OWL heap is first fit, fastest memory first (on the Genius: ITCM, DTCM,
// D1, then SDRAM), so allocating in STATE-ABI's order puts the hot state in fast memory and lets only large,
// rarely touched buffers spill to SDRAM: the hot block first, then the control blocks, then the per-sample blocks
// smallest first. An object without blocks gets one flat allocation of pdlang_ctx_size() bytes.
void PdlangPatch::allocateContext(){
#ifdef PDLANG_FLAT_CTX
  int n = 0; // -DPDLANG_FLAT_CTX: one allocation, for comparison
#else
  int n = pdlang_block_info(0, NULL);
#endif
  if(n < 1 || n > PDLANG_MAX_BLOCKS){
    nblocks = 1;
    blockmem[0] = new uint8_t[(size_t)pdlang_ctx_size() + 15];
    ctx = align16(blockmem[0]);
    pdlang_init(ctx, getSampleRate());
    return;
  }
  pdlang_block_info_t info[PDLANG_MAX_BLOCKS];
  int order[PDLANG_MAX_BLOCKS];
  for(int i = 0; i < n; ++i){
    pdlang_block_info(i, &info[i]);
    order[i] = i;
  }
  // block 0 first; then CONTROL before SAMPLE, each smallest first (insertion sort over blocks 1..n-1)
  for(int i = 2; i < n; ++i){
    int b = order[i], j = i;
    while(j > 1){
      int a = order[j - 1];
      bool before = info[b].access != info[a].access ? info[b].access == PDLANG_ACCESS_CONTROL
                                                     : info[b].size < info[a].size;
      if(!before)
        break;
      order[j] = a;
      --j;
    }
    order[j] = b;
  }
  void* bases[PDLANG_MAX_BLOCKS];
  for(int k = 0; k < n; ++k){
    int i = order[k];
    blockmem[k] = new uint8_t[info[i].size + 15];
    bases[i] = align16(blockmem[k]);
  }
  nblocks = n;
  ctx = bases[0];
  pdlang_init_blocks(bases, getSampleRate());
}

// the @owl parameters and buttons the patch declares, and its Channel-* host receives (pdlang_param_info)
void PdlangPatch::addParameters(){
  int count = pdlang_param_info(0, NULL);
  for(int i = 0; i < count && nmap < PDLANG_MAX_PARAMS; ++i){
    pdlang_param_info_t info;
    pdlang_param_info(i, &info);
    const char* raw = info.raw;
    Map m = {info.hash, NONE, 0, info.min, info.max};
    bool isIn = info.type == 0 || info.type == 2;
    if(info.type == 4){
      // a plain receive exposed with --host-receive (pdlang.mk): the OWL names [r Channel-A..E], [r Channel-Push], 0..1
      if(strncmp(info.name, "Channel-", 8) != 0)
        continue;
      raw = info.name + 8;
      m.min = 0.0f;
      m.max = 1.0f;
      isIn = true;
    }
    if(!raw || !raw[0])
      continue;
    if(parseButton(raw, &m.id))
      m.kind = isIn ? BUTTON_IN : BUTTON_OUT;
    else if(parseParam(raw, &m.id))
      m.kind = isIn ? PARAM_IN : PARAM_OUT;
    else
      continue;
    if(m.kind == PARAM_IN){
      registerParameter((PatchParameterId)m.id, info.name);
      if(info.type != 4 && m.max != m.min)
        setParameterValue((PatchParameterId)m.id, (info.def - m.min) / (m.max - m.min));
    }else if(m.kind == PARAM_OUT){
      char* nm = outnames[nmap];
      strncpy(nm, info.name, sizeof(outnames[0]) - 2);
      nm[sizeof(outnames[0]) - 2] = 0;
      strcat(nm, ">");
      registerParameter((PatchParameterId)m.id, nm);
    }
    map[nmap++] = m;
  }
}

void PdlangPatch::buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
  // deferred to processAudio: pdlang_send_message must not run concurrently with process
  if(bid < 32)
    buttonsPending |= 1u << bid;
}

void PdlangPatch::hostSend(uint32_t hash, const float* atoms, int32_t n){
  if(n < 1)
    return;
  for(int i = 0; i < nmap; ++i){
    Map& m = map[i];
    if(m.hash != hash || m.max == m.min)
      continue;
    float v = (atoms[0] - m.min) / (m.max - m.min);
    if(m.kind == PARAM_OUT)
      setParameterValue((PatchParameterId)m.id, v);
    else if(m.kind == BUTTON_OUT)
      setButton((PatchButtonId)m.id, v > 0.5f ? 4095 : 0);
  }
}

void PdlangPatch::processAudio(AudioBuffer& buffer){
  uint32_t pending = buttonsPending;
  buttonsPending &= ~pending;
  for(int i = 0; i < nmap; ++i){
    Map& m = map[i];
    if(m.kind == PARAM_IN){
      float v = getParameterValue((PatchParameterId)m.id) * (m.max - m.min) + m.min;
      pdlang_send_message(ctx, m.hash, 0.0, &v, 1);
    }else if(m.kind == BUTTON_IN && (pending & (1u << m.id))){
      float v = isButtonPressed((PatchButtonId)m.id) ? m.max : m.min;
      pdlang_send_message(ctx, m.hash, 0.0, &v, 1);
    }
  }
  int n = buffer.getSize();
  if(n > blocksize)
    n = blocksize;
  float* left = buffer.getSamples(LEFT_CHANNEL);
  float* right = buffer.getSamples(RIGHT_CHANNEL);
  if(nin > 0){
    for(int c = 0; c < nin; ++c){
      const float* src = c == 0 ? left : c == 1 ? right : NULL;
      for(int i = 0; i < n; ++i)
        in[c * n + i] = src ? src[i] : 0.0f;
    }
    pdlang_process_in(ctx, in, out, n);
  }else{
    pdlang_process(ctx, out, n);
  }
  for(int i = 0; i < n; ++i){
    left[i] = out[i];
    right[i] = out[n + i];
  }
}

// [print] and [print~], and the C externals shim's post / pd_error (STATE-ABI: host-defined, imported only by patches
// that use them): to the OWL debug message
extern "C" {
  typedef struct { uint32_t type; float f; const char* s; } pdlang_atom; // type: 0 bang, 1 float, 2 symbol
  void pdlang_host_print(void* ctx, const char* label, uint32_t ts, const pdlang_atom* atoms, int32_t n){
    if(n > 0 && atoms[0].type == 1)
      debugMessage(label, atoms[0].f);
    else
      debugMessage(n > 0 && atoms[0].type == 2 && atoms[0].s ? atoms[0].s : label);
  }
  void pdlang_host_print_str(void* ctx, uint32_t ts, const char* s){
    debugMessage(s);
  }
}

extern "C" void pdlang_host_send(void* ctx, uint32_t hash, uint32_t ts, const float* atoms, int32_t n){
  if(PdlangPatch::instance)
    PdlangPatch::instance->hostSend(hash, atoms, n);
}
