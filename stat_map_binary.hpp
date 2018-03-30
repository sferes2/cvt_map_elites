#ifndef STAT_MAP_BINARY_HPP
#define STAT_MAP_BINARY_HPP

#include <boost/multi_array.hpp>
#include <numeric>
#include <sferes/stat/stat.hpp>

#include "cvt_binary_map.hpp"

namespace sferes {
    namespace stat {

        // - write in generic binary format
        // - work only for evofloat & sampled !
        // (the binary map in sferes form is in gen_* if you have StatMap)
        SFERES_STAT(MapBinary, Stat){
            public :
                template <typename E>
                void refresh(const E& ea){
                    if (ea.gen() % Params::pop::dump_period != 0) return;
        std::string fname = ea.res_dir() + "/"
            + "map_" + boost::lexical_cast<std::string>(ea.gen())
            + std::string(".bin");
        auto archive = _multi_array_to_vector(ea.archive(), ea);
        cvt::binary_map::write(archive, fname);
    } // namespace stat

protected:
    // convert a (possibly sparse) boost::multi_array of phenotypes
    // to a list of vectors that can be used in other software
    template <typename A, typename EA>
    cvt::binary_map::BinaryMap _multi_array_to_vector(const A& archive, const EA& ea)
    {
        cvt::binary_map::BinaryMap map;
        map.elems.clear();

        for (size_t i = 0; i < archive.size(); ++i) {
            if (archive[i]) {
                // Write the descriptor
                std::vector<float> desc = archive[i]->fit().desc();
                cvt::binary_map::Elem elem;
                elem.desc = desc;
                elem.fit = archive[i]->fit().value();
                elem.phen = archive[i]->data();
                map.elems.push_back(elem);
            }
        }
        return map;
    }
}; // namespace sferes
}
}

#endif
