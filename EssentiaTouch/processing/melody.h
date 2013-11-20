//
//  melody.h
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#ifndef __EssentiaTouch__melody__
#define __EssentiaTouch__melody__

#include <iostream>
#include <vector>
#import "essentia.h"

#endif /* defined(__EssentiaTouch__melody__) */

namespace essentia {
namespace standard {

    class Melody  {

    public:Melody(){
    
    }
    
    void initEssentia();
    void shutDownEssentia();
    void calculatePredominantMelody(float*audio,float* output, int numSamples);
    void calculateOnset(float*audio,float* output, int numSamples);
    
};

} // namespace streaming
} // namespace essentia