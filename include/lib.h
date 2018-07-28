#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <armadillo>
#include <random>
#include <string>

const double two_pi = 2.0*3.14159265358979323846;

//global variables that are set in lib.C
extern int ntrajs;
extern int nsteps;
extern double dt, dt2;

extern int qdim;
extern int cdim;
extern double jA, jlx, jbx; //Defines Hamil in Joe's code

extern double phase;
extern arma::vec sigx;
extern arma::vec sigp;
extern arma::vec x0;
extern arma::vec p0;

//lib.C functions
void read_input(void); //read run.in
void boxmuller (arma::vec& randvec); //make random normal noise
