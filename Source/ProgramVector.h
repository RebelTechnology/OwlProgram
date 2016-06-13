#ifndef __PROGRAM_VECTOR_H
#define __PROGRAM_VECTOR_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define OWL_PEDAL_HARDWARE    0x11
#define OWL_MODULAR_HARDWARE  0x12
#define OWL_RACK_HARDWARE     0x13

   typedef enum { 
     AUDIO_IDLE_STATUS = 0, 
     AUDIO_READY_STATUS, 
     AUDIO_PROCESSED_STATUS, 
     AUDIO_EXIT_STATUS, 
     AUDIO_ERROR_STATUS 
   } ProgramVectorAudioStatus;

   typedef struct {
     uint8_t checksum;
     uint8_t hardware_version;
     int16_t* audio_input;
     int16_t* audio_output;
     uint8_t audio_bitdepth;
     uint16_t audio_blocksize;
     uint32_t audio_samplingrate;
     uint16_t* parameters;
     uint8_t parameters_size;
     uint16_t buttons;
     int8_t error;
     void (*registerPatch)(const char* name, uint8_t inputChannels, uint8_t outputChannels);
     void (*registerPatchParameter)(uint8_t id, const char* name);
     void (*programReady)(void);
     void (*programStatus)(ProgramVectorAudioStatus status);
     int (*serviceCall)(int service, void** params, int len);
     uint32_t cycles_per_block;
     // outgoing
     uint32_t heap_bytes_used;
     char* message;
     uint32_t stateChanged;
     /* uint32_t buttonsChanged; */
     /* uint32_t parametersChanged; */
     void (*buttonChangedCallback)(uint8_t bid, bool value);
     void (*encoderChangedCallback)(uint8_t bid, int32_t delta);
   } ProgramVector;

#define CHECKSUM_ERROR_STATUS      -10
#define OUT_OF_MEMORY_ERROR_STATUS -20
#define CONFIGURATION_ERROR_STATUS -30

   extern ProgramVector programVector;
#define getProgramVector() (&programVector)

#ifdef __cplusplus
}
#endif

#endif /* __PROGRAM_VECTOR_H */
