#include <stdint.h>
#include "system_tables.h"
#include "ServiceCall.h"
#include "FastPowTable.h"
#include "FastLogTable.h"
#include "basicmaths.h"

void setSystemTables(ProgramVector* pv){
  void* array;
  int size;
  void* args[] = {(void*)SYSTEM_TABLE_LOG, (void*)&array, (void*)&size};
  int ret = 0;
  ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_ARRAY, args, 3);
  if(ret == OWL_SERVICE_OK && array != NULL && size > 0)
    fast_log_set_table((const float*)array, size);
  else
    fast_log_set_table(fast_log_table, fast_log_table_size);
  args[0] = (void*)SYSTEM_TABLE_POW;
  ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_ARRAY, args, 3);
  if(ret == OWL_SERVICE_OK && array != NULL && size > 0)
    fast_pow_set_table((const uint32_t*)array, size);
  else
    fast_pow_set_table(fast_pow_table, fast_pow_table_size);
}
