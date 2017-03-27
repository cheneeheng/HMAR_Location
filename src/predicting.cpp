/*
 * predicting.cpp
 *
 *  Created on: Mar 19, 2017
 *      Author: chen
 */

#include "predicting.h"

int predictFromNode(
	Graph Graph_,
	point_d &point_)
{
	return decideBoundary(point_, point_, Graph_.getCentroidList());
}

int decideMovement(
	Graph Graph_,
	point_d vel_,
	predict_t &predict_)
{
	// #### TODO: vel limit
	if (l2Norm(vel_)<0.005) 	{ predict_.velocity = 0; }
	else						{ predict_.velocity = l2Norm(vel_); }
	return EXIT_SUCCESS;
}

int decideCurvatureExt(
	point_d point_,
	vector<point_d> &curve_mem_,
	predict_t &predict_,
	int num_points_)
{
	//	predict_.curvature = 1 - pdfExp(0.1, 0, predict_.curvature);
	double curve = 0.0;
	decideCurvature(point_, curve_mem_, curve, num_points_);
	predict_.curvature.push_back(curve);
	if(predict_.curvature.size()>10)
	{
		predict_.curvature.erase(predict_.curvature.begin());
	}
	return EXIT_SUCCESS;
}

//int checkPlane(Graph Graph_, pos_, curve_mem_, predict_)
//{
//	double det = 0.0;
//	vector<vector<double> > tmp;
//	tmp.push_back(
//			point2vector(
//					minusPoint(
//							points_cluster[1],
//							points_cluster[0])));
//	tmp.push_back(
//			point2vector(
//					minusPoint(
//							points_cluster[points_cluster.size()-1],
//							points_cluster[0])));
//	for(int i=2;i<points_cluster.size()-1;i++)
//	{
//		if(vel_tmp[i]>0.005) //TODO : verify vel_lim
//		{
//			tmp.push_back(
//					point2vector(
//							minusPoint(
//									points_cluster[i],
//									points_cluster[0])));
//			det = max(determinant(tmp),det);
//			tmp.pop_back();
//		}
//	}
//	return EXIT_SUCCESS;
//}

double decideLocSecInt(
	Graph &Graph_,
	Graph &Graph_update_,
	point_d point_,
	point_d &delta_t_,
	int &sec_idx_,
	int &loc_idx_,
	int &loc_last_idx_,
	int label1_,
	int label2_)
{
	double last, tmp_dis;
	vector<double> sm;
	edge_tt edge_tmp;
	Graph_update_.getEdgeSectorMap(label1_, label2_, 0, sm);
	edge_tmp = Graph_.getEdge(label1_, label2_, 0);
	last 	 = loc_last_idx_;
	tmp_dis  =
			decideLocationInterval(
					loc_idx_,
					loc_last_idx_,
					point_,
					edge_tmp.loc_start,
					edge_tmp.loc_mid,
					edge_tmp.loc_end,
					edge_tmp.tan,
					20);
	if(loc_idx_-last>1)
	{
		for(int i=0;i<loc_idx_-last;i++)
		{
			decideSectorInterval(
					sec_idx_,
					last+1+i,
					delta_t_,
					point_,
					edge_tmp.loc_mid,
					edge_tmp.tan,
					edge_tmp.nor);
			sm[(last+1+i)*SEC_INT + sec_idx_] < l2Norm(delta_t_) ?
					sm[(last+1+i)*SEC_INT + sec_idx_] = l2Norm(delta_t_) :
					0;
		}
	}
	else
	{
		decideSectorInterval(
				sec_idx_,
				loc_idx_,
				delta_t_,
				point_,
				edge_tmp.loc_mid,
				edge_tmp.tan,
				edge_tmp.nor);
		sm[loc_idx_*SEC_INT + sec_idx_] < l2Norm(delta_t_) ?
				sm[loc_idx_*SEC_INT + sec_idx_] = l2Norm(delta_t_) : 0;
	}
	Graph_update_.setEdgeSectorMap(label1_, label2_, 0, sm);
	return tmp_dis;
}

