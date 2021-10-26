/* ------------------------------------------------------------
name: "FaustTest"
Code generated with Faust 2.30.14 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -light -mem -es 1 -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

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
#include "VoltsPerOctave.h"
#ifdef SOUNDFILE
#include "Resource.h"
#include "WaveReader.h"
#endif

// We have to undefine min/max from OWL's basicmaths.h, otherwise they cause
// errors when Faust calls functions with the same names in std:: namespace
#undef min
#undef max

#include <string.h>
#include <strings.h>

#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__

/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/
/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef __meta__
#define __meta__

/**
 The base class of Meta handler to be used in dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/
#ifdef SOUNDFILE
// Parent soundfile object uses exceptions in one of its method.
// This is probably the easiest may to make GCC happy about it when we compile
// with exceptions disabled.
#define try if(true)
#define catch(x) if(false)
/************************** BEGIN Soundfile.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef __Soundfile__
#define __Soundfile__

#include <string.h>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#define BUFFER_SIZE 1024
#define SAMPLE_RATE 44100
#define MAX_CHAN 64
#define MAX_SOUNDFILE_PARTS 256

#ifdef _MSC_VER
#define PRE_PACKED_STRUCTURE __pragma(pack(push, 1))
#define POST_PACKED_STRUCTURE \
    ;                         \
    __pragma(pack(pop))
#else
#define PRE_PACKED_STRUCTURE
#define POST_PACKED_STRUCTURE __attribute__((__packed__))
#endif

/*
 The soundfile structure to be used by the DSP code. Soundfile has a MAX_SOUNDFILE_PARTS parts 
 (even a single soundfile or an empty soundfile). 
 fLength, fOffset and fSR fields are filled accordingly by repeating
 the actual parts if needed.
 
 It has to be 'packed' to that the LLVM backend can correctly access it.

 Index computation:
    - p is the current part number [0..MAX_SOUNDFILE_PARTS-1] (must be proved by the type system)
    - i is the current position in the part. It will be constrained between [0..length]
    - idx(p,i) = fOffset[p] + max(0, min(i, fLength[p]));
*/

PRE_PACKED_STRUCTURE
struct Soundfile {
    FAUSTFLOAT** fBuffers;
    int* fLength;   // length of each part (so fLength[P] contains the length in frames of part P)
    int* fSR;       // sample rate of each part (so fSR[P] contains the SR of part P)
    int* fOffset;   // offset of each part in the global buffer (so fOffset[P] contains the offset in frames of part P)
    int fChannels;  // max number of channels of all concatenated files

    Soundfile()
    {
        fBuffers  = nullptr;
        fChannels = -1;
        fLength   = new int[MAX_SOUNDFILE_PARTS];
        fSR       = new int[MAX_SOUNDFILE_PARTS];
        fOffset   = new int[MAX_SOUNDFILE_PARTS];
    }
 
    ~Soundfile()
    {
        // Free the real channels only
        for (int chan = 0; chan < fChannels; chan++) {
            delete[] fBuffers[chan];
        }
        delete[] fBuffers;
        delete[] fLength;
        delete[] fSR;
        delete[] fOffset;
    }

} POST_PACKED_STRUCTURE;

/*
 The generic soundfile reader.
 */

class SoundfileReader {
    
   protected:
    
    int fDriverSR;
    
    void emptyFile(Soundfile* soundfile, int part, int& offset)
    {
        soundfile->fLength[part] = BUFFER_SIZE;
        soundfile->fSR[part] = SAMPLE_RATE;
        soundfile->fOffset[part] = offset;
        // Update offset
        offset += soundfile->fLength[part];
    }

    Soundfile* createSoundfile(int cur_chan, int length, int max_chan)
    {
        Soundfile* soundfile = new Soundfile();
        soundfile->fBuffers = new FAUSTFLOAT*[max_chan];
        
        for (int chan = 0; chan < cur_chan; chan++) {
            soundfile->fBuffers[chan] = new FAUSTFLOAT[length];
            memset(soundfile->fBuffers[chan], 0, sizeof(FAUSTFLOAT) * length);
        }
        
        soundfile->fChannels = cur_chan;
        return soundfile;
    }

    void getBuffersOffset(Soundfile* soundfile, FAUSTFLOAT** buffers, int offset)
    {
        for (int chan = 0; chan < soundfile->fChannels; chan++) {
            buffers[chan] = &soundfile->fBuffers[chan][offset];
        }
    }
    
    // Check if a soundfile exists and return its real path_name
    std::string checkFile(const std::vector<std::string>& sound_directories, const std::string& file_name)
    {
        if (checkFile(file_name)) {
            return file_name;
        } else {
            for (size_t i = 0; i < sound_directories.size(); i++) {
                std::string path_name = sound_directories[i] + "/" + file_name;
                if (checkFile(path_name)) { return path_name; }
            }
            return "";
        }
    }
    
    bool isResampling(int sample_rate) { return (fDriverSR > 0 && fDriverSR != sample_rate); }
 
    // To be implemented by subclasses

    /**
     * Check the availability of a sound resource.
     *
     * @param path_name - the name of the file, or sound resource identified this way
     *
     * @return true if the sound resource is available, false otherwise.
     */
    virtual bool checkFile(const std::string& path_name) = 0;
    
    /**
     * Check the availability of a sound resource.
     *
     * @param buffer - the sound buffer
     * @param size - the sound buffer length
     *
     * @return true if the sound resource is available, false otherwise.
     */

