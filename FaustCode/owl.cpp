/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2014 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.

 ************************************************************************
 ************************************************************************/

#ifndef __FaustPatch_h__
#define __FaustPatch_h__

#include "Patch.h"

#include <string.h>
#include <strings.h>

#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__

#include "faust/dsp/dsp.h"
#include "faust/gui/UI.h"


inline static float CPS2midi(float freqHz, double tune = 440.)
{
  if (freqHz > 0.f)
    return 12.f * log2f(freqHz/tune)+57.f;
  else
    return 0.;
}
  
inline static float midi2CPS(float pitch, float tune = 440.f)
{
  return tune * exp2f((pitch - 69.f) / 12.f);
}

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};

/**************************************************************************************

	OwlParameter : object used by OwlUI to ensures the connection between an owl parameter 
	and a faust widget
	
***************************************************************************************/

class OwlParameterBase
{
protected:
  Patch* fPatch;      // needed to register and read owl parameters
  FAUSTFLOAT* fZone;  // Faust widget zone
  const char* fLabel; // Faust widget label
public:
  OwlParameterBase() :
    fPatch(NULL), fZone(0), fLabel("") {}
  OwlParameterBase(const OwlParameterBase& w) :
    fPatch(w.fPatch), fZone(w.fZone), fLabel(w.fLabel) {}
  OwlParameterBase(Patch* pp, FAUSTFLOAT* z, const char* l) :
    fPatch(pp), fZone(z), fLabel(l) {}
  virtual void update()	{}
};

class OwlParameter : public OwlParameterBase
{
protected:
  PatchParameterId	fParameter;		// OWL parameter code : PARAMETER_A,...
  float				fMin;			// Faust widget minimal value
  float				fSpan;			// Faust widget value span (max-min)
	
public:
  OwlParameter() :
    fParameter(PARAMETER_A), fMin(0), fSpan(1) {}
  OwlParameter(const OwlParameter& w) :
    OwlParameterBase(w), fParameter(w.fParameter), fMin(w.fMin), fSpan(w.fSpan) {}
  OwlParameter(Patch* pp, PatchParameterId param, FAUSTFLOAT* z, const char* l, float lo, float hi) :
    OwlParameterBase(pp, z, l), fParameter(param), fMin(lo), fSpan(hi-lo) {
    fPatch->registerParameter(fParameter, fLabel);
  }
  void update()	{ *fZone = fMin + fSpan*fPatch->getParameterValue(fParameter); }
	
};

class OwlVariable : public OwlParameterBase
{
protected:
  float* fValue;
  float	fMin;			// Faust widget minimal value
  float	fSpan;			// Faust widget value span (max-min)
	
public:
  // OwlVariable() :
  //   fParameter(NULL), fMin(0), fSpan(1) {}
  // OwlVariable(const OwlVariable& w) :
  //   OwlParameterBase(w), fParameter(w.fParameter), fMin(w.fMin), fSpan(w.fSpan) {}
  OwlVariable(Patch* pp, float* t, FAUSTFLOAT* z, const char* l, float lo, float hi) :
    OwlParameterBase(pp, z, l), fValue(t), fMin(lo), fSpan(hi-lo) {
  }
  void update()	{ *fZone = *fValue; }
	
};

class OwlButton : public OwlParameterBase
{
protected:
  PatchButtonId	fButton;		// OWL button id : PUSHBUTTON, ...
public:
  OwlButton() :
    fButton(PUSHBUTTON) {}
  OwlButton(const OwlButton& w) :
    OwlParameterBase(w), fButton(w.fButton) {}
  OwlButton(Patch* pp, PatchButtonId button, FAUSTFLOAT* z, const char* l) :
    OwlParameterBase(pp, z, l), fButton(button) {}
  void update()	{ *fZone = fPatch->isButtonPressed(fButton); }
};


/**************************************************************************************

	OwlUI : Faust User Interface builder. Passed to buildUserInterface OwlUI ensures
	the mapping between owl parameters and faust widgets. It relies on specific
	metadata "...[OWL:PARAMETER_X]..." in widget's label for that. For example any 
	faust widget with metadata [OWL:PARAMETER_B] will be controlled by PARAMETER_B 
	(the second knob).
	
***************************************************************************************/

