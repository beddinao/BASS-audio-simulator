#include "../lib/MLX42/include/MLX42/MLX42.h"
#include "../lib/bass/bass.h"
#include <iostream>
#include <fstream>

#define	DPF		1
#define	BANDS		256
#define	HBANDS		128
#define	DWIDTH		1600
#define	DHEIGHT		800
#define	MAX_WIDTH		2500
#define	MAX_HEIGHT	2000
#define	MIN_WIDTH		300
#define	MIN_HEIGHT	100

typedef	struct	data
{
	mlx_t		*mlx_ptr;
	mlx_image_t	*mlx_img;
	DWORD		channel;
	float		fft[ BANDS*2 ];
	float		x_index;
	float		frame_c;
	long		chan_len;
	int		COLOR;
	int		move_i;
	int		bar_width;
	int		width;
	int		height;
	int		h_width;
	int		h_height;
	int		res_mul;
}	data;

enum	as_colors
{
	WHITE = 0xe0e0e0,
	GRAY = 0x2b2b2b,
	RED = 0xf70804,
	BLUE = 0x0568fc,
	BROWN = 0xb2803a,
	PURPLE = 0x603877,
	GREY = 0x878787,
};
