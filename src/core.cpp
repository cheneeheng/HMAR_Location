/*
 * core.cpp
 *
 *  Created on: Mar 26, 2017
 *      Author: chen
 */

#include "core.h"

bool directionCheck(
	Vector3d A,
	Vector3d B)
{
	for(int i=0;i<3;i++)
		if (((A(i) >= 0) && (B(i) < 0)) || ((A(i) < 0) && (B(i) >= 0)))
			return false;
	return true;
}

//int decideBoundary(
//	point_d 		&point1_,
//	point_d 		&point2_,
//	vector<point_d> centroids_)
//{
//	point2_.l = UNCLASSIFIED;
//	for(int ii=0;ii<centroids_.size();ii++)
//	{
//		double location_contact = 0.0;
//		point_d tmp_diff = minusPoint(point2_, centroids_[ii]);
//		if (
//				max_(
//						pdfExp(BOUNDARY_VAR, 0.0, l2Norm(tmp_diff)),
//						location_contact ))
//		{
//			location_contact = pdfExp(BOUNDARY_VAR, 0.0, l2Norm(tmp_diff));
//			if (max_(location_contact, centroids_[ii].l))
//			{
////				double tmpvel = l2Norm(minusPoint(point2_,point1_));
////				if (tmpvel > 0.005) continue;
//				point2_.l = ii;
//			}
//		}
//	}
//	return EXIT_SUCCESS;
//}

//int decideBoundary_(
//	point_d 		&point1_,
//	point_d 		&point2_,
//	vector<point_d> centroids_,
//	vector<int>		surfaces_flag_,
//	vector<point_d> surfaces_,
//	vector<double>	surfaces_limit)
//{
//	point2_.l = UNCLASSIFIED;
//
//	for(int ii=0;ii<centroids_.size();ii++)
//	{
//		point_d tmp_diff = minusPoint(point2_, centroids_[ii]);
//		double location_contact = pdfExp(BOUNDARY_VAR, 0.0, l2Norm(tmp_diff));
//		double boundary = sqrt(-log(centroids_[ii].l)*BOUNDARY_VAR*2);
//		if (max_(location_contact, centroids_[ii].l)) //|| pdfExp(0.005,0.0,(l2Norm(tmp_diff)-boundary))>0.75)
//		{
//			point2_.l = ii;
//
//			if ( surfaces_flag_[point2_.l] > 0)
//			{
//				if (!decideSurface(
//						point2_,
//						surfaces_[surfaces_flag_[point2_.l]-1],
//						surfaces_limit[surfaces_flag_[point2_.l]-1]))
//				{
//					point2_.l = UNCLASSIFIED;
//				}
//			}
//		}
//	}
//	return EXIT_SUCCESS;
//}

int checkBoundarySphere(
	Vector4d 		 &point_,
	vector<Vector4d> centroids_)
{
	point_[3] = UNCLASSIFIED;
	for(int ii=0;ii<centroids_.size();ii++)
	{
		double location_contact =
				pdfExp(BOUNDARY_VAR, 0.0, V4d3d(point_-centroids_[ii]).norm());
//		double boundary = sqrt(-log(centroids_[ii].l)*BOUNDARY_VAR*2);
		if (max_(location_contact, (double)centroids_[ii][3])) //|| pdfExp(0.005,0.0,(l2Norm(tmp_diff)-boundary))>0.75)
		{ point_[3] = ii; }
	}
	return EXIT_SUCCESS;
}

int checkBoundaryCuboid(
	Vector4d &point_,
	Vector3d box_min_,
	Vector3d box_max_)
{
	if(	point_[0] < box_max_[0] &&
		point_[1] < box_max_[1] &&
		point_[2] < box_max_[2] &&
		point_[0] > box_min_[0] &&
		point_[1] > box_min_[1] &&
		point_[2] > box_min_[2] )
	{
		return EXIT_SUCCESS;
	}
	else
	{
		point_[3] = UNCLASSIFIED;
		return EXIT_FAILURE;
	}
}

