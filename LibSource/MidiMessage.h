#ifndef _MidiMessage_h_
#define _MidiMessage_h_

#include "MidiStatus.h"

class MidiMessage {
 public:
  uint8_t data[4];
  MidiMessage(){}
  MidiMessage(uint8_t port, uint8_t d0, uint8_t d1, uint8_t d2){
    data[0] = port;
    data[1] = d0;
    data[2] = d1;
    data[3] = d2;
  }
  uint8_t getPort(){
    return (data[0] & 0xf0)>>4;
  }
  uint8_t getChannel(){
    return (data[1] & MIDI_CHANNEL_MASK);
  }
  uint8_t getStatus(){
    return (data[1] & MIDI_STATUS_MASK);
  }
  uint8_t getNote(){
    return data[2];
  }
  uint8_t getVelocity(){
    return data[3];
  }
  uint8_t getControllerNumber(){
    return data[2];
  }
  uint8_t getControllerValue(){
    return data[3];
  }
  int16_t getPitchBend(){
    int16_t pb = (data[2] | (data[3]<<7)) - 8192;
    return pb;
  }
  bool isNoteOn(){
    return ((data[1] & MIDI_STATUS_MASK) == NOTE_ON) && getVelocity() != 0;
  }
  bool isNoteOff(){
    return ((data[1] & MIDI_STATUS_MASK) == NOTE_OFF) || (((data[1] & MIDI_STATUS_MASK) == NOTE_ON) && getVelocity() == 0);
  }
  bool isControlChange(){
    return (data[1] & MIDI_STATUS_MASK) == CONTROL_CHANGE;
  }
  bool isProgramChange(){
    return (data[1] & MIDI_STATUS_MASK) == PROGRAM_CHANGE;
  }
  bool isPitchBend(){
    return (data[1] & MIDI_STATUS_MASK) == PITCH_BEND_CHANGE;
  }
  static MidiMessage cc(uint8_t ch, uint8_t cc, uint8_t value){
    return MidiMessage(USB_COMMAND_CONTROL_CHANGE, CONTROL_CHANGE|ch, cc, value);
  }
  static MidiMessage pc(uint8_t ch, uint8_t pc){
    return MidiMessage(USB_COMMAND_PROGRAM_CHANGE, PROGRAM_CHANGE|ch, pc, 0);
  }
};

#endif /* _MidiMessage_h_ */
