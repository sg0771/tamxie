//#pragma warning(disable : 4244)
//#pragma warning(disable : 4018)

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <stdint.h>

#define FAAC_CFG_VERSION 104

/* MPEG ID's */
#define MPEG2 1
#define MPEG4 0

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

/* Input Formats */
#define FAAC_INPUT_NULL    0
#define FAAC_INPUT_16BIT   1
#define FAAC_INPUT_24BIT   2
#define FAAC_INPUT_32BIT   3
#define FAAC_INPUT_FLOAT   4

#define SHORTCTL_NORMAL    0
#define SHORTCTL_NOSHORT   1
#define SHORTCTL_NOLONG    2


#define NFLAT_LS 448
#define MOVERLAPPED     0
#define MNON_OVERLAPPED 1
#define SINE_WINDOW 0
#define KBD_WINDOW  1

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* Memory functions */
#define AllocMemory(size) malloc(size)
#define FreeMemory(block) free(block)
#define SetMemory(block, value, size) memset(block, value, size)

#define MAX_CHANNELS 64
# define FRAME_LEN 1024
# define BLOCK_LEN_LONG 1024
# define BLOCK_LEN_SHORT 128
#define TNS_MAX_ORDER 20
#define DEF_TNS_GAIN_THRESH 1.4
#define DEF_TNS_COEFF_THRESH 0.1
#define DEF_TNS_COEFF_RES 4
#define DEF_TNS_RES_OFFSET 3
#define LEN_TNS_NFILTL 2
#define LEN_TNS_NFILTS 1

#define DELAY 2048
#define LEN_LTP_DATA_PRESENT 1
#define LEN_LTP_LAG 11
#define LEN_LTP_COEF 3
#define LEN_LTP_SHORT_USED 1
#define LEN_LTP_SHORT_LAG_PRESENT 1
#define LEN_LTP_SHORT_LAG 5
#define LTP_LAG_OFFSET 16
#define LEN_LTP_LONG_USED 1
#define MAX_LT_PRED_LONG_SFB 40
#define MAX_LT_PRED_SHORT_SFB 13
#define SHORT_SQ_OFFSET (BLOCK_LEN_LONG-(BLOCK_LEN_SHORT*4+BLOCK_LEN_SHORT/2))
#define CODESIZE 8
#define NOK_LT_BLEN (3 * BLOCK_LEN_LONG)

#define SBMAX_L 49
#define LPC 2

#define IXMAX_VAL 8191
#define PRECALC_SIZE (IXMAX_VAL+2)
#define LARGE_BITS 100000
#define SF_OFFSET 100
#define POW20(x)  pow(2.0,((double)x)*.25)
#define IPOW20(x)  pow(2.0,-((double)x)*.1875)

#define NSFB_LONG  51
#define NSFB_SHORT 15
#define MAX_SHORT_WINDOWS 8
#define MAX_SCFAC_BANDS ((NSFB_SHORT+1)*MAX_SHORT_WINDOWS)

#define FAAC_RELEASE 1
#define FAAC_VERSION "1.28"
#define LEN_SE_ID 3
#define LEN_TAG 4
#define LEN_GLOB_GAIN 8
#define LEN_COM_WIN 1
#define LEN_ICS_RESERV 1
#define LEN_WIN_SEQ 2
#define LEN_WIN_SH 1
#define LEN_MAX_SFBL 6
#define LEN_MAX_SFBS 4
#define LEN_CB 4
#define LEN_SCL_PCM 8
#define LEN_PRED_PRES 1
#define LEN_PRED_RST 1
#define LEN_PRED_RSTGRP 5
#define LEN_PRED_ENAB 1
#define LEN_MASK_PRES 2
#define LEN_MASK 1
#define LEN_PULSE_PRES 1

#define LEN_TNS_PRES 1
#define LEN_TNS_NFILTL 2
#define LEN_TNS_NFILTS 1
#define LEN_TNS_COEFF_RES 1
#define LEN_TNS_LENGTHL 6
#define LEN_TNS_LENGTHS 4
#define LEN_TNS_ORDERL 5
#define LEN_TNS_ORDERS 3
#define LEN_TNS_DIRECTION 1
#define LEN_TNS_COMPRESS 1
#define LEN_GAIN_PRES 1

#define LEN_NEC_NPULSE 2
#define LEN_NEC_ST_SFB 6
#define LEN_NEC_POFF 5
#define LEN_NEC_PAMP 4
#define NUM_NEC_LINES 4
#define NEC_OFFSET_AMP 4

#define LEN_NCC 3
#define LEN_IS_CPE 1
#define LEN_CC_LR 1
#define LEN_CC_DOM 1
#define LEN_CC_SGN 1
#define LEN_CCH_GES 2
#define LEN_CCH_CGP 1
#define LEN_D_CNT 4
#define LEN_D_ESC 12
#define LEN_F_CNT 4
#define LEN_F_ESC 8
#define LEN_BYTE 8
#define LEN_PAD_DATA 8
#define LEN_PC_COMM 8

#define ID_SCE 0
#define ID_CPE 1
#define ID_CCE 2
#define ID_LFE 3
#define ID_DSE 4
#define ID_PCE 5
#define ID_FIL 6
#define ID_END 7


/* MPEG ID's */
#define MPEG2 1
#define MPEG4 0

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

#define BYTE_NUMBIT 8       /* bits in byte (char) */
#define LONG_NUMBIT 32      /* bits in uint32_t */
#define bit2byte(a) (((a)+BYTE_NUMBIT-1)/BYTE_NUMBIT)

#define PRED_ALPHA  0.90625
#define PRED_A      0.953125
#define PRED_B      0.953125

#define ALPHA PRED_ALPHA
#define A PRED_A
#define B PRED_B
#define MINVAR 1.e-10

/* Reset every RESET_FRAME frames. */
#define RESET_FRAME 8

/* Huffman tables */
#define MAXINDEX 289
#define NUMINTAB 2
#define FIRSTINTAB 0
#define LASTINTAB 1

#define INTENSITY_HCB 15
#define INTENSITY_HCB2 14

#define ABS(A) ((A) < 0 ? (-A) : (A))

namespace LibFaac {

	typedef struct psymodellist_t psymodellist_t;

	typedef struct faacEncConfiguration
	{
		/* config version */
		int version;

		/* library version */
		char *name;

		/* copyright string */
		char *copyright;

		/* MPEG version, 2 or 4 */
		uint32_t mpegVersion;

		/* AAC object type */
		uint32_t aacObjectType;

		/* Allow mid/side coding */
		uint32_t allowMidside;

		/* Use one of the channels as LFE channel */
		uint32_t useLfe;

		/* Use Temporal Noise Shaping */
		uint32_t useTns;

		/* bitrate / channel of AAC file */
		uint32_t bitRate;

		/* AAC file frequency bandwidth */
		uint32_t bandWidth;

		/* Quantizer quality */
		uint32_t quantqual;

		/* Bitstream output format (0 = Raw; 1 = ADTS) */
		uint32_t outputFormat;

		/* psychoacoustic model list */
		psymodellist_t *psymodellist;

		/* selected index in psymodellist */
		uint32_t psymodelidx;

		/*
		PCM Sample Input Format
		0	FAAC_INPUT_NULL			invalid, signifies a misconfigured config
		1	FAAC_INPUT_16BIT		native endian 16bit
		2	FAAC_INPUT_24BIT		native endian 24bit in 24 bits		(not implemented)
		3	FAAC_INPUT_32BIT		native endian 24bit in 32 bits		(DEFAULT)
		4	FAAC_INPUT_FLOAT		32bit floating point
		*/
		uint32_t inputFormat;

		/* block type enforcing (SHORTCTL_NORMAL/SHORTCTL_NOSHORT/SHORTCTL_NOLONG) */
		int shortctl;

		/*
		Channel Remapping

		Default			0, 1, 2, 3 ... 63  (64 is MAX_CHANNELS in coder.h)

		WAVE 4.0		2, 0, 1, 3
		WAVE 5.0		2, 0, 1, 3, 4
		WAVE 5.1		2, 0, 1, 4, 5, 3
		AIFF 5.1		2, 0, 3, 1, 4, 5
		*/
		int channel_map[64];

	} faacEncConfiguration, *faacEncConfigurationPtr;

	typedef struct
	{
		uint8_t *data;      /* data bits */
		long numBit;          /* number of bits in buffer */
		long size;            /* buffer size in bytes */
		long currentBit;      /* current bit position in bit stream */
		long numByte;         /* number of bytes read/written (only file) */
	} BitStream;

	typedef struct {
		float **costbl;
		float **negsintbl;
		uint16_t **reordertbl;
	} FFT_Tables;

	enum WINDOW_TYPE {
		ONLY_LONG_WINDOW,
		LONG_SHORT_WINDOW,
		ONLY_SHORT_WINDOW,
		SHORT_LONG_WINDOW
	};

	typedef struct {
		int order;                           /* Filter order */
		int direction;                       /* Filtering direction */
		int coefCompress;                    /* Are coeffs compressed? */
		int length;                          /* Length, in bands */
		double aCoeffs[TNS_MAX_ORDER + 1];     /* AR Coefficients */
		double kCoeffs[TNS_MAX_ORDER + 1];     /* Reflection Coefficients */
		int index[TNS_MAX_ORDER + 1];          /* Coefficient indices */
	} TnsFilterData;

	typedef struct {
		int numFilters;                             /* Number of filters */
		int coefResolution;                         /* Coefficient resolution */
		TnsFilterData tnsFilter[1 << LEN_TNS_NFILTL]; /* TNS filters */
	} TnsWindowData;

	typedef struct {
		int tnsDataPresent;
		int tnsMinBandNumberLong;
		int tnsMinBandNumberShort;
		int tnsMaxBandsLong;
		int tnsMaxBandsShort;
		int tnsMaxOrderLong;
		int tnsMaxOrderShort;
		TnsWindowData windowData[MAX_SHORT_WINDOWS]; /* TNS data per window */
	} TnsInfo;

	typedef struct
	{
		int weight_idx;
		double weight;
		int sbk_prediction_used[MAX_SHORT_WINDOWS];
		int sfb_prediction_used[MAX_SCFAC_BANDS];
		int delay[MAX_SHORT_WINDOWS];
		int global_pred_flag;
		int side_info;
		double *buffer;
		double *mdct_predicted;

		double *time_buffer;
		double *ltp_overlap_buffer;
	} LtpInfo;

	typedef struct
	{
		int psy_init_mc;
		double dr_mc[LPC][BLOCK_LEN_LONG], e_mc[LPC + 1 + 1][BLOCK_LEN_LONG];
		double K_mc[LPC + 1][BLOCK_LEN_LONG], R_mc[LPC + 1][BLOCK_LEN_LONG];
		double VAR_mc[LPC + 1][BLOCK_LEN_LONG], KOR_mc[LPC + 1][BLOCK_LEN_LONG];
		double sb_samples_pred_mc[BLOCK_LEN_LONG];
		int thisLineNeedsResetting_mc[BLOCK_LEN_LONG];
		int reset_count_mc;
	} BwpInfo;

	typedef struct {
		int window_shape;
		int prev_window_shape;
		int block_type;
		int desired_block_type;

		int global_gain;
		int scale_factor[MAX_SCFAC_BANDS];

		int num_window_groups;
		int window_group_length[8];
		int max_sfb;
		int nr_of_sfb;
		int sfb_offset[250];
		int lastx;
		double avgenrg;

		int spectral_count;

		/* Huffman codebook selected for each sf band */
		int book_vector[MAX_SCFAC_BANDS];

		/* Data of spectral bitstream elements, for each spectral pair,
		5 elements are required: 1*(esc)+2*(sign)+2*(esc value)=5 */
		int *data;

		/* Lengths of spectral bitstream elements */
		int *len;

		/* Holds the requantized spectrum */
		double *requantFreq;

		TnsInfo tnsInfo;
		LtpInfo ltpInfo;
		BwpInfo bwpInfo;

		int max_pred_sfb;
		int pred_global_flag;
		int pred_sfb_flag[MAX_SCFAC_BANDS];
		int reset_group_number;

	} CoderInfo;

	typedef struct {
		uint32_t sampling_rate;  /* the following entries are for this sampling rate */
		int num_cb_long;
		int num_cb_short;
		int cb_width_long[NSFB_LONG];
		int cb_width_short[NSFB_SHORT];
	} SR_INFO;

	typedef struct {
		int is_present;
		int ms_used[MAX_SCFAC_BANDS];
	} MSInfo;

	typedef struct {
		int tag;
		int present;
		int ch_is_left;
		int paired_ch;
		int common_window;
		int cpe;
		int sce;
		int lfe;
		MSInfo msInfo;
	} ChannelInfo;

	typedef struct {
		int size;
		int sizeS;

		/* Previous input samples */
		double *prevSamples;
		double *prevSamplesS;

		int block_type;

		void *data;
	} PsyInfo;

	typedef struct {
		double sampleRate;

		/* Hann window */
		double *hannWindow;
		double *hannWindowS;

		void *data;
	} GlobalPsyInfo;

	typedef struct
	{
		void(*PsyInit) (GlobalPsyInfo *gpsyInfo, PsyInfo *psyInfo,
			uint32_t numChannels, uint32_t sampleRate,
			int *cb_width_long, int num_cb_long,
			int *cb_width_short, int num_cb_short);
		void(*PsyEnd) (GlobalPsyInfo *gpsyInfo, PsyInfo *psyInfo,
			uint32_t numChannels);
		void(*PsyCalculate) (ChannelInfo *channelInfo, GlobalPsyInfo *gpsyInfo,
			PsyInfo *psyInfo, int *cb_width_long, int num_cb_long,
			int *cb_width_short, int num_cb_short,
			uint32_t numChannels);
		void(*PsyBufferUpdate) (FFT_Tables *fft_tables, GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo,
			double *newSamples, uint32_t bandwidth,
			int *cb_width_short, int num_cb_short);
		void(*BlockSwitch) (CoderInfo *coderInfo, PsyInfo *psyInfo,
			uint32_t numChannels);
	} psymodel_t;

	extern psymodel_t psymodel2;

	typedef struct {
		double *pow43;
		double *adj43;
		double quality;
	} AACQuantCfg;

	struct psymodellist_t {
		psymodel_t *model;
		char *name;
	};


	typedef struct _faacEncHandle_t {
		/* number of channels in AAC file */
		uint32_t numChannels;

		/* samplerate of AAC file */
		uint32_t sampleRate;
		uint32_t sampleRateIdx;

		uint32_t usedBytes;

		/* frame number */
		uint32_t frameNum;
		uint32_t flushFrame;

		/* Scalefactorband data */
		SR_INFO *srInfo;

		/* sample buffers of current next and next next frame*/
		double *sampleBuff[MAX_CHANNELS];
		double *nextSampleBuff[MAX_CHANNELS];
		double *next2SampleBuff[MAX_CHANNELS];
		double *next3SampleBuff[MAX_CHANNELS];
		double *ltpTimeBuff[MAX_CHANNELS];

		/* Filterbank buffers */
		double *sin_window_long;
		double *sin_window_short;
		double *kbd_window_long;
		double *kbd_window_short;
		double *freqBuff[MAX_CHANNELS];
		double *overlapBuff[MAX_CHANNELS];

		double *msSpectrum[MAX_CHANNELS];

		/* Channel and Coder data for all channels */
		CoderInfo coderInfo[MAX_CHANNELS];
		ChannelInfo channelInfo[MAX_CHANNELS];

		/* Psychoacoustics data */
		PsyInfo psyInfo[MAX_CHANNELS];
		GlobalPsyInfo gpsyInfo;

		/* Configuration data */
		faacEncConfiguration config;

		psymodel_t *psymodel;

		/* quantizer specific config */
		AACQuantCfg aacquantCfg;

		/* FFT Tables */
		FFT_Tables	fft_tables;

		/* output bits difference in average bitrate mode */
		int bitDiff;
	} faacEncStruct, *faacEncHandle;

	int  faacEncGetVersion(char **faac_id_string,
		char **faac_copyright_string);

	faacEncConfigurationPtr
		faacEncGetCurrentConfiguration(faacEncHandle hEncoder);


	int  faacEncSetConfiguration(faacEncHandle hEncoder,
		faacEncConfigurationPtr config);


	faacEncHandle  faacEncOpen(uint32_t sampleRate,
		uint32_t numChannels,
		uint32_t *inputSamples,
		uint32_t *maxOutputBytes);


