/*************************************
	PVDF Nanodomains NVT Sampling
		single threaded program
		written by GuanTong
			2024.4.30
**************************************/ 

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <fstream>
#include <stdlib.h> 
#include <stdio.h>
using namespace std;

const int N=50; // params of the box 
double seta_is[N][N][N],seta_fs[N][N][N]; // structural mx [0,2.0*pi)

int Lop_rlx,Lop_spl,step_n; // loop number for relaxation and sampling; steps under one loop
int plot_cycle,strc_cycle; // cycle number for output energy & polarization, structure
double Tspl,amp,P; // sampling temperature, MC step length, Polarization Intensity
double kB=1.38e-23,nat=2.71828,pi=3.1415926536,NA=6.02e23;//Boltzmann para,nature para,pi 
double pi_2,n_norm_factor,agl_ff; // pi*2.0, 1/N^3, 6.0/pi; 
double delta_angle_y,delta_angle_z; // force field angle refinement in y & z direction
double total_energy; // total energy of the current dipole cofiguration



// force field params kcal/mol 

double x_energy_factor[8]={0.0,2.26425,0.0,-1.08969,0.0,0.18769,0.0,-0.00968}; // has been tested


double ac_ff[24][24]={
-1.270995 ,-0.350970 ,-0.945007 ,3.470975 ,-6.548145 ,-4.859972 ,4.859972 ,6.548145 ,-3.470975 ,0.945007 ,0.350970 ,1.270995 ,-1.270995 ,-0.350970 ,-0.945007 ,3.470975 ,-6.548145 ,-4.859972 ,4.859972 ,6.548145 ,-3.470975 ,0.945007 ,0.350970 ,1.270995 ,
-0.754040 ,-2.750803 ,3.681865 ,-0.251895 ,-4.869165 ,-8.583817 ,8.583817 ,4.869165 ,0.251895 ,-3.681865 ,2.750803 ,0.754040 ,-0.754040 ,-2.750803 ,3.681865 ,-0.251895 ,-4.869165 ,-8.583817 ,8.583817 ,4.869165 ,0.251895 ,-3.681865 ,2.750803 ,0.754040 ,
0.450330 ,5.469083 ,-4.847280 ,-4.126700 ,1.492567 ,6.074050 ,-6.074050 ,-1.492567 ,4.126700 ,4.847280 ,-5.469083 ,-0.450330 ,0.450330 ,5.469083 ,-4.847280 ,-4.126700 ,1.492567 ,6.074050 ,-6.074050 ,-1.492567 ,4.126700 ,4.847280 ,-5.469083 ,-0.450330 ,
1.487213 ,-3.039185 ,0.929358 ,1.569655 ,-0.719348 ,-1.970197 ,1.970197 ,0.719348 ,-1.569655 ,-0.929358 ,3.039185 ,-1.487213 ,1.487213 ,-3.039185 ,0.929358 ,1.569655 ,-0.719348 ,-1.970197 ,1.970197 ,0.719348 ,-1.569655 ,-0.929358 ,3.039185 ,-1.487213 ,
-3.142840 ,0.268762 ,-6.597577 ,1.023610 ,1.899740 ,-0.821175 ,0.821175 ,-1.899740 ,-1.023610 ,6.597577 ,-0.268762 ,3.142840 ,-3.142840 ,0.268762 ,-6.597577 ,1.023610 ,1.899740 ,-0.821175 ,0.821175 ,-1.899740 ,-1.023610 ,6.597577 ,-0.268762 ,3.142840 ,
4.058422 ,-6.428117 ,5.279837 ,0.821445 ,-0.546340 ,-1.949302 ,1.949302 ,0.546340 ,-0.821445 ,-5.279837 ,6.428117 ,-4.058422 ,4.058422 ,-6.428117 ,5.279837 ,0.821445 ,-0.546340 ,-1.949302 ,1.949302 ,0.546340 ,-0.821445 ,-5.279837 ,6.428117 ,-4.058422 ,
-4.058422 ,6.428117 ,-5.279837 ,-0.821445 ,0.546340 ,1.949302 ,-1.949302 ,-0.546340 ,0.821445 ,5.279837 ,-6.428117 ,4.058422 ,-4.058422 ,6.428117 ,-5.279837 ,-0.821445 ,0.546340 ,1.949302 ,-1.949302 ,-0.546340 ,0.821445 ,5.279837 ,-6.428117 ,4.058422 ,
3.142840 ,-0.268762 ,6.597577 ,-1.023610 ,-1.899740 ,0.821175 ,-0.821175 ,1.899740 ,1.023610 ,-6.597577 ,0.268762 ,-3.142840 ,3.142840 ,-0.268762 ,6.597577 ,-1.023610 ,-1.899740 ,0.821175 ,-0.821175 ,1.899740 ,1.023610 ,-6.597577 ,0.268762 ,-3.142840 ,
-1.487213 ,3.039185 ,-0.929358 ,-1.569655 ,0.719348 ,1.970197 ,-1.970197 ,-0.719348 ,1.569655 ,0.929358 ,-3.039185 ,1.487213 ,-1.487213 ,3.039185 ,-0.929358 ,-1.569655 ,0.719348 ,1.970197 ,-1.970197 ,-0.719348 ,1.569655 ,0.929358 ,-3.039185 ,1.487213 ,
-0.450330 ,-5.469082 ,4.847280 ,4.126700 ,-1.492567 ,-6.074050 ,6.074050 ,1.492567 ,-4.126700 ,-4.847280 ,5.469083 ,0.450330 ,-0.450330 ,-5.469082 ,4.847280 ,4.126700 ,-1.492567 ,-6.074050 ,6.074050 ,1.492567 ,-4.126700 ,-4.847280 ,5.469083 ,0.450330 ,
0.754040 ,2.750803 ,-3.681865 ,0.251895 ,4.869165 ,8.583817 ,-8.583817 ,-4.869165 ,-0.251895 ,3.681865 ,-2.750803 ,-0.754040 ,0.754040 ,2.750803 ,-3.681865 ,0.251895 ,4.869165 ,8.583817 ,-8.583817 ,-4.869165 ,-0.251895 ,3.681865 ,-2.750803 ,-0.754040 ,
1.270995 ,0.350970 ,0.945007 ,-3.470975 ,6.548145 ,4.859972 ,-4.859972 ,-6.548145 ,3.470975 ,-0.945007 ,-0.350970 ,-1.270995 ,1.270995 ,0.350970 ,0.945007 ,-3.470975 ,6.548145 ,4.859972 ,-4.859972 ,-6.548145 ,3.470975 ,-0.945007 ,-0.350970 ,-1.270995 ,
-1.270995 ,-0.350970 ,-0.945007 ,3.470975 ,-6.548145 ,-4.859972 ,4.859972 ,6.548145 ,-3.470975 ,0.945007 ,0.350970 ,1.270995 ,-1.270995 ,-0.350970 ,-0.945007 ,3.470975 ,-6.548145 ,-4.859972 ,4.859972 ,6.548145 ,-3.470975 ,0.945007 ,0.350970 ,1.270995 ,
-0.754040 ,-2.750803 ,3.681865 ,-0.251895 ,-4.869165 ,-8.583817 ,8.583817 ,4.869165 ,0.251895 ,-3.681865 ,2.750803 ,0.754040 ,-0.754040 ,-2.750803 ,3.681865 ,-0.251895 ,-4.869165 ,-8.583817 ,8.583817 ,4.869165 ,0.251895 ,-3.681865 ,2.750803 ,0.754040 ,
0.450330 ,5.469083 ,-4.847280 ,-4.126700 ,1.492567 ,6.074050 ,-6.074050 ,-1.492567 ,4.126700 ,4.847280 ,-5.469083 ,-0.450330 ,0.450330 ,5.469083 ,-4.847280 ,-4.126700 ,1.492567 ,6.074050 ,-6.074050 ,-1.492567 ,4.126700 ,4.847280 ,-5.469083 ,-0.450330 ,
1.487213 ,-3.039185 ,0.929358 ,1.569655 ,-0.719348 ,-1.970197 ,1.970197 ,0.719348 ,-1.569655 ,-0.929358 ,3.039185 ,-1.487213 ,1.487213 ,-3.039185 ,0.929358 ,1.569655 ,-0.719348 ,-1.970197 ,1.970197 ,0.719348 ,-1.569655 ,-0.929358 ,3.039185 ,-1.487213 ,
-3.142840 ,0.268762 ,-6.597577 ,1.023610 ,1.899740 ,-0.821175 ,0.821175 ,-1.899740 ,-1.023610 ,6.597577 ,-0.268762 ,3.142840 ,-3.142840 ,0.268762 ,-6.597577 ,1.023610 ,1.899740 ,-0.821175 ,0.821175 ,-1.899740 ,-1.023610 ,6.597577 ,-0.268762 ,3.142840 ,
4.058422 ,-6.428117 ,5.279837 ,0.821445 ,-0.546340 ,-1.949302 ,1.949302 ,0.546340 ,-0.821445 ,-5.279837 ,6.428117 ,-4.058422 ,4.058422 ,-6.428117 ,5.279837 ,0.821445 ,-0.546340 ,-1.949302 ,1.949302 ,0.546340 ,-0.821445 ,-5.279837 ,6.428117 ,-4.058422 ,
-4.058422 ,6.428117 ,-5.279837 ,-0.821445 ,0.546340 ,1.949302 ,-1.949302 ,-0.546340 ,0.821445 ,5.279837 ,-6.428117 ,4.058422 ,-4.058422 ,6.428117 ,-5.279837 ,-0.821445 ,0.546340 ,1.949302 ,-1.949302 ,-0.546340 ,0.821445 ,5.279837 ,-6.428117 ,4.058422 ,
3.142840 ,-0.268762 ,6.597577 ,-1.023610 ,-1.899740 ,0.821175 ,-0.821175 ,1.899740 ,1.023610 ,-6.597577 ,0.268762 ,-3.142840 ,3.142840 ,-0.268762 ,6.597577 ,-1.023610 ,-1.899740 ,0.821175 ,-0.821175 ,1.899740 ,1.023610 ,-6.597577 ,0.268762 ,-3.142840 ,
-1.487213 ,3.039185 ,-0.929358 ,-1.569655 ,0.719348 ,1.970197 ,-1.970197 ,-0.719348 ,1.569655 ,0.929358 ,-3.039185 ,1.487213 ,-1.487213 ,3.039185 ,-0.929358 ,-1.569655 ,0.719348 ,1.970197 ,-1.970197 ,-0.719348 ,1.569655 ,0.929358 ,-3.039185 ,1.487213 ,
-0.450330 ,-5.469082 ,4.847280 ,4.126700 ,-1.492567 ,-6.074050 ,6.074050 ,1.492567 ,-4.126700 ,-4.847280 ,5.469083 ,0.450330 ,-0.450330 ,-5.469082 ,4.847280 ,4.126700 ,-1.492567 ,-6.074050 ,6.074050 ,1.492567 ,-4.126700 ,-4.847280 ,5.469083 ,0.450330 ,
0.754040 ,2.750803 ,-3.681865 ,0.251895 ,4.869165 ,8.583817 ,-8.583817 ,-4.869165 ,-0.251895 ,3.681865 ,-2.750803 ,-0.754040 ,0.754040 ,2.750803 ,-3.681865 ,0.251895 ,4.869165 ,8.583817 ,-8.583817 ,-4.869165 ,-0.251895 ,3.681865 ,-2.750803 ,-0.754040 ,
1.270995 ,0.350970 ,0.945007 ,-3.470975 ,6.548145 ,4.859972 ,-4.859972 ,-6.548145 ,3.470975 ,-0.945007 ,-0.350970 ,-1.270995 ,1.270995 ,0.350970 ,0.945007 ,-3.470975 ,6.548145 ,4.859972 ,-4.859972 ,-6.548145 ,3.470975 ,-0.945007 ,-0.350970 ,-1.270995
};


