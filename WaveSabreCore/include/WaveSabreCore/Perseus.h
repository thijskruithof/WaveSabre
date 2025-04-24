/**
Perseus, just a resonator.

Based on the Rings eurorack module,
https://pichenettes.github.io/mutable-instruments-documentation/modules/rings/

Copyright 2015 Emilie Gillet.

Author: Emilie Gillet (emilie.o.gillet@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

See http://creativecommons.org/licenses/MIT/ for more information.
**/

#ifndef __WAVESABRECORE_PERSEUS_H__
#define __WAVESABRECORE_PERSEUS_H__

#include "Device.h"
#include "DelayBuffer.h"
#include "StateVariableFilter.h"
#include "Helpers.h"

#include <math.h>

#define M_PI_F 3.14159265358979323846f
#define M_PI_POW_2 M_PI_F * M_PI_F
#define M_PI_POW_3 M_PI_POW_2 * M_PI_F
#define M_PI_POW_5 M_PI_POW_3 * M_PI_POW_2
#define M_PI_POW_7 M_PI_POW_5 * M_PI_POW_2
#define M_PI_POW_9 M_PI_POW_7 * M_PI_POW_2
#define M_PI_POW_11 M_PI_POW_9 * M_PI_POW_2

#define SLOPE(out, in, positive, negative) { \
  float error = (in) - out; \
  out += (error > 0 ? positive : negative) * error; \
}

#define MAKE_INTEGRAL_FRACTIONAL(x) \
  int x ## _integral = static_cast<int>(x); \
  float x ## _fractional = x - static_cast<float>(x ## _integral);

namespace WaveSabreCore
{
	class Perseus : public Device
	{
	public:
		enum class ParamIndices
		{
            Frequency,
			Structure,
            Brightness,
            Damping,
            Position,

			NumParams,
		};

		Perseus();
		virtual ~Perseus();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

        static const int kMaxModes = 64;
        static const int kMaxPolyphony = 4;
        static const int kNumStrings = kMaxPolyphony * 2;

	private:
        static const int kNumChords = 11;
        static const int kMaxBlockSize = 1;

        struct PerformanceState 
        {
            bool strum;
            bool internal_exciter;
            bool internal_strum;
            bool internal_note;

            float tonic;
            float note;
            float fm;
            int chord;
        };

        enum FilterMode 
        {
            FILTER_MODE_LOW_PASS,
            FILTER_MODE_BAND_PASS,
            FILTER_MODE_BAND_PASS_NORMALIZED,
            FILTER_MODE_HIGH_PASS
        };

        enum FrequencyApproximation 
        {
            FREQUENCY_EXACT,
            FREQUENCY_ACCURATE,
            FREQUENCY_FAST,
            FREQUENCY_DIRTY
        };

        // Naive Chamberlin SVF.
        class NaiveSvf 
        {
        public:
            NaiveSvf() { }
            ~NaiveSvf() { }

            void Init() 
            {
                set_f_q(0.01f, 100.0f);
                Reset();
            }

            void Reset() 
            {
                lp_ = bp_ = 0.0f;
            }

            // Set frequency and resonance from true units. Various approximations
            // are available to avoid the cost of sinf.
            inline void set_f_q(float f, float resonance) 
            {
                f = f < 0.158f ? f : 0.158f;
                f_ = 2.0f * M_PI_F * f;
                damp_ = 1.0f / resonance;
            }

            template<FilterMode mode>
            inline float Process(float in) {
                float hp, notch, bp_normalized;
                bp_normalized = bp_ * damp_;
                notch = in - bp_normalized;
                lp_ += f_ * bp_;
                hp = notch - lp_;
                bp_ += f_ * hp;

                if (mode == FILTER_MODE_LOW_PASS) {
                    return lp_;
                }
                else if (mode == FILTER_MODE_BAND_PASS) {
                    return bp_;
                }
                else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                    return bp_normalized;
                }
                else if (mode == FILTER_MODE_HIGH_PASS) {
                    return hp;
                }
            }

            inline float lp() const { return lp_; }
            inline float bp() const { return bp_; }

            template<FilterMode mode>
            inline void Process(const float* in, float* out, size_t size) {
                float hp, notch, bp_normalized;
                float lp = lp_;
                float bp = bp_;
                while (size--) {
                    bp_normalized = bp * damp_;
                    notch = *in++ - bp_normalized;
                    lp += f_ * bp;
                    hp = notch - lp;
                    bp += f_ * hp;

                    if (mode == FILTER_MODE_LOW_PASS) {
                        *out++ = lp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS) {
                        *out++ = bp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                        *out++ = bp_normalized;
                    }
                    else if (mode == FILTER_MODE_HIGH_PASS) {
                        *out++ = hp;
                    }
                }
                lp_ = lp;
                bp_ = bp;
            }

            inline void Split(const float* in, float* low, float* high, size_t size) {
                float hp, notch, bp_normalized;
                float lp = lp_;
                float bp = bp_;
                while (size--) {
                    bp_normalized = bp * damp_;
                    notch = *in++ - bp_normalized;
                    lp += f_ * bp;
                    hp = notch - lp;
                    bp += f_ * hp;
                    *low++ = lp;
                    *high++ = hp;
                }
                lp_ = lp;
                bp_ = bp;
            }