	int  faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder, uint8_t **ppBuffer,
		uint32_t *pSizeOfDecoderSpecificInfo);


	int  faacEncEncode(faacEncHandle hEncoder, int32_t * inputBuffer, uint32_t samplesInput,
		uint8_t *outputBuffer,
		uint32_t bufferSize);

	int  faacEncClose(faacEncHandle hEncoder);

	void GetChannelInfo(ChannelInfo *channelInfo, int numChannels, int useLfe);
	void TnsInit(faacEncHandle hEncoder);
	void TnsEncode(TnsInfo* tnsInfo, int numberOfBands, int maxSfb, enum WINDOW_TYPE blockType,
		int* sfbOffsetTable, double* spec);
	void TnsEncodeFilterOnly(TnsInfo* tnsInfo, int numberOfBands, int maxSfb,
		enum WINDOW_TYPE blockType, int *sfbOffsetTable, double *spec);
	void TnsDecodeFilterOnly(TnsInfo* tnsInfo, int numberOfBands, int maxSfb,
		enum WINDOW_TYPE blockType, int *sfbOffsetTable, double *spec);

	void AACQuantizeInit(CoderInfo *coderInfo, uint32_t numChannels,
		AACQuantCfg *aacquantCfg);
	void AACQuantizeEnd(CoderInfo *coderInfo, uint32_t numChannels,
		AACQuantCfg *aacquantCfg);

	int AACQuantize(CoderInfo *coderInfo,
		PsyInfo *psyInfo,
		ChannelInfo *channelInfo,
		int *cb_width,
		int num_cb,
		double *xr,
		AACQuantCfg *aacquantcfg);

	int SortForGrouping(CoderInfo* coderInfo,
		PsyInfo *psyInfo,
		ChannelInfo *channelInfo,
		int *sfb_width_table,
		double *xr);
	void CalcAvgEnrg(CoderInfo *coderInfo,
		const double *xr);

	void fft_initialize(FFT_Tables *fft_tables);
	void fft_terminate(FFT_Tables *fft_tables);
	void rfft(FFT_Tables *fft_tables, double *x, int logm);
	void fft(FFT_Tables *fft_tables, double *xr, double *xi, int logm);
	void ffti(FFT_Tables *fft_tables, double *xr, double *xi, int logm);

	int GetSRIndex(uint32_t sampleRate);
	int GetMaxPredSfb(int samplingRateIdx);
	uint32_t MaxBitrate(uint32_t sampleRate);
	uint32_t MinBitrate();
	uint32_t MaxBitresSize(uint32_t bitRate, uint32_t sampleRate);
	uint32_t BitAllocation(double pe, int short_block);

	int  faacEncGetVersion(char **faac_id_string,
		char **faac_copyright_string);

	int  faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder,
		uint8_t** ppBuffer,
		uint32_t* pSizeOfDecoderSpecificInfo);

	faacEncConfigurationPtr  faacEncGetCurrentConfiguration(faacEncHandle hEncoder);
	int  faacEncSetConfiguration(faacEncHandle hEncoder, faacEncConfigurationPtr config);

	faacEncHandle  faacEncOpen(uint32_t sampleRate,
		uint32_t numChannels,
		uint32_t *inputSamples,
		uint32_t *maxOutputBytes);

	int  faacEncEncode(faacEncHandle hEncoder,
		int *inputBuffer,
		uint32_t samplesInput,
		uint8_t *outputBuffer,
		uint32_t bufferSize
	);

	int  faacEncClose(faacEncHandle hEncoder);

	int WriteBitstream(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int numChannels);

	BitStream *OpenBitStream(int size, uint8_t *buffer);

	int CloseBitStream(BitStream *bitStream);

	int PutBit(BitStream *bitStream,
		uint32_t data,
		int numBit);

	void PredCalcPrediction(double *act_spec,
		double *last_spec,
		int btype,
		int nsfb,
		int *isfb_width,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		int chanNum);

	void PredInit(faacEncHandle hEncoder);

	void CopyPredInfo(CoderInfo *right, CoderInfo *left);

	void			FilterBankInit(faacEncHandle hEncoder);

	void			FilterBankEnd(faacEncHandle hEncoder);

	void			FilterBank(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		double *p_in_data,
		double *p_out_mdct,
		double *p_overlap,
		int overlap_select);

	void			IFilterBank(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		double *p_in_data,
		double *p_out_mdct,
		double *p_overlap,
		int overlap_select);

	void specFilter(double *freqBuff,
		int sampleRate,
		int lowpassFreq,
		int specLen);
	void HuffmanInit(CoderInfo *coderInfo, uint32_t numChannels);
	void HuffmanEnd(CoderInfo *coderInfo, uint32_t numChannels);

	int BitSearch(CoderInfo *coderInfo,
		int *quant);

	int NoiselessBitCount(CoderInfo *coderInfo,
		int *quant,
		int hop,
		int min_book_choice[112][3]);

	static int CalculateEscSequence(int input, int *len_esc_sequence);

	int CalcBits(CoderInfo *coderInfo,
		int book,
		int *quant,
		int offset,
		int length);

	int OutputBits(CoderInfo *coderInfo,
		int book,
		int *quant,
		int offset,
		int length);

	int SortBookNumbers(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);

	int WriteScalefactors(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);





	uint16_t huff1[][2] = {
		{ 11,  2040 },
		{ 9,  497 },{ 11,  2045 },{ 10,  1013 },{ 7,  104 },{ 10,  1008 },
		{ 11,  2039 },{ 9,  492 },{ 11,  2037 },{ 10,  1009 },{ 7,  114 },
		{ 10,  1012 },{ 7,  116 },{ 5,  17 },{ 7,  118 },{ 9,  491 },
		{ 7,  108 },{ 10,  1014 },{ 11,  2044 },{ 9,  481 },{ 11,  2033 },
		{ 9,  496 },{ 7,  97 },{ 9,  502 },{ 11,  2034 },{ 9,  490 },
		{ 11,  2043 },{ 9,  498 },{ 7,  105 },{ 9,  493 },{ 7,  119 },
		{ 5,  23 },{ 7,  111 },{ 9,  486 },{ 7,  100 },{ 9,  485 },
		{ 7,  103 },{ 5,  21 },{ 7,  98 },{ 5,  18 },{ 1,  0 },
		{ 5,  20 },{ 7,  101 },{ 5,  22 },{ 7,  109 },{ 9,  489 },
		{ 7,  99 },{ 9,  484 },{ 7,  107 },{ 5,  19 },{ 7,  113 },
		{ 9,  483 },{ 7,  112 },{ 9,  499 },{ 11,  2046 },{ 9,  487 },
		{ 11,  2035 },{ 9,  495 },{ 7,  96 },{ 9,  494 },{ 11,  2032 },
		{ 9,  482 },{ 11,  2042 },{ 10,  1011 },{ 7,  106 },{ 9,  488 },
		{ 7,  117 },{ 5,  16 },{ 7,  115 },{ 9,  500 },{ 7,  110 },
		{ 10,  1015 },{ 11,  2038 },{ 9,  480 },{ 11,  2041 },{ 10,  1010 },
		{ 7,  102 },{ 9,  501 },{ 11,  2047 },{ 9,  503 },{ 11,  2036 }
	};
	uint16_t huff2[][2] = {
		{ 9,  499 },
		{ 7,  111 },{ 9,  509 },{ 8,  235 },{ 6,  35 },{ 8,  234 },
		{ 9,  503 },{ 8,  232 },{ 9,  506 },{ 8,  242 },{ 6,  45 },
		{ 7,  112 },{ 6,  32 },{ 5,  6 },{ 6,  43 },{ 7,  110 },
		{ 6,  40 },{ 8,  233 },{ 9,  505 },{ 7,  102 },{ 8,  248 },
		{ 8,  231 },{ 6,  27 },{ 8,  241 },{ 9,  500 },{ 7,  107 },
		{ 9,  501 },{ 8,  236 },{ 6,  42 },{ 7,  108 },{ 6,  44 },
		{ 5,  10 },{ 6,  39 },{ 7,  103 },{ 6,  26 },{ 8,  245 },
		{ 6,  36 },{ 5,  8 },{ 6,  31 },{ 5,  9 },{ 3,  0 },
		{ 5,  7 },{ 6,  29 },{ 5,  11 },{ 6,  48 },{ 8,  239 },
		{ 6,  28 },{ 7,  100 },{ 6,  30 },{ 5,  12 },{ 6,  41 },
		{ 8,  243 },{ 6,  47 },{ 8,  240 },{ 9,  508 },{ 7,  113 },
		{ 9,  498 },{ 8,  244 },{ 6,  33 },{ 8,  230 },{ 8,  247 },
		{ 7,  104 },{ 9,  504 },{ 8,  238 },{ 6,  34 },{ 7,  101 },
		{ 6,  49 },{ 4,  2 },{ 6,  38 },{ 8,  237 },{ 6,  37 },
		{ 7,  106 },{ 9,  507 },{ 7,  114 },{ 9,  510 },{ 7,  105 },
		{ 6,  46 },{ 8,  246 },{ 9,  511 },{ 7,  109 },{ 9,  502 }
	};
	uint16_t huff3[][2] = {
		{ 1,  0 },
		{ 4,  9 },{ 8,  239 },{ 4,  11 },{ 5,  25 },{ 8,  240 },
		{ 9,  491 },{ 9,  486 },{ 10,  1010 },{ 4,  10 },{ 6,  53 },
		{ 9,  495 },{ 6,  52 },{ 6,  55 },{ 9,  489 },{ 9,  493 },
		{ 9,  487 },{ 10,  1011 },{ 9,  494 },{ 10,  1005 },{ 13,  8186 },
		{ 9,  492 },{ 9,  498 },{ 11,  2041 },{ 11,  2040 },{ 10,  1016 },
		{ 12,  4088 },{ 4,  8 },{ 6,  56 },{ 10,  1014 },{ 6,  54 },
		{ 7,  117 },{ 10,  1009 },{ 10,  1003 },{ 10,  1004 },{ 12,  4084 },
		{ 5,  24 },{ 7,  118 },{ 11,  2036 },{ 6,  57 },{ 7,  116 },
		{ 10,  1007 },{ 9,  499 },{ 9,  500 },{ 11,  2038 },{ 9,  488 },
		{ 10,  1002 },{ 13,  8188 },{ 8,  242 },{ 9,  497 },{ 12,  4091 },
		{ 10,  1013 },{ 11,  2035 },{ 12,  4092 },{ 8,  238 },{ 10,  1015 },
		{ 15,  32766 },{ 9,  496 },{ 11,  2037 },{ 15,  32765 },{ 13,  8187 },
		{ 14,  16378 },{ 16,  65535 },{ 8,  241 },{ 10,  1008 },{ 14,  16380 },
		{ 9,  490 },{ 10,  1006 },{ 14,  16379 },{ 12,  4086 },{ 12,  4090 },
		{ 15,  32764 },{ 11,  2034 },{ 12,  4085 },{ 16,  65534 },{ 10,  1012 },
		{ 11,  2039 },{ 15,  32763 },{ 12,  4087 },{ 12,  4089 },{ 15,  32762 }
	};
	uint16_t huff4[][2] = {
		{ 4,  7 },
		{ 5,  22 },{ 8,  246 },{ 5,  24 },{ 4,  8 },{ 8,  239 },
		{ 9,  495 },{ 8,  243 },{ 11,  2040 },{ 5,  25 },{ 5,  23 },
		{ 8,  237 },{ 5,  21 },{ 4,  1 },{ 8,  226 },{ 8,  240 },
		{ 7,  112 },{ 10,  1008 },{ 9,  494 },{ 8,  241 },{ 11,  2042 },
		{ 8,  238 },{ 8,  228 },{ 10,  1010 },{ 11,  2038 },{ 10,  1007 },
		{ 11,  2045 },{ 4,  5 },{ 5,  20 },{ 8,  242 },{ 4,  9 },
		{ 4,  4 },{ 8,  229 },{ 8,  244 },{ 8,  232 },{ 10,  1012 },
		{ 4,  6 },{ 4,  2 },{ 8,  231 },{ 4,  3 },{ 4,  0 },
		{ 7,  107 },{ 8,  227 },{ 7,  105 },{ 9,  499 },{ 8,  235 },
		{ 8,  230 },{ 10,  1014 },{ 7,  110 },{ 7,  106 },{ 9,  500 },
		{ 10,  1004 },{ 9,  496 },{ 10,  1017 },{ 8,  245 },{ 8,  236 },
		{ 11,  2043 },{ 8,  234 },{ 7,  111 },{ 10,  1015 },{ 11,  2041 },
		{ 10,  1011 },{ 12,  4095 },{ 8,  233 },{ 7,  109 },{ 10,  1016 },
		{ 7,  108 },{ 7,  104 },{ 9,  501 },{ 10,  1006 },{ 9,  498 },
		{ 11,  2036 },{ 11,  2039 },{ 10,  1009 },{ 12,  4094 },{ 10,  1005 },
		{ 9,  497 },{ 11,  2037 },{ 11,  2046 },{ 10,  1013 },{ 11,  2044 }
	};
	uint16_t huff5[][2] = {
		{ 13,  8191 },
		{ 12,  4087 },{ 11,  2036 },{ 11,  2024 },{ 10,  1009 },{ 11,  2030 },
		{ 11,  2041 },{ 12,  4088 },{ 13,  8189 },{ 12,  4093 },{ 11,  2033 },
		{ 10,  1000 },{ 9,  488 },{ 8,  240 },{ 9,  492 },{ 10,  1006 },
		{ 11,  2034 },{ 12,  4090 },{ 12,  4084 },{ 10,  1007 },{ 9,  498 },
		{ 8,  232 },{ 7,  112 },{ 8,  236 },{ 9,  496 },{ 10,  1002 },
		{ 11,  2035 },{ 11,  2027 },{ 9,  491 },{ 8,  234 },{ 5,  26 },
		{ 4,  8 },{ 5,  25 },{ 8,  238 },{ 9,  495 },{ 11,  2029 },
		{ 10,  1008 },{ 8,  242 },{ 7,  115 },{ 4,  11 },{ 1,  0 },
		{ 4,  10 },{ 7,  113 },{ 8,  243 },{ 11,  2025 },{ 11,  2031 },
		{ 9,  494 },{ 8,  239 },{ 5,  24 },{ 4,  9 },{ 5,  27 },
		{ 8,  235 },{ 9,  489 },{ 11,  2028 },{ 11,  2038 },{ 10,  1003 },
		{ 9,  499 },{ 8,  237 },{ 7,  114 },{ 8,  233 },{ 9,  497 },
		{ 10,  1005 },{ 11,  2039 },{ 12,  4086 },{ 11,  2032 },{ 10,  1001 },
		{ 9,  493 },{ 8,  241 },{ 9,  490 },{ 10,  1004 },{ 11,  2040 },
		{ 12,  4089 },{ 13,  8188 },{ 12,  4092 },{ 12,  4085 },{ 11,  2026 },
		{ 10,  1011 },{ 10,  1010 },{ 11,  2037 },{ 12,  4091 },{ 13,  8190 }
	};
	uint16_t huff6[][2] = {
		{ 11,  2046 },
		{ 10,  1021 },{ 9,  497 },{ 9,  491 },{ 9,  500 },{ 9,  490 },
		{ 9,  496 },{ 10,  1020 },{ 11,  2045 },{ 10,  1014 },{ 9,  485 },
		{ 8,  234 },{ 7,  108 },{ 7,  113 },{ 7,  104 },{ 8,  240 },
		{ 9,  486 },{ 10,  1015 },{ 9,  499 },{ 8,  239 },{ 6,  50 },
		{ 6,  39 },{ 6,  40 },{ 6,  38 },{ 6,  49 },{ 8,  235 },
		{ 9,  503 },{ 9,  488 },{ 7,  111 },{ 6,  46 },{ 4,  8 },
		{ 4,  4 },{ 4,  6 },{ 6,  41 },{ 7,  107 },{ 9,  494 },
		{ 9,  495 },{ 7,  114 },{ 6,  45 },{ 4,  2 },{ 4,  0 },
		{ 4,  3 },{ 6,  47 },{ 7,  115 },{ 9,  506 },{ 9,  487 },
		{ 7,  110 },{ 6,  43 },{ 4,  7 },{ 4,  1 },{ 4,  5 },
		{ 6,  44 },{ 7,  109 },{ 9,  492 },{ 9,  505 },{ 8,  238 },
		{ 6,  48 },{ 6,  36 },{ 6,  42 },{ 6,  37 },{ 6,  51 },
		{ 8,  236 },{ 9,  498 },{ 10,  1016 },{ 9,  484 },{ 8,  237 },
		{ 7,  106 },{ 7,  112 },{ 7,  105 },{ 7,  116 },{ 8,  241 },
		{ 10,  1018 },{ 11,  2047 },{ 10,  1017 },{ 9,  502 },{ 9,  493 },
		{ 9,  504 },{ 9,  489 },{ 9,  501 },{ 10,  1019 },{ 11,  2044 }
	};
	uint16_t huff7[][2] = {
		{ 1,  0 },
		{ 3,  5 },{ 6,  55 },{ 7,  116 },{ 8,  242 },{ 9,  491 },
		{ 10,  1005 },{ 11,  2039 },{ 3,  4 },{ 4,  12 },{ 6,  53 },
		{ 7,  113 },{ 8,  236 },{ 8,  238 },{ 9,  494 },{ 9,  501 },
		{ 6,  54 },{ 6,  52 },{ 7,  114 },{ 8,  234 },{ 8,  241 },
		{ 9,  489 },{ 9,  499 },{ 10,  1013 },{ 7,  115 },{ 7,  112 },
		{ 8,  235 },{ 8,  240 },{ 9,  497 },{ 9,  496 },{ 10,  1004 },
		{ 10,  1018 },{ 8,  243 },{ 8,  237 },{ 9,  488 },{ 9,  495 },
		{ 10,  1007 },{ 10,  1009 },{ 10,  1017 },{ 11,  2043 },{ 9,  493 },
		{ 8,  239 },{ 9,  490 },{ 9,  498 },{ 10,  1011 },{ 10,  1016 },
		{ 11,  2041 },{ 11,  2044 },{ 10,  1006 },{ 9,  492 },{ 9,  500 },
		{ 10,  1012 },{ 10,  1015 },{ 11,  2040 },{ 12,  4093 },{ 12,  4094 },
		{ 11,  2038 },{ 10,  1008 },{ 10,  1010 },{ 10,  1014 },{ 11,  2042 },
		{ 11,  2045 },{ 12,  4092 },{ 12,  4095 }
	};
	uint16_t huff8[][2] = {
		{ 5,  14 },
		{ 4,  5 },{ 5,  16 },{ 6,  48 },{ 7,  111 },{ 8,  241 },
		{ 9,  506 },{ 10,  1022 },{ 4,  3 },{ 3,  0 },{ 4,  4 },
		{ 5,  18 },{ 6,  44 },{ 7,  106 },{ 7,  117 },{ 8,  248 },
		{ 5,  15 },{ 4,  2 },{ 4,  6 },{ 5,  20 },{ 6,  46 },
		{ 7,  105 },{ 7,  114 },{ 8,  245 },{ 6,  47 },{ 5,  17 },
		{ 5,  19 },{ 6,  42 },{ 6,  50 },{ 7,  108 },{ 8,  236 },
		{ 8,  250 },{ 7,  113 },{ 6,  43 },{ 6,  45 },{ 6,  49 },
		{ 7,  109 },{ 7,  112 },{ 8,  242 },{ 9,  505 },{ 8,  239 },
		{ 7,  104 },{ 6,  51 },{ 7,  107 },{ 7,  110 },{ 8,  238 },
		{ 8,  249 },{ 10,  1020 },{ 9,  504 },{ 7,  116 },{ 7,  115 },
		{ 8,  237 },{ 8,  240 },{ 8,  246 },{ 9,  502 },{ 9,  509 },
		{ 10,  1021 },{ 8,  243 },{ 8,  244 },{ 8,  247 },{ 9,  503 },
		{ 9,  507 },{ 9,  508 },{ 10,  1023 }
	};
	uint16_t huff9[][2] = {
		{ 1,  0 },
		{ 3,  5 },{ 6,  55 },{ 8,  231 },{ 9,  478 },{ 10,  974 },
		{ 10,  985 },{ 11,  1992 },{ 11,  1997 },{ 12,  4040 },{ 12,  4061 },
		{ 13,  8164 },{ 13,  8172 },{ 3,  4 },{ 4,  12 },{ 6,  53 },
		{ 7,  114 },{ 8,  234 },{ 8,  237 },{ 9,  482 },{ 10,  977 },
		{ 10,  979 },{ 10,  992 },{ 11,  2008 },{ 12,  4047 },{ 12,  4053 },
		{ 6,  54 },{ 6,  52 },{ 7,  113 },{ 8,  232 },{ 8,  236 },
		{ 9,  481 },{ 10,  975 },{ 10,  989 },{ 10,  987 },{ 11,  2000 },
		{ 12,  4039 },{ 12,  4052 },{ 12,  4068 },{ 8,  230 },{ 7,  112 },
		{ 8,  233 },{ 9,  477 },{ 9,  483 },{ 10,  978 },{ 10,  988 },
		{ 11,  1996 },{ 11,  1994 },{ 11,  2014 },{ 12,  4056 },{ 12,  4074 },
		{ 13,  8155 },{ 9,  479 },{ 8,  235 },{ 9,  476 },{ 9,  486 },
		{ 10,  981 },{ 10,  990 },{ 11,  1995 },{ 11,  2013 },{ 11,  2012 },
		{ 12,  4045 },{ 12,  4066 },{ 12,  4071 },{ 13,  8161 },{ 10,  976 },
		{ 9,  480 },{ 9,  484 },{ 10,  982 },{ 11,  1989 },{ 11,  2001 },
		{ 11,  2011 },{ 12,  4050 },{ 11,  2016 },{ 12,  4057 },{ 12,  4075 },
		{ 13,  8163 },{ 13,  8169 },{ 11,  1988 },{ 9,  485 },{ 10,  983 },
		{ 11,  1990 },{ 11,  1999 },{ 11,  2010 },{ 12,  4043 },{ 12,  4058 },
		{ 12,  4067 },{ 12,  4073 },{ 13,  8166 },{ 13,  8179 },{ 13,  8183 },
		{ 11,  2003 },{ 10,  984 },{ 10,  993 },{ 11,  2004 },{ 11,  2009 },
		{ 12,  4051 },{ 12,  4062 },{ 13,  8157 },{ 13,  8153 },{ 13,  8162 },
		{ 13,  8170 },{ 13,  8177 },{ 13,  8182 },{ 11,  2002 },{ 10,  980 },
		{ 10,  986 },{ 11,  1991 },{ 11,  2007 },{ 11,  2018 },{ 12,  4046 },
		{ 12,  4059 },{ 13,  8152 },{ 13,  8174 },{ 14,  16368 },{ 13,  8180 },
		{ 14,  16370 },{ 11,  2017 },{ 10,  991 },{ 11,  1993 },{ 11,  2006 },
		{ 12,  4042 },{ 12,  4048 },{ 12,  4069 },{ 12,  4070 },{ 13,  8171 },
		{ 13,  8175 },{ 14,  16371 },{ 14,  16372 },{ 14,  16373 },{ 12,  4064 },
		{ 11,  1998 },{ 11,  2005 },{ 12,  4038 },{ 12,  4049 },{ 12,  4065 },
		{ 13,  8160 },{ 13,  8168 },{ 13,  8176 },{ 14,  16369 },{ 14,  16376 },
		{ 14,  16374 },{ 15,  32764 },{ 12,  4072 },{ 11,  2015 },{ 12,  4041 },
		{ 12,  4055 },{ 12,  4060 },{ 13,  8156 },{ 13,  8159 },{ 13,  8173 },
		{ 13,  8181 },{ 14,  16377 },{ 14,  16379 },{ 15,  32765 },{ 15,  32766 },
		{ 13,  8167 },{ 12,  4044 },{ 12,  4054 },{ 12,  4063 },{ 13,  8158 },
		{ 13,  8154 },{ 13,  8165 },{ 13,  8178 },{ 14,  16378 },{ 14,  16375 },
		{ 14,  16380 },{ 14,  16381 },{ 15,  32767 }
	};
	uint16_t huff10[][2] = {
		{ 6,  34 },
		{ 5,  8 },{ 6,  29 },{ 6,  38 },{ 7,  95 },{ 8,  211 },
		{ 9,  463 },{ 10,  976 },{ 10,  983 },{ 10,  1005 },{ 11,  2032 },
		{ 11,  2038 },{ 12,  4093 },{ 5,  7 },{ 4,  0 },{ 4,  1 },
		{ 5,  9 },{ 6,  32 },{ 7,  84 },{ 7,  96 },{ 8,  213 },
		{ 8,  220 },{ 9,  468 },{ 10,  973 },{ 10,  990 },{ 11,  2023 },
		{ 6,  28 },{ 4,  2 },{ 5,  6 },{ 5,  12 },{ 6,  30 },
		{ 6,  40 },{ 7,  91 },{ 8,  205 },{ 8,  217 },{ 9,  462 },
		{ 9,  476 },{ 10,  985 },{ 10,  1009 },{ 6,  37 },{ 5,  11 },
		{ 5,  10 },{ 5,  13 },{ 6,  36 },{ 7,  87 },{ 7,  97 },
		{ 8,  204 },{ 8,  221 },{ 9,  460 },{ 9,  478 },{ 10,  979 },
		{ 10,  999 },{ 7,  93 },{ 6,  33 },{ 6,  31 },{ 6,  35 },
		{ 6,  39 },{ 7,  89 },{ 7,  100 },{ 8,  216 },{ 8,  223 },
		{ 9,  466 },{ 9,  482 },{ 10,  989 },{ 10,  1006 },{ 8,  209 },
		{ 7,  85 },{ 6,  41 },{ 7,  86 },{ 7,  88 },{ 7,  98 },
		{ 8,  206 },{ 8,  224 },{ 8,  226 },{ 9,  474 },{ 10,  980 },
		{ 10,  995 },{ 11,  2027 },{ 9,  457 },{ 7,  94 },{ 7,  90 },
		{ 7,  92 },{ 7,  99 },{ 8,  202 },{ 8,  218 },{ 9,  455 },
		{ 9,  458 },{ 9,  480 },{ 10,  987 },{ 10,  1000 },{ 11,  2028 },
		{ 9,  483 },{ 8,  210 },{ 8,  203 },{ 8,  208 },{ 8,  215 },
		{ 8,  219 },{ 9,  454 },{ 9,  469 },{ 9,  472 },{ 10,  970 },
		{ 10,  986 },{ 11,  2026 },{ 11,  2033 },{ 9,  481 },{ 8,  212 },
		{ 8,  207 },{ 8,  214 },{ 8,  222 },{ 8,  225 },{ 9,  464 },
		{ 9,  470 },{ 10,  977 },{ 10,  981 },{ 10,  1010 },{ 11,  2030 },
		{ 11,  2043 },{ 10,  1001 },{ 9,  461 },{ 9,  456 },{ 9,  459 },
		{ 9,  465 },{ 9,  471 },{ 9,  479 },{ 10,  975 },{ 10,  992 },
		{ 10,  1007 },{ 11,  2022 },{ 11,  2040 },{ 12,  4090 },{ 10,  1003 },
		{ 9,  477 },{ 9,  467 },{ 9,  473 },{ 9,  475 },{ 10,  978 },
		{ 10,  972 },{ 10,  988 },{ 10,  1002 },{ 11,  2029 },{ 11,  2035 },
		{ 11,  2041 },{ 12,  4089 },{ 11,  2034 },{ 10,  974 },{ 9,  484 },
		{ 10,  971 },{ 10,  984 },{ 10,  982 },{ 10,  994 },{ 10,  997 },
		{ 11,  2024 },{ 11,  2036 },{ 11,  2037 },{ 11,  2039 },{ 12,  4091 },
		{ 11,  2042 },{ 10,  1004 },{ 10,  991 },{ 10,  993 },{ 10,  996 },
		{ 10,  998 },{ 10,  1008 },{ 11,  2025 },{ 11,  2031 },{ 12,  4088 },
		{ 12,  4094 },{ 12,  4092 },{ 12,  4095 }
	};
	uint16_t huff11[][2] = {
		{ 4,  0 },
		{ 5,  6 },{ 6,  25 },{ 7,  61 },{ 8,  156 },{ 8,  198 },
		{ 9,  423 },{ 10,  912 },{ 10,  962 },{ 10,  991 },{ 11,  2022 },
		{ 11,  2035 },{ 12,  4091 },{ 11,  2028 },{ 12,  4090 },{ 12,  4094 },
		{ 10,  910 },{ 5,  5 },{ 4,  1 },{ 5,  8 },{ 6,  20 },
		{ 7,  55 },{ 7,  66 },{ 8,  146 },{ 8,  175 },{ 9,  401 },
		{ 9,  421 },{ 9,  437 },{ 10,  926 },{ 10,  960 },{ 10,  930 },
		{ 10,  973 },{ 11,  2006 },{ 8,  174 },{ 6,  23 },{ 5,  7 },
		{ 5,  9 },{ 6,  24 },{ 7,  57 },{ 7,  64 },{ 8,  142 },
		{ 8,  163 },{ 8,  184 },{ 9,  409 },{ 9,  428 },{ 9,  449 },
		{ 10,  945 },{ 10,  918 },{ 10,  958 },{ 10,  970 },{ 8,  157 },
		{ 7,  60 },{ 6,  21 },{ 6,  22 },{ 6,  26 },{ 7,  59 },
		{ 7,  68 },{ 8,  145 },{ 8,  165 },{ 8,  190 },{ 9,  406 },
		{ 9,  430 },{ 9,  441 },{ 10,  929 },{ 10,  913 },{ 10,  933 },
		{ 10,  981 },{ 8,  148 },{ 8,  154 },{ 7,  54 },{ 7,  56 },
		{ 7,  58 },{ 7,  65 },{ 8,  140 },{ 8,  155 },{ 8,  176 },
		{ 8,  195 },{ 9,  414 },{ 9,  427 },{ 9,  444 },{ 10,  927 },
		{ 10,  911 },{ 10,  937 },{ 10,  975 },{ 8,  147 },{ 8,  191 },
		{ 7,  62 },{ 7,  63 },{ 7,  67 },{ 7,  69 },{ 8,  158 },
		{ 8,  167 },{ 8,  185 },{ 9,  404 },{ 9,  418 },{ 9,  442 },
		{ 9,  451 },{ 10,  934 },{ 10,  935 },{ 10,  955 },{ 10,  980 },
		{ 8,  159 },{ 9,  416 },{ 8,  143 },{ 8,  141 },{ 8,  144 },
		{ 8,  152 },{ 8,  166 },{ 8,  182 },{ 8,  196 },{ 9,  415 },
		{ 9,  431 },{ 9,  447 },{ 10,  921 },{ 10,  959 },{ 10,  948 },
		{ 10,  969 },{ 10,  999 },{ 8,  168 },{ 9,  438 },{ 8,  171 },
		{ 8,  164 },{ 8,  170 },{ 8,  178 },{ 8,  194 },{ 8,  197 },
		{ 9,  408 },{ 9,  420 },{ 9,  440 },{ 10,  908 },{ 10,  932 },
		{ 10,  964 },{ 10,  966 },{ 10,  989 },{ 10,  1000 },{ 8,  173 },
		{ 10,  943 },{ 9,  402 },{ 8,  189 },{ 8,  188 },{ 9,  398 },
		{ 9,  407 },{ 9,  410 },{ 9,  419 },{ 9,  433 },{ 10,  909 },
		{ 10,  920 },{ 10,  951 },{ 10,  979 },{ 10,  977 },{ 10,  987 },
		{ 11,  2013 },{ 8,  180 },{ 10,  990 },{ 9,  425 },{ 9,  411 },
		{ 9,  412 },{ 9,  417 },{ 9,  426 },{ 9,  429 },{ 9,  435 },
		{ 10,  907 },{ 10,  946 },{ 10,  952 },{ 10,  974 },{ 10,  993 },
		{ 10,  992 },{ 11,  2002 },{ 11,  2021 },{ 8,  183 },{ 11,  2019 },
		{ 9,  443 },{ 9,  424 },{ 9,  422 },{ 9,  432 },{ 9,  434 },
		{ 9,  439 },{ 10,  923 },{ 10,  922 },{ 10,  954 },{ 10,  949 },
		{ 10,  982 },{ 11,  2007 },{ 10,  996 },{ 11,  2008 },{ 11,  2026 },
		{ 8,  186 },{ 11,  2024 },{ 10,  928 },{ 9,  445 },{ 9,  436 },
		{ 10,  906 },{ 9,  452 },{ 10,  914 },{ 10,  938 },{ 10,  944 },
		{ 10,  956 },{ 10,  983 },{ 11,  2004 },{ 11,  2012 },{ 11,  2011 },
		{ 11,  2005 },{ 11,  2032 },{ 8,  193 },{ 11,  2043 },{ 10,  968 },
		{ 10,  931 },{ 10,  917 },{ 10,  925 },{ 10,  940 },{ 10,  942 },
		{ 10,  965 },{ 10,  984 },{ 10,  994 },{ 10,  998 },{ 11,  2020 },
		{ 11,  2023 },{ 11,  2016 },{ 11,  2025 },{ 11,  2039 },{ 9,  400 },
		{ 11,  2034 },{ 10,  915 },{ 9,  446 },{ 9,  448 },{ 10,  916 },
		{ 10,  919 },{ 10,  941 },{ 10,  963 },{ 10,  961 },{ 10,  978 },
		{ 11,  2010 },{ 11,  2009 },{ 11,  2015 },{ 11,  2027 },{ 11,  2036 },
		{ 11,  2042 },{ 9,  405 },{ 11,  2040 },{ 10,  957 },{ 10,  924 },
		{ 10,  939 },{ 10,  936 },{ 10,  947 },{ 10,  953 },{ 10,  976 },
		{ 10,  995 },{ 10,  997 },{ 11,  2018 },{ 11,  2014 },{ 11,  2029 },
		{ 11,  2033 },{ 11,  2041 },{ 11,  2044 },{ 9,  403 },{ 12,  4093 },
		{ 10,  988 },{ 10,  950 },{ 10,  967 },{ 10,  972 },{ 10,  971 },
		{ 10,  985 },{ 10,  986 },{ 11,  2003 },{ 11,  2017 },{ 11,  2030 },
		{ 11,  2031 },{ 11,  2037 },{ 11,  2038 },{ 12,  4092 },{ 12,  4095 },
		{ 9,  413 },{ 9,  450 },{ 8,  181 },{ 8,  161 },{ 8,  150 },
		{ 8,  151 },{ 8,  149 },{ 8,  153 },{ 8,  160 },{ 8,  162 },
		{ 8,  172 },{ 8,  169 },{ 8,  177 },{ 8,  179 },{ 8,  187 },
		{ 8,  192 },{ 9,  399 },{ 5,  4 }
	};
	uint32_t huff12[][2] = {
		{ 18,  262120 },
		{ 18,  262118 },{ 18,  262119 },{ 18,  262117 },{ 19,  524277 },{ 19,  524273 },
		{ 19,  524269 },{ 19,  524278 },{ 19,  524270 },{ 19,  524271 },{ 19,  524272 },
		{ 19,  524284 },{ 19,  524285 },{ 19,  524287 },{ 19,  524286 },{ 19,  524279 },
		{ 19,  524280 },{ 19,  524283 },{ 19,  524281 },{ 18,  262116 },{ 19,  524282 },
		{ 18,  262115 },{ 17,  131055 },{ 17,  131056 },{ 16,  65525 },{ 17,  131054 },
		{ 16,  65522 },{ 16,  65523 },{ 16,  65524 },{ 16,  65521 },{ 15,  32758 },
		{ 15,  32759 },{ 14,  16377 },{ 14,  16373 },{ 14,  16375 },{ 14,  16371 },
		{ 14,  16374 },{ 14,  16370 },{ 13,  8183 },{ 13,  8181 },{ 12,  4089 },
		{ 12,  4087 },{ 12,  4086 },{ 11,  2041 },{ 12,  4084 },{ 11,  2040 },
		{ 10,  1017 },{ 10,  1015 },{ 10,  1013 },{ 9,  504 },{ 9,  503 },
		{ 8,  250 },{ 8,  248 },{ 8,  246 },{ 7,  121 },{ 6,  58 },
		{ 6,  56 },{ 5,  26 },{ 4,  11 },{ 3,  4 },{ 1,  0 },
		{ 4,  10 },{ 4,  12 },{ 5,  27 },{ 6,  57 },{ 6,  59 },
		{ 7,  120 },{ 7,  122 },{ 8,  247 },{ 8,  249 },{ 9,  502 },
		{ 9,  505 },{ 10,  1012 },{ 10,  1014 },{ 10,  1016 },{ 11,  2037 },
		{ 11,  2036 },{ 11,  2038 },{ 11,  2039 },{ 12,  4085 },{ 12,  4088 },
		{ 13,  8180 },{ 13,  8182 },{ 13,  8184 },{ 14,  16376 },{ 14,  16372 },
		{ 16,  65520 },{ 15,  32756 },{ 16,  65526 },{ 15,  32757 },{ 18,  262114 },
		{ 19,  524249 },{ 19,  524250 },{ 19,  524251 },{ 19,  524252 },{ 19,  524253 },
		{ 19,  524254 },{ 19,  524248 },{ 19,  524242 },{ 19,  524243 },{ 19,  524244 },
		{ 19,  524245 },{ 19,  524246 },{ 19,  524274 },{ 19,  524255 },{ 19,  524263 },
		{ 19,  524264 },{ 19,  524265 },{ 19,  524266 },{ 19,  524267 },{ 19,  524262 },
		{ 19,  524256 },{ 19,  524257 },{ 19,  524258 },{ 19,  524259 },{ 19,  524260 },
		{ 19,  524261 },{ 19,  524247 },{ 19,  524268 },{ 19,  524276 },{ 19,  524275 }
	};





	void LtpInit(faacEncHandle hEncoder);
	void LtpEnd(faacEncHandle hEncoder);
	int LtpEncode(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		LtpInfo *ltpInfo,
		TnsInfo *tnsInfo,
		double *p_spectrum,
		double *p_time_signal);
	void LtpReconstruct(CoderInfo *coderInfo, LtpInfo *ltpInfo, double *p_spectrum);
	void  LtpUpdate(LtpInfo *ltpInfo, double *time_signal,
		double *overlap_signal, int block_size_long);


	void MSEncode(CoderInfo *coderInfo, ChannelInfo *channelInfo, double *spectrum[MAX_CHANNELS], int maxchan, int allowms);
	void MSReconstruct(CoderInfo *coderInfo, ChannelInfo *channelInfo, int numberOfChannels);


#define TAKEHIRO_IEEE754_HACK 1

#define XRPOW_FTOI(src,dest) ((dest) = (int)(src))
#define QUANTFAC(rx)  adj43[rx]
#define ROUNDFAC 0.4054

	static int FixNoise(CoderInfo *coderInfo,
		const double *xr,
		double *xr_pow,
		int *xi,
		double *xmin,
		double *pow43,
		double *adj43);

	static void CalcAllowedDist(CoderInfo *coderInfo, PsyInfo *psyInfo,
		double *xr, double *xmin, int quality);


	void AACQuantizeInit(CoderInfo *coderInfo, uint32_t numChannels,
		AACQuantCfg *aacquantCfg)
	{
		uint32_t channel, i;

		aacquantCfg->pow43 = (double*)AllocMemory(PRECALC_SIZE * sizeof(double));
		aacquantCfg->adj43 = (double*)AllocMemory(PRECALC_SIZE * sizeof(double));

		aacquantCfg->pow43[0] = 0.0;
		for (i = 1; i<PRECALC_SIZE; i++)
			aacquantCfg->pow43[i] = pow((double)i, 4.0 / 3.0);

#if TAKEHIRO_IEEE754_HACK
		aacquantCfg->adj43[0] = 0.0;
		for (i = 1; i < PRECALC_SIZE; i++)
			aacquantCfg->adj43[i] = i - 0.5 - pow(0.5 * (aacquantCfg->pow43[i - 1] + aacquantCfg->pow43[i]), 0.75);
#else // !TAKEHIRO_IEEE754_HACK
		for (i = 0; i < PRECALC_SIZE - 1; i++)
			aacquantCfg->adj43[i] = (i + 1) - pow(0.5 * (aacquantCfg->pow43[i] + aacquantCfg->pow43[i + 1]), 0.75);
		aacquantCfg->adj43[i] = 0.5;
#endif

		for (channel = 0; channel < numChannels; channel++) {
			coderInfo[channel].requantFreq = (double*)AllocMemory(BLOCK_LEN_LONG * sizeof(double));
		}
	}

	void AACQuantizeEnd(CoderInfo *coderInfo, uint32_t numChannels,
		AACQuantCfg *aacquantCfg)
	{
		uint32_t channel;

		if (aacquantCfg->pow43)
		{
			FreeMemory(aacquantCfg->pow43);
			aacquantCfg->pow43 = NULL;
		}
		if (aacquantCfg->adj43)
		{
			FreeMemory(aacquantCfg->adj43);
			aacquantCfg->adj43 = NULL;
		}

		for (channel = 0; channel < numChannels; channel++) {
			if (coderInfo[channel].requantFreq) FreeMemory(coderInfo[channel].requantFreq);
		}
	}

	static void BalanceEnergy(CoderInfo *coderInfo,
		const double *xr, const int *xi,
		double *pow43)
	{
		const double ifqstep = pow(2.0, 0.25);
		const double logstep_1 = 1.0 / log(ifqstep);
		int sb;
		int nsfb = coderInfo->nr_of_sfb;
		int start, end;
		int l;
		double en0, enq;
		int shift;

		for (sb = 0; sb < nsfb; sb++)
		{
			double qfac_1;

			start = coderInfo->sfb_offset[sb];
			end = coderInfo->sfb_offset[sb + 1];

			qfac_1 = pow(2.0, -0.25*(coderInfo->scale_factor[sb] - coderInfo->global_gain));

			en0 = 0.0;
			enq = 0.0;
			for (l = start; l < end; l++)
			{
				double xq;

				if (!sb && !xi[l])
					continue;

				xq = pow43[xi[l]];

				en0 += xr[l] * xr[l];
				enq += xq * xq;
			}

			if (enq == 0.0)
				continue;

			enq *= qfac_1 * qfac_1;

			shift = (int)(log(sqrt(enq / en0)) * logstep_1 + 1000.5);
			shift -= 1000;

			shift += coderInfo->scale_factor[sb];
			coderInfo->scale_factor[sb] = shift;
		}
	}

	static void UpdateRequant(CoderInfo *coderInfo, int *xi,
		double *pow43)
	{
		double *requant_xr = coderInfo->requantFreq;
		int sb;
		int i;

		for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
		{
			double invQuantFac =
				pow(2.0, -0.25*(coderInfo->scale_factor[sb] - coderInfo->global_gain));
			int start = coderInfo->sfb_offset[sb];
			int end = coderInfo->sfb_offset[sb + 1];

			for (i = start; i < end; i++)
				requant_xr[i] = pow43[xi[i]] * invQuantFac;
		}
	}

	int AACQuantize(CoderInfo *coderInfo,
		PsyInfo *psyInfo,
		ChannelInfo *channelInfo,
		int *cb_width,
		int num_cb,
		double *xr,
		AACQuantCfg *aacquantCfg)
	{
		int sb, i, do_q = 0;
		int bits = 0, sign;
		double xr_pow[FRAME_LEN];
		double xmin[MAX_SCFAC_BANDS];
		int xi[FRAME_LEN];

		/* Use local copy's */
		int *scale_factor = coderInfo->scale_factor;

		/* Set all scalefactors to 0 */
		coderInfo->global_gain = 0;
		for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
			scale_factor[sb] = 0;

		/* Compute xr_pow */
		for (i = 0; i < FRAME_LEN; i++) {
			double temp = fabs(xr[i]);
			xr_pow[i] = sqrt(temp * sqrt(temp));
			do_q += (temp > 1E-20);
		}

		if (do_q) {
			CalcAllowedDist(coderInfo, psyInfo, xr, xmin, (int)aacquantCfg->quality);
			coderInfo->global_gain = 0;
			FixNoise(coderInfo, xr, xr_pow, xi, xmin,
				aacquantCfg->pow43, aacquantCfg->adj43);
			BalanceEnergy(coderInfo, xr, xi, aacquantCfg->pow43);
			UpdateRequant(coderInfo, xi, aacquantCfg->pow43);

			for (i = 0; i < FRAME_LEN; i++) {
				sign = (xr[i] < 0) ? -1 : 1;
				xi[i] *= sign;
				coderInfo->requantFreq[i] *= sign;
			}
		}
		else {
			coderInfo->global_gain = 0;
			SetMemory(xi, 0, FRAME_LEN * sizeof(int));
		}

		BitSearch(coderInfo, xi);

		/* offset the difference of common_scalefac and scalefactors by SF_OFFSET  */
		for (i = 0; i < coderInfo->nr_of_sfb; i++) {
			if ((coderInfo->book_vector[i] != INTENSITY_HCB) && (coderInfo->book_vector[i] != INTENSITY_HCB2)) {
				scale_factor[i] = coderInfo->global_gain - scale_factor[i] + SF_OFFSET;
			}
		}
		coderInfo->global_gain = scale_factor[0];
#if 0
		printf("global gain: %d\n", coderInfo->global_gain);
		for (i = 0; i < coderInfo->nr_of_sfb; i++)
			printf("sf %d: %d\n", i, coderInfo->scale_factor[i]);
#endif
		// clamp to valid diff range
		{
			int previous_scale_factor = coderInfo->global_gain;
			int previous_is_factor = 0;
			for (i = 0; i < coderInfo->nr_of_sfb; i++) {
				if ((coderInfo->book_vector[i] == INTENSITY_HCB) ||
					(coderInfo->book_vector[i] == INTENSITY_HCB2)) {
					const int diff = scale_factor[i] - previous_is_factor;
					if (diff < -60) scale_factor[i] = previous_is_factor - 60;
					else if (diff > 59) scale_factor[i] = previous_is_factor + 59;
					previous_is_factor = scale_factor[i];
					//            printf("sf %d: %d diff=%d **\n", i, coderInfo->scale_factor[i], diff);
				}
				else if (coderInfo->book_vector[i]) {
					const int diff = scale_factor[i] - previous_scale_factor;
					if (diff < -60) scale_factor[i] = previous_scale_factor - 60;
					else if (diff > 59) scale_factor[i] = previous_scale_factor + 59;
					previous_scale_factor = scale_factor[i];
					//            printf("sf %d: %d diff=%d\n", i, coderInfo->scale_factor[i], diff);
				}
			}
		}

		coderInfo->spectral_count = 0;
		sb = 0;
		for (i = 0; i < coderInfo->nr_of_sfb; i++) {
			OutputBits(
				coderInfo,
				coderInfo->book_vector[i],
				xi,
				coderInfo->sfb_offset[i],
				coderInfo->sfb_offset[i + 1] - coderInfo->sfb_offset[i]);

			if (coderInfo->book_vector[i])
				sb = i;
		}

		// FIXME: Check those max_sfb/nr_of_sfb. Isn't it the same?
		coderInfo->max_sfb = coderInfo->nr_of_sfb = sb + 1;

		return bits;
	}


#if TAKEHIRO_IEEE754_HACK

	typedef union {
		float f;
		int i;
	} fi_union;

#define MAGIC_FLOAT (65536*(128))
#define MAGIC_INT 0x4b000000

	static void QuantizeBand(const double *xp, int *pi, double istep,
		int offset, int end, double *adj43)
	{
		int j;
		fi_union *fi;

		fi = (fi_union *)pi;
		for (j = offset; j < end; j++)
		{
			double x0 = istep * xp[j];

			x0 += (double)MAGIC_FLOAT;
			fi[j].f = (float)x0;
			fi[j].f = (float)(x0 + (adj43 - MAGIC_INT)[fi[j].i]);
			fi[j].i -= MAGIC_INT;
		}
	}
#else
#if 0
	static void Quantize(const double *xr, int *ix, double istep)
	{
		int j;

		for (j = FRAME_LEN / 8; j > 0; --j) {
			double x1, x2, x3, x4, x5, x6, x7, x8;
			int rx1, rx2, rx3, rx4, rx5, rx6, rx7, rx8;

			x1 = *xr++ * istep;
			x2 = *xr++ * istep;
			XRPOW_FTOI(x1, rx1);
			x3 = *xr++ * istep;
			XRPOW_FTOI(x2, rx2);
			x4 = *xr++ * istep;
			XRPOW_FTOI(x3, rx3);
			x5 = *xr++ * istep;
			XRPOW_FTOI(x4, rx4);
			x6 = *xr++ * istep;
			XRPOW_FTOI(x5, rx5);
			x7 = *xr++ * istep;
			XRPOW_FTOI(x6, rx6);
			x8 = *xr++ * istep;
			XRPOW_FTOI(x7, rx7);
			x1 += QUANTFAC(rx1);
			XRPOW_FTOI(x8, rx8);
			x2 += QUANTFAC(rx2);
			XRPOW_FTOI(x1, *ix++);
			x3 += QUANTFAC(rx3);
			XRPOW_FTOI(x2, *ix++);
			x4 += QUANTFAC(rx4);
			XRPOW_FTOI(x3, *ix++);
			x5 += QUANTFAC(rx5);
			XRPOW_FTOI(x4, *ix++);
			x6 += QUANTFAC(rx6);
			XRPOW_FTOI(x5, *ix++);
			x7 += QUANTFAC(rx7);
			XRPOW_FTOI(x6, *ix++);
			x8 += QUANTFAC(rx8);
			XRPOW_FTOI(x7, *ix++);
			XRPOW_FTOI(x8, *ix++);
		}
	}
