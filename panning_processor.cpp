//
// Created by cahlien on 8/16/24.
//

#include "panning_processor.h"
#include <QDebug>

PanningProcessor::PanningProcessor(QObject *parent)
    : QObject{parent}
    , juce::AudioProcessor()
    , m_panParameter{nullptr}
    , m_panner{}
{
    m_panParameter = new juce::AudioParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f);
    addParameter(m_panParameter);
}

PanningProcessor::~PanningProcessor() = default;

void PanningProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    qInfo() << "PanningProcessor::prepareToPlay()";
    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    m_panner.prepare(spec);
}

void PanningProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    qInfo() << "PanningProcessor::processBlock() with " << buffer.getNumChannels() << " channels.";
        juce::ScopedNoDenormals noDenormals;
        juce::dsp::AudioBlock<float> audioBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);

        // Apply the panning effect
        m_panner.process(context);
}

void PanningProcessor::setPan(float pan)
{
    qInfo() << "PanningProcessor::setPan()";
    m_panner.setPan(pan);
}