double ad_ff[24][24]={
3.256250 ,1.985255 ,1.634285 ,0.689278 ,4.160253 ,-2.387893 ,-7.247865 ,-2.387893 ,4.160253 ,0.689278 ,1.634285 ,1.985255 ,3.256250 ,1.985255 ,1.634285 ,0.689278 ,4.160253 ,-2.387893 ,-7.247865 ,-2.387893 ,4.160253 ,0.689278 ,1.634285 ,1.985255 ,
2.807350 ,2.053310 ,-0.697493 ,2.984372 ,2.732477 ,-2.136688 ,-10.720505 ,-2.136688 ,2.732477 ,2.984372 ,-0.697493 ,2.053310 ,2.807350 ,2.053310 ,-0.697493 ,2.984372 ,2.732477 ,-2.136688 ,-10.720505 ,-2.136688 ,2.732477 ,2.984372 ,-0.697493 ,2.053310 ,
-1.825770 ,-1.375440 ,4.093643 ,-0.753637 ,-4.880337 ,-3.387770 ,2.686280 ,-3.387770 ,-4.880337 ,-0.753637 ,4.093643 ,-1.375440 ,-1.825770 ,-1.375440 ,4.093643 ,-0.753637 ,-4.880337 ,-3.387770 ,2.686280 ,-3.387770 ,-4.880337 ,-0.753637 ,4.093643 ,-1.375440 ,
0.687375 ,2.174587 ,-0.864598 ,0.064760 ,1.634415 ,0.915067 ,-1.055130 ,0.915067 ,1.634415 ,0.064760 ,-0.864598 ,2.174587 ,0.687375 ,2.174587 ,-0.864598 ,0.064760 ,1.634415 ,0.915067 ,-1.055130 ,0.915067 ,1.634415 ,0.064760 ,-0.864598 ,2.174587 ,
4.753170 ,1.610330 ,1.879092 ,-4.718485 ,-3.694875 ,-1.795135 ,-2.616310 ,-1.795135 ,-3.694875 ,-4.718485 ,1.879092 ,1.610330 ,4.753170 ,1.610330 ,1.879092 ,-4.718485 ,-3.694875 ,-1.795135 ,-2.616310 ,-1.795135 ,-3.694875 ,-4.718485 ,1.879092 ,1.610330 ,
-1.362295 ,2.696127 ,-3.731990 ,1.547847 ,2.369293 ,1.822952 ,-0.126350 ,1.822952 ,2.369293 ,1.547847 ,-3.731990 ,2.696127 ,-1.362295 ,2.696127 ,-3.731990 ,1.547847 ,2.369293 ,1.822952 ,-0.126350 ,1.822952 ,2.369293 ,1.547847 ,-3.731990 ,2.696127 ,
1.362295 ,-2.696127 ,3.731990 ,-1.547847 ,-2.369293 ,-1.822952 ,0.126350 ,-1.822952 ,-2.369293 ,-1.547847 ,3.731990 ,-2.696127 ,1.362295 ,-2.696127 ,3.731990 ,-1.547847 ,-2.369293 ,-1.822952 ,0.126350 ,-1.822952 ,-2.369293 ,-1.547847 ,3.731990 ,-2.696127 ,
-4.753170 ,-1.610330 ,-1.879092 ,4.718485 ,3.694875 ,1.795135 ,2.616310 ,1.795135 ,3.694875 ,4.718485 ,-1.879092 ,-1.610330 ,-4.753170 ,-1.610330 ,-1.879092 ,4.718485 ,3.694875 ,1.795135 ,2.616310 ,1.795135 ,3.694875 ,4.718485 ,-1.879092 ,-1.610330 ,
-0.687375 ,-2.174587 ,0.864598 ,-0.064760 ,-1.634415 ,-0.915067 ,1.055130 ,-0.915067 ,-1.634415 ,-0.064760 ,0.864598 ,-2.174587 ,-0.687375 ,-2.174587 ,0.864598 ,-0.064760 ,-1.634415 ,-0.915067 ,1.055130 ,-0.915067 ,-1.634415 ,-0.064760 ,0.864598 ,-2.174587 ,
1.825770 ,1.375440 ,-4.093643 ,0.753637 ,4.880337 ,3.387770 ,-2.686280 ,3.387770 ,4.880337 ,0.753637 ,-4.093643 ,1.375440 ,1.825770 ,1.375440 ,-4.093643 ,0.753637 ,4.880337 ,3.387770 ,-2.686280 ,3.387770 ,4.880337 ,0.753637 ,-4.093643 ,1.375440 ,
-2.807350 ,-2.053310 ,0.697493 ,-2.984372 ,-2.732477 ,2.136688 ,10.720505 ,2.136688 ,-2.732477 ,-2.984372 ,0.697493 ,-2.053310 ,-2.807350 ,-2.053310 ,0.697493 ,-2.984372 ,-2.732477 ,2.136688 ,10.720505 ,2.136688 ,-2.732477 ,-2.984372 ,0.697493 ,-2.053310 ,
-3.256250 ,-1.985255 ,-1.634285 ,-0.689278 ,-4.160253 ,2.387893 ,7.247865 ,2.387893 ,-4.160253 ,-0.689278 ,-1.634285 ,-1.985255 ,-3.256250 ,-1.985255 ,-1.634285 ,-0.689278 ,-4.160253 ,2.387893 ,7.247865 ,2.387893 ,-4.160253 ,-0.689278 ,-1.634285 ,-1.985255 ,
3.256250 ,1.985255 ,1.634285 ,0.689278 ,4.160253 ,-2.387893 ,-7.247865 ,-2.387893 ,4.160253 ,0.689278 ,1.634285 ,1.985255 ,3.256250 ,1.985255 ,1.634285 ,0.689278 ,4.160253 ,-2.387893 ,-7.247865 ,-2.387893 ,4.160253 ,0.689278 ,1.634285 ,1.985255 ,
2.807350 ,2.053310 ,-0.697493 ,2.984372 ,2.732477 ,-2.136688 ,-10.720505 ,-2.136688 ,2.732477 ,2.984372 ,-0.697493 ,2.053310 ,2.807350 ,2.053310 ,-0.697493 ,2.984372 ,2.732477 ,-2.136688 ,-10.720505 ,-2.136688 ,2.732477 ,2.984372 ,-0.697493 ,2.053310 ,
-1.825770 ,-1.375440 ,4.093643 ,-0.753637 ,-4.880337 ,-3.387770 ,2.686280 ,-3.387770 ,-4.880337 ,-0.753637 ,4.093643 ,-1.375440 ,-1.825770 ,-1.375440 ,4.093643 ,-0.753637 ,-4.880337 ,-3.387770 ,2.686280 ,-3.387770 ,-4.880337 ,-0.753637 ,4.093643 ,-1.375440 ,
0.687375 ,2.174587 ,-0.864598 ,0.064760 ,1.634415 ,0.915067 ,-1.055130 ,0.915067 ,1.634415 ,0.064760 ,-0.864598 ,2.174587 ,0.687375 ,2.174587 ,-0.864598 ,0.064760 ,1.634415 ,0.915067 ,-1.055130 ,0.915067 ,1.634415 ,0.064760 ,-0.864598 ,2.174587 ,
4.753170 ,1.610330 ,1.879092 ,-4.718485 ,-3.694875 ,-1.795135 ,-2.616310 ,-1.795135 ,-3.694875 ,-4.718485 ,1.879092 ,1.610330 ,4.753170 ,1.610330 ,1.879092 ,-4.718485 ,-3.694875 ,-1.795135 ,-2.616310 ,-1.795135 ,-3.694875 ,-4.718485 ,1.879092 ,1.610330 ,
-1.362295 ,2.696127 ,-3.731990 ,1.547847 ,2.369293 ,1.822952 ,-0.126350 ,1.822952 ,2.369293 ,1.547847 ,-3.731990 ,2.696127 ,-1.362295 ,2.696127 ,-3.731990 ,1.547847 ,2.369293 ,1.822952 ,-0.126350 ,1.822952 ,2.369293 ,1.547847 ,-3.731990 ,2.696127 ,
1.362295 ,-2.696127 ,3.731990 ,-1.547847 ,-2.369293 ,-1.822952 ,0.126350 ,-1.822952 ,-2.369293 ,-1.547847 ,3.731990 ,-2.696127 ,1.362295 ,-2.696127 ,3.731990 ,-1.547847 ,-2.369293 ,-1.822952 ,0.126350 ,-1.822952 ,-2.369293 ,-1.547847 ,3.731990 ,-2.696127 ,
-4.753170 ,-1.610330 ,-1.879092 ,4.718485 ,3.694875 ,1.795135 ,2.616310 ,1.795135 ,3.694875 ,4.718485 ,-1.879092 ,-1.610330 ,-4.753170 ,-1.610330 ,-1.879092 ,4.718485 ,3.694875 ,1.795135 ,2.616310 ,1.795135 ,3.694875 ,4.718485 ,-1.879092 ,-1.610330 ,
-0.687375 ,-2.174587 ,0.864598 ,-0.064760 ,-1.634415 ,-0.915067 ,1.055130 ,-0.915067 ,-1.634415 ,-0.064760 ,0.864598 ,-2.174587 ,-0.687375 ,-2.174587 ,0.864598 ,-0.064760 ,-1.634415 ,-0.915067 ,1.055130 ,-0.915067 ,-1.634415 ,-0.064760 ,0.864598 ,-2.174587 ,
1.825770 ,1.375440 ,-4.093643 ,0.753637 ,4.880337 ,3.387770 ,-2.686280 ,3.387770 ,4.880337 ,0.753637 ,-4.093643 ,1.375440 ,1.825770 ,1.375440 ,-4.093643 ,0.753637 ,4.880337 ,3.387770 ,-2.686280 ,3.387770 ,4.880337 ,0.753637 ,-4.093643 ,1.375440 ,
-2.807350 ,-2.053310 ,0.697493 ,-2.984372 ,-2.732477 ,2.136688 ,10.720505 ,2.136688 ,-2.732477 ,-2.984372 ,0.697493 ,-2.053310 ,-2.807350 ,-2.053310 ,0.697493 ,-2.984372 ,-2.732477 ,2.136688 ,10.720505 ,2.136688 ,-2.732477 ,-2.984372 ,0.697493 ,-2.053310 ,
-3.256250 ,-1.985255 ,-1.634285 ,-0.689278 ,-4.160253 ,2.387893 ,7.247865 ,2.387893 ,-4.160253 ,-0.689278 ,-1.634285 ,-1.985255 ,-3.256250 ,-1.985255 ,-1.634285 ,-0.689278 ,-4.160253 ,2.387893 ,7.247865 ,2.387893 ,-4.160253 ,-0.689278 ,-1.634285 ,-1.985255
};