#endif
	static void QuantizeBand(const double *xp, int *ix, double istep,
		int offset, int end, double *adj43)
	{
		int j;

		for (j = offset; j < end; j++)
		{
			double x0 = istep * xp[j];
			x0 += adj43[(int)x0];
			ix[j] = (int)x0;
		}
	}
#endif

	static void CalcAllowedDist(CoderInfo *coderInfo, PsyInfo *psyInfo,
		double *xr, double *xmin, int quality)
	{
		int sfb, start, end, l;
		const double globalthr = 132.0 / (double)quality;
		int last = coderInfo->lastx;
		int lastsb = 0;
		int *cb_offset = coderInfo->sfb_offset;
		int num_cb = coderInfo->nr_of_sfb;
		double avgenrg = coderInfo->avgenrg;

		for (sfb = 0; sfb < num_cb; sfb++)
		{
			if (last > cb_offset[sfb])
				lastsb = sfb;
		}

		for (sfb = 0; sfb < num_cb; sfb++)
		{
			double thr, tmp;
			double enrg = 0.0;

			start = cb_offset[sfb];
			end = cb_offset[sfb + 1];

			if (sfb > lastsb)
			{
				xmin[sfb] = 0;
				continue;
			}

			if (coderInfo->block_type != ONLY_SHORT_WINDOW)
			{
				double enmax = -1.0;
				double lmax;

				lmax = start;
				for (l = start; l < end; l++)
				{
					if (enmax < (xr[l] * xr[l]))
					{
						enmax = xr[l] * xr[l];
						lmax = l;
					}
				}

				start = (int)lmax - 2;
				end = (int)lmax + 3;
				if (start < 0)
					start = 0;
				if (end > last)
					end = last;
			}

			for (l = start; l < end; l++)
			{
				enrg += xr[l] * xr[l];
			}

			thr = enrg / ((double)(end - start)*avgenrg);
			thr = pow(thr, 0.1*(lastsb - sfb) / lastsb + 0.3);

			tmp = 1.0 - ((double)start / (double)last);
			tmp = tmp * tmp * tmp + 0.075;

			thr = 1.0 / (1.4*thr + tmp);

			xmin[sfb] = ((coderInfo->block_type == ONLY_SHORT_WINDOW) ? 0.65 : 1.12)
				* globalthr * thr;
		}
	}

	static int FixNoise(CoderInfo *coderInfo,
		const double *xr,
		double *xr_pow,
		int *xi,
		double *xmin,
		double *pow43,
		double *adj43)
	{
		int i, sb;
		int start, end;
		double diffvol;
		double tmp;
		const double ifqstep = pow(2.0, 0.1875);
		const double log_ifqstep = 1.0 / log(ifqstep);
		const double maxstep = 0.05;

		for (sb = 0; sb < coderInfo->nr_of_sfb; sb++)
		{
			double sfacfix;
			double fixstep = 0.25;
			int sfac;
			double fac;
			int dist;
			double sfacfix0 = 1.0, dist0 = 1e50;
			double maxx;

			start = coderInfo->sfb_offset[sb];
			end = coderInfo->sfb_offset[sb + 1];

			if (!xmin[sb])
				goto nullsfb;

			maxx = 0.0;
			for (i = start; i < end; i++)
			{
				if (xr_pow[i] > maxx)
					maxx = xr_pow[i];
			}

			//printf("band %d: maxx: %f\n", sb, maxx);
			if (maxx < 10.0)
			{
			nullsfb:
				for (i = start; i < end; i++)
					xi[i] = 0;
				coderInfo->scale_factor[sb] = 10;
				continue;
			}

			sfacfix = 1.0 / maxx;
			sfac = (int)(log(sfacfix) * log_ifqstep - 0.5);
			for (i = start; i < end; i++)
				xr_pow[i] *= sfacfix;
			maxx *= sfacfix;
			coderInfo->scale_factor[sb] = sfac;
			QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
				adj43);
			//printf("\tsfac: %d\n", sfac);

		calcdist:
			diffvol = 0.0;
			for (i = start; i < end; i++)
			{
				tmp = xi[i];
				diffvol += tmp * tmp;  // ~x^(3/2)
			}

			if (diffvol < 1e-6)
				diffvol = 1e-6;
			tmp = pow(diffvol / (double)(end - start), -0.666);

			if (fabs(fixstep) > maxstep)
			{
				double dd = 0.5*(tmp / xmin[sb] - 1.0);

				if (fabs(dd) < fabs(fixstep))
				{
					fixstep = dd;

					if (fabs(fixstep) < maxstep)
						fixstep = maxstep * ((fixstep > 0) ? 1 : -1);
				}
			}

			if (fixstep > 0)
			{
				if (tmp < dist0)
				{
					dist0 = tmp;
					sfacfix0 = sfacfix;
				}
				else
				{
					if (fixstep > .1)
						fixstep = .1;
				}
			}
			else
			{
				dist0 = tmp;
				sfacfix0 = sfacfix;
			}

			dist = (tmp > xmin[sb]);
			fac = 0.0;
			if (fabs(fixstep) >= maxstep)
			{
				if ((dist && (fixstep < 0))
					|| (!dist && (fixstep > 0)))
				{
					fixstep = -0.5 * fixstep;
				}

				fac = 1.0 + fixstep;
			}
			else if (dist)
			{
				fac = 1.0 + fabs(fixstep);
			}

			if (fac != 0.0)
			{
				if (maxx * fac >= IXMAX_VAL)
				{
					// restore best noise
					fac = sfacfix0 / sfacfix;
					for (i = start; i < end; i++)
						xr_pow[i] *= fac;
					maxx *= fac;
					sfacfix *= fac;
					coderInfo->scale_factor[sb] = (int)(log(sfacfix) * log_ifqstep - 0.5);
					QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
						adj43);
					continue;
				}

				if (coderInfo->scale_factor[sb] < -10)
				{
					for (i = start; i < end; i++)
						xr_pow[i] *= fac;
					maxx *= fac;
					sfacfix *= fac;
					coderInfo->scale_factor[sb] = (int)(log(sfacfix) * log_ifqstep - 0.5);
					QuantizeBand(xr_pow, xi, IPOW20(coderInfo->global_gain), start, end,
						adj43);
					goto calcdist;
				}
			}
		}
		return 0;
	}

	int SortForGrouping(CoderInfo* coderInfo,
		PsyInfo *psyInfo,
		ChannelInfo *channelInfo,
		int *sfb_width_table,
		double *xr)
	{
		int i, j, ii;
		int index = 0;
		double xr_tmp[FRAME_LEN];
		int group_offset = 0;
		int k = 0;
		int windowOffset = 0;


		/* set up local variables for used quantInfo elements */
		int* sfb_offset = coderInfo->sfb_offset;
		int* nr_of_sfb = &(coderInfo->nr_of_sfb);
		int* window_group_length;
		int num_window_groups;
		*nr_of_sfb = coderInfo->max_sfb;              /* Init to max_sfb */
		window_group_length = coderInfo->window_group_length;
		num_window_groups = coderInfo->num_window_groups;

		/* calc org sfb_offset just for shortblock */
		sfb_offset[k] = 0;
		for (k = 1; k <*nr_of_sfb + 1; k++) {
			sfb_offset[k] = sfb_offset[k - 1] + sfb_width_table[k - 1];
		}

		/* sort the input spectral coefficients */
		index = 0;
		group_offset = 0;
		for (i = 0; i< num_window_groups; i++) {
			for (k = 0; k<*nr_of_sfb; k++) {
				for (j = 0; j < window_group_length[i]; j++) {
					for (ii = 0; ii< sfb_width_table[k]; ii++)
						xr_tmp[index++] = xr[ii + sfb_offset[k] + BLOCK_LEN_SHORT*j + group_offset];
				}
			}
			group_offset += BLOCK_LEN_SHORT*window_group_length[i];
		}

		for (k = 0; k<FRAME_LEN; k++) {
			xr[k] = xr_tmp[k];
		}


		/* now calc the new sfb_offset table for the whole p_spectrum vector*/
		index = 0;
		sfb_offset[index++] = 0;
		windowOffset = 0;
		for (i = 0; i < num_window_groups; i++) {
			for (k = 0; k <*nr_of_sfb; k++) {
				sfb_offset[index] = sfb_offset[index - 1] + sfb_width_table[k] * window_group_length[i];
				index++;
			}
			windowOffset += window_group_length[i];
		}

		*nr_of_sfb = *nr_of_sfb * num_window_groups;  /* Number interleaved bands. */

		return 0;
	}

	void CalcAvgEnrg(CoderInfo *coderInfo,
		const double *xr)
	{
		int end, l;
		int last = 0;
		double totenrg = 0.0;

		end = coderInfo->sfb_offset[coderInfo->nr_of_sfb];
		for (l = 0; l < end; l++)
		{
			if (xr[l])
			{
				last = l;
				totenrg += xr[l] * xr[l];
			}
		}
		last++;

		coderInfo->lastx = last;
		coderInfo->avgenrg = totenrg / last;
	}


	void PredInit(faacEncHandle hEncoder)
	{
		uint32_t channel;

		for (channel = 0; channel < hEncoder->numChannels; channel++) {
			BwpInfo *bwpInfo = &(hEncoder->coderInfo[channel].bwpInfo);

			bwpInfo->psy_init_mc = 0;
			bwpInfo->reset_count_mc = 0;
		}
	}

	void PredCalcPrediction(double *act_spec, double *last_spec, int btype,
		int nsfb,
		int *isfb_width,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		int chanNum)
	{
		int i, k, j, cb_long;
		int leftChanNum;
		int isRightWithCommonWindow;
		double num_bit, snr[SBMAX_L];
		double energy[BLOCK_LEN_LONG], snr_p[BLOCK_LEN_LONG], temp1, temp2;
		ChannelInfo *thisChannel;

		/* Set pointers for specified channel number */
		/* int psy_init; */
		int *psy_init;
		double(*dr)[BLOCK_LEN_LONG], (*e)[BLOCK_LEN_LONG];
		double(*K)[BLOCK_LEN_LONG], (*R)[BLOCK_LEN_LONG];
		double(*VAR)[BLOCK_LEN_LONG], (*KOR)[BLOCK_LEN_LONG];
		double *sb_samples_pred;
		int *thisLineNeedsResetting;
		/* int reset_count; */
		int *reset_count;
		int *pred_global_flag;
		int *pred_sfb_flag;
		int *reset_group;

		/* Set pointers for this chanNum */
		pred_global_flag = &(coderInfo[chanNum].pred_global_flag);
		pred_sfb_flag = coderInfo[chanNum].pred_sfb_flag;
		reset_group = &(coderInfo[chanNum].reset_group_number);
		psy_init = &coderInfo[chanNum].bwpInfo.psy_init_mc;
		dr = &coderInfo[chanNum].bwpInfo.dr_mc[0];
		e = &coderInfo[chanNum].bwpInfo.e_mc[0];
		K = &coderInfo[chanNum].bwpInfo.K_mc[0];
		R = &coderInfo[chanNum].bwpInfo.R_mc[0];
		VAR = &coderInfo[chanNum].bwpInfo.VAR_mc[0];
		KOR = &coderInfo[chanNum].bwpInfo.KOR_mc[0];
		sb_samples_pred = &coderInfo[chanNum].bwpInfo.sb_samples_pred_mc[0];
		thisLineNeedsResetting = &coderInfo[chanNum].bwpInfo.thisLineNeedsResetting_mc[0];
		reset_count = &coderInfo[chanNum].bwpInfo.reset_count_mc;

		thisChannel = &(channelInfo[chanNum]);
		*psy_init = (*psy_init && (btype != 2));

		if ((*psy_init) == 0) {
			for (j = 0; j<BLOCK_LEN_LONG; j++) {
				thisLineNeedsResetting[j] = 1;
			}
			*psy_init = 1;
		}

		if (btype == 2) {
			pred_global_flag[0] = 0;
			/* SHORT WINDOWS reset all the co-efficients    */
			if (thisChannel->ch_is_left) {
				(*reset_count)++;
				if (*reset_count >= 31 * RESET_FRAME)
					*reset_count = RESET_FRAME;
			}
			return;
		}


		/**************************************************/
		/*  Compute state using last_spec                 */
		/**************************************************/
		for (i = 0; i<BLOCK_LEN_LONG; i++)
		{
			/* e[0][i]=last_spec[i]; */
			e[0][i] = last_spec[i] + sb_samples_pred[i];

			for (j = 1; j <= LPC; j++)
				e[j][i] = e[j - 1][i] - K[j][i] * R[j - 1][i];

			for (j = 1; j<LPC; j++)
				dr[j][i] = K[j][i] * e[j - 1][i];

			for (j = 1; j <= LPC; j++) {
				VAR[j][i] = ALPHA*VAR[j][i] + .5*(R[j - 1][i] * R[j - 1][i] + e[j - 1][i] * e[j - 1][i]);
				KOR[j][i] = ALPHA*KOR[j][i] + R[j - 1][i] * e[j - 1][i];
			}

			for (j = LPC - 1; j >= 1; j--)
				R[j][i] = A*(R[j - 1][i] - dr[j][i]);
			R[0][i] = A*e[0][i];
		}


		/**************************************************/
		/* Reset state here if resets were sent           */
		/**************************************************/
		for (i = 0; i<BLOCK_LEN_LONG; i++) {
			if (thisLineNeedsResetting[i]) {
				for (j = 0; j <= LPC; j++)
				{
					K[j][i] = 0.0;
					e[j][i] = 0.0;
					R[j][i] = 0.0;
					VAR[j][i] = 1.0;
					KOR[j][i] = 0.0;
					dr[j][i] = 0.0;
				}
			}
		}



		/**************************************************/
		/* Compute predictor coefficients, predicted data */
		/**************************************************/
		for (i = 0; i<BLOCK_LEN_LONG; i++)
		{
			for (j = 1; j <= LPC; j++) {
				if (VAR[j][i]>MINVAR)
					K[j][i] = KOR[j][i] / VAR[j][i] * B;
				else
					K[j][i] = 0;
			}
		}


		for (k = 0; k<BLOCK_LEN_LONG; k++)
		{
			sb_samples_pred[k] = 0.0;
			for (i = 1; i <= LPC; i++)
				sb_samples_pred[k] += K[i][k] * R[i - 1][k];
		}


		/***********************************************************/
		/* If this is the right channel of a channel_pair_element, */
		/* AND common_window is 1 in this channel_pair_element,    */
		/* THEN copy predictor data to use from the left channel.  */
		/* ELSE determine independent predictor data and resets.   */
		/***********************************************************/
		/* BE CAREFUL HERE, this assumes that predictor data has   */
		/* already been determined for the left channel!!          */
		/***********************************************************/
		isRightWithCommonWindow = 0;     /* Is this a right channel with common_window?*/
		if ((thisChannel->cpe) && (!(thisChannel->ch_is_left))) {
			leftChanNum = thisChannel->paired_ch;
			if (channelInfo[leftChanNum].common_window) {
				isRightWithCommonWindow = 1;
			}
		}

		if (isRightWithCommonWindow) {

			/**************************************************/
			/* Use predictor data from the left channel.      */
			/**************************************************/
			CopyPredInfo(&(coderInfo[chanNum]), &(coderInfo[leftChanNum]));

			/* Make sure to turn off bands with intensity stereo */
#if 0
			if (thisChannel->is_info.is_present) {
				for (i = 0; i<nsfb; i++) {
					if (thisChannel->is_info.is_used[i]) {
						pred_sfb_flag[i] = 0;
					}
				}
			}
#endif

			cb_long = 0;
			for (i = 0; i<nsfb; i++)
			{
				if (!pred_sfb_flag[i]) {
					for (j = cb_long; j<cb_long + isfb_width[i]; j++)
						sb_samples_pred[j] = 0.0;
				}
				cb_long += isfb_width[i];
			}

			/* Disable prediction for bands nsfb through SBMAX_L */
			for (i = j; i<BLOCK_LEN_LONG; i++) {
				sb_samples_pred[i] = 0.0;
			}
			for (i = nsfb; i<SBMAX_L; i++) {
				pred_sfb_flag[i] = 0;
			}

			/* Is global enable set, if not enabled predicted samples are zeroed */
			if (!pred_global_flag[0]) {
				for (j = 0; j<BLOCK_LEN_LONG; j++)
					sb_samples_pred[j] = 0.0;
			}
			for (j = 0; j<BLOCK_LEN_LONG; j++)
				act_spec[j] -= sb_samples_pred[j];

		}
		else {

			/**************************************************/
			/* Determine whether to enable/disable prediction */
			/**************************************************/

			for (k = 0; k<BLOCK_LEN_LONG; k++) {
				energy[k] = act_spec[k] * act_spec[k];
				snr_p[k] = (act_spec[k] - sb_samples_pred[k])*(act_spec[k] - sb_samples_pred[k]);
			}

			cb_long = 0;
			for (i = 0; i<nsfb; i++) {
				pred_sfb_flag[i] = 1;
				temp1 = 0.0;
				temp2 = 0.0;
				for (j = cb_long; j<cb_long + isfb_width[i]; j++) {
					temp1 += energy[j];
					temp2 += snr_p[j];
				}
				if (temp2<1.e-20)
					temp2 = 1.e-20;
				if (temp1 != 0.0)
					snr[i] = -10.*log10((double)temp2 / temp1);
				else
					snr[i] = 0.0;

				if (snr[i] <= 0.0) {
					pred_sfb_flag[i] = 0;
					for (j = cb_long; j<cb_long + isfb_width[i]; j++)
						sb_samples_pred[j] = 0.0;
				}
				cb_long += isfb_width[i];
			}

			/* Disable prediction for bands nsfb through SBMAX_L */
			for (i = j; i<BLOCK_LEN_LONG; i++) {
				sb_samples_pred[i] = 0.0;
			}
			for (i = nsfb; i<SBMAX_L; i++) {
				pred_sfb_flag[i] = 0;
			}

			num_bit = 0.0;
			for (i = 0; i<nsfb; i++)
				if (snr[i]>0.0)
					num_bit += snr[i] / 6.*isfb_width[i];

			/* Determine global enable, if not enabled predicted samples are zeroed */
			pred_global_flag[0] = 1;
			if (num_bit<50) {
				pred_global_flag[0] = 0; num_bit = 0.0;
				for (j = 0; j<BLOCK_LEN_LONG; j++)
					sb_samples_pred[j] = 0.0;
			}
			for (j = 0; j<BLOCK_LEN_LONG; j++)
				act_spec[j] -= sb_samples_pred[j];

		}

		/**********************************************************/
		/* If this is a left channel, determine pred resets.      */
		/* If this is a right channel, using pred reset data from */
		/* left channel.  Keep left and right resets in sync.     */
		/**********************************************************/
		if ((thisChannel->cpe) && (!(thisChannel->ch_is_left))) {
			/*  if (!thisChannel->ch_is_left) {*/
			/**********************************************************/
			/* Using predictor reset data from the left channel.      */
			/**********************************************************/
			reset_count = &coderInfo[leftChanNum].bwpInfo.reset_count_mc;
			/* Reset the frame counter */
			for (i = 0; i<BLOCK_LEN_LONG; i++) {
				thisLineNeedsResetting[i] = 0;
			}
			reset_group = &(coderInfo[chanNum].reset_group_number);
			if (*reset_count % RESET_FRAME == 0)
			{ /* Send a reset in this frame */
				*reset_group = *reset_count / 8;
				for (i = *reset_group - 1; i < BLOCK_LEN_LONG; i += 30)
				{
					thisLineNeedsResetting[i] = 1;
				}
			}
			else
				*reset_group = -1;
		}
		else {
			/******************************************************************/
			/* Determine whether a prediction reset is required - if so, then */
			/* set reset flag for the appropriate group.                      */
			/******************************************************************/

			/* Increase counter on left channel, keep left and right resets in sync */
			(*reset_count)++;

			/* Reset the frame counter */
			for (i = 0; i<BLOCK_LEN_LONG; i++) {
				thisLineNeedsResetting[i] = 0;
			}
			if (*reset_count >= 31 * RESET_FRAME)
				*reset_count = RESET_FRAME;
			if (*reset_count % RESET_FRAME == 0)
			{ /* Send a reset in this frame */
				*reset_group = *reset_count / 8;
				for (i = *reset_group - 1; i < BLOCK_LEN_LONG; i += 30)
				{
					thisLineNeedsResetting[i] = 1;
				}
			}
			else
				*reset_group = -1;
		}


		/* Ensure that prediction data is sent when there is a prediction
		* reset.
		*/
		if (*reset_group != -1 && pred_global_flag[0] == 0)
		{
			pred_global_flag[0] = 1;
			for (i = 0; i < nsfb; i++)
				pred_sfb_flag[i] = 0;
		}
	}


	void CopyPredInfo(CoderInfo *right, CoderInfo *left)
	{
		int band;

		right->pred_global_flag = left->pred_global_flag;
		right->reset_group_number = left->reset_group_number;

		for (band = 0; band<MAX_SCFAC_BANDS; band++) {
			right->pred_sfb_flag[band] = left->pred_sfb_flag[band];
		}
	}


	static int CountBitstream(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int numChannels);
	static int WriteADTSHeader(faacEncHandle hEncoder,
		BitStream *bitStream,
		int writeFlag);
	static int WriteCPE(CoderInfo *coderInfoL,
		CoderInfo *coderInfoR,
		ChannelInfo *channelInfo,
		BitStream* bitStream,
		int objectType,
		int writeFlag);
	static int WriteSCE(CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int objectType,
		int writeFlag);
	static int WriteLFE(CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int objectType,
		int writeFlag);
	static int WriteICSInfo(CoderInfo *coderInfo,
		BitStream *bitStream,
		int objectType,
		int common_window,
		int writeFlag);
	static int WriteICS(CoderInfo *coderInfo,
		BitStream *bitStream,
		int commonWindow,
		int objectType,
		int writeFlag);
	static int WriteLTPPredictorData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WritePredictorData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WritePulseData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WriteTNSData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WriteGainControlData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WriteSpectralData(CoderInfo *coderInfo,
		BitStream *bitStream,
		int writeFlag);
	static int WriteAACFillBits(BitStream* bitStream,
		int numBits,
		int writeFlag);
	static int FindGroupingBits(CoderInfo *coderInfo);
	static long BufferNumBit(BitStream *bitStream);
	static int WriteByte(BitStream *bitStream,
		uint32_t data,
		int numBit);
	static int ByteAlign(BitStream* bitStream,
		int writeFlag, int bitsSoFar);

	static int WriteFAACStr(BitStream *bitStream, char *version, int write)
	{
		int i;
		char str[200];
		int len, padbits, count;
		int bitcnt;

		sprintf(str, "libfaac %s", version);

		len = strlen(str) + 1;
		padbits = (8 - ((bitStream->numBit + 7) % 8)) % 8;
		count = len + 3;

		bitcnt = LEN_SE_ID + 4 + ((count < 15) ? 0 : 8) + count * 8;
		if (!write)
			return bitcnt;

		PutBit(bitStream, ID_FIL, LEN_SE_ID);
		if (count < 15)
		{
			PutBit(bitStream, count, 4);
		}
		else
		{
			PutBit(bitStream, 15, 4);
			PutBit(bitStream, count - 14, 8);
		}

		PutBit(bitStream, 0, padbits);
		PutBit(bitStream, 0, 8);
		PutBit(bitStream, 0, 8); // just in case
		for (i = 0; i < len; i++)
			PutBit(bitStream, str[i], 8);

		PutBit(bitStream, 0, 8 - padbits);

		return bitcnt;
	}


	int WriteBitstream(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int numChannel)
	{
		int channel;
		int bits = 0;
		int bitsLeftAfterFill, numFillBits;

		CountBitstream(hEncoder, coderInfo, channelInfo, bitStream, numChannel);

		if (hEncoder->config.outputFormat == 1) {
			bits += WriteADTSHeader(hEncoder, bitStream, 1);
		}
		else {
			bits = 0; // compilier will remove it, byt anyone will see that current size of bitstream is 0
		}

		if (hEncoder->frameNum == 4)
			WriteFAACStr(bitStream, hEncoder->config.name, 1);

		for (channel = 0; channel < numChannel; channel++) {

			if (channelInfo[channel].present) {

				/* Write out a single_channel_element */
				if (!channelInfo[channel].cpe) {

					if (channelInfo[channel].lfe) {
						/* Write out lfe */
						bits += WriteLFE(&coderInfo[channel],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							1);
					}
					else {
						/* Write out sce */
						bits += WriteSCE(&coderInfo[channel],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							1);
					}

				}
				else {

					if (channelInfo[channel].ch_is_left) {
						/* Write out cpe */
						bits += WriteCPE(&coderInfo[channel],
							&coderInfo[channelInfo[channel].paired_ch],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							1);
					}
				}
			}
		}

		/* Compute how many fill bits are needed to avoid overflowing bit reservoir */
		/* Save room for ID_END terminator */
		if (bits < (8 - LEN_SE_ID)) {
			numFillBits = 8 - LEN_SE_ID - bits;
		}
		else {
			numFillBits = 0;
		}

		/* Write AAC fill_elements, smallest fill element is 7 bits. */
		/* Function may leave up to 6 bits left after fill, so tell it to fill a few extra */
		numFillBits += 6;
		bitsLeftAfterFill = WriteAACFillBits(bitStream, numFillBits, 1);
		bits += (numFillBits - bitsLeftAfterFill);

		/* Write ID_END terminator */
		bits += LEN_SE_ID;
		PutBit(bitStream, ID_END, LEN_SE_ID);

		/* Now byte align the bitstream */
		/*
		* This byte_alignment() is correct for both MPEG2 and MPEG4, although
		* in MPEG4 the byte_alignment() is officially done before the new frame
		* instead of at the end. But this is basically the same.
		*/
		bits += ByteAlign(bitStream, 1, bits);

		return bits;
	}

	static int CountBitstream(faacEncHandle hEncoder,
		CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int numChannel)
	{
		int channel;
		int bits = 0;
		int bitsLeftAfterFill, numFillBits;


		if (hEncoder->config.outputFormat == 1) {
			bits += WriteADTSHeader(hEncoder, bitStream, 0);
		}
		else {
			bits = 0; // compilier will remove it, byt anyone will see that current size of bitstream is 0
		}

		if (hEncoder->frameNum == 4)
			bits += WriteFAACStr(bitStream, hEncoder->config.name, 0);


		for (channel = 0; channel < numChannel; channel++) {

			if (channelInfo[channel].present) {

				/* Write out a single_channel_element */
				if (!channelInfo[channel].cpe) {

					if (channelInfo[channel].lfe) {
						/* Write out lfe */
						bits += WriteLFE(&coderInfo[channel],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							0);
					}
					else {
						/* Write out sce */
						bits += WriteSCE(&coderInfo[channel],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							0);
					}

				}
				else {

					if (channelInfo[channel].ch_is_left) {
						/* Write out cpe */
						bits += WriteCPE(&coderInfo[channel],
							&coderInfo[channelInfo[channel].paired_ch],
							&channelInfo[channel],
							bitStream,
							hEncoder->config.aacObjectType,
							0);
					}
				}
			}
		}

		/* Compute how many fill bits are needed to avoid overflowing bit reservoir */
		/* Save room for ID_END terminator */
		if (bits < (8 - LEN_SE_ID)) {
			numFillBits = 8 - LEN_SE_ID - bits;
		}
		else {
			numFillBits = 0;
		}

		/* Write AAC fill_elements, smallest fill element is 7 bits. */
		/* Function may leave up to 6 bits left after fill, so tell it to fill a few extra */
		numFillBits += 6;
		bitsLeftAfterFill = WriteAACFillBits(bitStream, numFillBits, 0);
		bits += (numFillBits - bitsLeftAfterFill);

		/* Write ID_END terminator */
		bits += LEN_SE_ID;

		/* Now byte align the bitstream */
		bits += ByteAlign(bitStream, 0, bits);

		hEncoder->usedBytes = bit2byte(bits);

		return bits;
	}

	static int WriteADTSHeader(faacEncHandle hEncoder,
		BitStream *bitStream,
		int writeFlag)
	{
		int bits = 56;

		if (writeFlag) {
			/* Fixed ADTS header */
			PutBit(bitStream, 0xFFFF, 12); /* 12 bit Syncword */
			PutBit(bitStream, hEncoder->config.mpegVersion, 1); /* ID == 0 for MPEG4 AAC, 1 for MPEG2 AAC */
			PutBit(bitStream, 0, 2); /* layer == 0 */
			PutBit(bitStream, 1, 1); /* protection absent */
			PutBit(bitStream, hEncoder->config.aacObjectType - 1, 2); /* profile */
			PutBit(bitStream, hEncoder->sampleRateIdx, 4); /* sampling rate */
			PutBit(bitStream, 0, 1); /* private bit */
			PutBit(bitStream, hEncoder->numChannels, 3); /* ch. config (must be > 0) */
														 /* simply using numChannels only works for
														 6 channels or less, else a channel
														 configuration should be written */
			PutBit(bitStream, 0, 1); /* original/copy */
			PutBit(bitStream, 0, 1); /* home */

#if 0 // Removed in corrigendum 14496-3:2002
			if (hEncoder->config.mpegVersion == 0)
				PutBit(bitStream, 0, 2); /* emphasis */
#endif

										 /* Variable ADTS header */
			PutBit(bitStream, 0, 1); /* copyr. id. bit */
			PutBit(bitStream, 0, 1); /* copyr. id. start */
			PutBit(bitStream, hEncoder->usedBytes, 13);
			PutBit(bitStream, 0x7FF, 11); /* buffer fullness (0x7FF for VBR) */
			PutBit(bitStream, 0, 2); /* raw data blocks (0+1=1) */

		}

		/*
		* MPEG2 says byte_aligment() here, but ADTS always is multiple of 8 bits
		* MPEG4 has no byte_alignment() here
		*/
		/*
		if (hEncoder->config.mpegVersion == 1)
		bits += ByteAlign(bitStream, writeFlag);
		*/

#if 0 // Removed in corrigendum 14496-3:2002
		if (hEncoder->config.mpegVersion == 0)
			bits += 2; /* emphasis */
#endif

		return bits;
	}

	static int WriteCPE(CoderInfo *coderInfoL,
		CoderInfo *coderInfoR,
		ChannelInfo *channelInfo,
		BitStream* bitStream,
		int objectType,
		int writeFlag)
	{
		int bits = 0;

		if (writeFlag) {
			/* write ID_CPE, single_element_channel() identifier */
			PutBit(bitStream, ID_CPE, LEN_SE_ID);

			/* write the element_identifier_tag */
			PutBit(bitStream, channelInfo->tag, LEN_TAG);

			/* common_window? */
			PutBit(bitStream, channelInfo->common_window, LEN_COM_WIN);
		}

		bits += LEN_SE_ID;
		bits += LEN_TAG;
		bits += LEN_COM_WIN;


		/* if common_window, write ics_info */
		if (channelInfo->common_window) {
			int numWindows, maxSfb;

			bits += WriteICSInfo(coderInfoL, bitStream, objectType, channelInfo->common_window, writeFlag);
			numWindows = coderInfoL->num_window_groups;
			maxSfb = coderInfoL->max_sfb;

			if (writeFlag) {
				PutBit(bitStream, channelInfo->msInfo.is_present, LEN_MASK_PRES);
				if (channelInfo->msInfo.is_present == 1) {
					int g;
					int b;
					for (g = 0; g<numWindows; g++) {
						for (b = 0; b<maxSfb; b++) {
							PutBit(bitStream, channelInfo->msInfo.ms_used[g*maxSfb + b], LEN_MASK);
						}
					}
				}
			}
			bits += LEN_MASK_PRES;
			if (channelInfo->msInfo.is_present == 1)
				bits += (numWindows*maxSfb*LEN_MASK);
		}

		/* Write individual_channel_stream elements */
		bits += WriteICS(coderInfoL, bitStream, channelInfo->common_window, objectType, writeFlag);
		bits += WriteICS(coderInfoR, bitStream, channelInfo->common_window, objectType, writeFlag);

		return bits;
	}

	static int WriteSCE(CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int objectType,
		int writeFlag)
	{
		int bits = 0;
		if (writeFlag) {
			/* write Single Element Channel (SCE) identifier */
			PutBit(bitStream, ID_SCE, LEN_SE_ID);

			/* write the element identifier tag */
			PutBit(bitStream, channelInfo->tag, LEN_TAG);
		}

		bits += LEN_SE_ID;
		bits += LEN_TAG;

		/* Write an Individual Channel Stream element */
		bits += WriteICS(coderInfo, bitStream, 0, objectType, writeFlag);

		return bits;
	}

	static int WriteLFE(CoderInfo *coderInfo,
		ChannelInfo *channelInfo,
		BitStream *bitStream,
		int objectType,
		int writeFlag)
	{
		int bits = 0;

		if (writeFlag) {
			/* write ID_LFE, lfe_element_channel() identifier */
			PutBit(bitStream, ID_LFE, LEN_SE_ID);

			/* write the element_identifier_tag */
			PutBit(bitStream, channelInfo->tag, LEN_TAG);
		}

		bits += LEN_SE_ID;
		bits += LEN_TAG;

		/* Write an individual_channel_stream element */
		bits += WriteICS(coderInfo, bitStream, 0, objectType, writeFlag);

		return bits;
	}

	static int WriteICSInfo(CoderInfo *coderInfo,
		BitStream *bitStream,
		int objectType,
		int common_window,
		int writeFlag)
	{
		int grouping_bits;
		int bits = 0;

		if (writeFlag) {
			/* write out ics_info() information */
			PutBit(bitStream, 0, LEN_ICS_RESERV);  /* reserved Bit*/

												   /* Write out window sequence */
			PutBit(bitStream, coderInfo->block_type, LEN_WIN_SEQ);  /* block type */

																	/* Write out window shape */
			PutBit(bitStream, coderInfo->window_shape, LEN_WIN_SH);  /* window shape */
		}

		bits += LEN_ICS_RESERV;
		bits += LEN_WIN_SEQ;
		bits += LEN_WIN_SH;

		/* For short windows, write out max_sfb and scale_factor_grouping */
		if (coderInfo->block_type == ONLY_SHORT_WINDOW) {
			if (writeFlag) {
				PutBit(bitStream, coderInfo->max_sfb, LEN_MAX_SFBS);
				grouping_bits = FindGroupingBits(coderInfo);
				PutBit(bitStream, grouping_bits, MAX_SHORT_WINDOWS - 1);  /* the grouping bits */
			}
			bits += LEN_MAX_SFBS;
			bits += MAX_SHORT_WINDOWS - 1;
		}
		else { /* Otherwise, write out max_sfb and predictor data */
			if (writeFlag) {
				PutBit(bitStream, coderInfo->max_sfb, LEN_MAX_SFBL);
			}
			bits += LEN_MAX_SFBL;

		if (objectType == LTP)
		{
			bits++;
			if (writeFlag)
				PutBit(bitStream, coderInfo->ltpInfo.global_pred_flag, 1); /* Prediction Global used */

			bits += WriteLTPPredictorData(coderInfo, bitStream, writeFlag);
			if (common_window)
				bits += WriteLTPPredictorData(coderInfo, bitStream, writeFlag);
		}
		else {
			bits++;
			if (writeFlag)
				PutBit(bitStream, coderInfo->pred_global_flag, LEN_PRED_PRES);  /* predictor_data_present */

			bits += WritePredictorData(coderInfo, bitStream, writeFlag);
		}
	}

	return bits;
}

