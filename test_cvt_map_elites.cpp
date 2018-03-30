
#include <cmath>
#include <iostream>

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/multi_array.hpp>

#include <boost/test/unit_test.hpp>

#include <sferes/eval/parallel.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/run.hpp>
#include <sferes/stat/best_fit.hpp>

#include "cvt_map_elites.hpp"
#include "fit_map.hpp"
#include "stat_map.hpp"
#include "stat_map_binary.hpp"

#ifndef NO_PARALLEL
#include <sferes/eval/parallel.hpp>
#else
#include <sferes/eval/eval.hpp>
#endif

using namespace sferes::gen::evo_float;

struct Params {
    struct ea {
        SFERES_CONST size_t number_of_clusters = 10000;
        SFERES_CONST size_t number_of_dimensions = 2;

        typedef boost::array<double, number_of_dimensions> point_t;
        static std::vector<point_t> centroids;
    };

    struct pop {
        // number of initial random points
        SFERES_CONST size_t init_size = 100;
        // size of a batch
        SFERES_CONST size_t size = 100;
        SFERES_CONST size_t nb_gen = 5001;
        SFERES_CONST size_t dump_period = 500;
    };
    struct parameters {
        SFERES_CONST float min = -5;
        SFERES_CONST float max = 5;
    };
    struct evo_float {
        SFERES_CONST float cross_rate = 0.25f;
        SFERES_CONST float mutation_rate = 0.1f;
        SFERES_CONST float eta_m = 10.0f;
        SFERES_CONST float eta_c = 10.0f;
        SFERES_CONST mutation_t mutation_type = polynomial;
        SFERES_CONST cross_over_t cross_over_type = sbx;
    };
};

// Rastrigin
FIT_MAP(Rastrigin){public : template <typename Indiv>
    void eval(Indiv & ind){float f = 10 * ind.size();
for (size_t i = 0; i < ind.size(); ++i)
    f += ind.data(i) * ind.data(i) - 10 * cos(2 * M_PI * ind.data(i));
this->_value = -f;

std::vector<float> data;
data.push_back(ind.gen().data(0));
data.push_back(ind.gen().data(1));

this->set_desc(data);
}

bool dead() { return false; }
};

typedef Params::ea::point_t point_t;

// define the centroids
std::vector<point_t> Params::ea::centroids;

std::vector<point_t> load_centroids(const std::string& centroids_filename)
{
    std::vector<point_t> centroids;

    std::ifstream fin(centroids_filename.c_str());

    if (!fin) {
        std::cerr << "Error: Could not load the centroids." << std::endl;
        exit(1);
    }

    std::vector<std::string> lines;

    std::string line;
    while (std::getline(fin, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    fin.close();

    if (lines.size() != Params::ea::number_of_clusters) {
        std::cerr << "Error: The number of clusters "
                  << Params::ea::number_of_clusters
                  << " is not equal to the number of loaded elements "
                  << lines.size() << ".\n";
        exit(1);
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        std::vector<std::string> cols;

        std::string temp;
        std::istringstream stringStream;
        stringStream.str(lines[i]);

        while (stringStream >> temp)
            cols.push_back(temp);

        if (cols.size() != Params::ea::number_of_dimensions) {
            std::cerr << "Error: The number of dimensions "
                      << Params::ea::number_of_dimensions
                      << " is not equal to the dimensionality (" << cols.size()
                      << ") of loaded element with index " << i << ".\n";
            exit(1);
        }

        point_t p;
        for (size_t j = 0; j < cols.size(); ++j)
            p[j] = atof(cols[j].c_str());

        centroids.push_back(p);
    }

    std::cout << "\nLoaded " << centroids.size() << " centroids.\n";

    return centroids;
}

int main(int argc, char** argv)
{
    using namespace sferes;

    if (argc == 1) {
        std::cerr << "\nError: please provide the centroids filename.\n";
        exit(1);
    }

    if (argc > 2) {
        std::cerr << "\nError: the number of arguments must be 1.\n";
        exit(1);
    }

    Params::ea::centroids = load_centroids(argv[1]);

    typedef Rastrigin<Params> fit_t;
    typedef gen::EvoFloat<10, Params> gen_t;
    typedef phen::Parameters<gen_t, fit_t, Params> phen_t;
    typedef eval::Parallel<Params> eval_t;
    typedef boost::fusion::vector<stat::Map<phen_t, Params>, stat::MapBinary<phen_t, Params>> stat_t;
    typedef modif::Dummy<> modifier_t;
    typedef ea::CVTMapElites<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;

    ea_t ea;
    ea.run();
    return 0;
}
