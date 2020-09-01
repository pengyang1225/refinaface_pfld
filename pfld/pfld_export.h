//
// Created by pengyang on 20-8-21.
//

#ifndef LIBTORCH_YOLOV5_PFLD_EXPORT_H
#define LIBTORCH_YOLOV5_PFLD_EXPORT_H



typedef struct Direction_point
{
    int x;
    int y;
}Direction_point;

typedef struct DirectionNum_point
{
    Direction_point* point;
    int num;
}DirectionNum_point;

typedef struct followImage_
{
    unsigned char * data;
    int   w;
    int   h;
    int byte_size;
}followImage;

typedef void				MAX_VOID;

typedef MAX_VOID *			MAX_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

void pfld_init(MAX_HANDLE* handle, const char* model_path, const bool gpu_flag);


void  pfld_detect(MAX_HANDLE handle,followImage* image,DirectionNum_point* direct_num,const float conf);
void  pfld_free(MAX_HANDLE *handle);





#ifdef __cplusplus
}
#endif



#endif //LIBTORCH_YOLOV5_PFLD_EXPORT_H
