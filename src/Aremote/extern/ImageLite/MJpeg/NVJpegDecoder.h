
#pragma once

namespace ImageLite
{
	namespace JpegGpu
	{
		struct gpuparams_t;
		//
		class NVJpegDecoder
		{
		public:
			//
			struct gpuimage_t
			{
				int32_t height, width, channel, pitch, pad;
				int32_t size() noexcept
				{
					return ((pitch + pad) * height);
				}
				void info(std::ostream& os) const
				{
					os << "width: " << width;
					os << ", height: " << height;
					os << ", channel: " << channel;
					os << ", pitch: " << pitch;
					os << ", pad: " << pad;
				}
				friend std::ostream& operator << (std::ostream& os, gpuimage_t const& obj)
				{
					obj.info(os);
					return os;
				}
			};
		private:
			//
			std::unique_ptr<gpuparams_t> nvparam;
			bool                         nvisenable = false;
			//
			void init();
			void clean();
			void imgalloc();
			void imginfo(std::vector<uint8_t> const&);
			void imgbuffer(std::vector<uint8_t>&);
			void imgdecompress(std::vector<uint8_t> const&, std::vector<uint8_t>&);

		public:
			//
			gpuimage_t imgparam{};
			std::error_code error;
			//
			NVJpegDecoder() noexcept;
			NVJpegDecoder(int32_t);
			~NVJpegDecoder();
			//
			bool isenable();
			bool imgread(std::string const&, std::vector<uint8_t>&);
			bool imgstream(std::vector<uint8_t> const&, std::vector<uint8_t>&);
			//
			static const char* cudaGetError(uint32_t);
		};
	}
}