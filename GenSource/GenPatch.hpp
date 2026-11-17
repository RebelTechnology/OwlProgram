#ifndef __GenPatch_hpp__
#define __GenPatch_hpp__

#include "Patch.h"
#include "gen.h"
#include "genlib.h"
#include "PatchMetadata.h"

#if __has_include("metadata.h")
#include "metadata.h"
#endif

/* Optional per-patch map from gen~ parameter names to OWL parameter ids.
 * Put a GenParameterMap.h next to the export (in PATCHSOURCE) that defines
 *   static const GenParameterMapping gen_parameter_map[] = { {"cutoff", PARAMETER_A, GEN_SCALE_LOG}, ... };
 * Mapped names take precedence over the name conventions below (A..H, AA..HH, Exp, Push...),
 * and keep their gen~ name when they are registered. The optional scale is the knob response:
 * GEN_SCALE_LINEAR (the default) maps the 0..1 parameter to min..max, GEN_SCALE_LOG to
 * min * (max/min)^v (when 0 < min < max), as Faust's [scale:log] does in FaustSource/owl.cpp.
 * GEN_SCALE_INT makes a stepped parameter (a menu or a switch): {"shape", PARAMETER_E, GEN_SCALE_INT, 4}
 * gives min + k * (max-min)/(steps-1) for k = floor(v * steps), so each of the steps values gets an equal
 * share of the knob's travel and the top of the travel gives max. steps 0 (the default) takes
 * max-min+1 steps, the integers of an integer range. */
enum GenParameterScale {
  GEN_SCALE_LINEAR = 0,
  GEN_SCALE_LOG = 1,
  GEN_SCALE_INT = 2
};
struct GenParameterMapping {
  const char* name;
  PatchParameterId pid;
  GenParameterScale scale = GEN_SCALE_LINEAR;
  uint8_t steps = 0;
};
/* Optional map of the export's outputs, for an export with other than one output per audio channel (a
 * trigger, an envelope, a counter). Define GEN_OUTPUT_MAP and, in GenParameterMap.h,
 *   static const GenOutputMapping gen_output_map[] = {
 *     {NULL, 0, GEN_OUTPUT_AUDIO, 0},                       // out1 to audio channel 0 (left)
 *     {"env>", 3, GEN_OUTPUT_PARAMETER, PARAMETER_F},       // out4's block mean to output parameter F
 *     {"count>", 1, GEN_OUTPUT_PARAMETER, PARAMETER_G, "occurrences"}, // out2 / occurrences, block mean
 *     {"z>", 2, GEN_OUTPUT_PARAMETER, PARAMETER_G, NULL, true}, // out3, bipolar: (mean + 1) / 2 to G
 *     {NULL, 2, GEN_OUTPUT_BUTTON, PUSHBUTTON},             // out3 to the push gate: high for a block when
 *   };                                                      // the block's maximum is over 0.5
 * Then only the mapped outputs go anywhere (an output may be mapped more than once), and an audio channel no
 * output is mapped to is silent. Without a map, the outputs beyond the audio channels go to F, G, ... */
enum GenOutputKind {
  GEN_OUTPUT_AUDIO = 0,
  GEN_OUTPUT_PARAMETER = 1,
  GEN_OUTPUT_BUTTON = 2
};
struct GenOutputMapping {
  const char* name; // registered name of a GEN_OUTPUT_PARAMETER (ending with '>')
  uint8_t output; // the export's output, 0 for out1
  GenOutputKind kind;
  uint16_t id; // the audio channel, PatchParameterId or PatchButtonId
  const char* per = NULL; // GEN_OUTPUT_PARAMETER: the gen~ parameter the block mean is divided by
  bool bipolar = false; // GEN_OUTPUT_PARAMETER: a -1..1 output, sent as (x + 1) / 2 in 0..1
};
#if __has_include("GenParameterMap.h")
#include "GenParameterMap.h"
#define GEN_PARAMETER_MAP
#endif

#define GEN_OWL_PARAM_FREQ "freq"
#define GEN_OWL_PARAM_GAIN "gain"
#define GEN_OWL_PARAM_GATE "gate"
#define GEN_OWL_PARAM_BEND "bend"
#define GEN_OWL_PARAM_E "Exp"
#define GEN_OWL_PARAM_PUSH "Push"
#define GEN_OWL_PARAM_BUTTON_A "ButtonA"
#define GEN_OWL_PARAM_BUTTON_B "ButtonB"
#define GEN_OWL_PARAM_BUTTON_C "ButtonC"
#define GEN_OWL_PARAM_BUTTON_D "ButtonD"

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

