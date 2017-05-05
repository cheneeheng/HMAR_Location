/*
 * ActionPrediction.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: chen
 */

#include "ActionPrediction.h"

ActionPrediction::ActionPrediction(
	Graph *Graph_,
	bool learn_) : Evaluate(Graph_)
{
	label1_ap = -1;
	learn = learn_;
	predict = {};
	state = {};
	last_loc.clear();
	init.clear();
	pva_avg.clear();
	pva_avg_mem.clear();
	predict_mem.clear();

	reshapeVector(range_in, G->GetNumberOfNodes());
	reshapeVector(last_loc, G->GetNumberOfNodes());
	reshapePredict(predict, G->GetNumberOfNodes());

	for(int is=0;is<G->GetNumberOfNodes();is++) init.push_back(true);
}

ActionPrediction::~ActionPrediction() { }

void ActionPrediction::PredictInit()
{
	state = G->GetActionState();

	state.grasp 	= RELEASE;
	state.label1 	= label1_ap;
	state.label2 	= label1_ap;
	state.pct_err 	= -1;
	state.mov 		=  0;
	state.con 		= -1;
	state.sur 		= -1;

	vector<string> al_tmp = G->GetActionLabel();
	map<string,pair<int,int> > ac_tmp = G->GetActionCategory();
	for(int i=ac_tmp["GEOMETRIC"].first;i<ac_tmp["GEOMETRIC"].second+1;i++)
	{
		state.ll	[al_tmp[i]] =   0;
		state.goal	[al_tmp[i]] = 0.0;
		state.window[al_tmp[i]] = 0.0;
	}

	G->SetActionState(state);
}

void ActionPrediction::PredictExt(
	vector<point_d> &pva_avg_,
	int contact_)
{
	pva_avg = pva_avg_;
	state = G->GetActionState();
	if (contact_==1)
	{
		if (state.grasp==RELEASE)
		{
			state.grasp = GRABBED_CLOSE;
			G->SetActionState(state);
			this->Predict();
		}
		else
		{
			state.grasp = GRABBED;
			G->SetActionState(state);
			this->Predict();
		}
	}
	else
	{
		if (state.grasp==GRABBED)
		{
			state.grasp = RELEASE_CLOSE;
			G->SetActionState(state);
			this->Predict();
		}
		else
		{
			state.grasp = RELEASE;
			G->SetActionState(state);
		}
	}
	pva_avg_ = pva_avg;
}

int ActionPrediction::Predict()
{
	// 1. Contact trigger
	// 1.1 Check if the object is within a sphere volume of the location areas
	this->PredictFromNode();

	// 2. Prediction during motion
	if (pva_avg[0].l < 0)
	{
		pva_avg_mem.push_back(pva_avg);
		this->PredictFromEdge();
	}

	// 3. Prediction within location area
	else
	{
		init.clear();
		for(int is=0;is<G->GetNumberOfNodes();is++) { init.push_back(true); }

		// ### TODO not really correct because the clusters are still there
		// just to show how the sectormap changes with time
		if (label1_ap!=pva_avg[0].l && label1_ap>=0)
		{
			predict_mem.clear();
			reshapeVector(last_loc, G->GetNumberOfNodes());

			if (learn)
			{
				this->RebuildSectorMap(pva_avg_mem, label1_ap, pva_avg[0].l);
				pva_avg_mem.clear();
			}
		}

		label1_ap = pva_avg[0].l;

		// there is a problem where the object gets out of bound for a while and return again
		vector<map<string, double> > p_tmp;
		G->GetPredictionReset(p_tmp);
		G->SetPrediction(p_tmp);

		node_tt nt{};
		G->GetNode(label1_ap,nt);
		state = G->GetActionState();
		vector<string> al_tmp = G->GetActionLabel();
		map<string,pair<int,int> > ac_tmp = G->GetActionCategory();
		for(int i=0;i<al_tmp.size();i++)
		{
			if (!strcmp(nt.name.c_str(),al_tmp[i].c_str()))
			{
				state.label1 = i;
				state.label2 = i;
			}
		}

		state.con = nt.contact;
		state.sur = nt.surface;
		state.pct_err = -1;
		state.mov = l2Norm(pva_avg[1]);
		for(int i=ac_tmp["GEOMETRIC"].first;i<ac_tmp["GEOMETRIC"].second+1;i++)
		{
			state.goal[al_tmp[i]] = 0.0;
			state.window[al_tmp[i]] = 0.0;
		}
		G->SetActionState(state);

	}

	// 4. OUTPUT
	// says what i am currently doing
	return EXIT_SUCCESS;
}