bool decideGoal(
	predict_t &predict_,
	int label2_,
	double &sm_i_, //sectormap single
	double delta_t_,
	double loc_error_)
{
	if (loc_error_ > 1.075) //## TODO :NEED TO VERIFY
	{
		predict_.range[label2_] = RANGE_EXCEED;
		return false;
	}

	if (delta_t_ <= sm_i_)
	{
		predict_.range[label2_] = RANGE_IN;
		predict_.pct_err[label2_]   = 0.999999;
	}
	else
	{
		predict_.range[label2_] = RANGE_OUT;
		predict_.err[label2_] = delta_t_ - sm_i_;
		predict_.pct_err[label2_] = pdfExp(0.01, 0.0, predict_.err[label2_]);
	}
	return true;
}

int decideWindow(
	predict_t &predict_,
	vector<double> sm_,
	int loc_idx_,
	int label2_)
{
	double max_val = 0.0;
	for(int s=0;s<SEC_INT/2;s++)
	{
		double tmp =
				sm_[loc_idx_*SEC_INT+s] +
				sm_[loc_idx_*SEC_INT+s+SEC_INT/2];
		max_val = max(tmp,max_val);
	}
	predict_.window[label2_] = max_val;
//	predict_.window[label2_] = min(pdfExp(0.01,0,max_val-0.001), 1.0);
	return EXIT_SUCCESS;
}

int decideSectorMapChangeRate(
	predict_t &predict_,
	vector<double> sm_,
	vector<double> sm2_,
	int loc_idx_,
	int label2_)
{
	vector<double> diff_tmp; diff_tmp.resize(LOC_INT);
	int idx1 = (loc_idx_-10) < 0 ? 0 : (loc_idx_-10); // faster and dont get affected by other change along the trajectory
	int idx2 = (loc_idx_+10) > LOC_INT ? LOC_INT : (loc_idx_+10); // faster and dont get affected by other change along the trajectory
	for(int l=idx1;l<idx2;l++)
	{
		double old_sm = 0.0;
		for(int s=0;s<SEC_INT/2;s++)
		{
			double tmp =
					sm2_[l*SEC_INT+s] +
					sm2_[l*SEC_INT+s+SEC_INT/2];
			old_sm = max(tmp, old_sm);
		}
		double new_sm = 0.0;
		for(int s=0;s<SEC_INT/2;s++)
		{
			double tmp =
					sm_[l*SEC_INT+s] +
					sm_[l*SEC_INT+s+SEC_INT/2];
			new_sm = max(tmp, new_sm);
		}
		diff_tmp[l] = fabs(old_sm-new_sm);
	}
	vector<double> ddt_tmp;
	for(int n=2;n<diff_tmp.size();n++)
	{
		ddt_tmp.push_back(
				diff_tmp[n] -
				diff_tmp[n-1] -
				diff_tmp[n-1] +
				diff_tmp[n-2]);
	}

	// -1 because calculating difference
	// sudden zero is because the original diff is already bigger than the one that is being registered
	predict_.err_diff[label2_] =
			*max_element(ddt_tmp.begin() + loc_idx_ - 2, ddt_tmp.end());
	predict_.pct_err_diff[label2_] =
			1 - pdfExp(0.01, 0.0, predict_.err_diff[label2_]);

	return EXIT_SUCCESS;
}