double bc_ff[24][24]={
0.078535 ,2.527110 ,1.944650 ,-2.027670 ,7.717775 ,8.823920 ,-8.823920 ,-7.717775 ,2.027670 ,-1.944650 ,-2.527110 ,-0.078535 ,0.078535 ,2.527110 ,1.944650 ,-2.027670 ,7.717775 ,8.823920 ,-8.823920 ,-7.717775 ,2.027670 ,-1.944650 ,-2.527110 ,-0.078535 ,
-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,
-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,
-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,
-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,
-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,
0.906625 ,-4.304120 ,-0.554155 ,0.479420 ,-1.572915 ,-3.286495 ,3.286495 ,1.572915 ,-0.479420 ,0.554155 ,4.304120 ,-0.906625 ,0.906625 ,-4.304120 ,-0.554155 ,0.479420 ,-1.572915 ,-3.286495 ,3.286495 ,1.572915 ,-0.479420 ,0.554155 ,4.304120 ,-0.906625 ,
-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,
-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,
-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,
-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,
-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,
0.078535 ,2.527110 ,1.944650 ,-2.027670 ,7.717775 ,8.823920 ,-8.823920 ,-7.717775 ,2.027670 ,-1.944650 ,-2.527110 ,-0.078535 ,0.078535 ,2.527110 ,1.944650 ,-2.027670 ,7.717775 ,8.823920 ,-8.823920 ,-7.717775 ,2.027670 ,-1.944650 ,-2.527110 ,-0.078535 ,
-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,
-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,
-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,
-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,
-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,
0.906625 ,-4.304120 ,-0.554155 ,0.479420 ,-1.572915 ,-3.286495 ,3.286495 ,1.572915 ,-0.479420 ,0.554155 ,4.304120 ,-0.906625 ,0.906625 ,-4.304120 ,-0.554155 ,0.479420 ,-1.572915 ,-3.286495 ,3.286495 ,1.572915 ,-0.479420 ,0.554155 ,4.304120 ,-0.906625 ,
-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,-3.151798 ,2.123997 ,-5.833992 ,-0.342025 ,-1.026575 ,-1.337193 ,1.337193 ,1.026575 ,0.342025 ,5.833992 ,-2.123997 ,3.151798 ,
-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,-0.008958 ,1.855235 ,0.763585 ,-1.365635 ,-2.926315 ,-0.516017 ,0.516017 ,2.926315 ,1.365635 ,-0.763585 ,-1.855235 ,0.008958 ,
-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,-1.496170 ,4.894420 ,-0.165773 ,-2.935290 ,-2.206968 ,1.454180 ,-1.454180 ,2.206968 ,2.935290 ,0.165773 ,-4.894420 ,1.496170 ,
-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,-1.946500 ,-0.574662 ,4.681507 ,1.191410 ,-3.699535 ,-4.619870 ,4.619870 ,3.699535 ,-1.191410 ,-4.681507 ,0.574662 ,1.946500 ,
-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460 ,-1.192460 ,2.176140 ,0.999643 ,1.443305 ,1.169630 ,3.963947 ,-3.963947 ,-1.169630 ,-1.443305 ,-0.999643 ,-2.176140 ,1.192460
};


