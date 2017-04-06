/*
 * labeling_sec.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: chen
 */

#include "labeling_sec.h"

stack<clock_t> tictoc_stack;
void tic() {tictoc_stack.push(clock());}
void toc()
{
	cout << "Time elapsed: "
       << ((double)(clock() - tictoc_stack.top())) / CLOCKS_PER_SEC
       << std::endl;
	tictoc_stack.pop();
}

// Keeps the original label and removes any clusters that it finds
int findMovementConstraint(
	Graph *Graph_,
	vector<point_d> &points_,
	vector<point_d> vels_,
	int label1_,
	int label2_)
{
	bool flag 	= true;
	point_d point_mem;
	vector<point_d> points_cluster; // ### TODO: can be extended to include multiple clusters
	vector<point_d> points_tmp, points_out, centroids;
	vector<double> vel_tmp;
	vector<int> contact;
	points_tmp = points_;
	clustering(points_tmp, DBSCAN_EPS_MOV, DBSCAN_MIN_MOV);
//	combineNearCluster(points_tmp, centroids, contact);
//	combineNearCluster(points_tmp, centroids, contact); ### TODO: can be extended to include multiple clusters
//	reshapeVector(points_cluster, centroids.size()); ### TODO: can be extended to include multiple clusters
	for(int i=0;i<points_tmp.size();i++)
	{
		if(points_tmp[i].l<0)
		{
			if(flag)
			{
				flag = false;
				point_mem = points_tmp[i];
			}
			if(l2Norm(minusPoint(point_mem,points_[i]))>0.20 && !flag) //TODO : verify 0.10 ratio
			{
				continue;
			}
			points_out.push_back(points_[i]);
			point_mem = points_[i];
		}
		else
		{
			points_cluster.push_back(points_[i]);
			vel_tmp.push_back(l2Norm(vels_[i]));
		}
	}

	// Visualize
	if(0)
	{
		vector<point_d> P = points_;
		P.insert(P.end(),points_out.begin(),points_out.end());
		for(int i=0;i<points_.size();i++) { P[i].l = 1; }
		vector<vector<unsigned char> > color_code; colorCode(color_code);
		vector<string> label_; label_.resize(5);
		vector<string> label_ref; label_ref.resize(5);
		vector<int> loc_idx_zero;
		showData(P, label_, label_ref, loc_idx_zero, color_code, true, false, false);
	}

	if (points_out.size()>20)
	{
		points_.clear();
		points_ = points_out;
	}

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
//	vector<int> constraint;
//	Graph_.getEdgeMovementConstraint(label1_, label2_, 0, constraint);
//	if (det > 0.2) //TODO : verify determinant error
//	{
//		constraint[MOV_CONST_SURF] = 1;
//	}
//	Graph_.setEdgeMovementConstraint(label1_, label2_, 0, constraint);

	return EXIT_SUCCESS;
}

int fitCurve(
	vector<point_d> points_avg_,
	vector<point_d> &points_est_,
	vector<point_d> &coeffs_)
{
	vector<point_d> covs;
	reshapeVector(points_est_,(points_avg_.size())*4);
	polyCurveFitPoint(points_avg_, points_est_, coeffs_, covs, true);

	// Visualize by comparing both original and estimated points
	if(0)
	{
		vector<point_d> P = points_avg_;
		P.insert(P.end(),points_est_.begin(),points_est_.end());
		for(int i=0;i<points_avg_.size();i++) { P[i].l = 1; }
		vector<vector<unsigned char> > color_code; colorCode(color_code);
		vector<string> label_; label_.resize(2);
		vector<string> label_ref; label_ref.resize(2);
		vector<int> loc_idx_zero;
		showData(P, label_, label_ref, loc_idx_zero, color_code, true, false, false);
	}

	printer(18);
	return EXIT_SUCCESS;
}

int decideSectorIntervalExt(
	edge_tt	edge_,
	point_d point_,
	point_d &delta_t_,
	int &sec_idx_,
	int loc_idx_)
{
	vector<point_d> mid	= edge_.loc_mid;
	vector<point_d> tan	= edge_.tan;
	vector<point_d> nor	= edge_.nor;
	return
			decideSectorInterval(
					sec_idx_, loc_idx_, delta_t_, point_, mid, tan, nor);
}

double decideLocationIntervalExt(
	edge_tt	edge_,
	point_d point_,
	int &loc_idx_,
	int &loc_last_idx_,
	int loc_offset_,
	bool loc_init_)
{
	vector<point_d> beg	= edge_.loc_start;
	vector<point_d> mid	= edge_.loc_mid;
	vector<point_d> end	= edge_.loc_end;
	vector<point_d> tan	= edge_.tan;
	return
			decideLocationInterval(
					loc_idx_, loc_last_idx_, point_,
					beg, mid, end, tan, loc_offset_);
}

double decideLocationIntervalExt(
	edge_tt	edge_,
	point_d point_,
	vector<int> &loc_idxs_,
	int &loc_last_idx_,
	int loc_offset_,
	bool loc_init_)
{
	vector<point_d> beg	= edge_.loc_start;
	vector<point_d> mid	= edge_.loc_mid;
	vector<point_d> end	= edge_.loc_end;
	vector<point_d> tan	= edge_.tan;
	return decideLocationInterval(
			loc_idxs_, loc_last_idx_, point_,
			beg, mid, end, tan, loc_offset_, loc_init_);
}

double dLIE(
	edge_tt	edge_,
	point_d point_,
	int &loc_idx_,
	int loc_last_idx_,
	int loc_offset_,
	bool loc_init_)
{
	vector<point_d> beg	= edge_.loc_start;
	vector<point_d> mid	= edge_.loc_mid;
	vector<point_d> end	= edge_.loc_end;
	vector<point_d> tan	= edge_.tan;
	return dLI(
			loc_idx_, loc_last_idx_, point_,
			beg, mid, end, tan, loc_offset_, loc_init_);
}