int decideOscillate(
	predict_t &predict_,
	point_d vel_,
	point_d acc_,
	vector<double> loc_mem_,
	vector<double> sec_mem_,
	vector<point_d> tan_tmp_,
	vector<point_d> nor_tmp_,
	int loc_idx_,
	int label2_)
{
	point_d tmpN0 =
			rodriguezVec(
					2*M_PI*(double)sec_mem_[0]/(double)SEC_INT,
					tan_tmp_[loc_mem_[0]],
					nor_tmp_[loc_mem_[0]]);

	tmpN0 = multiPoint(tmpN0, 1/l2Norm(tmpN0));

	for(int ii=1;ii<sec_mem_.size();ii++)
	{
		if (loc_mem_[0]==loc_mem_[ii]) {continue;}

		point_d tmpN = rodriguezVec(
						2*M_PI*(double)sec_mem_[ii]/(double)SEC_INT,
						tan_tmp_[loc_mem_[ii]],
						nor_tmp_[loc_mem_[ii]]);
		vector<double> tmpRTI =
							transInv(
									rodriguezRot(
											tan_tmp_[loc_mem_[ii]],
											tan_tmp_[loc_mem_[0]]));
		point_d tmpNRot;
		tmpNRot.x = tmpRTI[0]*tmpN.x + tmpRTI[1]*tmpN.y + tmpRTI[2]*tmpN.z;
		tmpNRot.y = tmpRTI[3]*tmpN.x + tmpRTI[4]*tmpN.y + tmpRTI[5]*tmpN.z;
		tmpNRot.z = tmpRTI[6]*tmpN.x + tmpRTI[7]*tmpN.y + tmpRTI[8]*tmpN.z;
		double angle_tmp =
				atan2(
						l2Norm(
								crossProduct(
										point2vector(tmpNRot),
										point2vector(tmpN0))),
						dotProduct(
								point2vector(tmpNRot),
								point2vector(tmpN0)));
		if (!vectorDirectionCheck(
				crossProduct(
						point2vector(tmpN0),
						point2vector(
								multiPoint(
										tmpNRot,
										1/l2Norm(tmpNRot)))),
				point2vector(tan_tmp_[loc_mem_[0]])))
		{angle_tmp *= -1;}
		angle_tmp = fmod((2*M_PI + angle_tmp),(2*M_PI));
		sec_mem_[ii] = ceil(angle_tmp*(SEC_INT/2)/M_PI) - 1 ;
	}

	double X[2], d2[3], tmps[2];

	X[0]  = abs(dotProduct(point2vector(tan_tmp_[loc_idx_]), point2vector(vel_)) /
				(l2Norm(tan_tmp_[loc_idx_])*l2Norm(vel_)));
	X[1]  = predict_.pct_err[label2_];
	d2[0] = 0.0;
	d2[1] = 0.0;
	d2[2] = predict_.pct_err_diff[label2_];
	tmps[0] = sec_mem_[1] - sec_mem_[0];
	tmps[1] = sec_mem_[2] - sec_mem_[1];
	tmps[0] =
			(fabs(tmps[0]) > SEC_INT/2) ?
					((tmps[0] > 0) ?
							(tmps[0] - SEC_INT) :
							(tmps[0] + SEC_INT))
					: tmps[0];
	tmps[1] =
			(fabs(tmps[1]) > SEC_INT/2) ?
					((tmps[1] > 0) ?
							(tmps[1] - SEC_INT) :
							(tmps[1] + SEC_INT))
					: tmps[1];
	d2[0] = fabs((tmps[1] - tmps[0])/SEC_INT);
	d2[1] = fabs((loc_mem_[2] + loc_mem_[0] - (2*loc_mem_[1]))/LOC_INT);
	d2[0] = 1 - pdfExp(0.01, 0, d2[0]);
	d2[1] = 1 - pdfExp(0.01, 0, d2[1]);

	// might need to filter the output
	// Here the result of this formula/equation should in theory be similar/equal to acceleration.
	// However the advantage of using this is the fact that the d2[1..3] can be weighted individually to represent different action.
	// Since this is in the representation of the sector-map,
	// it gives a more intuitive explanation of the property of an action,
	// instead of giving just a spatial representation in 3D as in the case of acceleration.
	predict_.oscillate[label2_] =
			((1.0 - X[1]) *         d2[2]) +
			((      X[1]) *
					((1.0 - X[0]) * d2[0] +
					 (      X[0]) * d2[1]));

	return EXIT_SUCCESS;
}