            template<FilterMode mode>
            inline void Process(const float* in, float* out, size_t size, size_t decimate) {
                float hp, notch, bp_normalized;
                float lp = lp_;
                float bp = bp_;
                size_t n = decimate - 1;
                while (size--) {
                    bp_normalized = bp * damp_;
                    notch = *in++ - bp_normalized;
                    lp += f_ * bp;
                    hp = notch - lp;
                    bp += f_ * hp;

                    ++n;
                    if (n == decimate) {
                        if (mode == FILTER_MODE_LOW_PASS) {
                            *out++ = lp;
                        }
                        else if (mode == FILTER_MODE_BAND_PASS) {
                            *out++ = bp;
                        }
                        else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                            *out++ = bp_normalized;
                        }
                        else if (mode == FILTER_MODE_HIGH_PASS) {
                            *out++ = hp;
                        }
                        n = 0;
                    }
                }
                lp_ = lp;
                bp_ = bp;
            }

        private:
            float f_;
            float damp_;
            float lp_;
            float bp_;
        };



        class ZScorer 
        {
        public:
            ZScorer() { }
            ~ZScorer() { }

            void Init(float cutoff) 
            {
                coefficient_ = cutoff;
                mean_ = 0.0f;
                variance_ = 0.00f;
            }

            inline float Normalize(float sample) 
            {
                return Update(sample) / sqrtf(variance_);
            }

            inline bool Test(float sample, float threshold) 
            {
                float value = Update(sample);
                return value > sqrtf(variance_) * threshold;
            }

            inline bool Test(float sample, float threshold, float absolute_threshold) 
            {
                float value = Update(sample);
                return value > sqrtf(variance_) * threshold && value > absolute_threshold;
            }

        private:
            inline float Update(float sample) 
            {
                float centered = sample - mean_;
                mean_ += coefficient_ * centered;
                variance_ += coefficient_ * (centered * centered - variance_);
                return centered;
            }

            float coefficient_;
            float mean_;
            float variance_;
        };

        class Compressor 
        {
        public:
            Compressor() { }
            ~Compressor() { }

            void Init(float attack, float decay, float max_gain) 
            {
                attack_ = attack;
                decay_ = decay;
                level_ = 0.0f;
                skew_ = 1.0f / max_gain;
            }

            void Process(const float* in, float* out, size_t size) 
            {
                float level = level_;
                while (size--) 
                {
                    SLOPE(level, fabsf(*in), attack_, decay_);
                    *out++ = *in++ / (skew_ + level);
                }
                level_ = level;
            }

        private:
            float attack_;
            float decay_;
            float level_;
            float skew_;
        };

        class OnsetDetector 
        {
        public:
            OnsetDetector() { }
            ~OnsetDetector() { }

            void Init(
                float low,
                float low_mid,
                float mid_high,
                float decimated_sr,
                float ioi_time) {
                float ioi_f = 1.0f / (ioi_time * decimated_sr);
                compressor_.Init(ioi_f * 10.0f, ioi_f * 0.05f, 40.0f);

                low_mid_filter_.Init();
                mid_high_filter_.Init();
                low_mid_filter_.set_f_q(low_mid, 0.5f);
                mid_high_filter_.set_f_q(mid_high, 0.5f);

                attack_[0] = low_mid;
                decay_[0] = low * 0.25f;

                attack_[1] = low_mid;
                decay_[1] = low * 0.25f;

                attack_[2] = low_mid;
                decay_[2] = low * 0.25f;

                for (int i = 0; i <= 3; ++i)
                {
                    envelope_[i] = 0.0f;
                    energy_[i] = 0.0f;
                }

                z_df_.Init(ioi_f * 0.05f);

                inhibit_time_ = static_cast<int>(ioi_time * decimated_sr);
                inhibit_decay_ = 1.0f / (ioi_time * decimated_sr);

                inhibit_threshold_ = 0.0f;
                inhibit_counter_ = 0;
                onset_df_ = 0.0f;
            }

            bool Process(const float* samples, size_t size) {
                // Automatic gain control.
                compressor_.Process(samples, bands_[0], size);

                // Quick and dirty filter bank - split the signal in three bands.
                mid_high_filter_.Split(bands_[0], bands_[1], bands_[2], size);
                low_mid_filter_.Split(bands_[1], bands_[0], bands_[1], size);

                // Compute low-pass energy and onset detection function
                // (derivative of energy) in each band.
                float onset_df = 0.0f;
                float total_energy = 0.0f;
                for (int i = 0; i < 3; ++i) {
                    float* s = bands_[i];
                    float energy = 0.0f;
                    float envelope = envelope_[i];
                    size_t increment = 4 >> i;
                    for (size_t j = 0; j < size; j += increment) {
                        SLOPE(envelope, s[j] * s[j], attack_[i], decay_[i]);
                        energy += envelope;
                    }
                    energy = sqrtf(energy) * float(increment);
                    envelope_[i] = envelope;

                    float derivative = energy - energy_[i];
                    onset_df += derivative + fabsf(derivative);
                    energy_[i] = energy;
                    total_energy += energy;
                }

                onset_df_ += 0.05f * (onset_df - onset_df_);
                bool outlier_in_df = z_df_.Test(onset_df_, 1.0f, 0.01f);
                bool exceeds_energy_threshold = total_energy >= inhibit_threshold_;
                bool not_inhibited = !inhibit_counter_;
                bool has_onset = outlier_in_df && exceeds_energy_threshold && not_inhibited;

                if (has_onset) {
                    inhibit_threshold_ = total_energy * 1.5f;
                    inhibit_counter_ = inhibit_time_;
                }
                else {
                    inhibit_threshold_ -= inhibit_decay_ * inhibit_threshold_;
                    if (inhibit_counter_) {
                        --inhibit_counter_;
                    }
                }
                return has_onset;
            }

        private:
            Compressor compressor_;
            NaiveSvf low_mid_filter_;
            NaiveSvf mid_high_filter_;

            float attack_[3];
            float decay_[3];
            float energy_[3];
            float envelope_[3];
            float onset_df_;

            float bands_[3][32];

            ZScorer z_df_;

            float inhibit_threshold_;
            float inhibit_decay_;
            int inhibit_time_;
            int inhibit_counter_;
        };

        class Strummer 
        {
        public:
            Strummer() { }
            ~Strummer() { }

            void Init(float ioi, float sr) 
            {
                onset_detector_.Init(
                    8.0f / (float)Helpers::CurrentSampleRate,
                    160.0f / (float)Helpers::CurrentSampleRate,
                    1600.0f / (float)Helpers::CurrentSampleRate,
                    sr,
                    ioi);
                inhibit_timer_ = static_cast<int>(ioi * sr);
                inhibit_counter_ = 0;
                previous_note_ = 69.0f;
            }

            void Process(
                const float* in,
                size_t size,
                PerformanceState* performance_state) 
            {

                bool has_onset = in && onset_detector_.Process(in, size);
                bool note_changed = fabs(performance_state->note - previous_note_) > 0.4f;

                int inhibit_timer = inhibit_timer_;
                if (performance_state->internal_strum) 
                {
                    bool has_external_note_cv = !performance_state->internal_note;
                    bool has_external_exciter = !performance_state->internal_exciter;
                    if (has_external_note_cv) 
                    {
                        performance_state->strum = note_changed;
                    }
                    else if (has_external_exciter) 
                    {
                        performance_state->strum = has_onset;
                        // Use longer inhibit time for onset detector.
                        inhibit_timer *= 4;
                    }
                    else 
                    {
                        // Nothing is connected. Should the module play itself in this case?
                        performance_state->strum = false;
                    }
                }

                if (inhibit_counter_) 
                {
                    --inhibit_counter_;
                    performance_state->strum = false;
                }
                else 
                {
                    if (performance_state->strum) 
                    {
                        inhibit_counter_ = inhibit_timer;
                    }
                }
                previous_note_ = performance_state->note;
            }

        private:
            float previous_note_;
            int inhibit_counter_;
            int inhibit_timer_;

            OnsetDetector onset_detector_;
        };

        struct Patch 
        {
            float frequency = 0.5f;
            float structure = 0.5f;
            float brightness = 0.5f;
            float damping = 0.5f;
            float position = 0.5f;
        };

        enum ResonatorModel 
        {
            RESONATOR_MODEL_MODAL,
            RESONATOR_MODEL_SYMPATHETIC_STRING,
            RESONATOR_MODEL_STRING,

            RESONATOR_MODEL_LAST
        };

        class OnePole {
        public:
            OnePole() { }
            ~OnePole() { }

            void Init() {
                set_f<FREQUENCY_DIRTY>(0.01f);
                Reset();
            }

            void Reset() {
                state_ = 0.0f;
            }

            template<FrequencyApproximation approximation>
            static inline float tan(float f) {
                if (approximation == FREQUENCY_EXACT) {
                    // Clip coefficient to about 100.
                    f = f < 0.497f ? f : 0.497f;
                    return tanf(M_PI_F * f);
                }
                else if (approximation == FREQUENCY_DIRTY) {
                    // Optimized for frequencies below 8kHz.
                    const float a = 3.736e-01f * M_PI_POW_3;
                    return f * (M_PI_F + a * f * f);
                }
                else if (approximation == FREQUENCY_FAST) {
                    // The usual tangent approximation uses 3.1755e-01 and 2.033e-01, but
                    // the coefficients used here are optimized to minimize error for the
                    // 16Hz to 16kHz range, with a sample rate of 48kHz.
                    const float a = 3.260e-01f * M_PI_POW_3;
                    const float b = 1.823e-01f * M_PI_POW_5;
                    float f2 = f * f;
                    return f * (M_PI_F + f2 * (a + b * f2));
                }
                else if (approximation == FREQUENCY_ACCURATE) {
                    // These coefficients don't need to be tweaked for the audio range.
                    const float a = 3.333314036e-01f * M_PI_POW_3;
                    const float b = 1.333923995e-01f * M_PI_POW_5;
                    const float c = 5.33740603e-02f * M_PI_POW_7;
                    const float d = 2.900525e-03f * M_PI_POW_9;
                    const float e = 9.5168091e-03f * M_PI_POW_11;
                    float f2 = f * f;
                    return f * (M_PI_F + f2 * (a + f2 * (b + f2 * (c + f2 * (d + f2 * e)))));
                }
            }

            // Set frequency and resonance from true units. Various approximations
            // are available to avoid the cost of tanf.
            template<FrequencyApproximation approximation>
            inline void set_f(float f) {
                g_ = tan<approximation>(f);
                gi_ = 1.0f / (1.0f + g_);
            }

            template<FilterMode mode>
            inline float Process(float in) {
                float lp;
                lp = (g_ * in + state_) * gi_;
                state_ = g_ * (in - lp) + lp;

                if (mode == FILTER_MODE_LOW_PASS) {
                    return lp;
                }
                else if (mode == FILTER_MODE_HIGH_PASS) {
                    return in - lp;
                }
                else {
                    return 0.0f;
                }
            }

            template<FilterMode mode>
            inline void Process(float* in_out, size_t size) {
                while (size--) {
                    *in_out = Process<mode>(*in_out);
                    ++in_out;
                }
            }

        private:
            float g_;
            float gi_;
            float state_;
        };


        class Svf 
        {
        public:
            Svf() { }
            ~Svf() { }

            void Init() {
                set_f_q<FREQUENCY_DIRTY>(0.01f, 100.0f);
                Reset();
            }

            void Reset() {
                state_1_ = state_2_ = 0.0f;
            }

            // Copy settings from another filter.
            inline void set(const Svf& f) {
                g_ = f.g();
                r_ = f.r();
                h_ = f.h();
            }

            // Set all parameters from LUT.
            inline void set_g_r_h(float g, float r, float h) {
                g_ = g;
                r_ = r;
                h_ = h;
            }

            // Set frequency and resonance coefficients from LUT, adjust remaining
            // parameter.
            inline void set_g_r(float g, float r) {
                g_ = g;
                r_ = r;
                h_ = 1.0f / (1.0f + r_ * g_ + g_ * g_);
            }

            // Set frequency from LUT, resonance in true units, adjust the rest.
            inline void set_g_q(float g, float resonance) {
                g_ = g;
                r_ = 1.0f / resonance;
                h_ = 1.0f / (1.0f + r_ * g_ + g_ * g_);
            }

            // Set frequency and resonance from true units. Various approximations
            // are available to avoid the cost of tanf.
            template<FrequencyApproximation approximation>
            inline void set_f_q(float f, float resonance) {
                g_ = OnePole::tan<approximation>(f);
                r_ = 1.0f / resonance;
                h_ = 1.0f / (1.0f + r_ * g_ + g_ * g_);
            }

            template<FilterMode mode>
            inline float Process(float in) {
                float hp, bp, lp;
                hp = (in - r_ * state_1_ - g_ * state_1_ - state_2_) * h_;
                bp = g_ * hp + state_1_;
                state_1_ = g_ * hp + bp;
                lp = g_ * bp + state_2_;
                state_2_ = g_ * bp + lp;

                if (mode == FILTER_MODE_LOW_PASS) {
                    return lp;
                }
                else if (mode == FILTER_MODE_BAND_PASS) {
                    return bp;
                }
                else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                    return bp * r_;
                }
                else if (mode == FILTER_MODE_HIGH_PASS) {
                    return hp;
                }
            }

            template<FilterMode mode_1, FilterMode mode_2>
            inline void Process(float in, float* out_1, float* out_2) {
                float hp, bp, lp;
                hp = (in - r_ * state_1_ - g_ * state_1_ - state_2_) * h_;
                bp = g_ * hp + state_1_;
                state_1_ = g_ * hp + bp;
                lp = g_ * bp + state_2_;
                state_2_ = g_ * bp + lp;

                if (mode_1 == FILTER_MODE_LOW_PASS) {
                    *out_1 = lp;
                }
                else if (mode_1 == FILTER_MODE_BAND_PASS) {
                    *out_1 = bp;
                }
                else if (mode_1 == FILTER_MODE_BAND_PASS_NORMALIZED) {
                    *out_1 = bp * r_;
                }
                else if (mode_1 == FILTER_MODE_HIGH_PASS) {
                    *out_1 = hp;
                }

                if (mode_2 == FILTER_MODE_LOW_PASS) {
                    *out_2 = lp;
                }
                else if (mode_2 == FILTER_MODE_BAND_PASS) {
                    *out_2 = bp;
                }
                else if (mode_2 == FILTER_MODE_BAND_PASS_NORMALIZED) {
                    *out_2 = bp * r_;
                }
                else if (mode_2 == FILTER_MODE_HIGH_PASS) {
                    *out_2 = hp;
                }
            }

            template<FilterMode mode>
            inline void Process(const float* in, float* out, size_t size) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;

                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;

                    float value;
                    if (mode == FILTER_MODE_LOW_PASS) {
                        value = lp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS) {
                        value = bp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                        value = bp * r_;
                    }
                    else if (mode == FILTER_MODE_HIGH_PASS) {
                        value = hp;
                    }

                    *out = value;
                    ++out;
                    ++in;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            template<FilterMode mode>
            inline void ProcessAdd(const float* in, float* out, size_t size, float gain) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;

                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;

                    float value;
                    if (mode == FILTER_MODE_LOW_PASS) {
                        value = lp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS) {
                        value = bp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                        value = bp * r_;
                    }
                    else if (mode == FILTER_MODE_HIGH_PASS) {
                        value = hp;
                    }

                    *out += gain * value;
                    ++out;
                    ++in;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            template<FilterMode mode>
            inline void Process(const float* in, float* out, size_t size, size_t stride) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;

                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;

                    float value;
                    if (mode == FILTER_MODE_LOW_PASS) {
                        value = lp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS) {
                        value = bp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                        value = bp * r_;
                    }
                    else if (mode == FILTER_MODE_HIGH_PASS) {
                        value = hp;
                    }

                    *out = value;
                    out += stride;
                    in += stride;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            inline void ProcessMultimode(
                const float* in,
                float* out,
                size_t size,
                float mode) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;
                float hp_gain = mode < 0.5f ? -mode * 2.0f : -2.0f + mode * 2.0f;
                float lp_gain = mode < 0.5f ? 1.0f - mode * 2.0f : 0.0f;
                float bp_gain = mode < 0.5f ? 0.0f : mode * 2.0f - 1.0f;
                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;
                    *out = hp_gain * hp + bp_gain * bp + lp_gain * lp;
                    ++in;
                    ++out;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            inline void ProcessMultimodeLPtoHP(
                const float* in,
                float* out,
                size_t size,
                float mode) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;
                float hp_gain = Helpers::Min(-mode * 2.0f + 1.0f, 0.0f);
                float bp_gain = 1.0f - 2.0f * fabsf(mode - 0.5f);
                float lp_gain = Helpers::Max(1.0f - mode * 2.0f, 0.0f);
                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;
                    *out = hp_gain * hp + bp_gain * bp + lp_gain * lp;
                    ++in;
                    ++out;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            template<FilterMode mode>
            inline void Process(
                const float* in, float* out_1, float* out_2, size_t size,
                float gain_1, float gain_2) {
                float hp, bp, lp;
                float state_1 = state_1_;
                float state_2 = state_2_;

                while (size--) {
                    hp = (*in - r_ * state_1 - g_ * state_1 - state_2) * h_;
                    bp = g_ * hp + state_1;
                    state_1 = g_ * hp + bp;
                    lp = g_ * bp + state_2;
                    state_2 = g_ * bp + lp;

                    float value;
                    if (mode == FILTER_MODE_LOW_PASS) {
                        value = lp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS) {
                        value = bp;
                    }
                    else if (mode == FILTER_MODE_BAND_PASS_NORMALIZED) {
                        value = bp * r_;
                    }
                    else if (mode == FILTER_MODE_HIGH_PASS) {
                        value = hp;
                    }

                    *out_1 += value * gain_1;
                    *out_2 += value * gain_2;
                    ++out_1;
                    ++out_2;
                    ++in;
                }
                state_1_ = state_1;
                state_2_ = state_2;
            }

            inline float g() const { return g_; }
            inline float r() const { return r_; }
            inline float h() const { return h_; }

        private:
            float g_;
            float r_;
            float h_;

            float state_1_;
            float state_2_;
        };

        class Resonator {
        public:
            Resonator() { }
            ~Resonator() { }

            void Init();
            void Process(
                const float* in,
                float* out,
                float* aux,
                size_t size);

            inline void set_frequency(float frequency) {
                frequency_ = frequency;
            }

            inline void set_structure(float structure) {
                structure_ = structure;
            }

            inline void set_brightness(float brightness) {
                brightness_ = brightness;
            }

            inline void set_damping(float damping) {
                damping_ = damping;
            }

            inline void set_position(float position) {
                position_ = position;
            }

            inline void set_resolution(int resolution) {
                resolution -= resolution & 1; // Must be even!
                resolution_ = (resolution < kMaxModes) ? resolution : kMaxModes;
            }

        private:
            int ComputeFilters();
            float frequency_;
            float structure_;
            float brightness_;
            float position_;
            float previous_position_;
            float damping_;

            int resolution_;

            Svf f_[kMaxModes];
        };

        static const size_t kDelayLineSize = 2048;

        class DampingFilter {
        public:
            DampingFilter() { }
            ~DampingFilter() { }

            void Init() {
                x_ = 0.0f;
                x__ = 0.0f;
                brightness_ = 0.0f;
                brightness_increment_ = 0.0f;
                damping_ = 0.0f;
                damping_increment_ = 0.0f;
            }

            inline void Configure(float damping, float brightness, size_t size) {
                if (!size) {
                    damping_ = damping;
                    brightness_ = brightness;
                    damping_increment_ = 0.0f;
                    brightness_increment_ = 0.0f;
                }
                else {
                    float step = 1.0f / static_cast<float>(size);
                    damping_increment_ = (damping - damping_) * step;
                    brightness_increment_ = (brightness - brightness_) * step;
                }
            }

            inline float Process(float x) {
                float h0 = (1.0f + brightness_) * 0.5f;
                float h1 = (1.0f - brightness_) * 0.25f;
                float y = damping_ * (h0 * x_ + h1 * (x + x__));
                x__ = x_;
                x_ = x;
                brightness_ += brightness_increment_;
                damping_ += damping_increment_;
                return y;
            }
        private:
            float x_;
            float x__;
            float brightness_;
            float brightness_increment_;
            float damping_;
            float damping_increment_;
        };

        template<typename T, size_t max_delay>
        class DelayLine {
        public:
            DelayLine() { }
            ~DelayLine() { }

            void Init() {
                Reset();
            }

            void Reset() {
                for (int i = 0; i < max_delay; ++i)
                    line_[i] = T(0);
                delay_ = 1;
                write_ptr_ = 0;
            }

            inline void set_delay(size_t delay) {
                delay_ = delay;
            }

            inline void Write(const T sample) {
                line_[write_ptr_] = sample;
                write_ptr_ = (write_ptr_ - 1 + max_delay) % max_delay;
            }

            inline const T Allpass(const T sample, size_t delay, const T coefficient) {
                T read = line_[(write_ptr_ + delay) % max_delay];
                T write = sample + coefficient * read;
                Write(write);
                return -write * coefficient + read;
            }

            inline const T WriteRead(const T sample, float delay) {
                Write(sample);
                return Read(delay);
            }

            inline const T Read() const {
                return line_[(write_ptr_ + delay_) % max_delay];
            }

            inline const T Read(size_t delay) const {
                return line_[(write_ptr_ + delay) % max_delay];
            }

            inline const T Read(float delay) const {
                MAKE_INTEGRAL_FRACTIONAL(delay)
                    const T a = line_[(write_ptr_ + delay_integral) % max_delay];
                const T b = line_[(write_ptr_ + delay_integral + 1) % max_delay];
                return a + (b - a) * delay_fractional;
            }

            inline const T ReadHermite(float delay) const {
                MAKE_INTEGRAL_FRACTIONAL(delay)
                    int t = (write_ptr_ + delay_integral + max_delay);
                const T xm1 = line_[(t - 1) % max_delay];
                const T x0 = line_[(t) % max_delay];
                const T x1 = line_[(t + 1) % max_delay];
                const T x2 = line_[(t + 2) % max_delay];
                const float c = (x1 - xm1) * 0.5f;
                const float v = x0 - x1;
                const float w = c + v;
                const float a = w + v + (x2 - x0) * 0.5f;
                const float b_neg = w + a;
                const float f = delay_fractional;
                return (((a * f) - b_neg) * f + c) * f + x0;
            }

        private:
            size_t write_ptr_;
            size_t delay_;
            T line_[max_delay];
        };


        class DCBlocker {
        public:
            DCBlocker() { }
            ~DCBlocker() { }

            void Init(float pole) {
                x_ = 0.0f;
                y_ = 0.0f;
                pole_ = pole;
            }

            inline void Process(float* in_out, size_t size) {
                float x = x_;
                float y = y_;
                const float pole = pole_;
                while (size--) {
                    float old_x = x;
                    x = *in_out;
                    *in_out++ = y = y * pole + x - old_x;
                }
                x_ = x;
                y_ = y;
            }

        private:
            float pole_;
            float x_;
            float y_;
        };

        typedef DelayLine<float, kDelayLineSize> StringDelayLine;
        typedef DelayLine<float, kDelayLineSize / 2> StiffnessDelayLine;

        class String {
        public:
            String() { }
            ~String() { }

            void Init(bool enable_dispersion);
            void Process(const float* in, float* out, float* aux, size_t size);

            inline void set_frequency(float frequency) {
                frequency_ = frequency;
            }

            inline void set_frequency(float frequency, float coefficient) {
                frequency_ += coefficient * (frequency - frequency_);
            }

            inline void set_dispersion(float dispersion) {
                dispersion_ = dispersion;
            }

            inline void set_brightness(float brightness) {
                brightness_ = brightness;
            }

            inline void set_damping(float damping) {
                damping_ = damping;
            }

            inline void set_position(float position) {
                position_ = position;
            }

            inline StringDelayLine* mutable_string() { return &string_; }

        private:
            template<bool enable_dispersion>
            void ProcessInternal(const float* in, float* out, float* aux, size_t size);

            float frequency_;
            float dispersion_;
            float brightness_;
            float damping_;
            float position_;

            float delay_;
            float clamped_position_;
            float previous_dispersion_;
            float previous_damping_compensation_;

            bool enable_dispersion_;
            bool enable_iir_damping_;
            float dispersion_noise_;

            // Very crappy linear interpolation upsampler used for low pitches that
            // do not fit the delay line. Rarely used.
            float src_phase_;
            float out_sample_[2];
            float aux_sample_[2];

            float curved_bridge_;

            StringDelayLine string_;
            StiffnessDelayLine stretch_;

            DampingFilter fir_damping_filter_;
            Svf iir_damping_filter_;
            DCBlocker dc_blocker_;
        };

        enum CosineOscillatorMode {
            COSINE_OSCILLATOR_APPROXIMATE,
            COSINE_OSCILLATOR_EXACT
        };

        class CosineOscillator {
        public:
            CosineOscillator() { }
            ~CosineOscillator() { }

            template<CosineOscillatorMode mode>
            inline void Init(float frequency) {
                if (mode == COSINE_OSCILLATOR_APPROXIMATE) {
                    InitApproximate(frequency);
                }
                else {
                    iir_coefficient_ = 2.0f * cosf(2.0f * M_PI_F * frequency);
                    initial_amplitude_ = iir_coefficient_ * 0.25f;
                }
                Start();
            }

            inline void InitApproximate(float frequency) {
                float sign = 16.0f;
                frequency -= 0.25f;
                if (frequency < 0.0f) {
                    frequency = -frequency;
                }
                else {
                    if (frequency > 0.5f) {
                        frequency -= 0.5f;
                    }
                    else {
                        sign = -16.0f;
                    }
                }
                iir_coefficient_ = sign * frequency * (1.0f - 2.0f * frequency);
                initial_amplitude_ = iir_coefficient_ * 0.25f;
            }

            inline void Start() {
                y1_ = initial_amplitude_;
                y0_ = 0.5f;
            }

            inline float value() const {
                return y1_ + 0.5f;
            }

            inline float Next() {
                float temp = y0_;
                y0_ = iir_coefficient_ * y0_ - y1_;
                y1_ = temp;
                return temp + 0.5f;
            }

        private:
            float y1_;
            float y0_;
            float iir_coefficient_;
            float initial_amplitude_;
        };

        class Follower {
        public:
            Follower() { }
            ~Follower() { }

            void Init(float low, float low_mid, float mid_high) {
                low_mid_filter_.Init();
                mid_high_filter_.Init();

                low_mid_filter_.set_f_q(low_mid, 0.5f);
                mid_high_filter_.set_f_q(mid_high, 0.5f);
                attack_[0] = low_mid;
                decay_[0] = sqrtf(low_mid * low);

                attack_[1] = sqrtf(low_mid * mid_high);
                decay_[1] = low_mid;

                attack_[2] = sqrtf(mid_high * 0.5f);
                decay_[2] = sqrtf(mid_high * low_mid);

                for (int i = 0; i < 3; ++i)
                    detector_[i] = 0.0f;

                centroid_ = 0.0f;
            }

            void Process(
                float sample,
                float* envelope,
                float* centroid) {
                float bands[3] = { 0.0f, 0.0f, 0.0f };

                bands[2] = mid_high_filter_.Process<FILTER_MODE_HIGH_PASS>(sample);
                bands[1] = low_mid_filter_.Process<FILTER_MODE_HIGH_PASS>(
                    mid_high_filter_.lp());
                bands[0] = low_mid_filter_.lp();

                float weighted = 0.0f;
                float total = 0.0f;
                float frequency = 0.0f;
                for (int i = 0; i < 3; ++i) {
                    SLOPE(detector_[i], fabsf(bands[i]), attack_[i], decay_[i]);
                    weighted += detector_[i] * frequency;
                    total += detector_[i];
                    frequency += 0.5f;
                }

                float error = weighted / (total + 0.001f) - centroid_;
                float coefficient = error > 0.0f ? 0.05f : 0.001f;
                centroid_ += error * coefficient;

                *envelope = total;
                *centroid = centroid_;
            }

        private:
            NaiveSvf low_mid_filter_;
            NaiveSvf mid_high_filter_;

            float attack_[3];
            float decay_[3];
            float detector_[3];

            float centroid_;
        };

        
        class Plucker {
        public:
            Plucker() { }
            ~Plucker() { }

            void Init() {
                svf_.Init();
                comb_filter_.Init();
                remaining_samples_ = 0;
                comb_filter_period_ = 0.0f;
            }

            void Trigger(float frequency, float cutoff, float position) {
                float ratio = position * 0.9f + 0.05f;
                float comb_period = 1.0f / frequency * ratio;
                remaining_samples_ = static_cast<size_t>(comb_period);
                while (comb_period >= 255.0f) {
                    comb_period *= 0.5f;
                }
                comb_filter_period_ = comb_period;
                comb_filter_gain_ = (1.0f - position) * 0.8f;
                svf_.set_f_q<FREQUENCY_DIRTY>(Helpers::Min(cutoff, 0.499f), 1.0f);
            }

            void Process(float* out, size_t size) {
                const float comb_gain = comb_filter_gain_;
                const float comb_delay = comb_filter_period_;
                for (size_t i = 0; i < size; ++i) {
                    float in = 0.0f;
                    if (remaining_samples_) {
                        in = 2.0f * Helpers::RandFloat() - 1.0f;
                        --remaining_samples_;
                    }
                    out[i] = in + comb_gain * comb_filter_.Read(comb_delay);
                    comb_filter_.Write(out[i]);
                }
                svf_.Process<FILTER_MODE_LOW_PASS>(out, out, size);
            }

        private:
            Svf svf_;
            DelayLine<float, 256> comb_filter_;
            size_t remaining_samples_;
            float comb_filter_period_;
            float comb_filter_gain_;
        };


        class NoteFilter {
        public:
            enum {
                N = 4  // Median filter order
            };
            NoteFilter() { }
            ~NoteFilter() { }

            void Init(
                float sample_rate,
                float time_constant_fast_edge,
                float time_constant_steady_part,
                float edge_recovery_time,
                float edge_avoidance_delay) {
                fast_coefficient_ = 1.0f / (time_constant_fast_edge * sample_rate);
                slow_coefficient_ = 1.0f / (time_constant_steady_part * sample_rate);
                lag_coefficient_ = 1.0f / (edge_recovery_time * sample_rate);

                size_t delay = size_t(edge_avoidance_delay * sample_rate);
                if (delay > 15)
                    delay = 15;
                delayed_stable_note_.Init();
                delayed_stable_note_.set_delay(delay);

                stable_note_ = note_ = 69.0f;
                coefficient_ = fast_coefficient_;
                stable_coefficient_ = slow_coefficient_;
                for (int i = 0; i < N; ++i)
                    previous_values_[i] = note_;
            }

            static void bubbleSort(float arr[], int n) 
            {
                for (int i = 0; i < n - 1; i++) 
                {
                    for (int j = 0; j < n - i - 1; j++) 
                    {
                        if (arr[j] > arr[j + 1]) 
                        {
                            // Swap arr[j] and arr[j+1]
                            int temp = arr[j];
                            arr[j] = arr[j + 1];
                            arr[j + 1] = temp;
                        }
                    }
                }
            }

            inline float Process(float note, bool strum) {
                // If there is a sharp change, follow it instantly.
                if (fabs(note - note_) > 0.4f || strum) {
                    stable_note_ = note_ = note;
                    coefficient_ = fast_coefficient_;
                    stable_coefficient_ = slow_coefficient_;
                    for (int i = 0; i < N; ++i)
                        previous_values_[i] = note;
                }
                else {
                    // Median filtering of the raw ADC value.
                    float sorted_values[N];
                    float p = previous_values_[0];
                    previous_values_[0] = previous_values_[1];
                    previous_values_[1] = previous_values_[2];
                    previous_values_[2] = previous_values_[3];
                    previous_values_[3] = note;
                    for (int i = 0; i < N; ++i)
                        sorted_values[i] = previous_values_[i];
                    bubbleSort(sorted_values, 4);
                    float median = 0.5f * (sorted_values[(N - 1) / 2] + sorted_values[N / 2]);

                    // Adaptive lag processor.
                    note_ += coefficient_ * (median - note_);
                    stable_note_ += stable_coefficient_ * (note_ - stable_note_);

                    coefficient_ += lag_coefficient_ * (slow_coefficient_ - coefficient_);
                    stable_coefficient_ += lag_coefficient_ * \
                        (lag_coefficient_ - stable_coefficient_);

                    delayed_stable_note_.Write(stable_note_);
                }
                return note_;
            }

            inline float note() const {
                return note_;
            }

            inline float stable_note() const {
                return delayed_stable_note_.Read();
            }

        private:
            float previous_values_[N];
            float note_;
            float stable_note_;
            DelayLine<float, 16> delayed_stable_note_;

            float coefficient_;
            float stable_coefficient_;

            float fast_coefficient_;
            float slow_coefficient_;
            float lag_coefficient_;
        };

        static inline float SoftLimit(float x) {
            return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
        }


        class Limiter {
        public:
            Limiter() { }
            ~Limiter() { }

            void Init() {
                peak_ = 0.5f;
            }

            void Process(
                float* l,
                float* r,
                size_t size,
                float pre_gain) {
                while (size--) {
                    float l_pre = *l * pre_gain;
                    float r_pre = *r * pre_gain;

                    float l_peak = fabs(l_pre);
                    float r_peak = fabs(r_pre);
                    float s_peak = fabs(r_pre - l_pre);

                    float peak = Helpers::Max(Helpers::Max(l_peak, r_peak), s_peak);
                    SLOPE(peak_, peak, 0.05f, 0.00002f);

                    // Clamp to 8Vpp, clipping softly towards 10Vpp
                    float gain = (peak_ <= 1.0f ? 1.0f : 1.0f / peak_);
                    *l++ = SoftLimit(l_pre * gain * 0.8f);
                    *r++ = SoftLimit(r_pre * gain * 0.8f);
                }
            }

        private:
            float peak_;
        };




        class Part 
        {
        public:
            Part()  { }
            ~Part() { }

            void Init();

            void Process(
                const PerformanceState& performance_state,
                const Patch& patch,
                const float* in,
                float* out,
                float* aux,
                size_t size);

            inline bool bypass() const { return bypass_; }
            inline void set_bypass(bool bypass) { bypass_ = bypass; }

            inline int polyphony() const { return polyphony_; }
            inline void set_polyphony(int polyphony) {
                int old_polyphony = polyphony_;
                polyphony_ = (polyphony < kMaxPolyphony) ? polyphony : kMaxPolyphony;
                for (int i = old_polyphony; i < polyphony_; ++i) {
                    note_[i] = note_[0] + i * 0.05f;
                }
                dirty_ = true;
            }

            inline ResonatorModel model() const { return model_; }
            inline void set_model(ResonatorModel model) {
                if (model != model_) {
                    model_ = model;
                    dirty_ = true;
                }
            }

        private:
            void ConfigureResonators();
            void RenderModalVoice(
                int voice,
                const PerformanceState& performance_state,
                const Patch& patch,
                float frequency,
                float filter_cutoff,
                size_t size);
            void RenderStringVoice(
                int voice,
                const PerformanceState& performance_state,
                const Patch& patch,
                float frequency,
                float filter_cutoff,
                size_t size);
            void RenderFMVoice(
                int voice,
                const PerformanceState& performance_state,
                const Patch& patch,
                float frequency,
                float filter_cutoff,
                size_t size);


            inline float Squash(float x) const {
                if (x < 0.5f) {
                    x *= 2.0f;
                    x *= x;
                    x *= x;
                    x *= x;
                    x *= x;
                    x *= 0.5f;
                }
                else {
                    x = 2.0f - 2.0f * x;
                    x *= x;
                    x *= x;
                    x *= x;
                    x *= x;
                    x = 1.0f - 0.5f * x;
                }
                return x;
            }

            void ComputeSympatheticStringsNotes(
                float tonic,
                float note,
                float parameter,
                float* destination,
                size_t num_strings);

            bool bypass_;
            bool dirty_;

            ResonatorModel model_;

            int num_voices_;
            int active_voice_;
            unsigned int step_counter_;
            int polyphony_;

            Resonator resonator_[kMaxPolyphony];
            String string_[kNumStrings];
            CosineOscillator lfo_[kNumStrings];

            Svf excitation_filter_[kMaxPolyphony];
            DCBlocker dc_blocker_[kMaxPolyphony];
            Plucker plucker_[kMaxPolyphony];

            float note_[kMaxPolyphony];
            NoteFilter note_filter_;

            float* resonator_input_;
            float* sympathetic_resonator_input_;
            float* noise_burst_buffer_;

            float* out_buffer_;
            float* aux_buffer_;

            Limiter limiter_;

            static float model_gains_[RESONATOR_MODEL_LAST];
        };

        Strummer strummer;
        Part part;
        Patch patch;
	};
}

#endif