int adjustSectorMap(
	edge_tt &edge_,
	vector<point_d> &points_,
	int &loc_last_idx_,
	int &loc_curr_idx_,
	double &delta_t_mem_,
	bool &loc_init_,
	int loc_offset_,
	int option_)
{
	point_d delta_t, delta_t_zero;
	int sec_idx = -1;
	int loc_idx = -1;
	int loc_last_idx_mem = loc_last_idx_;
	bool mem = loc_init_;
	switch(option_)
	{
		case 1:
		{
			int loc_idx;
			// to deal with cases where the beginning is out of the sectormap
			// required for the fitting part only
			// also for the ending

			if (dLIE(edge_, points_[1],
					loc_idx, loc_last_idx_, loc_offset_, loc_init_) > 0.001)
			{
				if(mem || loc_last_idx_==LOC_INT-1)
				{
					loc_last_idx_ = loc_last_idx_mem;
					return EXIT_SUCCESS; //#TODO should be failure
				}
				else
				{
					loc_last_idx_ = loc_last_idx_mem;
					return EXIT_SUCCESS; //#TODO should be failure
				}
			}

			// find the delta_t
			decideSectorIntervalExt(
					edge_, points_[1], delta_t, sec_idx, loc_idx);


			// to fill up the line if loc_int is valid but tangent is not aligned to traj (at a curve)
			double tmpmin = 10.0;
			double tmpdeltatmin = 0.0;
			int min_idx = 0;

			if (loc_idx==loc_last_idx_)
			{
				min_idx			= loc_idx*SEC_INT + sec_idx;
				tmpmin			= l2Norm(minusPoint(points_[1], edge_.loc_mid[loc_idx]));
				tmpdeltatmin	= l2Norm(delta_t);
			}
			else if (loc_idx>loc_last_idx_)
			{
				for(int ll=loc_last_idx_+1;ll<loc_idx+1;ll++)
				{
					decideSectorIntervalExt(
							edge_, points_[1], delta_t_zero, sec_idx, ll);
					int tmp_idx = ll*SEC_INT + sec_idx;
					if (min_(l2Norm(minusPoint(points_[1], edge_.loc_mid[ll])), tmpmin))
					{
						tmpmin			= l2Norm(minusPoint(points_[1], edge_.loc_mid[ll]));
						tmpdeltatmin	= l2Norm(delta_t_zero);
						min_idx 		= tmp_idx;
					}
				}
			}


			if (loc_init_)
			{
				// if point starts before curve
				point_d proj_dir_tmp =
						multiPoint(
								edge_.tan[0],
								dotProduct(
										point2vector(minusPoint(points_[1],edge_.loc_mid[0])),
										point2vector(edge_.tan[0])));
				if (!vectorDirectionCheck(
						point2vector(proj_dir_tmp),
						point2vector(edge_.tan[0])))
				{
					loc_init_ = true;
					loc_last_idx_ = 0;
					loc_curr_idx_ = 0;
					return EXIT_SUCCESS;
				}
				else
				{
					loc_init_ = false;
					edge_.sector_map[min_idx] = max(edge_.sector_map[min_idx], tmpdeltatmin);
//					if (edge_.sector_map[min_idx]>0.3)  {return EXIT_FAILURE;}
					loc_last_idx_ = min_idx/SEC_INT;
					loc_curr_idx_ = min_idx/SEC_INT;
					return EXIT_SUCCESS;
				}
			}

			edge_.sector_map[min_idx] = max(edge_.sector_map[min_idx], tmpdeltatmin);
//			if (edge_.sector_map[min_idx]>0.3)  {return EXIT_FAILURE;}

			if ((min_idx/SEC_INT) - loc_last_idx_ > 1)
			{
				for(int l=loc_last_idx_+1;l<(min_idx/SEC_INT);l++)
				{
					decideSectorIntervalExt(
							edge_, points_[1], delta_t_zero, sec_idx, l);
					int tmp_idx = l*SEC_INT + sec_idx;
					edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
				}
				loc_curr_idx_ = (min_idx/SEC_INT);
			}
			else if ((min_idx/SEC_INT) - loc_last_idx_ == 1)
			{
				loc_curr_idx_ = (min_idx/SEC_INT);
			}

//			cout << edge_.sector_map[min_idx] << endl; 0.449658

			loc_last_idx_ = loc_curr_idx_;
			return EXIT_SUCCESS;






//			// extend the delta_t to last location to "fill up holes"
//			// compare trajectory and tangent
//			if (dotProduct(
//					point2vector(edge_.tan[loc_idx]),
//					point2vector(
//							multiPoint(
//									minusPoint(
//											points_[1],
//											points_[0]),
//									1/l2Norm(
//											minusPoint(
//													points_[1],
//													points_[0])))))>0.88)
//			{
//				loc_init_ = false;
//				if (loc_idx>loc_last_idx_)
//				{
//					for(int ll=loc_last_idx_+1;ll<loc_idx+1;ll++)
//					{
//						double tmp =
//								delta_t_mem_ +
//								((l2Norm(delta_t) - delta_t_mem_) *
//										(((double)(ll-loc_last_idx_-1) + 1) / (loc_idx - loc_last_idx_)));
//						int tmp_idx = ll*SEC_INT + sec_idx;
//						decideSectorIntervalExt(
//								edge_, points_[1], delta_t_zero, sec_idx, ll);
//						edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], tmp);
//					}
//					int tmp_idx = loc_idx*SEC_INT + sec_idx;
//					edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t));
//					delta_t_mem_ = l2Norm(delta_t);
//					loc_last_idx_ = loc_idx;
//					loc_curr_idx_ = loc_last_idx_;
//				}
//				else if (loc_idx==loc_last_idx_)
//				{
//					int tmp_idx = loc_idx*SEC_INT + sec_idx;
//					edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t));
//					delta_t_mem_ = l2Norm(delta_t);
//					loc_last_idx_ = loc_idx;
//					loc_curr_idx_ = loc_last_idx_;
//				}
//				// should not happen cause the dot product means that it is in the same direction
////				else
////				{
////					for(int ll=loc_idx;ll<loc_last_idx_;ll++)
////					{
////						double tmp =
////								delta_t_mem_ +
////								((l2Norm(delta_t) - delta_t_mem_) *
////										(((double)(ll-loc_idx) + 1) / (loc_idx - loc_last_idx_)));
////						int tmp_idx = ll*SEC_INT + sec_idx;
////						decideSectorIntervalExt(
////								edge_, points_[1], delta_t_zero, sec_idx, ll);
////						edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], tmp);
////					}
////					delta_t_mem_ = l2Norm(delta_t);
////					(loc_last_idx_ > loc_curr_idx_) ? loc_curr_idx_ = loc_last_idx_: 0;
////				}
//				return EXIT_SUCCESS;
//			}
//			else
//			{
//				cout << "AA : " << loc_idx << " ";
//				// to fill up the line if loc_int is valid but tangent is not aligned to traj (at a curve)
//				double tmpmin = 10.0;
//				double tmpdeltatmin = 0.0;
//				int min_idx = 0;
//
//				if (loc_idx==loc_last_idx_)
//				{
//					decideSectorIntervalExt(
//							edge_, points_[1], delta_t_zero, sec_idx, loc_idx);
//					int tmp_idx = loc_idx*SEC_INT + sec_idx;
//					if (min_(l2Norm(minusPoint(points_[1], edge_.loc_mid[loc_idx])), tmpmin))
//					{
//						tmpmin			= l2Norm(minusPoint(points_[1], edge_.loc_mid[loc_idx]));
//						tmpdeltatmin	= l2Norm(delta_t_zero);
//						min_idx 		= tmp_idx;
//					}
//				}
//				else if (loc_idx>loc_last_idx_)
//				{
//					for(int ll=loc_last_idx_+1;ll<loc_idx+1;ll++)
//					{
//						decideSectorIntervalExt(
//								edge_, points_[1], delta_t_zero, sec_idx, ll);
//						int tmp_idx = ll*SEC_INT + sec_idx;
//						if (min_(l2Norm(minusPoint(points_[1], edge_.loc_mid[ll])), tmpmin))
//						{
//							tmpmin			= l2Norm(minusPoint(points_[1], edge_.loc_mid[ll]));
//							tmpdeltatmin	= l2Norm(delta_t_zero);
//							min_idx 		= tmp_idx;
//						}
//					}
//				}
//				cout << min_idx/SEC_INT << " " << loc_init_ << " ";
//
//				if (loc_init_)
//				{
//					// if point starts before curve
//					point_d proj_dir_tmp =
//							multiPoint(
//									edge_.tan[0],
//									dotProduct(
//											point2vector(minusPoint(points_[1],edge_.loc_mid[0])),
//											point2vector(edge_.tan[0])));
//					cout << vectorDirectionCheck(
//							point2vector(proj_dir_tmp),
//							point2vector(edge_.tan[0])) << endl;
//					if (!vectorDirectionCheck(
//							point2vector(proj_dir_tmp),
//							point2vector(edge_.tan[0])))
//					{
//						loc_last_idx_ = 0;
//						loc_curr_idx_ = 0;
//						return EXIT_SUCCESS;
//					}
//					else
//					{
//						edge_.sector_map[min_idx] = max(edge_.sector_map[min_idx], tmpmin);
//						loc_last_idx_ = 0;
//						loc_curr_idx_ = 0;
//						return EXIT_SUCCESS;
//					}
//				}
//
//				edge_.sector_map[min_idx] = max(edge_.sector_map[min_idx], tmpmin);
//
//				if ((min_idx/SEC_INT) - loc_curr_idx_ > 1)
//				{
//					for(int l=loc_curr_idx_+1;l<(min_idx/SEC_INT);l++)
//					{
//						decideSectorIntervalExt(
//								edge_, points_[1], delta_t_zero, sec_idx, l);
//						int tmp_idx = l*SEC_INT + sec_idx;
//						edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
//					}
//					loc_curr_idx_ = (min_idx/SEC_INT);
//				}
//				else if ((min_idx/SEC_INT) - loc_curr_idx_ == 1)
//				{
//					loc_curr_idx_ = (min_idx/SEC_INT);
//				}
////				else if ((min_idx/SEC_INT) - loc_curr_idx_ < 0) // if it goes backward
////				{
////					for(int l=(min_idx/SEC_INT)+1;l<loc_curr_idx_;l++)
////					{
////						decideSectorIntervalExt(
////								edge_, points_[1], delta_t_zero, sec_idx, l);
////						int tmp_idx = l*SEC_INT + sec_idx;
////						edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
////					}
////					loc_curr_idx_ = (min_idx/SEC_INT);
////				}
//				loc_last_idx_ = loc_curr_idx_;
//				return EXIT_SUCCESS;
//			}
//
//			loc_last_idx_ = loc_last_idx_mem;
//			return EXIT_FAILURE;

			break;
		}
		case 2:
		{
			int loc_idx;

			double tmpdlie =
					dLIE(
							edge_, points_[1], loc_idx, loc_last_idx_,
							loc_offset_, loc_init_);

			if (tmpdlie > 0.001)
			{
				if(mem || loc_last_idx_==LOC_INT-1)
				{
					loc_last_idx_ = loc_last_idx_mem;
					return EXIT_FAILURE;
				}
				else
				{
					loc_last_idx_ = loc_last_idx_mem;
					return EXIT_FAILURE;
				}
			}

			// to fill up the line if loc_int is valid but tangent is not aligned to traj (at a curve)
			double tmpmin = 10.0;
			double tmpdeltatmin = 0.0;
			int min_idx = 0;

			decideSectorIntervalExt(
					edge_, points_[1], delta_t_zero, sec_idx, loc_idx);
			int tmp_idx = loc_idx*SEC_INT + sec_idx;
			edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));


			if (loc_idx>10) //offset because we shift back 10 at beginning
			{
				for(int l=loc_last_idx_+10;l<loc_idx;l++)
				{
					decideSectorIntervalExt(
							edge_, points_[1], delta_t_zero, sec_idx, l);
					int tmp_idx = l*SEC_INT + sec_idx;
					edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
				}
			}

//			if (loc_idx==loc_last_idx_)
//			{
//				decideSectorIntervalExt(
//						edge_, points_[1], delta_t_zero, sec_idx, loc_idx);
//				int tmp_idx = loc_idx*SEC_INT + sec_idx;
//				edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
//			}
//			else
//			{
//				for(int l=loc_curr_idx_+1;l<loc_idx+1;l++)
//				{
//					decideSectorIntervalExt(
//							edge_, points_[1], delta_t_zero, sec_idx, l);
//					int tmp_idx = l*SEC_INT + sec_idx;
//					edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
//				}
//			}

			loc_curr_idx_ = (min_idx/SEC_INT);
			loc_last_idx_ = loc_curr_idx_;

			return EXIT_SUCCESS;

			break;
		}