int predictFromSectorMap(
	Graph &Graph_,
	Graph &Graph_update_,
	point_d point_,
	point_d vel_,
	point_d acc_,
	predict_t &predict_,
	vector<int> &loc_last_idxs_,
	int label1_)
{
	for(int i=0;i<Graph_.getNumberOfNodes();i++)
	{
		predict_.range[i] 			= RANGE_NULL;
		predict_.err[i] 			= 0.0;
		predict_.pct_err[i]   		= 0.0;
		predict_.err_diff[i] 		= 0.0;
		predict_.pct_err_diff[i]   	= 0.0;
		predict_.oscillate[i]		= 0.0;
		predict_.window[i]			= 0.0;

		if (label1_==i) {continue;}
		if (Graph_.getEdgeCounter(label1_, i, 0)==0) {continue;}

		vector<point_d> tan_tmp, nor_tmp;
		vector<double> sm_tmp, sm_tmp2;
		Graph_.getEdgeTan(label1_, i, 0, tan_tmp);
		Graph_.getEdgeNor(label1_, i, 0, nor_tmp);
		Graph_.getEdgeSectorMap(label1_, i, 0, sm_tmp);

		point_d delta_t;
		int loc_idx, sec_idx;
		double tmp_dis;

		// [LOC SEC INT]*******************************************************
		loc_idx = -1;
		tmp_dis = decideLocSecInt(
				Graph_, Graph_update_, point_, delta_t,
				sec_idx, loc_idx, loc_last_idxs_[i], label1_, i);
		// *******************************************************[LOC SEC INT]

		// [GLA]***************************************************************
		bool flag =
				decideGoal(
						predict_, i, sm_tmp[loc_idx*SEC_INT + sec_idx],
						l2Norm(delta_t), tmp_dis);
		if (!flag) {continue;}
		// ***************************************************************[GLA]

		// [LOC INT CONSTRAINT]************************************************
		decideWindow(predict_, sm_tmp, loc_idx, i);
		// ************************************************[LOC INT CONSTRAINT]

		// [REPETITIVE]********************************************************
		vector<double> loc_mem;
		vector<double> sec_mem;
		Graph_.getEdgeLocMem(label1_, i, 0, loc_mem);
		Graph_.getEdgeSecMem(label1_, i, 0, sec_mem);
		if (loc_mem.size() < 3)
		{
			loc_mem.push_back(loc_idx);
			sec_mem.push_back(sec_idx);
			Graph_.setEdgeLocMem(label1_, i, 0, loc_mem);
			Graph_.setEdgeSecMem(label1_, i, 0, sec_mem);
		}
		else
		{
			// [LOC INT CONSTRAINT]********************************************
			Graph_update_.getEdgeSectorMap(label1_, i, 0, sm_tmp2);
			decideSectorMapChangeRate(predict_, sm_tmp, sm_tmp2, loc_idx, i);
			// ********************************************[LOC INT CONSTRAINT]

			loc_mem.erase(loc_mem.begin());
			sec_mem.erase(sec_mem.begin());
			loc_mem.push_back(loc_idx);
			sec_mem.push_back(sec_idx);
			Graph_.setEdgeLocMem(label1_, i, 0, loc_mem);
			Graph_.setEdgeSecMem(label1_, i, 0, sec_mem);

			decideOscillate(
					predict_, vel_, acc_, loc_mem, sec_mem, tan_tmp, nor_tmp,
					loc_idx, i);
		}
		// ********************************************************[REPETITIVE]
	}

	return EXIT_SUCCESS;
}

