// _  _ ____ _    ____ ___ _
//  \/  |  | |    |  |  |  |
// _/\_ |__| |___ |__|  |  |___
//
// component source [O'Leary et al. 2014](https://www.sciencedirect.com/science/article/pii/S089662731400292X)
// component info: Integral controller of conductances and synapses

#ifndef INTEGRALCONTROLLER
#define INTEGRALCONTROLLER
#include "mechanism.hpp"
#include <limits>

//inherit controller class spec
class IntegralController: public mechanism {

protected:
    // flag used to switch between
    // controlling channels and synapses
    // meaning:
    // 0 --- unset, will throw an error
    // 1 --- channels
    // 2 --- synapses
    int control_type = 0;
    double Target = 0;
public:
    // timescales
    double tau_m = std::numeric_limits<double>::infinity();
    double tau_g = 5e3;

    // mRNA concentration
    double m = 0;

    // area of the container this is in
    double container_A;

    // specify parameters + initial conditions for
    // mechanism that controls a conductance
    IntegralController(double tau_m_, double tau_g_, double m_) {

        tau_m = tau_m_;
        tau_g = tau_g_;
        m = m_;


        // if (tau_m<=0) {mexErrMsgTxt("[IntegralController] tau_m must be > 0. Perhaps you meant to set it to Inf?\n");}
        if (tau_g<=0) {mexErrMsgTxt("[IntegralController] tau_g must be > 0. Perhaps you meant to set it to Inf?\n");}
    }


    void integrate(void);

    void checkSolvers(int);

    void init(void);

    void connect(conductance *);
    void connect(synapse*);
    void connect(compartment*);

    int getFullStateSize(void);
    int getFullState(double * cont_state, int idx);
    double getState(int);
    string getClass(void);

};


void IntegralController::init() {
    
    compartment* temp_comp;

    if (control_type == 1) {
        temp_comp = channel->container;
        
    } else if (control_type == 2) {
        temp_comp  = syn->post_syn;
    } else {
        mexErrMsgTxt("IntegralController can only control conductances or synapses\n");
    }

    int n_mech = temp_comp->n_mech;

    bool targetMissing = true;

    for (int i = 0; i < n_mech; i++) {

        string this_mech = temp_comp->getMechanismPointer(i)->getClass().c_str();

        if (this_mech == "CalciumTarget") {
            if (verbosity==0) {
                mexPrintf("IntegralController(%s) connected to [CalciumTarget]\n",controlling_class.c_str());
            }

            Target = temp_comp->getMechanismPointer(i)->getState(0);
            
            targetMissing = false;
        }
    }

    // attempt to read Ca_target from compartment -- legacy code support
    if (targetMissing) {
        Target = temp_comp->Ca_target;
    }
}

string IntegralController::getClass() {
    return "IntegralController";
}


double IntegralController::getState(int idx) {
    if (idx == 1) {return m;}
    else if (idx == 2) {return channel->gbar;}
    else {return std::numeric_limits<double>::quiet_NaN();}

}


int IntegralController::getFullStateSize(){return 2; }


int IntegralController::getFullState(double *cont_state, int idx) {
    // give it the current mRNA level
    cont_state[idx] = m;

    idx++;

    // and also output the current gbar of the thing
    // being controller
    if (channel) {
      cont_state[idx] = channel->gbar;
    }
    else if (syn) {
        cont_state[idx] = syn->gmax;
    }
    idx++;
    return idx;
}


void IntegralController::connect(conductance * channel_) {

    // connect to a channel
    channel = channel_;


    // make sure the compartment that we are in knows about us
    (channel->container)->addMechanism(this);



    controlling_class = (channel_->getClass()).c_str();

    // attempt to read the area of the container that this
    // controller should be in.
    container_A  = (channel->container)->A;

    control_type = 1;


}

void IntegralController::connect(compartment* comp_) {
    mexErrMsgTxt("[IntegralController] This mechanism cannot connect to a compartment object");
}

void IntegralController::connect(synapse* syn_) {

    // connect to a synpase
    syn = syn_;


    // make sure the compartment that we are in knows about us
    (syn->post_syn)->addMechanism(this);


    // attempt to read the area of the container that this
    // controller should be in.
    container_A  = (syn->post_syn)->A;

    control_type = 2;

}


void IntegralController::integrate(void) {


    switch (control_type) {
        case 0:
            mexErrMsgTxt("[IntegralController] misconfigured controller. Make sure this object is contained by a conductance or synapse object");
            break;


        case 1:

            {
            // if the target is NaN, we will interpret this
            // as the controller being disabled
            // and do nothing
            if (isnan(Target)) {return;}

            double Ca_error = Target - (channel->container)->Ca_prev;

            // integrate mRNA
            m += (dt/tau_m)*(Ca_error);

            // mRNA levels below zero don't make any sense
            if (m < 0) {m = 0;}

            // copy the protein levels from this channel
            double gdot = ((dt/tau_g)*(m - channel->gbar*container_A));

            // make sure it doesn't go below zero
            if (channel->gbar + gdot/container_A < 0) {
                channel->gbar = 0;
            } else {
                channel->gbar += gdot/container_A;
            }


            break;

            }
        case 2:
            {
            // if the target is NaN, we will interpret this
            // as the controller being disabled
            // and do nothing

            if (isnan(Target)) {return;}

            double Ca_error = Target - (syn->post_syn)->Ca_prev;

            // integrate mRNA
            m += (dt/tau_m)*(Ca_error);

            // mRNA levels below zero don't make any sense
            if (m < 0) {m = 0;}

            // copy the protein levels from this syn
            double gdot = ((dt/tau_g)*(m - syn->gmax*1e-3));

            // make sure it doesn't go below zero
            if (syn->gmax + gdot*1e3 < 0) {
                syn->gmax = 0;
            } else {
                syn->gmax += gdot*1e3;
            }


            break;

            }

        default:
            mexErrMsgTxt("[IntegralController] misconfigured controller");
            break;

    }


}



void IntegralController::checkSolvers(int k) {
    if (k == 0){
        return;
    } else {
        mexErrMsgTxt("[IntegralController] unsupported solver order\n");
    }
}




#endif
