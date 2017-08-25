/*
 * imageset.h
 *
 *  Copyright (C) 2017 Diamond Light Source
 *
 *  Author: James Parkhurst
 *
 *  This code is distributed under the BSD license, a copy of which is
 *  included in the root directory of this package.
 */

#ifndef DXTBX_IMAGESET_H
#define DXTBX_IMAGESET_H

#include <map>
#include <scitbx/array_family/shared.h>
#include <scitbx/array_family/versa.h>
#include <scitbx/array_family/accessors/c_grid.h>
#include <scitbx/array_family/tiny_types.h>
#include <dxtbx/model/beam.h>
#include <dxtbx/model/detector.h>
#include <dxtbx/model/goniometer.h>
#include <dxtbx/model/scan.h>
#include <dxtbx/format/image.h>
#include <dxtbx/error.h>

namespace dxtbx {

  using scitbx::af::int2;
  using model::Beam;
  using model::Detector;
  using model::Panel;
  using model::Goniometer;
  using model::Scan;
  using format::ImageTile;
  using format::Image;
  using format::ImageBuffer;

  namespace detail {
    
    template <typename T>
    T safe_dereference(boost::shared_ptr<T> ptr) {
      T * item = ptr.get();
      DXTBX_ASSERT(item != NULL);
      return *item;
    }

  }


  /**
   * Hold an external lookup item
   */
  template <typename T>
  class ExternalLookupItem {
  public:

    /** Construct the exteranal lookup item */
    ExternalLookupItem() {}

    /**
     * Get the filename
     */
    std::string get_filename() const {
      return filename_;
    }

    /**
     * Set the filename
     * @param filename The filename
     */
    void set_filename(const std::string &filename) {
      filename_ = filename;
    }

    /**
     * Get the data
     */
    Image<T> get_data() const {
      return data_;
    }

    /**
     * Set the data
     * @param data - The data
     */
    void set_data(const Image<T> &data) {
      data_ = data;
    }

  protected:

    std::string filename_;
    Image<T> data_;
  };


  /**
   * The external lookup group
   */
  class ExternalLookup {
  public:

    /**
     * @returns an internal reference to the mask
     */
    ExternalLookupItem<bool>& mask() {
      return mask_;
    }

    /**
     * @returns an internal reference to the gain
     */
    ExternalLookupItem<double>& gain() {
      return gain_;
    }

    /**
     * @returns an internal reference to the pedestal
     */
    ExternalLookupItem<double>& pedestal() {
      return pedestal_;
    }

  protected:

    ExternalLookupItem<bool> mask_;
    ExternalLookupItem<double> gain_;
    ExternalLookupItem<double> pedestal_;
  };


  /**
   * A class to hold data about the imageset
   */
  class ImageSetData {
  public:

    typedef boost::shared_ptr<Beam> beam_ptr;
    typedef boost::shared_ptr<Detector> detector_ptr;
    typedef boost::shared_ptr<Goniometer> goniometer_ptr;
    typedef boost::shared_ptr<Scan> scan_ptr;

    ImageSetData() {}

    ImageSetData(boost::python::object reader,
                 boost::python::object masker)
      : reader_(reader),
        masker_(masker),
        beams_(boost::python::len(reader)),
        detectors_(boost::python::len(reader)),
        goniometers_(boost::python::len(reader)),
        scans_(boost::python::len(reader)) {
      DXTBX_ASSERT(boost::python::len(reader) == boost::python::len(masker));
    }
    

    ImageBuffer get_data(std::size_t index) {
      boost::python::tuple d = reader_.attr("read")(index);
        
      ImageBuffer buffer(image);

      return buffer;
    }

    Image<bool> get_mask(std::size_t index) {
      return boost::python::extract< Image<bool> >(
        data_.attr("mask")(index))();
    }

    bool has_single_file_reader() const {
      return boost::python::extract< bool >(
        data_.attr("reader").attr("is_single_file_reader")())();
    }

    std::string get_path(std::size_t index) const {
      return boost::python::extract< std::string >(
        data_.attr("path")(index))();
    }