    virtual bool checkFile(unsigned char* buffer, size_t size) { return true; }

    /**
     * Get the channels and length values of the given sound resource.
     *
     * @param path_name - the name of the file, or sound resource identified this way
     * @param channels - the channels value to be filled with the sound resource number of channels
     * @param length - the length value to be filled with the sound resource length in frames
     *
     */
    virtual void getParamsFile(const std::string& path_name, int& channels, int& length) = 0;
    
    /**
     * Get the channels and length values of the given sound resource.
     *
     * @param buffer - the sound buffer
     * @param size - the sound buffer length
     * @param channels - the channels value to be filled with the sound resource number of channels
     * @param length - the length value to be filled with the sound resource length in frames
     *
     */
    virtual void getParamsFile(unsigned char* buffer, size_t size, int& channels, int& length) {}

    /**
     * Read one sound resource and fill the 'soundfile' structure accordingly
     *
     * @param soundfile - the soundfile to be filled
     * @param path_name - the name of the file, or sound resource identified this way
     * @param part - the part number to be filled in the soundfile
     * @param offset - the offset value to be incremented with the actual sound resource length in frames
     * @param max_chan - the maximum number of mono channels to fill
     *
     */
    virtual void readFile(Soundfile* soundfile, const std::string& path_name, int part, int& offset, int max_chan) = 0;
    
    /**
     * Read one sound resource and fill the 'soundfile' structure accordingly
     *
     * @param soundfile - the soundfile to be filled
     * @param buffer - the sound buffer
     * @param size - the sound buffer length
     * @param part - the part number to be filled in the soundfile
     * @param offset - the offset value to be incremented with the actual sound resource length in frames
     * @param max_chan - the maximum number of mono channels to fill
     *
     */
    virtual void readFile(Soundfile* soundfile, unsigned char* buffer, size_t size, int part, int& offset, int max_chan) {}

  public:
    
    virtual ~SoundfileReader() {}
    
    void setSampleRate(int sample_rate) { fDriverSR = sample_rate; }
   
    Soundfile* createSoundfile(const std::vector<std::string>& path_name_list, int max_chan)
    {
        try {
            int cur_chan = 1; // At least one channel
            int total_length = 0;
            
            // Compute total length and channels max of all files
            for (int i = 0; i < int(path_name_list.size()); i++) {
                int chan, length;
                if (path_name_list[i] == "__empty_sound__") {
                    length = BUFFER_SIZE;
                    chan = 1;
                } else {
                    getParamsFile(path_name_list[i], chan, length);
                }
                cur_chan = std::max<int>(cur_chan, chan);
                total_length += length;
            }
           
            // Complete with empty parts
            total_length += (MAX_SOUNDFILE_PARTS - path_name_list.size()) * BUFFER_SIZE;
            
            // Create the soundfile
            Soundfile* soundfile = createSoundfile(cur_chan, total_length, max_chan);
            
            // Init offset
            int offset = 0;
            
            // Read all files
            for (int i = 0; i < int(path_name_list.size()); i++) {
                if (path_name_list[i] == "__empty_sound__") {
                    emptyFile(soundfile, i, offset);
                } else {
                    readFile(soundfile, path_name_list[i], i, offset, max_chan);
                }
            }
            
            // Complete with empty parts
            for (int i = int(path_name_list.size()); i < MAX_SOUNDFILE_PARTS; i++) {
                emptyFile(soundfile, i, offset);
            }
            
            // Share the same buffers for all other channels so that we have max_chan channels available
            for (int chan = cur_chan; chan < max_chan; chan++) {
                soundfile->fBuffers[chan] = soundfile->fBuffers[chan % cur_chan];
            }
            
            return soundfile;
            
        } catch (...) {
            return nullptr;
        }
    }

    // Check if all soundfiles exist and return their real path_name
    std::vector<std::string> checkFiles(const std::vector<std::string>& sound_directories,
                                        const std::vector<std::string>& file_name_list)
    {
        std::vector<std::string> path_name_list;
        for (size_t i = 0; i < file_name_list.size(); i++) {
            std::string path_name = checkFile(sound_directories, file_name_list[i]);
            // If 'path_name' is not found, it is replaced by an empty sound (= silence)
            path_name_list.push_back((path_name == "") ? "__empty_sound__" : path_name);
        }
        return path_name_list;
    }

};

#endif
/**************************  END  Soundfile.h **************************/

#define MAX_SOUNDFILES 32
// This is just value for upper limit - actual memory used depends only on number of files loaded
#endif

static float fKey, fFreq, fGain, fGate;
static float fBend = 1.0f;

