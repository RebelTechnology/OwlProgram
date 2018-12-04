#ifndef __OWL_H
#define __OWL_H

/* In this file we declare all OWL foreign functions.
 * They can be used in a FAUST patch like this:
 * push = ffunction(int owl_pushbutton(int), <owl.h>,"");
 */
#ifdef __cplusplus
 extern "C" {
#endif

   int owl_pushbutton(int value);

#ifdef __cplusplus
}
#endif

#endif /*  __OWL_H */
