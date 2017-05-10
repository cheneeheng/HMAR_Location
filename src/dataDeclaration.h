/*
 * dataDeclaration.h
 *
 *  Created on: Jan 11, 2017
 *      Author: chen
 */

#ifndef DATADECLARATION_H_
#define DATADECLARATION_H_

//#define PC

#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <list>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <vector>
#include <stack>
#include <map>
#include <semaphore.h>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <iterator>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNamedColors.h>
#include <vtkLookupTable.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointSource.h>
#include <vtkPointData.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <vtkSphereSource.h>
#include <vtkPointPicker.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkDoubleArray.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkFloatArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>
#include <vtkSmartPointer.h>
#include <vtkParametricFunctionSource.h>
#include <vtkTupleInterpolator.h>
#include <vtkTubeFilter.h>
#include <vtkParametricSpline.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPolygon.h>
#include <vtkCurvatures.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkLight.h>
#include <vtkRect.h>
#include <vtkAxis.h>
#include <vtkGL2PSExporter.h>

#include <gsl/gsl_integration.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_linalg.h>

#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

#ifdef PC
	// For backward compatibility with new VTK generic data arrays
	#define InsertNextTypedTuple InsertNextTupleValue
	#include <opencv2/opencv.hpp>
	using namespace cv;
	#define SCN "../Scene/"
#else
	#define SCN "Scene/"
#endif

#define CRED "\x1B[31m"
#define CGRN "\x1B[32m"
#define CYEL "\x1B[33m"
#define CBLU "\x1B[34m"
#define CMAG "\x1B[35m"
#define CCYN "\x1B[36m"
#define CWHT "\x1B[37m"
#define CNOR "\x1B[0m"

#define Sqr(x) ((x)*(x))
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define Calloc(type,n) (type *)calloc( n, sizeof(type))
#define v vector

#define	TRN 0
#define	TST 1
#define	LBL 2
#define	DPL 3

#define RELEASE			0
#define GRABBED			1
#define RELEASE_CLOSE	2
#define GRABBED_CLOSE	3

//0 : all
//1 : motion
//2 : location
//3 : label only
#define VERBOSE 3

#define CLUSTER_LIMIT 0.20

//#define SURFACE_LIMIT 0.05

#define FILTER_WIN 5

#define BOUNDARY_VAR 0.1
#define P_WIN_VAR 0.001
#define P_SEC_VAR 0.0005
#define P_LOC_VAR 0.0005
#define P_DDD_VAR 0.0005
#define P_ERR_VAR 0.01

#define CONTACT_TRIGGER_RATIO 0.65

#define DBSCAN_EPS 0.015
#define DBSCAN_MIN 5
#define DBSCAN_EPS_MOV 0.015
#define DBSCAN_MIN_MOV 8
#define MAX_RANGE 0.05
#define	LOC_INT 100
#define	SEC_INT 36

// number of fit coefficients
// nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 (for cubic b-spline)
#define NCOEFFS	15
#define NBREAK 	(NCOEFFS - 2)
#define DEGREE 10 //k+1

#define RANGE_EXCEED	3
#define RANGE_OUT		2
#define	RANGE_IN		1
#define RANGE_NULL		0

// constraints during movements
#define MOV_CONST_CIRC 	2
#define MOV_CONST_SURF 	1
#define MOV_CONST_STOP 	0

#define WIN_HEIGHT		800
#define WIN_WIDTH 		1280
#define FONT_SIZE 		20

#define CLICK_EMPTY		0
#define CLICK_LABEL		1
#define CLICK_DELETE 	2

//******************** TAKEN FROM .....
#define UNCLASSIFIED 	-1
#define NOISE 			-2

#define CORE_POINT 		 1
#define NOT_CORE_POINT 	 0

#define SUCCESS 		 0
#define FAILURE 		-3

//typedef struct point_s point_t;
//struct point_s {
//    double x, y, z;
//    int cluster_id;
//};