    std::string get_master_path() const {
      return boost::python::extract< std::string >(
        data_.attr("path")(0))();
    }

    std::string get_image_identifier(std::size_t index) const {
      return boost::python::extract< std::string >(
        data_.attr("identifier")(index))();
    }

    std::string get_property(std::string name) const {
      return properties_.at(name);
    }

    void set_property(std::string name, std::string value) {
      properties_[name] = value;
    }

    beam_ptr get_beam(std::size_t index) const {
      DXTBX_ASSERT(index < beams_.size());
      return beams_[index];
    }

    void set_beam(std::size_t index, const beam_ptr &beam) {
      DXTBX_ASSERT(index < beams_.size());
      beams_[index] = beam;
    }

    detector_ptr get_detector(std::size_t index) const {
      DXTBX_ASSERT(index < detectors_.size());
      return detectors_[index];
    }

    void set_detector(std::size_t index, const detector_ptr &detector) {
      DXTBX_ASSERT(index < detectors_.size());
      detectors_[index] = detector;
    }

    goniometer_ptr get_goniometer(std::size_t index) const {
      DXTBX_ASSERT(index < goniometers_.size());
      return goniometers_[index];
    }

    void set_goniometer(std::size_t index, const goniometer_ptr &goniometer) {
      DXTBX_ASSERT(index < goniometers_.size());
      goniometers_[index] = goniometer;
    }

    scan_ptr get_scan(std::size_t index) const {
      DXTBX_ASSERT(index < scans_.size());
      return scans_[index];
    }

    void set_scan(std::size_t index, const scan_ptr &scan) {
      DXTBX_ASSERT(index < scans_.size());
      scans_[index] = scan;
    }

    std::size_t size() const {
      return boost::python::len(data_);
    }

    /**
     * @returns The external lookup
     */
    ExternalLookup& external_lookup() {
      return external_lookup_;
    }

  protected:

    boost::python::object data_;
    scitbx::af::shared<beam_ptr> beams_;
    scitbx::af::shared<detector_ptr> detectors_;
    scitbx::af::shared<goniometer_ptr> goniometers_;
    scitbx::af::shared<scan_ptr> scans_;
    std::map<std::string,std::string> properties_;
    ExternalLookup external_lookup_;
  };



  /**
   * A class to represent an imageset
   */
  class ImageSet {
  public:

    typedef ImageSetData::beam_ptr beam_ptr;
    typedef ImageSetData::detector_ptr detector_ptr;
    typedef ImageSetData::goniometer_ptr goniometer_ptr;
    typedef ImageSetData::scan_ptr scan_ptr;

    /**
     * Cache an image
     */
    class DataCache {
    public:

      ImageBuffer image;
      int index;

      DataCache()
        : index(-1) {}
    };


    /**
     * Default constructor throws an exception.
     * This only here so overloaded functions that
     * never return can be compiled without warnings.
     */
    ImageSet() {
      DXTBX_ERROR("ImageSet needs imageset data");
    }

    /**
     * Construct the imageset
     * @param data The imageset data
     */
    ImageSet(const ImageSetData &data)
      : data_(data),
        indices_(data.size()) {
      for (std::size_t i = 0; i < indices_.size(); ++i) {
        indices_[i] = i;
      }
    }

    /**
     * Construct the imageset
     * @param data The imageset data
     * @param indices The image indices
     */
    ImageSet(
          const ImageSetData &data,
          const scitbx::af::const_ref<std::size_t> &indices)
      : data_(data),
        indices_(indices.begin(), indices.end()) {
      DXTBX_ASSERT(scitbx::af::max(indices) < data.size());
    }

    /**
     * @returns the imageset data
     */
    ImageSetData data() const {
      return data_;
    }

    /**
     * @returns The image indices
     */
    scitbx::af::shared<std::size_t> indices() const {
      return indices_;
    }

    /**
     * @returns The number of images
     */
    std::size_t size() const {
      return indices_.size();
    }