//int decideBoundaryPredict(
//	point_d 		&point1_,
//	point_d 		&point2_,
//	vector<point_d> centroids_,
//	vector<int>		surfaces_flag_,
//	vector<point_d> surfaces_,
//	vector<double>	surfaces_limit)
//{
//	point2_.l = UNCLASSIFIED;
//	for(int ii=0;ii<centroids_.size();ii++)
//	{
//		point_d tmp_diff = minusPoint(point2_, centroids_[ii]);
//		double location_contact = pdfExp(BOUNDARY_VAR, 0.0, l2Norm(tmp_diff));
//		double boundary = sqrt(-log(centroids_[ii].l)*BOUNDARY_VAR*2);
//		if (max_(location_contact, centroids_[ii].l))// || pdfExp(0.005,0.0,(l2Norm(tmp_diff)-boundary))>0.8)
//		{
//			point2_.l = ii;
//			if (surfaces_flag_[point2_.l]>0)
//			{
//				if (!decideSurface(
//						point2_,
//						surfaces_[surfaces_flag_[point2_.l]-1],
//						surfaces_limit[surfaces_flag_[point2_.l]-1]))
//				{
//					point2_.l = UNCLASSIFIED;
//				}
//			}
//		}
//	}
//	return EXIT_SUCCESS;
//}

int decideBoundarySphere(
	Vector4d 		 &point_,
	vector<Vector4d> centroids_)
{
	return checkBoundarySphere(point_, centroids_);
}

int decideBoundaryCuboid(
	Vector4d &point_,
	Vector3d box_min_,
	Vector3d box_max_)
{
	return checkBoundaryCuboid(point_, box_min_, box_max_);
}

int decideBoundaryClosest(
	Vector4d 		&point_,
	vector<Vector4d> centroids_)
{
	point_[3] = UNCLASSIFIED;
	vector<double> tmp;
	for(int ii=0;ii<centroids_.size();ii++)
	{
		tmp.push_back(
				pdfExp(BOUNDARY_VAR, 0.0, V4d3d(point_-centroids_[ii]).norm()));
	}
	point_[3] =
			distance(
					tmp.begin(),
					max_element(tmp.begin(), tmp.end()));
	return EXIT_SUCCESS;
}

double checkSurfaceDistance(
	Vector4d centroids_,
	Vector4d surface_eq_)
{
	return 	centroids_[0] * surface_eq_[0] +
			centroids_[1] * surface_eq_[1] +
			centroids_[2] * surface_eq_[2] - surface_eq_[3];
}

bool decideSurface(
	Vector4d centroids_,
	Vector4d surface_eq_,
	double 	limit_)
{
	if(fabs(checkSurfaceDistance(centroids_,surface_eq_))<limit_)
	{ return true; }
	else
	{ return false; }
}

double dLI(
	int &loc_idx_,
	int loc_last_idx_,
	Vector4d point_,
	vector<double> len_,
	vector<Vector4d> mid_,
	vector<Vector3d> tangent_,
	int loc_offset_,
	bool loc_init_)
{
	int idx_tmp;
	double d1,d2,d3,d4,d5,d6,d6min,d6min2; d1=d2=d3=d4=d5=d6=0.0; d6min = 10;
	Vector3d proj_dir_tmp, beg, mid, end;

	// Added an offset buffer to prevent the location prediction from jumping too much
	int idx1 = ( loc_last_idx_ < 0 ? 0 : loc_last_idx_ );
	int idx2 = ( idx1+loc_offset_ > LOC_INT ? LOC_INT : idx1+loc_offset_ );
	for(int l=idx1;l<idx2;l++)
	{
		mid = V4d3d(mid_[l])* mid_[l][3];
		beg = mid - (tangent_[l] * (len_[l]/2));
		end = mid + (tangent_[l] * (len_[l]/2));
		proj_dir_tmp = tangent_[l] * ((V4d3d(point_)-mid).dot(tangent_[l]));

		d1 = (beg-mid).norm();
		d2 = (end-mid).norm();
		d3 = (beg-(mid+proj_dir_tmp)).norm();
		d4 = (end-(mid+proj_dir_tmp)).norm();
		d5 = (beg-end).norm();

		if (directionCheck(proj_dir_tmp, tangent_[l]))
		{
			if (l == idx1)	{d6min2 = d3-d5; d6 = d6min2;}
			if (d4<=d2 && (d3-d5) < 0.01) //### TODO small error deviation (deadzone)
			{
				d6 = d3-d5;
			}
		}
		else
		{
			if (l == idx1)	{d6min2 = d4-d5; d6 = d6min2;}
			if (d3<=d1 && (d4-d5) < 0.01)
			{
				d6 = d4-d5;
			}
		}

		// ignore the first loc_int so that we don't get "stuck" at curves
		if(l==idx1) continue;

		if (min_(d6,d6min))
		{
			d6min = d6;
			idx_tmp = l;
		}
		else
		{
			// breaks only when the location is larger than the current one
			if(d6min < 0.0001 && l>idx1) // inside
			{
				break;
			}
		}
	}

	// to prevent unknown locations at start and end
	if (d6min > 0.001 && loc_init_)
	{
		loc_idx_ = loc_last_idx_;
	}
	else
	{
		loc_idx_ = idx_tmp;
	}
	return d6min;
}

