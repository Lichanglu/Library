



#include <stdio.h>
#include <stdlib.h>

#include "png.h"


#define RGB_FILE  "logo.png"


// 0 y  1 u  2 V
static unsigned char  rgb_to_yuv(int type, int R, int G, int B)
{
	float value = 0.0;
	unsigned char retvalue = 0;



	if(type == 0) {
		value = (0.257 * R) + (0.504 * G) + (0.098 * B) + 16;
	} else if(type == 1) {
		value = (0.439 * R) - (0.368 * G) - (0.071 * B) + 128;
	} else if(type == 2) {
		value = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128;
	}

	retvalue = value;
	//PRINTF("retvalue =%d,value=%f\n",retvalue,value);
	return retvalue;
}


int write2_yuv_4xx(char *buff, int height, int width)
{

	FILE *fp;
	unsigned char *pY, *pU, *pV;
	unsigned int i;
	static int g_time = 0;
	char buf[100];
	
	sprintf(buf, "/%dx%d_%d.yuv", width, height, g_time++);
	//    if(time != 50)
	//       return 1;
	fp = fopen(buf, "wb");
	pY = (unsigned char *)buff;
	pU = (unsigned char *)buff + width * height;
	pV = (unsigned char *)buff + width * height + 1;

	for(i = 0; i < height * width; i++) {
		fputc(*pY, fp);
		pY++;
	}

	for(i = 0; i < height * width / 2 ; i++) {
		fputc(*pU, fp);
		pU++;
		pU++;
	}

	for(i = 0; i < height * width / 2; i++) {
		fputc(*pV, fp);
		pV++;
		pV++;
	}

	fclose(fp);
	printf("write over  height=%d   width=%d   time = %d\n", height, width, g_time);
	return 1;
}



int main(void)
{
  
  
	  unsigned char *row_pointers[1080]; // TODO large enough?
	  unsigned char head[33]; //pngͷ
	  //	Int r, g, b;
	  int x, y;
	  png_structp png_ptr;
	  png_infop info_ptr;	
	  png_uint_32 width, height;
	  int bit_depth, color_type, interlace_type;
	  FILE *infile;
	  int pos = 0;
	  unsigned char* R;
	  unsigned char* G;
	  unsigned char* B;
  
	  char *test_buff = NULL;

	
	//  Convert_init();
	  infile = fopen(RGB_FILE, "rb");
  
	  if(infile == NULL) {
		  printf("Failed to open image file [%s]\n", RGB_FILE);
		  return -1;
	  }
  
	  if(fread(head, 1, sizeof(head), infile) < sizeof(head)) {
		  printf("Failed fread png head=%d,pos=%d\n", sizeof(head), pos);
		  fclose(infile);
		  return -1;
	  }
  
	  fseek(infile, 0L, SEEK_SET);
  
	  if(Png_checkHead(head, sizeof(head))) {
		  printf("Png head error\n");
		  fclose(infile);
		  return -1;
	  }
  
	  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  
	  if(png_ptr == NULL) {
		  printf("Failed to create read struct\n");
		  fclose(infile);

		  return -1;
	  }
  
	  info_ptr = png_create_info_struct(png_ptr);
  
	  if(info_ptr == NULL) {
		  printf("Failed to create info struct\n");
		  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		  fclose(infile);

		  return -1;
	  }
  
	  if(setjmp(png_jmpbuf(png_ptr))) {
		  printf("Failed png_jmpbuf\n");
		  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		  fclose(infile);
		
		  return -1;
	  }
  
	  png_init_io(png_ptr, infile);
  
	  png_read_info(png_ptr, info_ptr);
	  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				   &interlace_type, NULL, NULL);
  
	  png_set_strip_16(png_ptr);
	  png_set_strip_alpha(png_ptr);
	  png_set_packing(png_ptr);
	  png_set_packswap(png_ptr);
  
	  if(color_type == PNG_COLOR_TYPE_PALETTE) {
		  png_set_palette_to_rgb(png_ptr);
	  }
  
	  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		  png_set_gray_1_2_4_to_8(png_ptr);
	  }
  
	  for(y = 0; y < height; y++) {
		  row_pointers[y] = png_malloc(png_ptr,
									   png_get_rowbytes(png_ptr, info_ptr));
	  }
  
	  png_read_image(png_ptr, row_pointers);
	  png_read_end(png_ptr, info_ptr);
	  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,  &interlace_type, NULL, NULL);
	  R=(unsigned char*)malloc(width*height);
	  G=(unsigned char*)malloc(width*height);
	  B=(unsigned char*)malloc(width*height);
	  test_buff = (char *)malloc(width * height * 2);

		if(test_buff == NULL || R == NULL || G == NULL || B ==NULL) {
			printf("Error \n");
			return -1;
		}
		
		for(y = 0; y < height; y++) {
			for(x = 0; x < width; x++) {
			pos = y * width + x;

			R[pos] = row_pointers[y][x * 3 + 0];
			G[pos] = row_pointers[y][x * 3 + 1];
			B[pos] = row_pointers[y][x * 3 + 2];

			*(test_buff + pos) = rgb_to_yuv(0, R[pos] , G[pos] , B[pos]);

			if(pos % 2  == 0) {
				*(test_buff + pos + width * height) = rgb_to_yuv(1,  R[pos] , G[pos] , B[pos]);
				*(test_buff + pos + width * height + 1) = rgb_to_yuv(2,  R[pos] , G[pos] , B[pos]);

			}
		}
	}
	write2_yuv_4xx(test_buff,height,width);
	  

	
	return 0;
}