typedef struct node_s node_t;
struct node_s {
    unsigned int index;
    node_t *next;
};

typedef struct epsilon_neighbours_s epsilon_neighbours_t;
struct epsilon_neighbours_s {
    unsigned int num_members;
    node_t *head, *tail;
};

//********************

typedef struct point_sd point_d;
struct point_sd
{
    double x, y, z, l;
};

typedef struct pva_s pva_t;
struct pva_s
{
	point_d pos, vel, acc;
};

typedef struct node_ss node_tt;
struct node_ss
{
	string 	 name;
	int		 index; // used to check for new nodes
	int		 contact;
	Vector4d centroid; // Sphere
	int		 surface; // Surface
	Vector3d box_max;
	Vector3d box_min;
};

typedef struct edge_ss edge_tt;
struct edge_ss
{
	string				name;
	unsigned int 		idx1;
	unsigned int 		idx2;
	vector<double> 		sector_map; // locations int * sectors int
	vector<double> 		sector_const;
	vector<Vector3d> 	tan; // locations int
	vector<Vector3d> 	nor; // locations int
	vector<Vector4d> 	loc_mid; // locations int
	vector<double>  	loc_len; // locations int
	double 				total_len;
	int 				counter;
	vector<int> 		mov_const; // 0/1 activation of the mov_const labels
	vector<double> 		loc_mem; // to calculate d2(loc)
	vector<double> 		sec_mem; // to calculate d2(sec)
	vector<double> 		err_mem; // to calculate d2(err)
};

// prediction for node
typedef struct predict_sn predict_n;
struct predict_sn
{
	double 			acc; // acc
	double 			vel; // velocity limit 0/1
	double 			surface_dist; // surface distance

	double 			curvature; // curvature value : 1 for line
	vector<double>	range; // in or outside
	vector<double> 	err; // prediction error = diff from the sectormap
	vector<double> 	pct_err; // prob shared between multiple predictions of inside
	vector<double>	err_diff; // change in the error compared to original
	vector<double>	pct_err_diff; // change in the error compared to original
	vector<double>	oscillate; // repetitive movement
	vector<double>	ddl; // rate of change of loc int
	vector<double>	window; // knot in trajectory
};

// prediction for edge
typedef struct predict_se predict_e;
struct predict_se
{
	double 			acc; // acc
	double 			vel; // velocity limit 0/1
	double 			curvature; // curvature value : 1 for line
	vector<double>	range; // in or outside
	vector<double> 	err; // prediction error = diff from the sectormap
	vector<double> 	pct_err; // prob shared between multiple predictions of inside
	vector<double>	err_diff; // change in the error compared to original
	vector<double>	pct_err_diff; // change in the error compared to original
	vector<double>	oscillate; // repetitive movement
	vector<double>	ddl; // rate of change of loc int
	vector<double>	window; // knot in trajectory
};

typedef struct kb_s kb_t;
struct kb_s
{
	vector<Vector4d>		 		surface_eq;
	vector<Vector3d> 				surface_mid;
	vector<Vector3d> 				surface_min; // from mid
	vector<Vector3d> 				surface_max; // from mid
	vector<Matrix3d> 				surface_rot;
	vector<double> 					surface_lim;
	map<int,vector<string> > 		label;
	map<string,pair<int,int> > 		ac;
	vector<string> 					al;
	map<string,map<string,string> >	ol;
};

typedef struct state_s state_t;
struct state_s
{
	int grasp;					// if it is grasped
	int label1;					// last node
	int label2;					// next node
	double mov;					// velocity
	double pct_err;				// highest probability
	int sur;					// which surface
	double sur_dist;			// surface distance
//	map<string,double> ll;		// last location probabilities
	map<string,double> goal;	// probabilities
	map<string,double> window;	// window radius
};

#endif /* DATADECLARATION_H_ */
