#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
using namespace cv;

Mat converterParaCinzaMedia(Mat image) {
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < newImage.size().height; y++) {
        for(int x = 0; x < newImage.size().width; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));  
            Vec<unsigned char, 3>& pixel = newImage.at<Vec3b>(Point(x, y));

            pixel[0] = pixel[1] = pixel[2] = (oldImagePixel[0] + oldImagePixel[1] + oldImagePixel[2]) / 3;
        } 
    }

    return newImage;
}

Mat converterParaCinzaPonderado(Mat image) {
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < newImage.size().height; y++) {
        for(int x = 0; x < newImage.size().width; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));  
            Vec<unsigned char, 3>& pixel = newImage.at<Vec3b>(Point(x, y));

            pixel[0] = pixel[1] = pixel[2] = oldImagePixel[0] * 0.114 + oldImagePixel[1] * 0.587 + oldImagePixel[2] * 0.229; // blue
        } 
    }

    return newImage;
}

void makeHistogramCsv(Mat image) {
    std::ofstream myfile;
    myfile.open ("colors.csv");
    for(int y = 0; y < image.size().height; y++) {
        for(int x = 0; x < image.size().width; x++) {     
            Vec<unsigned char, 3>& imagePixel = image.at<Vec3b>(Point(x, y));

            myfile << +imagePixel[0] << ",";
            myfile << +imagePixel[1] << ",";
            myfile << +imagePixel[2] << "\n";
        } 
    }

    myfile.close();


    // system("./criarGrafico.py");
}

int presentHistogram(Mat image) {

    int histRed[256] = {0};
    int histGreen[256] = {0};
    int histBlue[256] = {0};
    
    for (int i = 0; i < image.rows; i++)
        for (int j = 0; j < image.cols; j++)
        {
            Vec<unsigned char, 3>& pixels = image.at<Vec3b>(Point(j, i));
            int red = pixels[0];
            int green = pixels[1];
            int blue = pixels[2];
            histRed[red] = histRed[red] +1;
            histBlue[blue] = histBlue[blue] +1;
            histGreen[green] = histGreen[green] +1;
        }
    Mat HistPlotR (500, 256, CV_8UC3, Scalar(0, 0, 0));
    Mat HistPlotG (500, 256, CV_8UC3, Scalar(0, 0, 0));
    Mat HistPlotB (500, 256, CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < 256; i=i+2)
    {
        line(HistPlotR, Point(i, 500), Point(i, 500-histRed[i]), Scalar(0, 0, 255),1,8,0);
        line(HistPlotG, Point(i, 500), Point(i, 500-histGreen[i]), Scalar(0, 255, 0),1,8,0);
        line(HistPlotB, Point(i, 500), Point(i, 500-histBlue[i]), Scalar(255, 0, 0),1,8,0);
    }
    namedWindow("Red Histogram");
    namedWindow("Green Histogram");
    namedWindow("Blue Histogram");
    imshow("Red Histogram", HistPlotR);
    imshow("Green Histogram", HistPlotG);
    imshow("Blue Histogram", HistPlotB);
    waitKey(0);
    return 0;
}

int showHistogram(Mat image) {
    /// Separate the image in 3 places ( B, G and R )
  std::vector<Mat> bgr_planes;
  split( image, bgr_planes );

  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat b_hist, g_hist, r_hist;

  /// Compute the histograms:
  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = 900; int hist_h = 800;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
  }

    /// Display
  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
  imshow("calcHist Demo", histImage );

  waitKey(0);

  return 0;
}

Mat converterParaCorInvertida(Mat image) {
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < newImage.size().height; y++) {
        for(int x = 0; x < newImage.size().width; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));       
            Vec<unsigned char, 3>& newImagePixel = newImage.at<Vec3b>(Point(x, y));

            newImagePixel[0] = 255 - oldImagePixel[0]; // blue
            newImagePixel[1] = 255 - oldImagePixel[1]; // green
            newImagePixel[2] = 255 - oldImagePixel[2]; // red
        } 
    }

    return newImage;
}

Mat limiarizar(Mat image, int limiar) {
    Mat grayImage = converterParaCinzaPonderado(image);
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));

    for(int y = 0; y < newImage.size().height; y++) {
        for(int x = 0; x < newImage.size().width; x++) {
            Vec3b& grayImagePixel = grayImage.at<Vec3b>(Point(x, y));       
            Vec3b& newImagePixel = newImage.at<Vec3b>(Point(x, y));

            newImagePixel[0] = newImagePixel[1] =  newImagePixel[2] = (grayImagePixel[0] > limiar) ? 255 : 0;
        }
    }

    return newImage;
}

Mat isolarCanalDeCor(std::string cor, Mat image) {
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < image.size().height; y++) {
        for(int x = 0; x < image.size().width; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));       
            Vec<unsigned char, 3>& newImagePixel = newImage.at<Vec3b>(Point(x, y));

            if(cor == "B") {
                newImagePixel[0] = oldImagePixel[0];
                newImagePixel[1] = newImagePixel[2] = 0;
            }

            if(cor == "G") {
                newImagePixel[1] = oldImagePixel[1];
                newImagePixel[0] = newImagePixel[2] = 0;
            }

            if(cor == "R") {
                newImagePixel[2] = oldImagePixel[2];
                newImagePixel[0] = newImagePixel[1] = 0;
            }

        }
    }

    return newImage;
}

