#ifndef CVT_MAP_ELITES_STAT_MAP_PROGRESS_HPP_
#define CVT_MAP_ELITES_STAT_MAP_PROGRESS_HPP_

#include <boost/multi_array.hpp>
#include <numeric>
#include <sferes/stat/stat.hpp>

namespace sferes {
    namespace stat {
        SFERES_STAT(MapProgress, Stat)
        {
        public:
            typedef boost::shared_ptr<Phen> phen_t;
            typedef boost::array<float, Params::ea::number_of_dimensions> point_t;

            size_t number_of_dimensions;

            MapProgress() : number_of_dimensions(Params::ea::number_of_dimensions) {}

            template <typename E>
            void refresh(const E& ea)
            {
                this->_create_log_file(ea, "progress_archive.dat");
                _write_progress(ea, *this->_log_file);
            }

            template <class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar& BOOST_SERIALIZATION_NVP(number_of_dimensions);
            }

        protected:
            template <typename EA>
            void _write_progress(const EA& ea, std::ofstream& ofs) const
            {
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

                // dividing by archive_size might be problematic if it is =0
                // archive_mean /= archive_size;
                archive_mean /= archive.size();

                ofs << ea.gen() << " " << ea.nb_evals() << " " << archive_size << " "
                    << archive_min << " " << archive_mean << " " << archive_max
                    << std::endl;
            }
        };
    } // namespace stat
} // namespace sferes

#endif