static int WriteICS(CoderInfo *coderInfo,
	BitStream *bitStream,
	int commonWindow,
	int objectType,
	int writeFlag)
{
	/* this function writes out an individual_channel_stream to the bitstream and */
	/* returns the number of bits written to the bitstream */
	int bits = 0;

	/* Write the 8-bit global_gain */
	if (writeFlag)
		PutBit(bitStream, coderInfo->global_gain, LEN_GLOB_GAIN);
	bits += LEN_GLOB_GAIN;


	/* Write ics information */
	if (!commonWindow) {
		bits += WriteICSInfo(coderInfo, bitStream, objectType, commonWindow, writeFlag);
	}

	bits += SortBookNumbers(coderInfo, bitStream, writeFlag);
	bits += WriteScalefactors(coderInfo, bitStream, writeFlag);

	bits += WritePulseData(coderInfo, bitStream, writeFlag);
	bits += WriteTNSData(coderInfo, bitStream, writeFlag);
	bits += WriteGainControlData(coderInfo, bitStream, writeFlag);

	bits += WriteSpectralData(coderInfo, bitStream, writeFlag);

	/* Return number of bits */
	return bits;
}

static int WriteLTPPredictorData(CoderInfo *coderInfo, BitStream *bitStream, int writeFlag)
{
	int i, last_band;
	int bits;
	LtpInfo *ltpInfo = &coderInfo->ltpInfo;

	bits = 0;

	if (ltpInfo->global_pred_flag)
	{

		if (writeFlag)
			PutBit(bitStream, 1, 1); /* LTP used */
		bits++;

		switch (coderInfo->block_type)
		{
		case ONLY_LONG_WINDOW:
		case LONG_SHORT_WINDOW:
		case SHORT_LONG_WINDOW:
			bits += LEN_LTP_LAG;
			bits += LEN_LTP_COEF;
			if (writeFlag)
			{
				PutBit(bitStream, ltpInfo->delay[0], LEN_LTP_LAG);
				PutBit(bitStream, ltpInfo->weight_idx, LEN_LTP_COEF);
			}

			last_band = ((coderInfo->nr_of_sfb < MAX_LT_PRED_LONG_SFB) ?
				coderInfo->nr_of_sfb : MAX_LT_PRED_LONG_SFB);
			//            last_band = coderInfo->nr_of_sfb;

			bits += last_band;
			if (writeFlag)
				for (i = 0; i < last_band; i++)
					PutBit(bitStream, ltpInfo->sfb_prediction_used[i], LEN_LTP_LONG_USED);
			break;

		default:
			break;
		}
	}

	return (bits);
}

static int WritePredictorData(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	int bits = 0;

	/* Write global predictor data present */
	short predictorDataPresent = coderInfo->pred_global_flag;
	int numBands = min(coderInfo->max_pred_sfb, coderInfo->nr_of_sfb);

	if (writeFlag) {
		if (predictorDataPresent) {
			int b;
			if (coderInfo->reset_group_number == -1) {
				PutBit(bitStream, 0, LEN_PRED_RST); /* No prediction reset */
			}
			else {
				PutBit(bitStream, 1, LEN_PRED_RST);
				PutBit(bitStream, (uint32_t)coderInfo->reset_group_number,
					LEN_PRED_RSTGRP);
			}

			for (b = 0; b<numBands; b++) {
				PutBit(bitStream, coderInfo->pred_sfb_flag[b], LEN_PRED_ENAB);
			}
		}
	}
	bits += (predictorDataPresent) ?
		(LEN_PRED_RST +
		((coderInfo->reset_group_number) != -1)*LEN_PRED_RSTGRP +
			numBands*LEN_PRED_ENAB) : 0;

	return bits;
}

static int WritePulseData(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	int bits = 0;

	if (writeFlag) {
		PutBit(bitStream, 0, LEN_PULSE_PRES);  /* no pulse_data_present */
	}

	bits += LEN_PULSE_PRES;

	return bits;
}

static int WriteTNSData(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	int bits = 0;
	int numWindows;
	int len_tns_nfilt;
	int len_tns_length;
	int len_tns_order;
	int filtNumber;
	int resInBits;
	int bitsToTransmit;
	uint32_t unsignedIndex;
	int w;

	TnsInfo* tnsInfoPtr = &coderInfo->tnsInfo;

	if (writeFlag) {
		PutBit(bitStream, tnsInfoPtr->tnsDataPresent, LEN_TNS_PRES);
	}
	bits += LEN_TNS_PRES;

	/* If TNS is not present, bail */
	if (!tnsInfoPtr->tnsDataPresent) {
		return bits;
	}

	/* Set window-dependent TNS parameters */
	if (coderInfo->block_type == ONLY_SHORT_WINDOW) {
		numWindows = MAX_SHORT_WINDOWS;
		len_tns_nfilt = LEN_TNS_NFILTS;
		len_tns_length = LEN_TNS_LENGTHS;
		len_tns_order = LEN_TNS_ORDERS;
	}
	else {
		numWindows = 1;
		len_tns_nfilt = LEN_TNS_NFILTL;
		len_tns_length = LEN_TNS_LENGTHL;
		len_tns_order = LEN_TNS_ORDERL;
	}

	/* Write TNS data */
	bits += (numWindows * len_tns_nfilt);
	for (w = 0; w<numWindows; w++) {
		TnsWindowData* windowDataPtr = &tnsInfoPtr->windowData[w];
		int numFilters = windowDataPtr->numFilters;
		if (writeFlag) {
			PutBit(bitStream, numFilters, len_tns_nfilt); /* n_filt[] = 0 */
		}
		if (numFilters) {
			bits += LEN_TNS_COEFF_RES;
			resInBits = windowDataPtr->coefResolution;
			if (writeFlag) {
				PutBit(bitStream, resInBits - DEF_TNS_RES_OFFSET, LEN_TNS_COEFF_RES);
			}
			bits += numFilters * (len_tns_length + len_tns_order);
			for (filtNumber = 0; filtNumber<numFilters; filtNumber++) {
				TnsFilterData* tnsFilterPtr = &windowDataPtr->tnsFilter[filtNumber];
				int order = tnsFilterPtr->order;
				if (writeFlag) {
					PutBit(bitStream, tnsFilterPtr->length, len_tns_length);
					PutBit(bitStream, order, len_tns_order);
				}
				if (order) {
					bits += (LEN_TNS_DIRECTION + LEN_TNS_COMPRESS);
					if (writeFlag) {
						PutBit(bitStream, tnsFilterPtr->direction, LEN_TNS_DIRECTION);
						PutBit(bitStream, tnsFilterPtr->coefCompress, LEN_TNS_COMPRESS);
					}
					bitsToTransmit = resInBits - tnsFilterPtr->coefCompress;
					bits += order * bitsToTransmit;
					if (writeFlag) {
						int i;
						for (i = 1; i <= order; i++) {
							unsignedIndex = (uint32_t)(tnsFilterPtr->index[i])&(~(~0 << bitsToTransmit));
							PutBit(bitStream, unsignedIndex, bitsToTransmit);
						}
					}
				}
			}
		}
	}
	return bits;
}

static int WriteGainControlData(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	int bits = 0;

	if (writeFlag) {
		PutBit(bitStream, 0, LEN_GAIN_PRES);
	}

	bits += LEN_GAIN_PRES;

	return bits;
}

static int WriteSpectralData(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	int i, bits = 0;

	/* set up local pointers to data and len */
	/* data array contains data to be written */
	/* len array contains lengths of data words */
	int* data = coderInfo->data;
	int* len = coderInfo->len;

	if (writeFlag) {
		for (i = 0; i < coderInfo->spectral_count; i++) {
			if (len[i] > 0) {  /* only send out non-zero codebook data */
				PutBit(bitStream, data[i], len[i]); /* write data */
				bits += len[i];
			}
		}
	}
	else {
		for (i = 0; i < coderInfo->spectral_count; i++) {
			bits += len[i];
		}
	}

	return bits;
}

static int WriteAACFillBits(BitStream* bitStream,
	int numBits,
	int writeFlag)
{
	int numberOfBitsLeft = numBits;

	/* Need at least (LEN_SE_ID + LEN_F_CNT) bits for a fill_element */
	int minNumberOfBits = LEN_SE_ID + LEN_F_CNT;

	while (numberOfBitsLeft >= minNumberOfBits)
	{
		int numberOfBytes;
		int maxCount;

		if (writeFlag) {
			PutBit(bitStream, ID_FIL, LEN_SE_ID);   /* Write fill_element ID */
		}
		numberOfBitsLeft -= minNumberOfBits;    /* Subtract for ID,count */

		numberOfBytes = (int)(numberOfBitsLeft / LEN_BYTE);
		maxCount = (1 << LEN_F_CNT) - 1;  /* Max count without escaping */

										  /* if we have less than maxCount bytes, write them now */
		if (numberOfBytes < maxCount) {
			int i;
			if (writeFlag) {
				PutBit(bitStream, numberOfBytes, LEN_F_CNT);
				for (i = 0; i < numberOfBytes; i++) {
					PutBit(bitStream, 0, LEN_BYTE);
				}
			}
			/* otherwise, we need to write an escape count */
		}
		else {
			int maxEscapeCount, maxNumberOfBytes, escCount;
			int i;
			if (writeFlag) {
				PutBit(bitStream, maxCount, LEN_F_CNT);
			}
			maxEscapeCount = (1 << LEN_BYTE) - 1;  /* Max escape count */
			maxNumberOfBytes = maxCount + maxEscapeCount;
			numberOfBytes = (numberOfBytes > maxNumberOfBytes) ? (maxNumberOfBytes) : (numberOfBytes);
			escCount = numberOfBytes - maxCount;
			if (writeFlag) {
				PutBit(bitStream, escCount, LEN_BYTE);
				for (i = 0; i < numberOfBytes - 1; i++) {
					PutBit(bitStream, 0, LEN_BYTE);
				}
			}
		}
		numberOfBitsLeft -= LEN_BYTE*numberOfBytes;
	}

	return numberOfBitsLeft;
}

static int FindGroupingBits(CoderInfo *coderInfo)
{
	/* This function inputs the grouping information and outputs the seven bit
	'grouping_bits' field that the AAC decoder expects.  */

	int grouping_bits = 0;
	int tmp[8];
	int i, j;
	int index = 0;

	for (i = 0; i < coderInfo->num_window_groups; i++) {
		for (j = 0; j < coderInfo->window_group_length[i]; j++) {
			tmp[index++] = i;
		}
	}

	for (i = 1; i < 8; i++) {
		grouping_bits = grouping_bits << 1;
		if (tmp[i] == tmp[i - 1]) {
			grouping_bits++;
		}
	}

	return grouping_bits;
}

/* size in bytes! */
BitStream *OpenBitStream(int size, uint8_t *buffer)
{
	BitStream *bitStream;
	bitStream = (BitStream *)AllocMemory(sizeof(BitStream));
	bitStream->size = size;
	bitStream->numBit = 0;
	bitStream->currentBit = 0;
	bitStream->data = buffer;
	SetMemory(bitStream->data, 0, size);
	return bitStream;
}

int CloseBitStream(BitStream *bitStream)
{
	int bytes = bit2byte(bitStream->numBit);

	FreeMemory(bitStream);

	return bytes;
}

static long BufferNumBit(BitStream *bitStream)
{
	return bitStream->numBit;
}

static int WriteByte(BitStream *bitStream,
	uint32_t data,
	int numBit)
{
	long numUsed, idx;

	idx = (bitStream->currentBit / BYTE_NUMBIT) % bitStream->size;
	numUsed = bitStream->currentBit % BYTE_NUMBIT;

	if (numUsed == 0)
		bitStream->data[idx] = 0;

	bitStream->data[idx] |= (data & ((1 << numBit) - 1)) <<
		(BYTE_NUMBIT - numUsed - numBit);
	bitStream->currentBit += numBit;
	bitStream->numBit = bitStream->currentBit;

	return 0;
}

int PutBit(BitStream *bitStream,
	uint32_t data,
	int numBit)
{
	int num, maxNum, curNum;
	uint32_t bits;

	if (numBit == 0)
		return 0;

	/* write bits in packets according to buffer byte boundaries */
	num = 0;
	maxNum = BYTE_NUMBIT - bitStream->currentBit % BYTE_NUMBIT;
	while (num < numBit) {
		curNum = min(numBit - num, maxNum);
		bits = data >> (numBit - num - curNum);
		if (WriteByte(bitStream, bits, curNum)) {
			return 1;
		}
		num += curNum;
		maxNum = BYTE_NUMBIT;
	}

	return 0;
}

static int ByteAlign(BitStream *bitStream, int writeFlag, int bitsSoFar)
{
	int len, i, j;

	if (writeFlag)
	{
		len = BufferNumBit(bitStream);
	}
	else {
		len = bitsSoFar;
	}

	j = (8 - (len % 8)) % 8;

	if ((len % 8) == 0) j = 0;
	if (writeFlag) {
		for (i = 0; i<j; i++) {
			PutBit(bitStream, 0, 1);
		}
	}
	return j;
}



void GetChannelInfo(ChannelInfo *channelInfo, int numChannels, int useLfe)
{
	int sceTag = 0;
	int lfeTag = 0;
	int cpeTag = 0;
	int numChannelsLeft = numChannels;


	/* First element is sce, except for 2 channel case */
	if (numChannelsLeft != 2) {
		channelInfo[numChannels - numChannelsLeft].present = 1;
		channelInfo[numChannels - numChannelsLeft].tag = sceTag++;
		channelInfo[numChannels - numChannelsLeft].cpe = 0;
		channelInfo[numChannels - numChannelsLeft].lfe = 0;
		numChannelsLeft--;
	}

	/* Next elements are cpe's */
	while (numChannelsLeft > 1) {
		/* Left channel info */
		channelInfo[numChannels - numChannelsLeft].present = 1;
		channelInfo[numChannels - numChannelsLeft].tag = cpeTag++;
		channelInfo[numChannels - numChannelsLeft].cpe = 1;
		channelInfo[numChannels - numChannelsLeft].common_window = 0;
		channelInfo[numChannels - numChannelsLeft].ch_is_left = 1;
		channelInfo[numChannels - numChannelsLeft].paired_ch = numChannels - numChannelsLeft + 1;
		channelInfo[numChannels - numChannelsLeft].lfe = 0;
		numChannelsLeft--;

		/* Right channel info */
		channelInfo[numChannels - numChannelsLeft].present = 1;
		channelInfo[numChannels - numChannelsLeft].cpe = 1;
		channelInfo[numChannels - numChannelsLeft].common_window = 0;
		channelInfo[numChannels - numChannelsLeft].ch_is_left = 0;
		channelInfo[numChannels - numChannelsLeft].paired_ch = numChannels - numChannelsLeft - 1;
		channelInfo[numChannels - numChannelsLeft].lfe = 0;
		numChannelsLeft--;
	}

	/* Is there another channel left ? */
	if (numChannelsLeft) {
		if (useLfe) {
			channelInfo[numChannels - numChannelsLeft].present = 1;
			channelInfo[numChannels - numChannelsLeft].tag = lfeTag++;
			channelInfo[numChannels - numChannelsLeft].cpe = 0;
			channelInfo[numChannels - numChannelsLeft].lfe = 1;
		}
		else {
			channelInfo[numChannels - numChannelsLeft].present = 1;
			channelInfo[numChannels - numChannelsLeft].tag = sceTag++;
			channelInfo[numChannels - numChannelsLeft].cpe = 0;
			channelInfo[numChannels - numChannelsLeft].lfe = 0;
		}
		numChannelsLeft--;
	}
}


#define MAXLOGM 9
#define MAXLOGR 8

#if 1

void fft_initialize(FFT_Tables *fft_tables)
{
	int i;
	fft_tables->costbl = (float**)AllocMemory((MAXLOGM + 1) * sizeof(fft_tables->costbl[0]));
	fft_tables->negsintbl = (float**)AllocMemory((MAXLOGM + 1) * sizeof(fft_tables->negsintbl[0]));
	fft_tables->reordertbl = (uint16_t**)AllocMemory((MAXLOGM + 1) * sizeof(fft_tables->reordertbl[0]));

	for (i = 0; i< MAXLOGM + 1; i++)
	{
		fft_tables->costbl[i] = NULL;
		fft_tables->negsintbl[i] = NULL;
		fft_tables->reordertbl[i] = NULL;
	}
}

void fft_terminate(FFT_Tables *fft_tables)
{
	int i;

	for (i = 0; i< MAXLOGM + 1; i++)
	{
		if (fft_tables->costbl[i] != NULL)
			FreeMemory(fft_tables->costbl[i]);

		if (fft_tables->negsintbl[i] != NULL)
			FreeMemory(fft_tables->negsintbl[i]);

		if (fft_tables->reordertbl[i] != NULL)
			FreeMemory(fft_tables->reordertbl[i]);
	}

	FreeMemory(fft_tables->costbl);
	FreeMemory(fft_tables->negsintbl);
	FreeMemory(fft_tables->reordertbl);

	fft_tables->costbl = NULL;
	fft_tables->negsintbl = NULL;
	fft_tables->reordertbl = NULL;
}

static void reorder(FFT_Tables *fft_tables, double *x, int logm)
{
	int i;
	int size = 1 << logm;
	uint16_t *r;	//size


	if (fft_tables->reordertbl[logm] == NULL) // create bit reversing table
	{
		fft_tables->reordertbl[logm] = (uint16_t*)AllocMemory(size * sizeof(*(fft_tables->reordertbl[0])));

		for (i = 0; i < size; i++)
		{
			int reversed = 0;
			int b0;
			int tmp = i;

			for (b0 = 0; b0 < logm; b0++)
			{
				reversed = (reversed << 1) | (tmp & 1);
				tmp >>= 1;
			}
			fft_tables->reordertbl[logm][i] = reversed;
		}
	}

	r = fft_tables->reordertbl[logm];

	for (i = 0; i < size; i++)
	{
		int j = r[i];
		double tmp;

		if (j <= i)
			continue;

		tmp = x[i];
		x[i] = x[j];
		x[j] = tmp;
	}
}

static void fft_proc(
	double *xr,
	double *xi,
	float *refac,
	float *imfac,
	int size)
{
	int step, shift, pos;
	int exp, estep;

	estep = size;
	for (step = 1; step < size; step *= 2)
	{
		int x1;
		int x2 = 0;
		estep >>= 1;
		for (pos = 0; pos < size; pos += (2 * step))
		{
			x1 = x2;
			x2 += step;
			exp = 0;
			for (shift = 0; shift < step; shift++)
			{
				double v2r, v2i;

				v2r = xr[x2] * refac[exp] - xi[x2] * imfac[exp];
				v2i = xr[x2] * imfac[exp] + xi[x2] * refac[exp];

				xr[x2] = xr[x1] - v2r;
				xr[x1] += v2r;

				xi[x2] = xi[x1] - v2i;

				xi[x1] += v2i;

				exp += estep;

				x1++;
				x2++;
			}
		}
	}
}

static void check_tables(FFT_Tables *fft_tables, int logm)
{
	if (fft_tables->costbl[logm] == NULL)
	{
		int i;
		int size = 1 << logm;

		if (fft_tables->negsintbl[logm] != NULL)
			FreeMemory(fft_tables->negsintbl[logm]);

		fft_tables->costbl[logm] = (float*)AllocMemory((size / 2) * sizeof(*(fft_tables->costbl[0])));
		fft_tables->negsintbl[logm] = (float*)AllocMemory((size / 2) * sizeof(*(fft_tables->negsintbl[0])));

		for (i = 0; i < (size >> 1); i++)
		{
			double theta = (double)2.0 * M_PI * ((double)i) / (double)size;
			fft_tables->costbl[logm][i] = (float)cos(theta);
			fft_tables->negsintbl[logm][i] = -(float)sin(theta);
		}
	}
}

void fft(FFT_Tables *fft_tables, double *xr, double *xi, int logm)
{
	if (logm > MAXLOGM)
	{
		fprintf(stderr, "fft size too big\n");
		exit(1);
	}

	if (logm < 1)
	{
		//printf("logm < 1\n");
		return;
	}

	check_tables(fft_tables, logm);

	reorder(fft_tables, xr, logm);
	reorder(fft_tables, xi, logm);

	fft_proc(xr, xi, fft_tables->costbl[logm], fft_tables->negsintbl[logm], 1 << logm);
}

void rfft(FFT_Tables *fft_tables, double *x, int logm)
{
	double xi[1 << MAXLOGR];

	if (logm > MAXLOGR)
	{
		fprintf(stderr, "rfft size too big\n");
		exit(1);
	}

	memset(xi, 0, (1 << logm) * sizeof(xi[0]));

	fft(fft_tables, x, xi, logm);

	memcpy(x + (1 << (logm - 1)), xi, (1 << (logm - 1)) * sizeof(*x));
}

void ffti(FFT_Tables *fft_tables, double *xr, double *xi, int logm)
{
	int i, size;
	double fac;
	double *xrp, *xip;

	fft(fft_tables, xi, xr, logm);

	size = 1 << logm;
	fac = 1.0 / size;
	xrp = xr;
	xip = xi;

	for (i = 0; i < size; i++)
	{
		*xrp++ *= fac;
		*xip++ *= fac;
	}
}

#endif


#define  TWOPI       2*M_PI


static void		CalculateKBDWindow(double* win, double alpha, int length);
static double	Izero(double x);
static void		MDCT(FFT_Tables *fft_tables, double *data, int N);
static void		IMDCT(FFT_Tables *fft_tables, double *data, int N);



void FilterBankInit(faacEncHandle hEncoder)
{
	uint32_t i, channel;

	for (channel = 0; channel < hEncoder->numChannels; channel++) {
		hEncoder->freqBuff[channel] = (double*)AllocMemory(2 * FRAME_LEN * sizeof(double));
		hEncoder->overlapBuff[channel] = (double*)AllocMemory(FRAME_LEN * sizeof(double));
		SetMemory(hEncoder->overlapBuff[channel], 0, FRAME_LEN * sizeof(double));
	}

	hEncoder->sin_window_long = (double*)AllocMemory(BLOCK_LEN_LONG * sizeof(double));
	hEncoder->sin_window_short = (double*)AllocMemory(BLOCK_LEN_SHORT * sizeof(double));
	hEncoder->kbd_window_long = (double*)AllocMemory(BLOCK_LEN_LONG * sizeof(double));
	hEncoder->kbd_window_short = (double*)AllocMemory(BLOCK_LEN_SHORT * sizeof(double));

	for (i = 0; i<BLOCK_LEN_LONG; i++)
		hEncoder->sin_window_long[i] = sin((M_PI / (2 * BLOCK_LEN_LONG)) * (i + 0.5));
	for (i = 0; i<BLOCK_LEN_SHORT; i++)
		hEncoder->sin_window_short[i] = sin((M_PI / (2 * BLOCK_LEN_SHORT)) * (i + 0.5));

	CalculateKBDWindow(hEncoder->kbd_window_long, 4, BLOCK_LEN_LONG * 2);
	CalculateKBDWindow(hEncoder->kbd_window_short, 6, BLOCK_LEN_SHORT * 2);
}

void FilterBankEnd(faacEncHandle hEncoder)
{
	uint32_t channel;

	for (channel = 0; channel < hEncoder->numChannels; channel++) {
		if (hEncoder->freqBuff[channel]) FreeMemory(hEncoder->freqBuff[channel]);
		if (hEncoder->overlapBuff[channel]) FreeMemory(hEncoder->overlapBuff[channel]);
	}

	if (hEncoder->sin_window_long) FreeMemory(hEncoder->sin_window_long);
	if (hEncoder->sin_window_short) FreeMemory(hEncoder->sin_window_short);
	if (hEncoder->kbd_window_long) FreeMemory(hEncoder->kbd_window_long);
	if (hEncoder->kbd_window_short) FreeMemory(hEncoder->kbd_window_short);
}

void FilterBank(faacEncHandle hEncoder,
	CoderInfo *coderInfo,
	double *p_in_data,
	double *p_out_mdct,
	double *p_overlap,
	int overlap_select)
{
	double *p_o_buf, *first_window, *second_window;
	double *transf_buf;
	int k, i;
	int block_type = coderInfo->block_type;

	transf_buf = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));

	/* create / shift old values */
	/* We use p_overlap here as buffer holding the last frame time signal*/
	if (overlap_select != MNON_OVERLAPPED) {
		memcpy(transf_buf, p_overlap, FRAME_LEN * sizeof(double));
		memcpy(transf_buf + BLOCK_LEN_LONG, p_in_data, FRAME_LEN * sizeof(double));
		memcpy(p_overlap, p_in_data, FRAME_LEN * sizeof(double));
	}
	else {
		memcpy(transf_buf, p_in_data, 2 * FRAME_LEN * sizeof(double));
	}

	/*  Window shape processing */
	if (overlap_select != MNON_OVERLAPPED) {
		switch (coderInfo->prev_window_shape) {
		case SINE_WINDOW:
			if ((block_type == ONLY_LONG_WINDOW) || (block_type == LONG_SHORT_WINDOW))
				first_window = hEncoder->sin_window_long;
			else
				first_window = hEncoder->sin_window_short;
			break;
		case KBD_WINDOW:
			if ((block_type == ONLY_LONG_WINDOW) || (block_type == LONG_SHORT_WINDOW))
				first_window = hEncoder->kbd_window_long;
			else
				first_window = hEncoder->kbd_window_short;
			break;
		}

		switch (coderInfo->window_shape) {
		case SINE_WINDOW:
			if ((block_type == ONLY_LONG_WINDOW) || (block_type == SHORT_LONG_WINDOW))
				second_window = hEncoder->sin_window_long;
			else
				second_window = hEncoder->sin_window_short;
			break;
		case KBD_WINDOW:
			if ((block_type == ONLY_LONG_WINDOW) || (block_type == SHORT_LONG_WINDOW))
				second_window = hEncoder->kbd_window_long;
			else
				second_window = hEncoder->kbd_window_short;
			break;
		}
	}
	else {
		/* Always long block and sine window for LTP */
		first_window = hEncoder->sin_window_long;
		second_window = hEncoder->sin_window_long;
	}

	/* Set ptr to transf-Buffer */
	p_o_buf = transf_buf;

	/* Separate action for each Block Type */
	switch (block_type) {
	case ONLY_LONG_WINDOW:
		for (i = 0; i < BLOCK_LEN_LONG; i++) {
			p_out_mdct[i] = p_o_buf[i] * first_window[i];
			p_out_mdct[i + BLOCK_LEN_LONG] = p_o_buf[i + BLOCK_LEN_LONG] * second_window[BLOCK_LEN_LONG - i - 1];
		}
		MDCT(&hEncoder->fft_tables, p_out_mdct, 2 * BLOCK_LEN_LONG);
		break;

	case LONG_SHORT_WINDOW:
		for (i = 0; i < BLOCK_LEN_LONG; i++)
			p_out_mdct[i] = p_o_buf[i] * first_window[i];
		memcpy(p_out_mdct + BLOCK_LEN_LONG, p_o_buf + BLOCK_LEN_LONG, NFLAT_LS * sizeof(double));
		for (i = 0; i < BLOCK_LEN_SHORT; i++)
			p_out_mdct[i + BLOCK_LEN_LONG + NFLAT_LS] = p_o_buf[i + BLOCK_LEN_LONG + NFLAT_LS] * second_window[BLOCK_LEN_SHORT - i - 1];
		SetMemory(p_out_mdct + BLOCK_LEN_LONG + NFLAT_LS + BLOCK_LEN_SHORT, 0, NFLAT_LS * sizeof(double));
		MDCT(&hEncoder->fft_tables, p_out_mdct, 2 * BLOCK_LEN_LONG);
		break;

	case SHORT_LONG_WINDOW:
		SetMemory(p_out_mdct, 0, NFLAT_LS * sizeof(double));
		for (i = 0; i < BLOCK_LEN_SHORT; i++)
			p_out_mdct[i + NFLAT_LS] = p_o_buf[i + NFLAT_LS] * first_window[i];
		memcpy(p_out_mdct + NFLAT_LS + BLOCK_LEN_SHORT, p_o_buf + NFLAT_LS + BLOCK_LEN_SHORT, NFLAT_LS * sizeof(double));
		for (i = 0; i < BLOCK_LEN_LONG; i++)
			p_out_mdct[i + BLOCK_LEN_LONG] = p_o_buf[i + BLOCK_LEN_LONG] * second_window[BLOCK_LEN_LONG - i - 1];
		MDCT(&hEncoder->fft_tables, p_out_mdct, 2 * BLOCK_LEN_LONG);
		break;

	case ONLY_SHORT_WINDOW:
		p_o_buf += NFLAT_LS;
		for (k = 0; k < MAX_SHORT_WINDOWS; k++) {
			for (i = 0; i < BLOCK_LEN_SHORT; i++) {
				p_out_mdct[i] = p_o_buf[i] * first_window[i];
				p_out_mdct[i + BLOCK_LEN_SHORT] = p_o_buf[i + BLOCK_LEN_SHORT] * second_window[BLOCK_LEN_SHORT - i - 1];
			}
			MDCT(&hEncoder->fft_tables, p_out_mdct, 2 * BLOCK_LEN_SHORT);
			p_out_mdct += BLOCK_LEN_SHORT;
			p_o_buf += BLOCK_LEN_SHORT;
			first_window = second_window;
		}
		break;
	}

	if (transf_buf) FreeMemory(transf_buf);
}

