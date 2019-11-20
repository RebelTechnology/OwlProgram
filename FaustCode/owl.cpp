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

// We have to undefine min/max from Owl's basicmaths.h, otherwise they cause errors
// when Faust calls functions with the same names in std:: namespace
#undef min
#undef max

#include <string.h>
#include <strings.h>

#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__

#include "faust/dsp/dsp.h"
#include "faust/gui/UI.h"
#include "faust/gui/meta.h"

static float fFreq, fGain, fGate;
static float fBend = 1.0f;

class MonoVoiceAllocator {
  float& freq;
  float& gain;
  float& gate;
  float& bend;
  uint8_t notes[16];
  uint8_t lastNote = 0;
public:
  MonoVoiceAllocator(float& fq, float& gn, float& gt, float& bd): freq(fq), gain(gn), gate(gt), bend(bd) {
  }
  float getFreq(){
    return freq;
  }
  float getGain(){
    return gain;
  }
  float getGate(){
    return gate;
  }
  float getBend(){
    return bend;
  }
  void processMidi(MidiMessage msg){
    uint16_t samples = 0;
    if(msg.isNoteOn()){
      noteOn(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);
    }else if(msg.isNoteOff()){
      noteOff(msg.getNote(), (uint16_t)msg.getVelocity()<<5, samples);      
    }else if(msg.isPitchBend()){
      setPitchBend(msg.getPitchBend());
    }else if(msg.isControlChange()){
      if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
	allNotesOff();
    }
  }
  void setPitchBend(int16_t pb){
    float fb = pb*(2.0f/8192.0f);
    bend = exp2f(fb);
  }
  float noteToHz(uint8_t note){
    return 440.0f*exp2f((note-69)/12.0);
  }
  float velocityToGain(uint16_t velocity){
    return exp2f(velocity/4095.0f) -1;
    // return powf((1.0-depth) + (velocity/127.0*depth), 2.0);
  }
  void noteOn(uint8_t note, uint16_t velocity, uint16_t delay){
    if(lastNote < 16)
      notes[lastNote++] = note;
    freq = noteToHz(note);
    gain = velocityToGain(velocity);
    gate = 1;
  }
  void noteOff(uint8_t note, uint16_t velocity, uint16_t delay){
    int i;
    for(i=0; i<lastNote; ++i){
      if(notes[i] == note)
    	break;
    }
    if(lastNote > 1){
      lastNote--;
      while(i<lastNote){
	notes[i] = notes[i+1];
	i++;
      }
      freq = noteToHz(notes[lastNote-1]);
    }else{
      gate = 0;
      lastNote = 0;
    }
  }
  void allNotesOff(){
    lastNote = 0;
    bend = 0;
  }
};


enum ParserState {
   ST_NONE,
   ST_KEY,
   ST_VALUE
};


// This is a parser for Faust metadata. Currently it only reacts on midi declaration, but
// this can be extended in processItem class.
// To enable MIDI in current patch, add this to source Faust file:
// declare options "[midi:on]";

class MetaData : public Meta {
public:
  bool midiOn = false;

  void declare (const char* key, const char* value) {
    if (strcasecmp(key, "options") == 0) {
      // Parse options.
      // Faust provides a metadata parser, but they use stdlib stuff that won't work on OWL
      parseOptions(value);

    }
  }

private:
  void processItem(const char* item_key, const char* item_value) {
    if (strcasecmp(item_key, "midi") == 0 && strcasecmp(item_value, "on") == 0) {
      midiOn = true;
      //debugMessage("MIDI ON");
    }
  }