double bd_ff[24][24]={
8.911830 ,8.990365 ,11.517475 ,13.462125 ,11.434455 ,19.152230 ,27.976150 ,19.152230 ,11.434455 ,13.462125 ,11.517475 ,8.990365 ,8.911830 ,8.990365 ,11.517475 ,13.462125 ,11.434455 ,19.152230 ,27.976150 ,19.152230 ,11.434455 ,13.462125 ,11.517475 ,8.990365 ,
12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,
14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,
13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,
13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,
18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,
17.227910 ,18.134535 ,13.830415 ,13.276260 ,13.755680 ,12.182765 ,8.896270 ,12.182765 ,13.755680 ,13.276260 ,13.830415 ,18.134535 ,17.227910 ,18.134535 ,13.830415 ,13.276260 ,13.755680 ,12.182765 ,8.896270 ,12.182765 ,13.755680 ,13.276260 ,13.830415 ,18.134535 ,
18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,
13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,
13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,
14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,
12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,
8.911830 ,8.990365 ,11.517475 ,13.462125 ,11.434455 ,19.152230 ,27.976150 ,19.152230 ,11.434455 ,13.462125 ,11.517475 ,8.990365 ,8.911830 ,8.990365 ,11.517475 ,13.462125 ,11.434455 ,19.152230 ,27.976150 ,19.152230 ,11.434455 ,13.462125 ,11.517475 ,8.990365 ,
12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,
14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,
13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,
13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,
18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,
17.227910 ,18.134535 ,13.830415 ,13.276260 ,13.755680 ,12.182765 ,8.896270 ,12.182765 ,13.755680 ,13.276260 ,13.830415 ,18.134535 ,17.227910 ,18.134535 ,13.830415 ,13.276260 ,13.755680 ,12.182765 ,8.896270 ,12.182765 ,13.755680 ,13.276260 ,13.830415 ,18.134535 ,
18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,18.590205 ,15.438408 ,17.562405 ,11.728413 ,11.386387 ,10.359813 ,9.022620 ,10.359813 ,11.386387 ,11.728413 ,17.562405 ,15.438408 ,
13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,13.837035 ,13.828077 ,15.683313 ,16.446897 ,15.081263 ,12.154947 ,11.638930 ,12.154947 ,15.081263 ,16.446897 ,15.683313 ,13.828077 ,
13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,13.149660 ,11.653490 ,16.547910 ,16.382137 ,13.446848 ,11.239880 ,12.694060 ,11.239880 ,13.446848 ,16.382137 ,16.547910 ,11.653490 ,
14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,14.975430 ,13.028930 ,12.454268 ,17.135775 ,18.327185 ,14.627650 ,10.007780 ,14.627650 ,18.327185 ,17.135775 ,12.454268 ,13.028930 ,
12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620 ,12.168080 ,10.975620 ,13.151760 ,14.151403 ,15.594708 ,16.764337 ,20.728285 ,16.764337 ,15.594708 ,14.151403 ,13.151760 ,10.975620
};