void IFilterBank(faacEncHandle hEncoder,
	CoderInfo *coderInfo,
	double *p_in_data,
	double *p_out_data,
	double *p_overlap,
	int overlap_select)
{
	double *o_buf, *transf_buf, *overlap_buf;
	double *first_window, *second_window;

	double  *fp;
	int k, i;
	int block_type = coderInfo->block_type;

	transf_buf = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));
	overlap_buf = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));

	/*  Window shape processing */
	if (overlap_select != MNON_OVERLAPPED) {
		//      switch (coderInfo->prev_window_shape){
		//      case SINE_WINDOW:
		if ((block_type == ONLY_LONG_WINDOW) || (block_type == LONG_SHORT_WINDOW))
			first_window = hEncoder->sin_window_long;
		else
			first_window = hEncoder->sin_window_short;
		//          break;
		//      case KBD_WINDOW:
		//          if ( (block_type == ONLY_LONG_WINDOW) || (block_type == LONG_SHORT_WINDOW))
		//              first_window = hEncoder->kbd_window_long;
		//          else
		//              first_window = hEncoder->kbd_window_short;
		//          break;
		//      }

		//      switch (coderInfo->window_shape){
		//      case SINE_WINDOW:
		if ((block_type == ONLY_LONG_WINDOW) || (block_type == SHORT_LONG_WINDOW))
			second_window = hEncoder->sin_window_long;
		else
			second_window = hEncoder->sin_window_short;
		//          break;
		//      case KBD_WINDOW:
		//          if ( (block_type == ONLY_LONG_WINDOW) || (block_type == SHORT_LONG_WINDOW))
		//              second_window = hEncoder->kbd_window_long;
		//          else
		//              second_window = hEncoder->kbd_window_short;
		//          break;
		//      }
	}
	else {
		/* Always long block and sine window for LTP */
		first_window = hEncoder->sin_window_long;
		second_window = hEncoder->sin_window_long;
	}

	/* Assemble overlap buffer */
	memcpy(overlap_buf, p_overlap, BLOCK_LEN_LONG * sizeof(double));
	o_buf = overlap_buf;

	/* Separate action for each Block Type */
	switch (block_type) {
	case ONLY_LONG_WINDOW:
		memcpy(transf_buf, p_in_data, BLOCK_LEN_LONG * sizeof(double));
		IMDCT(&hEncoder->fft_tables, transf_buf, 2 * BLOCK_LEN_LONG);
		for (i = 0; i < BLOCK_LEN_LONG; i++)
			transf_buf[i] *= first_window[i];
		if (overlap_select != MNON_OVERLAPPED) {
			for (i = 0; i < BLOCK_LEN_LONG; i++) {
				o_buf[i] += transf_buf[i];
				o_buf[i + BLOCK_LEN_LONG] = transf_buf[i + BLOCK_LEN_LONG] * second_window[BLOCK_LEN_LONG - i - 1];
			}
		}
		else { /* overlap_select == NON_OVERLAPPED */
			for (i = 0; i < BLOCK_LEN_LONG; i++)
				transf_buf[i + BLOCK_LEN_LONG] *= second_window[BLOCK_LEN_LONG - i - 1];
		}
		break;

	case LONG_SHORT_WINDOW:
		memcpy(transf_buf, p_in_data, BLOCK_LEN_LONG * sizeof(double));
		IMDCT(&hEncoder->fft_tables, transf_buf, 2 * BLOCK_LEN_LONG);
		for (i = 0; i < BLOCK_LEN_LONG; i++)
			transf_buf[i] *= first_window[i];
		if (overlap_select != MNON_OVERLAPPED) {
			for (i = 0; i < BLOCK_LEN_LONG; i++)
				o_buf[i] += transf_buf[i];
			memcpy(o_buf + BLOCK_LEN_LONG, transf_buf + BLOCK_LEN_LONG, NFLAT_LS * sizeof(double));
			for (i = 0; i < BLOCK_LEN_SHORT; i++)
				o_buf[i + BLOCK_LEN_LONG + NFLAT_LS] = transf_buf[i + BLOCK_LEN_LONG + NFLAT_LS] * second_window[BLOCK_LEN_SHORT - i - 1];
			SetMemory(o_buf + BLOCK_LEN_LONG + NFLAT_LS + BLOCK_LEN_SHORT, 0, NFLAT_LS * sizeof(double));
		}
		else { /* overlap_select == NON_OVERLAPPED */
			for (i = 0; i < BLOCK_LEN_SHORT; i++)
				transf_buf[i + BLOCK_LEN_LONG + NFLAT_LS] *= second_window[BLOCK_LEN_SHORT - i - 1];
			SetMemory(transf_buf + BLOCK_LEN_LONG + NFLAT_LS + BLOCK_LEN_SHORT, 0, NFLAT_LS * sizeof(double));
		}
		break;

	case SHORT_LONG_WINDOW:
		memcpy(transf_buf, p_in_data, BLOCK_LEN_LONG * sizeof(double));
		IMDCT(&hEncoder->fft_tables, transf_buf, 2 * BLOCK_LEN_LONG);
		for (i = 0; i < BLOCK_LEN_SHORT; i++)
			transf_buf[i + NFLAT_LS] *= first_window[i];
		if (overlap_select != MNON_OVERLAPPED) {
			for (i = 0; i < BLOCK_LEN_SHORT; i++)
				o_buf[i + NFLAT_LS] += transf_buf[i + NFLAT_LS];
			memcpy(o_buf + BLOCK_LEN_SHORT + NFLAT_LS, transf_buf + BLOCK_LEN_SHORT + NFLAT_LS, NFLAT_LS * sizeof(double));
			for (i = 0; i < BLOCK_LEN_LONG; i++)
				o_buf[i + BLOCK_LEN_LONG] = transf_buf[i + BLOCK_LEN_LONG] * second_window[BLOCK_LEN_LONG - i - 1];
		}
		else { /* overlap_select == NON_OVERLAPPED */
			SetMemory(transf_buf, 0, NFLAT_LS * sizeof(double));
			for (i = 0; i < BLOCK_LEN_LONG; i++)
				transf_buf[i + BLOCK_LEN_LONG] *= second_window[BLOCK_LEN_LONG - i - 1];
		}
		break;

	case ONLY_SHORT_WINDOW:
		if (overlap_select != MNON_OVERLAPPED) {
			fp = o_buf + NFLAT_LS;
		}
		else { /* overlap_select == NON_OVERLAPPED */
			fp = transf_buf;
		}
		for (k = 0; k < MAX_SHORT_WINDOWS; k++) {
			memcpy(transf_buf, p_in_data, BLOCK_LEN_SHORT * sizeof(double));
			IMDCT(&hEncoder->fft_tables, transf_buf, 2 * BLOCK_LEN_SHORT);
			p_in_data += BLOCK_LEN_SHORT;
			if (overlap_select != MNON_OVERLAPPED) {
				for (i = 0; i < BLOCK_LEN_SHORT; i++) {
					transf_buf[i] *= first_window[i];
					fp[i] += transf_buf[i];
					fp[i + BLOCK_LEN_SHORT] = transf_buf[i + BLOCK_LEN_SHORT] * second_window[BLOCK_LEN_SHORT - i - 1];
				}
				fp += BLOCK_LEN_SHORT;
			}
			else { /* overlap_select == NON_OVERLAPPED */
				for (i = 0; i < BLOCK_LEN_SHORT; i++) {
					fp[i] *= first_window[i];
					fp[i + BLOCK_LEN_SHORT] *= second_window[BLOCK_LEN_SHORT - i - 1];
				}
				fp += 2 * BLOCK_LEN_SHORT;
			}
			first_window = second_window;
		}
		SetMemory(o_buf + BLOCK_LEN_LONG + NFLAT_LS + BLOCK_LEN_SHORT, 0, NFLAT_LS * sizeof(double));
		break;
	}

	if (overlap_select != MNON_OVERLAPPED)
		memcpy(p_out_data, o_buf, BLOCK_LEN_LONG * sizeof(double));
	else  /* overlap_select == NON_OVERLAPPED */
		memcpy(p_out_data, transf_buf, 2 * BLOCK_LEN_LONG * sizeof(double));

	/* save unused output data */
	memcpy(p_overlap, o_buf + BLOCK_LEN_LONG, BLOCK_LEN_LONG * sizeof(double));

	if (overlap_buf) FreeMemory(overlap_buf);
	if (transf_buf) FreeMemory(transf_buf);
}

void specFilter(double *freqBuff,
	int sampleRate,
	int lowpassFreq,
	int specLen
)
{
	int lowpass, xlowpass;

	/* calculate the last line which is not zero */
	lowpass = (lowpassFreq * specLen) / (sampleRate >> 1) + 1;
	xlowpass = (lowpass < specLen) ? lowpass : specLen;

	SetMemory(freqBuff + xlowpass, 0, (specLen - xlowpass) * sizeof(double));
}

static double Izero(double x)
{
	const double IzeroEPSILON = 1E-41;  /* Max error acceptable in Izero */
	double sum, u, halfx, temp;
	int n;

	sum = u = n = 1;
	halfx = x / 2.0;
	do {
		temp = halfx / (double)n;
		n += 1;
		temp *= temp;
		u *= temp;
		sum += u;
	} while (u >= IzeroEPSILON*sum);

	return(sum);
}

static void CalculateKBDWindow(double* win, double alpha, int length)
{
	int i;
	double IBeta;
	double tmp;
	double sum = 0.0;

	alpha *= M_PI;
	IBeta = 1.0 / Izero(alpha);

	/* calculate lower half of Kaiser Bessel window */
	for (i = 0; i<(length >> 1); i++) {
		tmp = 4.0*(double)i / (double)length - 1.0;
		win[i] = Izero(alpha*sqrt(1.0 - tmp*tmp))*IBeta;
		sum += win[i];
	}

	sum = 1.0 / sum;
	tmp = 0.0;

	/* calculate lower half of window */
	for (i = 0; i<(length >> 1); i++) {
		tmp += win[i];
		win[i] = sqrt(tmp*sum);
	}
}

static void MDCT(FFT_Tables *fft_tables, double *data, int N)
{
	double *xi, *xr;
	double tempr, tempi, c, s, cold, cfreq, sfreq; /* temps for pre and post twiddle */
	double freq = TWOPI / N;
	double cosfreq8, sinfreq8;
	int i, n;

	xi = (double*)AllocMemory((N >> 2) * sizeof(double));
	xr = (double*)AllocMemory((N >> 2) * sizeof(double));

	/* prepare for recurrence relation in pre-twiddle */
	cfreq = cos(freq);
	sfreq = sin(freq);
	cosfreq8 = cos(freq * 0.125);
	sinfreq8 = sin(freq * 0.125);
	c = cosfreq8;
	s = sinfreq8;

	for (i = 0; i < (N >> 2); i++) {
		/* calculate real and imaginary parts of g(n) or G(p) */
		n = (N >> 1) - 1 - 2 * i;

		if (i < (N >> 3))
			tempr = data[(N >> 2) + n] + data[N + (N >> 2) - 1 - n]; /* use second form of e(n) for n = N / 2 - 1 - 2i */
		else
			tempr = data[(N >> 2) + n] - data[(N >> 2) - 1 - n]; /* use first form of e(n) for n = N / 2 - 1 - 2i */

		n = 2 * i;
		if (i < (N >> 3))
			tempi = data[(N >> 2) + n] - data[(N >> 2) - 1 - n]; /* use first form of e(n) for n=2i */
		else
			tempi = data[(N >> 2) + n] + data[N + (N >> 2) - 1 - n]; /* use second form of e(n) for n=2i*/

																	 /* calculate pre-twiddled FFT input */
		xr[i] = tempr * c + tempi * s;
		xi[i] = tempi * c - tempr * s;

		/* use recurrence to prepare cosine and sine for next value of i */
		cold = c;
		c = c * cfreq - s * sfreq;
		s = s * cfreq + cold * sfreq;
	}

	/* Perform in-place complex FFT of length N/4 */
	switch (N) {
	case BLOCK_LEN_SHORT * 2:
		fft(fft_tables, xr, xi, 6);
		break;
	case BLOCK_LEN_LONG * 2:
		fft(fft_tables, xr, xi, 9);
	}

	/* prepare for recurrence relations in post-twiddle */
	c = cosfreq8;
	s = sinfreq8;

	/* post-twiddle FFT output and then get output data */
	for (i = 0; i < (N >> 2); i++) {
		/* get post-twiddled FFT output  */
		tempr = 2. * (xr[i] * c + xi[i] * s);
		tempi = 2. * (xi[i] * c - xr[i] * s);

		/* fill in output values */
		data[2 * i] = -tempr;   /* first half even */
		data[(N >> 1) - 1 - 2 * i] = tempi;  /* first half odd */
		data[(N >> 1) + 2 * i] = -tempi;  /* second half even */
		data[N - 1 - 2 * i] = tempr;  /* second half odd */

									  /* use recurrence to prepare cosine and sine for next value of i */
		cold = c;
		c = c * cfreq - s * sfreq;
		s = s * cfreq + cold * sfreq;
	}

	if (xr) FreeMemory(xr);
	if (xi) FreeMemory(xi);
}

static void IMDCT(FFT_Tables *fft_tables, double *data, int N)
{
	double *xi, *xr;
	double tempr, tempi, c, s, cold, cfreq, sfreq; /* temps for pre and post twiddle */
	double freq = 2.0 * M_PI / N;
	double fac, cosfreq8, sinfreq8;
	int i;

	xi = (double*)AllocMemory((N >> 2) * sizeof(double));
	xr = (double*)AllocMemory((N >> 2) * sizeof(double));

	/* Choosing to allocate 2/N factor to Inverse Xform! */
	fac = 2. / N; /* remaining 2/N from 4/N IFFT factor */

				  /* prepare for recurrence relation in pre-twiddle */
	cfreq = cos(freq);
	sfreq = sin(freq);
	cosfreq8 = cos(freq * 0.125);
	sinfreq8 = sin(freq * 0.125);
	c = cosfreq8;
	s = sinfreq8;

	for (i = 0; i < (N >> 2); i++) {
		/* calculate real and imaginary parts of g(n) or G(p) */
		tempr = -data[2 * i];
		tempi = data[(N >> 1) - 1 - 2 * i];

		/* calculate pre-twiddled FFT input */
		xr[i] = tempr * c - tempi * s;
		xi[i] = tempi * c + tempr * s;

		/* use recurrence to prepare cosine and sine for next value of i */
		cold = c;
		c = c * cfreq - s * sfreq;
		s = s * cfreq + cold * sfreq;
	}

	/* Perform in-place complex IFFT of length N/4 */
	switch (N) {
	case BLOCK_LEN_SHORT * 2:
		ffti(fft_tables, xr, xi, 6);
		break;
	case BLOCK_LEN_LONG * 2:
		ffti(fft_tables, xr, xi, 9);
	}

	/* prepare for recurrence relations in post-twiddle */
	c = cosfreq8;
	s = sinfreq8;

	/* post-twiddle FFT output and then get output data */
	for (i = 0; i < (N >> 2); i++) {

		/* get post-twiddled FFT output  */
		tempr = fac * (xr[i] * c - xi[i] * s);
		tempi = fac * (xi[i] * c + xr[i] * s);

		/* fill in output values */
		data[(N >> 1) + (N >> 2) - 1 - 2 * i] = tempr;
		if (i < (N >> 3))
			data[(N >> 1) + (N >> 2) + 2 * i] = tempr;
		else
			data[2 * i - (N >> 2)] = -tempr;

		data[(N >> 2) + 2 * i] = tempi;
		if (i < (N >> 3))
			data[(N >> 2) - 1 - 2 * i] = -tempi;
		else
			data[(N >> 2) + N - 1 - 2 * i] = tempi;

		/* use recurrence to prepare cosine and sine for next value of i */
		cold = c;
		c = c * cfreq - s * sfreq;
		s = s * cfreq + cold * sfreq;
	}

	if (xr) FreeMemory(xr);
	if (xi) FreeMemory(xi);
}


#if FAAC_RELEASE
static char *libfaacName = FAAC_VERSION;
#else
static char *libfaacName = FAAC_VERSION ".1 (" __DATE__ ") UNSTABLE";
#endif
static char *libCopyright =
"FAAC - Freeware Advanced Audio Coder (http://www.audiocoding.com/)\n"
" Copyright (C) 1999,2000,2001  Menno Bakker\n"
" Copyright (C) 2002,2003  Krzysztof Nikiel\n"
"This software is based on the ISO MPEG-4 reference source code.\n";

static const psymodellist_t psymodellist[] = {
	{ &psymodel2, "knipsycho psychoacoustic" },
	{ NULL }
};

/* Scalefactorband data table for 1024 transform length */
static SR_INFO s_srInfo[12 + 1] =
{
	{ 96000, 41, 12,
	{
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
		36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	},{
		4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 36
	}
	},{ 88200, 41, 12,
	{
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 16, 16, 24, 28,
		36, 44, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	},{
		4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 36
	}
	},{ 64000, 47, 12,
	{
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		8, 8, 8, 8, 12, 12, 12, 16, 16, 16, 20, 24, 24, 28,
		36, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
		40, 40, 40, 40, 40
	},{
		4, 4, 4, 4, 4, 4, 8, 8, 8, 16, 28, 32
	}
	},{ 48000, 49, 14,
	{
		4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
		12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 96
	},{
		4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16
	}
	},{ 44100, 49, 14,
	{
		4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
		12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 96
	},{
		4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 12, 16, 16, 16
	}
	},{ 32000, 51, 14,
	{
		4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,
		8,  8,  8,  12, 12, 12, 12, 16, 16, 20, 20, 24, 24, 28,
		28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32, 32
	},{
		4,  4,  4,  4,  4,  8,  8,  8,  12, 12, 12, 16, 16, 16
	}
	},{ 24000, 47, 15,
	{
		4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
		8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
		36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 64
	},{
		4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 20
	}
	},{ 22050, 47, 15,
	{
		4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  8,  8,  8,  8,  8,  8,  8,
		8,  8,  8,  12, 12, 12, 12, 16, 16, 16, 20, 20, 24, 24, 28, 28, 32,
		36, 36, 40, 44, 48, 52, 52, 64, 64, 64, 64, 64
	},{
		4,  4,  4,  4,  4,  4,  4,  8,  8,  8, 12, 12, 16, 16, 20
	}
	},{ 16000, 43, 15,
	{
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
		24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
	},{
		4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
	}
	},{ 12000, 43, 15,
	{
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
		24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
	},{
		4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
	}
	},{ 11025, 43, 15,
	{
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 20, 20, 20, 24,
		24, 28, 28, 32, 36, 40, 40, 44, 48, 52, 56, 60, 64, 64, 64
	},{
		4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 12, 12, 16, 20, 20
	}
	},{ 8000, 40, 15,
	{
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16,
		16, 16, 16, 16, 16, 16, 20, 20, 20, 20, 24, 24, 24, 28,
		28, 32, 36, 36, 40, 44, 48, 52, 56, 60, 64, 80
	},{
		4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 12, 16, 20, 20
	}
	},
	{ UINT32_MAX }
};

// base bandwidth for q=100
static const int bwbase = 16000;
// bandwidth multiplier (for quantiser)
static const int bwmult = 120;
// max bandwidth/samplerate ratio
static const double bwfac = 0.45;


int  faacEncGetVersion(char **faac_id_string,
	char **faac_copyright_string)
{
	if (faac_id_string)
		*faac_id_string = libfaacName;

	if (faac_copyright_string)
		*faac_copyright_string = libCopyright;

	return FAAC_CFG_VERSION;
}


int  faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder, uint8_t** ppBuffer, uint32_t* pSizeOfDecoderSpecificInfo)
{
	BitStream* pBitStream = NULL;

	if ((hEncoder == NULL) || (ppBuffer == NULL) || (pSizeOfDecoderSpecificInfo == NULL)) {
		return -1;
	}

	if (hEncoder->config.mpegVersion == MPEG2) {
		return -2; /* not supported */
	}

	*pSizeOfDecoderSpecificInfo = 2;
	*ppBuffer = (uint8_t*)malloc(2);

	if (*ppBuffer != NULL) {

		memset(*ppBuffer, 0, *pSizeOfDecoderSpecificInfo);
		pBitStream = OpenBitStream(*pSizeOfDecoderSpecificInfo, *ppBuffer);
		PutBit(pBitStream, hEncoder->config.aacObjectType, 5);
		PutBit(pBitStream, hEncoder->sampleRateIdx, 4);
		PutBit(pBitStream, hEncoder->numChannels, 4);
		CloseBitStream(pBitStream);

		return 0;
	}
	else {
		return -3;
	}
}


faacEncConfigurationPtr  faacEncGetCurrentConfiguration(faacEncHandle hEncoder)
{
	faacEncConfigurationPtr config = &(hEncoder->config);

	return config;
}

int  faacEncSetConfiguration(faacEncHandle hEncoder,
	faacEncConfigurationPtr config)
{
	int i;

	hEncoder->config.allowMidside = config->allowMidside;
	hEncoder->config.useLfe = config->useLfe;
	hEncoder->config.useTns = config->useTns;
	hEncoder->config.aacObjectType = config->aacObjectType;
	hEncoder->config.mpegVersion = config->mpegVersion;
	hEncoder->config.outputFormat = config->outputFormat;
	hEncoder->config.inputFormat = config->inputFormat;
	hEncoder->config.shortctl = config->shortctl;

	assert((hEncoder->config.outputFormat == 0) || (hEncoder->config.outputFormat == 1));

	switch (hEncoder->config.inputFormat)
	{
	case FAAC_INPUT_16BIT:
		//case FAAC_INPUT_24BIT:
	case FAAC_INPUT_32BIT:
	case FAAC_INPUT_FLOAT:
		break;

	default:
		return 0;
		break;
	}

	/* No SSR supported for now */
	if (hEncoder->config.aacObjectType == SSR)
		return 0;

	/* LTP only with MPEG4 */
	if ((hEncoder->config.aacObjectType == LTP) && (hEncoder->config.mpegVersion != MPEG4))
		return 0;

	/* Re-init TNS for new profile */
	TnsInit(hEncoder);

	/* Check for correct bitrate */
	if (config->bitRate > MaxBitrate(hEncoder->sampleRate))
		return 0;
#if 0
	if (config->bitRate < MinBitrate())
		return 0;
#endif

	if (config->bitRate && !config->bandWidth)
	{
		static struct {
			int rate; // per channel at 44100 sampling frequency
			int cutoff;
		}	rates[] = {

			{ 29500, 5000 },
			{ 37500, 7000 },
			{ 47000, 10000 },
			{ 64000, 16000 },
			{ 76000, 20000 },

			{ 0, 0 }
		};

		int f0, f1;
		int r0, r1;

		double tmpbitRate = (double)config->bitRate * 44100 / hEncoder->sampleRate;

		config->quantqual = 100;

		f0 = f1 = rates[0].cutoff;
		r0 = r1 = rates[0].rate;

		for (i = 0; rates[i].rate; i++)
		{
			f0 = f1;
			f1 = rates[i].cutoff;
			r0 = r1;
			r1 = rates[i].rate;
			if (rates[i].rate >= tmpbitRate)
				break;
		}

		if (tmpbitRate > r1)
			tmpbitRate = r1;
		if (tmpbitRate < r0)
			tmpbitRate = r0;

		if (f1 > f0)
			config->bandWidth =
			pow((double)tmpbitRate / r1,
				log((double)f1 / f0) / log((double)r1 / r0)) * (double)f1;
		else
			config->bandWidth = f1;

		config->bandWidth =
			(double)config->bandWidth * hEncoder->sampleRate / 44100;
		config->bitRate = tmpbitRate * hEncoder->sampleRate / 44100;

		if (config->bandWidth > bwbase)
			config->bandWidth = bwbase;
	}

	hEncoder->config.bitRate = config->bitRate;

	if (!config->bandWidth)
	{
		config->bandWidth = (config->quantqual - 100) * bwmult + bwbase;
	}

	hEncoder->config.bandWidth = config->bandWidth;

	// check bandwidth
	if (hEncoder->config.bandWidth < 100)
		hEncoder->config.bandWidth = 100;
	if (hEncoder->config.bandWidth >(hEncoder->sampleRate / 2))
		hEncoder->config.bandWidth = hEncoder->sampleRate / 2;

	if (config->quantqual > 500)
		config->quantqual = 500;
	if (config->quantqual < 10)
		config->quantqual = 10;

	hEncoder->config.quantqual = config->quantqual;

	/* set quantization quality */
	hEncoder->aacquantCfg.quality = config->quantqual;

	// reset psymodel
	hEncoder->psymodel->PsyEnd(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels);
	if (config->psymodelidx >= (sizeof(psymodellist) / sizeof(psymodellist[0]) - 1))
		config->psymodelidx = (sizeof(psymodellist) / sizeof(psymodellist[0])) - 2;

	hEncoder->config.psymodelidx = config->psymodelidx;
	hEncoder->psymodel = psymodellist[hEncoder->config.psymodelidx].model;
	hEncoder->psymodel->PsyInit(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels,
		hEncoder->sampleRate, hEncoder->srInfo->cb_width_long,
		hEncoder->srInfo->num_cb_long, hEncoder->srInfo->cb_width_short,
		hEncoder->srInfo->num_cb_short);

	/* load channel_map */
	for (i = 0; i < 64; i++)
		hEncoder->config.channel_map[i] = config->channel_map[i];

	/* OK */
	return 1;
}

faacEncHandle  faacEncOpen(uint32_t sampleRate,
	uint32_t numChannels,
	uint32_t *inputSamples,
	uint32_t *maxOutputBytes)
{
	uint32_t channel;
	faacEncHandle hEncoder;

	*inputSamples = FRAME_LEN*numChannels;
	*maxOutputBytes = (6144 / 8)*numChannels;

	hEncoder = (faacEncStruct*)AllocMemory(sizeof(faacEncStruct));
	SetMemory(hEncoder, 0, sizeof(faacEncStruct));

	hEncoder->numChannels = numChannels;
	hEncoder->sampleRate = sampleRate;
	hEncoder->sampleRateIdx = GetSRIndex(sampleRate);

	/* Initialize variables to default values */
	hEncoder->frameNum = 0;
	hEncoder->flushFrame = 0;

	/* Default configuration */
	hEncoder->config.version = FAAC_CFG_VERSION;
	hEncoder->config.name = libfaacName;
	hEncoder->config.copyright = libCopyright;
	hEncoder->config.mpegVersion = MPEG4;
	hEncoder->config.aacObjectType = LTP;
	hEncoder->config.allowMidside = 1;
	hEncoder->config.useLfe = 1;
	hEncoder->config.useTns = 0;
	hEncoder->config.bitRate = 0; /* default bitrate / channel */
	hEncoder->config.bandWidth = bwfac * hEncoder->sampleRate;
	if (hEncoder->config.bandWidth > bwbase)
		hEncoder->config.bandWidth = bwbase;
	hEncoder->config.quantqual = 100;
	hEncoder->config.psymodellist = (psymodellist_t *)psymodellist;
	hEncoder->config.psymodelidx = 0;
	hEncoder->psymodel =
		hEncoder->config.psymodellist[hEncoder->config.psymodelidx].model;
	hEncoder->config.shortctl = SHORTCTL_NORMAL;

	/* default channel map is straight-through */
	for (channel = 0; channel < 64; channel++)
		hEncoder->config.channel_map[channel] = channel;

	/*
	by default we have to be compatible with all previous software
	which assumes that we will generate ADTS
	/AV
	*/
	hEncoder->config.outputFormat = 1;

	/*
	be compatible with software which assumes 24bit in 32bit PCM
	*/
	hEncoder->config.inputFormat = FAAC_INPUT_32BIT;

	/* find correct sampling rate depending parameters */
	hEncoder->srInfo = &s_srInfo[hEncoder->sampleRateIdx];

	for (channel = 0; channel < numChannels; channel++)
	{
		hEncoder->coderInfo[channel].prev_window_shape = SINE_WINDOW;
		hEncoder->coderInfo[channel].window_shape = SINE_WINDOW;
		hEncoder->coderInfo[channel].block_type = ONLY_LONG_WINDOW;
		hEncoder->coderInfo[channel].num_window_groups = 1;
		hEncoder->coderInfo[channel].window_group_length[0] = 1;

		/* FIXME: Use sr_idx here */
		hEncoder->coderInfo[channel].max_pred_sfb = GetMaxPredSfb(hEncoder->sampleRateIdx);

		hEncoder->sampleBuff[channel] = NULL;
		hEncoder->nextSampleBuff[channel] = NULL;
		hEncoder->next2SampleBuff[channel] = NULL;
		hEncoder->ltpTimeBuff[channel] = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));
		SetMemory(hEncoder->ltpTimeBuff[channel], 0, 2 * BLOCK_LEN_LONG * sizeof(double));
	}

	/* Initialize coder functions */
	fft_initialize(&hEncoder->fft_tables);

	hEncoder->psymodel->PsyInit(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels,
		hEncoder->sampleRate, hEncoder->srInfo->cb_width_long,
		hEncoder->srInfo->num_cb_long, hEncoder->srInfo->cb_width_short,
		hEncoder->srInfo->num_cb_short);

	FilterBankInit(hEncoder);

	TnsInit(hEncoder);

	LtpInit(hEncoder);

	PredInit(hEncoder);

	AACQuantizeInit(hEncoder->coderInfo, hEncoder->numChannels,
		&(hEncoder->aacquantCfg));



	HuffmanInit(hEncoder->coderInfo, hEncoder->numChannels);

	/* Return handle */
	return hEncoder;
}

int  faacEncClose(faacEncHandle hEncoder)
{
	uint32_t channel;

	/* Deinitialize coder functions */
	hEncoder->psymodel->PsyEnd(&hEncoder->gpsyInfo, hEncoder->psyInfo, hEncoder->numChannels);

	FilterBankEnd(hEncoder);

	LtpEnd(hEncoder);

	AACQuantizeEnd(hEncoder->coderInfo, hEncoder->numChannels,
		&(hEncoder->aacquantCfg));

	HuffmanEnd(hEncoder->coderInfo, hEncoder->numChannels);

	fft_terminate(&hEncoder->fft_tables);

	/* Free remaining buffer memory */
	for (channel = 0; channel < hEncoder->numChannels; channel++)
	{
		if (hEncoder->ltpTimeBuff[channel])
			FreeMemory(hEncoder->ltpTimeBuff[channel]);
		if (hEncoder->sampleBuff[channel])
			FreeMemory(hEncoder->sampleBuff[channel]);
		if (hEncoder->nextSampleBuff[channel])
			FreeMemory(hEncoder->nextSampleBuff[channel]);
		if (hEncoder->next2SampleBuff[channel])
			FreeMemory(hEncoder->next2SampleBuff[channel]);
		if (hEncoder->next3SampleBuff[channel])
			FreeMemory(hEncoder->next3SampleBuff[channel]);
	}

	/* Free handle */
	if (hEncoder)
		FreeMemory(hEncoder);

	return 0;
}

