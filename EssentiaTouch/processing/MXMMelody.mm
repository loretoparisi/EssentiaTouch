//
//  MXMMelody.m
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#import "MXMMelody.h"

#include "melody.h"

@implementation MXMMelody

-(id)init {

    if(self == [super init]) {
    
    }
    
    return self;

}


-(void)initEssentia {

    
    melody = new essentia::standard::Melody();
    melody->initEssentia();
    
}

-(void)shutDown {

    melody->shutDownEssentia();
    delete melody;

}

-(void)calculatePredominantMelody:(float*)audio andResult:(float*)output andSamples:(int)numSamples {
    melody->calculatePredominantMelody(audio, output, numSamples);
}

-(void)calculateOnset:(float*)audio andResult:(float*)output andSamples:(int)numSamples {
    melody->calculateOnset(audio, output, numSamples);
}

@end