class MonoVoiceAllocator {
    float& key;
    float& freq;
    float& gain;
    float& gate;
    float& bend;
    uint8_t notes[16];
    uint8_t lastNote = 0;

public:
    MonoVoiceAllocator(float& ky, float& fq, float& gn, float& gt, float& bd)
        : key(ky)
        , freq(fq)
        , gain(gn)
        , gate(gt)
        , bend(bd) {
    }
    float getFreq() {
        return freq;
    }
    float getGain() {
        return gain;
    }
    float getGate() {
        return gate;
    }
    float getBend() {
        return bend;
    }
    void processMidi(MidiMessage msg) {
        uint16_t samples = 0;
        if (msg.isNoteOn()) {
            noteOn(msg.getNote(), (uint16_t)msg.getVelocity() << 5, samples);
        }
        else if (msg.isNoteOff()) {
            noteOff(msg.getNote(), (uint16_t)msg.getVelocity() << 5, samples);
        }
        else if (msg.isPitchBend()) {
            setPitchBend(msg.getPitchBend());
        }
        else if (msg.isControlChange()) {
            if (msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
                allNotesOff();
        }
    }
    void setPitchBend(int16_t pb) {
        float fb = pb * (2.0f / 8192.0f);
        bend = exp2f(fb);
    }
    float noteToHz(uint8_t note) {
        return 440.0f * exp2f((note - 69) / 12.0);
    }
    float velocityToGain(uint16_t velocity) {
        return exp2f(velocity / 4095.0f) - 1;
        // return powf((1.0-depth) + (velocity/127.0*depth), 2.0);
    }
    void noteOn(uint8_t note, uint16_t velocity, uint16_t delay) {
        if (lastNote < 16)
            notes[lastNote++] = note;
	key = note;
        freq = noteToHz(note);
        gain = velocityToGain(velocity);
        gate = 1.0f;
    }
    void noteOff(uint8_t note, uint16_t velocity, uint16_t delay) {
        int i;
        for (i = 0; i < lastNote; ++i) {
            if (notes[i] == note)
                break;
        }
        if (lastNote > 1) {
            lastNote--;
            while (i < lastNote) {
                notes[i] = notes[i + 1];
                i++;
            }
	    key = notes[lastNote - 1];
            freq = noteToHz(key);
        }
        else {
            gate = 0.0f;
            lastNote = 0;	    
        }
    }
    void allNotesOff() {
        lastNote = 0;
        bend = 0;
    }
};

enum ParserState {
    ST_NONE,
    ST_KEY,
    ST_VALUE,
};

/*************************************************************************************
 * To enable MIDI in current patch, add this to source Faust file:
 *     declare options "[midi:on]";
 *
 * You can also add a description to be displayed like this:
 *     declare message "Hello World";
 *
 * To add V/Oct scaling support, use something likes this:
 *     declare owl "[voct:input][voct:output]";
 * Declaring just one V/Oct converter or both of them is fine.
 **************************************************************************************/

class MetaData : public Meta {
public:
    bool midiOn = false;
    const char* message = NULL;
    bool vOctInput = false;
    bool vOctOutput = false;

    void declare(const char* key, const char* value) {
        /*
          This function would be called by Faust compiler based on metadata
          declared in source file.
         */
        if (strcasecmp(key, "options") == 0) {
            // Parse options.
            // Faust provides a metadata parser, but they use stdlib stuff that won't work on OWL
            parseOptions(key, value);
        }
        else if (strcasecmp(key, "message") == 0) {
            message = value;
        }
        else if (strcasecmp(key, "owl") == 0) {
            parseOptions(key, value);
        }
    }

private:
    void processItem(const char* key, const char* item_key, const char* item_value) {
        if (strcasecmp(key, "options") == 0 && strcasecmp(item_key, "midi") == 0 &&
            strcasecmp(item_value, "on") == 0) {
            midiOn = true;
        }
        else if (strcasecmp(key, "owl") == 0 && strcasecmp(item_key, "voct") == 0) {
            if (strcasecmp(item_value, "input") == 0) {
                vOctInput = true;
            }
            else if (strcasecmp(item_value, "output") == 0) {
                vOctOutput = true;
            }
        }
    }

    void parseOptions(const char* key, const char* value) {
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
                if (*c == ']') {
                    // Found end of value
                    item_value[value_pos + 1] = '\n';
                    processItem(key, item_key, item_value);
                    // Reset state
                    state = ST_NONE;
                }
                else if (value_pos < max_len - 1)
                    // Add next character to value
                    item_value[value_pos++] = *c;
            };
            c++;
        }
    }
};

/**************************************************************************************
 *
 * OwlParameter : object used by OwlUI to ensures the connection between an OWL
 * parameter and a FAUST widget
 *
 ***************************************************************************************/

class OwlParameterBase {
protected:
    Patch* fPatch; // needed to register and read owl parameters
    FAUSTFLOAT* fZone; // Faust widget zone
    bool isOutput;

public:
    OwlParameterBase(Patch* pp, FAUSTFLOAT* z, bool output)
        : fPatch(pp)
        , fZone(z)
        , isOutput(output) {
    }
    virtual void update() {
    }
    virtual ~OwlParameterBase() {}; // Not adding virtual destructor triggers undefined behavior
};

class OwlParameter : public OwlParameterBase {
protected:
    PatchParameterId fParameter; // OWL parameter code : PARAMETER_A,...
    float fMin; // Faust widget minimal value
    float fSpan; // Faust widget value span (max-min)

public:
    OwlParameter(Patch* pp, PatchParameterId param, FAUSTFLOAT* z,
        const char* l, float init, float lo, float hi, bool output = false)
        : OwlParameterBase(pp, z, output)
        , fParameter(param)
        , fMin(lo)
        , fSpan(hi - lo) {
        fPatch->registerParameter(param, l);
        fPatch->setParameterValue(fParameter, (init - lo) / fSpan);
    }
    void update() {
        if (isOutput) {
            fPatch->setParameterValue(fParameter, (*fZone - fMin) / fSpan);
        }
        else
            *fZone = fMin + fSpan * fPatch->getParameterValue(fParameter);
    }
    ~OwlParameter() {};
};