  void parseOptions(const char* value) {
    const char* c = value;
    const uint8_t max_len = 32;
    // We don't need to read long item values, so set a limit that would be used for truncating them.
    char item_key[max_len] = "";
    char item_value[max_len] = "";
    uint8_t key_pos;
    uint8_t value_pos;
    ParserState state = ST_NONE;
    while (*c) {
      if (state == ST_NONE) {
	if (*c == '[') {
	  // Start reading key
	  state = ST_KEY;
	  key_pos = 0;
	  value_pos = 0;
	}
      }
      else if (state == ST_KEY) {
	if (*c == ':') {
	  // Found end of key
	  item_key[key_pos + 1] = '\n';
	  // Start reading value
	  state = ST_VALUE;
	}
	else if (key_pos < max_len - 1)
	  // Add next character to key
	    item_key[key_pos++] = *c;
      }
      else if (state == ST_VALUE) {
	if  (*c == ']') {
	  // Found end of value
	  item_value[value_pos + 1] = '\n';
	  processItem(item_key, item_value);
	  // Reset state
	  state = ST_NONE;
	}
	else if (value_pos < max_len -  1)
	  // Add next character to value
	  item_value[value_pos++] = *c;
      };
      *c++;
    }
  }
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
  bool isOutput;
public:
  OwlParameterBase(Patch* pp, FAUSTFLOAT* z, bool output) :
    fPatch(pp), fZone(z), isOutput(output) {}
  virtual void update()	{}
};

class OwlParameter : public OwlParameterBase
{
protected:
  PatchParameterId fParameter;	// OWL parameter code : PARAMETER_A,...
  float	fMin;			// Faust widget minimal value
  float	fSpan;			// Faust widget value span (max-min)
	
public:
  OwlParameter(Patch* pp, PatchParameterId param, FAUSTFLOAT* z, const char* l, float init, float lo, float hi, bool output=false) :
    OwlParameterBase(pp, z, output), fParameter(param), fMin(lo), fSpan(hi-lo) {
    fPatch->registerParameter(param, l);
    fPatch->setParameterValue(fParameter, (init - lo) / fSpan);
  }
  void update()	{
    if (isOutput) {
      fPatch->setParameterValue(fParameter, (*fZone - fMin) / fSpan);
    }
    else
      *fZone = fMin + fSpan*fPatch->getParameterValue(fParameter);
	
  }
};

class OwlVariable : public OwlParameterBase
{
protected:
  float* fValue;
  float	fLo;			// Faust widget minimal value
  float	fHi;			// Faust widget value span (max-min)
	
public:
  OwlVariable(Patch* pp, float* t, FAUSTFLOAT* z, const char* l, float init, float lo, float hi, bool output=false) :
    OwlParameterBase(pp, z, output), fValue(t), fLo(lo), fHi(hi) {
    *fZone = init;
  }
  void update()	{
    if (isOutput) {
      // We don't output OwlVariable anywhere, so would this even be useful?
      *fValue = *fZone;
    }
    else {
      float value = *fValue;
      // clip value to min/max
      value = value > fLo ? (value < fHi ? value : fHi) : fLo;
      *fZone = value;
    }
  }  	
};

