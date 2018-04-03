#ifndef CVT_MAP_ELITES_BINARY_MAP_HPP_
#define CVT_MAP_ELITES_BINARY_MAP_HPP_

#include <array>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/multi_array.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

// this is c++11!

// this header can be included in your project to load maps generated by map-elites in binary (whithout any loss of precision)
// it does not depends on sferes
// it works only for EvoFloat and Sampled genotypes!
namespace cvt {
    namespace binary_map {
        struct Elem {
            std::vector<float> desc;
            std::vector<float> phen;
            float fit;
            template <class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar& BOOST_SERIALIZATION_NVP(desc);
                ar& BOOST_SERIALIZATION_NVP(phen);
                ar& BOOST_SERIALIZATION_NVP(fit);
            }
        };

        struct BinaryMap {
            std::vector<Elem> elems;
            std::vector<float> dims;

            template <class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar& BOOST_SERIALIZATION_NVP(elems);
                ar& BOOST_SERIALIZATION_NVP(dims);
            }
        };

        static void write(const BinaryMap& m, const std::string& filename)
        {
            std::ofstream ofs(filename.c_str());
            assert(ofs.good());
            std::cout << "writing : " << filename << std::endl;
            boost::archive::binary_oarchive oa(ofs);
            oa& BOOST_SERIALIZATION_NVP(m);
            std::cout << "done" << std::endl;
        };

        static BinaryMap load(const std::string& filename)
        {
            std::ifstream ifs(filename.c_str());
            assert(ifs.good());
            boost::archive::binary_iarchive ia(ifs);
            BinaryMap data;
            ia& BOOST_SERIALIZATION_NVP(data);
            return data;
        }
    } // namespace binary_map
} // namespace cvt

#endif
