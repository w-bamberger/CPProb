#include <boost/python/module.hpp>

namespace cpprob
{
  namespace proby
  {

    void
    export_bayesian_network();

    void
    export_categorical_distribution();

    void
    export_categorical_node();

    void
    export_conditional_categorical_node();

    void
    export_conditional_dirichlet_node();

    void
    export_constant_node();

    void
    export_dirichlet_node();

    void
    export_dirichlet_process_node();

    void
    export_dirichlet_process_parameters();

    void
    export_discrete_joint_random_variable();

    void
    export_discrete_node();

    void
    export_discrete_random_variable();

    void
    export_random_boolean();

    void
    export_random_conditional_probabilities();

    void
    export_random_integer();

    void
    export_random_probabilities();
  }
}

BOOST_PYTHON_MODULE (_proby)
{
  using namespace cpprob::proby;
  using namespace boost::python;

  /* Random variables. */
  export_discrete_random_variable();
  export_discrete_joint_random_variable();
  export_random_boolean();
  export_random_conditional_probabilities();
  export_random_integer();
  export_random_probabilities();

  /* Distributions. */
  export_categorical_distribution();

  /* Nodes. */
  export_discrete_node();
  export_categorical_node();
  export_conditional_categorical_node();
  export_conditional_dirichlet_node();
  export_dirichlet_node();
  export_dirichlet_process_node();
  export_dirichlet_process_parameters();
  export_constant_node();

  /* Model. */
  export_bayesian_network();
}