int  faacEncEncode(faacEncHandle hEncoder,
	int *inputBuffer,
	uint32_t samplesInput,
	uint8_t *outputBuffer,
	uint32_t bufferSize
)
{
	uint32_t channel, i;
	int sb, frameBytes;
	uint32_t offset;
	BitStream *bitStream; /* bitstream used for writing the frame to */
	TnsInfo *tnsInfo_for_LTP;
	TnsInfo *tnsDecInfo;

	/* local copy's of parameters */
	ChannelInfo *channelInfo = hEncoder->channelInfo;
	CoderInfo *coderInfo = hEncoder->coderInfo;
	uint32_t numChannels = hEncoder->numChannels;
	uint32_t sampleRate = hEncoder->sampleRate;
	uint32_t aacObjectType = hEncoder->config.aacObjectType;
	uint32_t mpegVersion = hEncoder->config.mpegVersion;
	uint32_t useLfe = hEncoder->config.useLfe;
	uint32_t useTns = hEncoder->config.useTns;
	uint32_t allowMidside = hEncoder->config.allowMidside;
	uint32_t bandWidth = hEncoder->config.bandWidth;
	uint32_t shortctl = hEncoder->config.shortctl;

	/* Increase frame number */
	hEncoder->frameNum++;

	if (samplesInput == 0)
		hEncoder->flushFrame++;

	/* After 4 flush frames all samples have been encoded,
	return 0 bytes written */
	if (hEncoder->flushFrame > 4)
		return 0;

	/* Determine the channel configuration */
	GetChannelInfo(channelInfo, numChannels, useLfe);

	/* Update current sample buffers */
	for (channel = 0; channel < numChannels; channel++)
	{
		double *tmp;

		if (hEncoder->sampleBuff[channel]) {
			for (i = 0; i < FRAME_LEN; i++) {
				hEncoder->ltpTimeBuff[channel][i] = hEncoder->sampleBuff[channel][i];
			}
		}
		if (hEncoder->nextSampleBuff[channel]) {
			for (i = 0; i < FRAME_LEN; i++) {
				hEncoder->ltpTimeBuff[channel][FRAME_LEN + i] =
					hEncoder->nextSampleBuff[channel][i];
			}
		}

		if (!hEncoder->sampleBuff[channel])
			hEncoder->sampleBuff[channel] = (double*)AllocMemory(FRAME_LEN * sizeof(double));

		tmp = hEncoder->sampleBuff[channel];

		hEncoder->sampleBuff[channel] = hEncoder->nextSampleBuff[channel];
		hEncoder->nextSampleBuff[channel] = hEncoder->next2SampleBuff[channel];
		hEncoder->next2SampleBuff[channel] = hEncoder->next3SampleBuff[channel];
		hEncoder->next3SampleBuff[channel] = tmp;

		if (samplesInput == 0)
		{
			/* start flushing*/
			for (i = 0; i < FRAME_LEN; i++)
				hEncoder->next3SampleBuff[channel][i] = 0.0;
		}
		else
		{
			int samples_per_channel = samplesInput / numChannels;

			/* handle the various input formats and channel remapping */
			switch (hEncoder->config.inputFormat)
			{
			case FAAC_INPUT_16BIT:
			{
				short *input_channel = (short*)inputBuffer + hEncoder->config.channel_map[channel];

				for (int i = 0; i < samples_per_channel; i++)
				{
					hEncoder->next3SampleBuff[channel][i] = (double)*input_channel;
					input_channel += numChannels;
				}
			}
			break;

			case FAAC_INPUT_32BIT:
			{
				int *input_channel = (int*)inputBuffer + hEncoder->config.channel_map[channel];

				for (int i = 0; i < samples_per_channel; i++)
				{
					hEncoder->next3SampleBuff[channel][i] = (1.0 / 256) * (double)*input_channel;
					input_channel += numChannels;
				}
			}
			break;

			case FAAC_INPUT_FLOAT:
			{
				float *input_channel = (float*)inputBuffer + hEncoder->config.channel_map[channel];

				for (int i = 0; i < samples_per_channel; i++)
				{
					hEncoder->next3SampleBuff[channel][i] = (double)*input_channel;
					input_channel += numChannels;
				}
			}
			break;

			default:
				return -1; /* invalid input format */
				break;
			}

			for (i = (int)(samplesInput / numChannels); i < FRAME_LEN; i++)
				hEncoder->next3SampleBuff[channel][i] = 0.0;
		}

		/* Psychoacoustics */
		/* Update buffers and run FFT on new samples */
		/* LFE psychoacoustic can run without it */
		if (!channelInfo[channel].lfe || channelInfo[channel].cpe)
		{
			hEncoder->psymodel->PsyBufferUpdate(
				&hEncoder->fft_tables,
				&hEncoder->gpsyInfo,
				&hEncoder->psyInfo[channel],
				hEncoder->next3SampleBuff[channel],
				bandWidth,
				hEncoder->srInfo->cb_width_short,
				hEncoder->srInfo->num_cb_short);
		}
	}

	if (hEncoder->frameNum <= 3) /* Still filling up the buffers */
		return 0;

	/* Psychoacoustics */
	hEncoder->psymodel->PsyCalculate(channelInfo, &hEncoder->gpsyInfo, hEncoder->psyInfo,
		hEncoder->srInfo->cb_width_long, hEncoder->srInfo->num_cb_long,
		hEncoder->srInfo->cb_width_short,
		hEncoder->srInfo->num_cb_short, numChannels);

	hEncoder->psymodel->BlockSwitch(coderInfo, hEncoder->psyInfo, numChannels);

	/* force block type */
	if (shortctl == SHORTCTL_NOSHORT)
	{
		for (channel = 0; channel < numChannels; channel++)
		{
			coderInfo[channel].block_type = ONLY_LONG_WINDOW;
		}
	}
	if (shortctl == SHORTCTL_NOLONG)
	{
		for (channel = 0; channel < numChannels; channel++)
		{
			coderInfo[channel].block_type = ONLY_SHORT_WINDOW;
		}
	}

	/* AAC Filterbank, MDCT with overlap and add */
	for (channel = 0; channel < numChannels; channel++) {
		int k;

		FilterBank(hEncoder,
			&coderInfo[channel],
			hEncoder->sampleBuff[channel],
			hEncoder->freqBuff[channel],
			hEncoder->overlapBuff[channel],
			MOVERLAPPED);

		if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
			for (k = 0; k < 8; k++) {
				specFilter(hEncoder->freqBuff[channel] + k*BLOCK_LEN_SHORT,
					sampleRate, bandWidth, BLOCK_LEN_SHORT);
			}
		}
		else {
			specFilter(hEncoder->freqBuff[channel], sampleRate,
				bandWidth, BLOCK_LEN_LONG);
		}
	}

	/* TMP: Build sfb offset table and other stuff */
	for (channel = 0; channel < numChannels; channel++) {
		channelInfo[channel].msInfo.is_present = 0;

		if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
			coderInfo[channel].max_sfb = hEncoder->srInfo->num_cb_short;
			coderInfo[channel].nr_of_sfb = hEncoder->srInfo->num_cb_short;

			coderInfo[channel].num_window_groups = 1;
			coderInfo[channel].window_group_length[0] = 8;
			coderInfo[channel].window_group_length[1] = 0;
			coderInfo[channel].window_group_length[2] = 0;
			coderInfo[channel].window_group_length[3] = 0;
			coderInfo[channel].window_group_length[4] = 0;
			coderInfo[channel].window_group_length[5] = 0;
			coderInfo[channel].window_group_length[6] = 0;
			coderInfo[channel].window_group_length[7] = 0;

			offset = 0;
			for (sb = 0; sb < coderInfo[channel].nr_of_sfb; sb++) {
				coderInfo[channel].sfb_offset[sb] = offset;
				offset += hEncoder->srInfo->cb_width_short[sb];
			}
			coderInfo[channel].sfb_offset[coderInfo[channel].nr_of_sfb] = offset;
		}
		else {
			coderInfo[channel].max_sfb = hEncoder->srInfo->num_cb_long;
			coderInfo[channel].nr_of_sfb = hEncoder->srInfo->num_cb_long;

			coderInfo[channel].num_window_groups = 1;
			coderInfo[channel].window_group_length[0] = 1;

			offset = 0;
			for (sb = 0; sb < coderInfo[channel].nr_of_sfb; sb++) {
				coderInfo[channel].sfb_offset[sb] = offset;
				offset += hEncoder->srInfo->cb_width_long[sb];
			}
			coderInfo[channel].sfb_offset[coderInfo[channel].nr_of_sfb] = offset;
		}
	}

	/* Perform TNS analysis and filtering */
	for (channel = 0; channel < numChannels; channel++) {
		if ((!channelInfo[channel].lfe) && (useTns)) {
			TnsEncode(&(coderInfo[channel].tnsInfo),
				coderInfo[channel].max_sfb,
				coderInfo[channel].max_sfb,
				(WINDOW_TYPE)coderInfo[channel].block_type,
				coderInfo[channel].sfb_offset,
				hEncoder->freqBuff[channel]);
		}
		else {
			coderInfo[channel].tnsInfo.tnsDataPresent = 0;      /* TNS not used for LFE */
		}
	}

	for (channel = 0; channel < numChannels; channel++)
	{
		if ((coderInfo[channel].tnsInfo.tnsDataPresent != 0) && (useTns))
			tnsInfo_for_LTP = &(coderInfo[channel].tnsInfo);
		else
			tnsInfo_for_LTP = NULL;

		if (channelInfo[channel].present && (!channelInfo[channel].lfe) &&
			(coderInfo[channel].block_type != ONLY_SHORT_WINDOW) &&
			(mpegVersion == MPEG4) && (aacObjectType == LTP))
		{
			LtpEncode(hEncoder,
				&coderInfo[channel],
				&(coderInfo[channel].ltpInfo),
				tnsInfo_for_LTP,
				hEncoder->freqBuff[channel],
				hEncoder->ltpTimeBuff[channel]);
		}
		else {
			coderInfo[channel].ltpInfo.global_pred_flag = 0;
		}
	}

	for (channel = 0; channel < numChannels; channel++)
	{
		if ((aacObjectType == MAIN) && (!channelInfo[channel].lfe)) {
			int numPredBands = min(coderInfo[channel].max_pred_sfb, coderInfo[channel].nr_of_sfb);
			PredCalcPrediction(hEncoder->freqBuff[channel],
				coderInfo[channel].requantFreq,
				coderInfo[channel].block_type,
				numPredBands,
				(coderInfo[channel].block_type == ONLY_SHORT_WINDOW) ?
				hEncoder->srInfo->cb_width_short : hEncoder->srInfo->cb_width_long,
				coderInfo,
				channelInfo,
				channel);
		}
		else {
			coderInfo[channel].pred_global_flag = 0;
		}
	}

	for (channel = 0; channel < numChannels; channel++) {
		if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
			SortForGrouping(&coderInfo[channel],
				&hEncoder->psyInfo[channel],
				&channelInfo[channel],
				hEncoder->srInfo->cb_width_short,
				hEncoder->freqBuff[channel]);
		}
		CalcAvgEnrg(&coderInfo[channel], hEncoder->freqBuff[channel]);

		// reduce LFE bandwidth
		if (!channelInfo[channel].cpe && channelInfo[channel].lfe)
		{
			coderInfo[channel].nr_of_sfb = coderInfo[channel].max_sfb = 3;
		}
	}

	MSEncode(coderInfo, channelInfo, hEncoder->freqBuff, numChannels, allowMidside);

	for (channel = 0; channel < numChannels; channel++)
	{
		CalcAvgEnrg(&coderInfo[channel], hEncoder->freqBuff[channel]);
	}

					   /* Quantize and code the signal */
		for (channel = 0; channel < numChannels; channel++) {
			if (coderInfo[channel].block_type == ONLY_SHORT_WINDOW) {
				AACQuantize(&coderInfo[channel], &hEncoder->psyInfo[channel],
					&channelInfo[channel], hEncoder->srInfo->cb_width_short,
					hEncoder->srInfo->num_cb_short, hEncoder->freqBuff[channel],
					&(hEncoder->aacquantCfg));
			}
			else {
				AACQuantize(&coderInfo[channel], &hEncoder->psyInfo[channel],
					&channelInfo[channel], hEncoder->srInfo->cb_width_long,
					hEncoder->srInfo->num_cb_long, hEncoder->freqBuff[channel],
					&(hEncoder->aacquantCfg));
			}
		}

	// fix max_sfb in CPE mode
	for (channel = 0; channel < numChannels; channel++)
	{
		if (channelInfo[channel].present
			&& (channelInfo[channel].cpe)
			&& (channelInfo[channel].ch_is_left))
		{
			CoderInfo *cil, *cir;

			cil = &coderInfo[channel];
			cir = &coderInfo[channelInfo[channel].paired_ch];

			cil->max_sfb = cir->max_sfb = max(cil->max_sfb, cir->max_sfb);
			cil->nr_of_sfb = cir->nr_of_sfb = cil->max_sfb;
		}
	}

	MSReconstruct(coderInfo, channelInfo, numChannels);

	for (channel = 0; channel < numChannels; channel++)
	{
		/* If short window, reconstruction not needed for prediction */
		if ((coderInfo[channel].block_type == ONLY_SHORT_WINDOW)) {
			int sind;
			for (sind = 0; sind < BLOCK_LEN_LONG; sind++) {
				coderInfo[channel].requantFreq[sind] = 0.0;
			}
		}
		else {

			if ((coderInfo[channel].tnsInfo.tnsDataPresent != 0) && (useTns))
				tnsDecInfo = &(coderInfo[channel].tnsInfo);
			else
				tnsDecInfo = NULL;

			if ((!channelInfo[channel].lfe) && (aacObjectType == LTP)) {  /* no reconstruction needed for LFE channel*/

				LtpReconstruct(&coderInfo[channel], &(coderInfo[channel].ltpInfo),
					coderInfo[channel].requantFreq);

				if (tnsDecInfo != NULL)
					TnsDecodeFilterOnly(&(coderInfo[channel].tnsInfo), coderInfo[channel].nr_of_sfb,
						coderInfo[channel].max_sfb, (WINDOW_TYPE)coderInfo[channel].block_type,
						coderInfo[channel].sfb_offset, coderInfo[channel].requantFreq);

				IFilterBank(hEncoder, &coderInfo[channel],
					coderInfo[channel].requantFreq,
					coderInfo[channel].ltpInfo.time_buffer,
					coderInfo[channel].ltpInfo.ltp_overlap_buffer,
					MOVERLAPPED);

				LtpUpdate(&(coderInfo[channel].ltpInfo),
					coderInfo[channel].ltpInfo.time_buffer,
					coderInfo[channel].ltpInfo.ltp_overlap_buffer,
					BLOCK_LEN_LONG);
			}
		}
	}

	/* Write the AAC bitstream */
	bitStream = OpenBitStream(bufferSize, outputBuffer);

	WriteBitstream(hEncoder, coderInfo, channelInfo, bitStream, numChannels);

	/* Close the bitstream and return the number of bytes written */
	frameBytes = CloseBitStream(bitStream);

	/* Adjust quality to get correct average bitrate */
	if (hEncoder->config.bitRate)
	{
		double fix;
		int desbits = numChannels * (hEncoder->config.bitRate * FRAME_LEN)
			/ hEncoder->sampleRate;
		int diff = (frameBytes * 8) - desbits;

		hEncoder->bitDiff += diff;
		fix = (double)hEncoder->bitDiff / desbits;
		fix *= 0.01;
		fix = max(fix, -0.2);
		fix = min(fix, 0.2);

		if (((diff > 0) && (fix > 0.0)) || ((diff < 0) && (fix < 0.0)))
		{
			hEncoder->aacquantCfg.quality *= (1.0 - fix);
			if (hEncoder->aacquantCfg.quality > 300)
				hEncoder->aacquantCfg.quality = 300;
			if (hEncoder->aacquantCfg.quality < 50)
				hEncoder->aacquantCfg.quality = 50;
		}
	}

	return frameBytes;
}






void HuffmanInit(CoderInfo *coderInfo, uint32_t numChannels)
{
	uint32_t channel;

	for (channel = 0; channel < numChannels; channel++) {
		coderInfo[channel].data = (int*)AllocMemory(5 * FRAME_LEN * sizeof(int));
		coderInfo[channel].len = (int*)AllocMemory(5 * FRAME_LEN * sizeof(int));

	}
}

void HuffmanEnd(CoderInfo *coderInfo, uint32_t numChannels)
{
	uint32_t channel;

	for (channel = 0; channel < numChannels; channel++) {
		if (coderInfo[channel].data) FreeMemory(coderInfo[channel].data);
		if (coderInfo[channel].len) FreeMemory(coderInfo[channel].len);

	}
}

int BitSearch(CoderInfo *coderInfo,
	int *quant)  /* Quantized spectral values */
				 /*
				 This function inputs a vector of quantized spectral data, quant[][], and returns a vector,
				 'book_vector[]' that describes how to group together the scalefactor bands into a smaller
				 number of sections.  There are MAX_SCFAC_BANDS elements in book_vector (equal to 49 in the
				 case of long blocks and 112 for short blocks), and each element has a huffman codebook
				 number assigned to it.

				 For a quick and simple algorithm, this function performs a binary
				 search across the sfb's (scale factor bands).  On the first approach, it calculates the
				 needed amount of bits if every sfb were its own section and transmitted its own huffman
				 codebook value side information (equal to 9 bits for a long block, 7 for a short).  The
				 next iteration combines adjacent sfb's, and calculates the bit rate for length two sfb
				 sections.  If any wider two-sfb section requires fewer bits than the sum of the two
				 single-sfb sections (below it in the binary tree), then the wider section will be chosen.
				 This process occurs until the sections are split into three uniform parts, each with an
				 equal amount of sfb's contained.

				 The binary tree is stored as a two-dimensional array.  Since this tree is not full, (there
				 are only 49 nodes, not 2^6 = 64), the numbering is a little complicated.  If the tree were
				 full, the top node would be 1.  It's children would be 2 and 3.  But, since this tree
				 is not full, the top row of three nodes are numbered {4,5,6}.  The row below it is
				 {8,9,10,11,12,13}, and so on.

				 The binary tree is called bit_stats[112][3].  There are 112 total nodes (some are not
				 used since it's not full).  bit_stats[x][0] holds the bit totals needed for the sfb sectioning
				 strategy represented by the node x in the tree.  bit_stats[x][1] holds the optimal huffman
				 codebook table that minimizes the bit rate, given the sectioning boundaries dictated by node x.
				 */

{
	int i, j, k;
	int hop;
	int min_book_choice[112][3];
	int bit_stats[240][3];
	int total_bit_count;
	int levels;
	int pow2levels;
	int fracpow2lev;

	/* Set local pointer to coderInfo book_vector */
	int* book_vector = coderInfo->book_vector;

	levels = (int)((log((double)coderInfo->nr_of_sfb) / log((double)2.0)) + 1);

	/* #define SLOW */

#ifdef SLOW
	for (i = 0; i < 5; i++) {
#else
	i = 0;
#endif
	hop = 1 << i;

	NoiselessBitCount(coderInfo, quant, hop, min_book_choice);

	/* load up the (not-full) binary search tree with the min_book_choice values */
	k = 0;
	total_bit_count = 0;

	pow2levels = 1 << (levels - i);
	fracpow2lev = pow2levels + (coderInfo->nr_of_sfb >> i);

	for (j = pow2levels; j < fracpow2lev; j++)
	{
		bit_stats[j][0] = min_book_choice[k][0]; /* the minimum bit cost for this section */
		bit_stats[j][1] = min_book_choice[k][1]; /* used with this huffman book number */

#ifdef SLOW
		if (i>0) {  /* not on the lowest level, grouping more than one signle scalefactor band per section*/
			if (bit_stats[j][0] < bit_stats[2 * j][0] + bit_stats[2 * j + 1][0]) {

				/* it is cheaper to combine surrounding sfb secionts into one larger huffman book section */
				for (n = k; n<k + hop; n++) { /* write the optimal huffman book value for the new larger section */
					if ((book_vector[n] != INTENSITY_HCB) && (book_vector[n] != INTENSITY_HCB2)) { /* Don't merge with IS bands */
						book_vector[n] = bit_stats[j][1];
					}
				}
			}
			else {  /* it was cheaper to transmit the smaller huffman table sections */
				bit_stats[j][0] = bit_stats[2 * j][0] + bit_stats[2 * j + 1][0];
			}
		}
		else
#endif
		{  /* during the first stage of the iteration, all sfb's are individual sections */
			if ((book_vector[k] != INTENSITY_HCB) && (book_vector[k] != INTENSITY_HCB2)) {
				book_vector[k] = bit_stats[j][1];  /* initially, set all sfb's to their own optimal section table values */
			}
		}
		total_bit_count = total_bit_count + bit_stats[j][0];
		k = k + hop;
	}
#ifdef SLOW
	}
#endif
/*   book_vector[k] = book_vector[k-1]; */
return(total_bit_count);
}


int NoiselessBitCount(CoderInfo *coderInfo,
	int *quant,
	int hop,
	int min_book_choice[112][3])
{
	int i, j, k;

	/*
	This function inputs:
	- the quantized spectral data, 'quant[]';
	- all of the huffman codebooks, 'huff[][]';
	- the size of the sections, in scalefactor bands (SFB's), 'hop';
	- an empty matrix, min_book_choice[][] passed to it;

	This function outputs:
	- the matrix, min_book_choice.  It is a two dimensional matrix, with its
	rows corresponding to spectral sections.  The 0th column corresponds to
	the bits needed to code a section with 'hop' scalefactors bands wide, all using
	the same huffman codebook.  The 1st column contains the huffman codebook number
	that allows the minimum number of bits to be used.

	Other notes:
	- Initally, the dynamic range is calculated for each spectral section.  The section
	can only be entropy coded with books that have an equal or greater dynamic range
	than the section's spectral data.  The exception to this is for the 11th ESC codebook.
	If the dynamic range is larger than 16, then an escape code is appended after the
	table 11 codeword which encodes the larger value explicity in a pseudo-non-uniform
	quantization method.

	*/

	int max_sb_coeff;
	int book_choice[12][2];
	int total_bits_cost = 0;
	int offset, length, end;
	int q;

	/* set local pointer to sfb_offset */
	int *sfb_offset = coderInfo->sfb_offset;
	int nr_of_sfb = coderInfo->nr_of_sfb;

	/* each section is 'hop' scalefactor bands wide */
	for (i = 0; i < nr_of_sfb; i = i + hop) {
#ifdef SLOW
		if ((i + hop) > nr_of_sfb)
			q = nr_of_sfb;
		else
#endif
			q = i + hop;

		{

			/* find the maximum absolute value in the current spectral section, to see what tables are available to use */
			max_sb_coeff = 0;
			for (j = sfb_offset[i]; j<sfb_offset[q]; j++) {  /* snl */
				if (ABS(quant[j]) > max_sb_coeff)
					max_sb_coeff = ABS(quant[j]);
			}

			j = 0;
			offset = sfb_offset[i];
#ifdef SLOW
			if ((i + hop) > nr_of_sfb) {
				end = sfb_offset[nr_of_sfb];
			}
			else
#endif
				end = sfb_offset[q];
			length = end - offset;

			/* all spectral coefficients in this section are zero */
			if (max_sb_coeff == 0) {
				book_choice[j][0] = CalcBits(coderInfo, 0, quant, offset, length);
				book_choice[j++][1] = 0;

			}
			else {  /* if the section does have non-zero coefficients */
				if (max_sb_coeff < 2) {
					book_choice[j][0] = CalcBits(coderInfo, 1, quant, offset, length);
					book_choice[j++][1] = 1;
					book_choice[j][0] = CalcBits(coderInfo, 2, quant, offset, length);
					book_choice[j++][1] = 2;
					book_choice[j][0] = CalcBits(coderInfo, 3, quant, offset, length);
					book_choice[j++][1] = 3;
				}
				else if (max_sb_coeff < 3) {
					book_choice[j][0] = CalcBits(coderInfo, 3, quant, offset, length);
					book_choice[j++][1] = 3;
					book_choice[j][0] = CalcBits(coderInfo, 4, quant, offset, length);
					book_choice[j++][1] = 4;
					book_choice[j][0] = CalcBits(coderInfo, 5, quant, offset, length);
					book_choice[j++][1] = 5;
				}
				else if (max_sb_coeff < 5) {
					book_choice[j][0] = CalcBits(coderInfo, 5, quant, offset, length);
					book_choice[j++][1] = 5;
					book_choice[j][0] = CalcBits(coderInfo, 6, quant, offset, length);
					book_choice[j++][1] = 6;
					book_choice[j][0] = CalcBits(coderInfo, 7, quant, offset, length);
					book_choice[j++][1] = 7;
				}
				else if (max_sb_coeff < 8) {
					book_choice[j][0] = CalcBits(coderInfo, 7, quant, offset, length);
					book_choice[j++][1] = 7;
					book_choice[j][0] = CalcBits(coderInfo, 8, quant, offset, length);
					book_choice[j++][1] = 8;
					book_choice[j][0] = CalcBits(coderInfo, 9, quant, offset, length);
					book_choice[j++][1] = 9;
				}
				else if (max_sb_coeff < 13) {
					book_choice[j][0] = CalcBits(coderInfo, 9, quant, offset, length);
					book_choice[j++][1] = 9;
					book_choice[j][0] = CalcBits(coderInfo, 10, quant, offset, length);
					book_choice[j++][1] = 10;
				}
				/* (max_sb_coeff >= 13), choose table 11 */
				else {
					book_choice[j][0] = CalcBits(coderInfo, 11, quant, offset, length);
					book_choice[j++][1] = 11;
				}
			}

			/* find the minimum bit cost and table number for huffman coding this scalefactor section */
			min_book_choice[i][1] = book_choice[0][1];
			min_book_choice[i][0] = book_choice[0][0];

			for (k = 1; k<j; k++) {
				if (book_choice[k][0] < min_book_choice[i][0]) {
					min_book_choice[i][1] = book_choice[k][1];
					min_book_choice[i][0] = book_choice[k][0];
				}
			}
			total_bits_cost += min_book_choice[i][0];
		}
	}
	return(total_bits_cost);
}



static int CalculateEscSequence(int input, int *len_esc_sequence)
/*
This function takes an element that is larger than 16 and generates the base10 value of the
equivalent escape sequence.  It returns the escape sequence in the variable, 'output'.  It
also passed the length of the escape sequence through the parameter, 'len_esc_sequence'.
*/

{
	float x, y;
	int output;
	int N;

	N = -1;
	y = (float)ABS(input);
	x = y / 16;

	while (x >= 1) {
		N++;
		x = x / 2;
	}

	*len_esc_sequence = 2 * N + 5;  /* the length of the escape sequence in bits */

	output = (int)((pow(2, N) - 1)*pow(2, N + 5) + y - pow(2, N + 4));
	return(output);
}

int OutputBits(CoderInfo *coderInfo,
	int book,
	int *quant,
	int offset,
	int length)
{
	/*
	This function inputs
	- a specific codebook number, 'book'
	- the quantized spectral data, 'quant[][]'
	- the offset into the spectral data to begin scanning, 'offset'
	- the 'length' of the segment to huffman code
	-> therefore, the segment quant[offset] to quant[offset+length-1]
	is huffman coded.

	This function outputs
	- the number of bits required, 'bits'  using the prescribed codebook, book applied to
	the given segment of spectral data.

	There are three parameters that are passed back and forth into this function.  data[]
	and len[] are one-dimensional arrays that store the codebook values and their respective
	bit lengths.  These are used when packing the data for the bitstream in OutputBits().  The
	index into these arrays is 'coderInfo->spectral_count''.  It gets incremented internally in this
	function as counter, then passed to the outside through outside_counter.  The next time
	OutputBits() is called, counter starts at the value it left off from the previous call.

	*/
	int esc_sequence;
	int len_esc;
	int index;
	int bits = 0;
	int tmp;
	int codebook, i, j;
	int counter;

	/* Set up local pointers to coderInfo elements data and len */
	int* data = coderInfo->data;
	int* len = coderInfo->len;
	counter = coderInfo->spectral_count;

	switch (book) {
	case 0:
	case INTENSITY_HCB2:
	case INTENSITY_HCB:
			/* This case also applies to intensity stereo encoding */
			coderInfo->data[counter] = 0;
			coderInfo->len[counter++] = 0;
			coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 1:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * quant[i] + 9 * quant[i + 1] + 3 * quant[i + 2] + quant[i + 3] + 40;
			codebook = huff1[index][LASTINTAB];
			tmp = huff1[index][FIRSTINTAB];
			bits += tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 2:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * quant[i] + 9 * quant[i + 1] + 3 * quant[i + 2] + quant[i + 3] + 40;
			codebook = huff2[index][LASTINTAB];
			tmp = huff2[index][FIRSTINTAB];
			bits += tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 3:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * ABS(quant[i]) + 9 * ABS(quant[i + 1]) + 3 * ABS(quant[i + 2]) + ABS(quant[i + 3]);
			codebook = huff3[index][LASTINTAB];
			tmp = huff3[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<4; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;

				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;
					}
			}

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 4:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * ABS(quant[i]) + 9 * ABS(quant[i + 1]) + 3 * ABS(quant[i + 2]) + ABS(quant[i + 3]);
			codebook = huff4[index][LASTINTAB];
			tmp = huff4[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<4; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;

				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;

					}
			}
		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 5:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 9 * (quant[i]) + (quant[i + 1]) + 40;
			codebook = huff5[index][LASTINTAB];
			tmp = huff5[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 6:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 9 * (quant[i]) + (quant[i + 1]) + 40;
			codebook = huff6[index][LASTINTAB];
			tmp = huff6[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;
		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 7:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 8 * ABS(quant[i]) + ABS(quant[i + 1]);
			codebook = huff7[index][LASTINTAB];
			tmp = huff7[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<2; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;

				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;

					}
			}

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 8:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 8 * ABS(quant[i]) + ABS(quant[i + 1]);
			codebook = huff8[index][LASTINTAB];
			tmp = huff8[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<2; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;
				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;
					}
			}
		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 9:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 13 * ABS(quant[i]) + ABS(quant[i + 1]);
			codebook = huff9[index][LASTINTAB];
			tmp = huff9[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<2; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;

				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;
					}
			}
		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 10:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 13 * ABS(quant[i]) + ABS(quant[i + 1]);
			codebook = huff10[index][LASTINTAB];
			tmp = huff10[index][FIRSTINTAB];
			bits = bits + tmp;
			data[counter] = codebook;
			len[counter++] = tmp;

			for (j = 0; j<2; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;

				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;

					}
			}

		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	case 11:
		/* First, calculate the indecies into the huffman tables */
		for (i = offset; i<offset + length; i = i + 2) {
			if ((ABS(quant[i]) >= 16) && (ABS(quant[i + 1]) >= 16)) {  /* both codewords were above 16 */
																	   /* first, code the orignal pair, with the larger value saturated to +/- 16 */
				index = 17 * 16 + 16;
			}
			else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
											 /* first, code the orignal pair, with the larger value saturated to +/- 16 */
				index = 17 * 16 + ABS(quant[i + 1]);
			}
			else if (ABS(quant[i + 1]) >= 16) { /* the second codeword was above 16, not the first one */
				index = 17 * ABS(quant[i]) + 16;
			}
			else {  /* there were no values above 16, so no escape sequences */
				index = 17 * ABS(quant[i]) + ABS(quant[i + 1]);
			}

			/* write out the codewords */
			tmp = huff11[index][FIRSTINTAB];
			codebook = huff11[index][LASTINTAB];
			bits += tmp;
			data[counter] = codebook;
			len[counter++] = tmp;
			/* Take care of the sign bits */
			for (j = 0; j<2; j++) {
				if (quant[i + j] > 0) {  /* send out '0' if a positive value */
					data[counter] = 0;
					len[counter++] = 1;
					bits += 1;
				}
				else
					if (quant[i + j] < 0) {  /* send out '1' if a negative value */
						data[counter] = 1;
						len[counter++] = 1;
						bits += 1;

					}
			}

			/* write out the escape sequences */
			if ((ABS(quant[i]) >= 16) && (ABS(quant[i + 1]) >= 16)) {  /* both codewords were above 16 */
																	   /* code and transmit the first escape_sequence */
				esc_sequence = CalculateEscSequence(quant[i], &len_esc);
				bits += len_esc;
				data[counter] = esc_sequence;
				len[counter++] = len_esc;


				/* then code and transmit the second escape_sequence */
				esc_sequence = CalculateEscSequence(quant[i + 1], &len_esc);
				bits += len_esc;
				data[counter] = esc_sequence;
				len[counter++] = len_esc;

			}
			else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
											 /* code and transmit the escape_sequence */
				esc_sequence = CalculateEscSequence(quant[i], &len_esc);
				bits += len_esc;
				data[counter] = esc_sequence;
				len[counter++] = len_esc;

			}
			else if (ABS(quant[i + 1]) >= 16) { /* the second codeword was above 16, not the first one */
												/* code and transmit the escape_sequence */
				esc_sequence = CalculateEscSequence(quant[i + 1], &len_esc);
				bits += len_esc;
				data[counter] = esc_sequence;
				len[counter++] = len_esc;

			}
		}
		coderInfo->spectral_count = counter;  /* send the current count back to the outside world */
		return(bits);
	}
	return 0;
	}

