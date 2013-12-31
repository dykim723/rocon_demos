/**
 * @file src/lib/histogram_analytics.cpp
 *
 * @brief Implementation for histogram analytics.
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

// Histograms
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Local
#include "../../include/colour_signal/histogram_analytics.hpp"


/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace colour_signal {
/**
 * @brief Initialise the container with the histograms and pre-compute analytics.
 *
 * Note, incoming histograms are typically generated by calcHist which produces
 * 1 column vectors of floats.
 *
 * Number of bins will typically correspond to pixel intensities (our use case)
 * and so will usually be 256 (one for each level of intensity from 0->255).
 *
 * @param histograms opencv matrix vectors of dimension [#bins, 1].
 */
HistogramAnalytics::HistogramAnalytics(const std::vector<cv::Mat> &histograms) throw(ecl::StandardException)
  : _histograms(histograms)
{
  ecl_assert_throw( histograms.size() == 3, ecl::StandardException(LOC,ecl::InvalidInputError,
                    std::string("Must receive bgr histograms for b, g and r [only ") + std::to_string(histograms.size()) + std::string(" given]")));
  for ( unsigned int i; i < _histograms.size(); ++i ) {
    ecl_assert_throw( histograms.at(i).cols == 1, ecl::StandardException(LOC,ecl::InvalidInputError,
                    std::string("Histograms must always have only one column of data [") + std::to_string(histograms.at(i).cols) + std::string("]")));
    // should also check each histogram #rows is the same.
  }
  _generateStatistics();

}

void HistogramAnalytics::_generateStatistics() {
  for(unsigned int i = 0; i < _histograms.size(); ++i ) {
    const cv::Mat& hist = _histograms.at(i);

    float pixel_count = 0;
    float sum_intensity = 0;
    float max = 0;
    float max_intensity = 0;

    for(int intensity = 0; intensity < hist.rows; intensity++)
    {
      const float value = static_cast<int>(hist.at<float>(intensity,0));
      //std::cout << value << " ";
      pixel_count += value;
      sum_intensity += value*intensity;
      if ( value > max ) {
        max = value;
        max_intensity = intensity;
      }
    }
    //std::cout << std::endl;

    _pixel_counts.push_back(pixel_count);
    _average_intensities.push_back(sum_intensity/pixel_count);
    _maximum_intensities.push_back(max_intensity);
    std::cout << "Statistics:" << std::endl;
    std::cout << "  No.: " << pixel_count << std::endl;
    std::cout << "  Avg: " << sum_intensity/pixel_count << std::endl;
    std::cout << "  Max: " << max << "px@" << max_intensity << std::endl;
  }
}

void HistogramAnalytics::showHistograms(const std::string& title) const {

  const cv::Mat& b_hist = _histograms.at(0);
  const cv::Mat& g_hist = _histograms.at(1);
  const cv::Mat& r_hist = _histograms.at(2);

  // use same parameters as bgrHistograms()
  int number_of_bins = b_hist.rows;
  float range[] = { 0, 256 };
  const float* histRange = { range };

  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/number_of_bins );

  cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
  cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
  cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

  /// Draw for each channel
  for( int i = 1; i < number_of_bins; i++ )
  {
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
            cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
            cv::Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
            cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
            cv::Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
            cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
            cv::Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  cv::namedWindow("Histograms [" + title + "]", CV_WINDOW_AUTOSIZE );
  imshow("Histograms [" + title + "]", histImage );
}



} // namespace colour_signal