class GenParameterBase {
public:
  ~GenParameterBase(){}
  virtual void update(Patch* patch, CommonState *context){}
};

class GenParameter : public GenParameterBase {
public:
  PatchParameterId pid;
  int8_t index;
  float min = 0.0f;
  float max = 1.0f;
  float logratio = 0.0f; // log(max/min) for GEN_SCALE_LOG, 0 for a linear response
  int steps = 0; // the number of values for GEN_SCALE_INT, 0 for a continuous response
  GenParameter(Patch* patch, CommonState *context, const char* name, PatchParameterId id, int8_t idx,
	       GenParameterScale scale = GEN_SCALE_LINEAR, uint8_t nsteps = 0) : pid(id), index(idx) {
#ifndef OWL_METADATA
    patch->registerParameter(id, name);
#endif
    if(gen::getparameterhasminmax(context, index)){
      min = gen::getparametermin(context, index);
      max = gen::getparametermax(context, index);
    }    
    if(scale == GEN_SCALE_LOG && min > 0 && max > min)
      logratio = logf(max / min);
    if(scale == GEN_SCALE_INT && max > min)
      steps = nsteps > 1 ? nsteps : (int)(max - min) + 1;
    float defaultvalue;
    gen::getparameter(context, index, &defaultvalue);
    // the patch parameter is 0..1, mapped to min..max in update()
    if(steps > 1){
      // the default's step k at k/(steps-1), inside its share [k/steps, (k+1)/steps) of the travel
      int k = (int)roundf((defaultvalue - min) * (steps - 1) / (max - min));
      defaultvalue = k < 0 ? 0 : k > steps - 1 ? 1 : (float)k / (steps - 1);
    }else if(logratio != 0 && defaultvalue > 0)
      defaultvalue = logf(defaultvalue / min) / logratio;
    else if(max != min)
      defaultvalue = (defaultvalue - min) / (max - min);
    patch->setParameterValue(id, defaultvalue);
  }
  void update(Patch* patch, CommonState *context){
    float value = patch->getParameterValue(pid);
    if(steps > 1){
      int k = (int)(value * steps);
      k = k < 0 ? 0 : k > steps - 1 ? steps - 1 : k;
      value = min + k * (max - min) / (steps - 1);
    }else if(logratio != 0)
      value = min * expf(logratio * value);
    else
      value = value * (max-min) + min;
    gen::setparameter(context, index, value, NULL);
  }
};

class GenButton : public GenParameterBase  {
public:
  PatchButtonId bid;
  int8_t index;
  float min = 0.0;
  float max = 1.0;
  GenButton(Patch* patch, CommonState *context, const char* name, PatchButtonId id, int8_t idx) : bid(id), index(idx) {
    if(gen::getparameterhasminmax(context, index)){
      min = gen::getparametermin(context, index);
      max = gen::getparametermax(context, index);
    }
  }
  void update(Patch* patch, CommonState *context){
    float value = patch->isButtonPressed(bid);
    value = value * (max-min) + min;
    gen::setparameter(context, index, value, NULL);
  }
};

class GenVariableParameter : public GenParameterBase  {
public:
  float* fp;
  int8_t index;
  GenVariableParameter(Patch* patch, CommonState *context, const char* name, float* f, int8_t idx)
    : fp(f), index(idx) {}
  void update(Patch* patch, CommonState *context){
    float value = *fp;
    // clip value to min/max (not needed, done inside gen~ code)
    // value = value > min ? (value < max ? value : max) : min;
    gen::setparameter(context, index, value, NULL);
  }
};

class GenPatch : public Patch {
private:
  class OutputParameter {
  public:
    uint16_t id;
    const char* name;
    FloatArray buffer;
    OutputParameter() : name(NULL) {}
    OutputParameter(uint16_t id, const char* name, FloatArray buffer)
      : id(id), name(name), buffer(buffer) {}
  };
  SimpleArray<OutputParameter> outputs;