//		{
//			vector<int> loc_idxs;
//			loc_init_ = false;
//			if (decideLocationIntervalExt(edge_, points_[1],
//					loc_idxs, loc_last_idx_, loc_offset_, loc_init_)>0.001 &&
//				loc_idxs.back()==LOC_INT-1)
//			{
//				loc_last_idx_ = loc_last_idx_mem;
//				return EXIT_FAILURE;
//			}
//			for(int ll=0;ll<loc_idxs.size();ll++)
//			{
//				decideSectorIntervalExt(
//						edge_, points_[1], delta_t, sec_idx, loc_idxs[ll]);
//				edge_.sector_map[loc_idxs[ll]*SEC_INT + sec_idx] =
//						max(
//								edge_.sector_map[loc_idxs[ll]*SEC_INT + sec_idx],
//								l2Norm(delta_t));
//			}
//			break;
//		}
		case 3:
		{
			decideLocationIntervalExt(
					edge_, points_[1], loc_idx, loc_last_idx_, loc_offset_,
					loc_init_);
			decideSectorIntervalExt(
					edge_, points_[1], delta_t, sec_idx, loc_idx);
			edge_.sector_map[loc_idx*SEC_INT + sec_idx] =
					max(
							edge_.sector_map[loc_idx*SEC_INT + sec_idx],
							l2Norm(delta_t));
			break;
		}
	}
	return EXIT_SUCCESS;
}

