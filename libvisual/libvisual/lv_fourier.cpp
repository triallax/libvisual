/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * The FFT implementation found in this file is based upon the NULLSOFT
 * Milkdrop FFT implementation.
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id: lv_fourier.c,v 1.15 2006/02/13 20:54:08 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_fourier.h"
#include "lv_common.h"
#include "lv_math.h"
#include <cmath>
#include <vector>
#include <map>
#include <utility>

// Log scale settings
#define AMP_LOG_SCALE_THRESHOLD0    0.001f
#define AMP_LOG_SCALE_DIVISOR       6.908f  // divisor = -log threshold

namespace LV {

  namespace {

    enum DFTMethod
    {
        DFT_METHOD_BRUTE_FORCE,
        DFT_METHOD_FFT
    };

    class DFTCache
    {
    public:

        class Entry
        {
        public:

            std::vector<float> bitrevtable;
            std::vector<float> sintable;
            std::vector<float> costable;

            // FIXME: Eliminate this constructor
            Entry () {}

            Entry (DFTMethod method, unsigned int spectrum_size);

        private:

            void fft_bitrev_table_init (unsigned int spectrum_size);
            void fft_cossin_table_init (unsigned int spectrum_size);
            void dft_cossin_table_init (unsigned int spectrum_size);
        };

        Entry const& get_entry (DFTMethod method, unsigned int spectrum_size);

    private:

        typedef std::map<unsigned int, Entry> Table;

        Table m_cache;
    };

    DFTCache dft_cache;

  } // anonymous namespace


  struct Fourier::Impl
  {
      // nothing for now
  };

  struct DFT::Impl
  {
      unsigned int       samples_in;
      unsigned int       spectrum_size;
      DFTMethod          method;
      std::vector<float> real;
      std::vector<float> imag;

      Impl (unsigned int samples_out, unsigned int samples_in);

      void perform_brute_force (float *output, float const* input);
      void perform_fft_radix2_dit (float *output, float const* input);

      DFTMethod best_method (unsigned int spectrum_size);
  };


  namespace {

    DFTCache::Entry const& DFTCache::get_entry (DFTMethod method, unsigned int spectrum_size)
    {
        Table::const_iterator entry = m_cache.find (spectrum_size);
        if (entry != m_cache.end ())
            return entry->second;

        // FIXME: Eliminate the copying
        m_cache.insert (std::make_pair (spectrum_size, Entry (method, spectrum_size)));

        return m_cache[spectrum_size];
    }

    DFTCache::Entry::Entry (DFTMethod method, unsigned int spectrum_size)
    {
        switch (method) {
            case DFT_METHOD_BRUTE_FORCE:
                dft_cossin_table_init (spectrum_size);
                break;

            case DFT_METHOD_FFT:
                fft_bitrev_table_init (spectrum_size);
                fft_cossin_table_init (spectrum_size);
                break;
        }
    }

    void DFTCache::Entry::fft_bitrev_table_init (unsigned int spectrum_size)
    {
        bitrevtable.clear ();
        bitrevtable.reserve (spectrum_size);

        for (unsigned int i = 0; i < spectrum_size; i++)
            bitrevtable.push_back (i);

        unsigned int j = 0;

        for (unsigned int i = 0; i < spectrum_size; i++) {
            if (j > i) {
                std::swap (bitrevtable[i], bitrevtable[j]);
            }

            unsigned int m = spectrum_size >> 1;

            while (m >= 1 && j >= m) {
                j -= m;
                m >>= 1;
            }

            j += m;
        }
    }

    void DFTCache::Entry::fft_cossin_table_init (unsigned int spectrum_size)
    {
        unsigned int dft_size = 2;
        unsigned int tab_size = 0;

        while (dft_size <= spectrum_size) {
            tab_size++;
            dft_size <<= 1;
        }

        sintable.clear ();
        sintable.reserve (tab_size);

        costable.clear ();
        costable.reserve (tab_size);

        dft_size = 2;

        while (dft_size <= spectrum_size) {
            float theta = -2.0f * VISUAL_MATH_PI / dft_size;

            costable.push_back (std::cos (theta));
            sintable.push_back (std::sin (theta));

            dft_size <<= 1;
        }
    }

    void DFTCache::Entry::dft_cossin_table_init (unsigned int spectrum_size)
    {
        unsigned int tab_size = (spectrum_size >> 1) + 1;

        sintable.clear ();
        sintable.reserve (tab_size);

        sintable.clear ();
        costable.reserve (tab_size);

        for (unsigned int i = 0; i < tab_size; i++) {
            float theta = (-2.0f * VISUAL_MATH_PI * i) / spectrum_size;

            costable.push_back (std::cos (theta));
            sintable.push_back (std::sin (theta));
        }
    }

  } // anonymous namespace


