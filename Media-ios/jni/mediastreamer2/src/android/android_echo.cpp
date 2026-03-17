/*
 * android_echo.h -Android echo cancellation utilities.
 *
 * Copyright (C) 2009-2012  Belledonne Communications, Grenoble, France
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <jni.h>

#include "android_echo.h"
#include "sys/system_properties.h"


//merge from opensource-13-12-13,by wanquan

#ifdef ANDROID
static SoundDeviceDescription devices[]={
	{	"HTC",		"Nexus One",	"qsd8k",	0,	300 },
	{	"HTC",		"HTC One X",	"tegra",	0,	150 },	/*has a very good acoustic isolation, which result in calibration saying no echo. */
										/*/But with speaker mode there is a strong echo if software ec is disabled.*/
	{	"HTC",		"HTC One SV",	"msm8960",	0,	200 },	
	{	"HTC",		"HTC Desire",	"",			0,	250 },
	{	"HTC",		"HTC Sensation Z710e",	"",	0,	200 },
	{	"HTC",		"HTC Wildfire",	"",			0,	270 },
	
	
	{	"LGE",		"Nexus 4",		"msm8960",	0,	230 }, /* has built-in AEC starting from 4.3*/
	{	"LGE",		"LS670",		"",			0,	170 },
	
	{	"motorola",	"DROID RAZR",	"",			0,	400 },
	{	"motorola",	"MB860",		"",			0,	200 },
	{	"motorola",	"XT907",		"",			0,	500 },
	{	"motorola",	"DROIX X2",		"",			0,	320 },

	{	"samsung",	"GT-S5360",		"bcm21553",	0,	250 }, /*<Galaxy Y*/
	{	"samsung",	"GT-S5360L",	"",			0,	250 }, /*<Galaxy Y*/
	{	"samsung",	"GT-S6102",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /*<Galaxy Y duo*/
	{	"samsung",	"GT-S5570",		"",			0,	160 }, /*<Galaxy Y duo*/
	{	"samsung",	"GT-S5300",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /*<Galaxy Pocket*/
	{	"samsung",	"GT-S5830i",		"",		0,	200 }, /* Galaxy S */
	{	"samsung",	"GT-S5830",		"",			0,	170 }, /* Galaxy S */
	{	"samsung",	"GT-S5660",		"",			0,	160 }, /* Galaxy Gio */
	{	"samsung",	"GT-I9000",		"",			0,	200 }, /* Galaxy S */
	{	"samsung",	"GT-I9001",		"",			0,	150 }, /* Galaxy S+ */
	{	"samsung",	"GT-I9070",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S Advance */
	{	"samsung",	"SPH-D700",		"",			0,	200 }, /* Galaxy S Epic 4G*/
	{	"samsung",	"GT-I9100",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /*Galaxy S2*/
	{	"samsung",	"GT-I9100P",	"s5pc210",	DEVICE_HAS_BUILTIN_AEC,	0 }, /*Galaxy S2*/
	{	"samsung",	"GT-S7562",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /*<Galaxy S Duo*/
	{	"samsung",	"SCH-I415",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S ??*/
	{	"samsung",	"SCH-I425",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S ??*/
	{	"samsung",	"SCH-I535",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S ??*/
	{	"samsung",	"SPH-D710",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S2 Epic 4G*/
	{	"samsung",	"GT-I9300",		"exynos4",	DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy S3*/
	{	"samsung",	"SAMSUNG-SGH-I747","",		DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S3*/
	{	"samsung",	"SPH-L710","",				DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S3*/
	{	"samsung",	"SPH-D710","",				DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S3*/
	{	"samsung",	"SGH-T999",		"",			DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy S3*/
	{	"samsung",	"SAMSUNG-SGH-I337","",		DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S4 ? */
	{	"samsung",	"GT-I9195",		"",			DEVICE_HAS_BUILTIN_AEC,	0 }, /* Galaxy S4 mini*/
	{	"samsung",	"GT-N7000",		"",			DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy Note*/
	{	"samsung",	"GT-N7100",		"",			DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy Note 2*/
	{	"samsung",	"GT-N7105",		"",			DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy Note 2*/
	{	"samsung",	"SGH-T889",		"",			DEVICE_HAS_BUILTIN_AEC,	0 },  /*Galaxy Note 2*/
	{	"samsung",	"Nexus S",		"s5pc110",	DEVICE_HAS_BUILTIN_AEC_CRAPPY,	180 }, /*Nexus S gives calibration around 240ms, but in practice the internal buffer size shrinks after a couple of seconds.*/
	{	"samsung",	"Galaxy Nexus", "",			0,	120 },
	{	"samsung",	"GT-S5570I",	"",			0,	250},
	{	"samsung",	"GT-P3100",		"",			DEVICE_HAS_BUILTIN_AEC, 0 }, /* Galaxy Tab*/
	{	"samsung",	"GT-P7500",		"",			DEVICE_HAS_BUILTIN_AEC, 0 }, /* Galaxy Tab*/
	{	"samsung",	"GT-P7510",		"",			DEVICE_HAS_BUILTIN_AEC, 0 }, /* Galaxy Tab*/
	{	"samsung",	"GT-I915",		"",			DEVICE_HAS_BUILTIN_AEC, 0 }, /* Verizon Tab*/
	{	"samsung",	"GT-I8190N",	"montblanc",	DEVICE_HAS_BUILTIN_AEC, 0, 16000 }, /* Galaxy S3 Mini*/
	{	"samsung",	"GT-I8190",	"montblanc",	DEVICE_HAS_BUILTIN_AEC,	0, 16000 },  /*Galaxy S3 mini*/
	
	
	{	"Sony Ericsson","ST15a",	"",			0, 	150 },
	{	"Sony Ericsson","S51SE",	"",			0,	150 },
	{	"Sony Ericsson","SK17i",	"",			0,	140 },
	{	"Sony Ericsson","ST17i",	"",			0,	130 },
	{	"Sony Ericsson","ST18i",	"",			0,	140 },
	{	"Sony Ericsson","ST25i",	"",			0,	320 },
	{	"Sony Ericsson","ST27i",	"",			0,	320 },
	{	"Sony Ericsson","LT15i",	"",			0,	150 },
	{	"Sony Ericsson","LT18i",	"",			0,	150 },
	{	"Sony Ericsson","LT26i",	"",			0,	294,8000 },
	{	"Sony Ericsson","LT26ii",	"",			0,	294,8000 },
	{	"Sony Ericsson","LT28h",	"",			0,	294,8000  },
	{	"Sony Ericsson","MT11i",	"",			0,	150 },
	{	"Sony Ericsson","MT15i",	"",			0,	150 },
	{	"Sony Ericsson","ST15i",	"msm7x30",		0,	150 },
	
	{	"asus",		"Nexus 7",	"", 			0, 170},
	{	"asus",		"K00E",		"clovertrail", 	0, 200},
	
	{	"Amazon",		"KFTT",		"omap4",	DEVICE_USE_ANDROID_MIC,200},

	{	"Xiaomi","MI-ONE Plus",	"msm8660",			DEVICE_HAS_BUILTIN_AEC,	0,8000 },

	{	"HUAWEI","HUAWEI C8813D",	"",			DEVICE_HAS_BUILTIN_AEC_CRAPPY,	137,48000 },
	{	"HUAWEI","HUAWEI C8813Q",	"",			DEVICE_HAS_BUILTIN_AEC_CRAPPY,	137,48000 },
	{	"HUAWEI","HUAWEI C8813DQ",	"",			DEVICE_HAS_BUILTIN_AEC_CRAPPY,	137,48000 },
	#if 1 //merge from opensource-14-02-19,by wanquan
	{	"LENOVO",		"Lenovo B6000-F",		"",DEVICE_HAS_BUILTIN_AEC_CRAPPY,300,0},
	#endif
	{	NULL, NULL, NULL, 0, 0,0}
};
//{	"Sony Ericsson","LT26i",	"",			FALSE,	294,8000 },
//	{	"Sony Ericsson","LT26i",	"",			FALSE,	277,-1 },
	//{	"Sony Ericsson","LT26ii",	"",			FALSE,	230 ,-1 },
static SoundDeviceDescription undefined={"Generic", "Generic", "Generic", 0, 250, 0};

#else

static SoundDeviceDescription devices[]={
	{	NULL, NULL, NULL, 0, 0, 0}
};

static SoundDeviceDescription undefined={"Generic", "Generic", "Generic", 0, 0, 0};

#endif

static SoundDeviceDescription *lookup_by_model(const char *manufacturer, const char* model){
	SoundDeviceDescription *d=&devices[0];
	while (d->manufacturer!=NULL) {
		if (strcasecmp(d->manufacturer,manufacturer)==0 && strcmp(d->model,model)==0){
			return d;
		}
		d++;
	}
	return NULL;
}

static SoundDeviceDescription *lookup_by_platform(const char *platform){
	SoundDeviceDescription *d=&devices[0];
	while (d->manufacturer!=NULL){
		if (strcmp(d->platform,platform)==0){
			return d;
		}
		d++;
	}
	return NULL;
}

#ifndef PROP_VALUE_MAX
#define PROP_VALUE_MAX 256
#endif

SoundDeviceDescription * sound_device_description_get(void){
	SoundDeviceDescription *d;
	char manufacturer[PROP_VALUE_MAX]={0};
	char model[PROP_VALUE_MAX]={0};
	char platform[PROP_VALUE_MAX]={0};
	
	bool_t exact_match=FALSE;
	bool_t declares_builtin_aec=FALSE;

#ifdef ANDROID
	
	if (__system_property_get("ro.product.manufacturer",manufacturer)<=0){
		ms_warning("Could not get product manufacturer.");
	}
	if (__system_property_get("ro.product.model",model)<=0){
		ms_warning("Could not get product model.");
	}
	if (__system_property_get("ro.board.platform",platform)<=0){
		ms_warning("Could not get board platform.");
	}
	ms_message("sound_device_description_get for model [%s/%s], trying with platform name [%s].",manufacturer,model,platform);
	/* First ask android if the device has an hardware echo canceller (only >=4.2)*/
	{
		JNIEnv *env=ms_get_jni_env();
		jclass aecClass = env->FindClass("android/media/audiofx/AcousticEchoCanceler");
		if (aecClass!=NULL){
			jmethodID isAvailableID = (env)->GetStaticMethodID(aecClass,"isAvailable","()Z");
			if (isAvailableID!=NULL){
				jboolean ret=(env)->CallStaticBooleanMethod(aecClass,isAvailableID);
				if (ret){
					ms_message("This device (%s/%s/%s) declares it has a built-in echo canceller.",manufacturer,model,platform);
					declares_builtin_aec=TRUE;
				}else ms_message("This device (%s/%s/%s) says it has no built-in echo canceller.",manufacturer,model,platform);
			}else{
				ms_error("isAvailable() not found in class AcousticEchoCanceler !");
				(env)->ExceptionClear(); //very important.
			}
			(env)->DeleteLocalRef(aecClass);
		}else{
			(env)->ExceptionClear(); //very important.
		}
	}
	
#endif
	
	d=lookup_by_model(manufacturer,model);
	if (!d){
		ms_message("No AEC information available for model [%s/%s], trying with platform name [%s].",manufacturer,model,platform);
		d=lookup_by_platform(platform);
		if (!d){
			ms_message("No AEC information available for platform [%s].",platform);
		}
	}else exact_match=TRUE;
	
	if (d) {
		ms_message("Found AEC information for [%s/%s/%s] from internal table: builtin=[%s], delay=[%i] ms",
				manufacturer,model,platform, (d->flags & DEVICE_HAS_BUILTIN_AEC) ? "yes" : "no", d->delay);
	}else d=&undefined;
	
	if (declares_builtin_aec){
		if (exact_match && (d->flags & DEVICE_HAS_BUILTIN_AEC_CRAPPY)){
			ms_warning("This device declares a builtin AEC but according to internal tables it is known to be misfunctionning, so trusting tables.");
		}else{
			d->flags=DEVICE_HAS_BUILTIN_AEC;
			d->delay=0;
		}
	}
	return d;
}


#if 0
struct EcDescription{
	const char *manufacturer;
	const char *model;
	const char *platform;
	int has_builtin_ec;
	int delay;
	int sample_rate;
};

static EcDescription ec_table[]={
	{	"HTC",		"Nexus One",	"qsd8k",	FALSE,	300,-1 },
	{	"HTC",		"HTC One X",	"tegra",	FALSE,	150,-1  },	//has a very good acoustic isolation, which result in calibration saying no echo. 
																//But with speaker mode there is a strong echo if software ec is disabled.
	{	"HTC",		"HTC Desire",	"",			FALSE,	250,-1  },
	{	"HTC",		"HTC Sensation Z710e",	"",	FALSE,	200,-1  },
	{	"HTC",		"HTC Wildfire",	"",			FALSE,	270,-1  },
	
	
	{	"LGE",		"Nexus 4",		"msm8960",	FALSE,	230 ,-1 },
	{	"LGE",		"LS670",		"",			FALSE,	170 ,-1 },
	
	{	"motorola",	"DROID RAZR",	"",			FALSE,	400 ,-1 },
	{	"motorola",	"MB860",		"",			FALSE,	200 ,-1 },
	{	"motorola",	"XT907",		"",			FALSE,	500 ,-1 },

	{	"samsung",	"GT-S5360",		"bcm21553",	FALSE,	250,-1  }, /*<Galaxy Y*/
	{	"samsung",	"GT-S5360L",	"",			FALSE,	250,-1  }, /*<Galaxy Y*/
	{	"samsung",	"GT-S6102",		"",			TRUE,	0,-1  }, /*<Galaxy Y duo*/
	{	"samsung",	"GT-S5570",		"",			FALSE,	160 ,-1 }, /*<Galaxy Y duo*/
	{	"samsung",	"GT-S5300",		"",			TRUE,	0 ,-1 }, /*<Galaxy Pocket*/
	{	"samsung",	"GT-S5830i",		"",		FALSE,	200,-1  }, /* Galaxy S */
	{	"samsung",	"GT-S5830",		"",			FALSE,	170,-1  }, /* Galaxy S */
	{	"samsung",	"GT-S5660",		"",			FALSE,	160 ,-1 }, /* Galaxy Gio */
	{	"samsung",	"GT-I9000",		"",			FALSE,	200 ,-1 }, /* Galaxy S */
	{	"samsung",	"GT-I9001",		"",			FALSE,	150,-1  }, /* Galaxy S+ */
	{	"samsung",	"GT-I9070",		"",			TRUE,	0,-1  }, /* Galaxy S Advance */
	{	"samsung",	"SPH-D700",		"",			FALSE,	200,-1  }, /* Galaxy S Epic 4G*/
	{	"samsung",	"GT-I9100",		"",			TRUE,	0 ,-1 }, /*Galaxy S2*/
	{	"samsung",	"GT-I9100P",	"s5pc210",	TRUE,	0 ,-1 }, /*Galaxy S2*/
	{	"samsung",	"GT-S7562",		"",			TRUE,	0 ,-1 }, /*<Galaxy S Duo*/
	{	"samsung",	"SCH-I415",		"",			TRUE,	0,-1  }, /* Galaxy S ??*/
	{	"samsung",	"SCH-I425",		"",			TRUE,	0,-1  }, /* Galaxy S ??*/
	{	"samsung",	"SCH-I535",		"",			TRUE,	0 ,-1 }, /* Galaxy S ??*/
	{	"samsung",	"SPH-D710","",				TRUE,	0,-1  }, /* Galaxy S3*/
	{	"samsung",	"GT-I9300",		"exynos4",	TRUE,	0,-1  },  /*Galaxy S3*/
	{	"samsung",	"SAMSUNG-SGH-I747","",		TRUE,	0 ,-1 }, /* Galaxy S3*/
	{	"samsung",	"SPH-L710","",				TRUE,	0,-1  }, /* Galaxy S3*/
	
	{	"samsung",	"SGH-T999",		"",			TRUE,	0,-1  },  /*Galaxy S3*/
	{	"samsung",	"GT-I8190",		"",			TRUE,	0,-1  },  /*Galaxy S3*/
	{	"samsung",	"SAMSUNG-SGH-I337","",		TRUE,	0 ,-1 }, /* Galaxy S4 ? */
	{	"samsung",	"GT-N7000",		"",			TRUE,	0,-1  },  /*Galaxy Note*/
	{	"samsung",	"GT-N7100",		"",			TRUE,	0,-1  },  /*Galaxy Note 2*/
	{	"samsung",	"GT-N7105",		"",			TRUE,	0,-1  },  /*Galaxy Note 2*/
	{	"samsung",	"SGH-T889",		"",			TRUE,	0 ,-1 },  /*Galaxy Note 2*/
	{	"samsung",	"Nexus S",		"s5pc110",	FALSE,	200,-1  },
	{	"samsung",	"Galaxy Nexus", "",			FALSE,	120 ,-1 },
	{	"samsung",	"GT-S5570I",	"",			FALSE,	250,-1 },
	{	"samsung",	"GT-P3100",		"",			TRUE, 0 ,-1 }, /* Galaxy Tab*/
	{	"samsung",	"GT-P7500",		"",			TRUE, 0 ,-1 }, /* Galaxy Tab*/
	{	"samsung",	"GT-P7510",		"",			TRUE, 0 ,-1 }, /* Galaxy Tab*/
	{	"samsung",	"GT-I915",		"",			TRUE, 0 ,-1 }, /* Verizon Tab*/
	
	
	{	"Sony Ericsson","SK17i",	"",			FALSE,	140 ,-1 },
	{	"Sony Ericsson","ST17i",	"",			FALSE,	130 ,-1 },
	{	"Sony Ericsson","ST18i",	"",			FALSE,	140 ,-1 },
	{	"Sony Ericsson","ST25i",	"",			FALSE,	320 ,-1 },
	{	"Sony Ericsson","ST27i",	"",			FALSE,	320 ,-1 },
	{	"Sony Ericsson","LT15i",	"",			FALSE,	150 ,-1 },
	{	"Sony Ericsson","LT18i",	"",			FALSE,	150,-1  },
	{	"Sony Ericsson","LT26i",	"",			FALSE,	294,8000 },
	{	"Sony Ericsson","LT26i",	"",			FALSE,	277,-1 },
	{	"Sony Ericsson","LT26ii",	"",			FALSE,	230 ,-1 },
	{	"Sony Ericsson","LT28h",	"",			FALSE,	210 ,-1 },
	{	"Sony Ericsson","MT11i",	"",			FALSE,	150 ,-1 },
	{	"Sony Ericsson","MT15i",	"",			FALSE,	150 ,-1 },

	{	"asus",			"Nexus 7",	"",			FALSE,	170 ,-1 },

	{	"Xiaomi","MI-ONE Plus",	"msm8660",			TRUE,	237,8000 },

	//{	"HUAWEI","HUAWEI C8813D",	"",			FALSE,	137,48000 },
	{	"HUAWEI","HUAWEI C8813Q",	"",			FALSE,	137,48000 },
	{	"HUAWEI","HUAWEI C8813DQ",	"",			FALSE,	137,48000 },
	
	{	NULL, NULL, NULL, FALSE, 0,-1 }
};
//
//
//{	"Xiaomi","MI-ONE Plus",	"msm8660",			FALSE,	215 },
static EcDescription *lookup_by_model(const char *manufacturer, const char* model){
	int i;
	for(i=0;ec_table[i].manufacturer!=NULL;i++){
		EcDescription *d=&ec_table[i];
		if (strcasecmp(d->manufacturer,manufacturer)==0 && strcmp(d->model,model)==0){
			return d;
		}
	}
	return NULL;
}

static EcDescription *lookup_by_platform(const char *platform){
	int i;
	
	for(i=0;ec_table[i].manufacturer!=NULL;i++){
		EcDescription *d=&ec_table[i];
		if (strcmp(d->platform,platform)==0){
			return d;
		}
	}
	return NULL;
}

int android_sound_get_echo_params(EchoCancellerParams *params){
	ms_error("android_sound_get_echo_params enter ");
	EcDescription *d;
	char manufacturer[PROP_VALUE_MAX]={0};
	char model[PROP_VALUE_MAX]={0};
	char platform[PROP_VALUE_MAX]={0};

	if (__system_property_get("ro.product.manufacturer",manufacturer)<=0){
		ms_warning("Could not get product manufacturer.");
		return -1;
	}
	if (__system_property_get("ro.product.model",model)<=0){
		ms_warning("Could not get product model.");
		return -1;
	}
	if (__system_property_get("ro.board.platform",platform)<=0){
		ms_warning("Could not get board platform.");
	}
	ms_error("android_sound_get_echo_params  information for %s/%s/%s",
				manufacturer,model,platform);
	#if V2_PHONE_SUPPORT
	d=lookup_by_model(manufacturer,model);
	if (d){
		ms_error("Found echo cancellation information 1 for %s/%s/%s: builtin=%s, delay=%i ms",
				manufacturer,model,platform,d->has_builtin_ec ? "yes" : "no", d->delay);
		params->has_builtin_ec=d->has_builtin_ec;
		params->delay=d->delay;
		params->sample_rate=d->sample_rate;
		return 0;
	}else{
		ms_warning("Lookup by model (%s/%s) failed.",manufacturer,model);
		d=lookup_by_platform(platform);
		if (d){
			ms_error("Found echo cancellation information 2 for %s/%s/%s: builtin=%s, delay=%i ms",
				manufacturer,model,platform,d->has_builtin_ec ? "yes" : "no", d->delay);
			params->has_builtin_ec=d->has_builtin_ec;
			params->delay=d->delay;
			params->sample_rate=d->sample_rate;
			return 0;
		}
	}
	#endif
	/* First ask android if the device has an hardware echo canceller (only >=4.2)*/
	{
		JNIEnv *env=ms_get_jni_env();
		jclass aecClass = env->FindClass("android/media/audiofx/AcousticEchoCanceler");
		if (aecClass!=NULL){
			aecClass= (jclass)env->NewGlobalRef(aecClass);
			jmethodID isAvailableID = env->GetStaticMethodID(aecClass,"isAvailable","()Z");
			if (isAvailableID!=NULL){
				jboolean ret=env->CallStaticBooleanMethod(aecClass,isAvailableID);
				if (ret){
					ms_error("This device (%s/%s/%s) has a built-in echo canceller.",manufacturer,model,platform);
					params->has_builtin_ec=TRUE;
					params->delay=0;
					env->DeleteGlobalRef(aecClass);
					return 0;
				}else ms_error("This device (%s/%s/%s) says it has no built-in echo canceller.",manufacturer,model,platform);
			}else{
				ms_error("isAvailable() not found in class AcousticEchoCanceler !fail");
				env->ExceptionClear(); //very important.
			}
			env->DeleteGlobalRef(aecClass);
		}else{
			env->ExceptionClear(); //very important.
		}
	}
	#if  V2_PHONE_SUPPORT
	return -1;
	#else
	d=lookup_by_model(manufacturer,model);
	if (!d){
		ms_warning("Lookup by model (%s/%s) failed.",manufacturer,model);
		d=lookup_by_platform(platform);
		if (!d){
			ms_warning("Lookup by platform (%s) also failed.",platform);
			return -1;
		}
	}
	ms_error("Found echo cancellation information for %s/%s/%s: builtin=%s, delay=%i ms",
				manufacturer,model,platform,d->has_builtin_ec ? "yes" : "no", d->delay);
	params->has_builtin_ec=d->has_builtin_ec;
	params->delay=d->delay;
	params->sample_rate=d->sample_rate;
	return 0;
	#endif
}
#endif