int adjustCurve(
	edge_tt &edge_,
	vector<point_d> &points_,
	int &loc_last_idx_,
	bool &loc_init_,
	int loc_offset_)
{
	point_d delta_t_zero;
	int sec_idx 			= -1;
	int loc_idx 			= -1;
	int loc_last_idx_mem 	= loc_last_idx_;
	bool mem 				= loc_init_;

	double tmpdlie =
			dLIE(
					edge_, points_[1], loc_idx, loc_last_idx_,
					loc_offset_, loc_init_);

	if (tmpdlie > 0.001)
	{
		if(mem || loc_last_idx_==LOC_INT-1)
		{
			loc_last_idx_ = loc_last_idx_mem;
			return EXIT_FAILURE;
		}
		else
		{
			loc_last_idx_ = loc_last_idx_mem;
			return EXIT_FAILURE;
		}
	}

	// to fill up the line if loc_int is valid but tangent is not aligned to traj (at a curve)

	decideSectorIntervalExt(
			edge_, points_[1], delta_t_zero, sec_idx, loc_idx);
	int tmp_idx = loc_idx*SEC_INT + sec_idx;
	edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));


	if (loc_idx>10) //offset because we shift back 10 at beginning
	{
		for(int l=loc_last_idx_+10;l<loc_idx;l++)
		{
			decideSectorIntervalExt(
					edge_, points_[1], delta_t_zero, sec_idx, l);
			int tmp_idx = l*SEC_INT + sec_idx;
			edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
		}
	}

	loc_last_idx_ = loc_idx;

	return EXIT_SUCCESS;
}

