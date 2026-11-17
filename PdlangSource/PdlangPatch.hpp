// Generic OWL host for any pdlang-compiled patch (ctx ABI, pdlang docs/STATE-ABI.md).
// Mirrors hvcc's c2owl wrapper (HeavyOWL.hpp):
//  - [r X @owl A..H/AA..DH]  : parameter in, getParameterValue()*(max-min)+min every block
//  - [r X @owl B1..B8/Push]  : button in, max/min on press/release
//  - [s X @owl A..H/...]     : parameter out (name + ">"), (v-min)/(max-min)
//  - [s X @owl B1..B8/Push]  : button out, on when normalised value > 0.5
// The pdlang ABI (docs/STATE-ABI.md) is declared and used only in PdlangPatch.cpp, so no generated header is needed.
#ifndef __PdlangPatch_hpp__
#define __PdlangPatch_hpp__

#include "Patch.h"

#define PDLANG_MAX_PARAMS 48

class PdlangPatch : public Patch {
  enum Kind { NONE, PARAM_IN, BUTTON_IN, PARAM_OUT, BUTTON_OUT };
  struct Map { uint32_t hash; uint8_t kind; uint8_t id; float min, max; };
  Map map[PDLANG_MAX_PARAMS];
  int nmap;
  char outnames[PDLANG_MAX_PARAMS][24];
  uint8_t* raw;
  void* ctx;
  float* in;
  float* out;
  int blocksize;
  int nin;
  volatile uint32_t buttonsPending; // bit per OWL button id, set in buttonChanged

  void addParameters();
  void addLegacyParameters();

public:
  static PdlangPatch* instance;

  PdlangPatch();
  ~PdlangPatch();
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples) override;
  void hostSend(uint32_t hash, const float* atoms, int32_t n);
  void processAudio(AudioBuffer& buffer) override;
};

#endif // __PdlangPatch_hpp__
