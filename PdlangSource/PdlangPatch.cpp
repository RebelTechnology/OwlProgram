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
}

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

  // pdlang's ARM object carries .ARM.exidx entries referencing the EHABI personality routines.
  // OWL patches never unwind (-fno-exceptions), so satisfy them here instead of pulling libgcc's
  // unwinder (and abort/_exit/_kill) into the patch.
#ifdef __arm__
  void __aeabi_unwind_cpp_pr0(void){}
  void __aeabi_unwind_cpp_pr1(void){}
#endif
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

// hvcc's hv_string_to_hash (MurmurHash2, seed = length)
static uint32_t hvHash(const char* s){
  const uint32_t m = 0x5bd1e995; const int r = 24;
  uint32_t len = strlen(s), h = len;
  const uint8_t* d = (const uint8_t*)s;
  while(len >= 4){
    uint32_t k = d[0] | (d[1] << 8) | (d[2] << 16) | ((uint32_t)d[3] << 24);
    k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; d += 4; len -= 4;
  }
  switch(len){
  case 3: h ^= d[2] << 16; // fall through
  case 2: h ^= d[1] << 8;  // fall through
  case 1: h ^= d[0]; h *= m;
  }
  h ^= h >> 13; h *= m; h ^= h >> 15;
  return h;
}

PdlangPatch* PdlangPatch::instance = NULL;

PdlangPatch::PdlangPatch() : nmap(0), buttonsPending(0) {
  instance = this;
  addParameters();
  addLegacyParameters();
  size_t sz = (size_t)pdlang_ctx_size();
  raw = new uint8_t[sz + 15];
  ctx = (void*)(((uintptr_t)raw + 15) & ~(uintptr_t)15);
  pdlang_init(ctx, getSampleRate());
  blocksize = getBlockSize();
  nin = pdlang_num_inputs();
  in = nin > 0 ? new float[nin * blocksize] : NULL;
  out = new float[2 * blocksize];
}

PdlangPatch::~PdlangPatch() {
  instance = NULL;
  delete[] out;
  delete[] in;
  delete[] raw;
}

// the @owl parameters and buttons the patch declares (pdlang_param_info)
void PdlangPatch::addParameters(){
  int count = pdlang_param_info(0, NULL);
  for(int i = 0; i < count && nmap < PDLANG_MAX_PARAMS; ++i){
    pdlang_param_info_t info;
    pdlang_param_info(i, &info);
    if(!info.raw || !info.raw[0])
      continue;
    Map m = {info.hash, NONE, 0, info.min, info.max};
    bool isIn = info.type == 0 || info.type == 2;
    if(parseButton(info.raw, &m.id))
      m.kind = isIn ? BUTTON_IN : BUTTON_OUT;
    else if(parseParam(info.raw, &m.id))
      m.kind = isIn ? PARAM_IN : PARAM_OUT;
    else
      continue;
    if(m.kind == PARAM_IN){
      registerParameter((PatchParameterId)m.id, info.name);
      if(m.max != m.min)
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

// legacy OWL names (c2owl's 'Channel-' prefix): [r Channel-A..E] parameters and
// [r Channel-Push] / [r Channel-B1..B8] buttons, 0..1. Plain receives are not in
// pdlang_param_info, so they cannot be detected: only assumed for a patch that
// declares no @owl parameters at all.
void PdlangPatch::addLegacyParameters(){
  static const char* const legacy[] = {"A", "B", "C", "D", "E", // as the library compiler: A..E
                                       "Push", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8"};
  size_t nlegacy = nmap == 0 ? sizeof(legacy) / sizeof(legacy[0]) : 0;
  for(size_t k = 0; k < nlegacy && nmap < PDLANG_MAX_PARAMS; ++k){
    Map m = {0, NONE, 0, 0.0f, 1.0f};
    if(parseButton(legacy[k], &m.id))
      m.kind = BUTTON_IN;
    else if(parseParam(legacy[k], &m.id))
      m.kind = PARAM_IN;
    char nm[24] = "Channel-";
    strcat(nm, legacy[k]);
    m.hash = hvHash(nm);
    if(m.kind == PARAM_IN){
      strcpy(outnames[nmap], nm);
      registerParameter((PatchParameterId)m.id, outnames[nmap]);
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

extern "C" void pdlang_host_send(void* ctx, uint32_t hash, uint32_t ts, const float* atoms, int32_t n){
  if(PdlangPatch::instance)
    PdlangPatch::instance->hostSend(hash, atoms, n);
}
