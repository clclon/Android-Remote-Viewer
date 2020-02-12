/*
throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_));
ImageLite::errCat.message(error_imgl_)
throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_size_is_large));
throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_png_lib_error), lodepng_error_text(err));

*/

__ERRITEM(empty_source,       "lua empty source")
__ERRITEM(bad_source,         "lua load source:")
__ERRITEM(bad_exec,           "lua execute source:")
__ERRITEM(not_main,           "function 'main' not found!")
__ERRITEM(not_access,         "lua script file not access")

#undef __ERRITEM
