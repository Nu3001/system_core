#define DUAL_KNOB 
// #define A1_BOARD
#define A2_BOARD

// configure CD part
#define SHINWA_SVS03

/*********************************************************/
/* The setup of search data table                           */
/* This setup is to select whether MCU uses the linear   */
/* velocity or the search data table for search operation. */
/*********************************************************/
#define MCU_FLASH_UPDATA_ENABLE
#define	SEARCH_TABLE_ENABLE	 /* Search data table enable	*/
#define TOC_DATA_CONTROL_SAVE
#define USER_SCRATCH_AREA_SKIP
#define USER_PROGRAM_RANDOM_PLAY

// configure mechanism used
//#define	OPT715K2_TC9414
//#define	KSS213C_TC9414
//#define   RAF146A_TC9414
//#define	DA11T3_TC9414
//#define   KSS213C_Zipang2
//#define	SanyoCP2_TC94A70
#define 	JVC726_Zipang2

// #define SET_1
// #define SET_2
// #define SET_3
#define SET_4

//#define	TNA_2007
// #define	JVC726_Zipang2_2125
//#define	DA23_Zipang2
// #define	TNA_2007_JVC
#define	FF_FR_SPEED_VARIABLE
//#define PRAM_98201W32			/*CDROM file transfer*/
//#define PRAM_98201W35
//#define PRAM_93201SZ03
//#define PRAM_93201SZ04
//#define PRAM_93201SZ05
//#define PRAM_93201SZ04b	// latest for Oasis-2 F201
//#define PRAM_93202_30  // for Oasis-2 F202
#define PRAM_93207SZD01	// for Oasis-2 F207

#define CD_RW_ENABLE
#define DISPLAY_RELATIVE_TRACK_NUMBER

/* define how sub-Q will be read */
// #define SUBQ_SBSY  // via INT0
#define SUBQ_4ms	// via 4mS timer

/* configure HW IIC */
// #define USE_HW_IIC_CHANNEL0
#define USE_HW_IIC_CHANNEL1
#define USE_HW_IIC_CHANNEL2
#define USE_HW_IIC_CHANNEL3
#define USE_HW_IIC_CHANNEL4

// define audio processor
#define DEFAULT_START_UP_VOLUME	5
#define DEFAULT_VOLUME_REF 5
//#define SOURCE_DEPENDENT_AUDIO

// configure power moding
#define POWER_OFF_STANDBY
#define AUDIO_MID_NEEDED

//efine AUX_NEEDED
//efine AUX_DETECT_AVAILABLE

//#define HERO_V1C03
#define HERO_V1B02

#define DIAG_NEEDED

#define FALSE 0
#define TRUE  1