int ActionPrediction::PredictFromNode()
{
	// initial case
	if (label1_ap < 0)
	{
		decideBoundaryClosest_(pva_avg[0], G->GetCentroidList());
		state = G->GetActionState();
		state.grasp = GRABBED_CLOSE;
		G->SetActionState(state);
	}
	else
	{
		state = G->GetActionState();
		// give a starting location during change from release to move
		if ((state.grasp==GRABBED_CLOSE) || (state.grasp==RELEASE_CLOSE))
		{
			decideBoundaryClosest_(pva_avg[0], G->GetCentroidList());
		}
		else
		{
			decideBoundaryPredict(
					pva_avg[0], pva_avg[0], G->GetCentroidList(),
					G->GetSurfaceFlagList(), G->GetSurfaceEq(),
					G->GetSurfaceLimit());

			// prevent from going to unknown goal locations during middle of movement
			for(int i=0;i<G->GetNumberOfNodes();i++)
			{
				if (predict.pct_err[i]>0 && last_loc[i]>LOC_INT/10 && last_loc[i]<LOC_INT-(LOC_INT/10))
				{
					if (predict.pct_err[(int)pva_avg[0].l]==0 && predict.range[(int)pva_avg[0].l]!=RANGE_EXCEED)
					{
//						cout << last_loc[i] <<"  " << (int)pva_avg[0].l << " # ";
						pva_avg[0].l = UNCLASSIFIED;
					}
					break;
				}
			}
		}
	}

//	for(int i=0;i<G->GetNumberOfNodes();i++)
//	{
//		cout << i << " : " << pva_avg[0].l << "   ";
//	}
//	cout << G->GetActionState().label2 << " : " << G->GetActionState().grasp << endl;

	return EXIT_SUCCESS;
}

int ActionPrediction::PredictFromEdge()
{
	reshapePredict(predict, G->GetNumberOfNodes());

	// 2.1. Check for motion
	this->DecideMovement();

	// 2.5. Check if the trajectory is within the range of sector map
	this->PredictFromSectorMap();

	// prediction average
	{
		predict_mem.push_back(predict);
		if (predict_mem.size()>3) {predict_mem.erase(predict_mem.begin());}
	}

	// 2.X. Predict the goal based on the trajectory error from sector map
	this->EvaluatePrediction();

//	if (label1_ap==0)
//	{
//		for(int i=0;i<G->GetNumberOfNodes();i++)
//		{
//			cout << i << " : " << last_loc[i] << "," << pct_err_eval[i] << "," << predict.pct_err[i] << "," << predict.range[i] << "  ::  ";
//		}
//		cout << endl;
//	}

	return EXIT_SUCCESS;
}

int ActionPrediction::DecideMovement()
{
	// #### TODO: vel limit
	if (l2Norm(pva_avg[1])<0.001)
	{
		predict.vel = 0;
		predict.acc = 0;
	}
	else
	{
		predict.vel = l2Norm(pva_avg[1]);
		predict.acc = l2Norm(pva_avg[2]);
	}
	return EXIT_SUCCESS;
}