class OwlButton : public OwlParameterBase
{
protected:
  PatchButtonId	fButton;		// OWL button id : PUSHBUTTON, ...
public:
  OwlButton(Patch* pp, PatchButtonId button, FAUSTFLOAT* z, const char* l) :
    OwlParameterBase(pp, z, true), fButton(button) {}
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
#define NO_PARAMETER     ((PatchParameterId)-1)
#define NO_BUTTON        ((PatchButtonId)-1)

MonoVoiceAllocator allocator(fFreq, fGain, fGate, fBend);

class OwlUI : public UI
{
  Patch* fPatch;
  PatchParameterId fParameter;					// current parameter ID, value NO_PARAMETER means not set
  int fParameterIndex = 0;						// number of OwlParameters collected so far
  OwlParameterBase* fParameterTable[MAXOWLPARAMETERS];
  PatchButtonId fButton;
  // check if the widget is an Owl parameter and, if so, add the corresponding OwlParameter
  void addInputOwlParameter(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi) {
    if(fParameterIndex < MAXOWLPARAMETERS){
      if(meta.midiOn && strcasecmp(label,"freq") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fFreq, zone, label, init, lo, hi);
      }else if(meta.midiOn && strcasecmp(label,"gain") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGain, zone, label, init, lo, hi);
      }else if(meta.midiOn && strcasecmp(label,"bend") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fBend, zone, label, init, lo, hi);
      }else if(fParameter != NO_PARAMETER){
	fParameterTable[fParameterIndex++] = new OwlParameter(fPatch, fParameter, zone, label, init, lo, hi);
      }
    }
    fParameter = NO_PARAMETER; 		// clear current parameter ID
  }

  void addOutputOwlParameter(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
    if(fParameterIndex < MAXOWLPARAMETERS){
      if(meta.midiOn && strcasecmp(label,"freq") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fFreq, zone, label, lo, lo, hi, true);
      }else if(meta.midiOn && strcasecmp(label,"gain") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGain, zone, label, lo, lo, hi, true);
      }else if(meta.midiOn && strcasecmp(label,"bend") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fBend, zone, label, lo, lo, hi, true);
      }else if(fParameter != NO_PARAMETER){
	fParameterTable[fParameterIndex++] = new OwlParameter(fPatch, fParameter, zone, label, lo, lo, hi, true);
      }
    }
    fParameter = NO_PARAMETER; 		// clear current parameter ID
  }

  void addOwlButton(const char* label, FAUSTFLOAT* zone) {
    if(fParameterIndex < MAXOWLPARAMETERS){
      if(meta.midiOn && strcasecmp(label,"gate") == 0){
	fParameterTable[fParameterIndex++] = new OwlVariable(fPatch, &fGate, zone, label, 0.0f, 0.0f, 1.0f);
      }else if(fButton != NO_BUTTON){
	fParameterTable[fParameterIndex++] = new OwlButton(fPatch, fButton, zone, label);
      }
    }
    fButton = NO_BUTTON; 		// clear current button ID
  }

  // we dont want to create a widget but we clear the current parameter ID just in case
  void skip() {
    fParameter = NO_PARAMETER; 		// clear current parameter ID
    fButton = NO_BUTTON;
  }

public:
  MetaData meta;
  OwlUI(Patch* pp) : fPatch(pp), fParameter(NO_PARAMETER), fParameterIndex(0), fButton(NO_BUTTON) {}
	
  virtual ~OwlUI() {
    for (int i=0; i<fParameterIndex; i++)
      delete fParameterTable[i];
}
	
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

  virtual void addButton(const char* label, FAUSTFLOAT* zone) { addOwlButton(label, zone); }
  virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) { addOwlButton(label, zone); }
  virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addInputOwlParameter(label, zone, init, lo, hi); }
  virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addInputOwlParameter(label, zone, init, lo, hi); }
  virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addInputOwlParameter(label, zone, init, lo, hi); }

  // -- passive widgets

  virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi)	{ addOutputOwlParameter(label, zone, lo, hi); }
  virtual void addVerticalBargraph  (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) { addOutputOwlParameter(label, zone, lo, hi); }
  // -- soundfiles
  virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) { skip(); }

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
      else if (strcasecmp(id,"ButtonA") == 0)  fButton = BUTTON_A;
      else if (strcasecmp(id,"ButtonB") == 0)  fButton = BUTTON_B;
      else if (strcasecmp(id,"ButtonC") == 0)  fButton = BUTTON_C;
      else if (strcasecmp(id,"ButtonD") == 0)  fButton = BUTTON_D;
    }else if(strcasecmp(k,"midi") == 0){
      // todo!
      // if (strcasecmp(id,"pitchwheel") == 0)  fParameter = PARAMETER_G; // mapped to pitch wheel
      // declare(&fHslider1, "midi", "pitchwheel");
      // declare(&fButton1, "midi", "ctrl 64");
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

    fBend = 1.0f;
    fDSP = new mydsp();
    fDSP->metadata(&fUI.meta);
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
    if (fUI.meta.midiOn)
      allocator.processMidi(msg);
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
