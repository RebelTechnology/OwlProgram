/************************** BEGIN WaveReader.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2020 GRAME, Centre National de Creation Musicale
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

#ifndef __WaveReader__
#define __WaveReader__

#undef min
#undef max

#include <string>
#include "Resource.h"

// WAVE file description
class WaveFile {
public:
    // The canonical WAVE format starts with the RIFF header
    
    /**
     Variable: chunk_id
     Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
     **/
    int chunk_id;
    
    /**
     Variable: chunk_size
     36 + SubChunk2Size, or more precisely: 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
     This is the size of the rest of the chunk following this number.
     This is the size of the entire file in bytes minus 8 bytes for the
     two fields not included in this count: ChunkID and ChunkSize.
     **/
    int chunk_size;
    
    /**
     Variable: format
     Contains the letters "WAVE" (0x57415645 big-endian form).
     **/
    int format;
    
    // The "WAVE" format consists of two subchunks: "fmt " and "data":
    // The "fmt " subchunk describes the sound data's format:
    
    /**
     Variable: subchunk_1_id
     Contains the letters "fmt " (0x666d7420 big-endian form).
     **/
    int subchunk_1_id;
    
    /**
     Variable: subchunk_1_size
     16 for PCM. This is the size of the rest of the Subchunk which follows this number.
     **/
    int subchunk_1_size;
    
    /**
     Variable: audio_format
     PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression.
     **/
    short audio_format;
    
    /**
     Variable: num_channels
     Mono = 1, Stereo = 2, etc.
     **/
    short num_channels;
    
    /**
     Variable: sample_rate
     8000, 44100, etc.
     **/
    int sample_rate;
    
    /**
     Variable: byte_rate
     == SampleRate * NumChannels * BitsPerSample/8
     **/
    int byte_rate;
    
    /**
     Variable: block_align
     == NumChannels * BitsPerSample/8
     The number of bytes for one sample including all channels. I wonder what happens
     when this number isn't an integer?
     **/
    short block_align;
    
    /**
     Variable: bits_per_sample
     8 bits = 8, 16 bits = 16, etc.
     **/
    short bits_per_sample;
    
    /**
     Here should come some extra parameters which i will avoid.
     **/
    
    // The "data" subchunk contains the size of the data and the actual sound:
    
    /**
     Variable: subchunk_2_id
     Contains the letters "data" (0x64617461 big-endian form).
     **/
    int subchunk_2_id;
    
    /**
     Variable: subchunk_2_size
     == NumSamples * NumChannels * BitsPerSample/8
     This is the number of bytes in the data. You can also think of this as the size
     of the read of the subchunk following this number.
     **/
    int subchunk_2_size;
    
    /**
     Variable: data
     The actual sound data.
     **/
    char* data;
    
};

// Base reader
class WaveReader {
public:    
    WaveFile* fWave;

    WaveReader() {
        fWave = new WaveFile();
    }

    virtual ~WaveReader() {
        if (fWave->data != NULL)
            delete[] fWave->data;
        delete fWave;
    }

    bool loadWaveHeader() {
        char buffer[4];
        
        read(buffer, 4);
        if (strncmp(buffer, "RIFF", 4) != 0) {
            debugMessage("Not a valid WAV file!");
            return false;
        }

        fWave->chunk_id = toInt(buffer, 4);
        
        read(buffer, 4);
        fWave->chunk_size = toInt(buffer, 4);
        
        read(buffer, 4);
        fWave->format = toInt(buffer, 4);
        
        read(buffer, 4);
        fWave->subchunk_1_id = toInt(buffer, 4);
        
        read(buffer, 4);
        fWave->subchunk_1_size = toInt(buffer, 4);
        
        read(buffer, 2);
        fWave->audio_format = toInt(buffer, 2);
        read(buffer, 2);
        fWave->num_channels = toInt(buffer, 2);
        
        read(buffer, 4);
        fWave->sample_rate = toInt(buffer, 4);
        
        read(buffer, 4);
        fWave->byte_rate = toInt(buffer, 4);
        
        read(buffer, 2);
        fWave->block_align = toInt(buffer, 2);
        read(buffer, 2);
        fWave->bits_per_sample = toInt(buffer, 2);
        
        read(buffer, 4);
        if (strncmp(buffer, "data", 4) != 0) {
            read(buffer, 4);
            int _extra_size = toInt(buffer, 4);
            char _extra_data[_extra_size];
            read(_extra_data, _extra_size);
            read(buffer, 4);
            fWave->subchunk_2_id = toInt(buffer, 4);
        } else {
            fWave->subchunk_2_id = toInt(buffer, 4);
        }
        
        read(buffer, 4);
        fWave->subchunk_2_size = toInt(buffer, 4);
        return true;
    }
    
    void loadWave()
    {
        // Read sound data
        fWave->data = new char[fWave->subchunk_2_size];
        read(fWave->data, fWave->subchunk_2_size);
    }

    virtual void read(char* buffer, unsigned int size) = 0;

protected:
    inline int toInt(char* buffer, int len)
    {
        int a = 0;
        for(int i = 0; i < len; i++) {
            ((char*)&a)[i] = buffer[i];
        }
        return a;
    }   
};

/*
 * WAV file reader that handles resource parser
 */
class WaveResourceReader : public WaveReader {
public:
    WaveResourceReader(const char* name) :
        offset(0) {
        resource = Resource::open(name);
    };

    ~WaveResourceReader() {
        Resource::destroy(resource);
    };

    void read(char* buffer, unsigned int size) override {
        size_t read_len = resource->read((void*)buffer, size, offset);
        offset += read_len;
    }
private:
    Resource* resource;
    size_t offset;
};

#endif
/**************************  END  WaveReader.h **************************/
