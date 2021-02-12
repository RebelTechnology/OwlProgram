#include <stdio.h>
#include <malloc.h>

#include "heap.h"
void *pvPortMalloc( size_t xWantedSize ){
  return malloc(xWantedSize);
}
void vPortFree( void *pv ){
  free(pv);
}
#include "basicmaths.h"
#include "message.h"
#include "Patch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "SampleBuffer.hpp"

#include "MemoryBuffer.hpp"
AudioBuffer* AudioBuffer::create(int channels, int samples){
  return new ManagedMemoryBuffer(channels, samples);
}
AudioBuffer::~AudioBuffer(){}

#include "registerpatch.h"

#define SAMPLE_RATE 48000
#define CHANNELS    2
#define BLOCKSIZE   128

extern "C" {
  // http://www.keil.com/forum/60479/
  void arm_bitreversal_32(uint32_t *pSrc, const uint16_t bitRevLen, const uint16_t *pBitRevTab)
{
  uint32_t r3 = (bitRevLen + 1) / 2;
  uint32_t *r2, *r6;
  uint32_t r4, r5;
  while(r3--)
  {
    r2 = (uint32_t *)((uint32_t)pSrc + pBitRevTab[0]);
    r6 = (uint32_t *)((uint32_t)pSrc + pBitRevTab[1]);
    r5 = r2[0];
    r4 = r6[0];
    r6[0] = r5;
    r2[0] = r4;
    r5 = r2[1];
    r4 = r6[1];
    r6[1] = r5;
    r2[1] = r4;
    pBitRevTab += 2;
  }
}

void arm_bitreversal_16(uint32_t *pSrc, const uint16_t bitRevLen, const uint16_t *pBitRevTab)
{
  #warning TODO!
  // ASSERT(false, "arm_bitreversal_16");
}
}

PatchProcessor processor;
ProgramVector programVector;

void assert_failed(const char* msg, const char* location, int line){
  printf("Assertion failed: %s, in %s line %d\n", msg, location, line);
  exit(-1);
}

void debugMessage(char const* msg, int a){
  printf("%s %d\n", msg, a);
}

void debugMessage(char const* msg, float a){
  printf("%s %f\n", msg, a);
}

void debugMessage(char const* msg, int a, int b){
  printf("%s %d %d\n", msg, a, b);
}

void debugMessage(char const* msg, float a, float b){
  printf("%s %f %f\n", msg, a, b);
}

void debugMessage(char const* msg, int a, int b, int c){
  printf("%s %d %d %d\n", msg, a, b, c);
}

void debugMessage(char const* msg, float a, float b, float c){
  printf("%s %f %f %f\n", msg, a, b, c);
}

void debugMessage(char const* msg){
  printf("%s\n", msg);
}

Patch::Patch(){}
Patch::~Patch(){}
PatchProcessor::PatchProcessor(){}
PatchProcessor::~PatchProcessor(){}
void Patch::registerParameter(PatchParameterId pid, const char* name){
  printf("Register parameter %c: %s\n", 'A'+pid, name);
}  
void Patch::processMidi(MidiMessage msg){}
void Patch::sendMidi(MidiMessage msg){
  printf("Sending MIDI [%x:%x:%x:%x]\n", msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
}  

float Patch::getSampleRate(){
  return SAMPLE_RATE;
}

int Patch::getBlockSize(){
  return BLOCKSIZE;
}

int Patch::getNumberOfChannels(){
  return CHANNELS;
}

float Patch::getParameterValue(PatchParameterId pid){
  return 0.0f;
}

void Patch::setParameterValue(PatchParameterId pid, float value){
  printf("Set parameter %c: %d\n", 'A'+pid, value);
}

void Patch::setButton(PatchButtonId bid, uint16_t value, uint16_t samples){
  printf("Set button %c: %d\n", 'A'+bid, value);
}

bool Patch::isButtonPressed(PatchButtonId bid){
  return false;
}

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

static Patch* testpatch = NULL;
void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  //if(patch == NULL)
  //  error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  testpatch = patch;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
#include "registerpatch.cpp"
  ASSERT(testpatch != NULL, "Missing patch");    
  int ret = 0;
  SampleBuffer* samples = new SampleBuffer(128);
  testpatch->processAudio(*samples);
  delete samples;
  return ret;
}