FILE *fp_plot,*fp_strc,*fp_acc,*fp_sys; // file pointers


//fuction names
double calc_is_energy();
int sr(int i);
double calc_cpl_energy(double seta_1,double seta_2);
double calc_y_energy(double seta1,double seta2);
double calc_z_energy(double seta1,double seta2);


int main()
{
	srand((unsigned)time(NULL));

	
	/*** initialization ***/
	
		
	//params initialize//
	
	//calculation params
	pi_2=pi*2.0;
	n_norm_factor=1.0/double(pow(N,3.0));
	agl_ff=6.0/pi;
	
	//angle refinement
	delta_angle_y=0.0;
	delta_angle_z=4.7124;

	
	//structure initialize//
			
	//start from an input snapshot
	fp_strc=fopen("input.txt","r+");	
	int temp;	
	for(int i=0;i<N;i++)
	{
		fscanf(fp_strc,"%d",&temp);
		for(int j=0;j<N;j++)
		{
			for(int k=0;k<N;k++)
			{
				fscanf(fp_strc,"%lf",&seta_is[i][j][k]);
				seta_fs[i][j][k]=seta_is[i][j][k];
			}
		}
	}	
	fclose(fp_strc);
	
	//starting structure output check	
	fp_strc=fopen("input.chk","w+");
	for(int i=0;i<N;i++)
	{
		fprintf(fp_strc,"%d %s",i,"\n");
		for(int j=0;j<N;j++)
		{
			for(int k=0;k<N;k++)
			{
				fprintf(fp_strc,"%lf\t",seta_fs[i][j][k]);
			}
			fprintf(fp_strc,"%s","\n");
		}
	}	
	fclose(fp_strc);
	
	
    /*** calculate energy ***/
	total_energy=calc_is_energy(); 
    fp_strc=fopen("ene.out","w+");
    fprintf(fp_strc,"%lf\n",total_energy);
    fclose(fp_strc);
	


	system("pause");
	return 0;

}