  CommonState *context;
  GenParameterBase** params;
  size_t param_count = 0;
  float freq = 440.0f;
  float gain = 0.0f;
  float gate = 0.0f;
  float bend = 1.0;
  MonoVoiceAllocator allocator;
  float** buffers;
#ifdef GEN_OUTPUT_MAP
  float** outbuffers;
  int outper[sizeof(gen_output_map)/sizeof(gen_output_map[0])]; // the index of each map entry's `per`, or -1
#endif
public:
  GenPatch() : allocator(freq, gain, gate, bend) {
    context = (CommonState*)gen::create(getSampleRate(), getBlockSize());
    int numParams = gen::num_params();
    params = new GenParameterBase*[numParams];
    for(int i = 0; i < numParams; i++){
      const char* name = gen::getparametername(context, i);
#ifdef GEN_PARAMETER_MAP
      bool mapped = false;
      for(size_t m = 0; m < sizeof(gen_parameter_map)/sizeof(gen_parameter_map[0]); ++m){
	if(strcmp(gen_parameter_map[m].name, name) == 0){
	  params[param_count++] = new GenParameter(this, context, name, gen_parameter_map[m].pid, i,
					       gen_parameter_map[m].scale, gen_parameter_map[m].steps);
	  mapped = true;
	  break;
	}
      }
      if(mapped)
	continue;
#endif
      if(strcasecmp(GEN_OWL_PARAM_FREQ, name) == 0){
	params[param_count++] = new GenVariableParameter(this, context, name, &freq, i);
      }else if(strcasecmp(GEN_OWL_PARAM_GAIN, name) == 0){
	params[param_count++] = new GenVariableParameter(this, context, name, &gain, i);
      }else if(strcasecmp(GEN_OWL_PARAM_GATE, name) == 0){
	params[param_count++] = new GenVariableParameter(this, context, name, &gate, i);
      }else if(strcasecmp(GEN_OWL_PARAM_BEND, name) == 0){
	params[param_count++] = new GenVariableParameter(this, context, name, &bend, i);
      }else if(strcasecmp(GEN_OWL_PARAM_E, name) == 0){
	params[param_count++] = new GenParameter(this, context, name, PARAMETER_E, i);
      }else if(strcasecmp(GEN_OWL_PARAM_PUSH, name) == 0){
	params[param_count++] = new GenButton(this, context, name, PUSHBUTTON, i);	
      }else if(strcasecmp(GEN_OWL_PARAM_BUTTON_A, name) == 0){
	params[param_count++] = new GenButton(this, context, name, BUTTON_A, i);
      }else if(strcasecmp(GEN_OWL_PARAM_BUTTON_B, name) == 0){
	params[param_count++] = new GenButton(this, context, name, BUTTON_B, i);
      }else if(strcasecmp(GEN_OWL_PARAM_BUTTON_C, name) == 0){
	params[param_count++] = new GenButton(this, context, name, BUTTON_C, i);
      }else if(strcasecmp(GEN_OWL_PARAM_BUTTON_D, name) == 0){
	params[param_count++] = new GenButton(this, context, name, BUTTON_D, i);
      }else if(strlen(name) == 1 && name[0] >= 'A' && name[0] <= 'H'){
	uint8_t index = name[0]-'A';
	params[param_count++] = new GenParameter(this, context, name, (PatchParameterId)index, i);
      }else if(strlen(name) == 2 && name[0] >= 'A' && name[0] <= 'H'
	                         && name[1] >= 'A' && name[1] <= 'H'){
	uint8_t index = PARAMETER_AA*(1+name[0]-'A') + name[1]-'A';
	params[param_count++] = new GenParameter(this, context, name, (PatchParameterId)index, i);
      }
    }
#ifdef GEN_OUTPUT_MAP
    // every output in a scratch buffer of its own, routed by gen_output_map in processAudio()
    outbuffers = new float*[gen::num_outputs()];
    for(int ch=0; ch<gen::num_outputs(); ++ch)
      outbuffers[ch] = new float[getBlockSize()];
    for(size_t m = 0; m < sizeof(gen_output_map)/sizeof(gen_output_map[0]); ++m){
      const GenOutputMapping& o = gen_output_map[m];
      outper[m] = -1;
      if(o.kind == GEN_OUTPUT_PARAMETER){
	registerParameter((PatchParameterId)o.id, o.name);
	for(int i = 0; o.per != NULL && i < gen::num_params(); i++)
	  if(strcmp(gen::getparametername(context, i), o.per) == 0)
	    outper[m] = i;
      }
    }
#endif
    buffers = new float*[gen::num_outputs()];
    size_t channels = getNumberOfChannels();
    for(int ch=channels; ch<gen::num_outputs(); ++ch)
      buffers[ch] = new float[getBlockSize()];

#ifdef GEN_OUTPUT_MAP
    size_t nof_outs = 0; // routed by gen_output_map instead
#else
    size_t nof_outs = gen::num_outputs()-std::min((size_t)gen::num_outputs(), channels);
#endif
    // debugMessage("outs", (int)channels, nof_outs, sizeof(OutputParameter[nof_outs]));
    if(nof_outs > 0)
      outputs = SimpleArray<OutputParameter>(new OutputParameter[nof_outs], nof_outs);
#ifdef OWL_METADATA
    size_t outputindex = 0;
    for(int i=0; i<PatchMetadata::parameter_count; ++i){
      const PatchMetadata::Control& ctrl = PatchMetadata::parameters[i];
      PatchParameterId pid = (PatchParameterId)ctrl.id;
      if(ctrl.flags & CONTROL_OUTPUT){
	if(outputindex < nof_outs)
	  outputs[outputindex] = OutputParameter(pid, ctrl.name, FloatArray(buffers[channels+outputindex], getBlockSize()));
	outputindex++;
      }
      size_t len = strlen(ctrl.name);
      if(ctrl.flags & CONTROL_OUTPUT && ctrl.name[len-1] != '>'){
	// add a > at end of name
	char name[len+2];
	strcpy(name, ctrl.name);
	name[len] = '>';
	name[len+1] = '\0';
	registerParameter(pid, name);
      }else{
	registerParameter(pid, ctrl.name);
      }
    }
    for(int i=0; i<PatchMetadata::button_count; ++i){
      const PatchMetadata::Control& ctrl = PatchMetadata::buttons[i];
      if(ctrl.flags & CONTROL_OUTPUT){
	if(outputindex < nof_outs)
	  outputs[outputindex] = OutputParameter(ctrl.id+0x100, ctrl.name, FloatArray(buffers[channels+outputindex], getBlockSize()));
	outputindex++;
      }
    }
    if(outputindex != nof_outs)
      debugMessage("Output parameters not matching", (int)outputindex, nof_outs);
#elif !defined(GEN_OUTPUT_MAP)
    char name[] = "Out0>";
    for(int ch=channels; ch<gen::num_outputs(); ++ch){
      name[3] = '1'+ch;
      PatchParameterId pid = (PatchParameterId)(PARAMETER_F+(ch-channels));
      registerParameter(pid, name);
      outputs[ch-channels] = OutputParameter(pid, name, FloatArray(buffers[ch], getBlockSize()));
    }
#endif
  }