// The maximun number of mappings between owl parameters and faust widgets 
#define MAXOWLPARAMETERS 20
#define MAXOWLBUTTONS    2
#define NO_PARAMETER     ((PatchParameterId)-1)
#define NO_BUTTON        ((PatchButtonId)-1)

static float fFreq, fGain, fGate;

class OwlUI : public UI
{
  Patch* 	fPatch;
  PatchParameterId	fParameter;					// current parameter ID, value NO_PARAMETER means not set
  int					fParameterIndex;						// number of OwlParameters collected so far
  OwlParameterBase* fParameterTable[MAXOWLPARAMETERS];
  PatchButtonId fButton;
  // check if the widget is an Owl parameter and, if so, add the corresponding OwlParameter
  void addOwlParameter(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
    if(fParameterIndex < MAXOWLPARAMETERS){
      // if(fParameter == PARAMETER_FREQ){
      // 	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fFreq, zone, label, lo, hi);
      // }else if(fParameter == PARAMETER_GAIN){
      // 	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGain, zone, label, lo, hi);
      // }else 
      if(strcasecmp(label,"freq") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fFreq, zone, label, lo, hi);
      }else if(strcasecmp(label,"gain") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGain, zone, label, lo, hi);
      }else if(fParameter >= PARAMETER_A){
	fParameterTable[fParameterIndex++] = new OwlParameter(fPatch, fParameter, zone, label, lo, hi);
      }
    }
    fParameter = NO_PARAMETER; 		// clear current parameter ID
  }

  void addOwlButton(const char* label, FAUSTFLOAT* zone) {
    if(fParameterIndex < MAXOWLPARAMETERS){
      // if(fButton == GATE_BUTTON){
      // 	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGate, zone, label, 0.0f, 1.0f);
      // }else 
      if(strcasecmp(label,"gate") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGate, zone, label, 0.0f, 1.0f);
      }else if(fButton >= PUSHBUTTON){
	fParameterTable[fParameterIndex++] = new OwlButton(fPatch, fButton, zone, label);
      }
    }

    fButton = NO_BUTTON; 		// clear current button ID
  }

  // we dont want to create a widget by-ut we clear the current parameter ID just in case
  void skip() {
    fParameter = NO_PARAMETER; 		// clear current parameter ID
    fButton = NO_BUTTON;
  }

public:

  OwlUI(Patch* pp) : fPatch(pp), fParameter(NO_PARAMETER), fParameterIndex(0), fButton(NO_BUTTON) {}
	
  virtual ~OwlUI() {}
	
  // should be called before compute() to update widget's zones registered as Owl parameters
  void update() {
    for (int i=0; i<fParameterIndex; i++)
      fParameterTable[i]->update();
  }

  //---------------------- virtual methods called by buildUserInterface ----------------
	
  // -- widget's layouts

  virtual void openTabBox(const char* label) {}
  virtual void openHorizontalBox(const char* label) {}
  virtual void openVerticalBox(const char* label) {}
  virtual void closeBox() {}

  // -- active widgets

  virtual void addButton(const char* label, FAUSTFLOAT* zone) 																			{ addOwlButton(label, zone); }
  virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 																		{ addOwlButton(label, zone); }
  virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlParameter(label, zone, lo, hi); }
  virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlParameter(label, zone, lo, hi); }
  virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 			{ addOwlParameter(label, zone, lo, hi); }

  // -- passive widgets

  virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }
  virtual void addVerticalBargraph  (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }
  // -- soundfiles
  virtual void addSoundfile(const char* label, Soundfile** sf_zone) { skip(); }

  // -- metadata declarations

  virtual void declare(FAUSTFLOAT* z, const char* k, const char* id) {
    if(strcasecmp(k,"OWL") == 0){
      if(strncasecmp(id, "PARAMETER_", 10) == 0)
	id += 10;
      if (strcasecmp(id,"A") == 0)  fParameter = PARAMETER_A;
      else if (strcasecmp(id,"B") == 0)  fParameter = PARAMETER_B;
      else if (strcasecmp(id,"C") == 0)  fParameter = PARAMETER_C;
      else if (strcasecmp(id,"D") == 0)  fParameter = PARAMETER_D;
      else if (strcasecmp(id,"E") == 0)  fParameter = PARAMETER_E;
      else if (strcasecmp(id,"F") == 0)  fParameter = PARAMETER_F;
      else if (strcasecmp(id,"G") == 0)  fParameter = PARAMETER_G;
      else if (strcasecmp(id,"H") == 0)  fParameter = PARAMETER_H;
      else if (strcasecmp(id,"PUSH") == 0)  fButton = PUSHBUTTON;
    }

  }
};

