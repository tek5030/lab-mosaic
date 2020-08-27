#include "lab_4.h"

#include "homography_estimator.h"
#include "feature_utils.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core/eigen.hpp"

#include <chrono>
#include <iomanip>

// Make shorthand aliases for timing tools.
using Clock = std::chrono::high_resolution_clock;
using DurationInMs = std::chrono::duration<double, std::milli>;

// Forward declarations of ugly drawing functions.
void drawKeypointDetections(cv::Mat& vis_img,
                            const cv::Mat& frame_img,
                            const std::vector<cv::KeyPoint>& keypoints,
                            DurationInMs duration);

void drawKeypointMatches(cv::Mat& vis_img,
                         const cv::Mat& frame_img,
                         const cv::Mat& ref_img,
                         const std::vector<cv::KeyPoint>& frame_keypoints,
                         const std::vector<cv::KeyPoint>& ref_keypoints,
                         const std::vector<cv::DMatch>& matches,
                         DurationInMs det_duration,
                         DurationInMs matching_duration);

void drawEstimationDetails(cv::Mat& vis_img, DurationInMs est_duration, size_t num_inliers);


void lab4()
{
  // Open video stream from camera.
  const int camera_id = 0; // Should be 0 or 1 on the lab PCs.
  cv::VideoCapture cap(camera_id);
  if (!cap.isOpened())
  {
    throw std::runtime_error("Could not open camera " + std::to_string(camera_id));
  }

  // Set up windows.
  const std::string match_win = "Lab 4: Feature detection and matching";
  cv::namedWindow(match_win);
  const std::string mosaic_win = "Lab 4: Mosaic";
  cv::namedWindow(mosaic_win);

  // Set up a similarity transform.
  // Question: What does this similarity transform do?
  int frame_cols = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int frame_rows = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  cv::Matx33f S_cv{
      0.5f, 0.0f, 0.25f * static_cast<float>(frame_cols),
      0.0f, 0.5f, 0.25f * static_cast<float>(frame_rows),
      0.0f, 0.0f, 1.0f};

  // Todo 1: Experiment with blob and corner feature detectors.
  // Todo 3: Experiment with feature matching
  // Set up objects for detection, description and matching.
  cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SURF::create();
  cv::Ptr<cv::Feature2D> desc_extractor = cv::xfeatures2d::SURF::create();
  cv::BFMatcher matcher{desc_extractor->defaultNorm()};

  // Create homography estimator.
  HomographyEstimator estimator;

  // Reference image for mosaic.
  cv::Mat ref_image;
  std::vector<cv::KeyPoint> ref_keypoints;
  cv::Mat ref_descriptors;

  while (true)
  {
    // Read a frame from the camera.
    cv::Mat frame;
    cap >> frame;

    if (frame.empty())
    { break; }

    // Convert frame to gray scale image.
    cv::Mat gray_frame;
    cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

    // Detect keypoints.
    // Measure how long the processing takes.
    auto start = Clock::now();
    std::vector<cv::KeyPoint> frame_keypoints;
    detector->detect(gray_frame, frame_keypoints);
    cv::KeyPointsFilter::retainBest(frame_keypoints, 500);
    auto end = Clock::now();
    DurationInMs feature_detection_duration = end - start;

    // Create image for visualization.
    cv::Mat vis_img;

    if (ref_descriptors.empty())
    {
      // No reference image, draw keypoints.
      // (Press space to create a reference image).
      drawKeypointDetections(vis_img, frame, frame_keypoints, feature_detection_duration);
    }
    else
    {
      // We have a reference, try to match features!
      cv::Mat frame_descriptors;
      std::vector<std::vector<cv::DMatch>> matches;

      // Match descriptors with ratio test.
      // Measure how long the processing takes.
      start = Clock::now();
      desc_extractor->compute(gray_frame, frame_keypoints, frame_descriptors);
      matcher.knnMatch(frame_descriptors, ref_descriptors, matches, 2);
      std::vector<cv::DMatch> good_matches = extractGoodRatioMatches(matches, 0.8f);
      end = Clock::now();
      DurationInMs feature_matching_duration = end - start;

      // Draw matching results.
      drawKeypointMatches(vis_img, frame, ref_image, frame_keypoints, ref_keypoints, good_matches, feature_detection_duration, feature_matching_duration);

      if (good_matches.size() >= 10)
      {
        // Extract pixel coordinates for corresponding points.
        Eigen::Matrix2Xf matching_pts1;
        Eigen::Matrix2Xf matching_pts2;
        extractMatchingPoints(frame_keypoints, ref_keypoints, good_matches, matching_pts1, matching_pts2);

        // Estimate homography.
        // Measure how long the processing takes.
        start = Clock::now();
        HomographyEstimate estimate = estimator.estimate(matching_pts1, matching_pts2);
        end = Clock::now();
        DurationInMs estimation_duration = end - start;

        // Convert the homography estimate to OpenCV matrix.
        cv::Matx33f H_cv;
        cv::eigen2cv(estimate.homography, H_cv);

        // Todo 7: Transform the reference image according to the similarity S, and insert into the mosaic.
        cv::Mat mosaic;

        // Todo 8: Transform the current frame according to S and the computed homography.
        cv::Mat frame_warp;

        // Todo 9: Compute a mask for the transformed current frame.
        cv::Mat mask = cv::Mat::ones(frame.size(), CV_8UC1);
        cv::Mat mask_warp;

        //Todo 10: Insert the current frame into the mosaic.

        // Draw estimation duration.
        drawEstimationDetails(vis_img, estimation_duration, estimate.num_inliers);

        // Show mosaicking result.
        if (!mosaic.empty())
        { cv::imshow(mosaic_win, mosaic); }
      }
    }

    // Show feature matching visualization.
    cv::imshow(match_win, vis_img);

    // Draw figures and receive key presses.
    int key = cv::waitKey(30);
    if (key == ' ')
    {
      // Set reference image for mosaic
      // and compute descriptors.
      ref_image = frame.clone();
      ref_keypoints = frame_keypoints;
      desc_extractor->compute(gray_frame, ref_keypoints, ref_descriptors);
    }
    else if (key == 'r')
    {
      // Reset.
      // Make all reference data empty.
      ref_image = cv::Mat{};
      ref_keypoints.clear();
      ref_descriptors = cv::Mat{};
    }
    else if (key > 0) break;
  }
}

