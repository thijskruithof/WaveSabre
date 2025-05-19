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

#include <WaveSabreCore/Perseus.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>


/**
Todo:
- Take deltaSamples into account from NoteOn, and allow for multiple NodeOn events within a single buffer
- Derive from SynthDevice instead of Device?
**/


namespace WaveSabreCore
{
	Perseus::Perseus()
		: Device((int)ParamIndices::NumParams)
	{
        strummer.Init(0.01f, Helpers::CurrentSampleRate / kMaxBlockSize);
        part.Init();
	}

	Perseus::~Perseus()
	{
	}

	void Perseus::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
        for (int i = 0; i < numSamples; ++i)
        {
            // Get input
            if (!inputBuffer.full()) {
                float f;
                f = inputs[0][i];
                inputBuffer.push(f);
            }

            //// TODO: Get strum from note on?
            //if (!strum) {
            //    strum = false;
            //    //strum = inputs[STRUM_INPUT].getVoltage() >= 1.0;
            //}

            // Render frames
            if (outputBuffer.empty()) {
                float in[24] = {};
                // Convert input buffer
                {
                    // Copy inputBuffer to in
                    if (inputBuffer.size() < 24)
                        memcpy(in + (24 - inputBuffer.size()), inputBuffer.startData(), inputBuffer.size() * 4);
                    else
                        memcpy(in, inputBuffer.startData(), 24 * 4);
                    //inputSrc.setRates(args.sampleRate, 48000);
                    inputBuffer.startIncr(inputBuffer.size());
                }

                // Polyphony
                int polyphony = 1 << polyphonyMode;
                if (part.polyphony() != polyphony)
                    part.set_polyphony(polyphony);
                // Model
                part.set_model(resonatorModel);

                //// Patch
                //Patch patch;
                //float structure = paramStructure;// +3.3 * dsp::quadraticBipolar(params[STRUCTURE_MOD_PARAM].getValue()) * inputs[STRUCTURE_MOD_INPUT].getVoltage() / 5.0;
                //patch.structure = clamp(structure, 0.0f, 0.9995f);
                //patch.brightness = paramBrightness; // clamp(params[BRIGHTNESS_PARAM].getValue() + 3.3 * dsp::quadraticBipolar(params[BRIGHTNESS_MOD_PARAM].getValue()) * inputs[BRIGHTNESS_MOD_INPUT].getVoltage() / 5.0, 0.0f, 1.0f);
                //patch.damping = paramDamping; // clamp(params[DAMPING_PARAM].getValue() + 3.3 * dsp::quadraticBipolar(params[DAMPING_MOD_PARAM].getValue()) * inputs[DAMPING_MOD_INPUT].getVoltage() / 5.0, 0.0f, 0.9995f);
                //patch.position = clamp(params[POSITION_PARAM].getValue() + 3.3 * dsp::quadraticBipolar(params[POSITION_MOD_PARAM].getValue()) * inputs[POSITION_MOD_INPUT].getVoltage() / 5.0, 0.0f, 0.9995f);

                // Performance
                PerformanceState performance_state;
                performance_state.note = strumNote; // 12.0 * inputs[PITCH_INPUT].getNormalVoltage(1 / 12.0);
                float transpose = patch.frequency; // params[FREQUENCY_PARAM].getValue();
                // Quantize transpose if pitch input is connected
                //if (inputs[PITCH_INPUT].isConnected()) {
                //    transpose = roundf(transpose);
                //}
                performance_state.tonic = 12.0 + Helpers::Clamp(transpose, 0.0f, 60.0f);
                performance_state.fm = 0.0f; // clamp(48.0 * 3.3 * dsp::quarticBipolar(params[FREQUENCY_MOD_PARAM].getValue()) * inputs[FREQUENCY_MOD_INPUT].getNormalVoltage(1.0) / 5.0, -48.0f, 48.0f);

                performance_state.internal_exciter = true; // !inputs[IN_INPUT].isConnected();
                performance_state.internal_strum = false; // !inputs[STRUM_INPUT].isConnected();
                performance_state.internal_note = true; // !inputs[PITCH_INPUT].isConnected();

                performance_state.strum = strum && !lastStrum;
                lastStrum = strum;
                strum = false;

                performance_state.chord = Helpers::Clamp((int)roundf(patch.structure * (kNumChords - 1)), 0, kNumChords - 1);

                // Process audio
                float out[24];
                float aux[24];
                {
                    strummer.Process(in, 24, &performance_state);
                    part.Process(performance_state, patch, in, out, aux, 24);
                }

                // Convert output buffer
                {
                    OutputSample outputFrames[24];
                    for (int i = 0; i < 24; i++) {
                        outputFrames[i].L = out[i];
                        outputFrames[i].R = aux[i];
                    }

                    memcpy(outputBuffer.endData(), outputFrames, 24 * sizeof(OutputSample));
                    //outputSrc.setRates(48000, args.sampleRate);
                    //int inLen = 24;
                    int outLen = (outputBuffer.capacity() < 24) ? outputBuffer.capacity() : 24;
                    //outputSrc.process(outputFrames, &inLen, outputBuffer.endData(), &outLen);
                    outputBuffer.endIncr(outLen);
                }
            }

            // Set output
            if (!outputBuffer.empty()) {
                OutputSample outputFrame = outputBuffer.shift();

                outputs[0][i] = Helpers::Clamp(outputFrame.L, -1.0f, 1.0f);
                outputs[1][i] = Helpers::Clamp(outputFrame.R, -1.0f, 1.0f);

                //// "Note that you need to insert a jack into each output to split the signals: when only one jack is inserted, both signals are mixed together."
                //if (outputs[ODD_OUTPUT].isConnected() && outputs[EVEN_OUTPUT].isConnected()) {
                //    outputs[ODD_OUTPUT].setVoltage(clamp(outputFrame.samples[0], -1.0, 1.0) * 5.0);
                //    outputs[EVEN_OUTPUT].setVoltage(clamp(outputFrame.samples[1], -1.0, 1.0) * 5.0);
                //}
                //else {
                //    float v = clamp(outputFrame.samples[0] + outputFrame.samples[1], -1.0, 1.0) * 5.0;
                //    outputs[ODD_OUTPUT].setVoltage(v);
                //    outputs[EVEN_OUTPUT].setVoltage(v);
                //}
            }
        }
	}

	void Perseus::SetParam(int index, float value)
	{
        switch ((ParamIndices)index)
        {
        case ParamIndices::Frequency:	patch.frequency = value; break;
        case ParamIndices::Structure:	patch.structure = value; break;
        case ParamIndices::Brightness:	patch.brightness = value; break;
        case ParamIndices::Damping:		patch.damping = value; break;
        case ParamIndices::Position:	patch.position = value; break;
        }
	}

	float Perseus::GetParam(int index) const
	{
        switch ((ParamIndices)index)
        {
        case ParamIndices::Frequency:	return patch.frequency;
        case ParamIndices::Structure:	return patch.structure;
        case ParamIndices::Brightness:	return patch.brightness;
        case ParamIndices::Damping:		return patch.damping;
        case ParamIndices::Position:	return patch.position;
        }

        return 0.0f;
	}

    void Perseus::AllNotesOff()
    {

    }

    void Perseus::NoteOn(int note, int velocity, int deltaSamples)
    {
        strum = true;
        strumNote = note;
    }

    void Perseus::NoteOff(int note, int deltaSamples)
    {

    }


    const float lut_pitch_ratio_high[] = {
   6.151958251e-04,  6.517772725e-04,  6.905339660e-04,  7.315952524e-04,
   7.750981699e-04,  8.211879055e-04,  8.700182794e-04,  9.217522585e-04,
   9.765625000e-04,  1.034631928e-03,  1.096154344e-03,  1.161335073e-03,
   1.230391650e-03,  1.303554545e-03,  1.381067932e-03,  1.463190505e-03,
   1.550196340e-03,  1.642375811e-03,  1.740036559e-03,  1.843504517e-03,
   1.953125000e-03,  2.069263856e-03,  2.192308688e-03,  2.322670146e-03,
   2.460783301e-03,  2.607109090e-03,  2.762135864e-03,  2.926381010e-03,
   3.100392680e-03,  3.284751622e-03,  3.480073118e-03,  3.687009034e-03,
   3.906250000e-03,  4.138527712e-03,  4.384617376e-03,  4.645340293e-03,
   4.921566601e-03,  5.214218180e-03,  5.524271728e-03,  5.852762019e-03,
   6.200785359e-03,  6.569503244e-03,  6.960146235e-03,  7.374018068e-03,
   7.812500000e-03,  8.277055425e-03,  8.769234752e-03,  9.290680586e-03,
   9.843133202e-03,  1.042843636e-02,  1.104854346e-02,  1.170552404e-02,
   1.240157072e-02,  1.313900649e-02,  1.392029247e-02,  1.474803614e-02,
   1.562500000e-02,  1.655411085e-02,  1.753846950e-02,  1.858136117e-02,
   1.968626640e-02,  2.085687272e-02,  2.209708691e-02,  2.341104808e-02,
   2.480314144e-02,  2.627801298e-02,  2.784058494e-02,  2.949607227e-02,
   3.125000000e-02,  3.310822170e-02,  3.507693901e-02,  3.716272234e-02,
   3.937253281e-02,  4.171374544e-02,  4.419417382e-02,  4.682209615e-02,
   4.960628287e-02,  5.255602595e-02,  5.568116988e-02,  5.899214454e-02,
   6.250000000e-02,  6.621644340e-02,  7.015387802e-02,  7.432544469e-02,
   7.874506562e-02,  8.342749089e-02,  8.838834765e-02,  9.364419230e-02,
   9.921256575e-02,  1.051120519e-01,  1.113623398e-01,  1.179842891e-01,
   1.250000000e-01,  1.324328868e-01,  1.403077560e-01,  1.486508894e-01,
   1.574901312e-01,  1.668549818e-01,  1.767766953e-01,  1.872883846e-01,
   1.984251315e-01,  2.102241038e-01,  2.227246795e-01,  2.359685782e-01,
   2.500000000e-01,  2.648657736e-01,  2.806155121e-01,  2.973017788e-01,
   3.149802625e-01,  3.337099635e-01,  3.535533906e-01,  3.745767692e-01,
   3.968502630e-01,  4.204482076e-01,  4.454493591e-01,  4.719371563e-01,
   5.000000000e-01,  5.297315472e-01,  5.612310242e-01,  5.946035575e-01,
   6.299605249e-01,  6.674199271e-01,  7.071067812e-01,  7.491535384e-01,
   7.937005260e-01,  8.408964153e-01,  8.908987181e-01,  9.438743127e-01,
   1.000000000e+00,  1.059463094e+00,  1.122462048e+00,  1.189207115e+00,
   1.259921050e+00,  1.334839854e+00,  1.414213562e+00,  1.498307077e+00,
   1.587401052e+00,  1.681792831e+00,  1.781797436e+00,  1.887748625e+00,
   2.000000000e+00,  2.118926189e+00,  2.244924097e+00,  2.378414230e+00,
   2.519842100e+00,  2.669679708e+00,  2.828427125e+00,  2.996614154e+00,
   3.174802104e+00,  3.363585661e+00,  3.563594873e+00,  3.775497251e+00,
   4.000000000e+00,  4.237852377e+00,  4.489848193e+00,  4.756828460e+00,
   5.039684200e+00,  5.339359417e+00,  5.656854249e+00,  5.993228308e+00,
   6.349604208e+00,  6.727171322e+00,  7.127189745e+00,  7.550994501e+00,
   8.000000000e+00,  8.475704755e+00,  8.979696386e+00,  9.513656920e+00,
   1.007936840e+01,  1.067871883e+01,  1.131370850e+01,  1.198645662e+01,
   1.269920842e+01,  1.345434264e+01,  1.425437949e+01,  1.510198900e+01,
   1.600000000e+01,  1.695140951e+01,  1.795939277e+01,  1.902731384e+01,
   2.015873680e+01,  2.135743767e+01,  2.262741700e+01,  2.397291323e+01,
   2.539841683e+01,  2.690868529e+01,  2.850875898e+01,  3.020397801e+01,
   3.200000000e+01,  3.390281902e+01,  3.591878555e+01,  3.805462768e+01,
   4.031747360e+01,  4.271487533e+01,  4.525483400e+01,  4.794582646e+01,
   5.079683366e+01,  5.381737058e+01,  5.701751796e+01,  6.040795601e+01,
   6.400000000e+01,  6.780563804e+01,  7.183757109e+01,  7.610925536e+01,
   8.063494719e+01,  8.542975067e+01,  9.050966799e+01,  9.589165292e+01,
   1.015936673e+02,  1.076347412e+02,  1.140350359e+02,  1.208159120e+02,
   1.280000000e+02,  1.356112761e+02,  1.436751422e+02,  1.522185107e+02,
   1.612698944e+02,  1.708595013e+02,  1.810193360e+02,  1.917833058e+02,
   2.031873347e+02,  2.152694823e+02,  2.280700718e+02,  2.416318240e+02,
   2.560000000e+02,  2.712225522e+02,  2.873502844e+02,  3.044370214e+02,
   3.225397888e+02,  3.417190027e+02,  3.620386720e+02,  3.835666117e+02,
   4.063746693e+02,  4.305389646e+02,  4.561401437e+02,  4.832636481e+02,
   5.120000000e+02,  5.424451043e+02,  5.747005687e+02,  6.088740429e+02,
   6.450795775e+02,  6.834380053e+02,  7.240773439e+02,  7.671332234e+02,
   8.127493386e+02,  8.610779292e+02,  9.122802874e+02,  9.665272962e+02,
   1.024000000e+03,  1.084890209e+03,  1.149401137e+03,  1.217748086e+03,
   1.290159155e+03,  1.366876011e+03,  1.448154688e+03,  1.534266447e+03,
    };

    /* extern */
    const float lut_pitch_ratio_low[] = {
       1.000000000e+00,  1.000225659e+00,  1.000451370e+00,  1.000677131e+00,
       1.000902943e+00,  1.001128806e+00,  1.001354720e+00,  1.001580685e+00,
       1.001806701e+00,  1.002032768e+00,  1.002258886e+00,  1.002485055e+00,
       1.002711275e+00,  1.002937546e+00,  1.003163868e+00,  1.003390242e+00,
       1.003616666e+00,  1.003843141e+00,  1.004069668e+00,  1.004296246e+00,
       1.004522874e+00,  1.004749554e+00,  1.004976285e+00,  1.005203068e+00,
       1.005429901e+00,  1.005656786e+00,  1.005883722e+00,  1.006110709e+00,
       1.006337747e+00,  1.006564836e+00,  1.006791977e+00,  1.007019169e+00,
       1.007246412e+00,  1.007473707e+00,  1.007701053e+00,  1.007928450e+00,
       1.008155898e+00,  1.008383398e+00,  1.008610949e+00,  1.008838551e+00,
       1.009066205e+00,  1.009293910e+00,  1.009521667e+00,  1.009749475e+00,
       1.009977334e+00,  1.010205245e+00,  1.010433207e+00,  1.010661221e+00,
       1.010889286e+00,  1.011117403e+00,  1.011345571e+00,  1.011573790e+00,
       1.011802061e+00,  1.012030384e+00,  1.012258758e+00,  1.012487183e+00,
       1.012715661e+00,  1.012944189e+00,  1.013172770e+00,  1.013401401e+00,
       1.013630085e+00,  1.013858820e+00,  1.014087607e+00,  1.014316445e+00,
       1.014545335e+00,  1.014774277e+00,  1.015003270e+00,  1.015232315e+00,
       1.015461411e+00,  1.015690560e+00,  1.015919760e+00,  1.016149011e+00,
       1.016378315e+00,  1.016607670e+00,  1.016837077e+00,  1.017066536e+00,
       1.017296046e+00,  1.017525609e+00,  1.017755223e+00,  1.017984889e+00,
       1.018214607e+00,  1.018444376e+00,  1.018674198e+00,  1.018904071e+00,
       1.019133996e+00,  1.019363973e+00,  1.019594002e+00,  1.019824083e+00,
       1.020054216e+00,  1.020284401e+00,  1.020514637e+00,  1.020744926e+00,
       1.020975266e+00,  1.021205659e+00,  1.021436104e+00,  1.021666600e+00,
       1.021897149e+00,  1.022127749e+00,  1.022358402e+00,  1.022589107e+00,
       1.022819863e+00,  1.023050672e+00,  1.023281533e+00,  1.023512446e+00,
       1.023743411e+00,  1.023974428e+00,  1.024205498e+00,  1.024436619e+00,
       1.024667793e+00,  1.024899019e+00,  1.025130297e+00,  1.025361627e+00,
       1.025593009e+00,  1.025824444e+00,  1.026055931e+00,  1.026287470e+00,
       1.026519061e+00,  1.026750705e+00,  1.026982401e+00,  1.027214149e+00,
       1.027445949e+00,  1.027677802e+00,  1.027909707e+00,  1.028141664e+00,
       1.028373674e+00,  1.028605736e+00,  1.028837851e+00,  1.029070017e+00,
       1.029302237e+00,  1.029534508e+00,  1.029766832e+00,  1.029999209e+00,
       1.030231638e+00,  1.030464119e+00,  1.030696653e+00,  1.030929239e+00,
       1.031161878e+00,  1.031394569e+00,  1.031627313e+00,  1.031860109e+00,
       1.032092958e+00,  1.032325859e+00,  1.032558813e+00,  1.032791820e+00,
       1.033024879e+00,  1.033257991e+00,  1.033491155e+00,  1.033724372e+00,
       1.033957641e+00,  1.034190964e+00,  1.034424338e+00,  1.034657766e+00,
       1.034891246e+00,  1.035124779e+00,  1.035358364e+00,  1.035592003e+00,
       1.035825694e+00,  1.036059437e+00,  1.036293234e+00,  1.036527083e+00,
       1.036760985e+00,  1.036994940e+00,  1.037228947e+00,  1.037463008e+00,
       1.037697121e+00,  1.037931287e+00,  1.038165506e+00,  1.038399777e+00,
       1.038634102e+00,  1.038868479e+00,  1.039102910e+00,  1.039337393e+00,
       1.039571929e+00,  1.039806518e+00,  1.040041160e+00,  1.040275855e+00,
       1.040510603e+00,  1.040745404e+00,  1.040980258e+00,  1.041215165e+00,
       1.041450125e+00,  1.041685138e+00,  1.041920204e+00,  1.042155323e+00,
       1.042390495e+00,  1.042625720e+00,  1.042860998e+00,  1.043096329e+00,
       1.043331714e+00,  1.043567151e+00,  1.043802642e+00,  1.044038185e+00,
       1.044273782e+00,  1.044509433e+00,  1.044745136e+00,  1.044980892e+00,
       1.045216702e+00,  1.045452565e+00,  1.045688481e+00,  1.045924450e+00,
       1.046160473e+00,  1.046396549e+00,  1.046632678e+00,  1.046868860e+00,
       1.047105096e+00,  1.047341385e+00,  1.047577727e+00,  1.047814123e+00,
       1.048050572e+00,  1.048287074e+00,  1.048523630e+00,  1.048760239e+00,
       1.048996902e+00,  1.049233618e+00,  1.049470387e+00,  1.049707210e+00,
       1.049944086e+00,  1.050181015e+00,  1.050417999e+00,  1.050655035e+00,
       1.050892125e+00,  1.051129269e+00,  1.051366466e+00,  1.051603717e+00,
       1.051841021e+00,  1.052078378e+00,  1.052315790e+00,  1.052553255e+00,
       1.052790773e+00,  1.053028345e+00,  1.053265971e+00,  1.053503650e+00,
       1.053741383e+00,  1.053979169e+00,  1.054217010e+00,  1.054454903e+00,
       1.054692851e+00,  1.054930852e+00,  1.055168907e+00,  1.055407016e+00,
       1.055645178e+00,  1.055883395e+00,  1.056121664e+00,  1.056359988e+00,
       1.056598366e+00,  1.056836797e+00,  1.057075282e+00,  1.057313821e+00,
       1.057552413e+00,  1.057791060e+00,  1.058029760e+00,  1.058268515e+00,
       1.058507323e+00,  1.058746185e+00,  1.058985101e+00,  1.059224071e+00,
    };

    inline float SemitonesToRatio(float semitones) 
    {
        float pitch = semitones + 128.0f;
        MAKE_INTEGRAL_FRACTIONAL(pitch)

            return lut_pitch_ratio_high[pitch_integral] * \
            lut_pitch_ratio_low[static_cast<int>(pitch_fractional * 256.0f)];
    }

    void Perseus::Part::Init() 
    {
        resonator_input_ = new float[kMaxBlockSize];
        sympathetic_resonator_input_ = new float[kMaxBlockSize];
        noise_burst_buffer_ = new float[kMaxBlockSize];
        out_buffer_ = new float[kMaxBlockSize];
        aux_buffer_ = new float[kMaxBlockSize];

        active_voice_ = 0;

        for (int i = 0; i < kMaxPolyphony; ++i)
            note_[i] = 0.0f;

        bypass_ = false;
        polyphony_ = 1;
        model_ = RESONATOR_MODEL_MODAL;
        dirty_ = true;

        for (int i = 0; i < kMaxPolyphony; ++i) {
            excitation_filter_[i].Init();
            plucker_[i].Init();
            dc_blocker_[i].Init(1.0f - 10.0f / Helpers::CurrentSampleRate);
        }

        //reverb_.Init(reverb_buffer);
        limiter_.Init();

        note_filter_.Init(
            Helpers::CurrentSampleRate / kMaxBlockSize,
            0.001f,  // Lag time with a sharp edge on the V/Oct input or trigger.
            0.010f,  // Lag time after the trigger has been received.
            0.050f,  // Time to transition from reactive to filtered.
            0.004f); // Prevent a sharp edge to partly leak on the previous voice.
    }

    void Perseus::Part::ConfigureResonators() {
        if (!dirty_) {
            return;
        }

        switch (model_) {
        case RESONATOR_MODEL_MODAL:
        {
            int resolution = 64 / polyphony_ - 4;
            for (int i = 0; i < polyphony_; ++i) {
                resonator_[i].Init();
                resonator_[i].set_resolution(resolution);
            }
        }
        break;

        case RESONATOR_MODEL_SYMPATHETIC_STRING:
        case RESONATOR_MODEL_STRING:
        {
            float lfo_frequencies[kNumStrings] = {
              0.5f, 0.4f, 0.35f, 0.23f, 0.211f, 0.2f, 0.171f
            };
            for (int i = 0; i < kNumStrings; ++i) {
                bool has_dispersion = model_ == RESONATOR_MODEL_STRING;
                string_[i].Init(has_dispersion);

                float f_lfo = float(kMaxBlockSize) / float(Helpers::CurrentSampleRate);
                f_lfo *= lfo_frequencies[i];
                lfo_[i].Init<COSINE_OSCILLATOR_APPROXIMATE>(f_lfo);
            }
            for (int i = 0; i < polyphony_; ++i) {
                plucker_[i].Init();
            }
        }
        break;

        default:
            break;
        }

        if (active_voice_ >= polyphony_) {
            active_voice_ = 0;
        }
        dirty_ = false;
    }

    // Chord table by Bryan Noll:
    float chords[Perseus::kMaxPolyphony][11][8] = {
      {
        { -12.0f, -0.01f, 0.0f,  0.01f, 0.02f, 11.98f, 11.99f, 12.0f }, // OCT
        { -12.0f, -5.0f,  0.0f,  6.99f, 7.0f,  11.99f, 12.0f,  19.0f }, // 5
        { -12.0f, -5.0f,  0.0f,  5.0f,  7.0f,  11.99f, 12.0f,  17.0f }, // sus4
        { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 12.0f,  19.0f }, // m 
        { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 10.0f,  19.0f }, // m7
        { -12.0f, -5.0f,  0.0f,  3.0f, 14.0f,   3.01f, 10.0f,  19.0f }, // m9
        { -12.0f, -5.0f,  0.0f,  3.0f,  7.0f,   3.01f, 10.0f,  17.0f }, // m11
        { -12.0f, -5.0f,  0.0f,  2.0f,  7.0f,   9.0f,  16.0f,  19.0f }, // 69
        { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.0f,  14.0f,  19.0f }, // M9
        { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.0f,  10.99f, 19.0f }, // M7
        { -12.0f, -5.0f,  0.0f,  4.0f,  7.0f,  11.99f, 12.0f,  19.0f } // M
      },
      {
        { -12.0f, 0.0f,  0.01f, 12.0f }, // OCT
        { -12.0f, 6.99f, 7.0f,  12.0f }, // 5
        { -12.0f, 5.0f,  7.0f,  12.0f }, // sus4
        { -12.0f, 3.0f, 11.99f, 12.0f }, // m 
        { -12.0f, 3.0f, 10.0f,  12.0f }, // m7
        { -12.0f, 3.0f, 10.0f,  14.0f }, // m9
        { -12.0f, 3.0f, 10.0f,  17.0f }, // m11
        { -12.0f, 2.0f,  9.0f,  16.0f }, // 69
        { -12.0f, 4.0f, 11.0f,  14.0f }, // M9
        { -12.0f, 4.0f,  7.0f,  11.0f }, // M7
        { -12.0f, 4.0f,  7.0f,  12.0f }, // M
      },
      {
        { 0.0f, -12.0f },
        { 0.0f, 2.0f },
        { 0.0f, 3.0f },
        { 0.0f, 4.0f },
        { 0.0f, 5.0f },
        { 0.0f, 7.0f },
        { 0.0f, 9.0f },
        { 0.0f, 10.0f },
        { 0.0f, 11.0f },
        { 0.0f, 12.0f },
        { -12.0f, 12.0f }
      },
      {
        { 0.0f, -12.0f },
        { 0.0f, 2.0f },
        { 0.0f, 3.0f },
        { 0.0f, 4.0f },
        { 0.0f, 5.0f },
        { 0.0f, 7.0f },
        { 0.0f, 9.0f },
        { 0.0f, 10.0f },
        { 0.0f, 11.0f },
        { 0.0f, 12.0f },
        { -12.0f, 12.0f }
      }
    };


    void Perseus::Part::ComputeSympatheticStringsNotes(
        float tonic,
        float note,
        float parameter,
        float* destination,
        size_t num_strings) {
        float notes[9] = {
            tonic,
            note - 12.0f,
            note - 7.01955f,
            note,
            note + 7.01955f,
            note + 12.0f,
            note + 19.01955f,
            note + 24.0f,
            note + 24.0f };
        const float detunings[4] = {
            0.013f,
            0.011f,
            0.007f,
            0.017f
        };

        if (parameter >= 2.0f) {
            // Quantized chords
            int chord_index = parameter - 2.0f;
            const float* chord = chords[polyphony_ - 1][chord_index];
            for (size_t i = 0; i < num_strings; ++i) {
                destination[i] = chord[i] + note;
            }
            return;
        }

        size_t num_detuned_strings = (num_strings - 1) >> 1;
        size_t first_detuned_string = num_strings - num_detuned_strings;

        for (size_t i = 0; i < first_detuned_string; ++i) {
            float note = 3.0f;
            if (i != 0) {
                note = parameter * 7.0f;
                parameter += (1.0f - parameter) * 0.2f;
            }

            MAKE_INTEGRAL_FRACTIONAL(note);
            note_fractional = Squash(note_fractional);

            float a = notes[note_integral];
            float b = notes[note_integral + 1];

            note = a + (b - a) * note_fractional;
            destination[i] = note;
            if (i + first_detuned_string < num_strings) {
                destination[i + first_detuned_string] = destination[i] + detunings[i & 3];
            }
        }
    }

    void Perseus::Part::RenderModalVoice(
        int voice,
        const PerformanceState& performance_state,
        const Patch& patch,
        float frequency,
        float filter_cutoff,
        size_t size) {
        // Internal exciter is a pulse, pre-filter.
        if (performance_state.internal_exciter &&
            voice == active_voice_ &&
            performance_state.strum) {
            resonator_input_[0] += 0.25f * SemitonesToRatio(
                filter_cutoff * filter_cutoff * 24.0f) / filter_cutoff;
        }

        // Process through filter.
        excitation_filter_[voice].Process<FILTER_MODE_LOW_PASS>(
            resonator_input_, resonator_input_, size);

        Resonator& r = resonator_[voice];
        r.set_frequency(frequency);
        r.set_structure(patch.structure);
        r.set_brightness(patch.brightness * patch.brightness);
        r.set_position(patch.position);
        r.set_damping(patch.damping);
        r.Process(resonator_input_, out_buffer_, aux_buffer_, size);
    }


    void Perseus::Part::RenderStringVoice(
        int voice,
        const PerformanceState& performance_state,
        const Patch& patch,
        float frequency,
        float filter_cutoff,
        size_t size) {
        // Compute number of strings and frequency.
        int num_strings = 1;
        float frequencies[kNumStrings];

        if (model_ == RESONATOR_MODEL_SYMPATHETIC_STRING) 
        {
            num_strings = 2 * kMaxPolyphony / polyphony_;
            float parameter = model_ == RESONATOR_MODEL_SYMPATHETIC_STRING
                ? patch.structure
                : 2.0f + performance_state.chord;
            ComputeSympatheticStringsNotes(
                performance_state.tonic + performance_state.fm,
                performance_state.tonic + note_[voice] + performance_state.fm,
                parameter,
                frequencies,
                num_strings);
            
            const float a3 = 440.0f / Helpers::CurrentSampleRate;

            for (int i = 0; i < num_strings; ++i) {
                frequencies[i] = SemitonesToRatio(frequencies[i] - 69.0f) * a3;
            }
        }
        else {
            frequencies[0] = frequency;
        }

        if (voice == active_voice_) {
            const float gain = 1.0f / sqrtf(static_cast<float>(num_strings) * 2.0f);
            for (size_t i = 0; i < size; ++i) {
                resonator_input_[i] *= gain;
            }
        }

        // Process external input.
        excitation_filter_[voice].Process<FILTER_MODE_LOW_PASS>(
            resonator_input_, resonator_input_, size);

        // Add noise burst.
        if (performance_state.internal_exciter) {
            if (voice == active_voice_ && performance_state.strum) {
                plucker_[voice].Trigger(frequency, filter_cutoff * 8.0f, patch.position);
            }
            plucker_[voice].Process(noise_burst_buffer_, size);
            for (size_t i = 0; i < size; ++i) {
                resonator_input_[i] += noise_burst_buffer_[i];
            }
        }
        dc_blocker_[voice].Process(resonator_input_, size);

        for (int i = 0; i < size; ++i)
        {
            out_buffer_[i] = 0.0f;
            aux_buffer_[i] = 0.0f;
        }

        float structure = patch.structure;
        float dispersion = structure < 0.24f
            ? (structure - 0.24f) * 4.166f
            : (structure > 0.26f ? (structure - 0.26f) * 1.35135f : 0.0f);

        for (int string = 0; string < num_strings; ++string) {
            int i = voice + string * polyphony_;
            String& s = string_[i];
            float lfo_value = lfo_[i].Next();

            float brightness = patch.brightness;
            float damping = patch.damping;
            float position = patch.position;
            float glide = 1.0f;
            float string_index = static_cast<float>(string) / static_cast<float>(num_strings);
            const float* input = resonator_input_;

            // When the internal exciter is used, string 0 is the main
            // source, the other strings are vibrating by sympathetic resonance.
            // When the internal exciter is not used, all strings are vibrating
            // by sympathetic resonance.
            if (string > 0 && performance_state.internal_exciter) {
                brightness *= (2.0f - brightness);
                brightness *= (2.0f - brightness);
                damping = 0.7f + patch.damping * 0.27f;
                float amount = (0.5f - fabs(0.5f - patch.position)) * 0.9f;
                position = patch.position + lfo_value * amount;
                glide = SemitonesToRatio((brightness - 1.0f) * 36.0f);
                input = sympathetic_resonator_input_;
            }

            s.set_dispersion(dispersion);
            s.set_frequency(frequencies[string], glide);
            s.set_brightness(brightness);
            s.set_position(position);
            s.set_damping(damping + string_index * (0.95f - damping));
            s.Process(input, out_buffer_, aux_buffer_, size);

            if (string == 0) {
                // Was 0.1f, Ben Wilson -> 0.2f
                float gain = 0.2f / static_cast<float>(num_strings);
                for (size_t i = 0; i < size; ++i) {
                    float sum = out_buffer_[i] - aux_buffer_[i];
                    sympathetic_resonator_input_[i] = gain * sum;
                }
            }
        }
    }

    const int kPingPattern[] = {
      1, 0, 2, 1, 0, 2, 1, 0
    };

    void Perseus::Part::Process(
        const PerformanceState& performance_state,
        const Patch& patch,
        const float* in,
        float* out,
        float* aux,
        size_t size) {

        // Copy inputs to outputs when bypass mode is enabled.
        if (bypass_) {
            for (int i = 0; i < size; ++i)
            {
                out[i] = in[i];
                aux[i] = in[i];
            }
            return;
        }

        ConfigureResonators();

        note_filter_.Process(
            performance_state.note,
            performance_state.strum);

        if (performance_state.strum) {
            note_[active_voice_] = note_filter_.stable_note();
            if (polyphony_ > 1 && polyphony_ & 1) {
                active_voice_ = kPingPattern[step_counter_ % 8];
                step_counter_ = (step_counter_ + 1) % 8;
            }
            else {
                active_voice_ = (active_voice_ + 1) % polyphony_;
            }
        }

        note_[active_voice_] = note_filter_.note();

        for (int i = 0; i < size; ++i)
        {
            out[i] = 0.0f;
            aux[i] = 0.0f;
        }

        const float a3 = 440.0f / Helpers::CurrentSampleRate;

        for (int voice = 0; voice < polyphony_; ++voice) {
            // Compute MIDI note value, frequency, and cutoff frequency for excitation
            // filter.
            float cutoff = patch.brightness * (2.0f - patch.brightness);
            float note = note_[voice] + performance_state.tonic + performance_state.fm;
            float frequency = SemitonesToRatio(note - 69.0f) * a3;
            float filter_cutoff_range = performance_state.internal_exciter
                ? frequency * SemitonesToRatio((cutoff - 0.5f) * 96.0f)
                : 0.4f * SemitonesToRatio((cutoff - 1.0f) * 108.0f);
            float filter_cutoff = voice == active_voice_
                ? filter_cutoff_range
                : (10.0f / Helpers::CurrentSampleRate);
            if (filter_cutoff > 0.499f)
                filter_cutoff = 0.499f;
            float filter_q = performance_state.internal_exciter ? 1.5f : 0.8f;

            // Process input with excitation filter. Inactive voices receive silence.
            excitation_filter_[voice].set_f_q<FREQUENCY_DIRTY>(filter_cutoff, filter_q);
            if (voice == active_voice_) 
            {
                for (int i = 0; i < size; ++i)
                    resonator_input_[i] = in[i];
            }
            else 
            {
                for (int i = 0; i < size; ++i)
                    resonator_input_[i] = 0.0f;
            }

            if (model_ == RESONATOR_MODEL_MODAL) {
                RenderModalVoice(
                    voice, performance_state, patch, frequency, filter_cutoff, size);
            }
            else {
                RenderStringVoice(
                    voice, performance_state, patch, frequency, filter_cutoff, size);
            }

            if (polyphony_ == 1) {
                // Send the two sets of harmonics / pickups to individual outputs.
                for (size_t i = 0; i < size; ++i) {
                    out[i] += out_buffer_[i];
                    aux[i] += aux_buffer_[i];
                }
            }
            else {
                // Dispatch odd/even voices to individual outputs.
                float* destination = voice & 1 ? aux : out;
                for (size_t i = 0; i < size; ++i) {
                    destination[i] += out_buffer_[i] - aux_buffer_[i];
                }
            }
        }

        

        // Apply limiter to string output.
        limiter_.Process(out, aux, size, model_gains_[model_]);
    }

    /* static */
    float Perseus::Part::model_gains_[] = {
      1.4f,  // RESONATOR_MODEL_MODAL
      1.0f,  // RESONATOR_MODEL_SYMPATHETIC_STRING
      1.4f,  // RESONATOR_MODEL_STRING
    };


    void Perseus::Resonator::Init() {
        for (int i = 0; i < kMaxModes; ++i) {
            f_[i].Init();
        }

        set_frequency(220.0f / Helpers::CurrentSampleRate);
        set_structure(0.25f);
        set_brightness(0.5f);
        set_damping(0.3f);
        set_position(0.999f);
        previous_position_ = 0.0f;
        set_resolution(kMaxModes);
    }

    inline float Interpolate(const float* table, float index, float size) {
        index *= size;
        MAKE_INTEGRAL_FRACTIONAL(index)
            float a = table[index_integral];
        float b = table[index_integral + 1];
        return a + (b - a) * index_fractional;
    }

    const float lut_stiffness[] = {
  -6.250000000e-02, -6.152343750e-02, -6.054687500e-02, -5.957031250e-02,
  -5.859375000e-02, -5.761718750e-02, -5.664062500e-02, -5.566406250e-02,
  -5.468750000e-02, -5.371093750e-02, -5.273437500e-02, -5.175781250e-02,
  -5.078125000e-02, -4.980468750e-02, -4.882812500e-02, -4.785156250e-02,
  -4.687500000e-02, -4.589843750e-02, -4.492187500e-02, -4.394531250e-02,
  -4.296875000e-02, -4.199218750e-02, -4.101562500e-02, -4.003906250e-02,
  -3.906250000e-02, -3.808593750e-02, -3.710937500e-02, -3.613281250e-02,
  -3.515625000e-02, -3.417968750e-02, -3.320312500e-02, -3.222656250e-02,
  -3.125000000e-02, -3.027343750e-02, -2.929687500e-02, -2.832031250e-02,
  -2.734375000e-02, -2.636718750e-02, -2.539062500e-02, -2.441406250e-02,
  -2.343750000e-02, -2.246093750e-02, -2.148437500e-02, -2.050781250e-02,
  -1.953125000e-02, -1.855468750e-02, -1.757812500e-02, -1.660156250e-02,
  -1.562500000e-02, -1.464843750e-02, -1.367187500e-02, -1.269531250e-02,
  -1.171875000e-02, -1.074218750e-02, -9.765625000e-03, -8.789062500e-03,
  -7.812500000e-03, -6.835937500e-03, -5.859375000e-03, -4.882812500e-03,
  -3.906250000e-03, -2.929687500e-03, -1.953125000e-03, -9.765625000e-04,
   0.000000000e+00,  0.000000000e+00,  0.000000000e+00,  0.000000000e+00,
   0.000000000e+00,  0.000000000e+00,  0.000000000e+00,  0.000000000e+00,
   0.000000000e+00,  0.000000000e+00,  0.000000000e+00,  0.000000000e+00,
   0.000000000e+00,  6.029410294e-05,  3.672617230e-04,  6.835957809e-04,
   1.009582073e-03,  1.345515115e-03,  1.691698412e-03,  2.048444725e-03,
   2.416076364e-03,  2.794925468e-03,  3.185334315e-03,  3.587655624e-03,
   4.002252878e-03,  4.429500650e-03,  4.869784943e-03,  5.323503537e-03,
   5.791066350e-03,  6.272895808e-03,  6.769427226e-03,  7.281109202e-03,
   7.808404022e-03,  8.351788076e-03,  8.911752293e-03,  9.488802580e-03,
   1.008346028e-02,  1.069626264e-02,  1.132776331e-02,  1.197853283e-02,
   1.264915914e-02,  1.334024813e-02,  1.405242417e-02,  1.478633069e-02,
   1.554263074e-02,  1.632200761e-02,  1.712516545e-02,  1.795282987e-02,
   1.880574864e-02,  1.968469234e-02,  2.059045506e-02,  2.152385512e-02,
   2.248573583e-02,  2.347696619e-02,  2.449844176e-02,  2.555108540e-02,
   2.663584813e-02,  2.775370999e-02,  2.890568094e-02,  3.009280173e-02,
   3.131614488e-02,  3.257681565e-02,  3.387595299e-02,  3.521473064e-02,
   3.659435812e-02,  3.801608189e-02,  3.948118641e-02,  4.099099536e-02,
   4.254687278e-02,  4.415022437e-02,  4.580249868e-02,  4.750518848e-02,
   4.925983210e-02,  5.106801479e-02,  5.293137017e-02,  5.485158172e-02,
   5.683038428e-02,  5.886956562e-02,  6.097096806e-02,  6.313649016e-02,
   6.536808837e-02,  6.766777886e-02,  7.003763933e-02,  7.247981084e-02,
   7.499649981e-02,  7.758997998e-02,  8.026259446e-02,  8.301675786e-02,
   8.585495846e-02,  8.877976048e-02,  9.179380636e-02,  9.489981918e-02,
   9.810060511e-02,  1.013990559e-01,  1.047981517e-01,  1.083009634e-01,
   1.119106556e-01,  1.156304895e-01,  1.194638260e-01,  1.234141283e-01,
   1.274849653e-01,  1.316800149e-01,  1.360030671e-01,  1.404580277e-01,
   1.450489216e-01,  1.497798965e-01,  1.546552266e-01,  1.596793166e-01,
   1.648567056e-01,  1.701920711e-01,  1.756902336e-01,  1.813561603e-01,
   1.871949702e-01,  1.932119385e-01,  1.994125013e-01,  2.058022605e-01,
   2.123869891e-01,  2.191726361e-01,  2.261653322e-01,  2.333713949e-01,
   2.407973346e-01,  2.484498605e-01,  2.563358863e-01,  2.644625367e-01,
   2.728371538e-01,  2.814673039e-01,  2.903607839e-01,  2.995256288e-01,
   3.089701187e-01,  3.187027863e-01,  3.287324247e-01,  3.390680953e-01,
   3.497191360e-01,  3.606951697e-01,  3.720061128e-01,  3.836621843e-01,
   3.956739150e-01,  4.080521572e-01,  4.208080940e-01,  4.339532500e-01,
   4.474995013e-01,  4.614590865e-01,  4.758446177e-01,  4.906690914e-01,
   5.059459012e-01,  5.216888491e-01,  5.379121581e-01,  5.546304856e-01,
   5.718589358e-01,  5.896130741e-01,  6.079089407e-01,  6.267630651e-01,
   6.461924814e-01,  6.662147434e-01,  6.868479405e-01,  7.081107139e-01,
   7.300222738e-01,  7.526024164e-01,  7.758715422e-01,  7.998506739e-01,
   8.245614757e-01,  8.500262730e-01,  8.762680723e-01,  9.033105820e-01,
   9.311782340e-01,  9.598962059e-01,  9.894904431e-01,  1.000000745e+00,
   1.000037649e+00,  1.000262504e+00,  1.000964607e+00,  1.002570034e+00,
   1.005639154e+00,  1.010861180e+00,  1.019043988e+00,  1.031097087e+00,
   1.048005353e+00,  1.070791059e+00,  1.100461817e+00,  1.137942574e+00,
   1.183990632e+00,  1.239094135e+00,  1.303356514e+00,  1.376372085e+00,
   1.457101344e+00,  1.543758274e+00,  1.633725943e+00,  1.723520185e+00,
   1.808823654e+00,  1.884612937e+00,  1.945398753e+00,  2.000000000e+00,
   2.000000000e+00,
    };

    const float lut_4_decades[] = {
   1.000000000e+00,  1.036632928e+00,  1.074607828e+00,  1.113973860e+00,
   1.154781985e+00,  1.197085030e+00,  1.240937761e+00,  1.286396945e+00,
   1.333521432e+00,  1.382372227e+00,  1.433012570e+00,  1.485508017e+00,
   1.539926526e+00,  1.596338544e+00,  1.654817100e+00,  1.715437896e+00,
   1.778279410e+00,  1.843422992e+00,  1.910952975e+00,  1.980956779e+00,
   2.053525026e+00,  2.128751662e+00,  2.206734069e+00,  2.287573200e+00,
   2.371373706e+00,  2.458244069e+00,  2.548296748e+00,  2.641648320e+00,
   2.738419634e+00,  2.838735965e+00,  2.942727176e+00,  3.050527890e+00,
   3.162277660e+00,  3.278121151e+00,  3.398208329e+00,  3.522694651e+00,
   3.651741273e+00,  3.785515249e+00,  3.924189758e+00,  4.067944321e+00,
   4.216965034e+00,  4.371444813e+00,  4.531583638e+00,  4.697588817e+00,
   4.869675252e+00,  5.048065717e+00,  5.232991147e+00,  5.424690937e+00,
   5.623413252e+00,  5.829415347e+00,  6.042963902e+00,  6.264335367e+00,
   6.493816316e+00,  6.731703824e+00,  6.978305849e+00,  7.233941627e+00,
   7.498942093e+00,  7.773650302e+00,  8.058421878e+00,  8.353625470e+00,
   8.659643234e+00,  8.976871324e+00,  9.305720409e+00,  9.646616199e+00,
   1.000000000e+01,  1.036632928e+01,  1.074607828e+01,  1.113973860e+01,
   1.154781985e+01,  1.197085030e+01,  1.240937761e+01,  1.286396945e+01,
   1.333521432e+01,  1.382372227e+01,  1.433012570e+01,  1.485508017e+01,
   1.539926526e+01,  1.596338544e+01,  1.654817100e+01,  1.715437896e+01,
   1.778279410e+01,  1.843422992e+01,  1.910952975e+01,  1.980956779e+01,
   2.053525026e+01,  2.128751662e+01,  2.206734069e+01,  2.287573200e+01,
   2.371373706e+01,  2.458244069e+01,  2.548296748e+01,  2.641648320e+01,
   2.738419634e+01,  2.838735965e+01,  2.942727176e+01,  3.050527890e+01,
   3.162277660e+01,  3.278121151e+01,  3.398208329e+01,  3.522694651e+01,
   3.651741273e+01,  3.785515249e+01,  3.924189758e+01,  4.067944321e+01,
   4.216965034e+01,  4.371444813e+01,  4.531583638e+01,  4.697588817e+01,
   4.869675252e+01,  5.048065717e+01,  5.232991147e+01,  5.424690937e+01,
   5.623413252e+01,  5.829415347e+01,  6.042963902e+01,  6.264335367e+01,
   6.493816316e+01,  6.731703824e+01,  6.978305849e+01,  7.233941627e+01,
   7.498942093e+01,  7.773650302e+01,  8.058421878e+01,  8.353625470e+01,
   8.659643234e+01,  8.976871324e+01,  9.305720409e+01,  9.646616199e+01,
   1.000000000e+02,  1.036632928e+02,  1.074607828e+02,  1.113973860e+02,
   1.154781985e+02,  1.197085030e+02,  1.240937761e+02,  1.286396945e+02,
   1.333521432e+02,  1.382372227e+02,  1.433012570e+02,  1.485508017e+02,
   1.539926526e+02,  1.596338544e+02,  1.654817100e+02,  1.715437896e+02,
   1.778279410e+02,  1.843422992e+02,  1.910952975e+02,  1.980956779e+02,
   2.053525026e+02,  2.128751662e+02,  2.206734069e+02,  2.287573200e+02,
   2.371373706e+02,  2.458244069e+02,  2.548296748e+02,  2.641648320e+02,
   2.738419634e+02,  2.838735965e+02,  2.942727176e+02,  3.050527890e+02,
   3.162277660e+02,  3.278121151e+02,  3.398208329e+02,  3.522694651e+02,
   3.651741273e+02,  3.785515249e+02,  3.924189758e+02,  4.067944321e+02,
   4.216965034e+02,  4.371444813e+02,  4.531583638e+02,  4.697588817e+02,
   4.869675252e+02,  5.048065717e+02,  5.232991147e+02,  5.424690937e+02,
   5.623413252e+02,  5.829415347e+02,  6.042963902e+02,  6.264335367e+02,
   6.493816316e+02,  6.731703824e+02,  6.978305849e+02,  7.233941627e+02,
   7.498942093e+02,  7.773650302e+02,  8.058421878e+02,  8.353625470e+02,
   8.659643234e+02,  8.976871324e+02,  9.305720409e+02,  9.646616199e+02,
   1.000000000e+03,  1.036632928e+03,  1.074607828e+03,  1.113973860e+03,
   1.154781985e+03,  1.197085030e+03,  1.240937761e+03,  1.286396945e+03,
   1.333521432e+03,  1.382372227e+03,  1.433012570e+03,  1.485508017e+03,
   1.539926526e+03,  1.596338544e+03,  1.654817100e+03,  1.715437896e+03,
   1.778279410e+03,  1.843422992e+03,  1.910952975e+03,  1.980956779e+03,
   2.053525026e+03,  2.128751662e+03,  2.206734069e+03,  2.287573200e+03,
   2.371373706e+03,  2.458244069e+03,  2.548296748e+03,  2.641648320e+03,
   2.738419634e+03,  2.838735965e+03,  2.942727176e+03,  3.050527890e+03,
   3.162277660e+03,  3.278121151e+03,  3.398208329e+03,  3.522694651e+03,
   3.651741273e+03,  3.785515249e+03,  3.924189758e+03,  4.067944321e+03,
   4.216965034e+03,  4.371444813e+03,  4.531583638e+03,  4.697588817e+03,
   4.869675252e+03,  5.048065717e+03,  5.232991147e+03,  5.424690937e+03,
   5.623413252e+03,  5.829415347e+03,  6.042963902e+03,  6.264335367e+03,
   6.493816316e+03,  6.731703824e+03,  6.978305849e+03,  7.233941627e+03,
   7.498942093e+03,  7.773650302e+03,  8.058421878e+03,  8.353625470e+03,
   8.659643234e+03,  8.976871324e+03,  9.305720409e+03,  9.646616199e+03,
   1.000000000e+04,
    };

    int Perseus::Resonator::ComputeFilters() {
        float stiffness = Interpolate(lut_stiffness, structure_, 256.0f);
        float harmonic = frequency_;
        float stretch_factor = 1.0f;
        float q = 500.0f * Interpolate(
            lut_4_decades,
            damping_,
            256.0f);
        float brightness_attenuation = 1.0f - structure_;
        // Reduces the range of brightness when structure is very low, to prevent
        // clipping.
        brightness_attenuation *= brightness_attenuation;
        brightness_attenuation *= brightness_attenuation;
        brightness_attenuation *= brightness_attenuation;
        float brightness = brightness_ * (1.0f - 0.2f * brightness_attenuation);
        float q_loss = brightness * (2.0f - brightness) * 0.85f + 0.15f;
        float q_loss_damping_rate = structure_ * (2.0f - structure_) * 0.1f;
        int num_modes = 0;
        int imax = resolution_;
        if (imax > kMaxModes)
            imax = kMaxModes;
        for (int i = 0; i < imax; ++i) {
            float partial_frequency = harmonic * stretch_factor;
            if (partial_frequency >= 0.49f) {
                partial_frequency = 0.49f;
            }
            else {
                num_modes = i + 1;
            }
            f_[i].set_f_q<FREQUENCY_FAST>(
                partial_frequency,
                1.0f + partial_frequency * q);
            stretch_factor += stiffness;
            if (stiffness < 0.0f) {
                // Make sure that the partials do not fold back into negative frequencies.
                stiffness *= 0.93f;
            }
            else {
                // This helps adding a few extra partials in the highest frequencies.
                stiffness *= 0.98f;
            }
            // This prevents the highest partials from decaying too fast.
            q_loss += q_loss_damping_rate * (1.0f - q_loss);
            harmonic += frequency_;
            q *= q_loss;
        }

        return num_modes;
    }

    class ParameterInterpolator {
    public:
        ParameterInterpolator() { }
        ParameterInterpolator(float* state, float new_value, size_t size) {
            Init(state, new_value, size);
        }

        ParameterInterpolator(float* state, float new_value, float step) {
            state_ = state;
            value_ = *state;
            increment_ = (new_value - *state) * step;
        }

        ~ParameterInterpolator() {
            *state_ = value_;
        }

        inline void Init(float* state, float new_value, size_t size) {
            state_ = state;
            value_ = *state;
            increment_ = (new_value - *state) / static_cast<float>(size);
        }

        inline float Next() {
            value_ += increment_;
            return value_;
        }

        inline float subsample(float t) {
            return value_ + increment_ * t;
        }

    private:
        float* state_;
        float value_;
        float increment_;
    };

    void Perseus::Resonator::Process(const float* in, float* out, float* aux, size_t size) {
        int num_modes = ComputeFilters();

        ParameterInterpolator position(&previous_position_, position_, size);
        while (size--) {
            CosineOscillator amplitudes;
            amplitudes.Init<COSINE_OSCILLATOR_APPROXIMATE>(position.Next());

            float input = *in++ * 0.125f;
            float odd = 0.0f;
            float even = 0.0f;
            amplitudes.Start();
            for (int i = 0; i < num_modes;) {
                odd += amplitudes.Next() * f_[i++].Process<FILTER_MODE_BAND_PASS>(input);
                even += amplitudes.Next() * f_[i++].Process<FILTER_MODE_BAND_PASS>(input);
            }
            *out++ = odd;
            *aux++ = even;
        }
    }



    void Perseus::String::Init(bool enable_dispersion) {
        enable_dispersion_ = enable_dispersion;

        string_.Init();
        stretch_.Init();
        fir_damping_filter_.Init();
        iir_damping_filter_.Init();

        set_frequency(220.0f / Helpers::CurrentSampleRate);
        set_dispersion(0.25f);
        set_brightness(0.5f);
        set_damping(0.3f);
        set_position(0.8f);

        delay_ = 1.0f / frequency_;
        clamped_position_ = 0.0f;
        previous_dispersion_ = 0.0f;
        dispersion_noise_ = 0.0f;
        curved_bridge_ = 0.0f;
        previous_damping_compensation_ = 0.0f;

        out_sample_[0] = out_sample_[1] = 0.0f;
        aux_sample_[0] = aux_sample_[1] = 0.0f;

        dc_blocker_.Init(1.0f - 20.0f / Helpers::CurrentSampleRate);
    }

