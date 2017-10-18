// _  _ ____ _    ____ ___ _    
//  \/  |  | |    |  |  |  |    
// _/\_ |__| |___ |__|  |  |___ 
//
// the A current, a potassium current 
// for some reason I don't understand, I get compiler 
// errors when I name this "A" or "Ka"
// so we'll have to live with this awkward name
// http://www.jneurosci.org/content/jneuro/18/7/2309.full.pdf
#ifndef ACURRENT
#define ACURRENT
#include "../../conductance.hpp"

//inherit conductance class spec
class ACurrent: public conductance {

public:

    // specify parameters + initial conditions 
    ACurrent(double g_, double E_, double m_, double h_)
    {
        gbar = g_;
        E = E_;
        m = m_;
        h = h_;
    }
    
    void integrate(double V, double Ca, double dt);
    void connect(compartment *pcomp_);
    double m_inf(double V);
    double h_inf(double V);
    double tau_m(double V);
    double tau_h(double V); 
};

void ACurrent::connect(compartment *pcomp_) {container = pcomp_;}

void ACurrent::integrate(double V, double Ca, double dt)
{
    m = m_inf(V) + (m - m_inf(V))*exp(-dt/tau_m(V));
    h = h_inf(V) + (h - h_inf(V))*exp(-dt/tau_h(V));
    g = gbar*m*m*m*h;
}

double ACurrent::m_inf(double V) {return 1.0/(1.0+exp((V+27.2)/-8.7)); }
double ACurrent::h_inf(double V) {return 1.0/(1.0+exp((V+56.9)/4.9)); }
double ACurrent::tau_m(double V) {return 11.6 - 10.4/(1.0+exp((V+32.9)/-15.2));}
double ACurrent::tau_h(double V) {return 38.6 - 29.2/(1.0+exp((V+38.9)/-26.5));}


#endif