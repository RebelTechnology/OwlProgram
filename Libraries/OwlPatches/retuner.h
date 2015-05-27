// -----------------------------------------------------------------------
//
//  Copyright (C) 2009-2011 Fons Adriaensen <fons@linuxaudio.org>
//    
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// -----------------------------------------------------------------------

#ifndef __RETUNER_H
#define __RETUNER_H


#ifdef USE_RESAMPLER
#include <zita-resampler/resampler.h>
#endif // USE_RESAMPLER

#include "StompBox.h"
#include "arm_math.h"
class FastFourierTransform {
  private:
    arm_rfft_fast_instance_f32 instance;
  public:
    void init(int len){
      arm_rfft_fast_init_f32(&instance, len);
  // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
    }
    void fft(float* in, float* out){
      arm_rfft_fast_f32(&instance, in, out, 0);
    }
    void ifft(float* in, float* out){
      arm_rfft_fast_f32(&instance, in, out, 1);
    }
};


class Retuner
{
public:
    Retuner (){}
    Retuner (int fsamp, float **fourPointers);
    ~Retuner (void);
    
    void init(int fsamp, float **fourPointers);

    int process (int nfram, float *inp, float *out);

    void set_refpitch (float v)
    {
        _refpitch = v;
    }

    void set_notebias (float v)
    {
        _notebias = v / 13.0f;
    }

    void set_corrfilt (float v)
    {
        _corrfilt = (4 * _frsize) / (v * _fsamp);
    }

    void set_corrgain (float v)
    {
        _corrgain = v;
    }

    void set_corroffs (float v)
    {
        _corroffs = v;
    }

    void set_notemask (int k)
    {
        _notemask = k;
    }
   
    int get_noteset (void)
    {
        int k;

        k = _notebits;
        _notebits = 0;
        return k;
    }

    float get_error (void)
    {
        return 12.0f * _error;
    }


private:

    void  findcycle (void);
    void  finderror (void);
    float cubic (float *v, float a);
    FastFourierTransform transform;
    int              _fsamp;
    int              _ifmin;
    int              _ifmax;
    bool             _upsamp;
    int              _fftlen;
    int              _ipsize;
    int              _frsize;
    int              _ipindex;
    int              _frindex;
    int              _frcount;
    float            _refpitch;
    float            _notebias;
    float            _corrfilt; 
    float            _corrgain;
    float            _corroffs;
    int              _notemask;
    int              _notebits;
    int              _lastnote;
    int              _count;
    float            _cycle;
    float            _error;
    float            _ratio;
    float            _phase;
    bool             _xfade;
    float            _rindex1;
    float            _rindex2;
    float           *_ipbuff;
    float           *_xffunc;
    float           *_fftTwind;
    float           *_fftWcorr;
    float           *_fftTdata;
    float           **_fftFdata;
#ifdef USE_RESAMPLER
    Resampler        _resampler;
#endif // USE_RESAMPLER
};


#endif
