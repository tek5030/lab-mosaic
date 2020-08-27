#pragma once

#include "Eigen/Dense"
#include <random>

using PointSelection = std::vector<Eigen::Index>;

struct HomographyEstimate
{
  Eigen::Matrix3f homography;
  size_t num_inliers;
  PointSelection inliers;
};

/// \brief Estimates a homography from point correspondences using RANSAC.
class HomographyEstimator
{
public:
  /// \brief Constructs the estimator.
  /// \param p The desired probability of getting a good sample.
  /// \param distance_threshold The maximum error a good sample can have as defined by the two-sided reprojection error.
  /// \param max_iterations The absolute maximum iterations allowed, ignoring p if necessary.
  explicit HomographyEstimator(float p = 0.99f, float distance_threshold = 3.f, int max_iterations = 10000);

  /// \brief Estimate a homography from point correspondences.
  /// \param pts1 Set of corresponding points from image 1.
  /// \param pts2 Set of corresponding points from image 2.
  /// \return The estimated homography.
  HomographyEstimate estimate(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2);

private:
  /// \brief Finds a set of inliers for estimating a homography.
  PointSelection ransacEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2);

  /// \brief Estimates a homography from point correspondences using DLT.
  Eigen::Matrix3f dltEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2) const;

  /// \brief Estimates a homography from point correspondences using the normalized DLT.
  Eigen::Matrix3f normalizedDltEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2) const;

  /// \brief Finds a normalizing similarity transform for a set of points.
  Eigen::Matrix3f findNormalizingSimilarity(const Eigen::Matrix2Xf& pts) const;

  /// \brief Computes the two-sided reprojection error for a given homography.
  float computeReprojectionError(const Eigen::Vector2f& pt1, const Eigen::Vector2f& pt2,
                                 const Eigen::Matrix3f& H, const Eigen::Matrix3f& H_inv) const;

  /// \brief Exracts points from a point set.
  Eigen::Matrix2Xf extractPoints(const Eigen::Matrix2Xf& pts, const PointSelection& selection) const;

  /// \brief Computes a random point selection.
  PointSelection randomlySelectPoints(Eigen::Index total_size, int sample_size);

  float p_;
  float distance_threshold_;
  int max_iterations_;
  std::random_device rd_;
  std::mt19937 generator_;
};