double dLIPredict(
	int &loc_idx_,
	int loc_last_idx_,
	Vector4d point_,
	vector<Vector4d> mid_,
	vector<double> len_,
	vector<Vector3d> tangent_,
	int loc_offset_,
	int loc_init_) //0 for true
{
	int idx_tmp=loc_last_idx_;
	double d1,d2,d3,d4,d5,d6,d6min,d6min2,min_dist;
	d1=d2=d3=d4=d5=d6=0.0; d6min = min_dist = 10;
	Vector3d proj_dir_tmp, beg, mid, end;

	// Added an offset buffer to prevent the location prediction from jumping too much
	int idx1 = ( loc_last_idx_-(loc_offset_/2) < 0 ? 0 : loc_last_idx_-(loc_offset_/2) );
	int idx2 = ( idx1+loc_offset_ > LOC_INT ? LOC_INT : idx1+loc_offset_ );
	for(int l=idx1;l<idx2;l++)
	{
		mid = V4d3d(mid_[l])* mid_[l][3];
		beg = mid - (tangent_[l] * (len_[l]/2));
		end = mid + (tangent_[l] * (len_[l]/2));
		proj_dir_tmp = tangent_[l] * ((V4d3d(point_)-mid).dot(tangent_[l]));

		d1 = (beg-mid).norm();
		d2 = (end-mid).norm();
		d3 = (beg-(mid+proj_dir_tmp)).norm();
		d4 = (end-(mid+proj_dir_tmp)).norm();
		d5 = (beg-end).norm();

		if (directionCheck(proj_dir_tmp, tangent_[l]))
		{
			// ignore the first loc_int so that we don't get "stuck" at curves
			if (l == idx1)	{d6min2 = d3-d5; d6 = d6min2; continue;}
			if (d4<=d2 && (d3-d5) < 0.005) //### TODO small error deviation (deadzone)
			{
				d6 = d3-d5;
				if (min_((V4d3d(point_)-mid).norm(), min_dist))
				{
					min_dist = (V4d3d(point_)-mid).norm();
					idx_tmp  = l;
				}
			}
		}
		else
		{
			// ignore the first loc_int so that we don't get "stuck" at curves
			if (l == idx1)	{d6min2 = d4-d5; d6 = d6min2; continue;}
			if (d3<=d1 && (d4-d5) < 0.005)
			{
				d6 = d4-d5;
				if (min_((V4d3d(point_)-mid).norm(), min_dist))
				{
					min_dist = (V4d3d(point_)-mid).norm();
					idx_tmp  = l;
				}
			}
		}
	}

	//	// to prevent unknown locations at start and end
//	if (d6min > 0.001)
//	{
//		loc_idx_ = loc_last_idx_;
//	}
//	else
//	{
//		loc_idx_ = idx_tmp;
//	}

	if(loc_init_==0) // initial
	{
		min_dist = 10;
		int tmp = 0;
		for(int l=idx1;l<idx2;l++)
		{
			mid = V4d3d(mid_[l])* mid_[l][3];
			if (min_((V4d3d(point_)-mid).norm(), min_dist))
			{
				min_dist = (V4d3d(point_)-mid).norm();
				tmp  = l;
			}
		}

		if (tmp<idx_tmp)
			loc_idx_ = tmp;
		else
			loc_idx_ = idx_tmp;
	}
	else
	{
		loc_idx_ = idx_tmp;
	}

	return d6;
}

