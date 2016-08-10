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

int main (int argc, const char * argv[])
{
    TimeSeriesClassificationData trainingData;      //This will store our training data

    GestureRecognitionPipeline pipeline;               //This is a wrapper for our classifier and any pre/post processing modules

	string dirPath = "/home/vlad/AndroidStudioProjects/DataCapture/dataSetGenerator/build";


	if (!trainingData.loadDatasetFromFile(dirPath + "/acc-training-set-segmented.data")) {
		printf("Cannot open training set\n");
		return 0;
	}

	printf("Successfully opened training data set ...\n");

    DTW dtw;
//    LowPassFilter lpf(0.1, 1, 3);
//    pipeline.setPreProcessingModule(lpf);

//    DoubleMovingAverageFilter filter( 1000, 3 );
//    pipeline.setPreProcessingModule(filter);

//    dtw.enableNullRejection( true );

    //Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
	//You can increase this value if you find that your real-time gestures are not being recognized
	//If you are getting too many false positives then you should decrease this value
//    dtw.setNullRejectionCoeff( 20 );
//    dtw.enableTrimTrainingData(true, 0.1, 90);
//    dtw.setOffsetTimeseriesUsingFirstSample(true);

    dtw.enableTrimTrainingData(true, 0.1, 90);

    pipeline.setClassifier( dtw );

    pipeline.train(trainingData, 2);

    //You can then get then get the accuracy of how well the pipeline performed during the k-fold cross validation testing
    double accuracy = pipeline.getCrossValidationAccuracy();

    printf("Accuracy: %f", accuracy);

}