/* Simple heap based memory manager.
 * Uses overloaded new/delete operators on OWL hardware.
 */
struct OwlMemoryManager : public dsp_memory_manager {
  void* allocate(size_t size)
  {
    void* res = new uint8_t[size];
    return res;
  }
  virtual void destroy(void* ptr)
  {
    delete (uint8_t*)ptr;
  }    
};

#endif // __FaustCommonInfrastructure__

/**************************BEGIN USER SECTION **************************/

<<includeIntrinsic>>

<<includeclass>>

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	FaustPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class FaustPatch : public Patch
{
  mydsp*   fDSP;
  OwlUI	fUI;
  OwlMemoryManager mem;
    
public:

  FaustPatch() : fUI(this)
  {
    // // Allocate memory for dsp object first to ensure high priority memory is used
    // void* allocated = mem.allocate(sizeof(mydsp));
    // // DSP static data is initialised using classInit.
    // mydsp::classInit(int(getSampleRate()), &mem);
    // // call mydsp constructor
    // fDSP = new (allocated) mydsp();
    // fDSP->instanceInit(int(getSampleRate()));
    // // Map OWL parameters and faust widgets 
    // fDSP->buildUserInterface(&fUI);

    fDSP = new mydsp();
    mydsp::fManager = &mem; // set custom memory manager
    mydsp::classInit(int(getSampleRate())); // initialise static tables
    fDSP->instanceInit(int(getSampleRate())); // initialise DSP instance
    fDSP->buildUserInterface(&fUI); // Map OWL parameters
  }

  ~FaustPatch(){
    delete fDSP;
    // mem.destroy(fDSP);
    // DSP static data is destroyed using classDestroy.
    mydsp::classDestroy();
  }

  void processMidi(MidiMessage msg){
    if(msg.isNoteOn()){
      fFreq = midi2CPS(msg.getNote());
      fGain = msg.getNote()/127.0f;
      fGate = 1.0f;
    }else if(msg.isNoteOff()){
      fGain = msg.getNote()/127.0f;
      fGate = 0.0f;
    }
  }

  void processAudio(AudioBuffer &buffer)
  {
    // Reasonably assume we will not have more than 32 channels
    float*  ins[32];
    float*  outs[32];
    int     n = buffer.getChannels();
        
    if ( (fDSP->getNumInputs() < 32) && (fDSP->getNumOutputs() < 32) ) {
            
      // create the table of input channels
      for(int ch=0; ch<fDSP->getNumInputs(); ++ch) {
	ins[ch] = buffer.getSamples(ch%n);
      }
            
      // create the table of output channels
      for(int ch=0; ch<fDSP->getNumOutputs(); ++ch) {
	outs[ch] = buffer.getSamples(ch%n);
      }
            
      // read OWL parameters and updates corresponding Faust Widgets zones
      fUI.update(); 
            
      // Process the audio samples
      fDSP->compute(buffer.getSize(), ins, outs);
    }
  }
};

extern "C" {
  void doSetButton(uint8_t id, uint16_t state, uint16_t samples);
  int owl_pushbutton(int value){
    static bool state = 0;
    static uint16_t counter = 0;
    value = (bool)value;
    if(state != value){
      state = value;
      doSetButton(PUSHBUTTON, state, counter);
    }
    if(++counter > getProgramVector()->audio_blocksize)
      counter = 0;
    return value;
  }
}

#endif // __FaustPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////
