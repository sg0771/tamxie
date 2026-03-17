#ifndef X264_ARM_PIXEL_H
#define X264_ARM_PIXEL_H

int x264_pixel_sad_16x16_neon(uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_16x8_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_8x16_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_8x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_8x4_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_4x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_4x4_neon  (uint8_t *, int, uint8_t *, int);

int x264_pixel_sad_aligned_16x16_neon(uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_16x8_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x16_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x4_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_4x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_4x4_neon  (uint8_t *, int, uint8_t *, int);

int x264_pixel_sad_aligned_16x16_neon_dual(uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_16x8_neon_dual (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x16_neon_dual (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x8_neon_dual  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_8x4_neon_dual  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_4x8_neon_dual  (uint8_t *, int, uint8_t *, int);
int x264_pixel_sad_aligned_4x4_neon_dual  (uint8_t *, int, uint8_t *, int);


int x264_pixel_sad_4x4_armv6( uint8_t *, int, uint8_t *, int );
int x264_pixel_sad_4x8_armv6( uint8_t *, int, uint8_t *, int );

void x264_pixel_sad_x3_16x16_neon(uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_16x8_neon (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_8x16_neon (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_8x8_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_8x4_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_4x8_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x3_4x4_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);

void x264_pixel_sad_x4_16x16_neon(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_16x8_neon (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_8x16_neon (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_8x8_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_8x4_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_4x8_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);
void x264_pixel_sad_x4_4x4_neon  (uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int *);

int x264_pixel_satd_16x16_neon(uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_16x8_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_8x16_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_8x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_8x4_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_4x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_satd_4x4_neon  (uint8_t *, int, uint8_t *, int);


int x264_pixel_ssd_16x16_neon(uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_16x8_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_8x16_neon (uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_8x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_8x4_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_4x8_neon  (uint8_t *, int, uint8_t *, int);
int x264_pixel_ssd_4x4_neon  (uint8_t *, int, uint8_t *, int);


int x264_pixel_sa8d_8x8_neon  ( uint8_t *, int, uint8_t *, int );
int x264_pixel_sa8d_16x16_neon( uint8_t *, int, uint8_t *, int );

uint64_t x264_pixel_var_8x8_neon( uint8_t *, int );
uint64_t x264_pixel_var_16x16_neon( uint8_t *, int );
int x264_pixel_var2_8x8_neon( uint8_t *, int, uint8_t *, int, int * );

uint64_t x264_pixel_hadamard_ac_8x8_neon( uint8_t *, int );
uint64_t x264_pixel_hadamard_ac_8x16_neon( uint8_t *, int );
uint64_t x264_pixel_hadamard_ac_16x8_neon( uint8_t *, int );
uint64_t x264_pixel_hadamard_ac_16x16_neon( uint8_t *, int );

float x264_pixel_ssim_end4_neon( int sum0[5][4], int sum1[5][4], int width );

#endif