int adjustCurveExt(
	Graph *Graph_,
	vector<point_d> coeffs_,
	int integral_limit_,
	int label1_,
	int label2_)
{
	double cx[DEGREE], cy[DEGREE], cz[DEGREE];
	double N 				= Graph_->getEdgeCounter(label1_,label2_,0);
	double total_len 		= 0;
	edge_tt edge_tmp 		= Graph_->getEdge(label1_, label2_, 0);
	edge_tt edge_tmp_mem	= edge_tmp;

	// [CURVE FIT]*************************************************************
	polyCurveLength(total_len, 0, integral_limit_, coeffs_);
	edge_tmp.total_len = total_len;
	for(int i=0;i<DEGREE;i++)
	{
		cx[i] = coeffs_[i].x;
		cy[i] = coeffs_[i].y;
		cz[i] = coeffs_[i].z;
	}
	int mem = 0;
	for(int i=0;i<LOC_INT;i++)
	{
		double lim[3], len[3];
		lim[0] = lim[1] = lim[2] = -1.0;
		len[0] =  (total_len/LOC_INT)* i;
		len[1] = ((total_len/LOC_INT)* i + (total_len/LOC_INT)*0.5);
		len[2] =  (total_len/LOC_INT)*(i+1);
		// ### HACK: resample points along curve and cal length.
		for(int ii=mem;ii<integral_limit_*100+1;ii++)
		{
			double tl = 0.0;
			double t  = (double)ii/100.0;
			polyCurveLength(tl, 0.0, t, coeffs_);
			if      (tl>=len[0] && lim[0]<0.0) {lim[0] = t;}
			else if (tl>=len[1] && lim[1]<0.0) {lim[1] = t;}
			else if (tl>=len[2] && lim[2]<0.0) {lim[2] = t; mem = ii; break;}
		}

		// Checking length values
		if(0)
		{
			double tmplen;
			polyCurveLength(tmplen, 0.0, lim[0], coeffs_);
			printf("length : %.4f %.4f %.4f ", len[0], tmplen, lim[0]);
			polyCurveLength(tmplen, 0.0, lim[1], coeffs_);
			printf("length : %.4f %.4f %.4f ", len[1], tmplen, lim[1]);
			polyCurveLength(tmplen, 0.0, lim[2], coeffs_);
			printf("length : %.4f %.4f %.4f \n", len[2], tmplen, lim[2]);
		}

		if (lim[2]<0) {lim[2] = integral_limit_;}

		point_d p_mid, p_tan, p_nor;
		p_mid.x = gsl_poly_eval (cx, DEGREE, lim[1]);
		p_mid.y = gsl_poly_eval (cy, DEGREE, lim[1]);
		p_mid.z = gsl_poly_eval (cz, DEGREE, lim[1]);

		edge_tmp.loc_mid  [i] = p_mid;
		edge_tmp.loc_start[i] = addPoint(p_mid , multiPoint(p_tan, lim[0]-lim[1]));
		edge_tmp.loc_end  [i] = addPoint(p_mid , multiPoint(p_tan, lim[2]-lim[1]));

		// N : counter
		if (N==0)
		{
			// at location interval 0
			if (i==0)
			{
				cal_tangent_normal(lim[1], p_tan, p_nor, coeffs_, DEGREE, true);
				edge_tmp.nor  	  [i] = multiPoint(p_nor , 1/l2Norm(p_nor));
				edge_tmp.tan  	  [i] = multiPoint(p_tan , 1/l2Norm(p_tan));
			}
			// rotates the normal at location interval 0
			else
			{
				cal_tangent_normal(lim[1], p_tan, p_nor, coeffs_, DEGREE, false);
				edge_tmp.tan  	  [i] = multiPoint(p_tan , 1/l2Norm(p_tan));
				vector<double> tmpRTI =
						transInv(
								rodriguezRot(
										edge_tmp.tan[0],
										edge_tmp.tan[i]));
				edge_tmp.nor[i].x =
						tmpRTI[0]*edge_tmp.nor[0].x +
						tmpRTI[1]*edge_tmp.nor[0].y +
						tmpRTI[2]*edge_tmp.nor[0].z;
				edge_tmp.nor[i].y =
						tmpRTI[3]*edge_tmp.nor[0].x +
						tmpRTI[4]*edge_tmp.nor[0].y +
						tmpRTI[5]*edge_tmp.nor[0].z;
				edge_tmp.nor[i].z =
						tmpRTI[6]*edge_tmp.nor[0].x +
						tmpRTI[7]*edge_tmp.nor[0].y +
						tmpRTI[8]*edge_tmp.nor[0].z;
			}
		}
		else
		{
			cal_tangent_normal(lim[1], p_tan, p_nor, coeffs_, DEGREE, false);
			edge_tmp.nor  	  [i] = multiPoint(p_nor , 1/l2Norm(p_nor));
			edge_tmp.tan  	  [i] = multiPoint(p_tan , 1/l2Norm(p_tan));
		}
	}
	// *************************************************************[CURVE FIT]

	// [ADJUSTMENT]************************************************************
	if (N>0)
	{
		// [AVERAGE]***********************************************************
		for(int l=0;l<LOC_INT;l++)
		{
			edge_tmp.loc_start[l] =
					addPoint(
							multiPoint(edge_tmp_mem.loc_start[l], N/(N+1)),
							multiPoint(edge_tmp.loc_start[l]	, 1/(N+1)));
			edge_tmp.loc_mid[l]	=
					addPoint(
							multiPoint(edge_tmp_mem.loc_mid[l]	, N/(N+1)),
							multiPoint(edge_tmp.loc_mid[l]		, 1/(N+1)));
			edge_tmp.loc_end[l]	=
					addPoint(
							multiPoint(edge_tmp_mem.loc_end[l]	, N/(N+1)),
							multiPoint(edge_tmp.loc_end[l] 	  	, 1/(N+1)));
			edge_tmp.tan[l] =
					addPoint(
							multiPoint(edge_tmp_mem.tan[l] 		, N/(N+1)),
							multiPoint(edge_tmp.tan[l] 	  	  	, 1/(N+1)));
			edge_tmp.tan[l]	=
					multiPoint(edge_tmp.tan[l], 1/l2Norm(edge_tmp.tan[l]));
			vector<double> tmpRTI =
					transInv(
							rodriguezRot(
									edge_tmp_mem.tan[l],
									edge_tmp.tan[l]));
			edge_tmp.nor[l].x =
					tmpRTI[0]*edge_tmp_mem.nor[l].x +
					tmpRTI[1]*edge_tmp_mem.nor[l].y +
					tmpRTI[2]*edge_tmp_mem.nor[l].z;
			edge_tmp.nor[l].y =
					tmpRTI[3]*edge_tmp_mem.nor[l].x +
					tmpRTI[4]*edge_tmp_mem.nor[l].y +
					tmpRTI[5]*edge_tmp_mem.nor[l].z;
			edge_tmp.nor[l].z =
					tmpRTI[6]*edge_tmp_mem.nor[l].x +
					tmpRTI[7]*edge_tmp_mem.nor[l].y +
					tmpRTI[8]*edge_tmp_mem.nor[l].z;
		}
		// ***********************************************************[AVERAGE]
		// [SECTOR MAP]********************************************************
		reshapeVector(edge_tmp.sector_map, LOC_INT*SEC_INT);
		vector<double> delta_t_mem; delta_t_mem.resize(3);

		for(int l=0;l<LOC_INT;l++)
		{
			for(int s=0;s<SEC_INT;s++)
			{
				vector<int> ind_loc;
				point_d tmpN, p_old[3];
				// [OLD POINT]*************************************************
				tmpN = rodriguezVec(
								2*M_PI*fmod((s+0.5),(double)SEC_INT)/SEC_INT,
								edge_tmp_mem.tan[l],
								edge_tmp_mem.nor[l]);
				p_old[0] =
						addPoint(
								edge_tmp_mem.loc_start[l],
								multiPoint(
										tmpN,
										edge_tmp_mem.sector_map[l*SEC_INT+s]));
				p_old[1] =
						addPoint(
								edge_tmp_mem.loc_mid[l],
								multiPoint(
										tmpN,
										edge_tmp_mem.sector_map[l*SEC_INT+s]));
				p_old[2] =
						addPoint(
								edge_tmp_mem.loc_end[l],
								multiPoint(
										tmpN,
										edge_tmp_mem.sector_map[l*SEC_INT+s]));
				// *************************************************[OLD POINT]
				for(int ii=2;ii<3;ii++)
				{
					vector<point_d> points_tmp = {p_old[ii],p_old[ii]};
					bool tmp_init = false;
					int last, curr; //##TODO
					if (l<10) 	{ last = 0; 	curr = 0; }
					else 		{ last = l-10; 	curr = l-10; }
					adjustCurve(edge_tmp, points_tmp, last, tmp_init, 20);
				}
			} //s

			//for(int i=1;i<3;i++) { if (loc_last[i-1]>loc_last[i]) { init[i]	= init[i-1]; loc_last[i] = loc_last[i-1]; } }

		} //l
		// ********************************************************[SECTOR MAP]
	}
	// ************************************************************[ADJUSTMENT]
	Graph_->setEdge(label1_, label2_, 0, edge_tmp);
	printer(19);
	return EXIT_SUCCESS;
}

