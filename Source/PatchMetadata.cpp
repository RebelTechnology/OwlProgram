#include "PatchMetadata.h"

#define __weak  __attribute__((weak))

__weak const char* PatchMetadata::name = PATCHNAME;
__weak const int PatchMetadata::channels_in = 2;
__weak const int PatchMetadata::channels_out = 2;
__weak const int PatchMetadata::parameter_count = 2;
__weak const int PatchMetadata::button_count = 2;
__weak const PatchMetadata::Control PatchMetadata::parameters[] = {};
__weak const PatchMetadata::Control PatchMetadata::buttons[] = {};
