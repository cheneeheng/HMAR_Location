/*
 * labeling.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: chen
 */

#include "labeling_loc.h"

//#define DELETE

int decideBoundaryExt(
	point_d 		&point1_,
	point_d 		&point2_,
	vector<point_d> centroids_)
{
	return decideBoundary(point1_, point2_, centroids_);
}

int contactBoundary(
	vector<point_d> &points_,
	vector<point_d> &centroids_,
	bool 			learn_)
{
	for (int i=0;i<points_.size();i++)
	{
		if (learn_)
		{
			if (points_[i].l < 0) { continue; }
			centroids_[points_[i].l].l =
					min(
							pdfExp(
									BOUNDARY_VAR,
									0.0,
									l2Norm(
											minusPoint(
													points_[i],
													centroids_[points_[i].l]))),
							centroids_[points_[i].l].l);
			centroids_[points_[i].l].l =
					max(
							0.60,
							centroids_[points_[i].l].l);
		}
		else
		{
			if (i > 0)
			{
				decideBoundaryExt(points_[i-1], points_[i], centroids_);
			}
			else
			{
				decideBoundaryExt(points_[i],   points_[i], centroids_);
			}
		}
	}
	return EXIT_SUCCESS;
}

int contactCheck(
	vector<point_d> &points_,
	vector<int> 	contact_,
	int 			num_locations_)
{
	vector<int> counter1; counter1.resize(num_locations_);
	vector<int> counter2; counter2.resize(num_locations_);
	for(int i=0;i<points_.size();i++)
	{
		if (points_[i].l<0) { continue; }
		if (contact_[i]==1) { counter1[points_[i].l]+=1; }
		counter2[points_[i].l]+=1;
	}
	for(int i=0;i<points_.size();i++)
	{
		if (points_[i].l<0) { continue; }
		if (counter1[points_[i].l]!=counter2[points_[i].l])
		{ continue; }
		points_[i].l = UNCLASSIFIED;
	}
	return EXIT_SUCCESS;
}

int clusteringExt(
	vector<point_d> &points_,
	vector<int> 	contact_,
	vector<point_d> &locations_,
	vector<string> 	&labels_)
{
	vector<vector<unsigned char> > color_code; colorCode(color_code);
	vector<int> loc_idx_zero;

	clustering(points_);

#ifdef DELETE
	while(1)
	{
		combineNearCluster(points_, locations_, contact_);
		int num_locations = locations_.size();
		reshapeVector(label_, 		num_locations);
		reshapeVector(loc_idx_zero,	num_locations);
		showData(points_, label_, loc_idx_zero, color_code, true, false, true);
		for(int i=0;i<points_.size();i++)
			if (loc_idx_zero[points_[i].l]<0)
				points_[i].l = UNCLASSIFIED;
		// removing the missing cluster labels
		int c = 0;
		for(int i=0;i<num_locations;i++)
		{
			if(loc_idx_zero[i]>=0)
			{
				loc_idx_zero[i] = c;
				c++;
			}
		}
		// updating cluster label
		for(int i=0;i<num_points;i++)
		{
			if (points_[i].l >= 0)
				points_[i].l = loc_idx_zero[points_[i].l];
			//printf("Location %02d: %02d\n", i, points_[i].l );
		}
		cout << ">>>>> Are the labels correct? [Y/N]\n";
		string mystr; getline (cin, mystr);
		if(!strcmp(mystr.c_str(),"Y"))
		{
			combineNearCluster(points_, locations_, contact_);
			num_locations = locations_.size();
			reshapeVector(label_, 		num_locations);
			reshapeVector(loc_idx_zero,	num_locations);
			break;
		}
	}
#else

	combineNearCluster(points_, locations_, contact_);
	int num_locations = locations_.size();

#endif

	//contactCheck(points_, contact_, num_locations);
	printer(14);

	contactBoundary(points_, locations_, true);
	contactBoundary(points_, locations_, false);

	return EXIT_SUCCESS;
}