#define CONSTRAIN(var, min, max) \
  if (var < (min)) { \
    var = (min); \
  } else if (var > (max)) { \
    var = (max); \
  }

    const float lut_svf_shift[] = {
       2.500000000e-01,  2.408119579e-01,  2.316544611e-01,  2.225575501e-01,
       2.135502761e-01,  2.046602549e-01,  1.959132760e-01,  1.873329789e-01,
       1.789406032e-01,  1.707548172e-01,  1.627916233e-01,  1.550643347e-01,
       1.475836177e-01,  1.403575876e-01,  1.333919506e-01,  1.266901772e-01,
       1.202537001e-01,  1.140821254e-01,  1.081734480e-01,  1.025242668e-01,
       9.712999179e-02,  9.198504051e-02,  8.708302003e-02,  8.241689360e-02,
       7.797913038e-02,  7.376183852e-02,  6.975688172e-02,  6.595598018e-02,
       6.235079694e-02,  5.893301078e-02,  5.569437701e-02,  5.262677742e-02,
       4.972226058e-02,  4.697307381e-02,  4.437168789e-02,  4.191081545e-02,
       3.958342416e-02,  3.738274529e-02,  3.530227864e-02,  3.333579426e-02,
       3.147733169e-02,  2.972119704e-02,  2.806195849e-02,  2.649444041e-02,
       2.501371653e-02,  2.361510230e-02,  2.229414676e-02,  2.104662398e-02,
       1.986852431e-02,  1.875604550e-02,  1.770558386e-02,  1.671372543e-02,
       1.577723728e-02,  1.489305906e-02,  1.405829467e-02,  1.327020425e-02,
       1.252619642e-02,  1.182382076e-02,  1.116076060e-02,  1.053482614e-02,
       9.943947824e-03,  9.386169992e-03,  8.859644866e-03,  8.362626781e-03,
       7.893466717e-03,  7.450607078e-03,  7.032576744e-03,  6.637986365e-03,
       6.265523903e-03,  5.913950392e-03,  5.582095932e-03,  5.268855886e-03,
       4.973187279e-03,  4.694105394e-03,  4.430680542e-03,  4.182035018e-03,
       3.947340207e-03,  3.725813861e-03,  3.516717519e-03,  3.319354065e-03,
       3.133065427e-03,  2.957230396e-03,  2.791262569e-03,  2.634608406e-03,
       2.486745394e-03,  2.347180309e-03,  2.215447582e-03,  2.091107747e-03,
       1.973745986e-03,  1.862970740e-03,  1.758412418e-03,  1.659722154e-03,
       1.566570656e-03,  1.478647104e-03,  1.395658114e-03,  1.317326764e-03,
       1.243391669e-03,  1.173606108e-03,  1.107737206e-03,  1.045565155e-03,
       9.868824789e-04,  9.314933471e-04,  8.792129165e-04,  8.298667176e-04,
       7.832900713e-04,  7.393275405e-04,  6.978324110e-04,  6.586662024e-04,
       6.216982059e-04,  5.868050482e-04,  5.538702800e-04,  5.227839874e-04,
       4.934424252e-04,  4.657476707e-04,  4.396072968e-04,  4.149340639e-04,
       3.916456285e-04,  3.696642688e-04,  3.489166247e-04,  3.293334538e-04,
       3.108493994e-04,  2.934027734e-04,  2.769353496e-04,  2.613921700e-04,
       2.467213608e-04,  2.328739600e-04,  2.198037532e-04,  2.074671201e-04,
       1.958228884e-04,  1.848321967e-04,  1.744583648e-04,  1.646667709e-04,
       1.554247368e-04,  1.467014179e-04,  1.384677010e-04,  1.306961070e-04,
       1.233606989e-04,  1.164369956e-04,  1.099018897e-04,  1.037335710e-04,
       9.791145345e-05,  9.241610615e-05,  8.722918894e-05,  8.233339098e-05,
       7.771237301e-05,  7.335071282e-05,  6.923385378e-05,  6.534805627e-05,
       6.168035179e-05,  5.821849973e-05,  5.495094649e-05,  5.186678690e-05,
       4.895572788e-05,  4.620805405e-05,  4.361459529e-05,  4.116669618e-05,
       3.885618709e-05,  3.667535690e-05,  3.461692730e-05,  3.267402848e-05,
       3.084017618e-05,  2.910925011e-05,  2.747547345e-05,  2.593339362e-05,
       2.447786409e-05,  2.310402715e-05,  2.180729775e-05,  2.058334818e-05,
       1.942809362e-05,  1.833767851e-05,  1.730846370e-05,  1.633701428e-05,
       1.542008813e-05,  1.455462508e-05,  1.373773675e-05,  1.296669683e-05,
       1.223893206e-05,  1.155201359e-05,  1.090364889e-05,  1.029167410e-05,
       9.714046817e-06,  9.168839263e-06,  8.654231857e-06,  8.168507146e-06,
       7.710044069e-06,  7.277312546e-06,  6.868868378e-06,  6.483348419e-06,
       6.119466033e-06,  5.776006796e-06,  5.451824445e-06,  5.145837051e-06,
       4.857023409e-06,  4.584419632e-06,  4.327115929e-06,  4.084253574e-06,
       3.855022035e-06,  3.638656274e-06,  3.434434189e-06,  3.241674210e-06,
       3.059733017e-06,  2.888003398e-06,  2.725912223e-06,  2.572918525e-06,
       2.428511705e-06,  2.292209816e-06,  2.163557965e-06,  2.042126787e-06,
       1.927511018e-06,  1.819328137e-06,  1.717217095e-06,  1.620837105e-06,
       1.529866508e-06,  1.444001699e-06,  1.362956111e-06,  1.286459263e-06,
       1.214255852e-06,  1.146104908e-06,  1.081778982e-06,  1.021063394e-06,
       9.637555088e-07,  9.096640684e-07,  8.586085474e-07,  8.104185525e-07,
       7.649332542e-07,  7.220008496e-07,  6.814780557e-07,  6.432296314e-07,
       6.071279262e-07,  5.730524541e-07,  5.408894912e-07,  5.105316968e-07,
       4.818777544e-07,  4.548320342e-07,  4.293042737e-07,  4.052092763e-07,
       3.824666271e-07,  3.610004248e-07,  3.407390278e-07,  3.216148157e-07,
       3.035639631e-07,  2.865262270e-07,  2.704447456e-07,  2.552658484e-07,
       2.409388772e-07,  2.274160171e-07,  2.146521368e-07,  2.026046381e-07,
       1.912333136e-07,  1.805002124e-07,  1.703695139e-07,  1.608074078e-07,
       1.517819816e-07,  1.432631135e-07,  1.352223728e-07,  1.276329242e-07,
       1.204694386e-07,
    };

    inline float Crossfade(float a, float b, float fade) {
        return a + (b - a) * fade;
    }

    template<bool enable_dispersion>
    void Perseus::String::ProcessInternal(
        const float* in,
        float* out,
        float* aux,
        size_t size) {
        float delay = 1.0f / frequency_;
        CONSTRAIN(delay, 4.0f, kDelayLineSize - 4.0f);

        // If there is not enough delay time in the delay line, we play at the
        // lowest possible note and we upsample on the fly with a shitty linear
        // interpolator. We don't care because it's a corner case (f0 < 11.7Hz)
        float src_ratio = delay * frequency_;
        if (src_ratio >= 0.9999f) {
            // When we are above 11.7 Hz, we make sure that the linear interpolator
            // does not get in the way.
            src_phase_ = 1.0f;
            src_ratio = 1.0f;
        }

        float clamped_position = 0.5f - 0.98f * fabs(position_ - 0.5f);

        // Linearly interpolate all comb-related CV parameters for each sample.
        ParameterInterpolator delay_modulation(
            &delay_, delay, size);
        ParameterInterpolator position_modulation(
            &clamped_position_, clamped_position, size);
        ParameterInterpolator dispersion_modulation(
            &previous_dispersion_, dispersion_, size);

        // For damping/absorption, the interpolation is done in the filter code.
        float lf_damping = damping_ * (2.0f - damping_);
        float rt60 = 0.07f * SemitonesToRatio(lf_damping * 96.0f) * Helpers::CurrentSampleRate;
        float rt60_base_2_12 = Helpers::Max(-120.0f * delay / src_ratio / rt60, -127.0f);
        float damping_coefficient = SemitonesToRatio(rt60_base_2_12);
        float brightness = brightness_ * brightness_;
        float noise_filter = SemitonesToRatio((brightness_ - 1.0f) * 48.0f);
        float damping_cutoff = Helpers::Min(
            24.0f + damping_ * damping_ * 48.0f + brightness_ * brightness_ * 24.0f,
            84.0f);
        float damping_f = Helpers::Min(frequency_ * SemitonesToRatio(damping_cutoff), 0.499f);

        // Crossfade to infinite decay.
        if (damping_ >= 0.95f) {
            float to_infinite = 20.0f * (damping_ - 0.95f);
            damping_coefficient += to_infinite * (1.0f - damping_coefficient);
            brightness += to_infinite * (1.0f - brightness);
            damping_f += to_infinite * (0.4999f - damping_f);
            damping_cutoff += to_infinite * (128.0f - damping_cutoff);
        }

        fir_damping_filter_.Configure(damping_coefficient, brightness, size);
        iir_damping_filter_.set_f_q<FREQUENCY_ACCURATE>(damping_f, 0.5f);
        ParameterInterpolator damping_compensation_modulation(
            &previous_damping_compensation_,
            1.0f - Interpolate(lut_svf_shift, damping_cutoff, 1.0f),
            size);

        while (size--) {
            src_phase_ += src_ratio;
            if (src_phase_ > 1.0f) {
                src_phase_ -= 1.0f;

                float delay = delay_modulation.Next();
                float comb_delay = delay * position_modulation.Next();

#ifndef MIC_W
                delay *= damping_compensation_modulation.Next();  // IIR delay.
#endif  // MIC_W
                delay -= 1.0f; // FIR delay.

                float s = 0.0f;

                if (enable_dispersion) {
                    float noise = 2.0f * Helpers::RandFloat() - 1.0f;
                    noise *= 1.0f / (0.2f + noise_filter);
                    dispersion_noise_ += noise_filter * (noise - dispersion_noise_);

                    float dispersion = dispersion_modulation.Next();
                    float stretch_point = dispersion <= 0.0f
                        ? 0.0f
                        : dispersion * (2.0f - dispersion) * 0.475f;
                    float noise_amount = dispersion > 0.75f
                        ? 4.0f * (dispersion - 0.75f)
                        : 0.0f;
                    float bridge_curving = dispersion < 0.0f
                        ? -dispersion
                        : 0.0f;

                    noise_amount = noise_amount * noise_amount * 0.025f;
                    float ac_blocking_amount = bridge_curving;

                    bridge_curving = bridge_curving * bridge_curving * 0.01f;
                    float ap_gain = -0.618f * dispersion / (0.15f + fabs(dispersion));

                    float delay_fm = 1.0f;
                    delay_fm += dispersion_noise_ * noise_amount;
                    delay_fm -= curved_bridge_ * bridge_curving;
                    delay *= delay_fm;

                    float ap_delay = delay * stretch_point;
                    float main_delay = delay - ap_delay;
                    if (ap_delay >= 4.0f && main_delay >= 4.0f) {
                        s = string_.ReadHermite(main_delay);
                        s = stretch_.Allpass(s, ap_delay, ap_gain);
                    }
                    else {
                        s = string_.ReadHermite(delay);
                    }
                    float s_ac = s;
                    dc_blocker_.Process(&s_ac, 1);
                    s += ac_blocking_amount * (s_ac - s);

                    float value = fabs(s) - 0.025f;
                    float sign = s > 0.0f ? 1.0f : -1.5f;
                    curved_bridge_ = (fabs(value) + value) * sign;
                }
                else {
                    s = string_.ReadHermite(delay);
                }

                s += *in;  // When f0 < 11.7 Hz, causes ugly bitcrushing on the input!
                s = fir_damping_filter_.Process(s);
#ifndef MIC_W
                s = iir_damping_filter_.Process<FILTER_MODE_LOW_PASS>(s);
#endif  // MIC_W
                string_.Write(s);

                out_sample_[1] = out_sample_[0];
                aux_sample_[1] = aux_sample_[0];

                out_sample_[0] = s;
                aux_sample_[0] = string_.Read(comb_delay);
            }
            *out++ += Crossfade(out_sample_[1], out_sample_[0], src_phase_);
            *aux++ += Crossfade(aux_sample_[1], aux_sample_[0], src_phase_);
            in++;
        }
    }

    void Perseus::String::Process(const float* in, float* out, float* aux, size_t size) {
        if (enable_dispersion_) {
            ProcessInternal<true>(in, out, aux, size);
        }
        else {
            ProcessInternal<false>(in, out, aux, size);
        }
    }
}
