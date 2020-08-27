#pragma once

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "Eigen/Dense"

/// \brief Extracts a set of good matches according to the ratio test.
/// \param matches Input set of matches, the best and the second best match for each putative correspondence.
/// \param max_ratio Maximum acceptable ratio between the best and the next best match.
/// \return The set of matches that pass the ratio test.
std::vector<cv::DMatch> extractGoodRatioMatches(const std::vector<std::vector<cv::DMatch>>& matches, float max_ratio);

/// \brief Extracts the point correspondences from matches as columns in Eigen matrices.
/// \param[in] keypts1 Keypoints from first (query) image.
/// \param[in] keypts2 Keypoints from second (train) image.
/// \param[in] matches Point correspondence matches between the two images.
/// \param[out] matched_pts1 Points from first image as columns in an Eigen matrix.
/// \param[out] matched_pts2 Points from second image as columns in an Eigen matrix.
void extractMatchingPoints(
    const std::vector<cv::KeyPoint>& keypts1,
    const std::vector<cv::KeyPoint>& keypts2,
    const std::vector<cv::DMatch>& matches,
    Eigen::Matrix2Xf& matched_pts1,
    Eigen::Matrix2Xf& matched_pts2);

/// \brief Detects keypoints in independent cells on a grid.
/// \param image The image to detect keypoints in.
/// \param detector The keypoint detector to use.
/// \param grid_size Size of the grid.
/// \param max_in_cell Maximum number of detections in each cell.
/// \param patch_width Width of patch used for detection or description.
/// \return The collection of detected keypoints in all cells.
std::vector<cv::KeyPoint> detectInGrid(const cv::Mat& image, cv::Ptr<cv::Feature2D> detector, cv::Size grid_size, int max_in_cell, int patch_width);