    /**
     * @returns The external lookup
     */
    ExternalLookup& external_lookup() {
      return data_.external_lookup();
    }

    /**
     * Get the raw image data
     * @param index The image index
     * @returns The raw image data
     */
    ImageBuffer get_raw_data(std::size_t index) {
      if (data_cache_.index == index) {
        return data_cache_.image;
      }
      ImageBuffer image = data_.get_data(indices_[index]);
      data_cache_.index = index;
      data_cache_.image = image;
      return image;
    }

    /**
     * Get the corrected data array (raw - pedestal) / gain
     * @param index The image index
     * @returns The corrected data array
     */
    Image<double> get_corrected_data(std::size_t index) {

      typedef scitbx::af::versa< double, scitbx::af::c_grid<2> > array_type;
      typedef scitbx::af::const_ref< double, scitbx::af::c_grid<2> > const_ref_type;

      // Get the multi-tile data, gain and pedestal
      Image<double> data = get_raw_data(index).as_double();
      Image<double> gain = get_gain(index);
      Image<double> pedestal = get_pedestal(index);
      DXTBX_ASSERT(data.n_tiles() == gain.n_tiles());
      DXTBX_ASSERT(data.n_tiles() == pedestal.n_tiles());

      // Loop through tiles
      Image<double> result;
      for (std::size_t i = 0; i < data.n_tiles(); ++i) {

        // Get the data, gain and pedestal for each tile
        const_ref_type r = data.tile(i).data().const_ref();
        const_ref_type g = gain.tile(i).data().const_ref();
        const_ref_type p = pedestal.tile(i).data().const_ref();
        DXTBX_ASSERT(g.size() == 0 || r.accessor().all_eq(g.accessor()));
        DXTBX_ASSERT(p.size() == 0 || r.accessor().all_eq(p.accessor()));

        // Create the result array
        array_type c(r.accessor());
        std::copy(r.begin(), r.end(), c.begin());

        // Apply pedestal
        if (p.size() > 0) {
          for (std::size_t j = 0; j < r.size(); ++j) {
            c[i] = c[i] - p[i];
          }
        }

        // Apply gain
        if (g.size() > 0) {
          for (std::size_t j = 0; j < r.size(); ++j) {
            DXTBX_ASSERT(g[i] > 0);
            c[i] = c[i] / g[i];
          }
        }
        result.push_back(ImageTile<double>(c));
      }

      // Return the result
      return result;
    }

    /**
     * Get the detector gain map. Either take this from the external gain map or
     * try to construct from the detector gain.
     * @param index The image index
     * @returns The gain
     */
    Image<double> get_gain(std::size_t index) {
      if (external_lookup().gain().get_data().empty()) {

        Detector detector = *get_detector_for_image(index);
        bool use_detector_gain = true;

        // Compute the gain for each panel
        std::vector<double> gain(detector.size(), 0);
        for (std::size_t i = 0; i < detector.size(); ++i) {
          gain[i] = detector[i].get_gain();
          if (gain[i] <= 0) {
            use_detector_gain = false;
            break;
          }
        }

        // If using the gain from the panel, construct a gain map
        if (use_detector_gain) {
          Image<double> result;
          for (std::size_t i = 0; i < detector.size(); ++i) {
            std::size_t xsize = detector[i].get_image_size()[0];
            std::size_t ysize = detector[i].get_image_size()[1];
            scitbx::af::c_grid<2> grid(ysize, xsize);
            scitbx::af::versa<double, scitbx::af::c_grid<2> > data(grid, gain[i]);
            result.push_back(ImageTile<double>(data));
          }
          external_lookup().gain().set_filename("");
          external_lookup().gain().set_data(result);
        }

      }
      return external_lookup().gain().get_data();
    }

    /**
     * Get the pedestal
     * @param index The image index
     * @returns The pedestal image
     */
    Image<double> get_pedestal(std::size_t index) {
      return external_lookup().pedestal().get_data();
    }

