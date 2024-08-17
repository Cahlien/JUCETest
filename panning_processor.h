//
// Created by cahlien on 8/16/24.
//

#ifndef JUCETEST_PANNING_PROCESSOR_H
#define JUCETEST_PANNING_PROCESSOR_H

#include <QObject>
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"

class PanningProcessor : public QObject, public juce::AudioProcessor
{
    Q_OBJECT
public:
    explicit PanningProcessor(QObject *parent = nullptr);
    ~PanningProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;
    void releaseResources() override {}

    void setPan(float pan);
    const juce::String getName() const override { return "Panning Processor"; }
    double getTailLengthSeconds() const override { return 0.0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    bool hasEditor() const override { return false; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return "Default"; }
    void changeProgramName(int index, const juce::String &newName) override {}
    juce::AudioProcessorEditor * createEditor() override { return nullptr; }
    void getStateInformation(juce::MemoryBlock &destData) override {}
    void setStateInformation(const void *data, int sizeInBytes) override {}
private:
    juce::AudioParameterFloat* m_panParameter;
    juce::dsp::Panner<float> m_panner;
};


#endif //JUCETEST_PANNING_PROCESSOR_H