class OwlVariable : public OwlParameterBase {
protected:
    float* fValue;
    float fLo; // Faust widget minimal value
    float fHi; // Faust widget value span (max-min)

public:
    OwlVariable(Patch* pp, float* t, FAUSTFLOAT* z, const char* l, float init,
        float lo, float hi, bool output = false)
        : OwlParameterBase(pp, z, output)
        , fValue(t)
        , fLo(lo)
        , fHi(hi) {
        *fZone = init;
    }
    void update() {
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
    ~OwlVariable() {};
};

class OwlButton : public OwlParameterBase {
protected:
    PatchButtonId fButton; // OWL button id : PUSHBUTTON, ...
public:
  OwlButton(Patch* pp, PatchButtonId button, FAUSTFLOAT* z, const char* l, bool output=false) :
    OwlParameterBase(pp, z, output), fButton(button) {}
  void update()	{
    if(isOutput){
      fPatch->setButton(fButton, *fZone,  0);
    }else{
      *fZone = fPatch->isButtonPressed(fButton);
    }
  }
};

class OwlCheckbox : public OwlParameterBase {
protected:
    PatchButtonId fButton; // OWL button id : PUSHBUTTON, ...
    bool wasHigh = false; // Flag for edge detection
    bool state = false;   // Current state
public:
    OwlCheckbox(Patch* pp, PatchButtonId button, FAUSTFLOAT* z, const char* l)
        : OwlParameterBase(pp, z, false)
        , fButton(button) {}
    void update() {
        bool isHigh = fPatch->isButtonPressed(fButton);
        state ^= isHigh && !wasHigh;
        wasHigh = isHigh;
        fPatch->setButton(fButton, state,  0);
        *fZone = state;
    }
};

/**************************************************************************************
 *
 * OwlResourceReader: Reads resources from flash storage and returns Soundfile objects.
 *
 * This is based on SoundfileReader in faust/gui/Soundfile.h with some changes:
 * - no allocation of excessive amounts of RAM for empty buffers
 * - no stdlib usage of vectors, maps,
 * - exception handling removed
 *
 ***************************************************************************************/

#define EMPTY_BUFFER_SIZE 0

#ifdef SOUNDFILE
class OwlResourceReader {
public:
    virtual ~OwlResourceReader() {}

    void setSampleRate(int sample_rate) { fDriverSR = sample_rate; }

    Soundfile* createSoundfile(const char** path_name_list, int path_name_size, int max_chan) {
        Soundfile *soundfile = NULL;
        int cur_chan = 1; // At least one channel
        int total_length = 1; // At least one byte - otherwise we can't allocate empty channels

        // Compute total length and channels max of all files
        for (int i = 0; i < path_name_size; i++) {
            int chan, length;
            if (strcmp(path_name_list[i], "__empty_sound__") == 0) {
                length = EMPTY_BUFFER_SIZE;
                chan = 1;
            } else {
                getParamsFile(path_name_list[i], chan, length);
            }
            cur_chan = std::max<int>(cur_chan, chan);
            total_length += length;
        }

        // Complete with empty parts
        total_length += (MAX_SOUNDFILE_PARTS - path_name_size) * EMPTY_BUFFER_SIZE;

        // Create the soundfile
        soundfile = createSoundfile(cur_chan, total_length, max_chan);

        // Init offset
        int offset = 0;

        // Read all files
        for (int i = 0; i < path_name_size; i++) {
            if (strcmp(path_name_list[i], "__empty_sound__") == 0){
                emptyFile(soundfile, i, offset);
            } else {
                readFile(soundfile, path_name_list[i], i, offset, max_chan);
            }
        }

        // Complete with empty parts
        for (int i = path_name_size; i < MAX_SOUNDFILE_PARTS; i++) {
            emptyFile(soundfile, i, offset);
        }

        // Share the same buffers for all other channels so that we have max_chan channels available
        for (int chan = cur_chan; chan < max_chan; chan++) {
            soundfile->fBuffers[chan] = soundfile->fBuffers[chan % cur_chan];
        }
        return soundfile;
    }

    /**
     * Check the availability of a sound resource.
     *
     * @param resource_name - the name of the file, or sound resource identified this way
     *
     * @return true if the sound resource is available, false otherwise.
     */
    bool checkFile(const char* resource_name) {
        Resource* resource = Resource::open(resource_name);
        bool result = resource != NULL;
        Resource::destroy(resource);
        if (!result){
            debugMessage("Resource not found");
        }
        return result;
    }

protected:
    int fDriverSR;

    void emptyFile(Soundfile* soundfile, int part, int& offset) {
        soundfile->fLength[part] = EMPTY_BUFFER_SIZE;
        soundfile->fSR[part] = SAMPLE_RATE;
        soundfile->fOffset[part] = offset;
        // Update offset
        offset += soundfile->fLength[part];
    }

    Soundfile* createSoundfile(int cur_chan, int length, int max_chan) {
        Soundfile* soundfile = new Soundfile();
        soundfile->fBuffers = new FAUSTFLOAT*[max_chan];

        for (int chan = 0; chan < cur_chan; chan++) {
            soundfile->fBuffers[chan] = new FAUSTFLOAT[length];
            memset(soundfile->fBuffers[chan], 0, sizeof(FAUSTFLOAT) * length);
        }

        soundfile->fChannels = cur_chan;
        return soundfile;
    }

    void getBuffersOffset(Soundfile* soundfile, FAUSTFLOAT** buffers, int offset) {
        for (int chan = 0; chan < soundfile->fChannels; chan++) {
            buffers[chan] = &soundfile->fBuffers[chan][offset];
        }
    }