    /**
     * Compute the mask
     * @param index The image index
     * @returns The image mask
     */
    Image<bool> get_mask(std::size_t index) {

      typedef scitbx::af::versa< double, scitbx::af::c_grid<2> > array_type;

      // Compute the trusted range mask
      Image<double> data = get_raw_data(index).as_double();
      Detector detector = *get_detector_for_image(index);
      DXTBX_ASSERT(data.n_tiles() == detector.size());
      Image<bool> mask;
      for (std::size_t i = 0; i < detector.size(); ++i) {
        Panel panel = detector[i];
        mask.push_back(
          ImageTile<bool>(
            panel.get_trusted_range_mask(
              data.tile(i).data().const_ref())));
      }

      // Combine with the dynamic mask
      Image<bool> dyn_mask = data_.get_mask(indices_[index]);
      if (!dyn_mask.empty()) {
        DXTBX_ASSERT(dyn_mask.n_tiles() == detector.size());
        for (std::size_t i = 0; i < detector.size(); ++i) {
          scitbx::af::ref< bool, scitbx::af::c_grid<2> > m1 = mask.tile(i).data().ref();
          scitbx::af::const_ref< bool, scitbx::af::c_grid<2> > m2 = dyn_mask.tile(i).data().const_ref();
          DXTBX_ASSERT(m1.size() == m2.size());
          for (std::size_t j = 0; j < m1.size(); ++j) {
            m1[j] = m1[j] && m2[j];
          }
        }
      }

      // Combine with the external lookup mask
      Image<bool> ext_mask = external_lookup().mask().get_data();
      if (!ext_mask.empty()) {
        DXTBX_ASSERT(ext_mask.n_tiles() == detector.size());
        for (std::size_t i = 0; i < detector.size(); ++i) {
          scitbx::af::ref< bool, scitbx::af::c_grid<2> > m1 = mask.tile(i).data().ref();
          scitbx::af::const_ref< bool, scitbx::af::c_grid<2> > m2 = ext_mask.tile(i).data().const_ref();
          DXTBX_ASSERT(m1.size() == m2.size());
          for (std::size_t j = 0; j < m1.size(); ++j) {
            m1[j] = m1[j] && m2[j];
          }
        }
      }

      // Return the mask
      return mask;
    }

    /**
     * Get the property
     * @param name The property name
     * @returns The property string
     */
    std::string get_property(const std::string &name) const {
      return data_.get_property(name);
    }

    /**
     * Set the property
     * @param name The property name
     * @param data The property string
     */
    void set_property(const std::string &name, const std::string &data) {
      data_.set_property(name, data);
    }

    /**
     * @param index The image index
     * @returns the beam at index
     */
    virtual
    beam_ptr get_beam_for_image(std::size_t index) const {
      DXTBX_ASSERT(index < indices_.size());
      return data_.get_beam(indices_[index]);
    }

    /**
     * @param index The image index
     * @returns the detector at index
     */
    virtual
    detector_ptr get_detector_for_image(std::size_t index) const {
      DXTBX_ASSERT(index < indices_.size());
      return data_.get_detector(indices_[index]);
    }

    /**
     * @param index The image index
     * @returns the goniometer at index
     */
    virtual
    goniometer_ptr get_goniometer_for_image(std::size_t index) const {
      DXTBX_ASSERT(index < indices_.size());
      return data_.get_goniometer(indices_[index]);
    }

    /**
     * @param index The image index
     * @returns the scan at index
     */
    virtual
    scan_ptr get_scan_for_image(std::size_t index) const {
      DXTBX_ASSERT(index < indices_.size());
      return data_.get_scan(indices_[index]);
    }

    /**
     * Set the beam model
     * @param index The image index
     * @param beam The beam model
     */
    virtual
    void set_beam_for_image(std::size_t index, const beam_ptr &beam) {
      DXTBX_ASSERT(index < indices_.size());
      data_.set_beam(indices_[index], beam);
    }

    /**
     * Set the detector model
     * @param index The image index
     * @param detector The detector model
     */
    virtual
    void set_detector_for_image(std::size_t index, const detector_ptr &detector) {
      DXTBX_ASSERT(index < indices_.size());
      data_.set_detector(indices_[index], detector);
    }

