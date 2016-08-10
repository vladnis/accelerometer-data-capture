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
	GestureRecognitionPipeline pipeline;
    TimeSeriesClassificationData trainingData;      //This will store our training data

	trainingData.setNumDimensions( 3 );

	DIR *dir;
	struct dirent *ent;
	string dirPath = "/home/vlad/AndroidStudioProjects/DataCapture/fileRetrivalScript/CapturedData/DCIM/continuous";

	if ((dir = opendir (dirPath.c_str())) == NULL) {
		 /* could not open directory */
		 perror ("Could not open data folder");
		 return EXIT_FAILURE;
	}

	/* print all the files and directories within directory */
	while ((ent = readdir (dir)) != NULL) {
		string filename = ent->d_name;

		int position = filename.find("#");
		if (position == std::string::npos) {
			printf("Skipped: %s\n", filename.c_str());
			continue;
		}
		string classLabelString = filename.substr(0, position);

		UINT classLabel = atoi(classLabelString.c_str());

		printf("Processing filename: %s Label: %d\n", filename.c_str(), classLabel);

		string filePath = dirPath + "/" + filename;

		std::ifstream infile(filePath);

		if (infile == NULL) {
			printf("File not found: %s\n", filePath.c_str());
			continue;
		}

		MatrixDouble timeseries;                                //This will store a single training sample

		double x, y, z;
		while (infile >> x >> y >> z) {
			VectorDouble sample(3);
			sample[0] = x;
			sample[1] = y;
			sample[2] = z;

	        timeseries.push_back( sample );
		}

        trainingData.addSample(classLabel, timeseries);

        printf("Number of training steps: %d\n", timeseries.getSize());

        timeseries.clear();
	}
	closedir (dir);

	trainingData.saveDatasetToFile("TrainingData.txt");

	return 0;

}