// Define a few BGR-colors for convenience.
namespace color
{
  const cv::Scalar green(0, 255, 0);
  const cv::Scalar red(0, 0, 255);
}

// Define font parameters.
namespace font
{
  constexpr auto face = cv::FONT_HERSHEY_PLAIN;
  constexpr auto scale = 1.0;
}

void drawKeypointDetections(cv::Mat& vis_img,
                            const cv::Mat& frame_img,
                            const std::vector<cv::KeyPoint>& keypoints,
                            DurationInMs duration)
{

  cv::drawKeypoints(frame_img, keypoints, vis_img, color::green);
  std::stringstream duration_info;
  duration_info << std::fixed << std::setprecision(0);
  duration_info << "Detection : " << duration.count() << "ms";
  cv::putText(vis_img, duration_info.str(), {10, 20}, font::face, font::scale, color::red);
}

void drawKeypointMatches(cv::Mat& vis_img,
                         const cv::Mat& frame_img,
                         const cv::Mat& ref_img,
                         const std::vector<cv::KeyPoint>& frame_keypoints,
                         const std::vector<cv::KeyPoint>& ref_keypoints,
                         const std::vector<cv::DMatch>& matches,
                         DurationInMs det_duration,
                         DurationInMs matching_duration)
{
  cv::drawMatches(frame_img, frame_keypoints, ref_img, ref_keypoints, matches, vis_img);
  std::stringstream det_duration_info;
  det_duration_info << std::fixed << std::setprecision(0);
  det_duration_info << "Detection : " << det_duration.count() << "ms";
  cv::putText(vis_img, det_duration_info.str(), {10, 20}, font::face, font::scale, color::red);

  std::stringstream match_duration_info;
  match_duration_info << std::fixed << std::setprecision(0);
  match_duration_info << "Matching  : " << matching_duration.count() << "ms";
  cv::putText(vis_img, match_duration_info.str(), {10, 40}, font::face, font::scale, color::red);
}

void drawEstimationDetails(cv::Mat& vis_img, DurationInMs est_duration, size_t num_inliers)
{
  std::stringstream duration_info;
  duration_info << std::fixed << std::setprecision(0);
  duration_info << "Estimation: " << est_duration.count() << "ms";
  cv::putText(vis_img, duration_info.str(), {10, 60}, font::face, font::scale, color::red);

  std::stringstream inlier_info;
  inlier_info << "Inliers: " << num_inliers;
  cv::putText(vis_img, inlier_info.str(), {10, 80}, font::face, font::scale, color::red);

}
