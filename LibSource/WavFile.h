#ifndef __WavFile_h__
#define __WavFile_h__

#include "IntArray.h"
#include "ShortArray.h"
#include "FloatArray.h"

// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

typedef struct {
  char     id[4];
  uint32_t size;  
} WavDataChunk;

typedef struct {
    char     chunk_id[4];
    uint32_t chunk_size;
    char     format[4];
    char     fmtchunk_id[4];
    uint32_t fmtchunk_size; // Chunk size: 16, 18 or 40
    uint16_t audio_format; // 1 = PCM, 3 = float
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bps;
} WavHeader;

class WavFile {
  WavHeader* header;
  WavDataChunk* datachunk;
  size_t size;
public:
  WavFile(void* data, size_t len) : header((WavHeader*)data), size(len) {
    datachunk = (WavDataChunk*)(header->fmtchunk_id + 8 + header->fmtchunk_size);
    void* end = (uint8_t*)data+len;
    // iterate through data chunks to find one with id 'data'
    while(datachunk->id + datachunk->size < end && strncmp(datachunk->id, "data", 4) != 0)
      datachunk = (WavDataChunk*)(datachunk->id + sizeof(WavDataChunk) + datachunk->size);
  }
  WavHeader* getHeader(){
    return header;
  }
  bool isValid(){
    if(!header || !datachunk ||
       strncmp(header->chunk_id, "RIFF", 4) ||
       strncmp(header->format, "WAVE", 4) ||
       strncmp(datachunk->id, "data", 4) ||
       datachunk->size == 0)
      return false;
    if(header->audio_format == 1 && header->bps == 8)
      return true; // 8-bit PCM
    if(header->audio_format == 1 && header->bps == 16)
      return true; // 16-bit PCM
    // if(header->audio_format == 1 && header->bps == 24)
    //   return true; // 24-bit PCM
    if(header->audio_format == 3 && header->bps == 32)
      return true; // 32-bit float
    return false;
  }
  
  size_t getNumberOfChannels(){
    return header->num_channels;
  }
  size_t getNumberOfSamples(){
    size_t total = datachunk->size/(header->bps/8);
    return total/getNumberOfChannels();
  }
  size_t getSize(){
    return size;
  }
  size_t getBitsPerSample(){
    return header->bps;
  }
  int16_t getAudioFormat(){
    return header->audio_format;
  }
  void* getData(){
    return datachunk->id + sizeof(WavDataChunk);
  }
  void read(size_t channel, FloatArray output, bool all=false){
    size_t channels = getNumberOfChannels();
    size_t len = getNumberOfSamples();
    if(len > output.getSize())
      len = output.getSize();
    size_t pos = channel % channels;
    if(header->audio_format == 1 && header->bps == 8){ // WAVE_FORMAT_PCM 8-bit
      int8_t* data = (int8_t*)getData();      
      for(size_t i=0; i<len; ++i){
        output[i] = (float)data[pos] / 128.0f;
        pos += all ? 1 : channels;
      }
    }else if(header->audio_format == 1 && header->bps == 16){ // WAVE_FORMAT_PCM 16-bit
      int16_t* data = (int16_t*)getData();      
      for(size_t i=0; i<len; ++i){
        output[i] = (float)data[pos] / 32768.0f;
        pos += all ? 1 : channels;
      }
      // todo: 24-bit data needs decoding
    // }else if(header->audio_format == 1 && header->bps == 24){ // WAVE_FORMAT_PCM 24-bit
    //   int32_t* data = (int32_t*)getData();      
    //   for(size_t i=0; i<len; ++i){
    // 	output[i] = (float)data[pos] / 8388608.0f;
    // 	pos += channels;
    //   }
    }else if(header->audio_format == 3 && header->bps == 32){ // WAVE_FORMAT_IEEE_FLOAT
      float* data = (float*)getData();
      for(size_t i=0; i<len; ++i){
        output[i] = data[pos];
        pos += all ? 1 : channels;
      }
    }
  }
  FloatArray createFloatArray(size_t channel){
    FloatArray output = FloatArray::create(getNumberOfSamples());
    read(channel, output);
    return output;
  }
  FloatArray createFloatArray(){
    FloatArray output = FloatArray::create(getNumberOfSamples() * getNumberOfChannels());
    read(0, output, true);
    return output;
  }
};

#endif // __WavFile_h__