int buildLocationArea(
	Graph 						&Graph_,
	vector<vector<point_d> > 	&pos_vel_acc_,
	vector<int> 				contact_)
{
	// [VARIABLES]*************************************************************
	bool 			flag = false;
	vector<point_d> points_avg;
	vector<string>  goal_action;
	vector<point_d> locations;
	vector<int> 	loc_idx_zero;
	vector<vector<unsigned char> > color_code; colorCode(color_code);
	// *************************************************************[VARIABLES]

	// [LABELLING LOCATION]****************************************************
	for(int i=0;i<Graph_.getNumberOfNodes();i++)
	{
		goal_action.push_back(Graph_.getNode(i).name);
	}
	for(int i=0;i<pos_vel_acc_.size();i++)
	{
		points_avg.push_back(pos_vel_acc_[i][0]);
	}
	clusteringExt(points_avg, contact_, locations, goal_action);
	if (Graph_.getNumberOfNodes()==0)
	{
		reshapeVector(goal_action, locations.size());
		vector<string> action_label_tmp;
		Graph_.getActionLabel(action_label_tmp);
		printer(15);
		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, true, false);
	}
	for(int i=0;i<pos_vel_acc_.size();i++)
	{
		pos_vel_acc_[i][0].l = points_avg[i].l;
	}
	printer(16);
	// ****************************************************[LABELLING LOCATION]

	// [NODES]*****************************************************************
	if (Graph_.getNumberOfNodes()>0)
	{
		for(int i=0;i<locations.size();i++)
		{
			int key = -1;
			double mem = 10.0;
			for(int ii=0;ii<Graph_.getNumberOfNodes();ii++)
			{
				if (min_(l2Norm(minusPoint(Graph_.getNode(ii).centroid, locations[i])), mem) &&
					l2Norm(minusPoint(Graph_.getNode(ii).centroid, locations[i])) < CLUSTER_LIMIT)
				{
					mem = l2Norm(minusPoint(Graph_.getNode(ii).centroid, locations[i]));
					key = ii;
				}
			}
			if (key<0)
			{
				goal_action.resize(goal_action.size()+1);
				flag = true;
				node_tt node_tmp;
				node_tmp.name 				= "";
				node_tmp.index 				= Graph_.getNodeList().size();
				node_tmp.centroid 			= locations[i];
				Graph_.setNode(node_tmp);
				Graph_.addEmptyEdgeForNewNode(node_tmp.index);
				Graph_.expandFilter(node_tmp.index+1);
				Graph_.updateFilter(node_tmp.index);
			}
			else
			{
				double tmp = locations[key].l;
				locations[key] =
						multiPoint(
								addPoint(
										Graph_.getNode(key).centroid,
										locations[key]),
								0.5);
				locations[key].l = tmp;
				contactBoundary(points_avg, locations, true);
				node_tt node_tmp  = Graph_.getNode(key);
				node_tmp.centroid = locations[key];
				Graph_.setNode(node_tmp);
			}
		}
	}
	else
	{
		for(int i=0;i<locations.size();i++)
		{
			node_tt node_tmp;
			node_tmp.name 		= goal_action[i];
			node_tmp.index 		= i;
			node_tmp.centroid 	= locations[i];
			Graph_.setNode(node_tmp);
			Graph_.addEmptyEdgeForNewNode(node_tmp.index);
			Graph_.expandFilter(node_tmp.index+1);
			Graph_.updateFilter(node_tmp.index);
		}
	}
	printer(17);
	// *****************************************************************[NODES]

//	if (flag)
//	{
//		vector<string> action_label_tmp;
//		Graph_.getActionLabel(action_label_tmp);
//		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, true, false);
//	}
//	else
//	{
//		vector<string> action_label_tmp;
//		Graph_.getActionLabel(action_label_tmp);
//		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, false, false);
//	}

	return EXIT_SUCCESS;
}