    /**
     * Set the goniometer model
     * @param index The image index
     * @param goniometer The goniometer model
     */
    virtual
    void set_goniometer_for_image(std::size_t index, const goniometer_ptr &goniometer) {
      DXTBX_ASSERT(index < indices_.size());
      data_.set_goniometer(indices_[index], goniometer);
    }

    /**
     * Set the scan model
     * @param index The image index
     * @param scan The scan model
     */
    virtual
    void set_scan_for_image(std::size_t index, const scan_ptr &scan) {
      DXTBX_ASSERT(scan == NULL || scan->get_num_images() == 1);
      DXTBX_ASSERT(index < indices_.size());
      data_.set_scan(indices_[index], scan);
    }

    /**
     * Get the image path
     * @param index The image index
     * @returns The image path
     */
    std::string get_path(std::size_t index) const {
      DXTBX_ASSERT(index < indices_.size());
      if (data_.has_single_file_reader()) {
        return data_.get_master_path();
      }
      return data_.get_path(indices_[index]);
    }

    /**
     * Get the image identifier
     * @param index The image index
     * @returns The image identifier
     */
    std::string get_image_identifier(std::size_t index) {
      DXTBX_ASSERT(index < indices_.size());
      return data_.get_image_identifier(indices_[index]);
    }

    /**
     * @returns The imageset itself
     */
    virtual
    ImageSet as_imageset() const {
      return *this;
    }

    /**
     * @returns The complete set
     */
    virtual
    ImageSet complete_set() const {
      return ImageSet(data_);
    }

    /**
     * @param first The first slice index
     * @param last The last slice index
     * @returns The partial set
     */
    virtual
    ImageSet partial_set(std::size_t first, std::size_t last) const {
      DXTBX_ASSERT(last > first);
      return ImageSet(
          data_,
          scitbx::af::const_ref<std::size_t>(
            &indices_[first], last - first));
    }

    /**
     * Compare the imageset with another
     * @param other The other imageset
     * @returns Are the imagesets the same
     */
    bool operator==(const ImageSet &other) const {
      if (size() == other.size()) {
        for (std::size_t i = 0; i < size(); ++i) {
          if (get_path(i) != other.get_path(i)) {
            return false;
          }
        }
        return true;
      }
      return false;
    }

    /**
     * Compare the imageset with another
     */
    bool operator!=(const ImageSet &other) const {
      return !(*this == other);
    }

  protected:

    ImageSetData data_;
    scitbx::af::shared<std::size_t> indices_;
    DataCache data_cache_;
  };


  /**
   * Class to represent a grid of images
   */
  class ImageGrid : public ImageSet {
  public:

    /**
     * Construct the grid
     * @param data The imageset data
     * @param grid_size The size of the grid
     */
    ImageGrid(const ImageSetData &data,
              int2 grid_size)
      : ImageSet(data),
        grid_size_(grid_size) {
      DXTBX_ASSERT(grid_size.all_gt(0));
      DXTBX_ASSERT(grid_size[0] * grid_size[1] == size());
    }

    /**
     * Construct the grid
     * @param data The imageset data
     * @param indices The imageset indices
     * @param grid_size The size of the grid
     */
    ImageGrid(
          const ImageSetData &data,
          const scitbx::af::const_ref<std::size_t> &indices,
          int2 grid_size)
      : ImageSet(data, indices),
        grid_size_(grid_size) {
      DXTBX_ASSERT(grid_size.all_gt(0));
      DXTBX_ASSERT(grid_size[0] * grid_size[1] == size());
    }

    /**
     * @returns The grid size
     */
    int2 get_grid_size() const {
      return grid_size_;
    }

    /**
     * Construct the image grid from the imageset
     * @param imageset The imageset
     * @param grid_size The grid_size
     * @returns the image grid
     */
    ImageGrid from_imageset(const ImageSet &imageset, int2 grid_size) const {
      ImageGrid result(imageset.data(), imageset.indices().const_ref(), grid_size);
      return result;
    }