double calc_is_energy()
{
	double is_energy=0.0;
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			for(int k=0;k<N;k++)
			{
			    is_energy=is_energy+calc_cpl_energy(seta_is[i][j][k],seta_is[sr(i+1)][j][k])+calc_y_energy(seta_is[i][j][k],seta_is[i][sr(j+1)][k])+calc_z_energy(seta_is[i][j][k],seta_is[i][j][sr(k+1)]);
			}
		}
	}
	return is_energy;
} 


int sr(int i) // PBC order 
{
	if(i<0)
	{
		return N-1;
	}
	else if(i>N-1)
	{
		return 0;
	}
	else
	{
		return i;
	}
}


double calc_cpl_energy(double seta_1,double seta_2)//x_coupling energy (intrachain interaction) kcal/mol
{
	double agl,cpl_energy=0.0;
	agl=seta_1-seta_2;
	
	// keep the delta angle in (-pi,pi)
	if(agl<-pi)
	{
		agl=agl+pi_2;
	}
	else if(agl>pi)
	{
		agl=agl-pi_2;
	}
	else;
	
	for(int n=0;n<8;n++)
	{
		cpl_energy=cpl_energy+x_energy_factor[n]*pow(agl,n+1);
	} 

	return cpl_energy;
}



double calc_y_energy(double seta1,double seta2) // seta1:i,j,k; seta2:i,j+1,k 
{
	double alpha,beta,n,m,x,y;
	int n0,m0;
	
	alpha=seta1+delta_angle_y;
	beta=seta2+delta_angle_y;
	
	n=agl_ff*alpha;
	m=agl_ff*beta;
	
	n0=int(n);
	m0=int(m);
	
	x=n-n0;
	y=m-m0;
	
	return 0.25*(ac_ff[n0][m0]*x*y+ad_ff[n0][m0]*x+bc_ff[n0][m0]*y+bd_ff[n0][m0]);
}




double calc_z_energy(double seta1,double seta2) // seta1:i,j,k; seta2:i,j,k+1
{
	double alpha,beta,n,m,x,y;
	int n0,m0;
	
	alpha=seta1+delta_angle_z;
	beta=seta2+delta_angle_z;
	
	n=agl_ff*alpha;
	m=agl_ff*beta;
	
	n0=int(n);
	m0=int(m);
	
	x=n-n0;
	y=m-m0;
	
	return 0.25*(ac_ff[n0][m0]*x*y+ad_ff[n0][m0]*x+bc_ff[n0][m0]*y+bd_ff[n0][m0]); 		
}

