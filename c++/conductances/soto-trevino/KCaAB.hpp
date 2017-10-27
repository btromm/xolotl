// _  _ ____ _    ____ ___ _    
//  \/  |  | |    |  |  |  |    
// _/\_ |__| |___ |__|  |  |___ 
//
// Calcium-dep potassium
// http://jn.physiology.org/content/94/1/590.short
#ifndef KCAAB
#define KCAAB
#include "../../conductance.hpp"

//inherit conductance class spec
class KCaAB: public conductance {

public:

    // specify parameters + initial conditions 
    KCaAB(double g_, double E_, double m_, double h_)
    {
        gbar = g_;
        E = E_;
        m = m_;
        h = 1;
    }
    
    KCaAB(double g_, double E_, double m_)
    {
        gbar = g_;
        E = E_;
        m = m_;
        h = 1;
    }
    
    void integrate(double V, double Ca, double dt);
    void connect(compartment *pcomp_);
    double m_inf(double V, double Ca);
    double tau_m(double V);
};

void KCaAB::connect(compartment *pcomp_) {container = pcomp_; }

void KCaAB::integrate(double V, double Ca, double dt)
{
    m = m_inf(V,Ca) + (m - m_inf(V,Ca))*exp(-dt/tau_m(V));
    g = gbar*m*m*m*m;
}

double KCaAB::m_inf(double V, double Ca) { return (Ca/(Ca+30))/(1.0+exp(-(V+51.0)/4)); }
double KCaAB::tau_m(double V) {return 90.3 - 75.09/(1.0+exp(-(V+46.0)/22.7));}


#endif