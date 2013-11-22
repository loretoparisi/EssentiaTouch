//
//  melody.h
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#ifndef __EssentiaTouch__melody__
#define __EssentiaTouch__melody__

#ifndef _WIN32
#import "essentia.h"
#else
#include "essentia.h"
#include "algorithmfactory.h"
#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"
#endif


#include <iostream>
#include <vector>

#endif /* defined(__EssentiaTouch__melody__) */

namespace essentia {
	namespace streaming {
		class FakeLoader : public Algorithm {
			
			public:
				essentia::streaming::Source<Real> _audio;
				AbsoluteSource<Real> _sampleRate ;
				AbsoluteSource<int> _channels ;

				FakeLoader() {
					_channels.push(1);
					_sampleRate.push((Real)44100);

					declareOutput(_audio, 1, "audio", "the input audio signal");
					//declareOutput(_sampleRate, 0, "sampleRate", "the sampling rate of the audio signal [Hz]");
					//declareOutput(_channels, 0, "numberChannels", "the number of channels");

					//declareOutput(_sampleRate, 0, "sampleRate", "the sampling rate of the audio signal [Hz]");
					//declareOutput(_channels, 0, "numberChannels", "the number of channels");
					//_audio.setBufferType(BufferUsage::forLargeAudioStream);
				}
				int ArrayPos = 0;
				AlgorithmStatus process() {
					OutputDebugStringA(".");
					if (ArrayPos < ArrayOfArray.size() ) {
						try {
							if (ArrayPos %100 ==0)
								OutputDebugStringA("\n");
							int count = ArrayOfArray[ArrayPos].size();
							_audio.acquire(count);
							std::vector<Real>& in_audio = *((std::vector<Real> *)_audio.getTokens());
							for (int i = 0; i < count; i++)
								in_audio[i] = ArrayOfArray[ArrayPos][i];
							_audio.release(count);
						}
						catch (EssentiaException ex)
						{
							OutputDebugStringA("\n.");
							return FINISHED;
						}
						ArrayPos++;
						return OK;
					}
					else {
						shouldStop(true);
						return FINISHED;
					}
				}
				void declareParameters() {
					//	declareParameter("filename", "the name of the file from which to read", "", Parameter::STRING);
				}

				static const char* name;
				static const char* description;
				static Algorithm *create() { return new FakeLoader(); }
				std::vector< std::vector<Real> > ArrayOfArray;
				inline void Add(float *sample, int channels, int count){
					//don't waste buffer then we do FFT calculation
					int fft_step = 2048;
					int this_step_size;
					for (int offset = 0; offset < count; offset += fft_step)
					{
						this_step_size = std::min(fft_step, count - offset);
						std::vector<Real> *in_audio = new std::vector<Real>(this_step_size);
						for (int i = offset; i < offset + this_step_size; i++)
							(*in_audio)[i%this_step_size] = sample[i*channels];
						ArrayOfArray.push_back(*in_audio);
					}
					//_audio.acquire(count);
					//std::vector<Real>& in_audio = *((std::vector<Real> *)_audio.getTokens());
					//for (int i = 0; i < count; i++)
					//	in_audio[i] = sample[i*channels];
					//_audio.release();
				}

		};
	}
}


namespace essentia {
namespace standard {
	
    class Melody   {
		
		Algorithm* equalLoudness;
		Algorithm* predominantMelody;

		essentia::streaming::Algorithm *streamingPredominantMelody ;

		essentia::streaming::FakeLoader *fakeLoader ;

		/////// PARAMS //////////////
		// don't change these default values as they guarantee that pitch extractor output
		// is correct, no tests were done on other values
		static const int framesize = 2048;
		static const int hopsize = 128;
		static const int sr = 44100;
		
		static const int frames_per_calculation = 480;

		std::vector<essentia::Real> audio;

		essentia::streaming::Source<Real> audioSource;
		// data storage
		Pool pool;


		int currentPos;
		int totalProcessed;

		FILE *analysis_csv = NULL;
    public:
		Melody();
		~Melody();

		static void initEssentia();
		static void shutDownEssentia();
		void calculatePredominantMelodyStreaming(float* output, int outcount);
		//void calculatePredominantMelody(float* output, int _count2);
		inline bool Add(float sample){
			//don't waste buffer then we do FFT calculation
			//audio[currentPos] = sample;
			//fakeLoader->Add(sample);
			currentPos++; totalProcessed++;
			if (currentPos >= frames_per_calculation * framesize){
				currentPos = 0;
				return true;
			}
			return false;
		}

		inline void Add(float *sample, int channels, int count){
			fakeLoader->Add(sample, channels, count);
		}

#ifdef _WIN32
		void setDebugFileName(const wchar_t *);
#endif

		void setDebugFileName(const char *);

};

} // namespace streaming
} // namespace essentia