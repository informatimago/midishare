/*
 *  Copyright (c) 1999 by Uros Bizjak <uros@kss-loka.si>
 *                        Takashi Iwai <iwai@ww.uni-erlangen.de>
 *
 *  SB16ASP/AWE32 CSP control
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __SB16_CSP_H
#define __SB16_CSP_H

/* CSP modes */
#define SND_SB_CSP_MODE_NONE            0x00
#define SND_SB_CSP_MODE_DSP_READ        0x01	/* Record from DSP */
#define SND_SB_CSP_MODE_DSP_WRITE       0x02	/* Play to DSP */
#define SND_SB_CSP_MODE_QSOUND          0x04	/* QSound */

/* CSP sample width */
#define SND_SB_CSP_SAMPLE_8BIT          0x01
#define SND_SB_CSP_SAMPLE_16BIT         0x02

/* CSP channels */
#define SND_SB_CSP_MONO                 0x01
#define SND_SB_CSP_STEREO               0x02

/* maximum size of micro code */
#define SND_SB_CSP_MAX_MICROCODE_FILE_SIZE      0x3000

/* microcode header */
typedef struct snd_sb_csp_mc_header {
	char codec_name[16];		/* id name of codec */
	unsigned short func_req;	/* requested function */
} snd_sb_csp_mc_header_t;

/* microcode to be loaded */
typedef struct snd_sb_csp_microcode {
	snd_sb_csp_mc_header_t info;
	unsigned char data[SND_SB_CSP_MAX_MICROCODE_FILE_SIZE];
} snd_sb_csp_microcode_t;

/* start CSP with pcm_format in mono/stereo */
typedef struct snd_sb_csp_start {
	int pcm_format;		/* pcm format from asound.h */
	int channels;		/* channels, look above */
} snd_sb_csp_start_t;

/* running state */
#define SND_SB_CSP_ST_IDLE      0x00
#define SND_SB_CSP_ST_LOADED    0x01
#define SND_SB_CSP_ST_RUNNING   0x02
#define SND_SB_CSP_ST_PAUSED    0x04
#define SND_SB_CSP_ST_QSOUND    0x10

/* CSP information */
typedef struct snd_sb_csp_info {
	char codec_name[16];	/* id name of codec */
	unsigned short func_nr;	/* function number */
	unsigned int acc_format;	/* accepted PCM formats */
	unsigned short acc_channels;	/* accepted channels */
	unsigned short csp_mode;	/* CSP mode, see above */
	unsigned int run_format;	/* current CSP format active */
	unsigned short run_channels;	/* current channels  */
	unsigned short version;	/* version id: 0x10 - 0x1f */
	unsigned short state;	/* state bits */
} snd_sb_csp_info_t;

/* QSound positions */
typedef struct snd_sb_csp_qsound {
	/* 33 Qsound positions
	 * from left (0x00) to right (0x20)
	 * in 180 degree field
	 */
	unsigned char left;	/* left position */
	unsigned char right;	/* right position */
} snd_sb_csp_qsound_t;

/* HWDEP controls */
/* get CSP information */
#define SND_SB_CSP_IOCTL_INFO           _IOR('H', 0x10, snd_sb_csp_info_t)
/* load microcode to CSP */
#define SND_SB_CSP_IOCTL_LOAD_CODE      _IOW('H', 0x11, snd_sb_csp_microcode_t)
/* start CSP */
#define SND_SB_CSP_IOCTL_START          _IOW('H', 0x12, snd_sb_csp_start_t)
/* stop CSP */
#define SND_SB_CSP_IOCTL_STOP           _IO('H', 0x13)
/* pause CSP and DMA transfer */
#define SND_SB_CSP_IOCTL_PAUSE          _IO('H', 0x14)
/* restart CSP and DMA transfer */
#define SND_SB_CSP_IOCTL_RESTART        _IO('H', 0x15)
/* start QSound codec */
#define SND_SB_CSP_IOCTL_QSOUND_START   _IO('H', 0x20)
/* stop QSound codec */
#define SND_SB_CSP_IOCTL_QSOUND_STOP    _IO('H', 0x21)
/* set QSound channel positions */
#define SND_SB_CSP_IOCTL_QSOUND_POS     _IOW('H', 0x22, snd_sb_csp_qsound_t)
/* get current QSound channel positions */
#define SND_SB_CSP_IOCTL_QSOUND_GET_POS _IOR('H', 0x23, snd_sb_csp_qsound_t)

#ifdef __KERNEL__
#include "sb.h"
#include "hwdep.h"
#define SND_HWDEP_TYPE_SB16CSP  0x10	/* temporarily defined here */
snd_hwdep_t *snd_sb_csp_new_device(sbdsp_t * codec);
#endif

#endif