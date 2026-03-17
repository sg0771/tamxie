
#include <Windows.h>

#include <stdint.h>

class  TSMuxer
{
	int64_t  system_clock_frequency = 27000000;
	int64_t  basicValue = 8589934592;
	int  FRISTTSLEN = 176;     //包含TS包头 和 PCR 
	int  MINTSLEN = 184;    //包含TS
	int  LASTLEN = 182;     //包含TS
	int  FRISTTSLENAUDIO = 182;
	const uint64_t delay = 200;
	uint8_t arrarff[188] = { 0 };

	struct TSBuffer
	{
		uint8_t* data = nullptr;
		uint32_t d_cur;//当前Pos
		uint32_t d_max;//已经使用的空间，不足就扩张

		TSBuffer(uint32_t size = 188)
		{
			data = new uint8_t[size];
			memset(data, 0xff, size);
			d_max = size;
			d_cur = 0;
		}
		virtual ~TSBuffer()
		{
			delete[] data;
		}
	};

	int  append_data(TSBuffer* c, uint8_t* data, uint32_t  size)
	{
		unsigned ns = c->d_cur + size;

		if (ns > c->d_max) //扩张
		{
			uint8_t* newdata = new uint8_t[ns];
			memcpy((void*)newdata, c->data, c->d_cur);
			c->d_max = ns;
			if (c->data)
			{
				delete[] c->data;
			}
			c->data = newdata;
		}

		memcpy(c->data + c->d_cur, data, size);
		c->d_cur = ns;
		return 0;
	}

	void put_byte(TSBuffer* c, uint8_t b)
	{
		append_data(c, &b, 1);
	}

	void put_be32(TSBuffer* c, uint32_t val)
	{
		put_byte(c, val >> 24);
		put_byte(c, val >> 16);
		put_byte(c, val >> 8);
		put_byte(c, val);
	}

	void put_be16(TSBuffer* c, uint16_t val)
	{
		put_byte(c, val >> 8);
		put_byte(c, val);
	}

	void put_be24(TSBuffer* c, uint32_t val)
	{
		put_be16(c, val >> 8);
		put_byte(c, val);
	}

	unsigned int crc32table[256] = {
		0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
		0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
		0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
		0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
		0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
		0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
		0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
		0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
		0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
		0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
		0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
		0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
		0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
		0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
		0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
		0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
		0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
		0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
		0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
		0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
		0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
		0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
		0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
		0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
		0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
		0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
		0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
		0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
		0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
		0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
		0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
		0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
		0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
		0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
		0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
		0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
		0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
		0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
		0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
		0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
		0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
		0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
		0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
	};

	uint32_t CRC32(const uint8_t* data, int len)
	{
		int i;
		uint32_t crc = 0xFFFFFFFF;
		for (i = 0; i < len; i++)
			crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *data++) & 0xFF];
		return crc;
	}