int CalcBits(CoderInfo *coderInfo,
	int book,
	int *quant,
	int offset,
	int length)
{
	/*
	This function inputs
	- a specific codebook number, 'book'
	- the quantized spectral data, 'quant[]'
	- the offset into the spectral data to begin scanning, 'offset'
	- the 'length' of the segment to huffman code
	-> therefore, the segment quant[offset] to quant[offset+length-1]
	is huffman coded.

	This function outputs
	- the number of bits required, 'bits'  using the prescribed codebook, book applied to
	the given segment of spectral data.

	*/

	int len_esc;
	int index;
	int bits = 0;
	int i, j;

	switch (book) {
	case 1:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * quant[i] + 9 * quant[i + 1] + 3 * quant[i + 2] + quant[i + 3] + 40;
			bits += huff1[index][FIRSTINTAB];
		}
		return (bits);
	case 2:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * quant[i] + 9 * quant[i + 1] + 3 * quant[i + 2] + quant[i + 3] + 40;
			bits += huff2[index][FIRSTINTAB];
		}
		return (bits);
	case 3:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * ABS(quant[i]) + 9 * ABS(quant[i + 1]) + 3 * ABS(quant[i + 2]) + ABS(quant[i + 3]);
			bits += huff3[index][FIRSTINTAB];
			for (j = 0; j<4; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 4:
		for (i = offset; i<offset + length; i = i + 4) {
			index = 27 * ABS(quant[i]) + 9 * ABS(quant[i + 1]) + 3 * ABS(quant[i + 2]) + ABS(quant[i + 3]);
			bits += huff4[index][FIRSTINTAB];
			for (j = 0; j<4; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 5:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 9 * (quant[i]) + (quant[i + 1]) + 40;
			bits += huff5[index][FIRSTINTAB];
		}
		return (bits);
	case 6:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 9 * (quant[i]) + (quant[i + 1]) + 40;
			bits += huff6[index][FIRSTINTAB];
		}
		return (bits);
	case 7:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 8 * ABS(quant[i]) + ABS(quant[i + 1]);
			bits += huff7[index][FIRSTINTAB];
			for (j = 0; j<2; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 8:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 8 * ABS(quant[i]) + ABS(quant[i + 1]);
			bits += huff8[index][FIRSTINTAB];
			for (j = 0; j<2; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 9:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 13 * ABS(quant[i]) + ABS(quant[i + 1]);
			bits += huff9[index][FIRSTINTAB];
			for (j = 0; j<2; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 10:
		for (i = offset; i<offset + length; i = i + 2) {
			index = 13 * ABS(quant[i]) + ABS(quant[i + 1]);
			bits += huff10[index][FIRSTINTAB];
			for (j = 0; j<2; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}
		}
		return (bits);
	case 11:
		/* First, calculate the indecies into the huffman tables */
		for (i = offset; i<offset + length; i = i + 2) {
			if ((ABS(quant[i]) >= 16) && (ABS(quant[i + 1]) >= 16)) {  /* both codewords were above 16 */
																	   /* first, code the orignal pair, with the larger value saturated to +/- 16 */
				index = 17 * 16 + 16;
			}
			else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
											 /* first, code the orignal pair, with the larger value saturated to +/- 16 */
				index = 17 * 16 + ABS(quant[i + 1]);
			}
			else if (ABS(quant[i + 1]) >= 16) { /* the second codeword was above 16, not the first one */
				index = 17 * ABS(quant[i]) + 16;
			}
			else {  /* there were no values above 16, so no escape sequences */
				index = 17 * ABS(quant[i]) + ABS(quant[i + 1]);
			}

			/* write out the codewords */
			bits += huff11[index][FIRSTINTAB];

			/* Take care of the sign bits */
			for (j = 0; j<2; j++) {
				if (quant[i + j] != 0) bits += 1; /* only for non-zero spectral coefficients */
			}

			/* write out the escape sequences */
			if ((ABS(quant[i]) >= 16) && (ABS(quant[i + 1]) >= 16)) {  /* both codewords were above 16 */
																	   /* code and transmit the first escape_sequence */
				CalculateEscSequence(quant[i], &len_esc);
				bits += len_esc;

				/* then code and transmit the second escape_sequence */
				CalculateEscSequence(quant[i + 1], &len_esc);
				bits += len_esc;
			}
			else if (ABS(quant[i]) >= 16) {  /* the first codeword was above 16, not the second one */
											 /* code and transmit the escape_sequence */
				CalculateEscSequence(quant[i], &len_esc);
				bits += len_esc;
			}
			else if (ABS(quant[i + 1]) >= 16) { /* the second codeword was above 16, not the first one */
												/* code and transmit the escape_sequence */
				CalculateEscSequence(quant[i + 1], &len_esc);
				bits += len_esc;
			}
		}
		return (bits);
	}
	return 0;
}

int SortBookNumbers(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)
{
	/*
	This function inputs the vector, 'book_vector[]', which is of length MAX_SCFAC_BANDS,
	and contains the optimal huffman tables of each sfb.  It returns the vector, 'output_book_vector[]', which
	has it's elements formatted for the encoded bit stream.  It's syntax is:

	{sect_cb[0], length_segment[0], ... ,sect_cb[num_of_sections], length_segment[num_of_sections]}

	The above syntax is true, unless there is an escape sequence.  An
	escape sequence occurs when a section is longer than 2 ^ (bit_len)
	long in units of scalefactor bands.  Also, the integer returned from
	this function is the number of bits written in the bitstream,
	'bit_count'.

	This function supports both long and short blocks.
	*/

	int i;
	int repeat_counter;
	int bit_count = 0;
	int previous;
	int max, bit_len/*,sfbs*/;
	int max_sfb, g, band;
	int sect_cb_bits = 4;

	/* Set local pointers to coderInfo elements */
	int* book_vector = coderInfo->book_vector;
	if (coderInfo->block_type == ONLY_SHORT_WINDOW) {
		max = 7;
		bit_len = 3;
	}
	else {  /* the block_type is a long,start, or stop window */
		max = 31;
		bit_len = 5;
	}

	/* Compute number of scalefactor bands */
	max_sfb = coderInfo->nr_of_sfb / coderInfo->num_window_groups;


	for (g = 0; g < coderInfo->num_window_groups; g++) {
		band = g*max_sfb;

		repeat_counter = 1;

		previous = book_vector[band];
		if (writeFlag) {
			PutBit(bitStream, book_vector[band], sect_cb_bits);
		}
		bit_count += sect_cb_bits;

		for (i = band + 1; i<band + max_sfb; i++) {

				if ((book_vector[i] != previous)) {
					if (writeFlag) {
						PutBit(bitStream, repeat_counter, bit_len);
					}
					bit_count += bit_len;

					if (repeat_counter == max) {  /* in case you need to terminate an escape sequence */
						if (writeFlag)
							PutBit(bitStream, 0, bit_len);
						bit_count += bit_len;
					}

					if (writeFlag)
						PutBit(bitStream, book_vector[i], sect_cb_bits);
					bit_count += sect_cb_bits;
					previous = book_vector[i];
					repeat_counter = 1;
				}
			/* if the length of the section is longer than the amount of bits available in */
			/* the bitsream, "max", then start up an escape sequence */
				else if ((book_vector[i] == previous) && (repeat_counter == max)) {
					if (writeFlag) {
						PutBit(bitStream, repeat_counter, bit_len);
					}
					bit_count += bit_len;
					repeat_counter = 1;
				}
				else {
					repeat_counter++;
				}
		}

		{
			if (writeFlag)
				PutBit(bitStream, repeat_counter, bit_len);
			bit_count += bit_len;

			if (repeat_counter == max) {  /* special case if the last section length is an */
										  /* escape sequence */
				if (writeFlag)
					PutBit(bitStream, 0, bit_len);
				bit_count += bit_len;
			}
		}
	}  /* Bottom of group iteration */

	return bit_count;
}

int WriteScalefactors(CoderInfo *coderInfo,
	BitStream *bitStream,
	int writeFlag)

{
	/* this function takes care of counting the number of bits necessary */
	/* to encode the scalefactors.  In addition, if the writeFlag == 1, */
	/* then the scalefactors are written out the bitStream output bit */
	/* stream.  it returns k, the number of bits written to the bitstream*/

	int i, j, bit_count = 0;
	int diff, length, codeword;
	int previous_scale_factor;
	int previous_is_factor;       /* Intensity stereo */
	int index = 0;
	int nr_of_sfb_per_group;

	/* set local pointer to coderInfo elements */
	int* scale_factors = coderInfo->scale_factor;

	if (coderInfo->block_type == ONLY_SHORT_WINDOW) { /* short windows */
		nr_of_sfb_per_group = coderInfo->nr_of_sfb / coderInfo->num_window_groups;
	}
	else {
		nr_of_sfb_per_group = coderInfo->nr_of_sfb;
		coderInfo->num_window_groups = 1;
		coderInfo->window_group_length[0] = 1;
	}

	previous_scale_factor = coderInfo->global_gain;
	previous_is_factor = 0;

	for (j = 0; j<coderInfo->num_window_groups; j++) {
		for (i = 0; i<nr_of_sfb_per_group; i++) {
			/* test to see if any codebooks in a group are zero */
			if ((coderInfo->book_vector[index] == INTENSITY_HCB) ||
				(coderInfo->book_vector[index] == INTENSITY_HCB2)) {
				/* only send scalefactors if using non-zero codebooks */
				diff = scale_factors[index] - previous_is_factor;
				if ((diff < 60) && (diff >= -60))
					length = huff12[diff + 60][FIRSTINTAB];
				else length = 0;
				bit_count += length;
				previous_is_factor = scale_factors[index];
				if (writeFlag == 1) {
					codeword = huff12[diff + 60][LASTINTAB];
					PutBit(bitStream, codeword, length);
				}
			}
			else if (coderInfo->book_vector[index]) {
				/* only send scalefactors if using non-zero codebooks */
				diff = scale_factors[index] - previous_scale_factor;
				if ((diff < 60) && (diff >= -60))
					length = huff12[diff + 60][FIRSTINTAB];
				else length = 0;
				bit_count += length;
				previous_scale_factor = scale_factors[index];
				if (writeFlag == 1) {
					codeword = huff12[diff + 60][LASTINTAB];
					PutBit(bitStream, codeword, length);
				}
			}
			index++;
		}
	}
	return bit_count;
}



/* short double_to_int(double sig_in); */
#define double_to_int(sig_in) \
   ((sig_in) > 32767 ? 32767 : ( \
       (sig_in) < -32768 ? -32768 : (sig_in)))

#define _MDCT_SCALE		512

/*  Purpose:    Codebook for LTP weight coefficients.  */
static double codebook[CODESIZE] =
{
	0.570829,
	0.696616,
	0.813004,
	0.911304,
	0.984900,
	1.067894,
	1.194601,
	1.369533
};


static double snr_pred(double *mdct_in, double *mdct_pred, int *sfb_flag, int *sfb_offset,
	int block_type, int side_info, int num_of_sfb)
{
	int i, j, flen;
	double snr_limit;
	double num_bit, snr[NSFB_LONG];
	double temp1, temp2;
	double energy[BLOCK_LEN_LONG], snr_p[BLOCK_LEN_LONG];

	if (block_type != ONLY_SHORT_WINDOW)
	{
		flen = BLOCK_LEN_LONG;
		snr_limit = 1.e-30;
	}
	else {
		flen = BLOCK_LEN_SHORT;
		snr_limit = 1.e-20;
	}

	for (i = 0; i < flen; i++)
	{
		energy[i] = mdct_in[i] * mdct_in[i];
		snr_p[i] = (mdct_in[i] - mdct_pred[i]) * (mdct_in[i] - mdct_pred[i]);
	}

	num_bit = 0.0;

	for (i = 0; i < num_of_sfb; i++)
	{
		temp1 = 0.0;
		temp2 = 0.0;
		for (j = sfb_offset[i]; j < sfb_offset[i + 1]; j++)
		{
			temp1 += energy[j];
			temp2 += snr_p[j];
		}

		if (temp2 < snr_limit)
			temp2 = snr_limit;

		if (temp1 > 1.e-20)
			snr[i] = -10. * log10(temp2 / temp1);
		else
			snr[i] = 0.0;

		sfb_flag[i] = 1;

		if (block_type != ONLY_SHORT_WINDOW)
		{
			if (snr[i] <= 0.0)
			{
				sfb_flag[i] = 0;
				for (j = sfb_offset[i]; j < sfb_offset[i + 1]; j++)
					mdct_pred[j] = 0.0;
			}
			else {
				num_bit += snr[i] / 6. * (sfb_offset[i + 1] - sfb_offset[i]);
			}
		}
	}

	if (num_bit < side_info)
	{
		//      printf("LTP not used!, num_bit: %f    ", num_bit);
		num_bit = 0.0;
		for (j = 0; j < flen; j++)
			mdct_pred[j] = 0.0;
		for (i = 0; i < num_of_sfb; i++)
			sfb_flag[i] = 0;
	}
	else {
		num_bit -= side_info;
		//      printf("LTP used!, num_bit: %f    ", num_bit);
	}

	return (num_bit);
}

static void prediction(double *buffer, double *predicted_samples, double *weight, int lag,
	int flen)
{
	int i, offset;
	int num_samples;

	offset = NOK_LT_BLEN - flen / 2 - lag;

	num_samples = flen;
	if (NOK_LT_BLEN - offset < flen)
		num_samples = NOK_LT_BLEN - offset;

	for (i = 0; i < num_samples; i++)
		predicted_samples[i] = *weight * _MDCT_SCALE*buffer[offset++];
	for (; i < flen; i++)
		predicted_samples[i] = 0.0;


}

static void w_quantize(double *freq, int *ltp_idx)
{
	int i;
	double dist, low;

	low = 1.0e+10;
	dist = 0.0;
	for (i = 0; i < CODESIZE; i++)
	{
		dist = (*freq - codebook[i]) * (*freq - codebook[i]);
		if (dist < low)
		{
			low = dist;
			*ltp_idx = i;
		}
	}

	*freq = codebook[*ltp_idx];
}

static int pitch(double *sb_samples, double *x_buffer, int flen, int lag0, int lag1,
	double *predicted_samples, double *gain, int *cb_idx)
{
	int i, j, delay;
	double corr1, corr2, lag_corr;
	double p_max, energy, lag_energy;

	/*
	* Below is a figure illustrating how the lag and the
	* samples in the buffer relate to each other.
	*
	* ------------------------------------------------------------------
	* |              |               |                |                 |
	* |    slot 1    |      2        |       3        |       4         |
	* |              |               |                |                 |
	* ------------------------------------------------------------------
	*
	* lag = 0 refers to the end of slot 4 and lag = DELAY refers to the end
	* of slot 2. The start of the predicted frame is then obtained by
	* adding the length of the frame to the lag. Remember that slot 4 doesn't
	* actually exist, since it is always filled with zeros.
	*
	* The above short explanation was for long blocks. For short blocks the
	* zero lag doesn't refer to the end of slot 4 but to the start of slot
	* 4 - the frame length of a short block.
	*
	* Some extra code is then needed to handle those lag values that refer
	* to slot 4.
	*/

	p_max = 0.0;
	lag_corr = lag_energy = 0.0;
	delay = lag0;


	for (i = lag0; i<lag1; i++)
	{
		energy = 0.0;
		corr1 = 0.0;
		for (j = 0; j < flen; j++)
		{
			if (j < i + BLOCK_LEN_LONG)
			{
				corr1 += sb_samples[j] * _MDCT_SCALE * x_buffer[NOK_LT_BLEN - flen / 2 - i + j];
				energy += _MDCT_SCALE * x_buffer[NOK_LT_BLEN - flen / 2 - i + j] * _MDCT_SCALE * x_buffer[NOK_LT_BLEN - flen / 2 - i + j];
			}
		}
		if (energy != 0.0)
			corr2 = corr1 / sqrt(energy);
		else
			corr2 = 0.0;

		if (p_max < corr2)
		{
			p_max = corr2;
			delay = i;
			lag_corr = corr1;
			lag_energy = energy;
		}
	}
	/* Compute the gain. */
	if (lag_energy != 0.0)
		*gain = lag_corr / (1.010 * lag_energy);
	else
		*gain = 0.0;

	/* Quantize the gain. */
	w_quantize(gain, cb_idx);
	//  printf("Delay: %d, Coeff: %f", delay, *gain);

	/* Get the predicted signal. */
	prediction(x_buffer, predicted_samples, gain, delay, flen);


	return (delay);
}

static double ltp_enc_tf(faacEncHandle hEncoder,
	CoderInfo *coderInfo, double *p_spectrum, double *predicted_samples,
	double *mdct_predicted, int *sfb_offset,
	int num_of_sfb, int last_band, int side_info,
	int *sfb_prediction_used, TnsInfo *tnsInfo)
{
	double bit_gain;

	/* Transform prediction to frequency domain. */
	FilterBank(hEncoder, coderInfo, predicted_samples, mdct_predicted,
		NULL, MNON_OVERLAPPED);

	/* Apply TNS analysis filter to the predicted spectrum. */
	if (tnsInfo != NULL)
		TnsEncodeFilterOnly(tnsInfo, num_of_sfb, num_of_sfb, (WINDOW_TYPE)coderInfo->block_type, sfb_offset,
			mdct_predicted);

	/* Get the prediction gain. */
	bit_gain = snr_pred(p_spectrum, mdct_predicted, sfb_prediction_used,
		sfb_offset, side_info, last_band, coderInfo->nr_of_sfb);

	return (bit_gain);
}

void LtpInit(faacEncHandle hEncoder)
{
	int i;
	uint32_t channel;

	for (channel = 0; channel < hEncoder->numChannels; channel++) {
		LtpInfo *ltpInfo = &(hEncoder->coderInfo[channel].ltpInfo);

		ltpInfo->buffer = (double*)AllocMemory(NOK_LT_BLEN * sizeof(double));
		ltpInfo->mdct_predicted = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));
		ltpInfo->time_buffer = (double*)AllocMemory(BLOCK_LEN_LONG * sizeof(double));
		ltpInfo->ltp_overlap_buffer = (double*)AllocMemory(BLOCK_LEN_LONG * sizeof(double));

		for (i = 0; i < NOK_LT_BLEN; i++)
			ltpInfo->buffer[i] = 0;

		ltpInfo->weight_idx = 0;
		for (i = 0; i < MAX_SHORT_WINDOWS; i++)
			ltpInfo->sbk_prediction_used[i] = ltpInfo->delay[i] = 0;

		for (i = 0; i < MAX_SCFAC_BANDS; i++)
			ltpInfo->sfb_prediction_used[i] = 0;

		ltpInfo->side_info = LEN_LTP_DATA_PRESENT;

		for (i = 0; i < 2 * BLOCK_LEN_LONG; i++)
			ltpInfo->mdct_predicted[i] = 0.0;

	}
}

void LtpEnd(faacEncHandle hEncoder)
{
	uint32_t channel;

	for (channel = 0; channel < hEncoder->numChannels; channel++) {
		LtpInfo *ltpInfo = &(hEncoder->coderInfo[channel].ltpInfo);

		if (ltpInfo->buffer)
			FreeMemory(ltpInfo->buffer);
		if (ltpInfo->mdct_predicted)
			FreeMemory(ltpInfo->mdct_predicted);
		if (ltpInfo->time_buffer)
			FreeMemory(ltpInfo->time_buffer);
		if (ltpInfo->ltp_overlap_buffer)
			FreeMemory(ltpInfo->ltp_overlap_buffer);
	}
}

int LtpEncode(faacEncHandle hEncoder,
	CoderInfo *coderInfo,
	LtpInfo *ltpInfo,
	TnsInfo *tnsInfo,
	double *p_spectrum,
	double *p_time_signal)
{
	int i, last_band;
	double num_bit[MAX_SHORT_WINDOWS];
	double *predicted_samples;

	ltpInfo->global_pred_flag = 0;
	ltpInfo->side_info = 0;

	predicted_samples = (double*)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));

	switch (coderInfo->block_type)
	{
	case ONLY_LONG_WINDOW:
	case LONG_SHORT_WINDOW:
	case SHORT_LONG_WINDOW:
		last_band = (coderInfo->nr_of_sfb < MAX_LT_PRED_LONG_SFB) ? coderInfo->nr_of_sfb : MAX_LT_PRED_LONG_SFB;

		ltpInfo->delay[0] =
			pitch(p_time_signal, ltpInfo->buffer, 2 * BLOCK_LEN_LONG,
				0, 2 * BLOCK_LEN_LONG, predicted_samples, &ltpInfo->weight,
				&ltpInfo->weight_idx);


		num_bit[0] =
			ltp_enc_tf(hEncoder, coderInfo, p_spectrum, predicted_samples,
				ltpInfo->mdct_predicted,
				coderInfo->sfb_offset, coderInfo->nr_of_sfb,
				last_band, ltpInfo->side_info, ltpInfo->sfb_prediction_used,
				tnsInfo);


		ltpInfo->global_pred_flag = (num_bit[0] == 0.0) ? 0 : 1;

		if (ltpInfo->global_pred_flag)
			for (i = 0; i < coderInfo->sfb_offset[last_band]; i++)
				p_spectrum[i] -= ltpInfo->mdct_predicted[i];
		else
			ltpInfo->side_info = 1;

		break;

	default:
		break;
	}

	if (predicted_samples) FreeMemory(predicted_samples);

	return (ltpInfo->global_pred_flag);
}

void LtpReconstruct(CoderInfo *coderInfo, LtpInfo *ltpInfo, double *p_spectrum)
{
	int i, last_band;

	if (ltpInfo->global_pred_flag)
	{
		switch (coderInfo->block_type)
		{
		case ONLY_LONG_WINDOW:
		case LONG_SHORT_WINDOW:
		case SHORT_LONG_WINDOW:
			last_band = (coderInfo->nr_of_sfb < MAX_LT_PRED_LONG_SFB) ?
				coderInfo->nr_of_sfb : MAX_LT_PRED_LONG_SFB;

			for (i = 0; i < coderInfo->sfb_offset[last_band]; i++)
				p_spectrum[i] += ltpInfo->mdct_predicted[i];
			break;

		default:
			break;
		}
	}
}

void  LtpUpdate(LtpInfo *ltpInfo, double *time_signal,
	double *overlap_signal, int block_size_long)
{
	int i;

	for (i = 0; i < NOK_LT_BLEN - 2 * block_size_long; i++)
		ltpInfo->buffer[i] = ltpInfo->buffer[i + block_size_long];

	for (i = 0; i < block_size_long; i++)
	{
		ltpInfo->buffer[NOK_LT_BLEN - 2 * block_size_long + i] = time_signal[i];
		ltpInfo->buffer[NOK_LT_BLEN - block_size_long + i] = overlap_signal[i];
	}
}


void MSEncode(CoderInfo *coderInfo, ChannelInfo *channelInfo, double *spectrum[MAX_CHANNELS], int maxchan, int allowms)
{
	int chn;

	for (chn = 0; chn < maxchan; chn++)
	{
		if (channelInfo[chn].present)
		{
			if ((channelInfo[chn].cpe) && (channelInfo[chn].ch_is_left))
			{
				int rch = channelInfo[chn].paired_ch;

				channelInfo[chn].msInfo.is_present = 0;
				channelInfo[rch].msInfo.is_present = 0;

				/* Perform MS if block_types are the same */
				if ((coderInfo[chn].block_type == coderInfo[rch].block_type)
					&& allowms)
				{
					int nsfb = coderInfo[chn].nr_of_sfb;
					MSInfo *msInfoL = &(channelInfo[chn].msInfo);
					MSInfo *msInfoR = &(channelInfo[rch].msInfo);
					int sfb;

					channelInfo[chn].common_window = 1;  /* Use common window */
					channelInfo[chn].msInfo.is_present = 1;
					channelInfo[rch].msInfo.is_present = 1;

					// make the same reference energy in both channels
					coderInfo[chn].avgenrg = coderInfo[rch].avgenrg =
						0.5 * (coderInfo[chn].avgenrg + coderInfo[rch].avgenrg);

					for (sfb = 0; sfb < nsfb; sfb++)
					{
						int ms = 0;
						int l, start, end;
						double sum, diff;
						double enrgs, enrgd, enrgl, enrgr;
						double maxs, maxd, maxl, maxr;

						start = coderInfo[chn].sfb_offset[sfb];
						end = coderInfo[chn].sfb_offset[sfb + 1];

						enrgs = enrgd = enrgl = enrgr = 0.0;
						maxs = maxd = maxl = maxr = 0.0;
						for (l = start; l < end; l++)
						{
							double lx = spectrum[chn][l];
							double rx = spectrum[rch][l];

							sum = 0.5 * (lx + rx);
							diff = 0.5 * (lx - rx);

							enrgs += sum * sum;
							maxs = max(maxs, fabs(sum));

							enrgd += diff * diff;
							maxd = max(maxd, fabs(diff));

							enrgl += lx * lx;
							enrgr += rx * rx;

							maxl = max(maxl, fabs(lx));
							maxr = max(maxr, fabs(rx));
						}

#if 1
						if ((min(enrgs, enrgd) < min(enrgl, enrgr))
							&& (min(maxs, maxd) < min(maxl, maxr)))
							ms = 1;
#else
						if (min(enrgs, enrgd) < min(enrgl, enrgr))
							ms = 1;
#endif

						//printf("%d:%d\n", sfb, ms);

						msInfoR->ms_used[sfb] = msInfoL->ms_used[sfb] = ms;

						if (ms)
							for (l = start; l < end; l++)
							{
								sum = spectrum[chn][l] + spectrum[rch][l];
								diff = spectrum[chn][l] - spectrum[rch][l];
								spectrum[chn][l] = 0.5 * sum;
								spectrum[rch][l] = 0.5 * diff;
							}
					}
				}
			}
		}
	}
}

void MSReconstruct(CoderInfo *coderInfo,
	ChannelInfo *channelInfo,
	int maxchan)
{
	int chn;

	for (chn = 0; chn < maxchan; chn++)
	{
		if (channelInfo[chn].present)
		{
			if (channelInfo[chn].cpe && channelInfo[chn].ch_is_left)
			{
				int rch = channelInfo[chn].paired_ch;

				MSInfo *msInfoL = &(channelInfo[chn].msInfo);

				if (msInfoL->is_present) {
					int nsfb = coderInfo[chn].nr_of_sfb;
					int sfb;

					for (sfb = 0; sfb < nsfb; sfb++)
					{
						int l, start, end;

						start = coderInfo[chn].sfb_offset[sfb];
						end = coderInfo[chn].sfb_offset[sfb + 1];

						if (msInfoL->ms_used[sfb])
						{
							for (l = start; l < end; l++)
							{
								double sum, diff;

								sum = coderInfo[chn].requantFreq[l];
								diff = coderInfo[rch].requantFreq[l];
								coderInfo[chn].requantFreq[l] = sum + diff;
								coderInfo[rch].requantFreq[l] = sum - diff;
							}
						}
					}
				}
			}
		}
	}
}


typedef struct
{
	/* bandwidth */
	int bandS;
	int lastband;

	/* SFB energy */
	float *fftEnrgS[8];
	float *fftEnrgNextS[8];
	float *fftEnrgNext2S[8];
	float *fftEnrgPrevS[8];
}
psydata_t;


static void Hann(GlobalPsyInfo * gpsyInfo, double *inSamples, int size)
{
	int i;

	/* Applying Hann window */
	if (size == BLOCK_LEN_LONG * 2)
	{
		for (i = 0; i < size; i++)
			inSamples[i] *= gpsyInfo->hannWindow[i];
	}
	else
	{
		for (i = 0; i < size; i++)
			inSamples[i] *= gpsyInfo->hannWindowS[i];
	}
}

static void PsyCheckShort(PsyInfo * psyInfo)
{
	double totvol = 0.0;
	double totchg, totchg2;
	psydata_t *psydata = (psydata_t *)psyInfo->data;
	int lastband = psydata->lastband;
	int firstband = 1;
	int sfb;

	/* long/short block switch */
	totchg = totchg2 = 0.0;
	for (sfb = 0; sfb < lastband; sfb++)
	{
		int win;
		double volb[16];
		double vavg[13];
		double maxdif = 0.0;
		double totmaxdif = 0.0;
		double e, v;

		// previous frame
		for (win = 0; win < 4; win++)
		{
			e = psydata->fftEnrgPrevS[win + 4][sfb];

			volb[win] = sqrt(e);
			totvol += e;
		}

		// current frame
		for (win = 0; win < 8; win++)
		{
			e = psydata->fftEnrgS[win][sfb];

			volb[win + 4] = sqrt(e);
			totvol += e;
		}
		// next frame
		for (win = 0; win < 4; win++)
		{
			e = psydata->fftEnrgNextS[win][sfb];

			volb[win + 12] = sqrt(e);
			totvol += e;
		}

		// ignore lowest SFBs
		if (sfb < firstband)
			continue;

		v = 0.0;
		for (win = 0; win < 4; win++)
		{
			v += volb[win];
		}
		vavg[0] = 0.25 * v;

		for (win = 1; win < 13; win++)
		{
			v -= volb[win - 1];
			v += volb[win + 3];
			vavg[win] = 0.25 * v;
		}

		for (win = 0; win < 8; win++)
		{
			int i;
			double mina, maxv;
			double voldif;
			double totvoldif;

			mina = vavg[win];
			for (i = 1; i < 5; i++)
				mina = min(mina, vavg[win + i]);

			maxv = volb[win + 2];
			for (i = 3; i < 6; i++)
				maxv = max(maxv, volb[win + i]);

			if (!maxv || !mina)
				continue;

			voldif = (maxv - mina) / mina;
			totvoldif = (maxv - mina) * (maxv - mina);

			if (voldif > maxdif)
				maxdif = voldif;

			if (totvoldif > totmaxdif)
				totmaxdif = totvoldif;
		}
		totchg += maxdif;
		totchg2 += totmaxdif;
	}

	totvol = sqrt(totvol);

	totchg2 = sqrt(totchg2);

	totchg = totchg / lastband;
	if (totvol)
		totchg2 /= totvol;
	else
		totchg2 = 0.0;

	psyInfo->block_type = ((totchg > 1.0) && (totchg2 > 0.04))
		? ONLY_SHORT_WINDOW : ONLY_LONG_WINDOW;

#if 0
	{
		static int total = 0, shorts = 0;
		char *flash = "    ";

		total++;
		if (psyInfo->block_type == ONLY_SHORT_WINDOW)
		{
			flash = "****";
			shorts++;
		}

		printf("totchg: %s %g %g\t%g\n", flash, totchg, totchg2,
			(double)shorts / total);
	}
#endif
}

static void PsyInit(GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo, uint32_t numChannels,
	uint32_t sampleRate, int *cb_width_long, int num_cb_long,
	int *cb_width_short, int num_cb_short)
{
	uint32_t channel;
	int i, j, size;

	gpsyInfo->hannWindow =
		(double *)AllocMemory(2 * BLOCK_LEN_LONG * sizeof(double));
	gpsyInfo->hannWindowS =
		(double *)AllocMemory(2 * BLOCK_LEN_SHORT * sizeof(double));

	for (i = 0; i < BLOCK_LEN_LONG * 2; i++)
		gpsyInfo->hannWindow[i] = 0.5 * (1 - cos(2.0 * M_PI * (i + 0.5) /
		(BLOCK_LEN_LONG * 2)));
	for (i = 0; i < BLOCK_LEN_SHORT * 2; i++)
		gpsyInfo->hannWindowS[i] = 0.5 * (1 - cos(2.0 * M_PI * (i + 0.5) /
		(BLOCK_LEN_SHORT * 2)));
	gpsyInfo->sampleRate = (double)sampleRate;

	for (channel = 0; channel < numChannels; channel++)
	{
		psydata_t *psydata = (psydata_t *)AllocMemory(sizeof(psydata_t));
		psyInfo[channel].data = psydata;
	}

	size = BLOCK_LEN_LONG;
	for (channel = 0; channel < numChannels; channel++)
	{
		psyInfo[channel].size = size;

		psyInfo[channel].prevSamples =
			(double *)AllocMemory(size * sizeof(double));
		memset(psyInfo[channel].prevSamples, 0, size * sizeof(double));
	}

	size = BLOCK_LEN_SHORT;
	for (channel = 0; channel < numChannels; channel++)
	{
		psydata_t *psydata = (psydata_t *)psyInfo[channel].data;

		psyInfo[channel].sizeS = size;

		psyInfo[channel].prevSamplesS =
			(double *)AllocMemory(size * sizeof(double));
		memset(psyInfo[channel].prevSamplesS, 0, size * sizeof(double));

		for (j = 0; j < 8; j++)
		{
			psydata->fftEnrgPrevS[j] =
				(float *)AllocMemory(NSFB_SHORT * sizeof(float));
			memset(psydata->fftEnrgPrevS[j], 0, NSFB_SHORT * sizeof(float));
			psydata->fftEnrgS[j] =
				(float *)AllocMemory(NSFB_SHORT * sizeof(float));
			memset(psydata->fftEnrgS[j], 0, NSFB_SHORT * sizeof(float));
			psydata->fftEnrgNextS[j] =
				(float *)AllocMemory(NSFB_SHORT * sizeof(float));
			memset(psydata->fftEnrgNextS[j], 0, NSFB_SHORT * sizeof(float));
			psydata->fftEnrgNext2S[j] =
				(float *)AllocMemory(NSFB_SHORT * sizeof(float));
			memset(psydata->fftEnrgNext2S[j], 0, NSFB_SHORT * sizeof(float));
		}
	}
}

static void PsyEnd(GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo, uint32_t numChannels)
{
	uint32_t channel;
	int j;

	if (gpsyInfo->hannWindow)
		FreeMemory(gpsyInfo->hannWindow);
	if (gpsyInfo->hannWindowS)
		FreeMemory(gpsyInfo->hannWindowS);

	for (channel = 0; channel < numChannels; channel++)
	{
		if (psyInfo[channel].prevSamples)
			FreeMemory(psyInfo[channel].prevSamples);
	}

	for (channel = 0; channel < numChannels; channel++)
	{
		psydata_t *psydata = (psydata_t *)psyInfo[channel].data;

		if (psyInfo[channel].prevSamplesS)
			FreeMemory(psyInfo[channel].prevSamplesS);
		for (j = 0; j < 8; j++)
		{
			if (psydata->fftEnrgPrevS[j])
				FreeMemory(psydata->fftEnrgPrevS[j]);
			if (psydata->fftEnrgS[j])
				FreeMemory(psydata->fftEnrgS[j]);
			if (psydata->fftEnrgNextS[j])
				FreeMemory(psydata->fftEnrgNextS[j]);
			if (psydata->fftEnrgNext2S[j])
				FreeMemory(psydata->fftEnrgNext2S[j]);
		}
	}

	for (channel = 0; channel < numChannels; channel++)
	{
		if (psyInfo[channel].data)
			FreeMemory(psyInfo[channel].data);
	}
}

