/*
* multi_panel_detector.h
*
*  Copyright (C) 2013 Diamond Light Source
*
*  Author: James Parkhurst
*
*  This code is distributed under the BSD license, a copy of which is
*  included in the root directory of this package.
*/
#ifndef DXTBX_MODEL_MULTI_PANEL_DETECTOR_H
#define DXTBX_MODEL_MULTI_PANEL_DETECTOR_H

#include <string>
//#include <boost/geometry.hpp>
//#include <boost/geometry/geometries/point.hpp>
//#include <boost/geometry/geometries/polygon.hpp>
#include <scitbx/vec2.h>
#include <scitbx/vec3.h>
#include <scitbx/mat3.h>
#include <scitbx/array_family/flex_types.h>
#include <scitbx/array_family/tiny_types.h>
#include <scitbx/array_family/shared.h>
#include <dxtbx/error.h>
#include "detector.h"

namespace dxtbx { namespace model {

  using scitbx::vec2;
  using scitbx::vec3;
  using scitbx::mat3;
  using scitbx::af::double6;
  using scitbx::af::int4;

  // int4 array type
  typedef scitbx::af::flex<int4>::type flex_int4;
  typedef scitbx::af::shared<int4> shared_int4;

  /**
  * A class representing a detector made up of multiple flat panel detectors.
  * The detector elements can be accessed in the same way as an array:
  *  detector[0] -> 1st detector panel.
  */
  class MultiPanelDetector : public DetectorBase {
  public:

    typedef std::pair<int, vec2<double> > coordinate_type;

    // Panel list typedefs
    typedef Detector panel_type;
    typedef scitbx::af::shared <panel_type> panel_list_type;
    typedef panel_list_type::iterator iterator;

    /** Default constructor */
    MultiPanelDetector()
      : type_("Unknown") {}

    /** Initialise the detector */
    MultiPanelDetector(std::string type)
      :type_(type) {}

    /** Virtual destructor */
    virtual ~MultiPanelDetector() {}

    /** Get the begin iterator */
    iterator begin() {
      return panel_list_.begin();
    }

    /** Get the end iterator */
    iterator end() {
      return panel_list_.end();
    }

    /** Add a panel to the list of panels */
    void add_panel(const panel_type &panel) {
      panel_list_.push_back(panel);
    }

    /** Remove all the panels */
    void remove_panels() {
      panel_list_.erase(panel_list_.begin(), panel_list_.end());
    }

    /** Remove a single panel */
    void remove_panel(std::size_t i) {
      panel_list_.erase(panel_list_.begin() + i);
    }

    /** Get the number of panels */
    std::size_t num_panels() const {
      return panel_list_.size();
    }

    /** Return a reference to a panel */
    panel_type& operator[](std::size_t index) {
      return panel_list_[index];
    }

    /** Return a const reference to a panel */
    const panel_type& operator[](std::size_t index) const {
      return panel_list_[index];
    }

    /** Check the detector panels are the same */
    bool operator==(const MultiPanelDetector &detector) {
      bool same = panel_list_.size() == detector.panel_list_.size();
      if (same) {
        for (std::size_t i = 0; i < panel_list_.size(); ++i) {
          same = same && (panel_list_[i] == detector.panel_list_[i]);
        }
      }
      return same;
    }

    /** Check the detector panels are not the same */
    bool operator!=(const MultiPanelDetector &detector) {
      return !(*this == detector);
    }

    /** Check the value is valid */
    bool is_value_in_trusted_range(int panel, double value) const {
      return 0 <= panel && panel < panel_list_.size()
          && panel_list_[panel].is_value_in_trusted_range(value);
    }

    /** Check the coordinate is valid */
    bool is_coord_valid(coordinate_type pxy) const {
      return 0 <= pxy.first && pxy.first < panel_list_.size()
          && panel_list_[pxy.first].is_coord_valid(pxy.second);
    }

    /** Map coordinates in mm to pixels */
    vec2<double> millimeter_to_pixel(coordinate_type pxy) const {
      return panel_list_[pxy.first].millimeter_to_pixel(pxy.second);
    }

    /** Map the coordinates in pixels to millimeters */
    vec2<double> pixel_to_millimeter(coordinate_type pxy) const {
      return panel_list_[pxy.first].pixel_to_millimeter(pxy.second);
    }

    /** Check if any panels intersect */
//    bool do_panels_intersect() const {
//      for (std::size_t j = 0; j < panel_list_.size()-1; ++j) {
//        for (std::size_t i = j+1; i < panel_list_.size(); ++i) {
//          if (panels_intersect(panel_list_[j], panel_list_[i])) {
//            return true;
//          }
//        }
//      }
//      return false;
//    }

  protected:

    /**
     * Check if the detector planes intersect.
     * @param a The first detector
     * @param b The second detector
     * @returns True/False do the detector planes intersect?
     */
//    static bool
//    panels_intersect(const Detector &a, const Detector &b) {

//      using namespace boost::geometry;

//      typedef boost::geometry::model::point <double, 3, cs::cartesian> point;
//      typedef boost::geometry::model::polygon <point> polygon;

//      // Get the rectange of detector points
//      double6 rect_a = a.get_image_rectangle();
//      double6 rect_b = b.get_image_rectangle();

//      // Create a polygon for the panel a plane
//      polygon poly_a;
//      append(poly_a, point(rect_a[0], rect_a[1], rect_a[2]));
//      append(poly_a, point(rect_a[3], rect_a[1], rect_a[5]));
//      append(poly_a, point(rect_a[3], rect_a[4], rect_a[5]));
//      append(poly_a, point(rect_a[0], rect_a[4], rect_a[2]));
//      append(poly_a, point(rect_a[0], rect_a[1], rect_a[2]));

//      // Create a polygon for the panel b plane
//      polygon poly_b;
//      append(poly_b, point(rect_b[0], rect_b[1], rect_b[2]));
//      append(poly_b, point(rect_b[3], rect_b[1], rect_b[5]));
//      append(poly_b, point(rect_b[3], rect_b[4], rect_b[5]));
//      append(poly_b, point(rect_b[0], rect_b[4], rect_b[2]));
//      append(poly_b, point(rect_b[0], rect_b[1], rect_b[2]));

//      // Check if the polygons intersect
//      return intersects(poly_a, poly_b);
//    }

    std::string type_;
    panel_list_type panel_list_;
  };

}} // namespace dxtbx::model

#endif // DXTBX_MODEL_MULTI_PANEL_DETECTOR_H