  ~GenPatch() {
    gen::destroy(context);
    for(size_t i=0; i<param_count; ++i)
      delete params[i];
    delete[] params;
    for(size_t i=getNumberOfChannels(); i<gen::num_outputs(); ++i)
      delete[] buffers[i];
    delete[] buffers;
    delete[] outputs.getData();
#ifdef GEN_OUTPUT_MAP
    for(int i=0; i<gen::num_outputs(); ++i)
      delete[] outbuffers[i];
    delete[] outbuffers;
#endif
  }

  void processMidi(MidiMessage msg){
    allocator.processMidi(msg);
  }

  void processAudio(AudioBuffer &buffer) {
    for(size_t i=0; i<param_count; ++i)
      params[i]->update(this, context);
    size_t channels = buffer.getChannels();
    size_t ch;
    for(ch=0; ch<channels; ++ch)
      buffers[ch] = buffer.getSamples(ch);
#ifdef GEN_OUTPUT_MAP
    size_t len = buffer.getSize();
    gen::perform(context, buffers, channels, outbuffers, gen::num_outputs(), len);
    for(ch=0; ch<channels; ++ch)
      buffer.getSamples(ch).clear();
    for(size_t m = 0; m < sizeof(gen_output_map)/sizeof(gen_output_map[0]); ++m){
      const GenOutputMapping& o = gen_output_map[m];
      if(o.output >= gen::num_outputs())
	continue;
      FloatArray out(outbuffers[o.output], len);
      if(o.kind == GEN_OUTPUT_AUDIO){
	if(o.id < channels)
	  buffer.getSamples(o.id).add(out);
      }else if(o.kind == GEN_OUTPUT_BUTTON){
	setButton((PatchButtonId)o.id, out.getMaxValue() > 0.5f);
      }else{
	float value = out.getMean();
	if(outper[m] >= 0){
	  float per;
	  gen::getparameter(context, outper[m], &per);
	  value = per != 0 ? value / per : 0;
	}
	if(o.bipolar)
	  value = (value + 1) * 0.5f;
	setParameterValue((PatchParameterId)o.id, value);
      }
    }
    return;
#endif
    gen::perform(context, buffers, channels, buffers, gen::num_outputs(), buffer.getSize());
    for(size_t i=0; i<outputs.getSize(); ++i){
      if(outputs[i].id > 0x100){
	float maxvalue = outputs[i].buffer.getMaxValue();
	setButton((PatchButtonId)(outputs[i].id-0x100), maxvalue > 0.5);
      }else{
	float avg = outputs[i].buffer.getMean();
	setParameterValue((PatchParameterId)outputs[i].id, avg);      
      }
    }
  }
};

#endif // __GenPatch_hpp__