int fitSectorMap(
	edge_tt &edge_,
	vector<point_d> &points_,
	int &loc_last_idx_,
	int loc_offset_,
	bool &loc_init_)
{
	int sec_idx 			= -1;
	int loc_idx 			= -1;
	int loc_last_idx_mem 	= loc_last_idx_;
	bool mem 				= loc_init_;
	point_d delta_t, delta_t_zero;

	// to deal with cases where the beginning is out of the sectormap
	// required for the fitting part only
	// also for the ending

	if (dLIE(edge_, points_[1],
			loc_idx, loc_last_idx_, loc_offset_, loc_init_) > 0.001)
	{
		if(mem || loc_last_idx_==LOC_INT-1)
		{
			loc_last_idx_ = loc_last_idx_mem;
			return EXIT_SUCCESS; //#TODO should be failure
		}
		else
		{
			loc_last_idx_ = loc_last_idx_mem;
			return EXIT_SUCCESS; //#TODO should be failure
		}
	}

	// find the delta_t
	decideSectorIntervalExt(
			edge_, points_[1], delta_t, sec_idx, loc_idx);


	// to fill up the line if loc_int is valid but tangent is not aligned to traj (at a curve)
	double min_dist 	= 10.0;
	double delta_t_min	= 0.0;
	int min_idx 		= 0;

	if (loc_idx==loc_last_idx_)
	{
		min_idx		= loc_idx*SEC_INT + sec_idx;
		min_dist	= l2Norm(minusPoint(points_[1], edge_.loc_mid[loc_idx]));
		delta_t_min	= l2Norm(delta_t);
	}
	else if (loc_idx>loc_last_idx_)
	{
		for(int ll=loc_last_idx_+1;ll<loc_idx+1;ll++)
		{
			decideSectorIntervalExt(
					edge_, points_[1], delta_t_zero, sec_idx, ll);
			int tmp_idx = ll*SEC_INT + sec_idx;
			if (min_(l2Norm(minusPoint(points_[1], edge_.loc_mid[ll])),
					 min_dist))
			{
				min_dist	= l2Norm(minusPoint(points_[1], edge_.loc_mid[ll]));
				delta_t_min	= l2Norm(delta_t_zero);
				min_idx 	= tmp_idx;
			}
		}
	}
	//else
	// we do not consider going back for now

	if (loc_init_)
	{
		// if point starts before curve
		point_d proj_dir_tmp =
				multiPoint(
						edge_.tan[0],
						dotProduct(
								point2vector(
										minusPoint(
												points_[1],edge_.loc_mid[0])),
								point2vector(edge_.tan[0])));
		if (!vectorDirectionCheck(
				point2vector(proj_dir_tmp),
				point2vector(edge_.tan[0])))
		{
			loc_init_ = true;
			loc_last_idx_ = 0;
			return EXIT_SUCCESS;
		}
		else
		{
			loc_init_ = false;
			edge_.sector_map[min_idx] =
					max(edge_.sector_map[min_idx], delta_t_min);
			if (edge_.sector_map[min_idx]>0.3)  {return EXIT_FAILURE;}
			loc_last_idx_ = min_idx/SEC_INT;
			return EXIT_SUCCESS;
		}
	}

	edge_.sector_map[min_idx] = max(edge_.sector_map[min_idx], delta_t_min);
	if (edge_.sector_map[min_idx]>0.3)  {return EXIT_FAILURE;}

	if ((min_idx/SEC_INT) - loc_last_idx_ > 1)
	{
		for(int l=loc_last_idx_+1;l<(min_idx/SEC_INT);l++)
		{
			decideSectorIntervalExt(
					edge_, points_[1], delta_t_zero, sec_idx, l);
			int tmp_idx = l*SEC_INT + sec_idx;
			edge_.sector_map[tmp_idx] = max(edge_.sector_map[tmp_idx], l2Norm(delta_t_zero));
			if (edge_.sector_map[min_idx]>0.3)  {return EXIT_FAILURE;}
		}
		loc_last_idx_ = (min_idx/SEC_INT);
	}
	else if ((min_idx/SEC_INT) - loc_last_idx_ == 1)
	{
		loc_last_idx_ = (min_idx/SEC_INT);
	}
	//else
	// we do not consider going back for now

	//cout << edge_.sector_map[min_idx] << endl; 0.449658

	return EXIT_SUCCESS;

}