public:
	TSBuffer m_bufAduio;
	TSBuffer m_pesAudioBuffer;
	TSBuffer m_bufVideo;
	TSBuffer m_pesVideoBuffer;
	TSMuxer() {
		memset(arrarff, 0xff, 188);
	}

	void HandleAudio(uint8_t* encBuf, uint32_t bufsize, int64_t timestamp, uint8_t** pOut, int* nOutSize)
	{
		m_bufAduio.d_cur = 0;
		m_pesAudioBuffer.d_cur = 0;
		CreatePesAudioPacket(&m_pesAudioBuffer, (uint8_t*)encBuf, bufsize, timestamp);
		bool bFritsTsPacket = true;
		uint32_t nread = 0;
		while (nread < m_pesAudioBuffer.d_cur)
		{
			if (bFritsTsPacket)
			{
				bFritsTsPacket = false;
				int remainSize = m_pesAudioBuffer.d_cur - nread;
				if (remainSize <= FRISTTSLENAUDIO)
				{
					PutFristTsAudio(&m_bufAduio, timestamp, FRISTTSLENAUDIO - m_pesAudioBuffer.d_cur);
					append_data(&m_bufAduio, m_pesAudioBuffer.data, m_pesAudioBuffer.d_cur);
				}
				else
				{
					PutFristTsAudio(&m_bufAduio, timestamp, 0);
					append_data(&m_bufAduio, m_pesAudioBuffer.data, FRISTTSLENAUDIO);
				}
				nread = FRISTTSLENAUDIO;
			}
			else
			{
				int remainSize = m_pesAudioBuffer.d_cur - nread;
				if (remainSize >= MINTSLEN)
				{
					PutMiddleTsAudio(&m_bufAduio);
					append_data(&m_bufAduio, m_pesAudioBuffer.data + nread, MINTSLEN);
					nread += MINTSLEN;
				}
				else
				{
					PutlastTsAudio(&m_bufAduio, MINTSLEN - remainSize);
					append_data(&m_bufAduio, m_pesAudioBuffer.data + nread, remainSize);
					nread += remainSize;
				}
			}
		}
		//if (m_cb) {
		//	m_cb(m_pSink, m_bufAduio.data, m_bufAduio.d_cur);
		//}
		*pOut = m_bufAduio.data;
		*nOutSize = m_bufAduio.d_cur;
	}

	void HandleVideo(uint8_t* encBuf, uint32_t bufsize, int64_t timestamp, uint8_t** pOut, int* nOutSize)
	{
		m_bufVideo.d_cur = 0;
		m_pesVideoBuffer.d_cur = 0;

		if (m_nCount % 20 == 0) {
			PutPat(&m_bufVideo);
			PutPmt(&m_bufVideo);
		}
		m_nCount++;

		CreatePesVideoPacket(&m_pesVideoBuffer, (uint8_t*)encBuf, bufsize, timestamp);
		bool bFritsTsPacket = true;;
		uint32_t nread = 0;
		int remainSize;
		while (nread < m_pesVideoBuffer.d_cur)
		{
			if (bFritsTsPacket)
			{
				bFritsTsPacket = false;
				remainSize = m_pesVideoBuffer.d_cur - nread;
				if (remainSize <= FRISTTSLEN)
				{
					PutFristTsVideo(&m_bufVideo, timestamp, FRISTTSLEN - m_pesVideoBuffer.d_cur);
					append_data(&m_bufVideo, m_pesVideoBuffer.data, m_pesVideoBuffer.d_cur);
				}
				else
				{
					PutFristTsVideo(&m_bufVideo, timestamp, 0);
					append_data(&m_bufVideo, m_pesVideoBuffer.data, FRISTTSLEN);
				}
				nread = FRISTTSLEN;
			}
			else
			{
				remainSize = m_pesVideoBuffer.d_cur - nread;
				if (remainSize >= MINTSLEN)
				{
					PutMiddleTsVideo(&m_bufVideo);
					append_data(&m_bufVideo, m_pesVideoBuffer.data + nread, MINTSLEN);
					nread += MINTSLEN;
				}
				else
				{
					PutlastTsVideo(&m_bufVideo, MINTSLEN - remainSize);
					append_data(&m_bufVideo, m_pesVideoBuffer.data + nread, remainSize);
					nread += remainSize;
				}
			}
		}
		*pOut = m_bufVideo.data;
		*nOutSize = m_bufVideo.d_cur;
	}