    /**
     * Get the channels and length values of the given sound resource.
     *
     * @param path_name - the name of the file, or sound resource identified this way
     * @param channels - the channels value to be filled with the sound resource number of channels
     * @param length - the length value to be filled with the sound resource length in frames
     *
     */
    void getParamsFile(const char* path_name, int& channels, int& length) {
        WaveResourceReader reader(path_name);
        channels = 1;
        length = 0;

        if (reader.loadWaveHeader()){
            channels = reader.fWave->num_channels;
            length = (reader.fWave->subchunk_2_size * 8) / (reader.fWave->num_channels * reader.fWave->bits_per_sample);
        }
    }

    /**
     * Read one sound resource and fill the 'soundfile' structure accordingly
     *
     * @param soundfile - the soundfile to be filled
     * @param path_name - the name of the file, or sound resource identified this way
     * @param part - the part number to be filled in the soundfile
     * @param offset - the offset value to be incremented with the actual sound resource length in frames
     * @param max_chan - the maximum number of mono channels to fill
     *
     */
    void readFile(Soundfile* soundfile, const char* path_name, int part, int& offset, int max_chan) {
        WaveResourceReader reader(path_name);
        if (!reader.loadWaveHeader()){
            return;
        }
        reader.loadWave();

        soundfile->fLength[part] = (reader.fWave->subchunk_2_size * 8) / (reader.fWave->num_channels * reader.fWave->bits_per_sample);
        soundfile->fSR[part] = reader.fWave->sample_rate;
        soundfile->fOffset[part] = offset;

        // Audio frames have to be written for each chan
        switch (reader.fWave->bits_per_sample){
        case 16:
            // 16 bit samples are signed ints
            for (int sample = 0; sample < soundfile->fLength[part]; sample++) {
                float factor = 1.f/32767.f;
                int16_t* frame = (int16_t*)&reader.fWave->data[reader.fWave->block_align * sample];
                for (int chan = 0; chan < reader.fWave->num_channels; chan++) {
                    soundfile->fBuffers[chan][offset + sample] = frame[chan] * factor;
                }
            }
            break;
        case 8:
            // 8 bit samples are unsigned ints
            for (int sample = 0; sample < soundfile->fLength[part]; sample++) {
                float factor = 1.f/255.f;
                uint8_t* frame = (uint8_t*)&reader.fWave->data[reader.fWave->block_align * sample];
                for (int chan = 0; chan < reader.fWave->num_channels; chan++) {
                    soundfile->fBuffers[chan][offset + sample] = frame[chan] * factor - 0.5f;
                }
            }
            break;
        default:
            debugMessage("Unsupported format");
            return;
        }

        //
        // Update offset
        offset += soundfile->fLength[part];
    }
};
#endif /* SOUNDFILE */


/**************************************************************************************
 *
 * OwlUI : Faust User Interface builder. Passed to buildUserInterface OwlU
 * ensures the mapping between owl parameters and faust widgets. It relies on
 * specific metadata "...[OWL:PARAMETER_X]..." in widget's label for that. For
 * example any faust widget with metadata [OWL:B] will be controlled by
 * PARAMETER_B (the second knob).
 *
 ***************************************************************************************/

// The maximun number of mappings between owl parameters and faust widgets
#define MAXOWLPARAMETERS 20
#define NO_PARAMETER     ((PatchParameterId)-1)
#define NO_BUTTON        ((PatchButtonId)-1)
#define PATH_LEN         24

MonoVoiceAllocator allocator(fKey, fFreq, fGain, fGate, fBend);
VoltsPerOctave* fVOctInput;
VoltsPerOctave* fVOctOutput;
Soundfile* defaultsound;

class OwlUI : public UI {
    Patch* fPatch;
    PatchParameterId fParameter; // current parameter ID, value NO_PARAMETER means not set
    int fParameterIndex;         // number of OwlParameters collected so far
    int fSampleRate;
    //std::map<std::string, Soundfile*> fSoundfileMap;    // Map to share loaded soundfiles
    OwlParameterBase* fParameterTable[MAXOWLPARAMETERS];
#ifdef SOUNDFILE
    int fSoundfileIndex;
    OwlResourceReader* fSoundReader = NULL;
    Soundfile* fSoundfiles[MAX_SOUNDFILES];
#endif
    PatchButtonId fButton;
    // check if the widget is an Owl parameter and, if so, add the corresponding OwlParameter
    void addInputOwlParameter(const char* label, FAUSTFLOAT* zone,
        FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi) {
        if (fParameterIndex < MAXOWLPARAMETERS) {
            if (meta.midiOn && strcasecmp(label, "freq") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fFreq, zone, label, init, lo, hi);
            }
            else if (meta.midiOn && strcasecmp(label, "gain") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fGain, zone, label, init, lo, hi);
            }
            else if (meta.midiOn && strcasecmp(label, "bend") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fBend, zone, label, init, lo, hi);
            }
            else if (meta.midiOn && strcasecmp(label, "key") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fKey, zone, label, init, lo, hi);
            }
            else if (fParameter != NO_PARAMETER) {
                fParameterTable[fParameterIndex++] =
                    new OwlParameter(fPatch, fParameter, zone, label, init, lo, hi);
            }
        }
        fParameter = NO_PARAMETER; // clear current parameter ID
	    fButton = NO_BUTTON;
    }

    void addOutputOwlParameter(
        const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
        if(label[strlen(label) - 1] != '>')
            debugMessage("Add '>' character for output parameters");
        if (fParameterIndex < MAXOWLPARAMETERS) {
	    if (fParameter != NO_PARAMETER) {
                fParameterTable[fParameterIndex++] = new OwlParameter(
                    fPatch, fParameter, zone, label, lo, lo, hi, true);
            }
            else if(fButton != NO_BUTTON){
                fParameterTable[fParameterIndex++] = new OwlButton(
                fPatch, fButton, zone, label, true);
            }
        }
        fParameter = NO_PARAMETER;
        fButton = NO_BUTTON;
    }

    void addOwlButton(const char* label, FAUSTFLOAT* zone) {
        if (fParameterIndex < MAXOWLPARAMETERS) {
            if (meta.midiOn && strcasecmp(label, "gate") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fGate, zone, label, 0.0f, 0.0f, 1.0f);
            }
            else if (fButton != NO_BUTTON) {
                fParameterTable[fParameterIndex++] =
                    new OwlButton(fPatch, fButton, zone, label);
            }
        }
        fParameter = NO_PARAMETER;
        fButton = NO_BUTTON; // clear current button ID
    }