int fitSectorMapInit(
	Graph *Graph_,
	vector<point_d> &points_,
	int label1_,
	int label2_,
	int loc_offset_)
{
	int loc_last_idx = 0;
	int loc_curr_idx = 0;
	double delta_t_mem = 0.0;
	bool init	= true;
	int offset	= loc_offset_;
	for(int i=1;i<points_.size();i++)
	{
		edge_tt edge_tmp = Graph_->getEdge(label1_, label2_, 0);
		vector<point_d> points_tmp {points_[i-1], points_[i]};
		if (init)	{offset = LOC_INT;}
		else		{offset = loc_offset_;}
		adjustSectorMap(
				edge_tmp,
				points_tmp,
				loc_last_idx,
				loc_curr_idx,
				delta_t_mem,
				init,
				offset,
				1);
		Graph_->setEdge(label1_, label2_, 0, edge_tmp);
	}
	printer(22);
	return EXIT_SUCCESS;
}

int fitSectorMapExt(
	Graph *Graph_,
	vector<point_d> &points_,
	int label1_,
	int label2_,
	int loc_offset_)
{
	edge_tt edge_tmp = Graph_->getEdge(label1_, label2_, 0);

	// add the option to check if the curve is too much
	// how to handle with it ??? ####
	if(0)
	{
		vector<point_d> curve_mem;
		vector<double> x, curve;  curve.resize(points_.size());
		for(int i=0;i<edge_tmp.loc_mid.size();i++)
		{
			decideCurvature(edge_tmp.loc_mid[i], curve_mem, curve[i], 3);
			x.push_back(i);
		}
		plotData(x,curve);
	}

	bool flag_init		= true;
	int offset			= loc_offset_;
	int loc_last_idx	= 0;
	int res				= -1;
	for(int i=1;i<points_.size();i++)
	{
//		edge_tmp = Graph_->getEdge(label1_, label2_, 0);

		if (flag_init)	{offset = LOC_INT;}
		else			{offset = loc_offset_;}

//		// this helps to prevent if the trajectory begin is a curve
//		if (
//				i==0 &&
//				dotProduct(
//						point2vector(edge_tmp.tan[0]),
//						point2vector(
//								multiPoint(
//										minusPoint(
//												points_[1],
//												points_[0]),
//										1/l2Norm(
//												minusPoint(
//														points_[1],
//														points_[0])))))<0.5)
//		{loc_last_idx = 4; loc_last_init = 4;}


		vector<point_d> points_tmp {points_[i-1], points_[i]};

		res =
				fitSectorMap(
						edge_tmp, points_tmp, loc_last_idx, offset, flag_init);

		if (res==EXIT_FAILURE) {printer(31); return EXIT_FAILURE;}

//		Graph_->setEdge(label1_, label2_, 0, edge_tmp);

		vector<point_d> points_tmp2(points_.begin(),points_.begin()+i);

		//VISUALIZE
		if(0)
		{
			vector<point_d> point_zero; vector<string> label_zero;
			vector<vector<unsigned char> > color_code; colorCode(color_code);
			showConnection(Graph_, points_tmp2, label_zero, color_code, true);
		}
	}

	Graph_->setEdge(label1_, label2_, 0, edge_tmp);

	printer(20);
	return EXIT_SUCCESS;
}

int findSectorMapConstraint(
	Graph *Graph_,
	int label1_,
	int label2_)
{
	edge_tt edge_tmp = Graph_->getEdge(label1_, label2_, 0);
	if (edge_tmp.sector_const.empty())
	{
		edge_tmp.sector_const.resize(LOC_INT*SEC_INT);
	}
	for(int i=0;i<SEC_INT*LOC_INT;i++)
	{
		edge_tmp.sector_const[i] =
				edge_tmp.sector_map[i] > MAX_RANGE ?
						edge_tmp.sector_map[i] : 0;
	}
	Graph_->setEdge(label1_, label2_, 0, edge_tmp);
	printer(21);
	return EXIT_SUCCESS;
}

int updateSectorMap(
	Graph *Graph_,
	vector<point_d> points_avg_,
	int label1_,
	int label2_)
{
	// fitCurve()				: Fit the curve based on pure trajectory points.
	//							: Estimate points that are used to do fitting later.
	// adjustCurveExt()			: Obtain the tan and nor from the estimated points. (only for the first run)
	//							: Adjust the tan and nor from the estimated points.
	// fitSectorMapInit()		: Fit the estimated points to the sector map. (only for the first run)
	// fitSectorMapExt()			: Fit the estimated points to the sector map.
	// findSectorMapConstraint(): Check the constraints of  the sector map.
	// Graph_.setEdgeCounter()	: Increment the counter.

	vector<point_d> points_est, coeffs;
	int res = -1;

	if (Graph_->getEdgeCounter(label1_,label2_,0) == 0)
	{
		fitCurve(points_avg_, points_est, coeffs);
		adjustCurveExt(Graph_, coeffs, points_avg_.size(), label1_, label2_);
		fitSectorMapInit(Graph_, points_est, label1_, label2_, 10);
		findSectorMapConstraint(Graph_, label1_, label2_);
		Graph_->setEdgeCounter(label1_, label2_, 0, 1);
	}
	else if (Graph_->getEdgeCounter(label1_,label2_,0) < 50)
	{
		fitCurve(points_avg_, points_est, coeffs);
		res = fitSectorMapExt(Graph_, points_est, label1_, label2_, 20);
		if (res==EXIT_FAILURE) {return EXIT_FAILURE;}
		//VISUALIZE
		if(1)
		{
			vector<point_d> point_zero; vector<string> label_zero;
			vector<vector<unsigned char> > color_code; colorCode(color_code);
			showConnection(Graph_, points_est, label_zero, color_code, true);
		}
		adjustCurveExt(Graph_, coeffs, points_avg_.size(), label1_, label2_);
		findSectorMapConstraint(Graph_, label1_, label2_);
		Graph_->setEdgeCounter(label1_, label2_, 0, 1);
	}
	else
	{
		fitCurve(points_avg_, points_est, coeffs);
		res = fitSectorMapExt(Graph_, points_est, label1_, label2_, 20);
		if (res==EXIT_FAILURE) {return EXIT_FAILURE;}
		findSectorMapConstraint(Graph_, label1_, label2_);
		Graph_->setEdgeCounter(label1_, label2_, 0, 1);
	}

	//VISUALIZE
	if(1)
	{
		vector<point_d> point_zero; vector<string> label_zero;
		vector<vector<unsigned char> > color_code; colorCode(color_code);
		showConnection(Graph_, points_est, label_zero, color_code, true);
	}

	return EXIT_SUCCESS;
}

