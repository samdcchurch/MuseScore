/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MUSE_VST_VSTAUDIOCLIENT_H
#define MUSE_VST_VSTAUDIOCLIENT_H

#include "audio/audiotypes.h"

#include "vstplugin.h"
#include "vsttypes.h"

namespace muse::vst {
class VstAudioClient
{
public:
    VstAudioClient() = default;
    ~VstAudioClient();

    void init(muse::audio::AudioPluginType type, VstPluginPtr plugin, muse::audio::audioch_t audioChannelsCount = 2);

    bool handleEvent(const VstEvent& event, const audio::samples_t sampleOffset);
    bool handleParamChange(const ParamChangeEvent& param, const audio::samples_t sampleOffset);
    void setVolumeGain(const muse::audio::gain_t newVolumeGain);

    muse::audio::samples_t process(float* output, muse::audio::samples_t samplesPerChannel);

    void flush();

    void setMaxSamplesPerBlock(unsigned int samples);
    void setSampleRate(unsigned int sampleRate);

    ParamsMapping paramsMapping(const std::set<Steinberg::Vst::CtrlNumber>& controllers) const;

private:
    struct SamplesInfo {
        unsigned int sampleRate = 0;
        unsigned int maxSamplesPerBlock = 0;

        bool isValid()
        {
            return sampleRate > 0 && maxSamplesPerBlock > 0;
        }
    };

    IAudioProcessorPtr pluginProcessor() const;
    PluginComponentPtr pluginComponent() const;

    void setUpProcessData();
    void updateProcessSetup();
    void extractInputSamples(muse::audio::samples_t sampleCount, const float* sourceBuffer);

    bool fillOutputBufferInstrument(muse::audio::samples_t sampleCount, float* output);
    bool fillOutputBufferFx(muse::audio::samples_t sampleCount, float* output);

    void ensureActivity();
    void disableActivity();

    void flushBuffers();

    void loadAllNotesOffParam();
    void addParamChange(const ParamChangeEvent& param, const audio::samples_t sampleOffset);

    bool m_isActive = false;
    muse::audio::gain_t m_volumeGain = 1.f; // 0.0 - 1.0

    VstPluginPtr m_pluginPtr = nullptr;
    mutable PluginComponentPtr m_pluginComponent = nullptr;

    SamplesInfo m_samplesInfo;

    std::vector<int> m_activeOutputBusses;
    std::vector<int> m_activeInputBusses;

    VstEventList m_eventList;
    VstParameterChanges m_paramChanges;
    VstProcessData m_processData;
    VstProcessContext m_processContext;

    bool m_needUnprepareProcessData = false;

    muse::audio::AudioPluginType m_type = muse::audio::AudioPluginType::Undefined;
    muse::audio::audioch_t m_audioChannelsCount = 0;

    std::optional<ParamChangeEvent> m_allNotesOffParam;
};
}

#endif // MUSE_VST_VSTAUDIOCLIENT_H
