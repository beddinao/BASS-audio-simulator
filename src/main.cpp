#include "as.h"

//	AUDIO CONTROL

void	move_channel_pos(data *_data, int key) {
	long	c_pos = BASS_ChannelBytes2Seconds(_data->channel, BASS_ChannelGetPosition(_data->channel, BASS_POS_BYTE));

	if (key == MLX_KEY_LEFT)	c_pos -= _data->move_i;
	else			c_pos += _data->move_i;

	if (c_pos <= _data->chan_len && c_pos >= 0)
		BASS_ChannelSetPosition(_data->channel, BASS_ChannelSeconds2Bytes(_data->channel, c_pos), BASS_POS_BYTE);
}

void	SP_channel(data *_data, int status) {
	if (status == BASS_ACTIVE_PAUSED)
		BASS_ChannelPlay(_data->channel, FALSE);
	else	BASS_ChannelPause(_data->channel);
}

//	HOOKS

void	close_handle(void *param) {
	data		*_data = (data*)param;
	mlx_terminate(_data->mlx_ptr);
	delete		_data;
	BASS_Free();
	exit(0);
}

void	handle_key_input(mlx_key_data_t keydata, void *param) {
	if (keydata.action != MLX_PRESS)	return;
	//
	data	*_data = (data*)param;
	if (keydata.key == MLX_KEY_ESCAPE)
		close_handle(_data);
	else if (keydata.key >= 262 && keydata.key <= 263)
		move_channel_pos(_data, keydata.key);
	else if (keydata.key == 32)
		SP_channel(_data, BASS_ChannelIsActive(_data->channel));
}

void	scroll_handle(double xdelta, double ydelta, void *param) {
	data	*_data = (data*)param;

	if (ydelta > 0 && _data->res_mul < 100)
		_data->res_mul += 1;
	else if (ydelta < 0 && _data->res_mul > 1)
		_data->res_mul -= 1;
}

void	resize_handle(int width, int height, void *param) {
	data	*_data = (data*)param;
	int	valid = 0;

	if (height > MIN_HEIGHT) {
		_data->height = height;
		_data->h_height = height / 2;
		valid = 1;
	}
	if (width > MIN_WIDTH) {
		_data->width = width;
		_data->bar_width = width / 3;
		_data->h_width = width / 2;
		valid = 1;
	}
	if (valid) {
		if (!mlx_resize_image(_data->mlx_img, _data->width, _data->height))
			exit(1);
	}
}

//	DRAWING

void	draw_bg(data *_data, int color) {
	for (int y = 0; y < _data->height; y++)
		for (int x = 0; x < _data->width; x++)
			mlx_put_pixel(_data->mlx_img, x, y, color);
}

void	updata_bar(data *_data) {
	int	win_x = _data->width/2 - _data->bar_width/2;
	int	win_y = _data->height - _data->height/15;
	long	c_pos = BASS_ChannelBytes2Seconds(_data->channel, BASS_ChannelGetPosition(_data->channel, BASS_POS_BYTE));
	float	index = 1. * ((float)_data->chan_len/(float)_data->bar_width);
	int	x = win_x;
	int	color;

	for (float l_x = 0; x < _data->bar_width+win_x; x++, l_x += index) {
		if (l_x < c_pos)	color = _data->COLOR;
		else		color = GRAY;
		mlx_put_pixel(_data->mlx_img, x, win_y, color << 8 | 0xFF);
	}
}

void	draw_Sline(mlx_image_t *mlx_img, float x, int start, int end, int color) {
	for (int y = start; y < end; y++)
		mlx_put_pixel(mlx_img, x, y, color);
}

int       adjust_color(int *range, int index, int color)
{
          int                 colors[2][3];
          float               coef;

          if (index == range[0])
                    return (color);
          coef = fabs(1.0 * (range[1] - index) / (range[1] - range[0]));
          colors[0][0] = color >> 16;
          colors[0][1] = (color & 0xFFFF) >> 8;
          colors[0][2] = color & 0xFF;
          colors[1][0] = (int)(colors[0][0] * coef);
          colors[1][1] = (int)(colors[0][1] * coef);
          colors[1][2] = (int)(colors[0][2] * coef);
          return ((colors[1][0] << 16)
                    + (colors[1][1] << 8)
                    + colors[1][2]);
}

int       calculate_height(data* _data, float band, float x)
{
          int height = (int)(band*(float)_data->height);

          if (abs( _data->h_width - x) < 10)		height /= 5;
          else if (abs(_data->h_width - x) < 400)		height *= _data->res_mul;
          else					height *= (_data->res_mul / 2);

          if (!height) return	(0);
          if (height >= _data->h_height)
		height = _data->h_height;
          return (height);
}