/* Do psychoacoustical analysis */
static void PsyCalculate(ChannelInfo * channelInfo, GlobalPsyInfo * gpsyInfo,
	PsyInfo * psyInfo, int *cb_width_long, int
	num_cb_long, int *cb_width_short,
	int num_cb_short, uint32_t numChannels)
{
	uint32_t channel;

	for (channel = 0; channel < numChannels; channel++)
	{
		if (channelInfo[channel].present)
		{

			if (channelInfo[channel].cpe &&
				channelInfo[channel].ch_is_left)
			{				/* CPE */

				int leftChan = channel;
				int rightChan = channelInfo[channel].paired_ch;

				PsyCheckShort(&psyInfo[leftChan]);
				PsyCheckShort(&psyInfo[rightChan]);
			}
			else if (!channelInfo[channel].cpe &&
				channelInfo[channel].lfe)
			{				/* LFE */
							// Only set block type and it should be OK
				psyInfo[channel].block_type = ONLY_LONG_WINDOW;
			}
			else if (!channelInfo[channel].cpe)
			{				/* SCE */
				PsyCheckShort(&psyInfo[channel]);
			}
		}
	}
}

static void PsyBufferUpdate(FFT_Tables *fft_tables, GlobalPsyInfo * gpsyInfo, PsyInfo * psyInfo,
	double *newSamples, uint32_t bandwidth,
	int *cb_width_short, int num_cb_short)
{
	int win;
	double transBuff[2 * BLOCK_LEN_LONG];
	double transBuffS[2 * BLOCK_LEN_SHORT];
	psydata_t *psydata = (psydata_t *)psyInfo->data;
	float *tmp;
	int sfb;

	psydata->bandS = (int)(psyInfo->sizeS * bandwidth * 2 / gpsyInfo->sampleRate);

	memcpy(transBuff, psyInfo->prevSamples, psyInfo->size * sizeof(double));
	memcpy(transBuff + psyInfo->size, newSamples, psyInfo->size * sizeof(double));

	for (win = 0; win < 8; win++)
	{
		int first = 0;
		int last = 0;

		memcpy(transBuffS, transBuff + (win * BLOCK_LEN_SHORT) + (BLOCK_LEN_LONG - BLOCK_LEN_SHORT) / 2,
			2 * psyInfo->sizeS * sizeof(double));

		Hann(gpsyInfo, transBuffS, 2 * psyInfo->sizeS);
		rfft(fft_tables, transBuffS, 8);

		// shift bufs
		tmp = psydata->fftEnrgPrevS[win];
		psydata->fftEnrgPrevS[win] = psydata->fftEnrgS[win];
		psydata->fftEnrgS[win] = psydata->fftEnrgNextS[win];
		psydata->fftEnrgNextS[win] = psydata->fftEnrgNext2S[win];
		psydata->fftEnrgNext2S[win] = tmp;

		for (sfb = 0; sfb < num_cb_short; sfb++)
		{
			double e;
			int l;

			first = last;
			last = first + cb_width_short[sfb];

			if (first < 1)
				first = 1;

			//if (last > psydata->bandS) // band out of range
			if (first >= psydata->bandS) // band out of range
				break;

			e = 0.0;
			for (l = first; l < last; l++)
			{
				double a = transBuffS[l];
				double b = transBuffS[l + psyInfo->sizeS];
				e += a * a + b * b;
			}

			psydata->fftEnrgNext2S[win][sfb] = (float)e;
		}
		psydata->lastband = sfb;
		for (; sfb < num_cb_short; sfb++)
		{
			psydata->fftEnrgNext2S[win][sfb] = 0;
		}
	}

	memcpy(psyInfo->prevSamples, newSamples, psyInfo->size * sizeof(double));
}

static void BlockSwitch(CoderInfo * coderInfo, PsyInfo * psyInfo, uint32_t numChannels)
{
	uint32_t channel;
	int desire = ONLY_LONG_WINDOW;

	/* Use the same block type for all channels
	If there is 1 channel that wants a short block,
	use a short block on all channels.
	*/
	for (channel = 0; channel < numChannels; channel++)
	{
		if (psyInfo[channel].block_type == ONLY_SHORT_WINDOW)
			desire = ONLY_SHORT_WINDOW;
	}

	for (channel = 0; channel < numChannels; channel++)
	{
		int lasttype = coderInfo[channel].block_type;

		if (desire == ONLY_SHORT_WINDOW
			|| coderInfo[channel].desired_block_type == ONLY_SHORT_WINDOW)
		{
			if (lasttype == ONLY_LONG_WINDOW || lasttype == SHORT_LONG_WINDOW)
				coderInfo[channel].block_type = LONG_SHORT_WINDOW;
			else
				coderInfo[channel].block_type = ONLY_SHORT_WINDOW;
		}
		else
		{
			if (lasttype == ONLY_SHORT_WINDOW || lasttype == LONG_SHORT_WINDOW)
				coderInfo[channel].block_type = SHORT_LONG_WINDOW;
			else
				coderInfo[channel].block_type = ONLY_LONG_WINDOW;
		}
		coderInfo[channel].desired_block_type = desire;
	}
}

psymodel_t psymodel2 =
{
	PsyInit,
	PsyEnd,
	PsyCalculate,
	PsyBufferUpdate,
	BlockSwitch
};


static uint32_t tnsSupportedSamplingRates[13] =
{ 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,0 };

/* Limit bands to > 2.0 kHz */
static uint16_t tnsMinBandNumberLong[12] =
{ 11, 12, 15, 16, 17, 20, 25, 26, 24, 28, 30, 31 };
static uint16_t tnsMinBandNumberShort[12] =
{ 2, 2, 2, 3, 3, 4, 6, 6, 8, 10, 10, 12 };

/**************************************/
/* Main/Low Profile TNS Parameters    */
/**************************************/
static uint16_t tnsMaxBandsLongMainLow[12] =
{ 31, 31, 34, 40, 42, 51, 46, 46, 42, 42, 42, 39 };

static uint16_t tnsMaxBandsShortMainLow[12] =
{ 9, 9, 10, 14, 14, 14, 14, 14, 14, 14, 14, 14 };

static uint16_t tnsMaxOrderLongMain = 20;
static uint16_t tnsMaxOrderLongLow = 12;
static uint16_t tnsMaxOrderShortMainLow = 7;


/*************************/
/* Function prototypes   */
/*************************/
static void Autocorrelation(int maxOrder,        /* Maximum autocorr order */
	int dataSize,        /* Size of the data array */
	double* data,        /* Data array */
	double* rArray);     /* Autocorrelation array */

static double LevinsonDurbin(int maxOrder,        /* Maximum filter order */
	int dataSize,        /* Size of the data array */
	double* data,        /* Data array */
	double* kArray);     /* Reflection coeff array */

static void StepUp(int fOrder, double* kArray, double* aArray);

static void QuantizeReflectionCoeffs(int fOrder, int coeffRes, double* rArray, int* indexArray);
static int TruncateCoeffs(int fOrder, double threshold, double* kArray);
static void TnsFilter(int length, double* spec, TnsFilterData* filter);
static void TnsInvFilter(int length, double* spec, TnsFilterData* filter);


/*****************************************************/
/* InitTns:                                          */
/*****************************************************/
void TnsInit(faacEncHandle hEncoder)
{
	uint32_t channel;
	int fsIndex = hEncoder->sampleRateIdx;
	int profile = hEncoder->config.aacObjectType;

	for (channel = 0; channel < hEncoder->numChannels; channel++) {
		TnsInfo *tnsInfo = &hEncoder->coderInfo[channel].tnsInfo;

		switch (profile) {
		case MAIN:
		case LTP:
			tnsInfo->tnsMaxBandsLong = tnsMaxBandsLongMainLow[fsIndex];
			tnsInfo->tnsMaxBandsShort = tnsMaxBandsShortMainLow[fsIndex];
			if (hEncoder->config.mpegVersion == 1) { /* MPEG2 */
				tnsInfo->tnsMaxOrderLong = tnsMaxOrderLongMain;
			}
			else { /* MPEG4 */
				if (fsIndex <= 5) /* fs > 32000Hz */
					tnsInfo->tnsMaxOrderLong = 12;
				else
					tnsInfo->tnsMaxOrderLong = 20;
			}
			tnsInfo->tnsMaxOrderShort = tnsMaxOrderShortMainLow;
			break;
		case LOW:
			tnsInfo->tnsMaxBandsLong = tnsMaxBandsLongMainLow[fsIndex];
			tnsInfo->tnsMaxBandsShort = tnsMaxBandsShortMainLow[fsIndex];
			if (hEncoder->config.mpegVersion == 1) { /* MPEG2 */
				tnsInfo->tnsMaxOrderLong = tnsMaxOrderLongLow;
			}
			else { /* MPEG4 */
				if (fsIndex <= 5) /* fs > 32000Hz */
					tnsInfo->tnsMaxOrderLong = 12;
				else
					tnsInfo->tnsMaxOrderLong = 20;
			}
			tnsInfo->tnsMaxOrderShort = tnsMaxOrderShortMainLow;
			break;
		}
		tnsInfo->tnsMinBandNumberLong = tnsMinBandNumberLong[fsIndex];
		tnsInfo->tnsMinBandNumberShort = tnsMinBandNumberShort[fsIndex];
	}
}


/*****************************************************/
/* TnsEncode:                                        */
/*****************************************************/
void TnsEncode(TnsInfo* tnsInfo,       /* TNS info */
	int numberOfBands,       /* Number of bands per window */
	int maxSfb,              /* max_sfb */
	enum WINDOW_TYPE blockType,   /* block type */
	int* sfbOffsetTable,     /* Scalefactor band offset table */
	double* spec)            /* Spectral data array */
{
	int numberOfWindows, windowSize;
	int startBand, stopBand, order;    /* Bands over which to apply TNS */
	int lengthInBands;               /* Length to filter, in bands */
	int w;
	int startIndex, length;
	double gain;

	switch (blockType) {
	case ONLY_SHORT_WINDOW:

		/* TNS not used for short blocks currently */
		tnsInfo->tnsDataPresent = 0;
		return;

		numberOfWindows = MAX_SHORT_WINDOWS;
		windowSize = BLOCK_LEN_SHORT;
		startBand = tnsInfo->tnsMinBandNumberShort;
		stopBand = numberOfBands;
		lengthInBands = stopBand - startBand;
		order = tnsInfo->tnsMaxOrderShort;
		startBand = min(startBand, tnsInfo->tnsMaxBandsShort);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsShort);
		break;

	default:
		numberOfWindows = 1;
		windowSize = BLOCK_LEN_SHORT;
		startBand = tnsInfo->tnsMinBandNumberLong;
		stopBand = numberOfBands;
		lengthInBands = stopBand - startBand;
		order = tnsInfo->tnsMaxOrderLong;
		startBand = min(startBand, tnsInfo->tnsMaxBandsLong);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsLong);
		break;
	}

	/* Make sure that start and stop bands < maxSfb */
	/* Make sure that start and stop bands >= 0 */
	startBand = min(startBand, maxSfb);
	stopBand = min(stopBand, maxSfb);
	startBand = max(startBand, 0);
	stopBand = max(stopBand, 0);

	tnsInfo->tnsDataPresent = 0;     /* default TNS not used */

									 /* Perform analysis and filtering for each window */
	for (w = 0; w<numberOfWindows; w++) {

		TnsWindowData* windowData = &tnsInfo->windowData[w];
		TnsFilterData* tnsFilter = windowData->tnsFilter;
		double* k = tnsFilter->kCoeffs;    /* reflection coeffs */
		double* a = tnsFilter->aCoeffs;    /* prediction coeffs */

		windowData->numFilters = 0;
		windowData->coefResolution = DEF_TNS_COEFF_RES;
		startIndex = w * windowSize + sfbOffsetTable[startBand];
		length = sfbOffsetTable[stopBand] - sfbOffsetTable[startBand];
		gain = LevinsonDurbin(order, length, &spec[startIndex], k);

		if (gain>DEF_TNS_GAIN_THRESH) {  /* Use TNS */
			int truncatedOrder;
			windowData->numFilters++;
			tnsInfo->tnsDataPresent = 1;
			tnsFilter->direction = 0;
			tnsFilter->coefCompress = 0;
			tnsFilter->length = lengthInBands;
			QuantizeReflectionCoeffs(order, DEF_TNS_COEFF_RES, k, tnsFilter->index);
			truncatedOrder = TruncateCoeffs(order, DEF_TNS_COEFF_THRESH, k);
			tnsFilter->order = truncatedOrder;
			StepUp(truncatedOrder, k, a);    /* Compute predictor coefficients */
			TnsInvFilter(length, &spec[startIndex], tnsFilter);      /* Filter */
		}
	}
}


/*****************************************************/
/* TnsEncodeFilterOnly:                              */
/* This is a stripped-down version of TnsEncode()    */
/* which performs TNS analysis filtering only        */
/*****************************************************/
void TnsEncodeFilterOnly(TnsInfo* tnsInfo,           /* TNS info */
	int numberOfBands,          /* Number of bands per window */
	int maxSfb,                 /* max_sfb */
	enum WINDOW_TYPE blockType, /* block type */
	int* sfbOffsetTable,        /* Scalefactor band offset table */
	double* spec)               /* Spectral data array */
{
	int numberOfWindows, windowSize;
	int startBand, stopBand;    /* Bands over which to apply TNS */
	int w;
	int startIndex, length;

	switch (blockType) {
	case ONLY_SHORT_WINDOW:
		numberOfWindows = MAX_SHORT_WINDOWS;
		windowSize = BLOCK_LEN_SHORT;
		startBand = tnsInfo->tnsMinBandNumberShort;
		stopBand = numberOfBands;
		startBand = min(startBand, tnsInfo->tnsMaxBandsShort);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsShort);
		break;

	default:
		numberOfWindows = 1;
		windowSize = BLOCK_LEN_LONG;
		startBand = tnsInfo->tnsMinBandNumberLong;
		stopBand = numberOfBands;
		startBand = min(startBand, tnsInfo->tnsMaxBandsLong);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsLong);
		break;
	}

	/* Make sure that start and stop bands < maxSfb */
	/* Make sure that start and stop bands >= 0 */
	startBand = min(startBand, maxSfb);
	stopBand = min(stopBand, maxSfb);
	startBand = max(startBand, 0);
	stopBand = max(stopBand, 0);


	/* Perform filtering for each window */
	for (w = 0; w<numberOfWindows; w++)
	{
		TnsWindowData* windowData = &tnsInfo->windowData[w];
		TnsFilterData* tnsFilter = windowData->tnsFilter;

		startIndex = w * windowSize + sfbOffsetTable[startBand];
		length = sfbOffsetTable[stopBand] - sfbOffsetTable[startBand];

		if (tnsInfo->tnsDataPresent  &&  windowData->numFilters) {  /* Use TNS */
			TnsInvFilter(length, &spec[startIndex], tnsFilter);
		}
	}
}


/*****************************************************/
/* TnsDecodeFilterOnly:                              */
/* This is a stripped-down version of TnsEncode()    */
/* which performs TNS synthesis filtering only       */
/*****************************************************/
void TnsDecodeFilterOnly(TnsInfo* tnsInfo,           /* TNS info */
	int numberOfBands,          /* Number of bands per window */
	int maxSfb,                 /* max_sfb */
	enum WINDOW_TYPE blockType, /* block type */
	int* sfbOffsetTable,        /* Scalefactor band offset table */
	double* spec)               /* Spectral data array */
{
	int numberOfWindows, windowSize;
	int startBand, stopBand;    /* Bands over which to apply TNS */
	int w;
	int startIndex, length;

	switch (blockType) {
	case ONLY_SHORT_WINDOW:
		numberOfWindows = MAX_SHORT_WINDOWS;
		windowSize = BLOCK_LEN_SHORT;
		startBand = tnsInfo->tnsMinBandNumberShort;
		stopBand = numberOfBands;
		startBand = min(startBand, tnsInfo->tnsMaxBandsShort);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsShort);
		break;

	default:
		numberOfWindows = 1;
		windowSize = BLOCK_LEN_LONG;
		startBand = tnsInfo->tnsMinBandNumberLong;
		stopBand = numberOfBands;
		startBand = min(startBand, tnsInfo->tnsMaxBandsLong);
		stopBand = min(stopBand, tnsInfo->tnsMaxBandsLong);
		break;
	}

	/* Make sure that start and stop bands < maxSfb */
	/* Make sure that start and stop bands >= 0 */
	startBand = min(startBand, maxSfb);
	stopBand = min(stopBand, maxSfb);
	startBand = max(startBand, 0);
	stopBand = max(stopBand, 0);


	/* Perform filtering for each window */
	for (w = 0; w<numberOfWindows; w++)
	{
		TnsWindowData* windowData = &tnsInfo->windowData[w];
		TnsFilterData* tnsFilter = windowData->tnsFilter;

		startIndex = w * windowSize + sfbOffsetTable[startBand];
		length = sfbOffsetTable[stopBand] - sfbOffsetTable[startBand];

		if (tnsInfo->tnsDataPresent  &&  windowData->numFilters) {  /* Use TNS */
			TnsFilter(length, &spec[startIndex], tnsFilter);
		}
	}
}


/*****************************************************/
/* TnsFilter:                                        */
/*   Filter the given spec with specified length     */
/*   using the coefficients specified in filter.     */
/*   Not that the order and direction are specified  */
/*   withing the TNS_FILTER_DATA structure.          */
/*****************************************************/
static void TnsFilter(int length, double* spec, TnsFilterData* filter)
{
	int i, j, k = 0;
	int order = filter->order;
	double* a = filter->aCoeffs;

	/* Determine loop parameters for given direction */
	if (filter->direction) {

		/* Startup, initial state is zero */
		for (i = length - 2; i>(length - 1 - order); i--) {
			k++;
			for (j = 1; j <= k; j++) {
				spec[i] -= spec[i + j] * a[j];
			}
		}

		/* Now filter completely inplace */
		for (i = length - 1 - order; i >= 0; i--) {
			for (j = 1; j <= order; j++) {
				spec[i] -= spec[i + j] * a[j];
			}
		}


	}
	else {

		/* Startup, initial state is zero */
		for (i = 1; i<order; i++) {
			for (j = 1; j <= i; j++) {
				spec[i] -= spec[i - j] * a[j];
			}
		}

		/* Now filter completely inplace */
		for (i = order; i<length; i++) {
			for (j = 1; j <= order; j++) {
				spec[i] -= spec[i - j] * a[j];
			}
		}
	}
}


/********************************************************/
/* TnsInvFilter:                                        */
/*   Inverse filter the given spec with specified       */
/*   length using the coefficients specified in filter. */
/*   Not that the order and direction are specified     */
/*   withing the TNS_FILTER_DATA structure.             */
/********************************************************/
static void TnsInvFilter(int length, double* spec, TnsFilterData* filter)
{
	int i, j, k = 0;
	int order = filter->order;
	double* a = filter->aCoeffs;
	double* temp;

	temp = (double *)AllocMemory(length * sizeof(double));

	/* Determine loop parameters for given direction */
	if (filter->direction) {

		/* Startup, initial state is zero */
		temp[length - 1] = spec[length - 1];
		for (i = length - 2; i>(length - 1 - order); i--) {
			temp[i] = spec[i];
			k++;
			for (j = 1; j <= k; j++) {
				spec[i] += temp[i + j] * a[j];
			}
		}

		/* Now filter the rest */
		for (i = length - 1 - order; i >= 0; i--) {
			temp[i] = spec[i];
			for (j = 1; j <= order; j++) {
				spec[i] += temp[i + j] * a[j];
			}
		}


	}
	else {

		/* Startup, initial state is zero */
		temp[0] = spec[0];
		for (i = 1; i<order; i++) {
			temp[i] = spec[i];
			for (j = 1; j <= i; j++) {
				spec[i] += temp[i - j] * a[j];
			}
		}

		/* Now filter the rest */
		for (i = order; i<length; i++) {
			temp[i] = spec[i];
			for (j = 1; j <= order; j++) {
				spec[i] += temp[i - j] * a[j];
			}
		}
	}
	if (temp) FreeMemory(temp);
}

static int TruncateCoeffs(int fOrder, double threshold, double* kArray)
{
	int i;

	for (i = fOrder; i >= 0; i--) {
		kArray[i] = (fabs(kArray[i])>threshold) ? kArray[i] : 0.0;
		if (kArray[i] != 0.0) return i;
	}

	return 0;
}

/*****************************************************/
/* QuantizeReflectionCoeffs:                         */
/*   Quantize the given array of reflection coeffs   */
/*   to the specified resolution in bits.            */
/*****************************************************/
static void QuantizeReflectionCoeffs(int fOrder,
	int coeffRes,
	double* kArray,
	int* indexArray)
{
	double iqfac, iqfac_m;
	int i;

	iqfac = ((1 << (coeffRes - 1)) - 0.5) / (M_PI / 2);
	iqfac_m = ((1 << (coeffRes - 1)) + 0.5) / (M_PI / 2);

	/* Quantize and inverse quantize */
	for (i = 1; i <= fOrder; i++) {
		indexArray[i] = (int)(0.5 + (asin(kArray[i])*((kArray[i] >= 0) ? iqfac : iqfac_m)));
		kArray[i] = sin((double)indexArray[i] / ((indexArray[i] >= 0) ? iqfac : iqfac_m));
	}
}

/*****************************************************/
/* Autocorrelation,                                  */
/*   Compute the autocorrelation function            */
/*   estimate for the given data.                    */
/*****************************************************/
static void Autocorrelation(int maxOrder,        /* Maximum autocorr order */
	int dataSize,        /* Size of the data array */
	double* data,        /* Data array */
	double* rArray)      /* Autocorrelation array */
{
	int order, index;

	for (order = 0; order <= maxOrder; order++) {
		rArray[order] = 0.0;
		for (index = 0; index<dataSize; index++) {
			rArray[order] += data[index] * data[index + order];
		}
		dataSize--;
	}
}



/*****************************************************/
/* LevinsonDurbin:                                   */
/*   Compute the reflection coefficients for the     */
/*   given data using LevinsonDurbin recursion.      */
/*   Return the prediction gain.                     */
/*****************************************************/
static double LevinsonDurbin(int fOrder,          /* Filter order */
	int dataSize,        /* Size of the data array */
	double* data,        /* Data array */
	double* kArray)      /* Reflection coeff array */
{
	int order, i;
	double signal;
	double error, kTemp;                /* Prediction error */
	double aArray1[TNS_MAX_ORDER + 1];    /* Predictor coeff array */
	double aArray2[TNS_MAX_ORDER + 1];    /* Predictor coeff array 2 */
	double rArray[TNS_MAX_ORDER + 1];     /* Autocorrelation coeffs */
	double* aPtr = aArray1;             /* Ptr to aArray1 */
	double* aLastPtr = aArray2;         /* Ptr to aArray2 */
	double* aTemp;

	/* Compute autocorrelation coefficients */
	Autocorrelation(fOrder, dataSize, data, rArray);
	signal = rArray[0];   /* signal energy */

						  /* Set up pointers to current and last iteration */
						  /* predictor coefficients.                       */
	aPtr = aArray1;
	aLastPtr = aArray2;
	/* If there is no signal energy, return */
	if (!signal) {
		kArray[0] = 1.0;
		for (order = 1; order <= fOrder; order++) {
			kArray[order] = 0.0;
		}
		return 0;

	}
	else {

		/* Set up first iteration */
		kArray[0] = 1.0;
		aPtr[0] = 1.0;        /* Ptr to predictor coeffs, current iteration*/
		aLastPtr[0] = 1.0;    /* Ptr to predictor coeffs, last iteration */
		error = rArray[0];

		/* Now perform recursion */
		for (order = 1; order <= fOrder; order++) {
			kTemp = aLastPtr[0] * rArray[order - 0];
			for (i = 1; i<order; i++) {
				kTemp += aLastPtr[i] * rArray[order - i];
			}
			kTemp = -kTemp / error;
			kArray[order] = kTemp;
			aPtr[order] = kTemp;
			for (i = 1; i<order; i++) {
				aPtr[i] = aLastPtr[i] + kTemp*aLastPtr[order - i];
			}
			error = error * (1 - kTemp*kTemp);

			/* Now make current iteration the last one */
			aTemp = aLastPtr;
			aLastPtr = aPtr;      /* Current becomes last */
			aPtr = aTemp;         /* Last becomes current */
		}
		return signal / error;    /* return the gain */
	}
}


/*****************************************************/
/* StepUp:                                           */
/*   Convert reflection coefficients into            */
/*   predictor coefficients.                         */
/*****************************************************/
static void StepUp(int fOrder, double* kArray, double* aArray)
{
	double aTemp[TNS_MAX_ORDER + 2];
	int i, order;

	aArray[0] = 1.0;
	aTemp[0] = 1.0;
	for (order = 1; order <= fOrder; order++) {
		aArray[order] = 0.0;
		for (i = 1; i <= order; i++) {
			aTemp[i] = aArray[i] + kArray[order] * aArray[order - i];
		}
		for (i = 1; i <= order; i++) {
			aArray[i] = aTemp[i];
		}
	}
}

/* Returns the sample rate index */
int GetSRIndex(uint32_t sampleRate)
{
	if (92017 <= sampleRate) return 0;
	if (75132 <= sampleRate) return 1;
	if (55426 <= sampleRate) return 2;
	if (46009 <= sampleRate) return 3;
	if (37566 <= sampleRate) return 4;
	if (27713 <= sampleRate) return 5;
	if (23004 <= sampleRate) return 6;
	if (18783 <= sampleRate) return 7;
	if (13856 <= sampleRate) return 8;
	if (11502 <= sampleRate) return 9;
	if (9391 <= sampleRate) return 10;

	return 11;
}

/* Returns the maximum bitrate per channel for that sampling frequency */
uint32_t MaxBitrate(uint32_t sampleRate)
{
	/*
	*  Maximum of 6144 bit for a channel
	*/
	return (uint32_t)(6144.0 * (double)sampleRate / (double)FRAME_LEN + .5);
}

/* Returns the minimum bitrate per channel for that sampling frequency */
uint32_t MinBitrate()
{
	return 8000;
}


/* Max prediction band for backward predictionas function of fs index */
const int MaxPredSfb[] = { 33, 33, 38, 40, 40, 40, 41, 41, 37, 37, 37, 34, 0 };

int GetMaxPredSfb(int samplingRateIdx)
{
	return MaxPredSfb[samplingRateIdx];
}

/* Calculate bit_allocation based on PE */
uint32_t BitAllocation(double pe, int short_block)
{
	double pew1;
	double pew2;
	double bit_allocation;

	if (short_block) {
		pew1 = 0.6;
		pew2 = 24.0;
	}
	else {
		pew1 = 0.3;
		pew2 = 6.0;
	}
	bit_allocation = pew1 * pe + pew2 * sqrt(pe);
	bit_allocation = min(max(0.0, bit_allocation), 6144.0);

	return (uint32_t)(bit_allocation + 0.5);
}

/* Returns the maximum bit reservoir size */
uint32_t MaxBitresSize(uint32_t bitRate, uint32_t sampleRate)
{
	return 6144 - (uint32_t)((double)bitRate / (double)sampleRate*(double)FRAME_LEN);
}

#define IMAXSIZE 8192 
class Encoder {
	int m_nFrameSize/* = 4096*/;
	int m_nSampleRate /*= 48000*/;
	int m_nChannels/* = 2*/;
	faacEncHandle m_h/* = NULL*/;
	uint32_t m_nMax/* = 0*/;
	uint32_t m_samplesInput/* = 0*/;
	uint8_t m_pBuf[IMAXSIZE];
public:
	int Open(int sample_rate /*= 44100*/, int channels /*= 2*/, int bitrate /*= 96*/) {
		m_h = faacEncOpen(sample_rate, channels, &m_samplesInput, &m_nMax);
		if (m_h == NULL) {
			return 0;
		}
		m_nSampleRate = sample_rate;
		m_nChannels = channels;
		m_nFrameSize = 1024 * 2 * m_nChannels;
		faacEncConfigurationPtr cfg = faacEncGetCurrentConfiguration(m_h);
		cfg->aacObjectType = LOW;// FLV need
		cfg->mpegVersion = MPEG2;
		cfg->useTns = 0;
		cfg->allowMidside = 1;
		cfg->bitRate = bitrate;// 96kbps
		cfg->bandWidth = sample_rate / 2;
		cfg->quantqual = 100;
		cfg->outputFormat = 1;//ADTS !bFLV;// FLV need 0 RAW format
		cfg->inputFormat = FAAC_INPUT_16BIT;
		if (!faacEncSetConfiguration(m_h, cfg)) {
			return 0;
		}
		return 1;
	}

	int  Encode(uint8_t *pBuf, uint8_t* pOut, int* iOutSize) {
		if (m_h) {
			int index = 3;
			do {
				int size = faacEncEncode(m_h, (int32_t *)pBuf, m_samplesInput, m_pBuf, m_nMax);//������
				if (size > 0) {
					*iOutSize = size;
					memcpy(pOut, m_pBuf, size);
					return 1;
				}
			} while (index--);
		}
		return 0;
	}

	void Close() {
		if (m_h) {
			faacEncClose(m_h);
			m_h = NULL;
		}
	}
};
};
//===============================================================
#include <jni.h>
#include <cstring>
#include <android/log.h>  // 可选：Android日志（若用于安卓平台）

#define LOG_TAG "FAACEncoderJNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
/**
 * 创建编码器JNI实现
 */
extern "C" JNIEXPORT jlong JNICALL Java_Apowersoft_WXMedia_AACEncoder_Create
        (JNIEnv *env, jobject thiz, jint sample_rate, jint channels) {
	LibFaac::Encoder *context = new LibFaac::Encoder();

	int bitrate = sample_rate * channels * 16 / 12 / 1000;
	int ret = context->Open(sample_rate, channels, bitrate);
	if (ret > 0) {
		LOGD("%s sample_rate=%d channel=%d %dbps ADTS OK", __FUNCTION__,sample_rate,channels,bitrate);
		return (jlong) reinterpret_cast<intptr_t>(context);
	}
	LOGD("%s sample_rate=%d channel=%d %dbps Error", __FUNCTION__,sample_rate,channels,bitrate);
	delete context;
	return NULL;
}

/**
 * 销毁编码器JNI实现
 */
extern "C" JNIEXPORT void JNICALL Java_Apowersoft_WXMedia_AACEncoder_Destroy
        (JNIEnv *env, jobject thiz, jlong handle) {
    if (handle == 0) return;
    LibFaac::Encoder *context = reinterpret_cast<LibFaac::Encoder*>(handle);  
	  LOGD("Destroy encoder  handle=%p Begin", context);
    if (context) {
        context->Close();
    	LOGD("Destroy encoder success! handle=%p End", context);
    	delete context;
    }
}

/**
 * 编码数据JNI实现
 */
extern "C" JNIEXPORT jint JNICALL Java_Apowersoft_WXMedia_AACEncoder_Encode
        (JNIEnv *env, jobject thiz, jlong handle, jbyteArray in, jbyteArray out) {
    // 1. 参数校验
    if (handle == 0 || in == nullptr || out == nullptr) {
        LOGE("Encode param error! handle=%ld, in=%p, out=%p", handle, in, out);
        return -1;
    }
    LibFaac::Encoder *context = reinterpret_cast<LibFaac::Encoder*>(handle);

    // 2. 获取输入输出缓冲区
    jbyte *inBuf = env->GetByteArrayElements(in, nullptr);
    jbyte *outBuf = env->GetByteArrayElements(out, nullptr);
    if (!inBuf || !outBuf) {
        LOGE("GetByteArrayElements failed!");
        env->ReleaseByteArrayElements(in, inBuf, JNI_ABORT);
        env->ReleaseByteArrayElements(out, outBuf, JNI_ABORT);
        return -1;
    }

	int encodeLen = 0;
	int ret = context->Encode((uint8_t *)inBuf, (uint8_t*)outBuf, &encodeLen);

    // 5. 释放缓冲区
    env->ReleaseByteArrayElements(in, inBuf, JNI_ABORT);  // 输入数据无需回写
    env->ReleaseByteArrayElements(out, outBuf, 0);       // 输出数据回写到Java数组
    // LOGD("Encode success! len=%d", encodeLen);
    return encodeLen;
}