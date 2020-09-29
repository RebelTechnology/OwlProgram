#ifndef __OWL_H
#define __OWL_H

/* In this file we declare all OWL foreign functions.
 * They can be used in a FAUST patch like this:
 * 
 * pushbutton = ffunction(int owl_pushbutton(int), <owl.h>,"");
 * 
 * sample2hertz = ffunction(float sample2hertz(float, float), <owl.h>, "");
 * 
 * hertz2sample = ffunction(float, hertz2sample(float, float), <owl.h>, "");
 * 
 * sample2volts = ffunction(float sample2volts(float), <owl.h>, "");
 * 
 * volts2hertz = ffunction(float volts2hertz(float), <owl.h>, "");
 * 
 * volts2sample = ffunction(float volts2sample(float), <owl.h>, "");
 * 
 * hertz2volts = ffunction(float hertz2volts(float), <owl.h>, "");
 * 
 * Or you can just call import("owl.lib"); in your patch and use definitions
 * from that file.
 */

#ifdef __cplusplus
 extern "C" {
#endif

   int owl_pushbutton(int value);
   float sample2hertz(float tune, float sample);
   float hertz2sample(float tune, float hertz);
   float sample2volts(float sample);
   float volts2hertz(float volts);
   float volts2sample(float volts);
   float hertz2volts(float hertz);

#ifdef __cplusplus
}
#endif

#endif /*  __OWL_H */
