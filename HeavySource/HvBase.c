/* Copyright 2014 Section6. All Rights Reserved. */

#include "HvBase.h"

void ctx_setBasePath(HvBase *const _c, const char *basePath) {
  hv_free(_c->basePath);
    if (basePath != NULL) {
      hv_size_t len = hv_strlen(basePath);
      _c->basePath = (char *) hv_malloc((len+1)*sizeof(char));
      hv_strncpy(_c->basePath, basePath, len);
    }
}

void ctx_cancelMessage(HvBase *_c, HvMessage *m) {
  mq_removeMessage(&_c->mq, m);
}

void ctx_scheduleMessageForReceiverV(HvBase *const _c, const char *name,
    const hv_uint32_t timestamp, const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  const int numElem = (int) hv_strlen(format);
  HvMessage *m = HV_MESSAGE_ON_STACK(numElem);
  msg_init(m, numElem, timestamp);
  for (int i = 0; i < numElem; i++) {
    switch (format[i]) {
      case 'b': msg_setBang(m,i); break;
      case 'f': msg_setFloat(m, i, (float) va_arg(ap, double)); break;
      case 's': msg_setSymbol(m, i, (char *) va_arg(ap, char *)); break;
      default: break;
    }
  }
  _c->f_scheduleMessageForReceiver(_c, name, m);

  va_end(ap);
}
