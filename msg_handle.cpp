#include "msg_handle.h"


 MAX_Message::Queue msgCamearHandle ;
 MAX_Message::Queue test_msgCamearHandle ;
 MAX_Message::Queue msgTransfromHandle;


 cv::Mat QImageToMat(QImage image)
 {
     cv::Mat mat;
     switch (image.format())
     {
     case QImage::Format_ARGB32:
     case QImage::Format_RGB32:
     case QImage::Format_ARGB32_Premultiplied:
         mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
         break;
     case QImage::Format_RGB888:
         mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
         cv::cvtColor(mat, mat, CV_BGR2RGB);
         break;
     case QImage::Format_Indexed8:
         mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
         break;
     }
     return mat;
 }
 QImage Mat2QImage(cv::Mat &image)
 {
     QImage img;
     unsigned char *mat_line;
     unsigned char *buffer_line;
     unsigned char *image_change_buffer = NULL;
     int image_change_buffer_width = 0;
     int image_change_buffer_height = 0;
     int image_change_buffer_channel = 0;
     int i;
     int j;

     if( 3 == image.channels() )
     {
         if( image_change_buffer_width != image.cols
                 || image_change_buffer_height != image.rows
                 || image_change_buffer_channel != image.channels() )
         {
             if( NULL != image_change_buffer )
             {
                 delete []image_change_buffer;
                 image_change_buffer = NULL;
             }
             image_change_buffer = new unsigned char[ image.cols * image.rows * image.channels() ];
             image_change_buffer_width = image.cols;
             image_change_buffer_height = image.rows;
             image_change_buffer_channel = image.channels();
         }
         for( i = 0; i < image_change_buffer_height; ++i )
         {
             mat_line = image.ptr<unsigned char>(i);
             buffer_line = ( image_change_buffer + image_change_buffer_width * 3 * i );
             for( j = 0; j < image_change_buffer_width; ++j )
             {
                 buffer_line[ j * 3 ] = mat_line[ j * 3 + 2 ];
                 buffer_line[ j * 3 + 1 ] = mat_line[ j * 3 + 1 ];
                 buffer_line[ j * 3 + 2 ] = mat_line[ j * 3 ];
             }
         }

         img = QImage( image_change_buffer,
                       image_change_buffer_width,
                       image_change_buffer_height,
                       image_change_buffer_width * image.channels(),
                       QImage::Format_RGB888 ).copy();
         delete []image_change_buffer;
     }
     else if( 1 == image.channels() )
     {
         img = QImage( (const unsigned char *)( image.data ),
                       image.cols,
                       image.rows,
                       image.cols * image.channels(),
                       QImage::Format_ARGB32 );
     }
     else
     {
         img = QImage( (const unsigned char *)( image.data ),
                       image.cols,
                       image.rows,
                       image.cols * image.channels(),
                       QImage::Format_RGB888 );
     }
     return img;
 }