private:
	uint8_t m_VideoCount = 0;	//视频计数
	uint8_t m_AduioCount = 0;	//音频计数
	uint8_t m_PATCount = 0;	//PAT计数
	uint8_t m_PMTCount = 0;	//PMT计数

	int64_t m_nCount = 0;
	uint16_t m_nprogram_map_PID = 0x0100;// 总的节目ID
	uint16_t m_nAudioPID = 0x0101;  //音频的PID
	uint16_t m_nVideoPID = 0x0102;  //视频的PID

	//filenum 填充ff的大小
	void PutFristTsVideo(TSBuffer* ptsbuf, int64_t timestamp, uint8_t fileNum)
	{
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x1;    //1bit 有效荷载单元起始指示符   
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 3;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_VideoCount > 15)
		{
			m_VideoCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);
		//8byte

		uint8_t adaptation_field_length = 7 + fileNum;// 长度  8bit 
		uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
		uint8_t random_access_indicator = 0x1; //1bit 为0x0   
		uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
		uint8_t PCR_flag = 0x1;//1bit 为0x0  时间戳标志位
		uint8_t OPCR_flag = 0x0;//1bit 为0x0  
		uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
		uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
		uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
		//2 byte
		put_byte(ptsbuf, adaptation_field_length);
		put_byte(ptsbuf, PCR_flag << 4);
		//时间戳
		int64_t pcr_base = (timestamp * system_clock_frequency / 90000 / 300) % basicValue; //bit 33

		uint16_t PCR_ext = (system_clock_frequency * timestamp / 90000) % 300;

		int64_t PCR = pcr_base * 300 + PCR_ext;

		uint32_t pcr_base32 = pcr_base >> 1;

		//6byte
		put_be32(ptsbuf, pcr_base32);

		uint8_t nextbit = pcr_base & 0x0000000000000001;
		if (PCR_ext > 255)
		{
			put_byte(ptsbuf, nextbit << 7 | 0x01);
			PCR_ext -= 256;
		}
		else
		{
			put_byte(ptsbuf, nextbit << 7 | 0x00);
		}
		put_byte(ptsbuf, PCR_ext);
		if (fileNum > 0)
		{
			append_data(ptsbuf, arrarff, fileNum);
		}

	}
	void PutMiddleTsVideo(TSBuffer* ptsbuf)
	{
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 01;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_VideoCount > 15)
		{
			m_VideoCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);
	}
	//填充 ff 的大小
	void PutlastTsVideo(TSBuffer* ptsbuf, uint8_t filesze)
	{
		uint8_t sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 03;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_VideoCount > 15)
		{
			m_VideoCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);

		uint8_t adaptation_field_length = 0 + filesze;// 长度  8bit 
		uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
		uint8_t random_access_indicator = 0x1; //1bit 为0x0   
		uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
		uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
		uint8_t OPCR_flag = 0x0;//1bit 为0x0  
		uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
		uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
		uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
		//2 byte

		if (filesze == 1)
		{
			adaptation_field_length = 0;;
			put_byte(ptsbuf, adaptation_field_length);
		}
		else if (filesze == 2)
		{
			adaptation_field_length = 1;
			put_byte(ptsbuf, adaptation_field_length);
			put_byte(ptsbuf, random_access_indicator << 6);
		}
		else
		{
			adaptation_field_length = filesze - 1;
			put_byte(ptsbuf, adaptation_field_length);
			put_byte(ptsbuf, random_access_indicator << 6);
			append_data(ptsbuf, arrarff, filesze - 2);
		}

	}

	void CreatePesVideoPacket(TSBuffer* ptsbuf, uint8_t* videobuf, uint32_t videoSize, int64_t timestamp)
	{
		uint8_t	nPTS_DTS_flags = 0x3;  //变量

		const uint8_t* p = videobuf, * buf_end = p + videoSize;
		uint32_t state = -1;
		uint32_t packet_start_code_prefix = 0x000001;		// 24bit PES包起始前缀码 固定为0x000001
		uint8_t	 stream_id = 0xE0;	        		//8bit PES流标识 视频为0xE0
		uint32_t pes_packet_length32 = 6;;		            //16bit PES包长度

		if (nPTS_DTS_flags == 0x2)
		{
			pes_packet_length32 += videoSize + 5 + 3;
		}
		else if (nPTS_DTS_flags == 0x3)
		{
			pes_packet_length32 += videoSize + 10 + 3;
		}
		uint16_t pes_packet_length16 = pes_packet_length32;
		if (pes_packet_length32 > 0xffff)
		{
			pes_packet_length16 = 0x00;
		}

		put_be24(ptsbuf, packet_start_code_prefix);
		put_byte(ptsbuf, stream_id);
		put_be16(ptsbuf, pes_packet_length16);

		//1byte 
		uint8_t fix = 0x2;  //2bit 固定10
		uint8_t	pes_scrambling_control = 0x0;		//2bit PES加扰控制
		uint8_t	pes_priority = 0x0;					//1bitPES优先级
		uint8_t	data_alignment_indicator = 0x0;		//1bit数据定位指示符
		uint8_t	copyright = 0x0;		     //1bit版权
		uint8_t	original_or_copy = 0x0;		//1bit 原版或拷贝
		put_byte(ptsbuf, fix << 6 | data_alignment_indicator << 2);

		//1byte 
		uint8_t	PTS_DTS_flags = nPTS_DTS_flags; 		//2bit  PTS/DTS标志
		uint8_t	ESCR_flag = 0x0;		     //1bit ESCR_flag
		uint8_t ES_rate_flag = 0x0;          //1bit ES_rate_flag
		uint8_t DSM_trick_mode_flag = 0x0;          //1bit DSM_trick_mode_flag
		uint8_t additional_copy_info_flag = 0x0;          //1bit additional_copy_info_flag
		uint8_t PES_CRC_flag = 0x0;          //1bit PES_CRC_flag
		uint8_t PES_extension_flag = 0x0;          //1bit PES_extension_flag
		put_byte(ptsbuf, PTS_DTS_flags << 6);

		//1byte 
		uint8_t PES_header_data_length = 0xa; //8bit PES_header_data_length
		if (nPTS_DTS_flags == 0x2)
		{
			PES_header_data_length -= 5;
		}
		put_byte(ptsbuf, PES_header_data_length);
		if (nPTS_DTS_flags == 0x3)
		{
			int64_t Dts64 = timestamp + delay;

			uint8_t fix1 = 0x3;//4bit 固定为0x3
			uint16_t PTS30_32 = Dts64 >> 30 & 0x7;  //3bit PTS 30——32 
			uint8_t marker_bit = 0x1;//1bit 固定为1
			put_byte(ptsbuf, fix1 << 4 | PTS30_32 << 1 | marker_bit);

			uint16_t PTS15_29 = (Dts64 & 0x3FFF8000) >> 15;  //3bit PTS 15——29 
			uint8_t marker_bit1 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, PTS15_29 << 1 | marker_bit1);

			uint16_t PTS0_14 = Dts64 & 0x7FFF;  //3bit PTS 15——29 
			uint8_t marker_bit2 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, PTS0_14 << 1 | marker_bit2);

			//DTS 5byte
			uint8_t fix2 = 0x1;//4bit 固定为0x1
			uint16_t DTS30_32 = PTS30_32;  //3bit DTS 30——32 
			uint8_t marker_bit3 = 0x1;//1bit 固定为1 
			put_byte(ptsbuf, fix2 << 4 | DTS30_32 << 1 | marker_bit3);


			uint16_t DTS15_29 = PTS15_29;  //15bit DTS 15——29 
			uint8_t marker_bit4 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, DTS15_29 << 1 | marker_bit4);

			uint16_t DTS0_14 = PTS0_14;  //15bit DTS 0——15
			uint8_t marker_bit5 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, DTS0_14 << 1 | marker_bit5);

			//if (bkey)
			//{
				//put_be32(ptsbuf, 0x00000001);
				//put_byte(ptsbuf, 0x09);
				//put_byte(ptsbuf, 0xf0);
			//}
		}
		else if (PTS_DTS_flags == 0x02)
		{

			int64_t nowDts = timestamp + delay;

			uint8_t fix1 = 0x3;//4bit 固定为0x3
			uint16_t PTS30_32 = nowDts >> 30 & 0x7;  //3bit PTS 30——32 
			uint8_t marker_bit = 0x1;//1bit 固定为1
			put_byte(ptsbuf, fix1 << 4 | PTS30_32 << 1 | marker_bit);

			uint16_t PTS15_29 = (nowDts & 0x3FFF8000) >> 15;  //3bit PTS 15——29 
			uint8_t marker_bit1 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, PTS15_29 << 1 | marker_bit1);


			uint16_t PTS0_14 = nowDts & 0x7FFF;  //3bit PTS 15——29 
			uint8_t marker_bit2 = 0x1;//1bit 固定为1 
			put_be16(ptsbuf, PTS0_14 << 1 | marker_bit2);


			//if (bkey)    // AUD NAL
			//{
				//put_be32(ptsbuf, 0x00000001);
				//put_byte(ptsbuf, 0x09);
				//put_byte(ptsbuf, 0xff);
			//}
		}
		append_data(ptsbuf, videobuf, videoSize);
	}
	void CreatePesAudioPacket(TSBuffer* ptsbuf, uint8_t* audiobuf, uint32_t audioSize, int64_t timestamp)
	{
		uint32_t packet_start_code_prefix = 0x000001;		// 24bit PES包起始前缀码 固定为0x000001
		uint8_t	 stream_id = 0xC0;	        		//8bit PES流标识 音频为0xC0
		uint16_t pes_packet_length = audioSize + 5 + 3;;		            //16bit PES包长度

		put_be24(ptsbuf, packet_start_code_prefix);
		put_byte(ptsbuf, stream_id);
		put_be16(ptsbuf, pes_packet_length);

		//1byte 
		uint8_t fix = 0x2;  //2bit 固定10
		uint8_t	pes_scrambling_control = 0x0;		//2bit PES加扰控制
		uint8_t	pes_priority = 0x0;					//1bitPES优先级
		uint8_t	data_alignment_indicator = 0x1;		//1bit数据定位指示符
		uint8_t	copyright = 0x0;		     //1bit版权
		uint8_t	original_or_copy = 0x0;		//1bit 原版或拷贝
		put_byte(ptsbuf, fix << 6 | data_alignment_indicator << 2);

		//1byte 
		uint8_t	PTS_DTS_flags = 0x2; 		 //2bit  PTS/DTS标志
		uint8_t	ESCR_flag = 0x0;		     //1bit ESCR_flag
		uint8_t ES_rate_flag = 0x0;          //1bit ES_rate_flag
		uint8_t DSM_trick_mode_flag = 0x0;          //1bit DSM_trick_mode_flag
		uint8_t additional_copy_info_flag = 0x0;          //1bit additional_copy_info_flag
		uint8_t PES_CRC_flag = 0x0;          //1bit PES_CRC_flag
		uint8_t PES_extension_flag = 0x0;          //1bit PES_extension_flag
		put_byte(ptsbuf, PTS_DTS_flags << 6);


		//1byte 
		uint8_t PES_header_data_length = 0x5; //8bit PES_header_data_length
		put_byte(ptsbuf, PES_header_data_length);


		int64_t nowPts = timestamp + delay;//+ delay;
		//PTS 5byte
		uint8_t fix1 = 0x2;//4bit 固定为0x3
		uint16_t PTS30_32 = nowPts >> 30 & 0x7;  //3bit PTS 30——32 
		uint8_t marker_bit = 0x1;//1bit 固定为1
		put_byte(ptsbuf, fix1 << 4 | PTS30_32 << 1 | marker_bit);

		uint16_t PTS15_29 = (nowPts & 0x3FFF8000) >> 15;  //3bit PTS 15——29 
		uint8_t marker_bit1 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf, PTS15_29 << 1 | marker_bit1);

		uint16_t PTS0_14 = nowPts & 0x7FFF;  //3bit PTS 15——29 
		uint8_t marker_bit2 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf, PTS0_14 << 1 | marker_bit2);
		append_data(ptsbuf, audiobuf, audioSize);
	}
	void PutFristTsAudio(TSBuffer* ptsbuf, int64_t timestamp, uint8_t fileNum)
	{
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x1;    //1bit 有效荷载单元起始指示符   
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 0x3;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_AduioCount > 15)
		{
			m_AduioCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);
		//8byte

		uint8_t adaptation_field_length = 1 + fileNum;// 长度  8bit 
		uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
		uint8_t random_access_indicator = 0x1; //1bit 为0x0   
		uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
		uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
		uint8_t OPCR_flag = 0x0;//1bit 为0x0  
		uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
		uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
		uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
		//2 byte
		put_byte(ptsbuf, adaptation_field_length);
		put_byte(ptsbuf, 0x0);

		if (fileNum > 0)
		{
			append_data(ptsbuf, arrarff, fileNum);
		}

	}
	void PutMiddleTsAudio(TSBuffer* ptsbuf)
	{
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 0x01;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_AduioCount > 15)
		{
			m_AduioCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);

	}
	void PutlastTsAudio(TSBuffer* ptsbuf, uint8_t filesze)
	{
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 03;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

		if (m_AduioCount > 15)
		{
			m_AduioCount = 0;
		}
		// 4byte
		put_byte(ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID >> 8);
		put_byte(ptsbuf, PID);

		put_byte(ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);

		uint8_t adaptation_field_length = 0 + filesze;// 长度  8bit 
		uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
		uint8_t random_access_indicator = 0x1; //1bit 为0x0   
		uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
		uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
		uint8_t OPCR_flag = 0x0;//1bit 为0x0  
		uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
		uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
		uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
		//2 byte

		if (filesze == 1)
		{
			adaptation_field_length = 0;;
			put_byte(ptsbuf, adaptation_field_length);
		}
		else if (filesze == 2)
		{
			adaptation_field_length = 1;
			put_byte(ptsbuf, adaptation_field_length);
			put_byte(ptsbuf, random_access_indicator << 6);
		}
		else
		{
			adaptation_field_length = filesze - 1;
			put_byte(ptsbuf, adaptation_field_length);
			put_byte(ptsbuf, random_access_indicator << 6);
			append_data(ptsbuf, arrarff, filesze - 2);
		}

	}

	void PutPmt(TSBuffer* bufDst)
	{
		TSBuffer ptsbuf;
		//插入ts 包头
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 1;	   //1bit 有效荷载单元起始指示符 
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		//	uint16_t PID = m_nprogram_map_PID;             //13bit PMT 是 0x100
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 1;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_PMTCount;		       //4bit 连续计数器 一个4bit的计数器，范围0-15

		m_PMTCount++;
		if (m_PMTCount > 15) {
			m_PMTCount = 0;
		}
		put_byte(&ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(&ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			m_nprogram_map_PID >> 8);
		put_byte(&ptsbuf, m_nprogram_map_PID);

		put_byte(&ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);

		//ts包头结束
		uint8_t Point_field = 0;
		put_byte(&ptsbuf, Point_field);

		int startCrc = ptsbuf.d_cur;
		//开始写入pmt数据

		// 6byte
		uint8_t table_id = 0x02;                 // 8bit pmt table_id 是0x02
		put_byte(&ptsbuf, table_id);

		uint8_t	section_syntax_indicator = 1;    // 1 bslbf 
		uint8_t zero = 0;	                     // 1bsl bf 
		uint8_t	reserved_1 = 0x03;               // 2 bslbf 固定为0x03
		uint16_t section_length = 23;          // 12  长度 uimsbf 首先两位bit置为00，它指示段的byte数，由段
		//长度域开始，包含CRC

		// 8byte
		put_be16(&ptsbuf, section_syntax_indicator << 15 |
			zero << 14 |
			reserved_1 << 12 |
			section_length);


		//2 byte  NO: 10byte
		uint16_t program_number = 0x01;          // 16 uimsbf 
		put_be16(&ptsbuf, program_number);

		//1 byte NO: 11byte
		uint8_t	reserved_2 = 0x03;               // 2bsl bf 固定为0x03
		uint8_t	version_number = 0x0;            // 5ui m sbf 固定为0
		uint8_t	current_next_indicator = 0x01;   // 1 bslbf 固定为0x01

		put_byte(&ptsbuf, reserved_2 << 6 |
			version_number << 1 |
			current_next_indicator
		);
		//2 byte  NO: 13byte
		uint8_t	section_number = 0x0;            // 8 uimsbf 固定为0x0
		uint8_t	last_section_number = 0x0;      // 8 uimsbf  固定为0x0
		put_byte(&ptsbuf, section_number);
		put_byte(&ptsbuf, last_section_number);

		//2 byte NO: 15byte
		uint8_t	reserved3 = 0x07;                // 3 bslbf 固定为0x07
		uint16_t PCR_PID = m_nVideoPID;         // 13 uimsbf 指明TS包的PID值，该TS包含有PCR域，

		put_byte(&ptsbuf, reserved3 << 5 | PCR_PID >> 8);
		put_byte(&ptsbuf, PCR_PID);


		//2 byte NO: 17byte
		uint8_t	reserved4 = 0x0F;                //4 bslbf 
		uint16_t program_info_length = 0x0000;    //12 u i m s b f 

		put_byte(&ptsbuf, reserved4 << 4 | program_info_length >> 8);
		put_byte(&ptsbuf, program_info_length);

		//write vidoe 5byte
		uint8_t steam_type;      //8bit  
		uint8_t	reserved5;        //3bit  固定为 0x07
		uint16_t elementary_PID; //13bit 
		uint8_t	 reserved6;       //4bit  固定为 0x0f 
		uint16_t ES_info_length; //12bit 固定为 0x00 

		//write aac 5byte
		steam_type = 0x0f;      //8bit  音频的steam_type 为0x0f
		reserved5 = 0x07;        //3bit  固定为 0x07
		elementary_PID = m_nAudioPID;   //13bit 视频的 
		reserved6 = 0x0f;        //4bit  固定为 0x0f 
		ES_info_length = 0x00;  //12bit 固定为 0x00 
		put_byte(&ptsbuf, steam_type);
		put_byte(&ptsbuf, reserved5 << 5 |
			elementary_PID >> 8);
		put_byte(&ptsbuf, elementary_PID);

		put_byte(&ptsbuf, reserved6 << 4 | ES_info_length >> 8);
		put_byte(&ptsbuf, ES_info_length);

		//write vidoe 5byte
		steam_type = 0x1b;      //8bit  视频的steam_type 为0x1b
		reserved5 = 0x07;        //3bit  固定为 0x07
		elementary_PID = m_nVideoPID; //13bit 视频的 
		reserved6 = 0x0f;       //4bit  固定为 0x0f 
		ES_info_length = 0x00; //12bit 固定为 0x00 

		put_byte(&ptsbuf, steam_type);
		put_byte(&ptsbuf, reserved5 << 5 |
			elementary_PID >> 8);
		put_byte(&ptsbuf, elementary_PID);

		put_byte(&ptsbuf, reserved6 << 4 | ES_info_length >> 8);
		put_byte(&ptsbuf, ES_info_length);

		// 4byte
		uint32_t nCrc32 = CRC32(ptsbuf.data + startCrc, ptsbuf.d_cur - startCrc);
		put_be32(&ptsbuf, nCrc32);
		ptsbuf.d_cur = 188;
		append_data(bufDst, ptsbuf.data, ptsbuf.d_cur);
	}

	void PutPat(TSBuffer* bufDst)
	{
		TSBuffer ptsbuf;
		uint8_t  sync_byte = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
		uint8_t transport_error_indicator = 0;		   //1bit 传输误码指示符 
		uint8_t payload_unit_start_indicator = 1;	   //1bit 有效荷载单元起始指示符 
		uint8_t transport_priority = 0;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
		uint16_t PID = 0x0000;                          //13bit Pat 是 0x0000
		uint8_t transport_scrambling_control = 0x0; //2bit 传输加扰控制 
		uint8_t adaptation_field_control = 1;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
		uint8_t continuity_counter = m_PATCount;		       //4bit 连续计数器 一个4bit的计数器，范围0-15

		m_PATCount++;

		if (m_PATCount > 15) {
			m_PATCount = 0;
		}
		put_byte(&ptsbuf, 0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
		put_byte(&ptsbuf, transport_priority << 7 |
			payload_unit_start_indicator << 6 |
			transport_priority << 5 |
			PID << 8);
		put_byte(&ptsbuf, PID >> 8);

		put_byte(&ptsbuf, transport_scrambling_control << 6 |
			adaptation_field_control << 4 |
			continuity_counter);

		//ts包头结束
		uint8_t Point_field = 0;
		put_byte(&ptsbuf, Point_field);
		int startCrc = ptsbuf.d_cur;
		//开始写入PAT数据

		uint8_t table_id = 0x00;                 // 8bit PAT table_id 是0x00
		put_byte(&ptsbuf, table_id);

		uint8_t	section_syntax_indicator = 1;    // 1 bslbf 
		uint8_t zero = 0;	                     // 1bsl bf 
		uint8_t	reserved_1 = 0x3;               // 2 bslbf 固定为0x3
		uint16_t section_length = 13;          // 12  长度 uimsbf 首先两位bit置为00，它指示段的byte数 
		//12;//表示这个字节后面有用的字节数，包括CRC32
		//长度域开始，包含CRC

		put_be16(&ptsbuf, section_syntax_indicator << 15 |
			zero << 14 |
			reserved_1 << 12 |
			section_length
		);

		//2 byte
		uint16_t transport_stream_id = 0x0001;          // 16 uimsbf 
		put_be16(&ptsbuf, transport_stream_id);

		//1 byte
		uint8_t	reserved_2 = 0x03;               // 2bsl bf 固定为0x03
		uint8_t	version_number = 0x0;            // 5ui m sbf 固定为0
		uint8_t	current_next_indicator = 0x01;   // 1 bslbf 固定为0x01

		put_byte(&ptsbuf, reserved_2 << 6 |
			version_number << 1 |
			current_next_indicator
		);
		//2 byte
		uint8_t	section_number = 0x0;            // 8 uimsbf 固定为0x0
		uint8_t	last_section_number = 0x0;      // 8 uimsbf  固定为0x0
		put_byte(&ptsbuf, section_number);
		put_byte(&ptsbuf, last_section_number);


		//2 byte
		uint16_t program_number = 0x0001;
		put_be16(&ptsbuf, program_number);

		//2 byte
		uint8_t	 reserved3 = 0x07;           // 3 bslbf 固定为0x07
		//uint16_t program_map_PID = m_nprogram_map_PID;  //13bit 节目映射表的PID，节目号大于0时对应的PID，每个节目对应一个
		put_byte(&ptsbuf, reserved3 << 5 | m_nprogram_map_PID >> 8);
		put_byte(&ptsbuf, m_nprogram_map_PID);


		// 4byte
		uint32_t nCrc32 = CRC32(ptsbuf.data + startCrc, ptsbuf.d_cur - startCrc);
		put_be32(&ptsbuf, nCrc32);
		ptsbuf.d_cur = 188;
		append_data(bufDst, ptsbuf.data, ptsbuf.d_cur);
	}
};

EXTERN_C void* TSMuxerCreate() {
	TSMuxer* muxer = new TSMuxer;
	return (void*)muxer;
}

EXTERN_C void TSMuxerDestroy(void* ptr) {
	if (ptr) {
		TSMuxer* muxer = (TSMuxer*)ptr;
		delete muxer;
	}
}

EXTERN_C void TSMuxerHandleVideo(void* ptr, uint8_t* vidoebuf, uint32_t bufsize, int64_t tic, uint8_t** pOut, int* nOutSize) {
	if (ptr) {
		TSMuxer* muxer = (TSMuxer*)ptr;
		muxer->HandleVideo(vidoebuf, bufsize, tic, pOut, nOutSize);
	}
}

EXTERN_C void TSMuxerHandleAudio(void* ptr, uint8_t* aacbuf, uint32_t bufsize, int64_t tic, uint8_t** pOut, int* nOutSize) {
	if (ptr) {
		TSMuxer* muxer = (TSMuxer*)ptr;
		muxer->HandleAudio(aacbuf, bufsize, tic, pOut, nOutSize);
	}
}