#ifdef SOUNDFILE
    void addOwlResources(const char** resource_names, int names_size, Soundfile** sf_zone){
        if (fSoundfileIndex < MAX_SOUNDFILES){
            Soundfile* sound_file = fSoundReader->createSoundfile(resource_names, names_size, MAX_CHAN);
            fSoundfiles[fSoundfileIndex++] = sound_file;
            if (sound_file != NULL) {
                *sf_zone = sound_file;
                return;
            }
        }
        // If failure, use 'defaultsound'
        *sf_zone = defaultsound;
    }
#endif /* SOUNDFILE */
    void addOwlCheckbox(const char* label, FAUSTFLOAT* zone) {
        if (fParameterIndex < MAXOWLPARAMETERS) {
            if (meta.midiOn && strcasecmp(label, "gate") == 0) {
                fParameterTable[fParameterIndex++] =
                    new OwlVariable(fPatch, &fGate, zone, label, 0.0f, 0.0f, 1.0f);
            }
            else if (fButton != NO_BUTTON) {
                fParameterTable[fParameterIndex++] =
                    new OwlCheckbox(fPatch, fButton, zone, label);
            }
        }
        fParameter = NO_PARAMETER;
        fButton = NO_BUTTON; // clear current button ID
    }

    // we dont want to create a widget but we clear the current parameter ID just in case
    void skip() {
        fParameter = NO_PARAMETER; // clear current parameter ID
        fButton = NO_BUTTON;
    }

public:
    MetaData meta;
    OwlUI(Patch* pp)
        : fPatch(pp)
        , fParameter(NO_PARAMETER)
        , fParameterIndex(0)
#ifdef SOUNDFILE
        , fSoundfileIndex(0)
