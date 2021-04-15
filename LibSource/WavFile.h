#ifndef __WavFile_h__
#define __WavFile_h__

#include "IntArray.h"
#include "ShortArray.h"
#include "FloatArray.h"

// supports only basic 44-byte wav headers with PCM type
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

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
    char     datachunk_id[4];
    uint32_t datachunk_size;
}WavHeader;

class WavFile {
  WavHeader* header;
public:
  WavFile(void* data) : header((WavHeader*)data){}
  WavHeader* getHeader(){
    return header;
  }
  bool isValid(){
    // support only basic 44-byte wav headers with PCM type
    if(strncmp(header->chunk_id, "RIFF", 4) ||
       strncmp(header->format, "WAVE", 4) ||
       strncmp(header->datachunk_id, "data", 4) ||
       header->datachunk_size == 0 ||
       header->fmtchunk_size != 16)
      return false;
    if(header->audio_format == 1 && header->bps == 16)
      return true;
    if(header->audio_format == 1 && header->bps == 32)
      return true;
    if(header->audio_format == 3 && header->bps == 32)
      return true;
    return false;
  }
  size_t getNumberOfChannels(){
    return header->num_channels;
  }
  size_t getNumberOfSamples(){
    return header->datachunk_size/(header->bps/8);    
  }
  size_t getBitsPerSample(){
    return header->bps;
  }
  void* getData(){
    // if(header->fmtchunk_size == 16)
    return ((uint8_t*)header) + sizeof(WavHeader);
  }
  FloatArray createFloatArray(){
    uint8_t* data = ((uint8_t*)header) + sizeof(WavHeader);
    size_t len = getNumberOfSamples();
    if(header->audio_format == 1 && header->bps == 16){ // WAVE_FORMAT_PCM
      ShortArray samples((int16_t*)data, len);
      FloatArray result = FloatArray::create(samples.getSize());
      samples.toFloat(result);
      return result;
    }else if(header->audio_format == 1 && header->bps == 32){ // WAVE_FORMAT_PCM
      IntArray samples((int32_t*)data, len);
      FloatArray result = FloatArray::create(samples.getSize());
      samples.toFloat(result);
      return result;
    // }else if(header->audio_format == 3 && header->bps == 32){ // WAVE_FORMAT_IEEE_FLOAT
    //   FloatArray samples((float*)data, len);
    //   FloatArray result = FloatArray::create(samples.getSize());
    //   samples.copyTo(result);
    //   return result;
    }
    return FloatArray();
  }
};

#endif // __WavFile_h__
