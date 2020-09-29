#ifndef __OWL_H
#define __OWL_H

/* In this file we declare all OWL foreign functions.
 * They can be used in a FAUST patch like this:
 * owl_push = ffunction(int owl_pushbutton(int), <owl.h>,"");
 * owl_button = ffunction(int owl_button(int, int), <owl.h>,"");
 * owl_parameter = ffunction(int owl_parameter(int, float), <owl.h>,"");
 */
#ifdef __cplusplus
 extern "C" {
#endif

   int owl_pushbutton(int value);
   int owl_button(int bid, int value);
   float owl_parameter(int pid, float value);

#ifdef __cplusplus
}
#endif

#endif /*  __OWL_H */
