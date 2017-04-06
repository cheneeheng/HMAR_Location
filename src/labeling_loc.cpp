/*
 * labeling.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: chen
 */

#include "labeling_loc.h"

#define DELETE

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
			if (points_[i].l < 0.0) { continue; }
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
							0.50,
							centroids_[points_[i].l].l);
			centroids_[points_[i].l].l =
					min(
							0.98,
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
	vector<string> 	&labels_,
	vector<string> 	labels_ref_,
	bool 			delete_)
{
	vector<vector<unsigned char> > color_code; colorCode(color_code);
	vector<int> loc_idx_zero;

	clustering(points_, DBSCAN_EPS, DBSCAN_MIN);

	if(delete_)
	{
		combineNearCluster(points_, locations_, contact_);
		int num_locations = locations_.size();
		reshapeVector(labels_, 	num_locations);
		reshapeVector(loc_idx_zero,	num_locations);
		printer(23);
		showData(points_, labels_, labels_ref_, loc_idx_zero, color_code, true, false, true);
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
		for(int i=0;i<points_.size();i++)
		{
			if (points_[i].l >= 0)
				points_[i].l = loc_idx_zero[points_[i].l];
			//printf("Location %02d: %02f\n", i, points_[i].l );
		}
		locations_.clear();
		combineNearCluster(points_, locations_, contact_);
	}
	else
	{
		combineNearCluster(points_, locations_, contact_);
	}

	//contactCheck(points_, contact_, num_locations);
	printer(14);

	contactBoundary(points_, locations_, true);
	contactBoundary(points_, locations_, false);

	return EXIT_SUCCESS;
}

int buildLocationArea(
	Graph 						*Graph_,
	vector<vector<point_d> > 	&pos_vel_acc_,
	vector<int> 				contact_)
{
	// [VARIABLES]*************************************************************
	bool 			flag = false;
	vector<point_d> points_avg;
	vector<point_d> locations;
	vector<string>  goal_action;
	vector<string>	action_label_tmp;
	vector<int> 	loc_idx_zero;
	vector<vector<unsigned char> > color_code; colorCode(color_code);
	// *************************************************************[VARIABLES]

	vector<int> contact_tmp;

	// [LABELLING LOCATION]****************************************************
	for(int i=0;i<Graph_->getNumberOfNodes();i++)
	{
		node_tt node_tmp = {};
		Graph_->getNode(i, node_tmp);
		goal_action.push_back(node_tmp.name);
		locations.push_back(node_tmp.centroid);
	}
	for(int i=0;i<pos_vel_acc_.size();i++)
	{
		points_avg.push_back(pos_vel_acc_[i][0]);
	}
	if (Graph_->getNumberOfNodes()==0)
	{
		Graph_->getActionLabel(action_label_tmp);
		clusteringExt(points_avg, contact_tmp, locations, goal_action, action_label_tmp, true);
		reshapeVector(goal_action, locations.size());
		printer(15);
		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, true, false);
	}
	else
	{
		Graph_->getActionLabel(action_label_tmp);
		clusteringExt(points_avg, contact_tmp, locations, goal_action, action_label_tmp, false);
	}
	for(int i=0;i<pos_vel_acc_.size();i++)
	{
		pos_vel_acc_[i][0].l = points_avg[i].l;
	}
	printer(16);
	// ****************************************************[LABELLING LOCATION]

	// [NODES]*****************************************************************
	if (Graph_->getNumberOfNodes()>0)
	{
		for(int i=0;i<locations.size();i++)
		{
			int key = -1;
			double mem = 10.0;
			for(int ii=0;ii<Graph_->getNumberOfNodes();ii++)
			{
				node_tt node_tmp = {};
				Graph_->getNode(ii, node_tmp);
				if (!strcmp(node_tmp.name.c_str(),"DRINK") || !strcmp(node_tmp.name.c_str(),"EAT"))
				{
					key = 0;
					continue;
				}
				if (min_(l2Norm(minusPoint(node_tmp.centroid, locations[i])), mem) &&
					l2Norm(minusPoint(node_tmp.centroid, locations[i])) < CLUSTER_LIMIT)
				{
					mem = l2Norm(minusPoint(node_tmp.centroid, locations[i]));
					key = ii;
				}
			}
			if (key<0)
			{
				goal_action.resize(goal_action.size()+1);
				flag = true;
				node_tt node_tmp = {};
				node_tmp.name 				= "";
				node_tmp.index 				= Graph_->getNodeList().size();
				node_tmp.centroid 			= locations[i];
				Graph_->setNode(node_tmp);
				Graph_->addEmptyEdgeForNewNode(node_tmp.index);
//				Graph_->expandFilter(node_tmp.index+1);
//				Graph_->updateFilter(node_tmp.index);
			}
			else
			{
				node_tt node_tmp = {};
				Graph_->getNode(key, node_tmp);
				if (!strcmp(node_tmp.name.c_str(),"DRINK") || !strcmp(node_tmp.name.c_str(),"EAT"))
				{
					continue;
				}
				double tmp1 = locations[key].l;
				double tmp2 = node_tmp.centroid.l;
				tmp1 = Sqr(-log(tmp1)*BOUNDARY_VAR*2);
				tmp2 = Sqr(-log(tmp2)*BOUNDARY_VAR*2);
				point_d loc_tmp =
						multiPoint(
								addPoint(
										node_tmp.centroid,
										locations[key]),
								0.5);
				tmp1 += l2Norm(minusPoint(locations[key],loc_tmp));
				tmp2 += l2Norm(minusPoint(node_tmp.centroid,loc_tmp));
				locations[key] = loc_tmp;
				locations[key].l =
						min(0.7, pdfExp(BOUNDARY_VAR, 0.0, max(tmp1,tmp2)));
				locations[key].l =
						max(0.7, locations[key].l);
				node_tmp.centroid = locations[key];
				Graph_->setNode(node_tmp);
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
			Graph_->setNode(node_tmp);
			Graph_->addEmptyEdgeForNewNode(node_tmp.index);
//			Graph_->expandFilter(node_tmp.index+1);
//			Graph_->updateFilter(node_tmp.index);
		}
	}
	printer(17);
	// *****************************************************************[NODES]

//	if (flag)
//	{
//		goal_action.clear();
//		for(int i=0;i<Graph_->getNumberOfNodes();i++)
//		{
//			node_tt node_tmp = {};
//			Graph_->getNode(i, node_tmp);
//			goal_action.push_back(node_tmp.name);
//		}
//		vector<string> action_label_tmp;
//		Graph_->getActionLabel(action_label_tmp);
//		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, true, false);
//	}
//	else
//	{
//		goal_action.clear();
//		for(int i=0;i<Graph_->getNumberOfNodes();i++)
//		{
//			node_tt node_tmp = {};
//			Graph_->getNode(i, node_tmp);
//			goal_action.push_back(node_tmp.name);
//		}
//		vector<string> action_label_tmp;
//		Graph_->getActionLabel(action_label_tmp);
//		showData(points_avg, goal_action, action_label_tmp, loc_idx_zero, color_code, true, false, false);
//	}
	return EXIT_SUCCESS;
}
