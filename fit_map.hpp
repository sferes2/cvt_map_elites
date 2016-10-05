
#include <sferes/fit/fitness.hpp>

#define FIT_MAP(Name) SFERES_FITNESS(Name, sferes::fit::FitMap)

namespace sferes {
namespace fit {
SFERES_FITNESS(FitMap, sferes::fit::Fitness) {
public:
  FitMap() : _desc(Params::ea::number_of_dimensions, -9.9f) {}

  const std::vector<float> &desc() const { return _desc; }

  void set_desc(std::vector<float> & x) {
    assert(x.size() == Params::ea::number_of_dimensions);
    _desc = x;
  }

protected:
  std::vector<float> _desc;
};
}
}