    /**
     * Convert the grid to an imageset
     * @returns An imageset
     */
    ImageSet as_imageset() const {
      ImageSet result(data_, indices_.const_ref());
      return result;
    }

    /**
     * Get the complete seta
     * @returns The complete sweep
     */
    ImageSet complete_set() const {
      DXTBX_ERROR("Cannot get complete set from image grid");
      return ImageSet();
    }

    /**
     * Get a partial set
     * @param first The first index
     * @param last The last index
     * @returns The partial sweep
     */
    ImageSet partial_set(std::size_t first, std::size_t last) const {
      DXTBX_ERROR("Cannot get partial set from image grid");
      return ImageSet();
    }

  protected:

    int2 grid_size_;
  };


  /**
   * A class to represent a sweep of data
   */
  class ImageSweep : public ImageSet {
  public:

    /**
     * Construct the sweep
     * @param data The imageset data
     * @param beam The beam model
     * @param detector The detector model
     * @param goniometer The gonioeter model
     * @param scan The scan model
     */
    ImageSweep(const ImageSetData &data,
               const Beam &beam,
               const Detector &detector,
               const Goniometer &goniometer,
               const Scan &scan)
      : ImageSet(data),
        beam_(new Beam(beam)),
        detector_(new Detector(detector)),
        goniometer_(new Goniometer(goniometer)),
        scan_(new Scan(scan)) {

      // Check the scan is the same length and number of images
      DXTBX_ASSERT(data.size() == scan.get_num_images());

      // Set the models for each image
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_beam_for_image(i, beam_);
        ImageSet::set_detector_for_image(i, detector_);
        ImageSet::set_goniometer_for_image(i, goniometer_);
        ImageSet::set_scan_for_image(i, scan_ptr(new Scan(scan[i])));
      }
    }