  Fourier::Fourier ()
      : m_impl (new Impl)
  {
      // empty
  }

  Fourier::~Fourier ()
  {
      // empty
  }

  DFT::DFT (unsigned int samples_out, unsigned int samples_in)
      : m_impl (new Impl (samples_out, samples_in))
  {
      // empty
  }

  void DFT::perform (float *output, float const* input)
  {
      visual_return_if_fail (output != NULL);
      visual_return_if_fail (input  != NULL);

      switch (m_impl->method) {
          case DFT_METHOD_BRUTE_FORCE:
              m_impl->perform_brute_force (output, input);
              break;

          case DFT_METHOD_FFT:
              m_impl->perform_fft_radix2_dit (output, input);
              break;
      }

      visual_math_vectorized_complex_to_norm_scale (output, &m_impl->real[0], &m_impl->imag[0],
                                                  m_impl->spectrum_size / 2,
                                                  1.0 / m_impl->spectrum_size);
  }

  void DFT::log_scale (float *output, float const* input, unsigned int size)
  {
      visual_return_if_fail (output != NULL);
      visual_return_if_fail (input  != NULL);

      return log_scale_standard (output, input, size);
  }

  void DFT::log_scale_standard (float *output, float const* input, unsigned int size)
  {
      visual_return_if_fail (output != NULL);
      visual_return_if_fail (input  != NULL);

      return log_scale_custom (output, input, size, AMP_LOG_SCALE_DIVISOR);
  }

  void DFT::log_scale_custom (float* output, float const* input, unsigned int size, float log_scale_divisor)
  {
      visual_return_if_fail (output != NULL);
      visual_return_if_fail (input  != NULL);

      for (unsigned int i = 0; i < size; i++) {
          if (input[i] > AMP_LOG_SCALE_THRESHOLD0)
              output[i] = 1.0f + log (input[i]) / log_scale_divisor;
          else
              output[i] = 0.0f;
      }
  }

  DFT::Impl::Impl (unsigned int samples_out_, unsigned int samples_in_)
      : samples_in    (samples_in_),
        spectrum_size (samples_in_),
        method        (best_method (spectrum_size)),
        real          (spectrum_size),
        imag          (spectrum_size)
  {
      // empty
  }

  DFTMethod DFT::Impl::best_method (unsigned int spectrum_size)
  {
      if (visual_math_is_power_of_2 (spectrum_size))
          return DFT_METHOD_FFT;
      else
          return DFT_METHOD_BRUTE_FORCE;
  }

  void DFT::Impl::perform_brute_force (float *output, float const* input)
  {
      DFTCache::Entry const& fcache = dft_cache.get_entry (method, spectrum_size);

      for (unsigned int i = 0; i < spectrum_size / 2 + 1; i++) {
          float xr = 0.0f;
          float xi = 0.0f;

          float wr = 1.0f;
          float wi = 0.0f;

          for (unsigned int j = 0; j < spectrum_size; j++) {
              xr += input[j] * wr;
              xi += input[j] * wi;

              float wtemp = wr;

              wr = wr    * fcache.costable[i] - wi * fcache.sintable[i];
              wi = wtemp * fcache.sintable[i] + wi * fcache.costable[i];
          }

          real[i] = xr;
          imag[i] = xi;
      }
  }

  void DFT::Impl::perform_fft_radix2_dit (float *output, float const* input)
  {
    DFTCache::Entry const& fcache = dft_cache.get_entry (method, spectrum_size);

    for (unsigned int i = 0; i < spectrum_size; i++) {
        unsigned int idx = fcache.bitrevtable[i];

        if (idx < samples_in)
            real[i] = input[idx];
        else
            real[i] = 0;
    }

    unsigned int dft_size = 2;
    unsigned int t = 0;

    while (dft_size <= spectrum_size) {
        float wpr = fcache.costable[t];
        float wpi = fcache.sintable[t];

        float wr = 1.0f;
        float wi = 0.0f;

        unsigned int half_dft_size = dft_size >> 1;

        for (unsigned int m = 0; m < half_dft_size; m++) {
            for (unsigned int i = m; i < spectrum_size; i += dft_size) {
                unsigned int j = i + half_dft_size;

                float tempr = wr * real[j] - wi * imag[j];
                float tempi = wr * imag[j] + wi * real[j];

                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;

                real[i] += tempr;
                imag[i] += tempi;
            }

            float wtemp;

            wr = (wtemp = wr) * wpr - wi * wpi;
            wi = wi * wpr + wtemp * wpi;
        }

        dft_size <<= 1;
        t++;
    }
  }

} // LV namespace
