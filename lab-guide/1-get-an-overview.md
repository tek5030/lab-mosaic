# Step 1: Get an overview
We will as usual start by presenting an overview of the method and the contents of this project.

## Algorithm overview
The main steps in today's lab are:
- Apply feature detectors and descriptors available in OpenCV to detect and describe keypoints.
  - Detect keypoints in the current frame.
  - Press <kbd>space</kbd> to set the current frame as the reference frame.

- Match keypoints between the reference frame and new current frames using OpenCV, and extract good matches by applying the ratio test.

- Use the point correspondences to estimate a homography between two images, using RANSAC and the (normalized) DLT:
  - Find a large inlier set by applying a DLT-estimator repeatedly on a minimal set of correspondences using RANSAC.
  - Apply normalized DLT on the inlier set.
  
- Use the estimated homography to combine the two images in an image mosaic:
  - Warp a downscaled reference image into the mosaic image.
  - Warp the current frame into the mosaic image based on the estimated homography.
  
## Introduction to the project source files
We have chosen to distribute the code on the following files:
- *main.cpp*
  
  Launches the program, catches any exceptions and prints their error message on the console.

- *lab_mosaic.h, lab_mosaic.cpp*
  
  Implements the lab loop:
  - Get next frame.
  - Detect keypoints.
  - Compute descriptors and match descriptors with reference frame.
  - Extract good matches based on ratio test.
  - Extract pixel coordinates for the resulting correspondences.
  - Estimate a homography between the reference frame and the current frame.
  - Create an image mosaic by combining the reference frame with the current frame.
  
- *feature_utils.h, feature_utils.cpp*
  
  Implements utility functions for feature matching. 
  You will be responsible for writing the ratio test function `extractGoodRatioMatches()` in *feature_utils.cpp*.
  
- *homography_estimator.h, homography_estimator.cpp*
  
  Implements a class that estimates homographies from point correspondences between two images using RANSAC and normalized DLT. 
  You will be responsible for implementing the reprojection error computation.
  
  Please continue to [the next step](2-features-in-opencv.md) to get started with features!