    /**
     * Construct the sweep
     * @param data The imageset data
     * @param indices The image indices
     * @param beam The beam model
     * @param detector The detector model
     * @param goniometer The gonioeter model
     * @param scan The scan model
     */
    ImageSweep(const ImageSetData &data,
               const scitbx::af::const_ref<std::size_t> &indices,
               const Beam &beam,
               const Detector &detector,
               const Goniometer &goniometer,
               const Scan &scan)
      : ImageSet(data, indices),
        beam_(new Beam(beam)),
        detector_(new Detector(detector)),
        goniometer_(new Goniometer(goniometer)),
        scan_(new Scan(scan)) {

      // Check the scan is the same length as number of indices
      DXTBX_ASSERT(indices.size() == scan.get_num_images());

      // Check indices are sequential
      for (std::size_t i = 1; i < indices.size(); ++i) {
        DXTBX_ASSERT(indices[i] == indices[i-1]+1);
      }

      // Set the models for each image
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_beam_for_image(i, beam_);
        ImageSet::set_detector_for_image(i, detector_);
        ImageSet::set_goniometer_for_image(i, goniometer_);
        ImageSet::set_scan_for_image(i, scan_ptr(new Scan(scan[i])));
      }
    }

    /**
     * @returns the array range
     */
    int2 get_array_range() const {
      DXTBX_ASSERT(scan_ != NULL);
      return scan_->get_array_range();
    }

    /**
     * @returns the beam model
     */
    beam_ptr get_beam() const {
      return beam_;
    }

    /**
     * @returns the detector model
     */
    detector_ptr get_detector() const {
      return detector_;
    }

    /**
     * @returns the goniometer model
     */
    goniometer_ptr get_goniometer() const {
      return goniometer_;
    }

    /**
     * @returns the scan model
     */
    scan_ptr get_scan() const {
      return scan_;
    }

    /**
     * Set the beam model
     * @param beam The beam model
     */
    void set_beam(const Beam &beam) {
      beam_ = beam_ptr(new Beam(beam));
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_beam_for_image(i, beam_);
      }
    }

    /**
     * Set the detector model
     * @param detector The detector model
     */
    void set_detector(const Detector &detector) {
      detector_ = detector_ptr(new Detector(detector));
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_detector_for_image(i, detector_);
      }
    }

    /**
     * Set the goniometer model
     * @param goniometer The goniometer model
     */
    void set_goniometer(const Goniometer &goniometer) {
      goniometer_ = goniometer_ptr(new Goniometer(goniometer));
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_goniometer_for_image(i, goniometer_);
      }
    }

    /**
     * Set the scan model
     * @param scan The scan model
     */
    void set_scan(const Scan &scan) {
      DXTBX_ASSERT(scan.get_num_images() == size());
      scan_ = scan_ptr(new Scan(scan));
      for (std::size_t i = 0; i < size(); ++i) {
        ImageSet::set_scan_for_image(i, scan_ptr(new Scan(scan[i])));
      }
    }

    /**
     * Override per-image model
     */
    void set_beam_for_image(std::size_t index, const beam_ptr &beam) {
      DXTBX_ERROR("Cannot set per-image model in sweep");
    }

    /**
     * Override per-image model
     */
    void set_detector_for_image(std::size_t index, const detector_ptr &detector) {
      DXTBX_ERROR("Cannot set per-image model in sweep");
    }

    /**
     * Override per-image model
     */
    void set_goniometer_for_image(std::size_t index, const goniometer_ptr &goniometer) {
      DXTBX_ERROR("Cannot set per-image model in sweep");
    }

    /**
     * Override per-image model
     */
    void set_scan_for_image(std::size_t index, const scan_ptr &scan) {
      DXTBX_ERROR("Cannot set per-image model in sweep");
    }


    /**
     * Convert the sweep to an imageset
     * @returns An imageset
     */
    ImageSet as_imageset() const {
      ImageSet result(data_, indices_.const_ref());
      return result;
    }

    /**
     * Get the complete seta
     * @returns The complete sweep
     */
    ImageSet complete_set() const {
      DXTBX_ERROR("Cannot get complete set from image sweep");
      return ImageSet();
    }

    /**
     * Get a partial set
     * @param first The first index
     * @param last The last index
     * @returns The partial sweep
     */
    ImageSet partial_set(std::size_t first, std::size_t last) const {
      DXTBX_ERROR("Cannot get partial set from image sweep");
      return ImageSet();
    }


    /**
     * Get the complete seta
     * @returns The complete sweep
     */
    ImageSweep complete_sweep() const {

      // Compute scan
      Scan scan = detail::safe_dereference(data_.get_scan(0));
      for (std::size_t i = 1; i < data_.size(); ++i) {
        scan += detail::safe_dereference(data_.get_scan(i));
      }

      // Construct a sweep
      ImageSweep result(
          data_,
          detail::safe_dereference(get_beam()),
          detail::safe_dereference(get_detector()),
          detail::safe_dereference(get_goniometer()),
          scan);

      // Return the sweep
      return result;
    }

    /**
     * Get a partial set
     * @param first The first index
     * @param last The last index
     * @returns The partial sweep
     */
    ImageSweep partial_sweep(std::size_t first, std::size_t last) const {

      // Check slice indices
      DXTBX_ASSERT(last > first);

      // Construct a partial scan
      Scan scan = detail::safe_dereference(ImageSet::get_scan_for_image(first));
      for (std::size_t i = first+1; i < last; ++i) {
        scan += detail::safe_dereference(ImageSet::get_scan_for_image(i));
      }

      // Construct the partial indices
      scitbx::af::const_ref<std::size_t> indices(&indices_[first], last - first);

      // Construct the partial sweep
      ImageSweep result(
          data_,
          indices,
          detail::safe_dereference(get_beam()),
          detail::safe_dereference(get_detector()),
          detail::safe_dereference(get_goniometer()),
          scan);

      // Return the sweep
      return result;
    }

  protected:

    beam_ptr beam_;
    detector_ptr detector_;
    goniometer_ptr goniometer_;
    scan_ptr scan_;

  };


}

#endif // DXTBX_IMAGESET_H