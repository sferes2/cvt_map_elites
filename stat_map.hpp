#ifndef CVT_MAP_ELITES_STAT_MAP_HPP_
#define CVT_MAP_ELITES_STAT_MAP_HPP_

#include <boost/multi_array.hpp>
#include <limits>
#include <numeric>
#include <sferes/stat/stat.hpp>

#define MAP_WRITE_PARENTS

namespace sferes {
namespace stat {
SFERES_STAT(Map, Stat) {
public:
  typedef boost::shared_ptr<Phen> phen_t;
  typedef boost::array<float, Params::ea::number_of_dimensions> point_t;

  size_t number_of_dimensions;

  Map() : number_of_dimensions(Params::ea::number_of_dimensions) {}

  template <typename E> void refresh(const E &ea) {
    this->_create_log_file(ea, "progress_archive.dat");
    _write_progress(ea, *this->_log_file);

    if (ea.gen() % Params::pop::dump_period == 0) {
      _write_archive(ea.archive(), std::string("archive_"), ea);
    }
  }

  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar &BOOST_SERIALIZATION_NVP(number_of_dimensions);
  }

protected:
  template <typename EA>
  void _write_archive(const std::vector<phen_t> &archive,
                      const std::string &prefix, const EA &ea) const {
    std::cout << "writing..." << prefix << ea.gen() << std::endl;
    std::string fname = ea.res_dir() + "/" + prefix +
                        boost::lexical_cast<std::string>(ea.gen()) +
                        std::string(".dat");

    std::ofstream ofs(fname.c_str());

    for (size_t i = 0; i < archive.size(); ++i) {
      if (archive[i]) {
        // Write the descriptor
        std::vector<float> desc = archive[i]->fit().desc();
        for (size_t j = 0; j < number_of_dimensions; ++j)
          ofs << std::setprecision(3) << desc[j] << " ";

        // Write the fitness
        ofs << " " << archive[i]->fit().value() << "  ";

        // Write the genotype
        std::vector<float> genotype = archive[i]->data();
        for (size_t j = 0; j < genotype.size(); ++j)
          ofs << std::setprecision(50) << genotype[j] << " ";

        ofs << std::endl;
      }
    }
  }

  template <typename EA>
  void _write_progress(const EA &ea, std::ofstream &ofs) const {
    double archive_min = std::numeric_limits<double>::max();
    double archive_max = std::numeric_limits<double>::lowest();
    double archive_mean = 0.0;
    size_t archive_size = 0;

    std::vector<phen_t> archive = ea.archive();

    for (size_t i = 0; i < archive.size(); ++i) {
      if (archive[i]) {
        archive_size++;

        archive_mean += archive[i]->fit().value();

        if (archive[i]->fit().value() < archive_min)
          archive_min = archive[i]->fit().value();

        if (archive[i]->fit().value() > archive_max)
          archive_max = archive[i]->fit().value();
      }
    }

    archive_mean /= archive.size();

    ofs << ea.gen() << " " << ea.nb_evals() << " " << archive_size << " "
        << archive_min << " " << archive_mean << " " << archive_max
        << std::endl;
  }
};
}
}

#endif
