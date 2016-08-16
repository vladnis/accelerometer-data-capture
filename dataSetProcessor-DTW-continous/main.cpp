/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial 
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 GRT Custom Makefile
 
 This example provides a basic makefile template for the GRT.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
#include <fstream>
using namespace std;


class KfoldTimeSeriesData {
protected:
	LabelledTimeSeriesClassificationData inputDataset;
	vector< vector< vector< UINT > > > crossValidationIndexs;
	bool crossValidationSetup;
	UINT 	kFoldValue;
	UINT numDimensions;
	vector< ClassTracker > classTracker;

public:
	KfoldTimeSeriesData(LabelledTimeSeriesClassificationData);
	virtual ~KfoldTimeSeriesData();
	bool spiltDataIntoKFolds(const UINT);
	TimeSeriesClassificationDataStream getTestFoldData(const UINT) const;
	LabelledTimeSeriesClassificationData getTrainingFoldData(const UINT,  const UINT) const;
	UINT getFoldSize();

};



KfoldTimeSeriesData::KfoldTimeSeriesData(GRT::LabelledTimeSeriesClassificationData inputDataset):
	inputDataset(inputDataset){
    crossValidationSetup = false;
    crossValidationIndexs.clear();
    kFoldValue = 0;
    classTracker = inputDataset.getClassTracker();
    numDimensions = inputDataset.getNumDimensions();
    UINT totalNumSamples = inputDataset.getNumSamples();
}

KfoldTimeSeriesData::~KfoldTimeSeriesData() {}

