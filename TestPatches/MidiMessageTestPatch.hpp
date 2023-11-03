#include "TestPatch.hpp"
#include "MidiMessage.h"

class MidiMessageTestPatch : public TestPatch {
public:
  MidiMessageTestPatch(){
    {
      TEST("ctor");
      MidiMessage msg;
      CHECK_EQUAL(sizeof(msg), sizeof(uint32_t));
      CHECK_EQUAL(msg.packed, 0);
      msg = MidiMessage(1, 2, 3, 4);
      CHECK_EQUAL(msg.data[0], 1);
      CHECK_EQUAL(msg.data[1], 2);
      CHECK_EQUAL(msg.data[2], 3);
      CHECK_EQUAL(msg.data[3], 4);
    }
    {
      TEST("pb");
      MidiMessage msg = MidiMessage::pb(15, -1234);
      CHECK(msg.isPitchBend());
      CHECK_EQUAL(msg.getChannel(), 15);
      CHECK_EQUAL(msg.getPitchBend(), -1234);
    }
    {
      TEST("cc");
      MidiMessage msg = MidiMessage::cc(3, 55, 127);
      CHECK(msg.isControlChange());
      CHECK_EQUAL(msg.getChannel(), 3);
      CHECK_EQUAL(msg.getControllerNumber(), 55);
      CHECK_EQUAL(msg.getControllerValue(), 127);
    }
    {
      TEST("pc");
      MidiMessage msg = MidiMessage::pc(12, 39);
      CHECK(msg.isProgramChange());
      CHECK_EQUAL(msg.getChannel(), 12);
      CHECK_EQUAL(msg.getProgramChange(), 39);
    }
    {
      TEST("note off");
      MidiMessage msg = MidiMessage::note(1, 39, 0);
      CHECK(msg.isNote());
      CHECK(msg.isNoteOff());
      CHECK_EQUAL(msg.getChannel(), 1);
      CHECK_EQUAL(msg.getNote(), 39);
      CHECK_EQUAL(msg.getVelocity(), 0);      
    }
    {
      TEST("note on");
      MidiMessage msg = MidiMessage::note(7, 44, 89);
      CHECK(msg.isNote());
      CHECK(msg.isNoteOn());
      CHECK_EQUAL(msg.getChannel(), 7);
      CHECK_EQUAL(msg.getNote(), 44);
      CHECK_EQUAL(msg.getVelocity(), 89);      
    }
    {
      TEST("cp");
      MidiMessage msg = MidiMessage::cp(7, 119);
      CHECK(msg.isChannelPressure());
      CHECK_EQUAL(msg.getChannel(), 7);
      CHECK_EQUAL(msg.getChannelPressure(), 119);
    }      
    {
      TEST("at");
      MidiMessage msg = MidiMessage::at(7, 119, 125);
      CHECK(msg.isPolyKeyPressure());
      CHECK_EQUAL(msg.getChannel(), 7);
      CHECK_EQUAL(msg.getNote(), 119);
      CHECK_EQUAL(msg.getPolyKeyPressure(), 125);
    }      
  }
};