#endif
        , fButton(NO_BUTTON) {
    }

    virtual ~OwlUI() {
        for (int i = 0; i < fParameterIndex; i++)
            delete fParameterTable[i];
        if (meta.vOctInput)
            delete fVOctInput;
        if (meta.vOctOutput)
            delete fVOctOutput;
#ifdef SOUNDFILE
        if (fSoundReader != NULL) {
            delete fSoundReader;
        }
        for (int i = 0; i < fSoundfileIndex; i++){
            delete fSoundfiles[i];
        }
#endif
    }

    // should be called before compute() to update widget's zones registered as OWL parameters
    void update() {
        for (int i = 0; i < fParameterIndex; i++)
            fParameterTable[i]->update();
    }

    //---------------------- virtual methods called by buildUserInterface ----------------

    // -- widget's layouts

    virtual void openTabBox(const char* label) {
    }
    virtual void openHorizontalBox(const char* label) {
    }
    virtual void openVerticalBox(const char* label) {
    }
    virtual void closeBox() {
    }

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) {
        addOwlButton(label, zone);
    }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {
        addOwlCheckbox(label, zone);
    }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone,
        FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) {
        addInputOwlParameter(label, zone, init, lo, hi);
    }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone,
        FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) {
        addInputOwlParameter(label, zone, init, lo, hi);
    }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone,
        FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) {
        addInputOwlParameter(label, zone, init, lo, hi);
    }

    // -- passive widgets

    virtual void addHorizontalBargraph(
        const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
        addOutputOwlParameter(label, zone, lo, hi);
    }
    virtual void addVerticalBargraph(
        const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
        addOutputOwlParameter(label, zone, lo, hi);
    }
    // -- soundfiles
    virtual void addSoundfile(const char* label, const char* url, Soundfile** sf_zone) {
#ifdef SOUNDFILE
        if (fSoundReader == NULL){
            fSoundReader = new OwlResourceReader();
            fSoundReader->setSampleRate(fPatch->getSampleRate());
            defaultsound = fSoundReader->createSoundfile(nullptr, 0, 1);
        }

        if (url[0] == '{'){
            char* paths[MAX_SOUNDFILE_PARTS];
            int total_paths = 0;
            bool reading_path = false;
            const char* c = url;
            const char* start_path;
            while (c){
                if (reading_path){
                    if (*c++ == '\''){
                        // Closing quote
                        reading_path = false;
                        uint32_t path_len = std::min<uint32_t>(c - start_path, PATH_LEN - 1);
                        paths[total_paths] = new char[path_len];
                        strncpy(paths[total_paths], start_path, path_len - 1);
                        paths[total_paths][path_len -1] = 0;
                        if (fSoundReader->checkFile(paths[total_paths]))
                            total_paths++;
                        else {
                            delete[] paths[total_paths];
                        }
                    }
                }
                else {
                    switch (*c){
                    case '\'':
                        // Opening quote
                        start_path = ++c;
                        reading_path = true;
                        break;
                    case '}':
                        c = 0; // Done!
                        break;
                    default:
                        // Path separator will end here
                        c++;
                        break;
                    }
                }
            }
            addOwlResources((const char**)paths, total_paths, sf_zone);
            // Parsed path names are not used after loading resources
            for (int i = 0; i < total_paths; i++){
                delete[] paths[i];
            }
        }
#else
        fParameter = NO_PARAMETER; // clear current parameter ID
        fButton = NO_BUTTON;
#endif /* SOUNDFILE */
    }

    // -- metadata declarations

    virtual void declare(FAUSTFLOAT* z, const char* k, const char* id) {
        if (strcasecmp(k, "OWL") == 0) {
            if (strncasecmp(id, "PARAMETER_", 10) == 0)
                id += 10;
            if (strlen(id) == 1) {
                // Single char parameter name.
                // Note that we can use I - P as aliases for AA-AH.
                // Rationale: Magus uses only single letters param names, has
                // no free space on display for second letter.
                if (*id >= 'A' && *id <= 'P') {
                    // Uppercase parameter name
                    fParameter = PatchParameterId(*id - 'A');
                }
                else if (*id >= '0' && *id <= '9') {
                    // Parameter can be numeric as well
                    fParameter = PatchParameterId(*id - '0');
                }
                else if (*id >= 'a' && *id <= 'P') {
                    // Lowercase parameter name
                    fParameter = PatchParameterId(*id - 'a');
                }
            }
            else if (strlen(id) == 2) {
                int param_tmp = -1;
                bool is_digit = false;
                if (*id >= 'A' && *id <= 'D') {
                    // Uppercase parameter name
                    param_tmp = *id - 'A';
                }
                else if (*id >= '0' && *id <= '9') {
                    // Parameter can be numeric as well
                    is_digit = true;
                    fParameter = PatchParameterId(*id - '0');
                }
                else if (*id >= 'a' && *id <= 'd') {
                    // Lowercase parameter name
                    param_tmp = *id - 'a';
                }
                if (param_tmp != -1) {
                    id++;
                    if (is_digit) {
                        // Get second digit for decimal param
                        param_tmp *= 10;
                        if (*id >= '0' && *id <= '9') {
                            fParameter = PatchParameterId(
                                std::min(int(PARAMETER_DH), param_tmp + *id - '0'));
                        }
                    }
                    else if (param_tmp == PARAMETER_B && *id > '0' && *id <= '9') {
                        fButton = PatchButtonId(BUTTON_A + *id - '1');
                    }
                    else {
                        // Inc to skip 1-character params
                        param_tmp++;
                        // This is first character for groups of 8 params
                        param_tmp *= 8;
                        // Second char for parameter name
                        if (*id >= 'A' && *id <= 'H') {
                            // Uppercase parameter name
                            fParameter = PatchParameterId(param_tmp + *id - 'A');
                        }
                        else if (*id >= 'a' && *id <= 'h') {
                            // Lowercase parameter name
                            fParameter = PatchParameterId(param_tmp + *id - 'a');
                        }
                    }
                }
            }
            else if (strcasecmp(id, "PUSH") == 0)
                fButton = PUSHBUTTON;
	    else if (strcasecmp(id, "ButtonA") == 0)
                fButton = BUTTON_A;
            else if (strcasecmp(id, "ButtonB") == 0)
                fButton = BUTTON_B;
            else if (strcasecmp(id, "ButtonC") == 0)
                fButton = BUTTON_C;
            else if (strcasecmp(id, "ButtonD") == 0)
                fButton = BUTTON_D;
	}
        // else if (strcasecmp(k, "midi") == 0) {
	//   if (strcasecmp(id, "pitchwheel") == 0){ // PB
	//   }else if (strcasecmp(id, "ctrl") == 0){ // CC
	//   }else if (strcasecmp(id, "chanpress") == 0){ // AT
	//   }else if (strcasecmp(id, "pgm") == 0){ // PC
	//   }
        // }
    }

    // -- V/Oct

    void addVOct() {
        if (meta.vOctInput)
            fVOctInput = new VoltsPerOctave(true);
        if (meta.vOctOutput)
            fVOctOutput = new VoltsPerOctave(false);
    }
};

/* Simple heap based memory manager.
 * Uses overloaded new/delete operators on OWL hardware.
 */
struct OwlMemoryManager : public dsp_memory_manager {
    void* allocate(size_t size) {
        void* res = new uint8_t[size];
        return res;
    }
    virtual void destroy(void* ptr) {
        delete (uint8_t*)ptr;
    }
};

#endif // __FaustCommonInfrastructure__

/**************************BEGIN USER SECTION **************************/


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include "faustmath.h"
#include <math.h>

class mydspSIG0 {
	
  private:
	
	int iRec0[2];
	
  public:
	
	int getNumInputsmydspSIG0() {
		return 0;
	}
	int getNumOutputsmydspSIG0() {
		return 1;
	}
	int getInputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitmydspSIG0(int sample_rate) {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iRec0[l0] = 0;
		}
	}
	
	void fillmydspSIG0(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec0[0] = (iRec0[1] + 1);
			table[i] = fast_sinf((9.58738019e-05f * float((iRec0[0] + -1))));
			iRec0[1] = iRec0[0];
		}
	}

};