void      draw_spectrum(data  *_data, float win_x, float band)
{
          int	height = calculate_height(_data, band, win_x);
          if (!height)	return;

          int	start = _data->h_height - height,
		end = _data->h_height + height;

          int	start_2 = _data->h_height - (height * 5),
		end_2 = _data->h_height + (height * 5);
	
	if (start < 0) start = 1;
	if (end > _data->height) end = _data->height - 1;
          if (start_2 < 0) start_2 = 1;
	if (end_2 > _data->height) end_2 = _data->height-1;

	int	ranges[5][2] = {
		{ _data->h_height, start },
		{ _data->h_height, end },
		{ _data->h_height, 10 },
		{ 0, start_2 },
		{ _data->height, end_2 }
	};

	int	C_COLOR = adjust_color( ranges[2], height, _data->COLOR );

	mlx_put_pixel(_data->mlx_img, win_x, start, WHITE << 8 | 0xFF);
	mlx_put_pixel(_data->mlx_img, win_x, end, WHITE << 8 | 0xFF);

	//draw_Sline(_data->mlx_img, win_x, start_2, end_2, WHITE << 8 | 0xFF);
	//draw_Sline(_data->mlx_img, win_x, end, end_2);

	for (int y = 0; y < start_2; y++)
		mlx_put_pixel(_data->mlx_img, win_x, y, adjust_color( ranges[3], y, GRAY) << 8 | 0xFF);
	for (int y = start + 1; y < end - 1; y++)
		mlx_put_pixel(_data->mlx_img, win_x, y, adjust_color((y  > _data->h_height ? ranges[1] : ranges[0]), y, C_COLOR) << 8 | 0xFF);
	for (int y = end_2; y < _data->height; y++)
		mlx_put_pixel(_data->mlx_img, win_x, y, adjust_color( ranges[4], y, GRAY) << 8 | 0xFF);
}

void	draw(data *_data) {
	int	dir = 1, x_2 = HBANDS - 1;
	draw_bg(_data, 0x000000FF);

	for (float x = 0; x < _data->width; x += _data->x_index) {
		if (x_2 <= 0) dir = 0;
		else if (x_2 == HBANDS - 1) dir = 1;

		draw_spectrum(_data, x, _data->fft[ x_2 ]);
		x_2 += (dir ? -2 : 2);
	}
}

//	TOOLS

int	determine_color(std::string	color) {
          if (color == "RED") return	RED;
          if (color == "BLUE") return	BLUE;
          if (color == "GREY") return	GREY;
          if (color == "PURPLE") return	PURPLE;
          if (color == "WHITE") return	WHITE;
          if (color == "BROWN") return	BROWN;
          return	-1;
}

bool      check_input(char **argv) {
          std::fstream	au_file;
          au_file.open(argv[1], std::ios::in);
          if (!au_file)	return (false);
          au_file.close();
          return    (determine_color(argv[2]) < 0 ? false : true);
}

std::string	generate_title(std::string input) {
	std::string	new_title(input);
	int		r_pos = new_title.rfind("/");

	if (r_pos != new_title.npos)
		new_title = new_title.substr(r_pos + 1);
	r_pos = new_title.find(".");
	if (r_pos != new_title.npos)
		new_title = new_title.substr(0, r_pos);
	return	new_title;
}

//	MAIN

void	loop_hook(void *param) {
	data		*_data = (data*)param;

	_data->frame_c += 1;
	if (_data->frame_c > DPF) {
		_data->frame_c = 0;
		if (BASS_ChannelIsActive(_data->channel) == BASS_ACTIVE_PLAYING) {
			BASS_ChannelGetData(_data->channel, _data->fft, BASS_DATA_FFT1024);
			draw(_data);
			updata_bar(_data);
		}
	}
}

int	main(int c, char **argv){
	if (c != 3 || !check_input(argv)) {
		std::cout << "USAGE: ./EXECUTABLE [AUDIO_FILE] [RED/BLUE/GREY/PURPLE/WHITE/BROWN]"
                              << std::endl;
                    return (1);
	}

	data	*_data = new data;

	_data->width = DWIDTH;
	_data->height = DHEIGHT;
	_data->h_height = DHEIGHT / 2;
	_data->h_width = DWIDTH / 2;
	_data->res_mul = 8;
	_data->mlx_ptr = mlx_init(_data->width, _data->height, generate_title(argv[1]).c_str(), true);

	if (!_data->mlx_ptr) {
		delete	_data;
		return	1;
	}
	_data->mlx_img = mlx_new_image(_data->mlx_ptr, _data->width, _data->height);
	if (!_data->mlx_img) {
		delete	_data;
		return	1;
	}

	draw_bg(_data, 0x000000FF);

	_data->frame_c = 0;
	_data->move_i = 5;
	_data->COLOR = determine_color(argv[2]);
	_data->x_index = 2.0 * ((float)_data->width/(float)BANDS);
	_data->bar_width = _data->width / 3;

	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
		delete	_data;
		return	1;
	}
	_data->channel = BASS_StreamCreateFile(FALSE, argv[1], 0, 0, BASS_SAMPLE_LOOP);
	if (!_data->channel) {
		delete	_data;
		return	1;
	}
	BASS_ChannelPlay(_data->channel, FALSE);
	//BASS_ChannelSetFX(_data->channel, BASS_FX_DX8_I3DL2REVERB, 1);
	_data->chan_len = BASS_ChannelBytes2Seconds(_data->channel, BASS_ChannelGetLength(_data->channel, BASS_POS_BYTE));

	mlx_image_to_window(_data->mlx_ptr, _data->mlx_img, 0, 0);
	mlx_key_hook(_data->mlx_ptr, handle_key_input, _data);
	mlx_close_hook(_data->mlx_ptr, close_handle, _data);
	mlx_scroll_hook(_data->mlx_ptr, scroll_handle, _data);
	mlx_resize_hook(_data->mlx_ptr , resize_handle, _data);

	mlx_loop_hook(_data->mlx_ptr, loop_hook, _data);
	mlx_loop(_data->mlx_ptr);
}