//double decideLocationInterval(
//	vector<int> &loc_idxs_,
//	int &loc_last_idx_,
//	point_d point_,
//	vector<point_d> beg_,
//	vector<point_d> mid_,
//	vector<point_d> end_,
//	vector<point_d> tangent_,
//	int loc_offset_,
//	bool loc_init_)
//{
//	int idx_tmp;
//	double d1,d2,d3,d4,d5,d6,d6min; d1=d2=d3=d4=d5=d6=0.0; d6min = 10;
//	point_d proj_dir_tmp;
//	bool flag = true;
//
//	// Added a buffer to prevent the location prediction from jumping too much
//	int idx1 = ( loc_last_idx_ < 0 ? 0 : loc_last_idx_ );
//	int idx2 = ( idx1+loc_offset_ > LOC_INT ? LOC_INT : idx1+loc_offset_ );
//	for(int l=idx1;l<idx2;l++)
//	{
//		proj_dir_tmp =
//				multiPoint(
//						tangent_[l],
//						dotProduct(minusPoint(point_,mid_[l]),tangent_[l]));
//		d1 = l2Norm(minusPoint(beg_[l],mid_[l]));
//		d2 = l2Norm(minusPoint(end_[l],mid_[l]));
//		d3 = l2Norm(minusPoint(beg_[l],addPoint(mid_[l],proj_dir_tmp)));
//		d4 = l2Norm(minusPoint(end_[l],addPoint(mid_[l],proj_dir_tmp)));
//		d5 = l2Norm(minusPoint(beg_[l],end_[l]));
//		if (directionCheck(proj_dir_tmp, tangent_[l]))
//		{
//			if (l == idx1)	{d6 = d3-d5;}
//			if (d4<=d2 && (d3-d5) < 0.01) //### TODO small error deviation (deadzone)
//			{
//				d6 = d3-d5;
//			}
//		}
//		else
//		{
//			if (l == idx1)	{d6 = d4-d5;}
//			if (d3<=d1 && (d4-d5) < 0.01)
//			{
//				d6 = d4-d5;
//			}
//		}
//		if (min_(fabs(d6),d6min))
//		{
//			d6min = fabs(d6);
//			idx_tmp = l;
//		}
//	}
//
//	loc_idxs_.clear();
//	// to prevent unknown locations at start and end
//	if (d6min > 0.005)
//	{
//		loc_idxs_.push_back(loc_last_idx_);
//		loc_last_idx_ = loc_idxs_.back();
//	}
//	else
//	{
//		if (loc_init_ || idx_tmp==loc_last_idx_)
//		{
//			loc_idxs_.push_back(idx_tmp);
//			loc_last_idx_ = loc_idxs_.back();
//		}
//		else
//		{
//			if (idx_tmp>loc_last_idx_)
//			{
//				for(int i=loc_last_idx_+1;i<idx_tmp+1;i++)
//				{
//					loc_idxs_.push_back(i);
//				}
//				loc_last_idx_ = idx_tmp;
//			}
//			else
//			{
//				for(int i=idx_tmp+1;i<loc_last_idx_+1;i++)
//				{
//					loc_idxs_.push_back(i);
//				}
//				loc_last_idx_ = idx_tmp;
//			}
//		}
//	}
//	return d6min;
//}
//
//double decideLocationInterval(
//	int &loc_idx_,
//	int &loc_last_idx_,
//	point_d point_,
//	vector<point_d> beg_,
//	vector<point_d> mid_,
//	vector<point_d> end_,
//	vector<point_d> tangent_,
//	int offset_)
//{
//	double d1,d2,d3,d4,d5,d6,d7;
//	point_d proj_dir_tmp;
//	int idx1 = ( loc_last_idx_ < 0 ? 0 : loc_last_idx_ );
////	int idx1 = (loc_last_idx_-offset_  < 0 ? 0 : loc_last_idx_-offset_);
//	int idx2 =
//			(offset_ < 0 ?
//					LOC_INT :
//					(loc_last_idx_+offset_ > LOC_INT ?
//							LOC_INT :
//							loc_last_idx_+offset_));
//	bool mem = false;
//	for(int l=idx1;l<idx2;l++)
//	{
//		proj_dir_tmp =
//				multiPoint(
//						tangent_[l],
//						dotProduct(minusPoint(point_,mid_[l]),tangent_[l]));
//		d1 = l2Norm(minusPoint(beg_[l],mid_[l]));
//		d2 = l2Norm(minusPoint(end_[l],mid_[l]));
//		d3 = l2Norm(minusPoint(beg_[l],addPoint(mid_[l],proj_dir_tmp)));
//		d4 = l2Norm(minusPoint(end_[l],addPoint(mid_[l],proj_dir_tmp)));
//		d5 = l2Norm(minusPoint(beg_[l],end_[l]));
//		if (directionCheck(proj_dir_tmp, tangent_[l]))
//		{
//			mem = true;
//			d7 = d3-d5;
//			if (l == 0)						{d6 = d3-d5;}
//			if (d4<=d2 && (d3-d5)<0.001)	{loc_idx_ = l; break;} //### small error deviation (deadzone)
//		}
//		else
//		{
//			//### helps to mitigate the deadzone
////			if (mem) {loc_idx_ = (l-1<0 ? 0:l-1); break;}
//			d7 = d4-d5;
//			if (l == 0)						{d6 = d4-d5;}
//			if (d3<=d1 && (d4-d5)<0.001) 	{loc_idx_ = l; break;}
//		}
//	}
//	// to prevent unknown locations at start and end
//	if (loc_idx_<0) {loc_idx_		= loc_last_idx_	; return d6;}
//	else			{loc_last_idx_	= loc_idx_		; return d7;}
//
//}

