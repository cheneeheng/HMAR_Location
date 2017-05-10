/*
 * DataParser.h
 *
 *  Created on: Apr 18, 2017
 *      Author: chen
 */

#ifndef DATAPARSER_H_
#define DATAPARSER_H_

#include "dataDeclaration.h"

class DataParser
{
	public:
		DataParser();
		virtual ~DataParser();

		void ClearParser();

		int ParseDataNoLabel();
		int ParseData();

		void SetDataParser(vector<vector<string> > data_);

		vector<int>		GetFrameParser() 	{return frames_parser;}
		vector<int>		GetContactParser()	{return contact_parser;}
		Vector4d 		GetFaceParser() 	{return face_parser;}
		vector<string> 	GetLabelParser() 	{return labels_parser;}
		vector<Vector4d>	GetPointParser() 	{return points_parser;}

	protected:
		vector<int>				frames_parser;
		vector<int>				contact_parser;
		vector<Vector4d>		points_parser;
		Vector4d 				face_parser;
		vector<string> 			labels_parser;

	private:
		vector<vector<string> > data_parser;

};

#endif /* DATAPARSER_H_ */
