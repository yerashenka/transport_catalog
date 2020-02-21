#pragma once

#include "transport_database.h"
#include "svg.h"
#include <limits>
#include <vector>
#include <utility>

namespace Visualisation {
class Projector {
 public:
  virtual ~Projector() = default;
  virtual Svg::Point ProjectStop(const std::string &stop_name) const = 0;
};


class GeoProjector : public Projector {
 public:
  ~GeoProjector() override = default;
  GeoProjector(const TransportDatabase::Database &db, double width,
               double height, double padding);

  Svg::Point ProjectStop(const std::string &stop_name) const override;

 private:
  const TransportDatabase::Database &db_;
  double padding_{0.};
  double zoom_coef_{0.};

  double min_lon_{std::numeric_limits<double>::max()};
  double max_lat_{std::numeric_limits<double>::min()};
};

class UniformProjector : public Projector {
private:
  struct StopPosition {
    std::string stop_name;
    Location::Point position;
  };

 public:
  ~UniformProjector() override = default;
  UniformProjector(const TransportDatabase::Database &db, double width,
                   double height, double padding);

  Svg::Point ProjectStop(const std::string &stop_name) const override {
    return stop_projection_.at(stop_name);
  }

 private:
  const TransportDatabase::Database &db_;
  const double height_;
  const double width_;
  const double padding_;

  std::map<Location::Point, size_t> x_stop_indexing_;
  std::map<Location::Point, size_t> y_stop_indexing_;
  std::map<std::string, Svg::Point> stop_projection_;
  std::unordered_set<std::string> reference_stops_;

  std::vector<StopPosition> ComputeUniformStopPositions(const TransportDatabase::Database &db);
  std::pair<size_t, size_t> CoordinatesIndexing(std::vector<StopPosition> &stop_positions);

  std::map<std::string, Svg::Point>
  CoordinatesCompression(const std::vector<StopPosition> &stop_positions, size_t x_count, size_t y_count) const ;

  void FindReferenceStops(const TransportDatabase::Database &db);
  std::pair<std::map<Location::Point, size_t>, size_t> IndexAxis(const std::vector<StopPosition> &stop_positions) const;
  bool CheckAdjacency(const std::string &current_stop, const std::set<std::string> &other_stops) const;
};


}