int ActionPrediction::PredictFromSectorMap()
{
	reshapeVector(range_in,G->GetNumberOfNodes());

	for(int i=0;i<G->GetNumberOfNodes();i++)
	{
		predict.range[i] 		= RANGE_NULL;
		predict.err[i] 			= 0.0;
		predict.pct_err[i]   	= 0.0;
		predict.err_diff[i] 	= 0.0;
		predict.pct_err_diff[i] = 0.0;
		predict.oscillate[i]	= 0.0;
		predict.window[i]		= 0.0;

		if (label1_ap==i) {continue;}
		if (G->GetEdgeCounter(label1_ap, i, 0)==0) {continue;}

		point_d delta_t;
		int loc_idx, sec_idx; loc_idx=sec_idx=-1;

		// LOC SEC INT
		bool init_tmp = init[i];
		double tmp_dis =
				this->DecideLocSecInt(
						G->GetEdge(label1_ap, i, 0), delta_t, sec_idx, loc_idx,
						last_loc[i], init_tmp);
		init[i] = init_tmp;

		if(last_loc[i]==0)
		{
			init[i] = true;
//			continue;
		}

		vector<double> sm_tmp;
		G->GetEdgeSectorMap(label1_ap, i, 0, sm_tmp);

		// GLA
		bool flag =
				this->DecideGoal(
						i, sm_tmp[loc_idx*SEC_INT + sec_idx],
						l2Norm(delta_t), tmp_dis);
		if (!flag) {continue;}

		// window
		this->DecideWindow(sm_tmp, loc_idx, i);
//		if(i == 2) cout << loc_idx << " " <<predict_.window[i] << endl;

	}



	// give priority to range in
	int sum_tmp = accumulate(range_in.begin(), range_in.end(), 0.0, addFunction);

//	if (sum_tmp>0)
//	{
//		for(int i=0;i<G->GetNumberOfNodes();i++)
//		{
//			if (predict.range[i]!=RANGE_IN)
//			{
//				predict.pct_err[i] *= 0.5;
//			}
//		}
//	}

	return EXIT_SUCCESS;
}

double ActionPrediction::DecideLocSecInt(
	edge_tt edge_,
	point_d &delta_t_,
	int &sec_idx_,
	int &loc_idx_,
	int &loc_last_idx_,
	bool &init_)
{
	double tmp_dis, offset, last;

	if (init_)	{offset = LOC_INT*1.00;}
	else		{offset = LOC_INT*0.50;}

	last = loc_last_idx_;

	tmp_dis  =
			dLIPredict(
					loc_idx_,
					loc_last_idx_,
					pva_avg[0],
					edge_.loc_start,
					edge_.loc_mid,
					edge_.loc_end,
					edge_.tan,
					offset,
					init_);

	if(loc_idx_>0) init_ = false;

	if(loc_idx_-loc_last_idx_>0)
	{
		for(int i=loc_last_idx_+1;i<loc_idx_+1;i++)
		{
			decideSectorInterval(
					sec_idx_,
					i,
					delta_t_,
					pva_avg[0],
					edge_.loc_mid,
					edge_.tan,
					edge_.nor);
		}
	}
	else if(loc_idx_-loc_last_idx_<0)
	{
		for(int i=loc_idx_+1;i<loc_last_idx_+1;i++)
		{
			decideSectorInterval(
					sec_idx_,
					i,
					delta_t_,
					pva_avg[0],
					edge_.loc_mid,
					edge_.tan,
					edge_.nor);
		}
	}
	else
	{
		if(!init_ && last>0) {loc_idx_ = last;}

		decideSectorInterval(
				sec_idx_,
				loc_idx_,
				delta_t_,
				pva_avg[0],
				edge_.loc_mid,
				edge_.tan,
				edge_.nor);
	}

	loc_last_idx_ = loc_idx_;
	return tmp_dis;
}

bool ActionPrediction::DecideGoal(
	int label2_,
	double &sm_i_, //sectormap single
	double delta_t_,
	double loc_error_)
{
	if (loc_error_ > 0.10) //## TODO :NEED TO VERIFY
	{
		predict.range[label2_] = RANGE_EXCEED;
		predict.err[label2_] = delta_t_ - sm_i_;
		predict.pct_err[label2_] = pdfExp(P_ERR_VAR, 0.0, predict.err[label2_]);
		return false;
	}

	if (delta_t_ <= sm_i_)
	{
		range_in[label2_] = 1;
		predict.range[label2_] = RANGE_IN;
		predict.pct_err[label2_]   = 0.999999;
	}
	else
	{
		predict.range[label2_] = RANGE_OUT;
		predict.err[label2_] = delta_t_ - sm_i_;
		predict.pct_err[label2_] = pdfExp(P_ERR_VAR, 0.0, predict.err[label2_]);
	}

	return true;
}

