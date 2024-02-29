# Step 2: Features in OpenCV
There are several feature methods available in the [features2d] and [xfeatures2d] modules in OpenCV. 
Some of these are detectors, some compute descriptors and some do both. 
They all extend the abstract [`cv::Feature2D`] base class.

First, please take a look at the documentation above to get an overview.
Then, lets try it out!


## How to construct
You construct detectors and descriptor extractors by calling their static `create()` factory method. 
This method will return an OpenCV smart pointer `cv::Ptr<cv::Feature2D>` to the detector/descriptor extractor object. 
You can also typically set different parameters using this method.

The following example will create a [FAST] feature detector and a [LATCH] descriptor extractor:

```c++
cv::Ptr<cv::Feature2D> detector = cv::FastFeatureDetector::create();
cv::Ptr<cv::Feature2D> desc_extractor = cv::xfeatures2d::LATCH::create();
```

In order to use the descriptors for matching, we will also need a *matcher*. 
The following example constructs a *brute force matcher* based on the default metric used by the descriptor:

```c++
cv::BFMatcher matcher{desc_extractor->defaultNorm()};
```

## How to use
We detect keypoints by calling the detector's `detect()` method. 
Since `detector` is a pointer, we need to use the `->` operator:

```c++
std::vector<cv::KeyPoint> frame_keypoints;
detector->detect(gray_frame, frame_keypoints);
cv::KeyPointsFilter::retainBest(frame_keypoints, 1000);
```

The detected keypoints are stored in a vector of `cv::KeyPoints`. 
Here, we have also used a `cv::KeyPointsFilter` to extract the strongest keypoints according to the detector's metric.

Take a look at [`cv::KeyPoints`]. 
What fields does it contain?

We compute descriptors for each keypoint by calling the descriptor extractor's `compute()` method:

```c++
cv::Mat frame_descriptors;
desc_extractor->compute(gray_frame, frame_keypoints, frame_descriptors);
```

The descriptors are stored in a `cv::Mat`.

We will use the matcher to match descriptors between the current frame and a reference frame. 
In order to apply the *ratio test* (from the lectures), we will need to extract the two best matches for each keypoint. 
We can do this with the matcher's `knnMatch()` method:

```c++
std::vector<std::vector<cv::DMatch>> matches;
matcher.knnMatch(frame_descriptors, ref_descriptors, matches, 2);
```

The result is stored in a vector of vectors, each with two `cv::DMatch` objects (the best and the second best matches).

Take a look at [`cv::DMatch`].
What fields does it contain?

## `runLabMosaic()`
Now, take look at `runLabMosaic()` in [lab_mosaic.cpp](../lab_mosaic.cpp), and find where each of the steps above are performed in the code.
This is a pretty advanced program, so ask the instructors if you have trouble understanding what is going on in this function.

Then, please continue to the [next step](3-experiment-with-feature-matching.md).


[features2d]:  https://docs.opencv.org/4.9.0/da/d9b/group__features2d.html
[xfeatures2d]: https://docs.opencv.org/4.9.0/d1/db4/group__xfeatures2d.html
[`cv::Feature2D`]: https://docs.opencv.org/4.9.0/d0/d13/classcv_1_1Feature2D.html
[`cv::KeyPoints`]: https://docs.opencv.org/4.9.0/d2/d29/classcv_1_1KeyPoint.html
[`cv::DMatch`]:    https://docs.opencv.org/4.9.0/d4/de0/classcv_1_1DMatch.html
[FAST]: https://www.edwardrosten.com/work/fast.html
[LATCH]: https://talhassner.github.io/home/publication/2016_WACV_2