int evaluatePrediction(
	Graph &Graph_,
	predict_t &predict_)
{
	map<string,pair<int,int> > ac_tmp;
	vector<string> al_tmp;
	vector<map<string, double> > f_tmp;
	Graph_.getActionCategory(ac_tmp);
	Graph_.getActionLabel(al_tmp);
	Graph_.getFilter(f_tmp);

	double curve = average(predict_.curvature);

	for(int i=0;i<Graph_.getNumberOfNodes();i++)
	{
		for(int ii = ac_tmp["GEOMETRIC"].first;
				ii < ac_tmp["GEOMETRIC"].second+1;
				ii++)
		{
			f_tmp[i][al_tmp[ii]] = f_tmp[i][al_tmp[ii]]*predict_.pct_err[i];
		}
		f_tmp[i]["CUT"]		= f_tmp[i]["CUT"]	* predict_.oscillate[i];
		f_tmp[i]["CLEAN"] 	= f_tmp[i]["CLEAN"]	* predict_.oscillate[i];
		f_tmp[i]["SCAN"] 	= f_tmp[i]["SCAN"]	* predict_.window[i];
		f_tmp[i]["WINDOW"]	= f_tmp[i]["WINDOW"]* predict_.window[i];
		f_tmp[i]["SLIDE"] 	= f_tmp[i]["SLIDE"]	* predict_.oscillate[i];

//		if (i==1)
//		{
//			cout << predict_.oscillate[i] << endl;
//		}

		f_tmp[i]["CURVE"] 	= f_tmp[i]["CURVE"]	* curve;
		f_tmp[i]["MOVE"] 	= f_tmp[i]["MOVE"]	* predict_.velocity;
		f_tmp[i]["STOP"] 	= f_tmp[i]["STOP"]	* fmod((predict_.velocity+1),2);
	}

	Graph_.setPrediction(f_tmp);

	return EXIT_SUCCESS;
}

int predictFromEdge(
	Graph &Graph_,
	Graph &Graph_update_,
	point_d pos_,
	point_d vel_,
	point_d acc_,
	vector<point_d> &curve_mem_,
	predict_t &predict_,
	vector<int> &last_loc_,
	int label1_)
{
	// 2.1. Check for motion
	decideMovement(Graph_, vel_, predict_);

	// 2.2. Check for trajectory curvature
	decideCurvatureExt(pos_, curve_mem_, predict_, 3);

	// 2.3. Check if repetitive motion is detected
	// 2.4. Check for window
	// 2.5. Check if the trajectory is within the range of sector map
	predictFromSectorMap(Graph_, Graph_update_, pos_, vel_, acc_, predict_, last_loc_, label1_);

	// 2.X. Predict the goal based on the trajectory error from sector map
	evaluatePrediction(Graph_, predict_);

	return EXIT_SUCCESS;
}

int predictAction(
	Graph &Graph_,
	Graph &Graph_update_,
	int contact_,
	vector<point_d> &pva_avg_,
	vector<point_d> &curve_mem_,
	predict_t &predict_,
	int &label1_,
	vector<int> &last_loc_,
	bool learn_)
{
	// 1. Contact trigger
	// 1.1 Check if the object is within a sphere volume of the location areas
	predictFromNode(Graph_, pva_avg_[0]);

	// 2. Prediction during motion
	if (pva_avg_[0].l < 0)
	{
		predictFromEdge(
				Graph_,
				Graph_update_,
				pva_avg_[0],
				pva_avg_[1],
				pva_avg_[2],
				curve_mem_,
				predict_,
				last_loc_,
				label1_);

		// all_of is c++11
		bool zeros =
				all_of(
						predict_.err.begin(),
						predict_.err.end(),
						[](double ii) { return ii==0.0; });
	}
	// 3. Prediction within location area
	else
	{
		if (learn_) { Graph_ = Graph_update_; }
		label1_ = pva_avg_[0].l;
		vector<map<string, double> > f_tmp;
		Graph_.getFilter(f_tmp);
		Graph_.setPrediction(f_tmp);
		reshapePredict(predict_, Graph_.getNumberOfNodes());
	}


	// 4. OUTPUT
	// says what i am currently doing

	return EXIT_SUCCESS;
}