int ActionPrediction::DecideWindow(
	vector<double> sm_,
	int loc_idx_,
	int label2_)
{
	double max_val = 0.0;
	for(int s=0;s<SEC_INT;s++)
	{
		max_val = max(sm_[loc_idx_*SEC_INT+s],max_val);
	}

	predict.window[label2_] = max_val;

	if(max_val > 0)
	{
		predict.window[label2_] = min(pdfExp(P_WIN_VAR,0,max_val), 1.0);
	}

	return EXIT_SUCCESS;
}

int ActionPrediction::EvaluatePrediction()
{
	vector<vector<double> > err; err.resize(G->GetNumberOfNodes());
	vector<vector<double> > win; win.resize(G->GetNumberOfNodes());
	reshapeVector(pct_err_eval,G->GetNumberOfNodes());
	reshapeVector(win_eval,G->GetNumberOfNodes());

	for(int i=0;i<predict_mem.size();i++)
	{
		for(int ii=0;ii<G->GetNumberOfNodes();ii++)
		{
			win[ii].push_back(predict_mem[i].window[ii]);
			err[ii].push_back(predict_mem[i].pct_err[ii]);
		}
	}

	double sum_tmp = 0.0;
	double counter = 0.0;
	bool flag = false;
	for(int i=0;i<G->GetNumberOfNodes();i++)
	{
		win_eval[i] = average(win[i]);
		pct_err_eval[i] = average(err[i]);
		sum_tmp += pct_err_eval[i];

		if (G->GetEdgeCounter(label1_ap,i,0)>0)
		{
			counter += 1.0;
		}

		if (last_loc[i]>LOC_INT/10)
		{
			flag = true;
		}
	}

	for(int i=0;i<G->GetNumberOfNodes();i++)
	{
		if (flag)
		{
			if (sum_tmp == 0.0)
				pct_err_eval[i] = 0.0;
			else
				pct_err_eval[i] /= sum_tmp;
		}
		else
		{
			// this will show zero probability but do we need it ????
//			if (predict_mem.back().range[i]==RANGE_EXCEED)
//			{
//				pct_err_eval[i] = 0.0;
//			}
//			else
//			{
//				if (G->GetEdgeCounter(label1_ap,i,0)>0)
//				{
//					pct_err_eval[i] = 1.0/counter;
//				}
//			}
			if (G->GetEdgeCounter(label1_ap,i,0)>0)
			{
				pct_err_eval[i] = 1.0/counter;
			}
		}
	}

	vel_eval = predict_mem.back().vel;
	label1_eval = label1_ap;

	this->UpdateEval();

	return EXIT_SUCCESS;
}

int ActionPrediction::RebuildSectorMap(
	vector<vector<point_d> > pva_avg_,
	int	label1_,
	int label2_)
{
	// Graph_.getEdgeList() = [#loc*#loc -> #edges -> #loc*#sec]

	if (pva_avg_.size() < 5) { return EXIT_FAILURE; }

	vector<point_d> pts_avg, vel_avg;
	for(int i=0;i<pva_avg_.size();i++)
	{
		pts_avg.push_back(pva_avg_[i][0]);
		vel_avg.push_back(pva_avg_[i][0]);
	}

	this->SetLabel1SM(label1_);
	this->SetLabel2SM(label2_);
	this->UpdateSectorMap(G, pts_avg);
	this->FindWindowConstraint(G);

	//VISUALIZE
	if(0)
	{
		vector<point_d> point_zero; vector<string> label_zero;
		vector<vector<unsigned char> > color_code; colorCode(color_code);
		showConnection(G, pts_avg, label_zero, color_code, true);
	}

	return EXIT_SUCCESS;
}
