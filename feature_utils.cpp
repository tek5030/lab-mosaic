#include "feature_utils.h"

std::vector<cv::DMatch> extractGoodRatioMatches(const std::vector<std::vector<cv::DMatch>>& matches, float max_ratio)
{
  // Todo 2: Implement the ratio test.
  std::vector<cv::DMatch> good_ratio_matches;

  return good_ratio_matches;
}

void extractMatchingPoints(const std::vector<cv::KeyPoint>& keypts1, const std::vector<cv::KeyPoint>& keypts2,
                           const std::vector<cv::DMatch>& matches, Eigen::Matrix2Xf& matched_pts1,
                           Eigen::Matrix2Xf& matched_pts2)
{
  matched_pts1.resize(Eigen::NoChange, matches.size());
  matched_pts2.resize(Eigen::NoChange, matches.size());
  for (size_t i = 0; i < matches.size(); ++i)
  {
    matched_pts1.col(i) = Eigen::Vector2f{keypts1[matches[i].queryIdx].pt.x, keypts1[matches[i].queryIdx].pt.y};
    matched_pts2.col(i) = Eigen::Vector2f{keypts2[matches[i].trainIdx].pt.x, keypts2[matches[i].trainIdx].pt.y};
  }
}

std::vector<cv::KeyPoint> detectInGrid(const cv::Mat& image, cv::Ptr<cv::Feature2D> detector, cv::Size grid_size, int max_in_cell, int patch_width)
{
  std::vector<cv::KeyPoint> all_keypoints;

  const int height = image.rows / grid_size.height;
  const int width = image.cols / grid_size.width;

  const int patch_rad = patch_width / 2;

  for (int x=0; x<grid_size.width; ++x)
  {
    for (int y=0; y<grid_size.height; ++y)
    {
      cv::Range row_range(std::max(y*height - patch_rad, 0), std::min((y+1)*height + patch_rad, image.rows));
      cv::Range col_range(std::max(x*width - patch_rad, 0), std::min((x+1)*width + patch_rad, image.cols));

      cv::Mat sub_img = image(row_range, col_range);
      
      std::vector<cv::KeyPoint> grid_keypoints;
      detector->detect(sub_img, grid_keypoints);

      cv::KeyPointsFilter::retainBest(grid_keypoints, max_in_cell);

      for (auto& keypoint : grid_keypoints)
      {
        keypoint.pt.x += col_range.start;
        keypoint.pt.y += row_range.start;
      }

      all_keypoints.insert(all_keypoints.end(), grid_keypoints.begin(), grid_keypoints.end());
    }
  }

  return all_keypoints;
}