Mat incrementarCanaisDeDor(std::string cor, int tipo, int valor, Mat image) {
    // Tipo 0: Inteiro, 1: Percentual
    Mat newImage(image.size().height, image.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < image.size().height; y++) {
        for(int x = 0; x < image.size().width; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));       
            Vec<unsigned char, 3>& newImagePixel = newImage.at<Vec3b>(Point(x, y));

            if(cor == "B") {
                double novoValor = (tipo == 0) ? oldImagePixel[0] + valor : ( oldImagePixel[0] + (oldImagePixel[0] * (valor / 100)));
                newImagePixel[0] = (novoValor <= 255) ? novoValor : 255;
                newImagePixel[1] = oldImagePixel[1];
                newImagePixel[2] = oldImagePixel[2];
            }
            
            if(cor == "G") {
                double novoValor = (tipo == 0) ? oldImagePixel[1] + valor : ( oldImagePixel[1] + (oldImagePixel[1] * (valor / 100)));
                newImagePixel[0] = oldImagePixel[0]; 
                newImagePixel[1] = (novoValor <= 255) ? novoValor : 255;
                newImagePixel[2] = oldImagePixel[2];
            }

            if(cor == "R") {
                double novoValor = (tipo == 0) ? oldImagePixel[2] + valor : ( oldImagePixel[2] + (oldImagePixel[2] * (valor / 100)));
                newImagePixel[0] = oldImagePixel[0];
                newImagePixel[1] = oldImagePixel[1];
                newImagePixel[2] = (novoValor > 255) ? 255 : novoValor;
            }
        }      
    }

    return newImage;
}

Mat zoomIn(int zoomValue, Mat image) {
    if (zoomValue == 1) return image;

    Mat newImage(image.size().height * zoomValue, image.size().width * zoomValue, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < image.rows; y++) {
        for(int x = 0; x < image.cols; x++) {
            Vec<unsigned char, 3>& oldImagePixel = image.at<Vec3b>(Point(x, y));       
            Vec<unsigned char, 3>& newImagePixel = newImage.at<Vec3b>(Point(x, y));

            newImage.at<Vec3b>(Point(x*zoomValue, y*zoomValue)) = oldImagePixel;
            newImage.at<Vec3b>(Point(x*zoomValue +1, y*zoomValue)) = oldImagePixel;
            newImage.at<Vec3b>(Point(x*zoomValue, y*zoomValue +1)) = oldImagePixel;
            newImage.at<Vec3b>(Point(x*zoomValue +1, y*zoomValue +1)) = oldImagePixel;
        }
    }

    return newImage;
}

Mat somarImagem(Mat img1, Mat img2) {
    int height = (img1.size().height > img2.size().height) ? img1.size().height : img2.size().height;
    int width  = (img1.size().width > img2.size().width) ? img1.size().width : img2.size().width;
    Mat newImg(height, width, CV_8UC3, Scalar(255, 255, 255));
    for(int y = 0; y < newImg.size().height; y++) {
        for(int x = 0; x < newImg.size().width; x++) {
            Vec<unsigned char, 3>& newImgPixel = newImg.at<Vec3b>(Point(x, y));
            Vec<unsigned char, 3>& pxlImg1 = img1.at<Vec3b>(Point(x, y));
            Vec<unsigned char, 3>& pxlImg2 = img2.at<Vec3b>(Point(x, y));
            if(x > img1.size().width || y > img1.size().height ||
               x > img2.size().width || y > img2.size().height) 
            {
                newImgPixel[0] = newImgPixel[1] = newImgPixel[2] = 255;
                continue;
            }

            int add0 = pxlImg1[0] + pxlImg2[0];
            int add1 = pxlImg1[1] + pxlImg2[1];
            int add2 = pxlImg1[2] + pxlImg2[2];
            newImgPixel[0] = (add0 > 255) ? 255 : add0;
            newImgPixel[1] = (add1 > 255) ? 255 : add1;
            newImgPixel[2] = (add2 > 255) ? 255 : add2;
        }
    }

    return newImg;
}

Mat subtrairImagem(Mat imgMinuendo, Mat imgSubtraendo) {
    Mat newImg(imgMinuendo.size().height, imgMinuendo.size().width, CV_8UC3, Scalar(0,0,0));
    for(int y = 0; y < newImg.size().height; y++) {
        for(int x = 0; x < newImg.size().width; x++) {
            Vec<unsigned char, 3>& newImgPixel = newImg.at<Vec3b>(Point(x, y));
            Vec<unsigned char, 3>& pxlMinuendo = imgMinuendo.at<Vec3b>(Point(x, y));
            Vec<unsigned char, 3>& pxlSubtraendo = imgSubtraendo.at<Vec3b>(Point(x, y));
            if(x > imgSubtraendo.size().width || y > imgSubtraendo.size().height) {
                newImgPixel[0] = pxlMinuendo[0];
                newImgPixel[1] = pxlMinuendo[1];
                newImgPixel[2] = pxlMinuendo[2];
                continue;
            }

            int sub0 = pxlMinuendo[0] - pxlSubtraendo[0];
            int sub1 = pxlMinuendo[1] - pxlSubtraendo[1];
            int sub2 = pxlMinuendo[2] - pxlSubtraendo[2];
            newImgPixel[0] = (sub0 < 0) ? 0 : sub0;
            newImgPixel[1] = (sub1 < 0) ? 0 : sub1;
            newImgPixel[2] = (sub2 < 0) ? 0 : sub2;
        }
    }
    
    return newImg;
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if(event == CV_EVENT_LBUTTONUP ) {
        printf("Left button up\n");
    }
}

int openWebcam() {
    VideoCapture cap;
    if(!cap.open(0))
        return 0;
    for(;;)
    {
          Mat frame;
          cap >> frame;
          frame = converterParaCinzaPonderado(frame);
          if( frame.empty() ) break;
          imshow("Webcam image", frame);
        //   if( waitKey(30) >= 0 ) break;
    }
    return 0;
}