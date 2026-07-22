#pragma once

#include <nlohmann/json.hpp>

namespace syrius_orbit::vda5050 {

enum class BlockingType {
  NONE,   // allows driving and other actions;
  SINGLE, // allows driving but no other actions;
  SOFT,   // allows other actions but not driving;
  HARD,   // is the only allowed action at that time.
};

NLOHMANN_JSON_SERIALIZE_ENUM(BlockingType, {
                                               {BlockingType::NONE, "NONE"},
                                               {BlockingType::SOFT, "SOFT"},
                                               {BlockingType::SINGLE, "SINGLE"},
                                               {BlockingType::HARD, "HARD"},
                                           });

struct BoundingBox {
  double x{};
  double y{};
  double z{};
  std::optional<double> theta{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(BoundingBox, x, y, z, theta);

struct Vertex {
  double x{};
  double y{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vertex, x, y);

} // namespace syrius_orbit::vda5050