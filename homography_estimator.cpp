#include "homography_estimator.h"

HomographyEstimator::HomographyEstimator(float p, float distance_threshold, int max_iterations)
    : p_{p}
    , distance_threshold_{distance_threshold}
    , max_iterations_{max_iterations}
    , rd_{}
    , generator_{rd_()}
{ }

HomographyEstimate HomographyEstimator::estimate(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2)
{
  if (pts1.cols() != pts2.cols())
  {
    throw std::runtime_error("Point correspondence matrices did not have same size");
  }

  // Find inliers.
  PointSelection is_inlier = ransacEstimator(pts1, pts2);

  if (is_inlier.size() < 4)
  {
    return HomographyEstimate{};
  }

  // Estimate homography from set of inliers.
  Eigen::Matrix2Xf inliers_1 = extractPoints(pts1, is_inlier);
  Eigen::Matrix2Xf inliers_2 = extractPoints(pts2, is_inlier);

  return {normalizedDltEstimator(inliers_1, inliers_2), is_inlier.size(), is_inlier};
}

PointSelection HomographyEstimator::ransacEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2)
{
  Eigen::Index best_num_inliers{0};
  PointSelection best_inliers;

  PointSelection test_inliers;
  Eigen::Matrix3f test_H;
  Eigen::Matrix3f test_H_inv;

  Eigen::Matrix2Xf samples_1;
  Eigen::Matrix2Xf samples_2;

  int num_iterations = max_iterations_;
  for (int curr_iteration = 0; curr_iteration < num_iterations; ++curr_iteration)
  {
    // Sample 4 random points.
    PointSelection rand_selection = randomlySelectPoints(pts1.cols(), 4);
    samples_1 = extractPoints(pts1, rand_selection);
    samples_2 = extractPoints(pts2, rand_selection);

    // Determine test homography.
    test_H = dltEstimator(samples_1, samples_2);
    test_H_inv = test_H.inverse();

    // Count number of inliers.
    Eigen::Index test_num_inliers{0};
    test_inliers.clear();
    for (Eigen::Index i=0; i < pts1.cols(); ++i)
    {
      if (computeReprojectionError(pts1.col(i), pts2.col(i), test_H, test_H_inv) < distance_threshold_)
      {
        test_inliers.push_back(i);
        test_num_inliers++;
      }
    }

    // Update homography if test homography has the most inliers so far.
    if (test_num_inliers > 4 && test_num_inliers > best_num_inliers)
    {
      // Update homography with largest inlier set.
      std::swap(best_inliers, test_inliers);
      best_num_inliers = test_num_inliers;

      // Update number of iterations.
      const float inlier_ratio = static_cast<float>(best_num_inliers) / static_cast<float>(pts1.cols());
      const float p_all_inliers = inlier_ratio*inlier_ratio*inlier_ratio*inlier_ratio;
      const float estimated_min_iterations = std::log(1.f - p_) / std::log(1.f - p_all_inliers);
      num_iterations = std::min(static_cast<int>(estimated_min_iterations), max_iterations_);
    }
  }

  return best_inliers;
}

Eigen::Matrix3f HomographyEstimator::dltEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2) const
{
  // Define these for convenience.
  using Vector9f = Eigen::Matrix<float, 9, 1>;
  using Matrix3fRowMajor = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;

  // Construct the equation matrix.
  Eigen::MatrixXf A(2 * pts1.cols(), 9);
  for (Eigen::Index i = 0; i < pts1.cols(); ++i)
  {
    const auto& pt1 = pts1.col(i);
    const auto& pt2 = pts2.col(i);

    A.block<2,9>(2*i, 0) <<
          0.f,     0.f, 0.f, -pt1.x(), -pt1.y(), -1.f,  pt2.y()*pt1.x(),  pt2.y()*pt1.y(),  pt2.y(),
      pt1.x(), pt1.y(), 1.f,      0.f,      0.f,  0.f, -pt2.x()*pt1.x(), -pt2.x()*pt1.y(), -pt2.x();
  }

  // Solve using SVD.
  Vector9f h = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeFullV).matrixV().rightCols<1>();

  // Map solution to a 3x3 homography matrix.
  Matrix3fRowMajor H(h.data());

  return H;
}

Eigen::Matrix3f HomographyEstimator::normalizedDltEstimator(const Eigen::Matrix2Xf& pts1, const Eigen::Matrix2Xf& pts2) const
{
  // Normalize points
  Eigen::Matrix3f S1 = findNormalizingSimilarity(pts1);
  Eigen::Matrix2Xf pts1_normalized = (S1*pts1.colwise().homogeneous()).colwise().hnormalized();

  Eigen::Matrix3f S2 = findNormalizingSimilarity(pts2);
  Eigen::Matrix2Xf pts2_normalized = (S2*pts2.colwise().homogeneous()).colwise().hnormalized();

  // Estimate the homography.
  Eigen::Matrix3f H = dltEstimator(pts1_normalized, pts2_normalized);

  // Transform back to the original frame.
  H = S2.inverse()*H*S1;

  // Standardize H
  if (H(2, 2) != 0)
  {
    H /= H(2, 2);
  }

  return H;
}

Eigen::Matrix3f HomographyEstimator::findNormalizingSimilarity(const Eigen::Matrix2Xf& pts) const
{
  // Centroid of points
  const Eigen::Vector2f center = pts.rowwise().mean();

  // Compute the mean distance from centroid for all pts
  const float r_mean = ((pts.colwise() - center).colwise().norm()).mean();

  // The normalizing similarity matrix S
  const float s = std::sqrt(2.0f) / r_mean;

  Eigen::Matrix3f S;
  S << s, 0.f, -s*center(0),
      0.f, s, -s*center(1),
      0.f, 0.f, 1.f;

  return S;
}

float HomographyEstimator::computeReprojectionError(const Eigen::Vector2f& pt1, const Eigen::Vector2f& pt2,
                                                    const Eigen::Matrix3f& H, const Eigen::Matrix3f& H_inv) const
{
  // Todo 5: Compute the two-sided reprojection error.
  // Map points onto each other using the homography.
  Eigen::Vector2f pt_1_in_2;
  Eigen::Vector2f pt_2_in_1;

  // Compute the two-sided reprojection error \sigma_i.
  return 2*distance_threshold_; // Remove this dummy result!
}

Eigen::Matrix2Xf HomographyEstimator::extractPoints(const Eigen::Matrix2Xf& pts, const PointSelection& selection) const
{
  Eigen::Matrix2Xf samples(2, selection.size());

  for (size_t i = 0; i < selection.size(); ++i)
  {
    samples.col(i) = pts.col(selection[i]);
  }

  return samples;
}

PointSelection HomographyEstimator::randomlySelectPoints(Eigen::Index total_size, int sample_size)
{
  PointSelection selection;
  std::uniform_int_distribution<Eigen::Index> distribution(0, total_size-1);

  for (int i=0; i < sample_size; ++i)
  {
    selection.push_back(distribution(generator_));
  }

  return selection;
}
