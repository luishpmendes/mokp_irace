#pragma once

#include "solver/solver.hpp"

namespace mokp {
/**********************************************
 * The IHS_Solver represents a solver for the
 * Multi-Objective Binary Knapsack Problem
 * using the Improved Harmony Search.
 **********************************************/
class IHS_Solver : public Solver {
    public:
    /*******************************
     * Size of the population.
     *******************************/
    unsigned population_size = 136;

    /***************************************
     * Probability of choosing from memory.
     ***************************************/
    double phmcr = 0.985368;

    /*********************************
     * Minimum pitch adjustment rate.
     *********************************/
    double ppar_min = 0.19737;

    /*********************************
     * Maximum pitch adjustment rate.
     *********************************/
    double ppar_max = 0.728174;

    /******************************
     * Minimum distance bandwidth.
     ******************************/
    double bw_min = 0.021088;

    /******************************
     * Maximum distance bandwidth.
     ******************************/
    double bw_max = 0.338321;

    /*********************************************
     * Constructs a new solver.
     *
     * @param instance the instance to be solved.
     *********************************************/
    IHS_Solver(const Instance & instance);

    /*********************************
     * Constructs a new empty solver.
     *********************************/
    IHS_Solver();

    /**********************
     * Solve the instance.
     **********************/
    void solve();

    /***************************************************************
     * Standard stream operator.
     *
     * @param os the standard output stream object.
     * @param solver the solver.
     *
     * @return the stream object.
     ***************************************************************/
    friend std::ostream & operator <<(std::ostream & os,
                                      const IHS_Solver & solver);
};

}
