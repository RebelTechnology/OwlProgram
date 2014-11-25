#ifndef __PatchController_h__
#define __PatchController_h__

#include "PatchProcessor.h"
#include "owlcontrol.h"

class PatchController;
extern PatchController patches;

enum PatchModeId {
  SINGLE_GREEN_MODE   = 0,
  SINGLE_RED_MODE     = 1,
  DUAL_GREEN_MODE     = 2,
  DUAL_RED_MODE       = 3,
  SERIES_GREEN_MODE   = 4,
  SERIES_RED_MODE     = 5,
  PARALLEL_GREEN_MODE = 6,
  PARALLEL_RED_MODE   = 7
};

class PatchController {
public:  
  PatchController();
  ~PatchController();
  void init();
  void reset();
  void process(AudioBuffer& buffer);
  /* void setPatch(LedPin slot, uint8_t index); */
  /* LedPin getActiveSlot(); */
  /* void toggleActiveSlot(); */
  /* void setActiveSlot(LedPin slot); */
  void initialisePatch(LedPin slot, uint8_t index);
  PatchProcessor* getInitialisingPatchProcessor();
  PatchProcessor* getActivePatchProcessor();
  PatchProcessor* getCurrentPatchProcessor(){
    /* deprecated: the FAUST OWL target depends on this method */
    return getActivePatchProcessor();
  }
private:
  void processParallel(AudioBuffer& buffer);
  PatchProcessor* initialisingProcessor;
  PatchProcessor green;
  PatchProcessor red;
  /* LedPin activeSlot = NONE; */
  uint8_t mode;
  uint16_t* parameterValues;

public:  
  uint16_t getParameter(int pid){
    return smem.parameters[pid];
  }
  void setParameter(int pid, uint16_t value){
    smem.parameters[pid] = value;
  }
  float getParameterValue(PatchParameterId pid){
    return smem.parameters[pid]/4096.0f;
  }
  bool isButtonPressed(PatchButtonId bid){
    return smem.buttons & (1<<bid);
  }
  void setButton(PatchButtonId bid, bool on){
    if(on)
      smem.buttons |= 1<<bid;
    else
      smem.buttons &= ~(1<<bid);
  }
  PatchModeId getPatchMode(){
    return (PatchModeId)smem.parameters[PATCH_MODE_PARAMETER_ID];
  }
  void setPatchMode(PatchModeId mode){
    smem.parameters[PATCH_MODE_PARAMETER_ID] = mode;
  }
  uint16_t getGreenPatchId(){
    return smem.parameters[GREEN_PATCH_PARAMETER_ID];
  }
  uint16_t getRedPatchId(){
    return smem.parameters[RED_PATCH_PARAMETER_ID];
  }
};

#endif // __PatchController_h__
