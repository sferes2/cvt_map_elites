
// Centroidal Voronoi Tessellation - MAP-Elites
// Vassilis Vassiliades - July 2016

#ifndef CVT_MAP_ELITES_HPP_
#define CVT_MAP_ELITES_HPP_

#include <algorithm>
#include <limits>

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <sferes/ea/ea.hpp>
#include <sferes/fit/fitness.hpp>
#include <sferes/parallel.hpp>
#include <sferes/stc.hpp>

namespace sferes {
namespace ea {
SFERES_EA(CVTMapElites, Ea) {
public:
  static constexpr size_t number_of_dimensions =
      Params::ea::number_of_dimensions;

  typedef boost::shared_ptr<Phen> indiv_t;
  typedef typename std::vector<indiv_t> pop_t;
  typedef boost::array<double, number_of_dimensions> point_t;

  static constexpr size_t number_of_clusters = Params::ea::number_of_clusters;
  std::vector<point_t> centroids;

  CVTMapElites() {
    _archive.resize(number_of_clusters);
    centroids = Params::ea::centroids;
  }

  void random_pop() {
    parallel::init();
    this->_pop.resize(Params::pop::init_size);
    BOOST_FOREACH (boost::shared_ptr<Phen> &indiv, this->_pop) {
      indiv = boost::shared_ptr<Phen>(new Phen());
      indiv->random();
    }
    this->_eval_pop(this->_pop, 0, this->_pop.size());
    BOOST_FOREACH (boost::shared_ptr<Phen> &indiv, this->_pop)
      _add_to_archive(indiv);
  }

  void epoch() {
    this->_pop.clear();

    for (size_t i = 0; i < _archive.size(); ++i)
      if (_archive[i])
        this->_pop.push_back(_archive[i]);

    pop_t ptmp;
    for (size_t i = 0; i < Params::pop::size; ++i) {
      indiv_t p1 = _selection(this->_pop);
      indiv_t p2 = _selection(this->_pop);
      boost::shared_ptr<Phen> i1, i2;
      p1->cross(p2, i1, i2);
      i1->mutate();
      i2->mutate();
      i1->develop();
      i2->develop();
      ptmp.push_back(i1);
      ptmp.push_back(i2);
    }
    this->_eval_pop(ptmp, 0, ptmp.size());

    for (size_t i = 0; i < ptmp.size(); ++i)
      _add_to_archive(ptmp[i]);
  }

  const std::vector<indiv_t> &archive() const { return _archive; }

  template <typename I> point_t get_point(const I &indiv) const {
    return _get_point(indiv);
  }

protected:
  std::vector<indiv_t> _archive;

  bool _add_to_archive(indiv_t i1) {
    if (i1->fit().dead())
      return false;

    point_t p = _get_point(i1);

    // Find the closest cluster
    size_t archive_index = -1;
    double min_dist = std::numeric_limits<double>::max();

    for (size_t i = 0; i < centroids.size(); ++i) {
      double dist = _calc_dist(centroids[i], p);

      if (dist < min_dist) {
        min_dist = dist;
        archive_index = i;
      }
    }

    // If the archive is empty or the stored individual is less fit
    if (!_archive[archive_index] ||
        i1->fit().value() > _archive[archive_index]->fit().value()) {
      _archive[archive_index] = i1;
      return true;
    }

    return false;
  }

  // Euclidean distance
  double _calc_dist(const point_t &p1, const point_t &p2) {
    double dist = 0.0;

    for (size_t i = 0; i < number_of_dimensions; ++i)
      dist += pow(p1[i] - p2[i], 2);

    return sqrt(dist);
  }

  template <typename I> point_t _get_point(const I &indiv) const {
    point_t p;
    for (size_t i = 0; i < number_of_dimensions; ++i)
      p[i] = indiv->fit().desc()[i];

    return p;
  }

  indiv_t _selection(const pop_t &pop) {
    int x1 = misc::rand<int>(0, pop.size());
    return pop[x1];
  }
};
}
}
#endif
