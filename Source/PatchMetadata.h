#ifndef __PatchMetadata_h
#define __PatchMetadata_h

#define CONTROL_INPUT     0
#define CONTROL_OUTPUT    1
#define CONTROL_UNIPOLAR  0
#define CONTROL_BIPOLAR   2

#define CONTROL_BUTTON    4
#define CONTROL_TOGGLE    8

#define NOF_PARAMETERS    20
#define NOF_BUTTONS       20

struct PatchMetadata {
public:
  struct Control {
    uint8_t id;
    uint8_t flags;
    const char* name;
  };
  static const char* name;
  static const int channels_in;
  static const int channels_out;
  static const int parameter_count;
  static const int button_count;
  static const Control parameters[NOF_PARAMETERS];
  static const Control buttons[NOF_BUTTONS];
};

#endif /* __PatchMetadata_h */
