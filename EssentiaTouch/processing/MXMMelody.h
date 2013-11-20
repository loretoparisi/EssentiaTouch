//
//  MXMMelody.h
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#import <UIKit/UIKit.h>

namespace essentia {
    namespace standard {
        class Melody;
    }
}

@interface MXMMelody : NSObject {

    essentia::standard::Melody *melody;

}

-(void)initEssentia;
-(void)shutDown;
-(void)calculatePredominantMelody:(float*)audio andResult:(float*)output andSamples:(int)numSamples;
-(void)calculateOnset:(float*)audio andResult:(float*)output andSamples:(int)numSamples;

@end