static mydspSIG0* newmydspSIG0(dsp_memory_manager* manager) { return (mydspSIG0*)new(manager->allocate(sizeof(mydspSIG0))) mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp, dsp_memory_manager* manager) { dsp->~mydspSIG0(); manager->destroy(dsp); }

static float* ftbl0mydspSIG0 = 0;

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fHbargraph0;
	int fSampleRate;
	float fConst0;
	float fRec1[2];
	
 public:
	static dsp_memory_manager* fManager;
	
	void metadata(Meta* m) { 
		m->declare("compile_options", "-lang cpp -inpl -light -mem -es 1 -scal -ftz 0");
		m->declare("filename", "FaustTest.dsp");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/deprecated", "This library is deprecated and is not maintained anymore. It will be removed in August 2017.");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/version", "1.0");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/deprecated", "This library is deprecated and is not maintained anymore. It will be removed in August 2017.");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/version", "1.0");
		m->declare("name", "FaustTest");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	static void classInit(int sample_rate) {
		mydspSIG0* sig0 = newmydspSIG0(fManager);
		sig0->instanceInitmydspSIG0(sample_rate);
		ftbl0mydspSIG0 = static_cast<float*>(fManager->allocate(262144));
		sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
		deletemydspSIG0(sig0, fManager);
	}
	static void classDestroy() {
		fManager->destroy(ftbl0mydspSIG0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = (1000.0f / std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate))));
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("FaustTest");
		ui_interface->declare(&fCheckbox0, "OWL", "B1");
		ui_interface->addCheckButton("btn1", &fCheckbox0);
		ui_interface->declare(&fHbargraph0, "OWL", "B1");
		ui_interface->addHorizontalBargraph("led2>", &fHbargraph0, 0.0f, 1.0f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = float(fCheckbox0);
		fHbargraph0 = FAUSTFLOAT((1.0f - fSlow0));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fConst0 + (fRec1[1] - fast_floorf((fConst0 + fRec1[1]))));
			output0[i] = FAUSTFLOAT((fSlow0 * ftbl0mydspSIG0[int((65536.0f * fRec1[0]))]));
			fRec1[1] = fRec1[0];
		}
	}

};

dsp_memory_manager* mydsp::fManager = 0;

    /***************************END USER SECTION ***************************/

    /*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

    /**************************************************************************************

        FaustPatch : an OWL patch that calls Faust generated DSP code

    ***************************************************************************************/

    class FaustPatch : public Patch {
    mydsp* fDSP;
    OwlUI fUI;
    OwlMemoryManager mem;

public:
    FaustPatch()
        : fUI(this) {
        // // Allocate memory for dsp object first to ensure high priority
        // memory is used void* allocated = mem.allocate(sizeof(mydsp));
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
        if (fUI.meta.message != NULL)
            debugMessage(fUI.meta.message);
        fUI.addVOct();

        mydsp::fManager = &mem; // set custom memory manager
        mydsp::classInit(int(getSampleRate())); // initialise static tables
        fDSP->instanceInit(int(getSampleRate())); // initialise DSP instance
        fDSP->buildUserInterface(&fUI); // Map OWL parameters
    }

    ~FaustPatch() {
        delete fDSP;
        // mem.destroy(fDSP);
        // DSP static data is destroyed using classDestroy.
        mydsp::classDestroy();
    }

    void processMidi(MidiMessage msg) {
        if (fUI.meta.midiOn)
            allocator.processMidi(msg);
    }

    void processAudio(AudioBuffer& buffer) {
        // Reasonably assume we will not have more than 32 channels
        float* ins[32];
        float* outs[32];
        int n = buffer.getChannels();

        if ((fDSP->getNumInputs() < 32) && (fDSP->getNumOutputs() < 32)) {
            // create the table of input channels
            for (int ch = 0; ch < fDSP->getNumInputs(); ++ch) {
                ins[ch] = buffer.getSamples(ch % n);
            }

            // create the table of output channels
            for (int ch = 0; ch < fDSP->getNumOutputs(); ++ch) {
                outs[ch] = buffer.getSamples(ch % n);
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
  void doSetPatchParameter(uint8_t id, int16_t value);

  int owl_pushbutton(int value) {
    static bool state = 0;
    static uint16_t counter = 0;
    value = (bool)value;
    if (state != value) {
      state = value;
      doSetButton(PUSHBUTTON, state, counter);
    }
    if (++counter > getProgramVector()->audio_blocksize)
      counter = 0;
    return value;
  }
  int owl_button(int bid, int value){
    doSetButton(bid, value, 0);
    return value;
  }
  float owl_parameter(int pid, float value){
    doSetPatchParameter(pid, (int16_t)(value*4096));
    return value;
  }

  // Uses input scaler and tuning
  float sample2hertz(float tune, float sample) {
    fVOctInput->setTune(tune);
    return fVOctInput->getFrequency(sample);
  }
  // Uses output scaler and tuning
  float hertz2sample(float tune, float hertz) {
    fVOctOutput->setTune(tune);
    return fVOctOutput->getSample(hertz);
  }
  // Uses input scaler
  float sample2volts(float sample) {
    return fVOctInput->sampleToVolts(sample);
  }
  // No scaler required
  float volts2hertz(float volts) {
    return VoltsPerOctave::voltsToHertz(volts);
  }
  // Uses output scaler
  float volts2sample(float volts) {
    return fVOctOutput->voltsToSample(volts);
  }
  // No scaler required
  float hertz2volts(float hertz) {
    return VoltsPerOctave::hertzToVolts(hertz);
  }
}

#endif // __FaustPatch_h__

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
