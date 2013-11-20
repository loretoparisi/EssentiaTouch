//
//  melody.cpp
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#include "melody.h"
#include "essentia.h"
#include "algorithmfactory.h"
#include "pool.h"

using namespace essentia;
using namespace standard;

void Melody::initEssentia() {

    // register the algorithms in the factory(ies)
    essentia::init();
    
    
}

void Melody::shutDownEssentia() {

    
    // shut down essentia
    essentia::shutdown();
    
}

void Melody::calculateOnset(float*_audio,float* output, int numSamples) {

    Real onsetRate;
    std::vector<Real> onsets;
    
    std::vector<Real> audio, unused;
    
    for(int i=0; i< numSamples; i++) {
        essentia::Real r = _audio[i];
        audio.push_back( r );
    }
    
    Algorithm* extractoronsetrate = AlgorithmFactory::create("OnsetRate");
    
    extractoronsetrate->input("signal").set(audio);
    extractoronsetrate->output("onsets").set(onsets);
    extractoronsetrate->output("onsetRate").set(onsetRate);
    
    extractoronsetrate->compute();
    
    printf("onsetRate: %f",onsetRate);
    
    for (int i = 0; i<onsets.size(); i++)
    {
        printf("\n%f",onsets[i]);
        output[i]=onsets[i];
    }
    
    delete extractoronsetrate;
    
}

void Melody::calculatePredominantMelody(float* _audio, float* output, int numSamples) {
    
    
    
    /////// PARAMS //////////////
    // don't change these default values as they guarantee that pitch extractor output
    // is correct, no tests were done on other values
    int framesize = 2048;
    int hopsize = 128;
    int sr = 44100;
    
    // instantiate algorithms
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    
    essentia::standard::Algorithm* equalLoudness = factory.create("EqualLoudness");
    
    essentia::standard::Algorithm* predominantMelody = factory.create("PredominantMelody",
                                                                      "frameSize", framesize,
                                                                      "hopSize", hopsize,
                                                                      "sampleRate", sr);
    
    // data storage
    essentia::Pool pool;
    
    std::vector<essentia::Real> audio;
    
    for(int i=0; i< numSamples; i++) {
        essentia::Real r = _audio[i];
        audio.push_back( r );
    }
    
    /// TODO: here we have to fill the real audio vector
    
    equalLoudness->input("signal").set(audio);
    
    std::vector<essentia::Real> audioEq;
    equalLoudness->output("signal").set(audioEq);
    predominantMelody->input("signal").set(audioEq);
    
    std::vector<essentia::Real> pitches;
    std::vector<essentia::Real> pitchConfidences;
    
    predominantMelody->output("pitch").set(pitches);
    predominantMelody->output("pitchConfidence").set(pitchConfidences);
    
    pool.add("tonal.predominant_melody.pitch",pitches);
    pool.add("tonal.predominant_melody.pitch_confidence",pitchConfidences);
    
    predominantMelody->compute();
    
    /*float tm = 0.0;
    for (int i = 0; i<pitches.size(); i++)
    {
        tm = tm + float(hopsize)/float(sr);
        printf("\n%f %f %f",pitches[i], pitchConfidences[i], tm);
        output[i]=pitches[i];
    }*/
    
    const std::vector<essentia::Real>& pitchesR = pool.value<std::vector<essentia::Real> >("tonal.predominant_melody.pitch");
    
    const std::vector<essentia::Real>& confidenceR = pool.value<std::vector<essentia::Real> >("tonal.predominant_melody.pitch_confidence");
    
    printf("number of frames: %lu", pool.value<std::vector<essentia::Real> >("tonal.predominant_melody.pitch").size() );
    
    float tm = 0.0;
    for (int i = 0; i<pitchesR.size(); i++)
    {
        tm = tm + float(hopsize)/float(sr);
        printf("%f %f %f",pitchesR[i], confidenceR[i], tm);
        output[i]=pitchesR[i];
    }
    
    // clean up
    delete predominantMelody;
    delete equalLoudness;
    
    
}