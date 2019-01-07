/*

    A tool for converting csv formatted temperatures to an image.
    This tool is useful if you want to use the same palettes,
    as the original software, but otherwise Python may be much easier.

    The Oprtis PIX tool will generate CVS screen shots at a desired period,
    for example every 10seconds. This tool allows you to generate these CSV 
    files to PPM files. 

    After which, the PPM files can be convert to PNG and subsequently, 
    aggregated to make a GIF

    Note: For some reason the output of this is a mirrored image
    of the original image... only use this with the accompanying bash script.
    Haven't had the time to fix this bug, but using ImageMagick seems to be fine.

	
	INPUT: 
		pathname where CSV files are stored
		filename of the current CSV file to convert

		ie: ./image_builder /home/user/data/csv/ abc.csv
	
	OUTPUT:
		a ppm file is saved in the same location as the pathname

    author: Jean Ferreira    
*/


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>

// IR device interfaces
#include "IRDevice.h"

// IR imager interfaces
#include "IRImager.h"

// Image converter
#include "ImageBuilder.h"

using namespace std;
using namespace evo;


//Where the csv files are stored, and where the ppm files will be saved
// string _SAVE_PATH_PREFIX = "/home/jean/Pictures/ir/test/";
// string _LOAD_PATH_PREFIX = "/home/jean/Pictures/ir/test/";


//description of this conversion taken from
// libirimager/IRImager.h
unsigned short convert_temp(float t) {
	return (short)(10*t + 1000);
}



void save_ppm_img(unsigned char* ppm, unsigned int size, string name) {
  ofstream img_file(name, std::ios::out);
  img_file.write((const char*) ppm, size);
  img_file.close();
}


int main(int argc, char** argv) {	
	
	string pathname_in = string(argv[1]);
	string filename_in = string(argv[2]);


	// ostringstream file_path;
	// // file_path << _LOAD_PATH_PREFIX << argv[1];
	// file_path << pathname_in << filename_in;
	// string file_name = file_path.str();

	string file_name = pathname_in + filename_in;
	cout << file_name << endl;
	

	ImageBuilder builder = new ImageBuilder(false);

	unsigned int height = 382;
	unsigned int width = 288;

	/*
		these values are used for distributing the colours
		of each pixel based on palette and temperature.
		Make sure these are correct for the experiment.
	*/
	float MIN_TEMPERATURE = 15;
	float MAX_TEMPERATURE = 47;

	unsigned short* data = new unsigned short[width*height];

	ifstream file(file_name);
    string str; 
    vector<vector<float>> all_tokens;
    //read each line of csv file
    while (getline(file, str)) {
    	float buf;                 
	    stringstream ss(str);      
	    vector<float> tokens; 
	    while (ss >> buf) {
	        tokens.push_back(buf);
	        if (ss.peek() == ';') //CSV delimiter is a semi-colon
            	ss.ignore();
	    }
	    all_tokens.push_back(tokens);
    }	

    int k = 0;
    for (int i = 0; i < all_tokens[0].size(); i++) { //0..287
    	for (int j = 0; j < all_tokens.size(); j++) { //0..381
    		data[k++] = convert_temp(all_tokens[j][i]);
    	}
    }


    //for other palette options, see /usr/include/libirimager/ImageBuilder.h
    int plt = eRainbowHi;
	EnumOptrisColoringPalette palette_type = static_cast<EnumOptrisColoringPalette>(plt);
	
	//where to store ppm file values
	unsigned char* buffer    = new unsigned char[height * width * 3];
		
	//apply palette and generate ppm with csv data
	builder.setManualTemperatureRange(MIN_TEMPERATURE, MAX_TEMPERATURE);
	builder.setPaletteScalingMethod(eManual);
	builder.setPalette(palette_type);
	builder.setData(width, height, data);
	builder.convertTemperatureToPaletteImage(buffer);


	/*
		save the ppm file using the same name as the csv file
		but add the type of palette as well
		For example if csv file is 'abc.csv'
		and the palette used was number 5, then the resulting
		ppm file will be 'abc_5.ppm'
	*/
	string delim = ".";
	ostringstream img_file_name;

	//filename w/o extension
	string clean_file_name = filename_in.substr(0, filename_in.find(delim)); 
	
	img_file_name << pathname_in << clean_file_name << "_" << palette_type << ".ppm";
	
	string img_ts = img_file_name.str(); //name used to save the file as.


	unsigned char* ppm;
	unsigned int size;
	builder.convert2PPM(ppm, &size, buffer, width, height);
	save_ppm_img(ppm, size, img_ts);

    

	return 0;
}

