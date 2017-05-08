/*
 * Parser.cpp
 *
 *  Created on: Apr 18, 2017
 *      Author: chen
 */

#include "DataParser.h"

DataParser::DataParser()
{
	data_parser.clear();
	frames_parser.clear();
	contact_parser.clear();
	points_parser.clear();
	labels_parser.clear();
	face_parser.x=face_parser.y=face_parser.z=0;
}

DataParser::~DataParser() { }

void DataParser::ClearParser()
{
	data_parser.clear();
	frames_parser.clear();
	contact_parser.clear();
	points_parser.clear();
	labels_parser.clear();
	face_parser.x=face_parser.y=face_parser.z=0;
}

void DataParser::SetDataParser(vector<vector<string> > data_)
{
	this->ClearParser();
	data_parser = data_;
	frames_parser .resize(data_parser.size());
	contact_parser.resize(data_parser.size());
	points_parser .resize(data_parser.size());
	labels_parser .resize(data_parser.size());
}

int DataParser::ParseDataNoLabel()
{
	for(int i=0;i<data_parser.size();i++)
	{
		if (data_parser[i].size()<8) return EXIT_FAILURE;
		frames_parser [i]	 = atoi(data_parser[i][0].c_str());
		contact_parser[i]   = atoi(data_parser[i][1].c_str());
		points_parser [i].x = atof(data_parser[i][2].c_str());
		points_parser [i].y = atof(data_parser[i][3].c_str());
		points_parser [i].z = atof(data_parser[i][4].c_str());
		points_parser [i].l = UNCLASSIFIED;
		if(i==200)
		{
			face_parser.x = atof(data_parser[i][5].c_str());
			face_parser.y = atof(data_parser[i][6].c_str());
			face_parser.z = atof(data_parser[i][7].c_str());
			face_parser.l = UNCLASSIFIED;
		}
	}
	return EXIT_SUCCESS;
}

int DataParser::ParseData()
{
	for(int i=0;i<data_parser.size();i++)
	{
		if (data_parser[i].size()<9) return EXIT_FAILURE;
		frames_parser [i]	 = atoi(data_parser[i][0].c_str());
		contact_parser[i]   = atoi(data_parser[i][1].c_str());
		points_parser [i].x = atof(data_parser[i][2].c_str());
		points_parser [i].y = atof(data_parser[i][3].c_str());
		points_parser [i].z = atof(data_parser[i][4].c_str());
		points_parser [i].l = UNCLASSIFIED;
		if(i==200)
		{
			face_parser.x = atof(data_parser[i][5].c_str());
			face_parser.y = atof(data_parser[i][6].c_str());
			face_parser.z = atof(data_parser[i][7].c_str());
			face_parser.l = UNCLASSIFIED;
		}
		labels_parser [i] = string(data_parser[i][8]);
	}
	return EXIT_SUCCESS;
}