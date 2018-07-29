#include "lib.h"
#include <ctime>

/***********************************************/
namespace sys {

arma::cx_mat Ve;
arma::cx_mat Ue;
arma::cx_cube Fe;
arma::vec Fsurf;
arma::vec eigs;

/* * * * * * * * * * * * * * * * * * * * * * * */
void HamilD(traj &curr_traj) {

  // Hamiltonian with flat adiabats
  double theta = two_pi * (jlx * std::erf(jbx * curr_traj.x(0)) + 1.0) / 4.0;
  double dthetadx0 =
      two_pi * jlx * jbx * std::exp(std::pow(jbx * curr_traj.x(0), 2)) / 4.0;
  double phi = 0.3 * jlx * curr_traj.x(1);
  double dphidx1 = 0.3 * jlx;

  Ve.zeros(qdim, qdim);
  Ve(0, 0) = jA * std::cos(theta);
  Ve(1, 1) = -jA * std::cos(theta);
  Ve(0, 1) = jA * std::sin(theta) * std::exp(arma::cx_double(0, phi));
  Ve(1, 0) = conj(Ve(0, 1));

  Fe.zeros(qdim, qdim, cdim);
  Fe(0, 0, 0) = -jA * std::sin(theta) * dthetadx0;
  Fe(1, 1, 0) = jA * std::sin(theta) * dthetadx0;
  Fe(0, 1, 0) =
      jA * std::cos(theta) * std::exp(arma::cx_double(0, phi)) * dthetadx0;
  Fe(1, 0, 0) = conj(Fe(0, 1, 0));

  Fe(0, 1, 1) = jA * std::sin(theta) * std::exp(arma::cx_double(0, phi)) *
                arma::cx_double(0, dphidx1);
  Fe(1, 0, 1) = conj(Fe(0, 1, 1));

  return;
}

/* * * * * * * * * * * * * * * * * * * * * * * */
void HamilA(traj &curr_traj) {

  // update Ve
  HamilD(curr_traj);

  double V12_sq = real(Ve(0, 1) * Ve(1, 0));
  // double Tra = 0;
  double Det = real(Ve(0, 0) * Ve(1, 1) - V12_sq);

  eigs.zeros(qdim);
  // eigs(0) = 0.5*(Tra - std::sqrt(std::pow(Tra,2) - 4.0*Det));
  // eigs(1) = 0.5*(Tra + std::sqrt(std::pow(Tra,2) - 4.0*Det));
  eigs(0) = -std::sqrt(-Det);
  eigs(1) = std::sqrt(-Det);

  Ue.zeros(qdim, qdim);
  double wave_norm = std::sqrt(std::pow(real(eigs(0) - Ve(1, 1)), 2) + V12_sq);
  Ue(0, 0) = (eigs(0) - Ve(1, 1)) / wave_norm;
  Ue(1, 0) = Ve(1, 0) / wave_norm;
  wave_norm = std::sqrt(std::pow(real(eigs(1) - Ve(1, 1)), 2) + V12_sq);
  Ue(0, 1) = (eigs(1) - Ve(1, 1)) / wave_norm;
  Ue(1, 1) = Ve(1, 0) / wave_norm;

  Fsurf.zeros(cdim);

  return;
}

} // namespace sys
namespace fssh {
/* * * * * * * * * * * * * * * * * * * * * * * */
void traj::propagate() {
  // x + p/m*dt/2
  x = x + p * dt2;

  // update Hamil to get Fsurf
  sys::HamilA();
  // p + F*dt
  p = p + (sys::Fsurf)*dt;

  // x + p/m*dt/2
  x = x + p * dt2;

  return;
}
} // namespace fssh

/***********************************************/
int main() {

  const clock_t begin_time = std::clock();

  read_input();

  for (int itraj = 0; itraj < ::ntrajs; itraj++) {
    traj curr_traj;
    curr_traj.initial();

    for (int istep = 0; istep < nsteps; istep++) {
      fssh::propagate();
    }
    std::cout << "traj: " << itraj << std::endl;
  }

  std::cout << "This calculation took "
            << float(std::clock() - begin_time) / CLOCKS_PER_SEC << " secs."
            << std::endl;

  return 0;
}