int decideSectorInterval(
	int &sec_idx_,
	int loc_idx_,
	Vector3d &delta_t_,
	Vector4d point_,
	vector<Vector4d> mid_,
	vector<Vector3d> tangent_,
	vector<Vector3d> normal_)
{
	//projection of point onto the tangent at mid
	Vector3d mid = V4d3d(mid_[loc_idx_])* mid_[loc_idx_][3];

	Vector3d proj_dir_tmp =
			tangent_[loc_idx_] *
			((V4d3d(point_) - mid).dot(tangent_[loc_idx_]));

	delta_t_ = V4d3d(point_) - (proj_dir_tmp + mid);

	double angle_tmp =
			atan2(
					(delta_t_.cross(normal_[loc_idx_])).norm(),
					 delta_t_.dot(normal_[loc_idx_]));

	if (!directionCheck(normal_[loc_idx_].cross(delta_t_), tangent_[loc_idx_]))
	{
		angle_tmp *= -1;
	}
	angle_tmp = fmod((2*M_PI + angle_tmp),(2*M_PI));
	sec_idx_ = ceil(angle_tmp*(SEC_INT/2)/M_PI) - 1 ;
	return EXIT_SUCCESS;
}

//int decideCurvature(
//	point_d point_,
//	vector<point_d> &curve_mem_,
//	double &curve_,
//	int num_points_)
//{
//	int s = curve_mem_.size()-1;
//	if (s+1<num_points_)
//	{
//		curve_mem_.push_back(point_);
//		curve_ = 0.0;
//	}
//	else
//	{
//		curve_mem_.erase(curve_mem_.begin());
//		curve_mem_.push_back(point_);
//		curve_ =
//				(l2Norm(minusPoint(curve_mem_[0], 	curve_mem_[s/2])) +
//				 l2Norm(minusPoint(curve_mem_[s/2],	curve_mem_[s]))) /
//				 l2Norm(minusPoint(curve_mem_[0],	curve_mem_[s]));
//		curve_ -= 1.0;
//	}
//	// HACK TODO
//	if (curve_>0.4) {curve_ = 0.4;}
//	return EXIT_SUCCESS;
//}
//
//int decideRateOfChangeOfDeltaT(
//	point_d delta_t_,
//	vector<point_d> &delta_t_mem_,
//	double &dd_delta_t_,
//	int num_points_)
//{
//	int s = delta_t_mem_.size()-1;
//	if (s+1<num_points_)
//	{
//		delta_t_mem_.push_back(delta_t_);
//		dd_delta_t_ = 0.0;
//	}
//	else
//	{
//		delta_t_mem_.erase(delta_t_mem_.begin());
//		delta_t_mem_.push_back(delta_t_);
//		dd_delta_t_ =
//				l2Norm(
//					minusPoint(
//							minusPoint(
//									delta_t_mem_[s  ],
//									delta_t_mem_[s/2]),
//							minusPoint(
//									delta_t_mem_[s/2],
//									delta_t_mem_[0  ])));
//	}
//	return EXIT_SUCCESS;
//}

int folderSelect1(
	const struct dirent *entry)
{
	if (entry->d_name[2] == '_')	{return 1;}
	else							{return 0;}
}

int folderSelect2(
	const struct dirent *entry)
{
	size_t found_extension = string(entry->d_name).find(".");
	if ((int)found_extension == -1) {return 1;}
	else							{return 0;}
}

int fileSelect(
	const struct dirent *entry)
{
	size_t found_extension = string(entry->d_name).find(".txt");
	if ((int)found_extension == -1) {return 0;}
	else							{return 1;}
}
