//
//  melody.cpp
//  EssentiaTouch
//
//  Created by Loreto Parisi on 13/11/13.
//  Copyright (c) 2013 Loreto Parisi. All rights reserved.
//

#include "pch.h"
#include "pool.h"


const char* essentia::streaming::FakeLoader::name = "FakeLoader";
const char* essentia::streaming::FakeLoader::description = DOC("blablabla");

using namespace essentia;
using namespace standard;

Melody::Melody()  {
	essentia::streaming::AlgorithmFactory::Registrar<essentia::streaming::FakeLoader> regFakeLoader;

	streamingPredominantMelody = essentia::streaming::AlgorithmFactory::instance().create("PredominantMelody");

	fakeLoader = (essentia::streaming::FakeLoader *)essentia::streaming::AlgorithmFactory::instance().create("FakeLoader"); // essentia::streaming::FakeLoader::create();

	currentPos = 0;
	totalProcessed = 0;
	audio.resize(framesize * frames_per_calculation);
	AlgorithmFactory &factory = AlgorithmFactory::instance();
	equalLoudness = factory.create("EqualLoudness");

	audioSource.setBufferType(essentia::streaming::BufferUsage::forLargeAudioStream);

	predominantMelody = factory.create("PredominantMelody",
		"frameSize", framesize,
		"hopSize", hopsize,
		"sampleRate", sr);


	streamingPredominantMelody = essentia::streaming::AlgorithmFactory::instance().create("PredominantMelody",
		"frameSize", framesize,
		"hopSize", hopsize,
		"sampleRate", sr);
	essentia::streaming::Algorithm* equalLoudness = essentia::streaming::AlgorithmFactory::instance().create("EqualLoudness");

	fakeLoader->output("audio") >> equalLoudness->input("signal");
	equalLoudness->output("signal") >> streamingPredominantMelody->input("signal");
	streamingPredominantMelody->output("pitch") >> PC(pool, "tonal.predominant_melody.pitch");
	streamingPredominantMelody->output("pitchConfidence") >> PC(pool, "tonal.predominant_melody.pitch_confidence");

}
Melody::~Melody() {
	// clean up
	delete predominantMelody;
	delete equalLoudness;
	fclose(analysis_csv);
	analysis_csv = NULL;
}


void Melody::initEssentia() {

	// register the algorithms in the factory(ies)
	essentia::init();
    
    
}

void Melody::shutDownEssentia() {
	// shut down essentia
    essentia::shutdown();

}

void Melody::calculatePredominantMelodyStreaming(float* output, int outcount) {

	scheduler::Network network(fakeLoader);
	try { 
		network.run();
	}
	catch (essentia::EssentiaException e){
		OutputDebugStringA(".");
	}
	const std::map<std::string, std::vector<Real> > &mappool = pool.getSingleVectorRealPool();

	std::vector<Real> pitches = mappool.at("tonal.predominant_melody.pitch");
	std::vector<Real> pitches_confidence = mappool.at("tonal.predominant_melody.pitch_confidence");
	float tm = 0;

	if (pitches.size() > 0) {
		char buffer[2048];
		sprintf(buffer, "\nCurrent time: %f", tm);
		for (int i = 0; i<pitches.size(); i++)
		{
			tm = tm + float(hopsize) / float(sr);
#ifdef _WIN32
			//			char buffer[2048];
			//			sprintf(buffer, "\nMelody: %f %f %f", tm, pitches[i], pitchConfidences[i]);
			//			OutputDebugStringA(buffer);
			if (analysis_csv != NULL)
			{
				fprintf(analysis_csv, "%f,%f\n", tm, pitches[i]);
			}
#else
			printf(buffer, "\nMelody: %f %f %f", pitches[i], pitchConfidences[i], tm);
#endif
		}
	}

	OutputDebugStringA(".");
	
	//std::vector<essentia::Real> pitches;
	//std::vector<essentia::Real> pitchConfidences;
	//streamingPredominantMelody->output("pitch").set(pitches);
	//streamingPredominantMelody->output("pitchConfidence").set(pitchConfidences);
}

/*

void Melody::calculatePredominantMelody(float* output, int outcount) {    
    // data storage
    //essentia::Pool pool;

    /// TODO: here we have to fill the real audio vector
    //equalLoudness->input("signal").set(audio);
    
    //std::vector<essentia::Real> audioEq;
    //equalLoudness->output("signal").set(audioEq);


	predominantMelody->input("signal").set(audio);
    
    std::vector<essentia::Real> pitches;
    std::vector<essentia::Real> pitchConfidences;
    
    predominantMelody->output("pitch").set(pitches);
    predominantMelody->output("pitchConfidence").set(pitchConfidences);
    
    //pool.add("tonal.predominant_melody.pitch",pitches);
    //pool.add("tonal.predominant_melody.pitch_confidence",pitchConfidences);
    
    predominantMelody->compute();
    
	float tm = totalProcessed / float(sr) - framesize * frames_per_calculation / float(sr);
	if (pitches.size() > 0) {
		char buffer[2048];
		sprintf(buffer, "\nCurrent time: %f", tm);
		for (int i = 0; i<pitches.size(); i++)
		{
			tm = tm + float(hopsize) / float(sr);
#ifdef _WIN32
//			char buffer[2048];
//			sprintf(buffer, "\nMelody: %f %f %f", tm, pitches[i], pitchConfidences[i]);
//			OutputDebugStringA(buffer);
			if ( analysis_csv !=NULL)
			{
				fprintf(analysis_csv, "%f,%f\n", tm, pitches[i]);
			}
#else
			printf(buffer, "\nMelody: %f %f %f", pitches[i], pitchConfidences[i], tm);
#endif
			//output[i]=pitches[i];
		}
	}
	else
	{
		OutputDebugStringA("\nWhy no pitches?");
	}
	predominantMelody->reset();
    
    
    
}
*/

#ifdef _WIN32

void Melody::setDebugFileName(const wchar_t *wlocaldata_path) {
	char localdata_path[1024];
	size_t res;
	wcstombs_s(&res, localdata_path, wlocaldata_path, 1024);
	setDebugFileName(localdata_path);
}
#endif

void Melody::setDebugFileName(const char *_filename) {
	if (analysis_csv != NULL)
	{
		fclose(analysis_csv);
	}
	fopen_s(&analysis_csv, _filename, "wb");
}