int findWindowConstraint(
	Graph *Graph_,
	int label1_,
	int label2_)
{
//	edge_tt edge_tmp = Graph_.getEdge(label1_, label2_, 0);
//	if (Graph_.getEdgeCounter(label1_, label2_, 0)>2)
//	{
//		vector<double> x,y,y_mem;
//		vector<double> ddt_tmp,x_tmp;
//
//		for(int l=0;l<LOC_INT;l++)
//		{
//			double max_val = 0.0;
//			for(int s=0;s<SEC_INT/2;s++)
//			{
//				double tmp =
//						edge_tmp.sector_map[l*SEC_INT+s] +
//						edge_tmp.sector_map[l*SEC_INT+s+SEC_INT/2];
//				max_val = max(tmp,max_val);
//			}
//			if (max_val<0.02) // TODO: the min constraint
//			{
//				vector<int> c;
//				Graph_.getEdgeMovementConstraint(label1_, label2_, 0, c);
//				c[MOV_CONST_CIRC] = 1;
//				Graph_.setEdgeMovementConstraint(label1_, label2_, 0, c);
//			}
//
////			x.push_back(l);
////			y_mem.push_back(max_val);
////			if (y_mem.size()<3)
////				y.push_back(max_val);
////			else
////				y.push_back(movingAverage(max_val, y_mem));
//		}
//
////		for(int n=2;n<y.size();n++)
////		{
////			x_tmp.push_back(n-2);
////			ddt_tmp.push_back(
////					y[n] -
////					y[n-1] -
////					y[n-1] +
////					y[n-2]);
////		}
////		plotData(x,y);
////		plotData(x_tmp,ddt_tmp);
//
//	}

	return EXIT_SUCCESS;
}

int findSectorMapChangeRate(
	Graph &Graph_,
	int label1_,
	int label2_)
{
	edge_tt edge_tmp = Graph_.getEdge(label1_, label2_, 0);
	if (Graph_.getEdgeCounter(label1_, label2_, 0)>2)
	{
		vector<double> diff_tmp; diff_tmp.resize(LOC_INT);
		for(int l=0;l<LOC_INT;l++)
		{
			double a = 0.0;
			for(int s=0;s<SEC_INT/2;s++)
			{
				double tmp =
						edge_tmp.sector_map[l*SEC_INT+s] +
						edge_tmp.sector_map[l*SEC_INT+s+SEC_INT/2];
				a = max(tmp, a);
			}
			diff_tmp[l] = a;
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

	}

	return EXIT_SUCCESS;
}

int buildSectorMap(
	Graph 						*Graph_,
	vector<vector<point_d> > 	pva_avg_,
	vector<int> 				contact_)
{
	// Graph_.getEdgeList() = [#loc*#loc -> #edges -> #loc*#sec]

	vector<point_d> pts_avg, vel_avg;
	for(int i=0;i<pva_avg_.size();i++)
	{
		pts_avg.push_back(pva_avg_[i][0]);
		vel_avg.push_back(pva_avg_[i][1]);
	}

	// label1	: start location
	// label2	: goal location
	// label_idx: index of data point for label1
	int label1, label2, label_idx;
	label1 = label2 = label_idx = -1;

	int res = -1;

	for(int i=0;i<pts_avg.size();i++)
	{
		if (pts_avg[i].l >= 0)
		{
			// Initial location
			if	(label1 < 0) { label1 = pts_avg[i].l; continue; }
			else
			{
				// Check if location has changed
				if (label_idx > 0)
				{
					label2 = pts_avg[i].l;
					if (label2==label1)	{ continue; }
					if (i - label_idx > 5) // to prevent only a few points evaluated for curve, might not need this ###
					{
						vector<point_d> pts_avg_tmp(
								pts_avg.begin()+label_idx,
								pts_avg.begin()+i);
						vector<point_d> vel_avg_tmp(
								vel_avg.begin()+label_idx,
								vel_avg.begin()+i);

						findMovementConstraint(
								Graph_,
								pts_avg_tmp,
								vel_avg_tmp,
								label1,
								label2);
						updateSectorMap(
								Graph_,
								pts_avg_tmp,
								label1,
								label2);
						findWindowConstraint(
								Graph_,
								label1,
								label2);

//						//VISUALIZE
//						if(Graph_->getEdgeCounter(label1, label2, 0)==10 && label1==0 && label2==1)
//						{
//							vector<point_d> point_zero; vector<string> label_zero;
//							vector<vector<unsigned char> > color_code; colorCode(color_code);
//							showConnection(Graph_, pts_avg_tmp, label_zero, color_code, true);
//						}

					}

					label1 		= label2;
					label2 		= -1;
					label_idx 	= -1;
				}
			}
		}
		// saves the data number of initial location
		else { if (label1 >=0 && label_idx < 0) { label_idx = i; } }
	}
	return EXIT_SUCCESS;
}