bool KfoldTimeSeriesData::spiltDataIntoKFolds(const GRT::UINT K) {

	kFoldValue = K;

    //K can not be zero
    if( K == 0 ){
        std::cout << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K  > inputDataset.getNumSamples()){
        std::cout << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be 0!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples in a specific class if the stratified sampling option is true
	for(UINT c=0; c < inputDataset.getNumClasses(); c++) {
		if( K > classTracker[c].counter ){
			cout << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be larger than the number of samples in any given class!" << std::endl;
			return false;
		}
	}


    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    vector< UINT > indexs( inputDataset.getNumSamples() );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( inputDataset.getNumSamples() / double(K) );

    //Create the random partion indexs
    Random random;
    UINT randomIndex = 0;

	//Break the data into seperate classes
	vector< vector< UINT > > classData( inputDataset.getNumClasses() );

	//Add the indexs to their respective classes
	for(UINT i = 0; i < inputDataset.getNumSamples(); i++) {
		classData[ inputDataset.getClassLabelIndexValue(
				inputDataset[i].getClassLabel() ) ].push_back( i );
	}

	//Randomize the order of the indexs in each of the class index buffers
	for(UINT c = 0; c < inputDataset.getNumClasses(); c++) {
		UINT numSamples = (UINT)classData[c].size();
		for(UINT x = 0; x < numSamples; x++) {
			//Pick a random index
			randomIndex = random.getRandomNumberInt(0, numSamples);

			//Swap the indexs
			SWAP( classData[c][ x ] , classData[c][ randomIndex ] );
		}
	}


    //Resize the cross validation indexs buffer
    crossValidationIndexs.resize( K );
    for (UINT k = 0; k < K; k++) {
    	crossValidationIndexs[k].resize(inputDataset.getNumClasses());
    }

    //Loop over each of the classes and add the data equally to each of the k folds until there is no data left
    vector< UINT >::iterator iter;
    for(UINT c = 0; c < inputDataset.getNumClasses(); c++){
        iter = classData[ c ].begin();
        UINT k = 0;
        while( iter != classData[c].end() ){
            crossValidationIndexs[ k ][c].push_back( *iter );
            iter++;
            k = ++k % K;
        }
    }

    crossValidationSetup = true;
    return true;

}

LabelledTimeSeriesClassificationData KfoldTimeSeriesData::getTrainingFoldData(const UINT foldIndex, const UINT numSamplesPerClass) const {

    UINT index = 0;
	unsigned int randomNumber;
	unsigned int indexClassLabel;
	unsigned int numSamplesRemaining;
    LabelledTimeSeriesClassificationData trainingData;

    if( !crossValidationSetup ) {
        cout << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << endl;
        return trainingData;
    }

    if( foldIndex >= kFoldValue ) {
    	cout << "Fold index too big" << endl;
    	return trainingData;
    }

    Random random;

    trainingData.setNumDimensions( numDimensions );

    /* Put all K-1 training folds in one data set */
    vector <vector< UINT > >  MergedIndexs(inputDataset.getNumClasses());
    for(UINT k = 0; k < kFoldValue; k++) {
           if( k == foldIndex ) {
           	continue;
           }
           for (UINT classLabel = 0 ; classLabel < crossValidationIndexs[k].size(); classLabel++) {
        	   for (UINT i = 0; i < crossValidationIndexs[k][classLabel].size(); i++) {
        		   MergedIndexs[classLabel].push_back(crossValidationIndexs[k][classLabel][i]);
        	   }
           }
    }

	/* For each class peak randomly "numSamplesPerClass" samples */
	for (unsigned int classLabel = 0; classLabel < inputDataset.getNumClasses() ; classLabel++) {

		for (unsigned int numSamples = 1; numSamples <= numSamplesPerClass; numSamples++) {

			numSamplesRemaining = MergedIndexs[classLabel].size();
			if (numSamplesRemaining == 0) {
				printf("The \"numSamplesPerClass\" variable is bigger that the samples for this class");
				break;
			}
			randomNumber = random.getRandomNumberInt(0, numSamplesRemaining);
			index = MergedIndexs[classLabel][randomNumber];

			/* Remove added sample so that it is not added again */
			MergedIndexs[classLabel].erase(MergedIndexs[classLabel].begin() + randomNumber);

			trainingData.addSample( inputDataset[ index ].getClassLabel(),
					inputDataset[ index ].getData() );
		}
	}


    return trainingData;
}

TimeSeriesClassificationDataStream KfoldTimeSeriesData::getTestFoldData(const UINT foldIndex) const {
    TimeSeriesClassificationDataStream testData;

    if( !crossValidationSetup ) {
        cout << "getTestFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << endl;
        return testData;
    }

    if( foldIndex >= kFoldValue ) {
    	cout << "Fold index too big" << endl;
    	return testData;
    }

    //Add the data to the training
    testData.setNumDimensions( numDimensions );

    UINT index = 0;
    for(UINT classLabel = 0; classLabel < crossValidationIndexs[foldIndex].size(); classLabel++) {
    	for (UINT i = 0; i <  crossValidationIndexs[foldIndex][classLabel].size(); i++) {
			index = crossValidationIndexs[foldIndex][classLabel][i];
			testData.addSample( inputDataset[ index ].getClassLabel(),
					inputDataset[ index ].getData() );
    	}
    }

    return testData;
}

UINT KfoldTimeSeriesData::getFoldSize() {
	if (crossValidationSetup) {
		UINT maxSize = crossValidationIndexs[0].size();
		for (UINT k = 0; k < kFoldValue; k++) {
			if (crossValidationIndexs[k].size() > maxSize) {
				maxSize = crossValidationIndexs[k].size();
			}
		}
		return inputDataset.getNumSamples() - maxSize;
	}
	return 0;
}



int main (int argc, const char * argv[])
{
    TimeSeriesClassificationData trainingData;      //This will store our training data

    GestureRecognitionPipeline pipeline;               //This is a wrapper for our classifier and any pre/post processing modules

	string dirPath = "/home/vlad/AndroidStudioProjects/DataCapture/dataSetGenerator/build";

	if (!trainingData.loadDatasetFromFile(dirPath + "/acc-training-set-segmented.data")) {
		printf("Cannot open training segmented set\n");
		return 0;
	}

	printf("Successfully opened training data set ...\n");

    DTW dtw;
//    LowPassFilter lpf(0.1, 1, 1);
//    pipeline.setPreProcessingModule(lpf);

//    DoubleMovingAverageFilter filter( 1000, 3 );
//    pipeline.setPreProcessingModule(filter);

    //dtw.enableNullRejection( true );

    //Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
	//You can increase this value if you find that your real-time gestures are not being recognized
	//If you are getting too many false positives then you should decrease this value
    //dtw.setNullRejectionCoeff( 5 );
    dtw.enableTrimTrainingData(true, 0.1, 90);
//    dtw.setOffsetTimeseriesUsingFirstSample(true);

    pipeline.setClassifier( dtw );

    UINT KFolds = 5;

    /* Separate input dataset using KFold */
    KfoldTimeSeriesData* kFoldTS  = new KfoldTimeSeriesData(trainingData);
	if( !kFoldTS->spiltDataIntoKFolds(KFolds) ) {
		printf("BaseTGTestModel: Failed to spiltDataIntoKFolds!");
		return 0;
	}

	UINT maxTrainigSetSize = trainingData.getNumSamples() * (KFolds - 1) / (KFolds * trainingData.getNumClasses());


	// KFolds


	ofstream myfile;
	  myfile.open ("example.txt");
	Float acc = 0;
	for (GRT::UINT k = 1 ; k < KFolds; k++) {

		printf("Running tests for: %d fold", k);
		//  maxTrainigSetSize
//		for (UINT trainingSetSize = 1; trainingSetSize <= maxTrainigSetSize; trainingSetSize ++) {

			/* Set up training datasets for current fold */
			TimeSeriesClassificationData trainingDataset = kFoldTS->getTrainingFoldData(k, maxTrainigSetSize);

			/* Set up validation datasets for current fold */
			TimeSeriesClassificationDataStream testDataset = kFoldTS->getTestFoldData(k);

			/* Log test dataset size */
			//printf("Data set size: training %d; testing %d",
//					trainingDataset.getNumSamples(), testDataset.getNumSamples());

			/* Run test for current fold */
			pipeline.train(trainingDataset);
			pipeline.test(testDataset);
			myfile << pipeline.getTestAccuracy() << "\n";
//		}
	}

	  myfile.close();

	printf("Accuracy = %f ; %d\n", acc, maxTrainigSetSize);

}
